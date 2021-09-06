// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CSkyBoxFix.h>
#include <state/CGameState.h>
#include <CGlobal.h>
#include <lua.hpp>
#include <string>

namespace dustbin {
  namespace state {
    CGameState::CGameState() : 
      m_pGlobal(CGlobal::getInstance()), 
      m_pSgmr(CGlobal::getInstance()->getSceneManager()), 
      m_pDrv(CGlobal::getInstance()->getVideoDriver()), 
      m_pGui(CGlobal::getInstance()->getGuiEnvironment()),
      m_pFs(CGlobal::getInstance()->getFileSystem())
    {
      m_cScreen = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());
    }

    CGameState::~CGameState() {
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      std::string l_sSettings[] = {
        "function getSettings()\n  return " + m_pGlobal->getGlobal("championship") + "\nend\n"
      };

      // First step: load the game settings and the settings
      // for this race from the global variables. The values
      // are stored as LUA tables so we need a LUA state for
      // decoding this data
      lua_State* l_pState = luaL_newstate();
      luaL_openlibs(l_pState);
      luaL_dostring(l_pState, l_sSettings[0].c_str());
      lua_getglobal(l_pState, "getSettings");
      if (!lua_isnil(l_pState, -1)) {
        if (lua_pcall(l_pState, 0, 1, 0) == 0) {
          m_cChampionship.loadFromStack(l_pState);
        }
        else {
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "Error while loading settings.");
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while starting game state.");
          throw std::exception();
        }
      }
      else {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "Error while loading settings.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while starting game state.");
        throw std::exception();
      }

      lua_close(l_pState);

      // Load the track, and don't forget to run the skybox fix beforehands
      std::string l_sTrack = "data/levels/" + m_cChampionship.m_thisrace.m_track + "/track.xml";

      if (m_pFs->existFile(l_sTrack.c_str())) {
        scenenodes::CSkyBoxFix* l_pFix = new scenenodes::CSkyBoxFix(m_pDrv, m_pSgmr, m_pFs, l_sTrack.c_str());
        m_pSgmr->loadScene(l_sTrack.c_str());
        l_pFix->hideOriginalSkybox(m_pSgmr->getRootSceneNode());
        delete l_pFix;

        m_pSgmr->addCameraSceneNode(nullptr, irr::core::vector3df(250, 250.0f, 250.0f));
      }
      else {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "Track \"" + m_cChampionship.m_thisrace.m_track + "\" not found.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while starting game state.");
        throw std::exception();
      }

      irr::scene::ISceneNode* l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId, m_pSgmr->getRootSceneNode());

      if (l_pNode == nullptr) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "No Starting Grid Scene Node found.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while starting game state.");
        throw std::exception();
      }

      scenenodes::CStartingGridSceneNode* l_pGrid = reinterpret_cast<scenenodes::CStartingGridSceneNode*>(l_pNode);

      // Fill the player vector
      for (size_t i = 0; i < m_cChampionship.m_thisrace.m_grid.size(); i++) {
        int l_iIndex = m_cChampionship.m_thisrace.m_grid[i] - 1;  // LUA index starts at 1, C++ index starts at 0

        gameclasses::SMarbleNodes* l_pMarble = l_pGrid->getNextMarble();
        m_vPlayers.push_back(new gameclasses::SPlayer(m_cChampionship.m_players[l_iIndex].m_playerid, m_cChampionship.m_players[l_iIndex].m_name, m_cChampionship.m_players[l_iIndex].m_texture, l_pMarble));
      }

      l_pGrid->removeUnusedMarbles();

      irr::f32 l_fAngle = l_pGrid->getAngle();

      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
      l_vOffset.rotateXZBy(l_fAngle);

      // Now we fill the viewport vector
      for (std::vector<SViewPort>::iterator it = m_cChampionship.m_viewports.begin(); it != m_cChampionship.m_viewports.end(); it++) {
        gameclasses::SMarbleNodes* l_pMarble = nullptr;

        for (std::vector<gameclasses::SPlayer*>::iterator it2 = m_vPlayers.begin(); it2 != m_vPlayers.end(); it2++) {
          if ((*it2)->m_iPlayer == (*it).m_playerid) {
            l_pMarble = (*it2)->m_pMarble;
            break;
          }
        }

        if (l_pMarble != nullptr) {
          l_pMarble->m_pPositional->updateAbsolutePosition();

          irr::core::vector2di l_cUpperLeft = irr::core::vector2di((*it).m_rect.m_upperleftcorner.m_x, (*it).m_rect.m_upperleftcorner.m_y),
                               l_cLowerRight = irr::core::vector2di((*it).m_rect.m_lowerrightcorner.m_x, (*it).m_rect.m_lowerrightcorner.m_y);

          irr::scene::ICameraSceneNode* l_pCam = m_pSgmr->addCameraSceneNode(m_pSgmr->getRootSceneNode(), l_pMarble->m_pPositional->getAbsolutePosition() + l_vOffset, l_pMarble->m_pPositional->getAbsolutePosition());
          l_pCam->setAspectRatio((((irr::f32)l_cLowerRight.X) - ((irr::f32)l_cUpperLeft.X)) / (((irr::f32)l_cLowerRight.Y) - ((irr::f32)l_cUpperLeft.Y)));
          l_pCam->updateAbsolutePosition();

          m_mViewports[(*it).m_playerid] = gfx::SViewPort(irr::core::recti(l_cUpperLeft, l_cLowerRight), (*it).m_playerid, l_pMarble->m_pPositional, l_pCam);
        }
      }
    }

    /**
    * This method is called when the state is deactivated
    */
    void CGameState::deactivate() {
      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++)
        delete* it;

      m_vPlayers  .clear();
      m_mViewports.clear();

      m_pSgmr->clear();
      m_pGui->clear();
    }

    /**
    * Find a single scene node by it's type
    * @param a_eType the type to search for
    * @param a_pParent the parent node to search
    */
    irr::scene::ISceneNode* CGameState::findSceneNodeByType(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      if (a_pParent->getType() == a_eType)
        return a_pParent;

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++) {
        irr::scene::ISceneNode *p = findSceneNodeByType(a_eType, *it);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
     * This is a callback method that gets invoked when the window is resized
     * @param a_cDim the new dimension of the window
     */
    void CGameState::onResize(const irr::core::dimension2du& a_cDim) {
    }

    /**
    * Change the Z-Layer for the Menu Controller
    * @param a_iZLayer the new Z-Layer
    */
    void CGameState::setZLayer(int a_iZLayer) {
    }

    /**
    * This method is called before the UI is cleared on window resize. It can be
    * used to save all necessary data to re-build the UI
    */
    void CGameState::beforeResize() {
    }

    /**
     * Return the state's ID
     */
    enState CGameState::getId() {
      return enState::GameState;
    }

    /**
     * Get the state of the mouse buttons. As the cursor control Irrlicht Object does not
     * report the state of the button I decided to hack it this way
     * @param a_iButton The mouse button
     */
    bool CGameState::isMouseDown(enMouseButton a_iButton) {
      return false;
    }

    /**
     * Event handling method. The main class passes all Irrlicht events to this method
     */
    bool CGameState::OnEvent(const irr::SEvent& a_cEvent) {
      return false;
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CGameState::run() {
      m_pDrv->beginScene();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        m_pDrv->setViewPort(it->second.m_cRect);

        if (it->second.m_pCamera != nullptr)
          m_pSgmr->setActiveCamera(it->second.m_pCamera);

        m_pSgmr->drawAll();
        m_pDrv->setViewPort(m_cScreen);
      }
      m_pDrv->endScene();

      return enState::None;
    }
  }
}