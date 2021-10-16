// (w) 2021 by Dustbin::Games / Christian Keimel

#ifndef NO_XEFFECT
#include <shader/CShaderHandleXEffectSplitscreen.h>
#include <shader/CShaderHandlerXEffect.h>
#endif

#include <scenenodes/CStartingGridSceneNode.h>
#include <controller/CControllerFactory.h>
#include <scenenodes/CMyCameraAnimator.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CCheckpointNode.h>
#include <shader/CShaderHandlerBase.h>
#include <scenenodes/CSkyBoxFix.h>
#include <scenenodes/CWorldNode.h>
#include <sound/CSoundInterface.h>
#include <shader/CMyShaderNone.h>
#include <sound/CSoundEnums.h>
#include <state/CGameState.h>
#include <lua/CLuaHelpers.h>
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
      m_pFs(CGlobal::getInstance()->getFileSystem()),
      m_eState(enGameState::Countdown),
      m_pOutputQueue(nullptr),
      m_pInputQueue(nullptr),
      m_pDynamics(nullptr),
      m_pShader(nullptr),
      m_bPaused(false),
      m_iFinished(-1),
      m_iStep(0)
    {
      m_cScreen = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());

      m_pCheckpointTextures[0] = m_pDrv->getTexture("data/textures/checkpoint_white.png");
      m_pCheckpointTextures[1] = m_pDrv->getTexture("data/textures/checkpoint_flash1.png");
      m_pCheckpointTextures[2] = m_pDrv->getTexture("data/textures/checkpoint_flash2.png");
    }

    CGameState::~CGameState() {
    }

    void CGameState::fillMovingMap(irr::scene::ISceneNode* a_pNode) {
      if (a_pNode != nullptr && a_pNode->isVisible()) {
        if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_PhysicsNodeId) {
          if (!reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode)->isStatic())
            m_mMoving[a_pNode->getID()] = a_pNode->getParent();
        }

        for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
          fillMovingMap(*it);
      }
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      m_iStep     = 0;
      m_iFadeOut  = -1;
      m_iFinished = -1;
      m_eState    = enGameState::Countdown;
      m_bPaused   = false;

      m_pGlobal->getIrrlichtDevice()->setResizable(false);
      m_pGlobal->getIrrlichtDevice()->getCursorControl()->setVisible(false);

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

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
          m_pChampionship = new SChampionShip();
          m_pChampionship->loadFromStack(l_pState);
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
      std::string l_sTrack = "data/levels/" + m_pChampionship->m_thisrace.m_track + "/track.xml";

      if (m_pFs->existFile(l_sTrack.c_str())) {
        scenenodes::CSkyBoxFix* l_pFix = new scenenodes::CSkyBoxFix(m_pDrv, m_pSgmr, m_pFs, l_sTrack.c_str());
        m_pSgmr->loadScene(l_sTrack.c_str());
        l_pFix->hideOriginalSkybox(m_pSgmr->getRootSceneNode());
        delete l_pFix;

        m_pSgmr->addCameraSceneNode(nullptr, irr::core::vector3df(250, 250.0f, 250.0f));
      }
      else {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "Track \"" + m_pChampionship->m_thisrace.m_track + "\" not found.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while starting game state.");
        throw std::exception();
      }

      fillCheckpointList(m_pSgmr->getRootSceneNode());

      if (m_mCheckpoints.size() == 0) {
        m_pGlobal->setGlobal("ERROR_MESSAGE", "Track \"" + m_pChampionship->m_thisrace.m_track + "\" does not contain checkpoints.");
        m_pGlobal->setGlobal("ERROR_HEAD", "Invalid track");
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
      for (size_t i = 0; i < m_pChampionship->m_thisrace.m_grid.size(); i++) {
        int l_iIndex = m_pChampionship->m_thisrace.m_grid[i] - 1;  // LUA index starts at 1, C++ index starts at 0

        gameclasses::SMarbleNodes* l_pMarble = l_pGrid->getNextMarble();
        gameclasses::SPlayer* p = new gameclasses::SPlayer(m_pChampionship->m_players[l_iIndex].m_playerid,
          m_pChampionship->m_players[l_iIndex].m_name,
          m_pChampionship->m_players[l_iIndex].m_texture,
          m_pChampionship->m_players[l_iIndex].m_controls,
          l_pMarble);

        m_vPlayers.push_back(p);

        m_aMarbles[i] = p->m_pMarble;
      }

      l_pGrid->removeUnusedMarbles();
      fillMovingMap(findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSgmr->getRootSceneNode()));

      irr::f32 l_fAngle = l_pGrid->getAngle();

      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
      l_vOffset.rotateXZBy(l_fAngle);

      // Now we fill the viewport vector
      for (std::vector<SViewPort>::iterator it = m_pChampionship->m_viewports.begin(); it != m_pChampionship->m_viewports.end(); it++) {
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
          l_pMarble->m_pViewport = &m_mViewports[(*it).m_playerid];
          m_mViewports[(*it).m_playerid].m_pPlayer = l_pMarble;

          // Initialize the viewport with the lap start checkpoints
          for (std::map<irr::s32, scenenodes::CCheckpointNode*>::iterator it = m_mCheckpoints.begin(); it != m_mCheckpoints.end(); it++) {
            if (it->second->m_bFirstInLap && it->second->getParent() != nullptr && it->second->getParent()->getType() == irr::scene::ESNT_MESH) {
              l_pMarble->m_pViewport->m_vNextCheckpoints.push_back(reinterpret_cast<irr::scene::IMeshSceneNode*>(it->second->getParent()));
            }
          }
        }
        else {
          irr::core::vector3df l_cPos = irr::core::vector3df(0.0f, 100.0f, 0.0f);

          irr::core::vector2di l_cUpperLeft = irr::core::vector2di((*it).m_rect.m_upperleftcorner.m_x, (*it).m_rect.m_upperleftcorner.m_y),
                               l_cLowerRight = irr::core::vector2di((*it).m_rect.m_lowerrightcorner.m_x, (*it).m_rect.m_lowerrightcorner.m_y);

          irr::scene::ICameraSceneNode* l_pCam = m_pSgmr->addCameraSceneNode(nullptr, l_cPos, irr::core::vector3df(0.0f, 0.0f, 0.0f));

          scenenodes::CMyCameraAnimator* l_pAnimator = new scenenodes::CMyCameraAnimator(m_pGlobal->getIrrlichtDevice());
          l_pCam->addAnimator(l_pAnimator);
          l_pAnimator->drop();

          l_pCam->setAspectRatio((((irr::f32)l_cLowerRight.X) - ((irr::f32)l_cUpperLeft.X)) / (((irr::f32)l_cLowerRight.Y) - ((irr::f32)l_cUpperLeft.Y)));
          l_pCam->updateAbsolutePosition();

          m_mViewports[(*it).m_playerid] = gfx::SViewPort(irr::core::recti(l_cUpperLeft, l_cLowerRight), (*it).m_playerid, nullptr, l_pCam);
        }
      }

      SSettings l_cSettings = m_pGlobal->getSettings();

      irr::u32 l_iAmbient = 196;

      switch (l_cSettings.m_gfx_ambientlight) {
        case 0: l_iAmbient = 32; break;
        case 1: l_iAmbient = 64; break;
        case 2: l_iAmbient = 96; break;
        case 3: l_iAmbient = 128; break;
        case 4: l_iAmbient = 160; break;
      }

      irr::core::dimension2du l_cDim = irr::core::dimension2du(m_mViewports.begin()->second.m_cRect.getSize().Width, m_mViewports.begin()->second.m_cRect.getSize().Height);

      switch (l_cSettings.m_gfx_shadows) {
        case 3:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, l_cSettings.m_gfx_ambientlight);
          }
          break;
#endif
          
        case 2:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, l_cSettings.m_gfx_ambientlight);
          }
          break;
#endif

        case 1:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, l_cSettings.m_gfx_ambientlight);
          }
          break;
#endif

        case 0:
          m_pShader = new shader::CShaderHandlerNone(m_pGlobal->getIrrlichtDevice(), l_cDim);
          break;
      }

      if (m_pShader != nullptr) {
        m_pShader->initialize();
      }

      l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSgmr->getRootSceneNode());

      if (l_pNode != nullptr) {
        m_pDynamics = new gameclasses::CDynamicThread(reinterpret_cast<scenenodes::CWorldNode*>(l_pNode), m_vPlayers, m_pChampionship->m_thisrace.m_laps, lua::loadLuaScript("data/levels/" + m_pChampionship->m_thisrace.m_track + "/dynamics.lua"));
        m_pInputQueue = new threads::CInputQueue();
        m_pDynamics->getOutputQueue()->addListener(m_pInputQueue);
        m_pOutputQueue = new threads::COutputQueue();
        m_pOutputQueue->addListener(m_pDynamics->getInputQueue());

        controller::CControllerFactory* l_pFactory = new controller::CControllerFactory(m_pDynamics->getInputQueue());

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
          (*it)->m_pController = l_pFactory->createController((*it)->m_pMarble->m_pPositional->getID(), (*it)->m_sController);
        }

        delete l_pFactory;

        m_pDynamics->startThread();
      }
      else {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "No world node found.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
        throw std::exception();
      }
    }

    /**
    * This method is called when the state is deactivated
    */
    void CGameState::deactivate() {
      m_pGlobal->getIrrlichtDevice()->setResizable(true);
      m_pGlobal->getIrrlichtDevice()->getCursorControl()->setVisible(true);

      if (m_pDynamics != nullptr) {
        m_pDynamics->stopThread();
        m_pDynamics->join();
        delete m_pDynamics;
        m_pDynamics = nullptr;
      }

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++)
        delete* it;

      m_vPlayers  .clear();
      m_mViewports.clear();

      m_pSgmr->clear();
      m_pGui->clear();

      m_pDrv->beginScene();
      m_pSgmr->drawAll();
      m_pDrv->endScene();

      if (m_pInputQueue != nullptr) {
        delete m_pInputQueue;
        m_pInputQueue = nullptr;
      }

      if (m_pOutputQueue != nullptr) {
        delete m_pOutputQueue;
        m_pOutputQueue = nullptr;
      }

      for (std::vector<messages::IMessage*>::iterator it = m_vMoveMessages.begin(); it != m_vMoveMessages.end(); it++) {
        delete* it;
      }
      m_vMoveMessages.clear();

      if (m_pShader != nullptr) {
        delete m_pShader;
        m_pShader = nullptr;
      }

      m_mCheckpoints.clear();

      if (m_pChampionship != nullptr)
        delete m_pChampionship;
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
    * Fill the "Checkpoint" list
    * @param a_pParent the parent node to search
    */
    void CGameState::fillCheckpointList(irr::scene::ISceneNode* a_pParent) {
      if (a_pParent->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_CheckpointNodeId) {
        scenenodes::CCheckpointNode* p = reinterpret_cast<scenenodes::CCheckpointNode*>(a_pParent);
        m_mCheckpoints[p->getID()] = p;
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++) {
        fillCheckpointList(*it);
      }
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
      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->update(a_cEvent);
      }

      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (!a_cEvent.KeyInput.PressedDown) {
          if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) {
            sendCancelrace(m_pOutputQueue);
          }
          else if (a_cEvent.KeyInput.Key == irr::KEY_PAUSE) {
            sendTogglepause(m_pOutputQueue);
          }
        }
      }

      return false;
    }

    /**
    * This method resets the scene after a viewport was rendered
    * @param a_pViewport the viewport that was rendered
    */
    void CGameState::afterDrawScene(gfx::SViewPort* a_pViewPort) {
      // Player "0" always is the player in "view track" so we don't need
      // to adjust the textures in this case
      if (a_pViewPort != nullptr && a_pViewPort->m_iPlayer != 0) {
        // Set the material "0" of the next checkpoints to green, flash materials "2" and "3"
        for (std::vector<irr::scene::IMeshSceneNode*>::iterator it = a_pViewPort->m_vNextCheckpoints.begin(); it != a_pViewPort->m_vNextCheckpoints.end(); it++) {
          irr::scene::IMeshSceneNode* p = *it;

          if (p->getMaterialCount() > 0) {
            p->getMaterial(0).setTexture(0, m_pCheckpointTextures[0]);
          }
        }
      }
    }

    /**
    * This method prepares the scene before a viewport is rendered
    * @param a_pViewport the viewport that will be rendered
    */
    void CGameState::beforeDrawScene(gfx::SViewPort* a_pViewPort) {
      // Player "0" always is the player in "view track" so we don't need
      // to adjust the textures in this case
      if (a_pViewPort != nullptr && a_pViewPort->m_iPlayer != 0) {
        // Make marbles behind the marble node transparent
        irr::core::vector3df l_cNormal   = (a_pViewPort->m_pCamera->getTarget() - a_pViewPort->m_pCamera->getAbsolutePosition()).normalize(),
                             l_cPosition = a_pViewPort->m_pCamera->getTarget() + 0.1f * (a_pViewPort->m_pCamera->getAbsolutePosition() - a_pViewPort->m_pCamera->getTarget());
        irr::core::plane3df  l_cPlane    = irr::core::plane3df(l_cPosition, l_cNormal);

        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr) {
            // ToDo: handle respawning marble
            if (m_aMarbles[i]->m_pPositional != a_pViewPort->m_pMarble && m_aMarbles[i]->m_pPositional != nullptr && m_aMarbles[i]->m_pRotational->getMesh()->getMeshBufferCount() > 0) {
              if (l_cPlane.classifyPointRelation(m_aMarbles[i]->m_pPositional->getAbsolutePosition()) == irr::core::ISREL3D_BACK) {
                irr::scene::IMeshBuffer* l_pBuffer = m_aMarbles[i]->m_pRotational->getMesh()->getMeshBuffer(0);
                m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;

                irr::video::S3DVertex* l_pVertices = (irr::video::S3DVertex*)l_pBuffer->getVertices();

                for (irr::u32 j = 0; j < l_pBuffer->getVertexCount(); j++)
                  l_pVertices[j].Color.setAlpha(96);
              }
              else m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = m_pShader == nullptr ? irr::video::EMT_SOLID : m_pShader->getMaterialType();
            }
            else m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = m_pShader == nullptr ? irr::video::EMT_SOLID : m_pShader->getMaterialType();
          }
        }

        // Set the material "0" of the next checkpoints to green, flash materials "2" and "3"
        for (std::vector<irr::scene::IMeshSceneNode*>::iterator it = a_pViewPort->m_vNextCheckpoints.begin(); it != a_pViewPort->m_vNextCheckpoints.end(); it++) {
          irr::scene::IMeshSceneNode* p = *it;

          if (p->getMaterialCount() > 0) {
            int l_iIndex = ((m_iStep % 240) < 120) ? 1 : 2;
            p->getMaterial(0).setTexture(0, m_pCheckpointTextures[l_iIndex]);
          }
        }
      }
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CGameState::run() {
      enState l_eRet = enState::None;

      messages::IMessage* l_pMsg = nullptr;

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->postControlMessage();
      }

      do {
        l_pMsg = m_pInputQueue->popMessage();
        if (l_pMsg != nullptr) {
          if (l_pMsg->getMessageId() == messages::enMessageIDs::ObjectMoved || l_pMsg->getMessageId() == messages::enMessageIDs::MarbleMoved) {
            m_vMoveMessages.push_back(l_pMsg);
          }
          else {
            if (l_pMsg->getMessageId() == messages::enMessageIDs::StepMsg) {
              for (std::vector<messages::IMessage*>::iterator it = m_vMoveMessages.begin(); it != m_vMoveMessages.end(); it++) {
                handleMessage(*it, true);
              }
              m_vMoveMessages.clear();
            }

            handleMessage(l_pMsg, true);
          }
        }
      } 
      while (l_pMsg != nullptr);

      m_pDrv->beginScene(true, true, irr::video::SColor(255, 0, 0, 0));

      if (m_pShader != nullptr)
        m_pShader->beginScene();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (m_pShader != nullptr) {
          if (it->second.m_pCamera != nullptr)
            m_pSgmr->setActiveCamera(it->second.m_pCamera);

          beforeDrawScene(&it->second);
          m_pShader->renderScene(it->second.m_cRect);
          afterDrawScene(&it->second);

          if (it->second.m_pPlayer != nullptr && it->second.m_pPlayer->m_iStateChange != -1) {
            switch (it->second.m_pPlayer->m_eState) {
              // Fade the viewport out and in if the player is respawning
              case gameclasses::SMarbleNodes::enMarbleState::Respawn1:
              case gameclasses::SMarbleNodes::enMarbleState::Respawn2: {
                int l_iStepSince = m_iStep - it->second.m_pPlayer->m_iStateChange;
                irr::f32 l_fFactor = 1.0f;

                if (l_iStepSince < 120) {
                  l_fFactor = ((irr::f32)l_iStepSince) / 120.0f;
                  if (l_fFactor > 1.0f)
                    l_fFactor = 1.0f;
                }

                if (it->second.m_pPlayer->m_eState == gameclasses::SMarbleNodes::enMarbleState::Respawn2)
                  l_fFactor = 1.0f - l_fFactor;

                m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFactor), 0, 0, 0), it->second.m_cRect);

                break;
              }

              // Blue overlay for stunned players
              case gameclasses::SMarbleNodes::enMarbleState::Stunned:
                m_pDrv->draw2DRectangle(irr::video::SColor(128, 0, 0, 255), it->second.m_cRect);
                break;
            }
          }
        }
      }
      m_pGui->drawAll();

      if (m_eState == enGameState::Countdown) {
        irr::f32 l_fFade = 0.0f;

        if (m_iStep < 120)
          l_fFade = 1.0f;
        else
          l_fFade = 1.0f - (((irr::f32)m_iStep - 120) / 180.0f);

        if (l_fFade > 0.0f)
          m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFade), 0, 0, 0), m_cScreen);

        m_pGlobal->getSoundInterface()->setSoundtrackFade(l_fFade);
      }
      else if (m_eState == enGameState::Racing) {
        if (m_iFinished != -1 && m_iStep - m_iFinished > 600) { /**< toDo: waiting time adjustable */
          m_eState = enGameState::Finished;
          m_iFadeOut = m_iStep;
        }

        if (m_bPaused) {
          m_pDrv->draw2DRectangle(irr::video::SColor(128, 255, 255, 255), m_cScreen);
        }
      }
      else if (m_eState == enGameState::Finished) {
        int l_iStepSince = m_iStep - m_iFadeOut;

        irr::f32 l_fFade = ((irr::f32)l_iStepSince) / 120.0f;

        if (l_fFade > 1.0f) {
          l_fFade = 1.0f;
          l_eRet = enState::LuaState;
        }

        m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFade), 0, 0, 0), m_cScreen);
        m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f - l_fFade);
      }

      m_pDrv->endScene();
      m_pDrv->setRenderTarget(0, false, false);

      return l_eRet;
    }

    /**** Methods inherited from "messages:IGameState ****/

    /**
      * This function receives messages of type "StepMsg"
      * @param a_StepNo The current step number
      */
    void CGameState::onStepmsg(irr::u32 a_StepNo) {
      m_iStep = a_StepNo;
    }

    /**
     * This function receives messages of type "Countdown"
     * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
     */
    void CGameState::onCountdown(irr::u8 a_Tick) {
      printf("On Countdown: %i\n", a_Tick);
      if (a_Tick == 0) {
        m_eState = enGameState::Racing;
        m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStRace);
        m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f);

        m_pGlobal->getSoundInterface()->play2d(L"data/sounds/countdown_go.ogg", 1.0f, 0.0f);
      }
      else if (a_Tick != 4) m_pGlobal->getSoundInterface()->play2d(L"data/sounds/countdown.ogg", 1.0f, 0.0f);
    }

    /**
     * This function receives messages of type "ObjectMoved"
     * @param a_ObjectId The ID of the object
     * @param a_Position The current position
     * @param a_Rotation The current rotation (Euler angles)
     * @param a_LinearVelocity The linear velocity
     * @param a_AngularVelocity The angualar (rotation) velocity
     */
    void CGameState::onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity) {
      if (m_mMoving.find(a_ObjectId) != m_mMoving.end()) {
        m_mMoving[a_ObjectId]->setPosition(a_Position);
        m_mMoving[a_ObjectId]->setRotation(a_Rotation);
      }
      else printf("Object %i not found.\n", a_ObjectId);
    }

    /**
     * This function receives messages of type "MarbleMoved"
     * @param a_ObjectId The ID of the object
     * @param a_Position The current position
     * @param a_Rotation The current rotation (Euler angles)
     * @param a_LinearVelocity The linear velocity
     * @param a_AngularVelocity The angualar (rotation) velocity
     * @param a_CameraPosition The position of the camera
     * @param a_CameraUp The Up-Vector of the camera
     * @param a_ControlX The marble's current controller state in X-Direction
     * @param a_ControlY The marble's current controller state in Y-Direction
     * @param a_Contact A Flag indicating whether or not the marble is in contact with another object
     * @param a_ControlBrake Flag indicating whether or not the marble's brake is active
     * @param a_ControlRearView Flag indicating whether or not the marble's player looks behind
     * @param a_ControlRespawn Flag indicating whether or not the manual respawn button is pressed
     */
    void CGameState::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      if (a_ObjectId >= 10000 && a_ObjectId < 10016) {
        irr::s32 l_iIndex = a_ObjectId - 10000;

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          p->m_pPositional->setPosition(a_Position);
          p->m_pPositional->setRotation(a_Rotation);
          p->m_pPositional->updateAbsolutePosition();
          
          if (p->m_pViewport != nullptr && p->m_bCamLink) {
            p->m_pViewport->m_pCamera->setPosition(a_CameraPosition);
            p->m_pViewport->m_pCamera->setTarget  (a_Position + 1.5f * a_CameraUp);
            p->m_pViewport->m_pCamera->setUpVector(a_CameraUp);
            p->m_pViewport->m_pCamera->updateAbsolutePosition();
          }
        }
      }
    }

    /**
     * This function receives messages of type "Trigger"
     * @param a_TriggerId ID of the trigger
     * @param a_ObjectId ID of the marble that caused the trigger
     */
    void CGameState::onTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId) {

    }

    /**
     * This function receives messages of type "PlayerRespawn"
     * @param a_MarbleId ID of the marble
     * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
     */
    void CGameState::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (a_State == 1) {
          p->m_bCamLink = false;
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Respawn1;
          p->m_iStateChange = m_iStep;
        }
        else {
          p->m_bCamLink = true;
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
          p->m_iStateChange = -1;
        }
      }
    }

    /**
     * This function receives messages of type "CameraRespawn"
     * @param a_MarbleId The ID of the marble which is respawning
     * @param a_Position The new position of the camera
     * @param a_Target The new target of the camera, i.e. the future position of the marble
     */
    void CGameState::onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target) {
      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          if (p->m_pViewport != nullptr && p->m_pViewport->m_pCamera != nullptr) {
            p->m_pViewport->m_pCamera->setPosition(a_Position);
            p->m_pViewport->m_pCamera->setTarget(a_Target);
            p->m_pViewport->m_pCamera->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));
          }

          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Respawn2;
          p->m_iStateChange = m_iStep;
        }
      }
    }

    /**
     * This function receives messages of type "PlayerStunned"
     * @param a_MarbleId ID of the marble
     * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
     */
    void CGameState::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      printf("onPlayerStunned: %i, %i\n", a_MarbleId, a_State);
      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;
        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          if (a_State == 1) {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Stunned;
            p->m_iStateChange = m_iStep;
          }
          else {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
            p->m_iStateChange = -1;
          }
        }
      }
    }

    /**
     * This function receives messages of type "PlayerFinished"
     * @param a_MarbleId ID of the finished marble
     * @param a_RaceTime Racetime of the finished player in simulation steps
     * @param a_Laps The number of laps the player has done
     */
    void CGameState::onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {
      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;
        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Finished;
          p->m_bCamLink = false;
        }
      }
    }

    /**
     * This function receives messages of type "RaceFinished"
     * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
     */
    void CGameState::onRacefinished(irr::u8 a_Cancelled) {
      m_iFinished = m_iStep;
      m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStFinish);
    }

    /**
     * This function receives messages of type "Checkpoint"
     * @param a_MarbleId ID of the marble
     * @param a_Checkpoint The checkpoint ID the player has passed
     */
    void CGameState::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) {
      int l_iId = a_MarbleId - 10000;

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr) {
        printf("onCheckpoint: Marble %i, Checkpoint %i\n", a_MarbleId, a_Checkpoint);
        gfx::SViewPort* l_pViewport = m_aMarbles[l_iId]->m_pViewport;
        if (l_pViewport != nullptr) {
          afterDrawScene(l_pViewport);
          l_pViewport->m_vNextCheckpoints.clear();

          for (std::vector<int>::iterator it = m_mCheckpoints[a_Checkpoint]->m_vLinks.begin(); it != m_mCheckpoints[a_Checkpoint]->m_vLinks.end(); it++) {
            if (m_mCheckpoints.find(*it) != m_mCheckpoints.end()) {
              scenenodes::CCheckpointNode* p = m_mCheckpoints[*it];
              if (p->getParent() != nullptr && p->getParent()->getType() == irr::scene::ESNT_MESH) {
                l_pViewport->m_vNextCheckpoints.push_back(reinterpret_cast<irr::scene::IMeshSceneNode*>(p->getParent()));
              }
            }
          }

          l_pViewport->m_iLastCp = a_Checkpoint;
        }
      }
    }

    /**
     * This function receives messages of type "LapStart"
     * @param a_MarbleId ID of the marble
     * @param a_LapNo Number of the started lap
     */
    void CGameState::onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) {
      printf("onLapstart: Marble %i, Lap %i\n", a_MarbleId, a_LapNo);
    }

    /**
     * This function receives messages of type "PauseChanged"
     * @param a_Paused The current paused state
     */
    void CGameState::onPausechanged(bool a_Paused) {
      m_bPaused = a_Paused;
    }
  }
}