// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CStartingGridSceneNode.h>
#include <controller/CControllerFactory.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CSkyBoxFix.h>
#include <scenenodes/CWorldNode.h>
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
      m_pFs(CGlobal::getInstance()->getFileSystem()),
      m_pOutputQueue(nullptr),
      m_pInputQueue(nullptr),
      m_pDynamics(nullptr),
      m_iStep(0)
    {
      m_cScreen = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());
    }

    CGameState::~CGameState() {
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      m_iStep = 0;

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
        gameclasses::SPlayer* p = new gameclasses::SPlayer(m_cChampionship.m_players[l_iIndex].m_playerid, 
          m_cChampionship.m_players[l_iIndex].m_name, 
          m_cChampionship.m_players[l_iIndex].m_texture, 
          m_cChampionship.m_players[l_iIndex].m_controls,
          l_pMarble);

        m_vPlayers.push_back(p);

        m_aMarbles[i] = p->m_pMarble;
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
          l_pMarble->m_pViewport = &m_mViewports[(*it).m_playerid];
        }
      }

      l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSgmr->getRootSceneNode());

      if (l_pNode != nullptr) {
        m_pDynamics = new gameclasses::CDynamicThread(reinterpret_cast<scenenodes::CWorldNode*>(l_pNode), m_vPlayers);
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
      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->update(a_cEvent);
      }
      return false;
    }

    /**
    * This method prepares the scene before a viewport is rendered
    * @param a_pViewport the viewport that will be rendered
    */
    void CGameState::beforeDrawScene(gfx::SViewPort* a_pViewPort) {
      if (a_pViewPort != nullptr) {
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
              else m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = irr::video::EMT_SOLID; // ToDo: Shader handler material
            }
            else m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = irr::video::EMT_SOLID; // ToDo: Shader handler material
          }
        }
      }
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CGameState::run() {
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

      m_pDrv->beginScene();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        m_pDrv->setViewPort(it->second.m_cRect);

        if (it->second.m_pCamera != nullptr)
          m_pSgmr->setActiveCamera(it->second.m_pCamera);

        beforeDrawScene(&it->second);
        m_pSgmr->drawAll();
        m_pDrv->setViewPort(m_cScreen);
      }
      m_pDrv->endScene();

      return enState::None;
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
          
          if (p->m_pViewport != nullptr) {
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
     * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Camera, 3 == Respawn Done)
     */
    void CGameState::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {

    }

    /**
     * This function receives messages of type "PlayerStunned"
     * @param a_MarbleId ID of the marble
     * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
     */
    void CGameState::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {

    }

    /**
     * This function receives messages of type "PlayerFinished"
     * @param a_MarbleId ID of the finished marble
     * @param a_RaceTime Racetime of the finished player in simulation steps
     * @param a_Laps The number of laps the player has done
     */
    void CGameState::onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {

    }

    /**
     * This function receives messages of type "RaceFinished"
     * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
     */
    void CGameState::onRacefinished(irr::u8 a_Cancelled) {

    }
  }
}