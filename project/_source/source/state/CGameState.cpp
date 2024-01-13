// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>
#include <_generated/lua/CLuaScript_scene.h>
#include <controller/ICustomEventReceiver.h>
#include <_generated/messages/CMessages.h>
#include <controller/CControllerFactory.h>
#include <controller/CAiControlThread.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinCamera.h>
#include <gui/CInGamePanelRenderer.h>
#include <controller/CControllerAI.h>
#include <helpers/CTextureHelpers.h>
#include <shaders/CDustbinShaders.h>
#include <scenenodes/CRostrumNode.h>
#include <helpers/CStringHelpers.h>
#include <scenenodes/CSkyBoxFix.h>
#include <scenenodes/CWorldNode.h>
#include <sound/ISoundInterface.h>
#include <scenenodes/CSpeedNode.h>
#include <network/CGameClient.h>
#include <network/CGameServer.h>
#include <scenenodes/CAiNode.h>
#include <sound/CSoundEnums.h>
#include <data/CDataStructs.h>
#include <state/CGameState.h>
#include <state/CMenuState.h>
#include <gui/CTutorialHUD.h>
#include <gui/CGameHUD.h>
#include <CGlobal.h>
#include <algorithm>
#include <Defines.h>
#include <random>
#include <string>
#include <map>

#ifdef _TOUCH_CONTROL
#include <scenenodes/CCameraTouchAnimator.h>
#else
#include <scenenodes/CMyCameraAnimator.h>
#endif


namespace dustbin {
  namespace state {
    CGameState::CGameState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) :
      IState           (a_pDevice, a_pGlobal),
      m_eState         (enGameState::Countdown),
      m_pInputQueue    (nullptr),
      m_pOutputQueue   (nullptr),
      m_iStep          (-1),
      m_iFinished      (-1),
      m_iFadeOut       (-1),
      m_fSfxVolume     (1.0f),
      m_iRenderFlags   (0),
      m_bEnded         (false),
      m_pShader        (nullptr),
      m_pSoundIntf     (nullptr),
      m_pDynamics      (nullptr),
      m_pRostrum       (nullptr),
      m_pRaceData      (nullptr),
      m_pCamAnimator   (nullptr),
      m_pCamera        (nullptr),
      m_pAiThread      (nullptr),
      m_pClient        (nullptr),
      m_pServer        (nullptr),
      m_iNumOfViewports(0),
      m_pGridNode      (nullptr),
      m_fGridAngle     (0.0f),
      m_pAiNode        (nullptr),
      m_pLuaScript     (nullptr),
      m_pPanelRndr     (nullptr)
#ifdef _TOUCH_CONTROL
      ,m_pTouchControl(nullptr)
#endif
    {
      m_cScreen = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());

      m_pCheckpointTextures[0] = m_pDrv->getTexture("data/textures/checkpoint_white.png");
      m_pCheckpointTextures[1] = m_pDrv->getTexture("data/textures/checkpoint_flash1.png");
      m_pCheckpointTextures[2] = m_pDrv->getTexture("data/textures/checkpoint_flash2.png");

      m_pSoundIntf = a_pGlobal->getSoundInterface();

      animators::IAnimatorTextureProvider::m_pInstance = this;
    }

    CGameState::~CGameState() {
      animators::IAnimatorTextureProvider::m_pInstance = nullptr;

      if (m_pLuaScript != nullptr) {
        delete m_pLuaScript;
        m_pLuaScript = nullptr;
      }

      if (m_pInputQueue != nullptr) {
        delete m_pInputQueue;
        m_pInputQueue = nullptr;
      }

      if (m_pOutputQueue != nullptr) {
        delete m_pOutputQueue;
        m_pOutputQueue = nullptr;
      }
    }

    void CGameState::fillMovingMap(irr::scene::ISceneNode* a_pNode) {
      if (a_pNode != nullptr && a_pNode->isVisible()) {
        if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_PhysicsNodeId) {
          if (!reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode)->isStatic())
            m_mMoving[a_pNode->getID()] = a_pNode->getParent();
        }

        if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_SpeedNodeId) {
          scenenodes::CSpeedNode *l_pNode = reinterpret_cast<scenenodes::CSpeedNode *>(a_pNode);
          l_pNode->fillTextureMaps();
          l_pNode->setMarbleSpeed(-1, 0);
          m_vSpeed.push_back(l_pNode);
        }

        a_pNode->setDebugDataVisible(0);

        for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
          fillMovingMap(*it);
      }

      // Fill list of viewport distribution
      SGameViewports::SViewportDistribution l_cOnePlayer = SGameViewports::SViewportDistribution(1, 1);
      l_cOnePlayer.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
      m_cViewports.m_mDistribution[1] = l_cOnePlayer;

      SGameViewports::SViewportDistribution l_cTwoPlayers = SGameViewports::SViewportDistribution(1, 2);
      l_cTwoPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
      l_cTwoPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
      m_cViewports.m_mDistribution[2] = l_cTwoPlayers;

      SGameViewports::SViewportDistribution l_cThreePlayers = SGameViewports::SViewportDistribution(2, 2);
      l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
      m_cViewports.m_mDistribution[3] = l_cThreePlayers;

      SGameViewports::SViewportDistribution l_cFourPlayers = SGameViewports::SViewportDistribution(2, 2);
      l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
      l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
      l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
      l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player));
      m_cViewports.m_mDistribution[4] = l_cFourPlayers;

      SGameViewports::SViewportDistribution l_cFivePlayers = SGameViewports::SViewportDistribution(2, 3);
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Racedata));
      m_cViewports.m_mDistribution[5] = l_cFivePlayers;

      SGameViewports::SViewportDistribution l_cSixPlayers = SGameViewports::SViewportDistribution(2, 3);
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Player));
      m_cViewports.m_mDistribution[6] = l_cSixPlayers;

      SGameViewports::SViewportDistribution l_cSevenPlayers = SGameViewports::SViewportDistribution(3, 3);
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 2, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Logo    ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
      m_cViewports.m_mDistribution[7] = l_cSevenPlayers;

      SGameViewports::SViewportDistribution l_cEightPlayers = SGameViewports::SViewportDistribution(3, 3);
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 2, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
      m_cViewports.m_mDistribution[8] = l_cEightPlayers;
    }

    /**
    * Assign a viewport to a player
    * @param a_fAngle the grid angle read from the grid scene node
    * @param a_pPlayer the player to assign the viewport to
    */
    void CGameState::assignViewport(irr::f32 a_fAngle, gameclasses::SPlayer *a_pPlayer) {
      irr::core::dimension2di l_cViewportSize = m_cScreen.getSize();

      if (m_iNumOfViewports > 0) {
        l_cViewportSize.Width  /= m_cViewports.m_mDistribution[m_iNumOfViewports].m_iColumns;
        l_cViewportSize.Height /= m_cViewports.m_mDistribution[m_iNumOfViewports].m_iRows;
#ifndef _TOUCH_CONTROL
        if (m_pDevice->getCursorControl() != nullptr)
          m_pDevice->getCursorControl()->setVisible(false);
#endif
      }

      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
      l_vOffset.rotateXZBy(a_fAngle);

      gameclasses::STournament *l_pTournament = m_pGlobal->getTournament();

      if (a_pPlayer->m_iViewport != -1) {
        if (a_pPlayer != nullptr && a_pPlayer->m_pMarble->m_pPositional) {
          SGameViewports::SViewportDef l_cViewportDef = m_cViewports.m_mDistribution[m_iNumOfViewports].m_vViewports[a_pPlayer->m_iViewport - 1];

          printf("Viewport %i / %i assigned to player \"%s\" (%i).\n", a_pPlayer->m_iViewport, m_iNumOfViewports, a_pPlayer->m_sName.c_str(), a_pPlayer->m_pMarble->m_pPositional->getID());

          irr::core::recti l_cRect = irr::core::recti(
              l_cViewportDef.m_iColumn      * l_cViewportSize.Width,
              l_cViewportDef.m_iRow         * l_cViewportSize.Height,
            (l_cViewportDef.m_iColumn + 1) * l_cViewportSize.Width,
            (l_cViewportDef.m_iRow    + 1) * l_cViewportSize.Height
          );


          irr::scene::ICameraSceneNode* l_pCam = m_pSmgr->addCameraSceneNode(m_pSmgr->getRootSceneNode(), a_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition() + l_vOffset, a_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition());
          l_pCam->setAspectRatio((((irr::f32)l_cRect.LowerRightCorner.X) - ((irr::f32)l_cRect.UpperLeftCorner.X)) / (((irr::f32)l_cRect.LowerRightCorner.Y) - ((irr::f32)l_cRect.UpperLeftCorner.Y)));
          l_pCam->updateAbsolutePosition();

          gfx::SViewPort l_cViewport = gfx::SViewPort(l_cRect, a_pPlayer->m_iPlayer, a_pPlayer->m_pMarble->m_pPositional, l_pCam);

          for (std::map<irr::s32, scenenodes::CCheckpointNode*>::iterator it = m_mCheckpoints.begin(); it != m_mCheckpoints.end(); it++) {
            if (it->second->m_bFirstInLap && it->second->getParent()->getType() == irr::scene::ESNT_MESH) {
              l_cViewport.m_vNextCheckpoints.push_back(reinterpret_cast<irr::scene::IMeshSceneNode *>(it->second->getParent()));
            }
          }

          l_cViewport.m_pPlayer = a_pPlayer->m_pMarble;
          m_mViewports[a_pPlayer->m_iPlayer] = l_cViewport;
          a_pPlayer->m_pMarble->m_pViewport = &m_mViewports[a_pPlayer->m_iPlayer];
        }

        m_pSoundIntf->setViewportMarble(a_pPlayer->m_pMarble->m_pPositional->getID());
      }
    }

    /**
    * Hide the AI node unless defined as visible in the settings
    */
    void CGameState::hideAiNode() {
      m_pAiNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_AiNodeId, m_pSmgr->getRootSceneNode());

      if (m_pAiNode != nullptr) {
        m_pAiNode->setVisible(getGlobal()->getSetting("show_ai_data") == "1");
      }
    }

    /**
    * Prepare the shader for the game
    */
    void CGameState::prepareShader() {
      if (m_cViewports.m_mDistribution.find(m_iNumOfViewports) == m_cViewports.m_mDistribution.end()) {
        // ToDo: Error message
      }

      irr::u32 l_iAmbient = 196;

      switch (m_cSettings.m_iAmbient) {
        case 0: l_iAmbient = 32; break;
        case 1: l_iAmbient = 64; break;
        case 2: l_iAmbient = 96; break;
        case 3: l_iAmbient = 128; break;
        case 4: l_iAmbient = 160; break;
      }

      irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();

      int l_iCols = (int)fmax(1, m_cViewports.m_mDistribution[m_iNumOfViewports].m_iColumns);
      int l_iRows = (int)fmax(1, m_cViewports.m_mDistribution[m_iNumOfViewports].m_iRows   );

      irr::core::dimension2du l_cDim = irr::core::dimension2du(l_cScreen.Width / l_iCols, l_cScreen.Height / l_iRows);
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      helpers::addToDebugLog("CGameState::activate {");

      m_iStep           = 0;
      m_iFadeOut        = -1;
      m_iFinished       = -1;
      m_eState          = enGameState::Countdown;
      m_bEnded          = false;
      m_fSfxVolume      = m_pGlobal->getSettingData().m_fSfxGame;
      m_iNumOfViewports = 0;

      m_pDrv->beginScene(true, true);
      m_pGlobal->drawNextRaceScreen(1.0f);
      m_pDrv->endScene();

      helpers::addToDebugLog("Load sounds...");
      m_pSoundIntf->preloadSound(L"data/sounds/hit.ogg"          , false);
      m_pSoundIntf->preloadSound(L"data/sounds/respawn_start.ogg", false);
      m_pSoundIntf->preloadSound(L"data/sounds/respawn.ogg"      , false);
      m_pSoundIntf->preloadSound(L"data/sounds/stunned.ogg"      , false);
      m_pSoundIntf->preloadSound(L"data/sounds/rolling.ogg"      , false);
      m_pSoundIntf->preloadSound(L"data/sounds/skid.ogg"         , false);
      m_pSoundIntf->preloadSound(L"data/sounds/wind.ogg"         , false);

      helpers::addToDebugLog("Initialize GUI...");
      m_pGlobal->clearGui();

      // m_pStepLabel = m_pGui->addStaticText(L"Step", irr::core::recti(0, 0, 1000, 200));

      helpers::addToDebugLog("Check for network game...");
      m_pClient = m_pGlobal->getGameClient();
      m_pServer = m_pGlobal->getGameServer();

      helpers::addToDebugLog("Load settings...");

      m_cSettings = m_pGlobal->getSettingData();

      gameclasses::STournament *l_pTournament = m_pGlobal->getTournament();
      m_pRaceData = l_pTournament->getRace();

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      helpers::addToDebugLog("Load track...");
      // Load the track, and don't forget to run the skybox fix beforehands
      std::string l_sTrack = "data/levels/" + m_pRaceData->m_sTrack + "/track.xml";

      if (m_pFs->existFile(l_sTrack.c_str())) {
        m_pSmgr->clear();
        scenenodes::CSkyBoxFix* l_pFix = new scenenodes::CSkyBoxFix(m_pDrv, m_pSmgr, m_pFs, l_sTrack.c_str());
        m_pSmgr->loadScene(l_sTrack.c_str());
        l_pFix->hideOriginalSkybox(m_pSmgr->getRootSceneNode());
        delete l_pFix;
        addStaticCameras(m_pSmgr->getRootSceneNode());

        m_pShader = m_pGlobal->getShader();
        m_pShader->addLightCamera();

        m_iRenderFlags = helpers::convertForShader(m_cSettings.m_iShadows, m_pShader);
        printf("Render Flags: %i\n", m_iRenderFlags);

        helpers::addNodeToShader(m_pShader, m_pSmgr->getRootSceneNode());

        m_pShader->clearShadowMaps();

        if (m_cSettings.m_iShadows > 0) {
          irr::u32 l_iFlags = (m_cSettings.m_iShadows == 1) ? 
            (irr::u32)shaders::enShadowMap::Solid 
            : 
            (irr::u32)shaders::enShadowMap::Transparent | (irr::u32)shaders::enShadowMap::TranspColor | (irr::u32)shaders::enShadowMap::Solid;

          m_pShader->startShadowMaps();
          m_pShader->renderShadowMap(l_iFlags);
          m_pShader->endShadowMaps();
        }
      }
      else {
        handleError("Error while starting game state.", "The specified race track file was not found.");
        return;
      }

#ifdef _OPENGL_ES
      helpers::adjustNodeMaterials(m_pSmgr->getRootSceneNode());
#endif

      helpers::addToDebugLog("Fill Checkpoint List...");
      fillCheckpointList(m_pSmgr->getRootSceneNode());

      if (m_mCheckpoints.size() == 0) {
        handleError("Error while starting game state.", "No Checkpoints found.");
        return;
      }

      helpers::addToDebugLog("Find starting grid...");
      irr::scene::ISceneNode* l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId, m_pSmgr->getRootSceneNode());

      if (l_pNode == nullptr) {
        handleError("Error while starting game state.", "No Starting Grid Scene Node found.");
        return;
      }

      m_pGridNode = reinterpret_cast<scenenodes::CStartingGridSceneNode *>(l_pNode);

      if (m_pGridNode == nullptr) {
        handleError("Error while starting game state.", "Starting Grid Scene Node has invalid Type.");
        return;
      }

      helpers::addToDebugLog("Setup grid...");
      m_fGridAngle = m_pGridNode->getAngle();
      m_pGridNode->setShader(m_pShader);

      data::SGameSettings::enAutoFinish l_eAutoFinish = l_pTournament->m_eAutoFinish;
      bool l_bGridReverse = l_pTournament->m_bReverse;

      // Find out how many viewports we need to create
      for (auto l_pPlr: l_pTournament->m_vPlayers) {
        if (l_pPlr->m_iViewport != -1)
          m_iNumOfViewports++;
      }

      helpers::addToDebugLog(std::to_string(m_iNumOfViewports) + " Viewport necessary.");

      helpers::addToDebugLog("Fill object maps...");
      fillMovingMap(findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode()));

      if (m_pInputQueue  == nullptr) m_pInputQueue  = new threads::CInputQueue ();
      if (m_pOutputQueue == nullptr) m_pOutputQueue = new threads::COutputQueue();

      hideAiNode();

      helpers::addToDebugLog("Start physics thread...");
      helpers::addToDebugLog("  Find world node");
      l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode());
      
      if (l_pNode != nullptr) {
        helpers::addToDebugLog("  World node found");

        helpers::addToDebugLog("  Create dynamics thread");
        m_pDynamics = new gameclasses::CDynamicThread(m_pClient != nullptr);

        helpers::addToDebugLog("  Add queues and listeners");
        m_pDynamics->getOutputQueue()->addListener(m_pInputQueue);
        m_pOutputQueue->addListener(m_pDynamics->getInputQueue());

        if (m_pServer != nullptr) {
          m_pDynamics->getOutputQueue()->addListener(m_pServer  ->getInputQueue());
          m_pServer  ->getOutputQueue()->addListener(m_pDynamics->getInputQueue());
        }

        if (m_pClient != nullptr) {
          m_pDynamics->getOutputQueue()->addListener(m_pClient  ->getInputQueue());
          m_pClient  ->getOutputQueue()->addListener(m_pDynamics->getInputQueue());
        }

        helpers::addToDebugLog("  Load physics script");
        std::string l_sPhysicsScript = helpers::loadTextFile("data/levels/" + m_pRaceData->m_sTrack + "/physics.lua");

        helpers::addToDebugLog("  Setup game");
        if (!m_pDynamics->setupGame(reinterpret_cast<scenenodes::CWorldNode*>(l_pNode), m_pGridNode, l_pTournament->m_vPlayers, m_pRaceData->m_iLaps, l_sPhysicsScript, l_eAutoFinish)) {
          handleError("LUA Error.", m_pDynamics->getLuaError());
          return;
        }

        int l_iIndex = 0;

        for (auto l_pPlr: m_pRaceData->m_vRanking) {
          irr::scene::ISceneNode *l_pMarble = assignMarbleToPlayer(l_pPlr->m_iPlayer, 10000 + l_iIndex);
          l_pPlr->m_iGridPos = l_iIndex + 1;
          m_pDynamics->assignPlayerToMarble(l_pPlr->m_pPlayer, l_pMarble);

          l_iIndex++;
        }
      }
      else {
        handleError("Error while starting game state.", "No world node found..");
        return;
      }
      
      if (m_pClient != nullptr) {
        m_pClient->stateChanged("state_game");
      }

      helpers::addToDebugLog("Initialize sounds...");

#ifndef _ANDROID
      for (int l_iMarble = 10000; l_iMarble < 10016; l_iMarble++) {
        // Tuple: 0 == Sound File, 1 == Looped, 2 == Doppler
        std::tuple<std::wstring, bool, bool> l_sSounds[] = {
          { L"data/sounds/hit.ogg"          , false , false },
          { L"data/sounds/respawn.ogg"      , false , false },
          { L"data/sounds/respawn_start.ogg", false , false },
          { L"data/sounds/rolling.ogg"      , true  , true  },
          { L"data/sounds/skid.ogg"         , true  , true  },
          { L"data/sounds/stunned.ogg"      , true  , false },
          { L"data/sounds/wind.ogg"         , true  , true  },
          { L"data/sounds/gameover.ogg"     , false , false },
          { L""                             , false , false }
          };

        for (int i = 0; std::get<0>(l_sSounds[i]) != L""; i++) {
          helpers::addToDebugLog("Preload sound #" + std::to_string(i) + " (" + helpers::ws2s(std::get<0>(l_sSounds[i])));
          m_pSoundIntf->preloadSound(std::get<0>(l_sSounds[i]), false);
        }
      }
#endif

      raceSetupDone();
      helpers::addToDebugLog("CGameState::activate }\n");
    }

    /**
    * This method is called when the state is deactivated
    */
    void CGameState::deactivate() {
      helpers::addToDebugLog("CGameState::deactivate() {");
      m_mMoving.clear();

      if (m_pClient != nullptr) m_pClient->resetMyMarbles();
      if (m_pServer != nullptr) m_pServer->resetMyMarbles();

      helpers::addToDebugLog("Show cursor");
      if (m_pDevice->getCursorControl() != nullptr)
        m_pDevice->getCursorControl()->setVisible(true);

      if (m_pLuaScript != nullptr) {
        helpers::addToDebugLog("delete LUA script");
        delete m_pLuaScript;
        m_pLuaScript = nullptr;
      }

      // Important: stop the AI thread
      // before the dynamics thread
      if (m_pAiThread != nullptr) {
        helpers::addToDebugLog("Stop AI thread");
        m_pAiThread->stopThread();
        helpers::addToDebugLog("Wait for AI thread to stop");
        m_pAiThread->join();
        helpers::addToDebugLog("Delete AI thread");
        delete m_pAiThread;
        m_pAiThread = nullptr;
      }

      if (m_pDynamics != nullptr) {
        helpers::addToDebugLog("Stop dynamics thread");
        m_pDynamics->stopThread();
        m_pDynamics->join();

        helpers::addToDebugLog("Clear input queue");
        messages::IMessage *l_pMsg = nullptr;

        do {
          if (m_pInputQueue != nullptr) {
            l_pMsg = m_pInputQueue->popMessage();
            if (l_pMsg != nullptr && !m_bWillBeDeleted) {
              helpers::addToDebugLog("Delete message" + std::to_string((int)l_pMsg->getMessageId()));
              handleMessage(l_pMsg);
            }
          }
        }
        while (l_pMsg != nullptr);

        helpers::addToDebugLog("Queue cleared");

        if (m_pServer != nullptr) {
          helpers::addToDebugLog("Remove server as listener from dynamics thread");
          m_pServer->getOutputQueue()->removeListener(m_pDynamics->getInputQueue());
        }

        if (m_pClient != nullptr) {
          helpers::addToDebugLog("Remove client as listener from dynamics thread");
          m_pClient->getOutputQueue()->removeListener(m_pDynamics->getInputQueue());
        }

        helpers::addToDebugLog("Delete dynamics thread");
        delete m_pDynamics;
        m_pDynamics = nullptr;

        m_pClient = nullptr;
        m_pServer = nullptr;
      }

      m_pGlobal->getTournament()->finishCurrentRace();

      m_pGlobal->getTournament()->saveToJSON();

      helpers::addToDebugLog("Clear vectors");
      m_mViewports.clear();

#ifdef _TOUCH_CONTROL
      m_pTouchControl = nullptr;
#endif

      if (m_pCamAnimator != nullptr) {
        helpers::addToDebugLog("Drop camera animator");
        m_pCamAnimator->drop();
        m_pCamAnimator = nullptr;
      }

      m_pShader->clear();
      helpers::addToDebugLog("Clear scene manager");
      m_pSmgr->clear();
      helpers::addToDebugLog("Clear GUI");
      m_pGlobal->clearGui();

      helpers::addToDebugLog("Delete buffered move messages");
      for (std::vector<messages::IMessage*>::iterator it = m_vMoveMessages.begin(); it != m_vMoveMessages.end(); it++) {
        delete* it;
      }
      m_vMoveMessages.clear();

      helpers::addToDebugLog("Clear checkpoint and camera vectors");
      m_mCheckpoints.clear();
      m_vCameras    .clear();
      m_vSpeed      .clear();

      helpers::addToDebugLog("Stop game sounds");
      m_pSoundIntf->stopGame();

      if (m_pPanelRndr != nullptr) {
        delete m_pPanelRndr;
        m_pPanelRndr = nullptr;
      }

      m_pShader = nullptr;
      helpers::addToDebugLog("CGameState::deactivate() }");
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
     * Return the state's ID
     */
    enState CGameState::getId() {
      return enState::Game;
    }

    /**
     * Event handling method. The main class passes all Irrlicht events to this method
     */
    bool CGameState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->update(a_cEvent);
      }

      if (!l_bRet && a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (!a_cEvent.KeyInput.PressedDown) {
          if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) {
            sendCancelrace(m_pOutputQueue);
            l_bRet = true;
          }
        }
        else if (a_cEvent.KeyInput.Key == irr::KEY_PLUS || a_cEvent.KeyInput.Key == irr::KEY_MINUS) {
          if (m_cSettings.m_iShadows < 5 && a_cEvent.KeyInput.Key == irr::KEY_PLUS) {
            m_cSettings.m_iShadows++;
          }
          else if (m_cSettings.m_iShadows > 0 && a_cEvent.KeyInput.Key == irr::KEY_MINUS) {
            m_cSettings.m_iShadows--;
          }

          printf("Shadow Mode: %i\n", m_cSettings.m_iShadows);

          m_iRenderFlags = helpers::convertForShader(m_cSettings.m_iShadows, m_pShader);

          m_pShader->clearShadowMaps();

          if (m_cSettings.m_iShadows > 0) {
            irr::u32 l_iFlags = (m_cSettings.m_iShadows == 1) ? 
              (irr::u32)shaders::enShadowMap::Solid 
            : 
              (irr::u32)shaders::enShadowMap::Transparent | (irr::u32)shaders::enShadowMap::TranspColor | (irr::u32)shaders::enShadowMap::Solid;

            m_pShader->startShadowMaps();
            m_pShader->renderShadowMap(l_iFlags);
            m_pShader->endShadowMaps();
          }
        }
        else {
          // Not available during gameplay
          if (m_pCamAnimator != nullptr) {
            int l_iIndex = -1;

            switch (a_cEvent.KeyInput.Key) {
              case irr::KEY_KEY_1: l_iIndex = 0; break;
              case irr::KEY_KEY_2: l_iIndex = 1; break;
              case irr::KEY_KEY_3: l_iIndex = 2; break;
              case irr::KEY_KEY_4: l_iIndex = 3; break;
              case irr::KEY_KEY_5: l_iIndex = 4; break;
              case irr::KEY_KEY_6: l_iIndex = 5; break;
              case irr::KEY_KEY_7: l_iIndex = 6; break;
              case irr::KEY_KEY_8: l_iIndex = 7; break;
              case irr::KEY_KEY_9: l_iIndex = 8; break;
              case irr::KEY_KEY_0: l_iIndex = 9; break;

              default: break;
            }

            if (l_iIndex >= 0 && l_iIndex < m_vCameras.size()) {
              if (m_pCamAnimator != nullptr && m_pCamera != nullptr) {
                irr::core::vector3df l_vPos;
                irr::f32 l_fAngleH, l_fAngleV;

                m_vCameras[l_iIndex]->getValues(l_vPos, l_fAngleV, l_fAngleH);
                m_pCamAnimator->setData(l_vPos, l_fAngleV, l_fAngleH);
              }
            }
          }
        }
      }

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (a_cEvent.KeyInput.Key == irr::KEY_BACK) {
          if (a_cEvent.KeyInput.PressedDown && m_mViewports.size() > 0) {
            if (m_mViewports.begin()->second.m_pMarble != nullptr) {
              if (m_mViewports.begin()->second.m_pHUD != nullptr) {
                if (m_mViewports.begin()->second.m_pHUD->onWithdrawButton())
                  return true;
              }

              int l_iMarble = m_mViewports.begin()->second.m_pMarble->getID();
              if (l_iMarble >= 10000 && l_iMarble < 10016) {
                messages::CPlayerWithdraw l_cMessage = messages::CPlayerWithdraw(l_iMarble);
                m_pOutputQueue->postMessage(&l_cMessage);
              }
            }
          }
          l_bRet = true;
        }
      }
#endif

      return l_bRet;
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

        if (a_pViewPort->m_pHUD != nullptr) {
          a_pViewPort->m_pHUD->afterDrawScene();
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
        if (a_pViewPort->m_pPlayer->m_eState != gameclasses::SMarbleNodes::enMarbleState::Finished) {
          // Make marbles behind the marble node transparent
          irr::core::vector3df l_cNormal   = (a_pViewPort->m_pCamera->getTarget() - a_pViewPort->m_pCamera->getAbsolutePosition()).normalize();
          irr::core::vector3df l_cPosition = a_pViewPort->m_pCamera->getTarget() + 0.1f * (a_pViewPort->m_pCamera->getAbsolutePosition() - a_pViewPort->m_pCamera->getTarget());
          irr::core::plane3df  l_cPlane    = irr::core::plane3df(l_cPosition, l_cNormal);

          for (int i = 0; i < 16; i++) {
            if (m_aMarbles[i] != nullptr) {
              bool l_bRespawn = m_aMarbles[i]->m_eState == gameclasses::SMarbleNodes::enMarbleState::Respawn1 || m_aMarbles[i]->m_eState == gameclasses::SMarbleNodes::enMarbleState::Respawn2;
              bool l_bOther   = m_aMarbles[i]->m_pPositional != a_pViewPort->m_pMarble && m_aMarbles[i]->m_pPositional != nullptr && m_aMarbles[i]->m_pRotational->getMesh()->getMeshBufferCount() > 0;

              if (l_bOther || l_bRespawn) {
                bool l_bBehind = l_cPlane.classifyPointRelation(m_aMarbles[i]->m_pPositional->getAbsolutePosition()) == irr::core::ISREL3D_BACK;
                if (l_bBehind || l_bRespawn) {
                  irr::scene::IMeshBuffer* l_pBuffer = m_aMarbles[i]->m_pRotational->getMesh()->getMeshBuffer(0);

                  irr::video::S3DVertex* l_pVertices = (irr::video::S3DVertex*)l_pBuffer->getVertices();

                  // The "Behind Camera" Alpha Value
                  irr::u32 l_iAlpha = 192;

                  // If the marble is respawning ..
                  if (l_bRespawn) {
                    // .. we calculate a factor ..
                    irr::f32 l_fFactor = 1.0f - (irr::f32)(m_iStep - m_aMarbles[i]->m_iRespawnStart) / 120.0f;
                    if (l_fFactor < 0.1f)
                      l_fFactor = 0.1f;

                    l_iAlpha = (irr::u32)(255.0f * l_fFactor);

                    // .. and if the marble is behind the camera we make sure
                    // the marble is transparent
                    if (l_iAlpha > 192 && l_bBehind)
                      l_iAlpha = 192;
                  }

                  for (irr::u32 j = 0; j < l_pBuffer->getVertexCount(); j++)
                    l_pVertices[j].Color.setAlpha(l_iAlpha);

                  m_pShader->adjustNodeMaterial(m_aMarbles[i]->m_pRotational, 0, m_pShader->getMaterial(shaders::enMaterialType::Transparent));
                }
                else {
                  m_pShader->adjustNodeMaterial(m_aMarbles[i]->m_pRotational, 0, m_pShader->getMaterial(shaders::enMaterialType::Marble));
                }
              }
              else {
                m_pShader->adjustNodeMaterial(m_aMarbles[i]->m_pRotational, 0, m_pShader->getMaterial(shaders::enMaterialType::Marble));
              }
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
        else {
          for (int i = 0; i < 16; i++) {
            if (m_aMarbles[i] != nullptr) {
              m_pShader->adjustNodeMaterial(m_aMarbles[i]->m_pRotational, 0, m_pShader->getMaterial(shaders::enMaterialType::Marble));
            }
          }
        }

        if (a_pViewPort->m_pHUD != nullptr) {
          a_pViewPort->m_pHUD->beforeDrawScene();
        }

        for (std::vector<scenenodes::CSpeedNode*>::iterator l_itNode = m_vSpeed.begin(); l_itNode != m_vSpeed.end(); l_itNode++) {
          (*l_itNode)->setMarbleSpeed(a_pViewPort->m_pPlayer->m_eState != gameclasses::SMarbleNodes::enMarbleState::Finished ? std::sqrt(a_pViewPort->m_pPlayer->m_fVeclocity) : -1.0f, m_iStep);
        }
      }
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CGameState::run() {
      messages::IMessage* l_pMsg = nullptr;

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr) {
          bool l_bLeft      = false;
          bool l_bRight     = false;
          bool l_bForward   = false;
          bool l_bBackward  = false;
          bool l_bBrake     = false;
          bool l_bRespawn   = false;
          bool l_bAutomatic = false;

          (*it)->m_pController->postControlMessage(
            l_bLeft, 
            l_bRight, 
            l_bForward, 
            l_bBackward, 
            l_bBrake, 
            l_bRespawn, 
            l_bAutomatic
          );

          if ((*it)->m_pMarble->m_pViewport->m_pHUD != nullptr) {
            (*it)->m_pMarble->m_pViewport->m_pHUD->updateAiHelp(
              l_bLeft, 
              l_bRight, 
              l_bForward, 
              l_bBackward, 
              l_bBrake, 
              l_bRespawn, 
              l_bAutomatic
            );
          }
        }
      }

#ifdef _TOUCH_CONTROL
      if (m_pCamAnimator != nullptr && m_pCamera != nullptr)
        m_pCamAnimator->animateNode(m_pCamera, 0);
#endif

      do {
        if (m_pInputQueue != nullptr) {
          l_pMsg = m_pInputQueue->popMessage();
          if (l_pMsg != nullptr) {
            for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
              if (it->second.m_pHUD != nullptr) {
                it->second.m_pHUD->handleMessage(l_pMsg, false);
              }
            }

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
      }
      while (l_pMsg != nullptr);

      if (!m_pDevice->run())
        return enState::Quit;

      if (m_pPanelRndr != nullptr)
        m_pPanelRndr->updateTextureIfNecessary();

      if (m_iRenderFlags != 0) {
        m_pShader->startShadowMaps();
        m_pShader->renderShadowMap(m_iRenderFlags);
        m_pShader->endShadowMaps();
      }

      m_pDrv->beginScene(true, true, irr::video::SColor(255, 0, 0, 0));

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (it->second.m_pCamera != nullptr)
          m_pSmgr->setActiveCamera(it->second.m_pCamera);

        beforeDrawScene(&it->second);
        m_pDrv->setViewPort(it->second.m_cRect);
        m_pShader->renderScene();
        afterDrawScene(&it->second);
      }

      m_pDrv->setViewPort(m_cScreen);
      m_pGui->drawAll();

      // Fade-out of the HUD after the race must be drawn after the rest of the GUI so that the gray rectangle
      // showing the result does not overlay the fade-out
      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (it->second.m_pPlayer != nullptr && it->second.m_pPlayer->m_iStateChange != -1) {
          if (it->second.m_pPlayer->m_eState == gameclasses::SMarbleNodes::enMarbleState::Finished) {
            if (m_pRostrum != nullptr) {
              int l_iStepSince = m_iStep - it->second.m_pPlayer->m_iStateChange;
              
              if (l_iStepSince > 160) {
                if (!it->second.m_pHUD->isResultParentVisible())
                  it->second.m_pHUD->showResultParent();

                it->second.m_pCamera->setPosition(m_pRostrum->getCameraPosition());
                it->second.m_pCamera->setTarget(m_pRostrum->getAbsolutePosition() - irr::core::vector3df(0.0f, 15.0f, 0.0f));
                it->second.m_pCamera->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));
              }
            }
          }
        }
      }

      if (m_eState == enGameState::Countdown) {
        irr::f32 l_fFade = 0.0f;

        if (m_iStep < 120)
          l_fFade = 1.0f;
        else
          l_fFade = 1.0f - (((irr::f32)m_iStep - 120) / 180.0f);

        if (l_fFade < 0.1f && l_fFade > 0.0f)
          m_pSoundIntf->setMenuFlag(false);

        m_pGlobal->getSoundInterface()->setSoundtrackFade(l_fFade);
      }
      else if (m_eState == enGameState::Racing) {
        if (m_iFinished != -1 && m_iStep - m_iFinished > 1800) { // Wait until the finished soundtrack has ended
          m_eState = enGameState::Finished;
          m_iFadeOut = m_iStep;

          for (auto l_cViewport: m_mViewports) {
            if (l_cViewport.second.m_pHUD != nullptr) {
              l_cViewport.second.m_pHUD->startFinalFadeOut();
            }
          }
        }
      }
      else if (m_eState == enGameState::Finished) {
        int l_iStepSince = m_iStep - m_iFadeOut;

        irr::f32 l_fFade = ((irr::f32)l_iStepSince) / 120.0f;

        if (l_fFade > 1.0f) {
          l_fFade = 1.0f;

          if (m_pDynamics != nullptr)
            m_pDynamics->stopThread();
        }

        m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f - l_fFade);
      }

      m_pDrv->endScene();

      return (m_bEnded || (m_pDynamics != nullptr && m_pDynamics->hasFinished())) ? enState::Menu : enState::None;
    }

    /**** Methods inherited from "messages:IGameState ****/

    /**
    * This function receives messages of type "StepMsg"
    * @param a_StepNo The current step number
    */
    void CGameState::onStepmsg(irr::u32 a_StepNo) {
      m_iStep = a_StepNo;

      // if (m_pStepLabel != nullptr)
      //   m_pStepLabel->setText((L"         " + std::to_wstring(a_StepNo)).c_str());

      if (m_pLuaScript != nullptr)
        m_pLuaScript->onstep(a_StepNo);

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->onStep(a_StepNo);
      }
    }

    /**
     * This function receives messages of type "Countdown"
     * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
     */
    void CGameState::onCountdown(irr::u8 a_Tick) {
      if (a_Tick == 0) {
        m_eState = enGameState::Racing;
        m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStRace);
        m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f);

        m_pGlobal->getSoundInterface()->play2d(en2dSounds::Countdown, CGlobal::getInstance()->getSettingData().m_fSfxGame, 0.0f);
      }
      else if (a_Tick != 4) m_pGlobal->getSoundInterface()->play2d(en2dSounds::CountdownGo, CGlobal::getInstance()->getSettingData().m_fSfxGame, 0.0f);

      if (m_pPanelRndr != nullptr)
        m_pPanelRndr->setCountdownTick(a_Tick);
    }

    /**
     * This function receives messages of type "ObjectMoved"
     * @param a_ObjectId The ID of the object
     * @param a_Position The current position
     * @param a_Rotation The current rotation (Euler angles)
     * @param a_LinearVelocity The linear velocity
     * @param a_AngularVelocity The angualar (rotation) velocity
     */
    void CGameState::onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity) {
      if (m_mMoving.find(a_ObjectId) != m_mMoving.end()) {
        m_mMoving[a_ObjectId]->setPosition(a_Position);

        if (a_Rotation.X == a_Rotation.X && a_Rotation.Y == a_Rotation.Y && a_Rotation.Z == a_Rotation.Z)
          m_mMoving[a_ObjectId]->setRotation(a_Rotation);
      }
      else printf("Object %i not found.\n", a_ObjectId);
      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (it->second.m_pPlayer != nullptr && it->second.m_pPlayer->m_pPlayer != nullptr && it->second.m_pPlayer->m_pPlayer->m_pController != nullptr) {
          controller::IControllerAI *l_pCtrl = it->second.m_pPlayer->m_pPlayer->m_pController->getAiController();
          if (l_pCtrl != nullptr) {
            l_pCtrl->onObjectMoved(a_ObjectId, a_Position, a_Rotation, a_LinearVelocity, a_AngularVelocity.getLength());
          }
        }
      }
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
    void CGameState::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      irr::f32 l_fRolling = m_fSfxVolume * std::fmin((float)(a_LinearVelocity.getLengthSQ() / 10000.0), (float)1.0);

      if (a_ObjectId >= 10000 && a_ObjectId < 10016) {
        irr::s32 l_iIndex = a_ObjectId - 10000;

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
          if ((*it)->m_pController != nullptr)
            (*it)->m_pController->onMarbleMoved(a_ObjectId, a_Position, a_LinearVelocity, a_Position + a_CameraPosition + 3.0f * a_CameraUp, a_CameraUp, a_Contact);
        }

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          irr::f32 l_fVel = a_LinearVelocity.getLengthSQ();
          irr::f32 l_fDif = std::abs(l_fVel - p->m_fVeclocity);

          p->m_fVeclocity = l_fVel;
          p->m_pPositional->setPosition(a_Position);
          p->m_pPositional->updateAbsolutePosition();
          p->m_pRotational->updateAbsolutePosition();

          if (a_Rotation.X == a_Rotation.X && a_Rotation.Y == a_Rotation.Y && a_Rotation.Z == a_Rotation.Z)
            p->m_pPositional->setRotation(a_Rotation);

          if (p->m_pViewport != nullptr && p->m_bCamLink) {
            p->m_pViewport->m_pCamera->setPosition(a_ControlRearView ? a_Position - a_CameraPosition + 3.0f * a_CameraUp : a_Position + a_CameraPosition + 3.0f * a_CameraUp);
            p->m_pViewport->m_pCamera->setTarget  (a_Position + 1.5f * a_CameraUp);
            p->m_pViewport->m_pCamera->setUpVector(a_CameraUp);
            p->m_pViewport->m_pCamera->updateAbsolutePosition();

            m_pSoundIntf->playViewportMarbleSound(a_ObjectId, a_Position, a_LinearVelocity, l_fRolling, a_ControlBrake, a_Contact);
          }
          if (m_mViewports.size() == 1) {
            irr::f32 l_fHit = l_fDif < 1000.0f ? 0.0f : l_fDif > 2000.0f ? 1.0f : (l_fDif - 1000.0f) / 1000.0f;
            m_pSoundIntf->playMarbleSounds(a_ObjectId, a_Position, a_LinearVelocity, l_fHit, l_fRolling, a_ControlBrake, a_Contact);
          }
        }

        

        if (m_mViewports.size() == 1) {
          if (m_mViewports.begin()->second.m_pMarble != nullptr && a_ObjectId == m_mViewports.begin()->second.m_pMarble->getID())
            m_pSoundIntf->setListenerPosition(m_mViewports.begin()->second.m_pCamera, a_LinearVelocity);
          else
            m_pSoundIntf->setListenerPosition(m_pSmgr->getActiveCamera(), irr::core::vector3df());
        }
      }
    }

    /**
     * This function receives messages of type "Trigger"
     * @param a_TriggerId ID of the trigger
     * @param a_ObjectId ID of the marble that caused the trigger
     */
    void CGameState::onTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId) {
      if (m_pLuaScript != nullptr)
        m_pLuaScript->ontrigger(a_ObjectId, a_TriggerId);

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->onTrigger(a_TriggerId, a_ObjectId);

        if ((*it)->m_pMarble                      != nullptr &&
            (*it)->m_pMarble->m_pViewport         != nullptr &&
            (*it)->m_pMarble->m_pViewport->m_pHUD != nullptr) {
          (*it)->m_pMarble->m_pViewport->m_pHUD->triggerCallback(a_ObjectId, a_TriggerId);
        }
      }
    }

    /**
    * This function receives messages of type "PlayerAssignMarble"
    * @param a_iPlayerId The ID of the player
    * @param a_iMarbleId The ID of the marble for the player
    */
    irr::scene::ISceneNode *CGameState::assignMarbleToPlayer(irr::s32 a_iPlayerId, irr::s32 a_iMarbleId) {
      threads::CInputQueue  *l_pTheInputQueue  = nullptr;
      threads::COutputQueue *l_pTheOutputQueue = nullptr;

      if (m_pDynamics != nullptr) {
        l_pTheInputQueue  = m_pDynamics->getInputQueue ();
        l_pTheOutputQueue = m_pDynamics->getOutputQueue();
      }
      else if (m_pClient != nullptr) {
        l_pTheInputQueue  = m_pClient->getInputQueue ();
        l_pTheOutputQueue = m_pClient->getOutputQueue();
      }

      gameclasses::SMarbleNodes *l_pMarble = m_pGridNode->getMarble(a_iMarbleId);
      irr::scene::ISceneNode    *l_pRet    = nullptr;

      if (l_pTheInputQueue != nullptr && l_pTheOutputQueue != nullptr && l_pMarble != nullptr) {
        l_pRet    = l_pMarble->m_pPositional;

        controller::CControllerFactory *l_pFactory = new controller::CControllerFactory(l_pTheInputQueue);

        printf("Assign Marble %i to player %i\n", a_iMarbleId, a_iPlayerId);
        gameclasses::STournament *l_pTrnmnt = m_pGlobal->getTournament();

        for (auto l_pPlr : l_pTrnmnt->m_vPlayers) {
          if (l_pPlr->m_iPlayer == a_iPlayerId) {
            m_pShader->adjustNodeMaterial(l_pMarble->m_pRotational, 0, m_pShader->getMaterial(shaders::enMaterialType::Marble));

            if (l_pPlr->m_sTexture == "")
              l_pPlr->m_sTexture = "default://number=" + std::to_string(a_iPlayerId);

            if (l_pPlr->m_sTexture.substr(0, std::string("default://").size()) == "default://") {
              if (l_pPlr->m_eAiHelp == data::SPlayerData::enAiHelp::BotMb3 || l_pPlr->m_sController == "class=marble3") {
                l_pPlr->m_sTexture += "&class=2";
              }
              else if (l_pPlr->m_eAiHelp == data::SPlayerData::enAiHelp::BotMb2 || l_pPlr->m_sController == "class=marble2") {
                l_pPlr->m_sTexture += "&class=1";
              }
            }

            l_pPlr->setMarbleNode(l_pMarble);
            l_pMarble->m_pPlayer = l_pPlr; 
            m_aMarbles[l_pMarble->m_pPositional->getID() - 10000] = l_pMarble;

            if (l_pPlr->m_eType == data::enPlayerType::Local) {
              assignViewport(m_fGridAngle, l_pPlr);

              l_pPlr->m_pController = l_pFactory->createController(
                l_pPlr->m_pMarble->m_pPositional->getID(), 
                l_pPlr->m_sController, 
                l_pPlr->m_eAiHelp, 
                reinterpret_cast<scenenodes::CAiNode*>(m_pAiNode),
                "data/levels/" + m_pRaceData->m_sTrack,
                l_pPlr->m_pMarble->m_pViewport->m_cRect,
                l_pPlr->m_bShowRanking
              );

              if (m_pClient != nullptr)
                m_pClient->setMyMarble(l_pPlr->m_pMarble->m_pPositional->getID());

              if (m_pServer != nullptr)
                m_pServer->setMyMarble(l_pPlr->m_pMarble->m_pPositional->getID());
            }
            else if (l_pPlr->m_eType == data::enPlayerType::Ai) {
              if (m_pAiThread == nullptr) {
                m_pAiThread = new controller::CAiControlThread(l_pTheOutputQueue, l_pTheInputQueue);
              }

              m_pAiThread->addAiMarble(l_pPlr->m_pMarble->m_pPositional->getID(), l_pPlr->m_sController, "data/levels/" + m_pRaceData->m_sTrack);

              if (m_pClient != nullptr)
                m_pClient->setMyMarble(l_pPlr->m_pMarble->m_pPositional->getID());
                  
              if (m_pServer != nullptr)
                m_pServer->setMyMarble(l_pPlr->m_pMarble->m_pPositional->getID());
            }
            break;
          }
        }

        if (m_pPanelRndr != nullptr)
          m_pPanelRndr->updateRanking(m_pRaceData->m_vRanking);

        delete l_pFactory;
      }
      return l_pRet;
    }

    /**
    * This function is called after setup has finished
    */
    void CGameState::raceSetupDone() {
      m_pSoundIntf->startGame();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        // it->second.m_pPlayer->m_pPlayer->m_pController
        // if (m_cGameData.m_bIsTutorial)
        //   it->second.m_pHUD = new gui::CTutorialHUD(it->second.m_pPlayer->m_pPlayer, it->second.m_cRect, m_cGameData.m_iLaps, m_pGui, &m_pRaceData->m_vRanking, m_pOutputQueue);
        // else
          it->second.m_pHUD = new gui::CGameHUD(it->second.m_pPlayer->m_pPlayer, it->second.m_cRect, m_pRaceData->m_iLaps, m_pGui, &m_pRaceData->m_vRanking);

        controller::IControllerAI *l_pCtrl = it->second.m_pPlayer->m_pPlayer->m_pController->getAiController();
        if (l_pCtrl != nullptr) {
          it->second.m_pHUD->setAiController(l_pCtrl);
          l_pCtrl->setHUD(it->second.m_pHUD);
        }

        for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
          if (it->second.m_pHUD != nullptr) {
            bool l_bHighligh     = m_cSettings.m_aGameGFX[m_mViewports.size()].m_bHighlight   ;
            bool l_bShowCtrl     = m_cSettings.m_aGameGFX[m_mViewports.size()].m_bShowControls;
            bool l_bShowRanking  = m_cSettings.m_aGameGFX[m_mViewports.size()].m_bShowRanking  && it->second.m_pPlayer->m_pPlayer->m_bShowRanking;
            bool l_bShowLapTimes = m_cSettings.m_aGameGFX[m_mViewports.size()].m_bShowLapTimes && it->second.m_pPlayer->m_pPlayer->m_bShowRanking;

            it->second.m_pHUD->setSettings(l_bHighligh, l_bShowCtrl, l_bShowRanking, l_bShowLapTimes);
          }
        }
        it->second.m_pHUD->drop();
      }

      if (m_mViewports.size() == 0) {
        // No viewport ==> view track, create a viewport
        irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
        l_vOffset.rotateXZBy(m_fGridAngle);

        m_pCamAnimator = new scenenodes::CMyCameraAnimator(m_pDevice);
        m_pCamera = m_pSmgr->addCameraSceneNode(m_pSmgr->getRootSceneNode(), l_vOffset);
        m_pCamera->addAnimator(m_pCamAnimator);
        m_pSmgr->setActiveCamera(m_pCamera);
        gfx::SViewPort l_cViewport = gfx::SViewPort(irr::core::recti(irr::core::position2di(0, 0), m_pDrv->getScreenSize()), 0, nullptr, m_pCamera);
        m_mViewports[0] = l_cViewport;

        if (m_vCameras.size() > 0) {
          irr::core::vector3df l_vPos;
          irr::f32 l_fAngleH, l_fAngleV;

          m_pCamAnimator->animateNode(m_pCamera, 0);
          (*m_vCameras.begin())->getValues(l_vPos, l_fAngleV, l_fAngleH);
          m_pCamAnimator->setData(l_vPos, l_fAngleV, l_fAngleH);
        }
      }

      m_pGridNode->removeUnusedMarbles();
      prepareShader();

      if (m_pDynamics != nullptr)
        m_pDynamics->startThread();

      if (m_pAiThread != nullptr)
        m_pAiThread->startThread();

      std::string l_sSceneScript = helpers::loadTextFile("data/levels/" + m_pRaceData->m_sTrack + "/scene.lua");

      if (l_sSceneScript != "") {
        m_pLuaScript = new lua::CLuaScript_scene(l_sSceneScript);
        m_pLuaScript->initializesingleton();
        m_pLuaScript->initialize();
      }

      for (auto l_cPlayer: m_pRaceData->m_vPlayers) {
        if (l_cPlayer->m_pController != nullptr && l_cPlayer->m_pMarble != nullptr && l_cPlayer->m_pMarble->m_pViewport != nullptr && l_cPlayer->m_pMarble->m_pViewport->m_pHUD != nullptr && l_cPlayer->m_pMarble->m_pViewport->m_pHUD->moveToFront()) {
          l_cPlayer->m_pController->moveGuiToFront();
        }
      }
    }

    /**
     * This function receives messages of type "PlayerRespawn"
     * @param a_MarbleId ID of the marble
     * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
     */
    void CGameState::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      m_pRaceData->onStateChange(a_MarbleId, a_State == 1 ? 2 : 0, m_iStep);

      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (a_State == 1) {
          p->m_bCamLink = false;
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Respawn1;
          p->m_iStateChange  = m_iStep;
          p->m_iRespawnStart = m_iStep;

          m_pSoundIntf->playMarbleOneShotSound(a_MarbleId, enOneShots::RespawnStart);
        }
        else {
          p->m_bCamLink = true;
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
          p->m_iStateChange  = -1;
          p->m_iRespawnStart = -1;

          for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
            if ((*it)->m_pController != nullptr)
              (*it)->m_pController->onMarbleRespawn(a_MarbleId);
          }
        }

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onplayerrespawn(a_MarbleId, a_State == 1 ? 1 : 3);
      }
    }

    /**
     * This function receives messages of type "CameraRespawn"
     * @param a_MarbleId The ID of the marble which is respawning
     * @param a_Position The new position of the camera
     * @param a_Target The new target of the camera, i.e. the future position of the marble
     */
    void CGameState::onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target) {
      m_pRaceData->onStateChange(a_MarbleId, 3, m_iStep);

      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          if (p->m_pViewport != nullptr && p->m_pViewport->m_pCamera != nullptr) {
            p->m_pViewport->m_pCamera->setPosition(a_Position);
            p->m_pViewport->m_pCamera->setTarget(a_Target);
            p->m_pViewport->m_pCamera->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));
          }

          m_pSoundIntf->playMarbleOneShotSound(a_MarbleId, enOneShots::RespawnDone);

          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Respawn2;
          p->m_iStateChange = m_iStep;
        }

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onplayerrespawn(a_MarbleId, 2);
      }
    }

    /**
     * This function receives messages of type "PlayerStunned"
     * @param a_MarbleId ID of the marble
     * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
     */
    void CGameState::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      m_pRaceData->onStateChange(a_MarbleId, a_State == 1 ? 1 : 0, m_iStep);

      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;
        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          if (a_State == 1) {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Stunned;
            p->m_iStateChange = m_iStep;

            m_pSoundIntf->playMarbleStunned(a_MarbleId, p->m_pPositional->getAbsolutePosition());
          }
          else {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
            p->m_iStateChange = -1;

            m_pSoundIntf->stopMarbleStunned(a_MarbleId);
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
    void CGameState::onPlayerfinished(irr::s32 a_MarbleId, irr::s32 a_RaceTime, irr::s32 a_Laps) {
      m_pRaceData->onStateChange(a_MarbleId, 4, m_iStep);

      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;
        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Finished;
          p->m_bCamLink = false;
          p->m_iStateChange = m_iStep;
          m_pSoundIntf->playMarbleOneShotSound(a_MarbleId, enOneShots::GameOver);

          if (p->m_pPlayer->m_pController != nullptr)
            p->m_pPlayer->m_pController->playerFinished();
        }
        if (m_pLuaScript != nullptr)
          m_pLuaScript->onplayerfinished(a_MarbleId, a_RaceTime, a_Laps);
      }
    }

    /**
     * This function receives messages of type "RaceFinished"
     * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
     */
    void CGameState::onRacefinished(irr::u8 a_Cancelled) {
      if (m_iFinished == -1) {
        printf("\nCGameState::onRacefinished: %s\n", a_Cancelled != 0 ? "true" : "false");
        m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStFinish);
      }

      if (m_iFinished == -1 || a_Cancelled != 0) {
        printf("\nCGameState::onRacefinished: %s\n", a_Cancelled != 0 ? "true" : "false");
        m_iFinished = a_Cancelled ? m_iStep - 1500 : m_iStep;

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onracefinished(a_Cancelled != 0);
      }
    }

    /**
     * This function receives messages of type "Checkpoint"
     * @param a_MarbleId ID of the marble
     * @param a_Checkpoint The checkpoint ID the player has passed
     * @param a_iStepNo The simulation step when the checkpoint was triggered
     */
    void CGameState::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint, irr::s32 a_iStepNo) {
      int l_iId = a_MarbleId - 10000;

      m_pRaceData->onCheckpoint(a_MarbleId, a_Checkpoint, a_iStepNo);

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr) {
        m_pSoundIntf->playMarbleOneShotSound(a_MarbleId, enOneShots::Checkpoint);

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

        if (m_pLuaScript != nullptr)
          m_pLuaScript->oncheckpoint(a_MarbleId, a_Checkpoint);

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
          if ((*it)->m_pController != nullptr)
            (*it)->m_pController->onCheckpoint(a_MarbleId, a_Checkpoint);
        }
      }
    }

    /**
     * This function receives messages of type "LapStart"
     * @param a_MarbleId ID of the marble
     * @param a_LapNo Number of the started lap
     */
    void CGameState::onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) {
      m_pRaceData->onLapStart(a_MarbleId);

      int l_iIndex = a_MarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        m_aMarbles[l_iIndex]->m_pPlayer->m_pRaceData ->m_iLapNo++;

        if (m_pPanelRndr != nullptr)
          m_pPanelRndr->updateCurrentLap(m_aMarbles[l_iIndex]->m_pPlayer->m_pRaceData->m_iLapNo);
        
      }
      m_pSoundIntf->playMarbleOneShotSound(a_MarbleId, enOneShots::Lap);
    }

    void CGameState::addStaticCameras(irr::scene::ISceneNode *a_pNode) {
      if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_DustbinCameraId) {
        m_vCameras.push_back(reinterpret_cast<scenenodes::CDustbinCamera *>(a_pNode));
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RostrumNodeId) {
        m_pRostrum = reinterpret_cast<scenenodes::CRostrumNode *>(a_pNode);
      }

      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
        addStaticCameras(*it);
    }

    /**
    * Some error has happened, show to the user
    * @param a_sHeadline the headline of the error
    * @param a_sMessage the message of the error
    */
    void CGameState::handleError(const std::string& a_sHeadline, const std::string& a_sMessage) {
      m_pGlobal->setGlobal("message_headline", a_sHeadline);
      m_pGlobal->setGlobal("message_text"    , a_sMessage);

      state::CMenuState *l_pMenu = reinterpret_cast<state::CMenuState *>(m_pGlobal->getState(state::enState::Menu));

      if (l_pMenu != nullptr) {
        l_pMenu->pushToMenuStack("menu_message");
      }

      m_iFinished = m_iStep;
      m_bEnded    = true;
    }

    /**
    * This function receives messages of type "ServerDisconnect"
    */
    void CGameState::onServerdisconnect() {
      m_pGlobal->setGlobal("message_headline", "Server Disconnected");
      m_pGlobal->setGlobal("message_text"    , "The server has closed the connection.");

      state::CMenuState *l_pMenu = reinterpret_cast<state::CMenuState *>(m_pGlobal->getState(state::enState::Menu));

      if (l_pMenu != nullptr) {
        l_pMenu->pushToMenuStack("menu_message");
      }

      m_iFinished = m_iStep;
      m_bEnded    = true;
    }

    /**
    * This function receives messages of type "RaceResult"
    * @param a_data Encoded SRacePlayer structure
    */
    void CGameState::onRaceresult(const std::string& a_data) {
    }

    /**
    * This function receives messages of type "EndRaceState"
    */
    void CGameState::onEndracestate() {
      m_bEnded = true;
    }

    /**
    * This function receives messages of type "PlayerRemoved"
    * @param a_playerid ID of the removed player
    */
    void CGameState::onPlayerremoved(irr::s32 a_playerid) {
      for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
        if ((*it)->m_iPlayer == a_playerid) {
          printf("Marble %i is withdrawn from race.\n", (*it)->m_pMarble->m_pPositional->getID());
            
          messages::CPlayerWithdraw l_cMsg = messages::CPlayerWithdraw((*it)->m_pMarble->m_pPositional->getID());

          // Message needs to be posted twice, request and confirm withdraw
          m_pDynamics->getInputQueue()->postMessage(&l_cMsg);
          m_pDynamics->getInputQueue()->postMessage(&l_cMsg);
          return;
        }
      }
    }

    /**
    * This function receives messages of type "PauseChanged"
    * @param a_Paused The current paused state
    */
    void CGameState::onPausechanged(bool a_Paused) {
      m_pSoundIntf->pauseGame(a_Paused);
    }

    /**
    * This function receives messages of type "LuaMessage"
    * @param a_NumberOne First number for any information
    * @param a_NumberTwo Other number for any information
    * @param a_Data String for any further information
    */
    void CGameState::onLuamessage(irr::s32 a_NumberOne, irr::s32 a_NumberTwo, const std::string& a_Data) {
      if (m_pLuaScript != nullptr)
        m_pLuaScript->onluamessage(a_NumberOne, a_NumberTwo, a_Data);
    }

    /**
    * Update the race positions
    */
    void CGameState::updateRacePositions() {
      if (m_pPanelRndr != nullptr)
        m_pPanelRndr->updateRanking(m_pRaceData->m_vRanking);
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_Deficit Deficit of the marble on the leader in steps
    */
    void CGameState::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
      int l_iIndex = a_MarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        updateRacePositions();

        for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
          if (it->second.m_pHUD != nullptr) {
            it->second.m_pHUD->updateRanking();
          }
        }

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onraceposition(a_MarbleId, a_Position, a_Laps, a_DeficitAhead, a_DeficitLeader);

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_pRaceData->m_vPlayers.begin(); it != m_pRaceData->m_vPlayers.end(); it++) {
          if ((*it)->m_pController != nullptr)
            (*it)->m_pController->onRaceposition(a_MarbleId, a_Position, a_Laps, a_DeficitAhead, a_DeficitLeader);
        }
      }
    }

    /**
    * This function receives messages of type "PlayerWithdrawn"
    * @param a_MarbleId ID of the marble
    */
    void CGameState::onPlayerwithdrawn(irr::s32 a_MarbleId) {
      int l_iIndex = a_MarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        m_aMarbles[l_iIndex]->m_pPlayer->m_pRaceData->m_bWithdrawn = true;
      }

      updateRacePositions();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++)
        if (it->second.m_pHUD != nullptr)
          it->second.m_pHUD->updateRanking();

      if (m_pLuaScript != nullptr)
        m_pLuaScript->onplayerwithdrawn(a_MarbleId);
    }

    /**
    * Get the texture with the race information
    * @return the texture with the race information
    */
    irr::video::ITexture *CGameState::getRaceInfoTexture() {
      if (m_pPanelRndr == nullptr) {
        m_pPanelRndr = new gui::CInGamePanelRenderer(m_pDevice, m_pRaceData->m_vPlayers, m_pRaceData->m_iLaps);
      }

      return m_pPanelRndr->getRaceInfoRTT();
    }

    /**
    * Get the texture for the race counter
    * @return the texture for the race counter
    */
    irr::video::ITexture *CGameState::getLapCountTexture() {
      if (m_pPanelRndr == nullptr && m_pRaceData != nullptr) {
        m_pPanelRndr = new gui::CInGamePanelRenderer(m_pDevice, m_pRaceData->m_vPlayers, m_pRaceData->m_iLaps);
      }

      return m_pPanelRndr != nullptr ? m_pPanelRndr->getLapCountRTT() : nullptr;
    }
  }
}