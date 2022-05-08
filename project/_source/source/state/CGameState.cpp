// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#ifndef NO_XEFFECT
#include <shader/CShaderHandleXEffectSplitscreen.h>
#include <shader/CShaderHandlerXEffect.h>
#endif

#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CMarbleCountSceneNode.h>
#include <controller/ICustomEventReceiver.h>
#include <_generated/messages/CMessages.h>
#include <controller/CControllerFactory.h>
#include <controller/CAiControlThread.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinCamera.h>
#include <shader/CShaderHandlerBase.h>
#include <controller/CControllerAI.h>
#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CSkyBoxFix.h>
#include <scenenodes/CWorldNode.h>
#include <sound/ISoundInterface.h>
#include <shader/CMyShaderNone.h>
#include <network/CGameClient.h>
#include <network/CGameServer.h>
#include <scenenodes/CAiNode.h>
#include <sound/CSoundEnums.h>
#include <data/CDataStructs.h>
#include <state/CGameState.h>
#include <gui/CGameHUD.h>
#include <CMainClass.h>
#include <CGlobal.h>
#include <algorithm>
#include <random>
#include <string>
#include <map>

#ifdef _TOUCH_CONTROL
#include <gui/CGuiTouchControl.h>
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
      m_bPaused        (false),
      m_pSoundIntf     (nullptr),
      m_pDynamics      (nullptr),
      m_pRostrum       (nullptr),
      m_pShader        (nullptr),
      m_pCamAnimator   (nullptr),
      m_pCamera        (nullptr),
      m_pAiThread      (nullptr),
      m_pRace          (nullptr),
      m_pClient        (nullptr),
      m_pServer        (nullptr),
      m_iNumOfViewports(0),
      m_pGridNode      (nullptr),
      m_fGridAngle     (0.0f),
      m_pAiNode        (nullptr)
#ifdef _TOUCH_CONTROL
      ,m_pTouchControl(nullptr)
#endif
    {
      m_cScreen = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());

      m_pCheckpointTextures[0] = m_pDrv->getTexture("data/textures/checkpoint_white.png");
      m_pCheckpointTextures[1] = m_pDrv->getTexture("data/textures/checkpoint_flash1.png");
      m_pCheckpointTextures[2] = m_pDrv->getTexture("data/textures/checkpoint_flash2.png");

      m_pSoundIntf = a_pGlobal->getSoundInterface();
    }

    CGameState::~CGameState() {
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
        else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_TrigerTimerNodeId) {
          scenenodes::CTriggerTimeNode *p = reinterpret_cast<scenenodes::CTriggerTimeNode *>(a_pNode);
          m_vTimerActions.push_back(scenenodes::STriggerVector(p->m_vActions));
        }
        else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_MarbleCountNodeId) {
          scenenodes::CMarbleCountSceneNode *p = reinterpret_cast<scenenodes::CMarbleCountSceneNode *>(a_pNode);
          m_vMarbleCounters.push_back(gameclasses::CMarbleCounter(p->m_iTriggerPlus, p->m_iTriggerMinus, p->m_vActions));
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
      l_cTwoPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
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
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
      l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
      m_cViewports.m_mDistribution[5] = l_cFivePlayers;

      SGameViewports::SViewportDistribution l_cSixPlayers = SGameViewports::SViewportDistribution(2, 3);
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player));
      l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player));
      m_cViewports.m_mDistribution[6] = l_cSixPlayers;

      SGameViewports::SViewportDistribution l_cSevenPlayers = SGameViewports::SViewportDistribution(3, 3);
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player  ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Logo    ));
      l_cSevenPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 2, SGameViewports::enType::Player  ));
      m_cViewports.m_mDistribution[7] = l_cSevenPlayers;

      SGameViewports::SViewportDistribution l_cEightPlayers = SGameViewports::SViewportDistribution(3, 3);
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Racedata));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 2, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 2, SGameViewports::enType::Player  ));
      l_cEightPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 2, SGameViewports::enType::Player  ));
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

        if (m_pDevice->getCursorControl() != nullptr)
          m_pDevice->getCursorControl()->setVisible(false);
      }

      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
      l_vOffset.rotateXZBy(a_fAngle);

      for (size_t i = 0; i < m_cPlayers.m_vPlayers.size(); i++) {
        if (m_cPlayers.m_vPlayers[i].m_iViewPort != -1 && m_cPlayers.m_vPlayers[i].m_iPlayerId == a_pPlayer->m_iPlayer) {
          if (a_pPlayer != nullptr && a_pPlayer->m_pMarble->m_pPositional) {
            SGameViewports::SViewportDef l_cViewportDef = m_cViewports.m_mDistribution[m_iNumOfViewports].m_vViewports[m_cPlayers.m_vPlayers[i].m_iViewPort - 1];

            printf("Viewport %i / %i assigned to player \"%s\" (%i).\n", m_cPlayers.m_vPlayers[i].m_iViewPort, m_iNumOfViewports, m_cPlayers.m_vPlayers[i].m_sName.c_str(), a_pPlayer->m_pMarble->m_pPositional->getID());

            irr::core::recti l_cRect = irr::core::recti(
               l_cViewportDef.m_iColumn      * l_cViewportSize.Width,
               l_cViewportDef.m_iRow         * l_cViewportSize.Height,
              (l_cViewportDef.m_iColumn + 1) * l_cViewportSize.Width,
              (l_cViewportDef.m_iRow    + 1) * l_cViewportSize.Height
            );


            irr::scene::ICameraSceneNode* l_pCam = m_pSmgr->addCameraSceneNode(m_pSmgr->getRootSceneNode(), a_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition() + l_vOffset, a_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition());
            l_pCam->setAspectRatio((((irr::f32)l_cRect.LowerRightCorner.X) - ((irr::f32)l_cRect.UpperLeftCorner.X)) / (((irr::f32)l_cRect.LowerRightCorner.Y) - ((irr::f32)l_cRect.UpperLeftCorner.Y)));
            l_pCam->updateAbsolutePosition();

            gfx::SViewPort l_cViewport = gfx::SViewPort(l_cRect, m_cPlayers.m_vPlayers[i].m_iPlayerId, a_pPlayer->m_pMarble->m_pPositional, l_pCam);

            for (std::map<irr::s32, scenenodes::CCheckpointNode*>::iterator it = m_mCheckpoints.begin(); it != m_mCheckpoints.end(); it++) {
              if (it->second->m_bFirstInLap && it->second->getParent()->getType() == irr::scene::ESNT_MESH) {
                l_cViewport.m_vNextCheckpoints.push_back(reinterpret_cast<irr::scene::IMeshSceneNode *>(it->second->getParent()));
              }
            }

            l_cViewport.m_pPlayer = a_pPlayer->m_pMarble;
            m_mViewports[m_cPlayers.m_vPlayers[i].m_iPlayerId] = l_cViewport;
            a_pPlayer->m_pMarble->m_pViewport = &m_mViewports[m_cPlayers.m_vPlayers[i].m_iPlayerId];
          }
        }
      }

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (it->second.m_pHUD != nullptr) {
          it->second.m_pHUD->setSettings(m_cSettings.m_aGameGFX[m_mViewports.size()].m_bHightlight, m_cSettings.m_aGameGFX[m_mViewports.size()].m_bShowControls, m_cSettings.m_aGameGFX[m_mViewports.size()].m_bShowRanking);
        }
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

      int l_iCols = m_cViewports.m_mDistribution[m_iNumOfViewports].m_iColumns;
      int l_iRows = m_cViewports.m_mDistribution[m_iNumOfViewports].m_iRows;

      irr::core::dimension2du l_cDim = irr::core::dimension2du(l_cScreen.Width / l_iCols, l_cScreen.Height / l_iRows);

      switch (m_cSettings.m_iShadows) {
        case 3:
  #ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, m_cSettings.m_iAmbient);
          }
          break;
  #endif

        case 2:
  #ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, m_cSettings.m_iAmbient);
          }
          break;
  #endif

        case 1:
  #ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, m_cSettings.m_iAmbient);
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
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      m_iStep           = 0;
      m_iFadeOut        = -1;
      m_iFinished       = -1;
      m_eState          = enGameState::Countdown;
      m_bPaused         = false;
      m_fSfxVolume      = m_pGlobal->getSettingData().m_fSfxGame;
      m_iNumOfViewports = 0;

      m_pGui->clear();

      m_pClient = m_pGlobal->getGameClient();
      m_pServer = m_pGlobal->getGameServer();

      m_cGameData = data::SGameData(m_pGlobal->getGlobal("gamedata"));
      m_cSettings = m_pGlobal->getSettingData();

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      // Load the track, and don't forget to run the skybox fix beforehands
      std::string l_sTrack = "data/levels/" + m_cGameData.m_sTrack + "/track.xml";

      if (m_pFs->existFile(l_sTrack.c_str())) {
        m_pSmgr->clear();
        scenenodes::CSkyBoxFix* l_pFix = new scenenodes::CSkyBoxFix(m_pDrv, m_pSmgr, m_pFs, l_sTrack.c_str());
        m_pSmgr->loadScene(l_sTrack.c_str());
        l_pFix->hideOriginalSkybox(m_pSmgr->getRootSceneNode());
        delete l_pFix;
        addStaticCameras(m_pSmgr->getRootSceneNode());
        printf("%i static cameras found.\n", (int)m_vCameras.size());
      }
      else {
        // ToDo Error Message
      }

#ifdef _OPENGL_ES
      adjustNodeMaterials(m_pSmgr->getRootSceneNode());
#endif

      fillCheckpointList(m_pSmgr->getRootSceneNode());

      if (m_mCheckpoints.size() == 0) {
        // ToDo Error Message
      }

      irr::scene::ISceneNode* l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId, m_pSmgr->getRootSceneNode());

      if (l_pNode == nullptr) {
        m_pGlobal->setGlobal("ERROR_MESSAGE", "No Starting Grid Scene Node found.");
        m_pGlobal->setGlobal("ERROR_HEAD", "Error while starting game state.");
        // ToDo Error Message
      }

      m_pGridNode = reinterpret_cast<scenenodes::CStartingGridSceneNode *>(l_pNode);

      if (m_pGridNode == nullptr) {
        printf("Grid node not found.\n");
        // ToDo Error Message
        return;
      }

      m_fGridAngle = m_pGridNode->getAngle();

      int  l_iGridOrder  = 0;
      int  l_iAutoFinish = 0;
      bool l_bGridRevert = false;

      std::string l_sSettings = m_pGlobal->getSetting("gamesetup");
      if (l_sSettings != "") {
        data::SGameSettings l_cSettings;
        l_cSettings.deserialize(l_sSettings);
        l_iGridOrder  = l_cSettings.m_iGridPos;
        l_bGridRevert = l_cSettings.m_bReverseGrid;
        l_iAutoFinish = l_cSettings.m_iAutoFinish;
      }

      data::SChampionship      l_cChampionship = data::SChampionship(m_pGlobal->getGlobal("championship"));
      data::SChampionshipRace *l_pLastRace     = l_cChampionship.getLastRace();

      std::string l_sPlayers = m_pGlobal->getGlobal("raceplayers");
      m_cPlayers.deserialize(l_sPlayers);

      printf("\n**********\n");
      printf("%s", m_cPlayers.toString().c_str());
      printf("\n**********\n");

      // Find out how many viewports we need to create
      for (size_t i = 0; i < m_cPlayers.m_vPlayers.size(); i++) {
        if (m_cPlayers.m_vPlayers[i].m_iViewPort != -1)
          m_iNumOfViewports++;
      }

      if (l_pLastRace != nullptr && l_iGridOrder != 0) {
        // Grid order: Result of last race
        if (l_iGridOrder == 1) {
          // Go through the players ..
          for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
            // .. next go through the last race result ..
            for (int i = 0; i < l_cChampionship.m_iGridSize; i++) {
              if (l_pLastRace->m_mAssignment.find(l_pLastRace->m_aResult[i].m_iId) != l_pLastRace->m_mAssignment.end()) {
                // .. and search for the player
                if ((*it).m_iPlayerId == l_pLastRace->m_mAssignment[l_pLastRace->m_aResult[i].m_iId]) {
                  (*it).m_iGridPos = i;
                  break;
                }
              }
            }
          }
        }
        else if (l_iGridOrder == 2) {
          // Get the current standings ..
          std::vector<data::SChampionshipPlayer> l_vStanding = l_cChampionship.getStandings();
          // .. iterate of the standings ..
          int l_iPos = 0;
          for (std::vector<data::SChampionshipPlayer>::iterator it2 = l_vStanding.begin(); it2 != l_vStanding.end(); it2++) {
            for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
              if ((*it).m_iPlayerId == (*it2).m_iPlayerId) {
                (*it).m_iGridPos = l_iPos;
                break;
              }
            }
            l_iPos++;
          }
        }
        else if (l_iGridOrder == 3) {
          std::vector<int> l_vGrid;
          for (int i = 1; i <= (int)m_cPlayers.m_vPlayers.size(); i++)
            l_vGrid.push_back(i);

          std::random_device l_cRd { };
          std::default_random_engine l_cRe { l_cRd() };

          std::shuffle(l_vGrid.begin(), l_vGrid.end(), l_cRe);
          for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
            (*it).m_iGridPos = *l_vGrid.begin();
            l_vGrid.erase(l_vGrid.begin());
          }
        }
      }

      // First we sort the player vector by the grid position as the first step will be marble assignment ..
      std::sort(m_cPlayers.m_vPlayers.begin(), m_cPlayers.m_vPlayers.end(), [](const data::SPlayerData &a_cPlayer1, const data::SPlayerData &a_cPlayer2) {
        return a_cPlayer1.m_iGridPos < a_cPlayer2.m_iGridPos;
      });

      if (l_bGridRevert && l_iGridOrder != 0 && l_pLastRace != nullptr)
        std::reverse(m_cPlayers.m_vPlayers.begin(), m_cPlayers.m_vPlayers.end());


      m_pRace = new data::SChampionshipRace(m_pGlobal->getGlobal("track"), (int)m_cPlayers.m_vPlayers.size(), m_cGameData.m_iLaps);

      fillMovingMap(findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode()));

      if (m_pInputQueue  == nullptr) m_pInputQueue  = new threads::CInputQueue ();
      if (m_pOutputQueue == nullptr) m_pOutputQueue = new threads::COutputQueue();

      hideAiNode();

      if (m_pClient == nullptr) {
        l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode());
      
        if (l_pNode != nullptr) {
          gameclasses::CDynamicThread::enAutoFinish l_eAutoFinish;

          switch (l_iAutoFinish) {
            case 0: l_eAutoFinish = gameclasses::CDynamicThread::enAutoFinish::AllPlayers  ; break;
            case 1: l_eAutoFinish = gameclasses::CDynamicThread::enAutoFinish::SecondToLast; break;
            case 2: l_eAutoFinish = gameclasses::CDynamicThread::enAutoFinish::FirstPlayer ; break;
            case 3: l_eAutoFinish = gameclasses::CDynamicThread::enAutoFinish::PlayersAndAI; break;
          }

          m_pDynamics = new gameclasses::CDynamicThread();

          m_pDynamics->getOutputQueue()->addListener(m_pInputQueue);
          m_pOutputQueue->addListener(m_pDynamics->getInputQueue());

          if (m_pServer != nullptr) {
            m_pDynamics->getOutputQueue()->addListener(m_pServer->getInputQueue());
            m_pServer->getOutputQueue()->addListener(m_pDynamics->getInputQueue());
          }

          m_pDynamics->setupGame(reinterpret_cast<scenenodes::CWorldNode*>(l_pNode), m_pGridNode, m_cPlayers.m_vPlayers, m_cGameData.m_iLaps, m_vTimerActions, m_vMarbleCounters, l_eAutoFinish);
        }
        else {
          m_pGlobal->setGlobal("ERROR_MESSAGE", "No world node found.");
          m_pGlobal->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
          // throw std::exception();
        }
      }
      else {
        m_pOutputQueue->addListener(m_pClient->getInputQueue());
        m_pClient->getOutputQueue()->addListener(m_pInputQueue);
        m_pClient->stateChanged("state_game");
      }

#ifdef _TOUCH_CONTROL
      if (m_pGlobal->getSettingData().m_bTouchControl && m_pCamAnimator == nullptr) {
        m_pTouchControl = reinterpret_cast<gui::CGuiTouchControl *>(m_pGui->addGUIElement(gui::g_TouchControlName, m_pGui->getRootGUIElement()));
      }
#endif
    }

    /**
    * This method is called when the state is deactivated
    */
    void CGameState::deactivate() {
      m_mMoving.clear();

      if (m_pDevice->getCursorControl() != nullptr)
        m_pDevice->getCursorControl()->setVisible(true);

      // Important: stop the AI thread
      // before the dynamics thread
      if (m_pAiThread != nullptr) {
        m_pAiThread->stopThread();
        m_pAiThread->join();
        delete m_pAiThread;
        m_pAiThread = nullptr;
      }

      if (m_pDynamics != nullptr) {
        m_pDynamics->stopThread();
        m_pDynamics->join();

        messages::IMessage *l_pMsg = nullptr;
        do {
          l_pMsg = m_pInputQueue->popMessage();
          if (l_pMsg != nullptr)
            handleMessage(l_pMsg);
        }
        while (l_pMsg != nullptr);

        if (m_pRace != nullptr) {
          const std::vector<data::SRacePlayer*> l_vResult = m_pDynamics->getRaceResult();

          printf("\n***** Race Result *****\n\n");
          int l_iNum = 0;
          for (std::vector<data::SRacePlayer*>::const_iterator it = l_vResult.begin(); it != l_vResult.end(); it++) {
            m_pRace->m_aResult[l_iNum] = data::SRacePlayer(*(*it));

            for (int i = 0; i < 16 && m_aMarbles[i] != nullptr; i++) {
              if (m_aMarbles[i]->m_pPositional->getID() == (*it)->m_iId) {
                std::string s = m_aMarbles[i]->m_pPlayer->m_sName;
                if (s.size() > 20)
                  s = s.substr(0, 20);

                while (s.size() < 20)
                  s += " ";

                printf("%2i: %s | %.2f\n", (*it)->m_iPos, s.c_str(), ((irr::f32)(*it)->m_iDeficitL) / 120.0f);
              }
            }

            l_iNum++;
          }

          printf("\n***********************\n\n");

          data::SChampionship l_cChampionship = data::SChampionship(m_pGlobal->getGlobal("championship"));
          l_cChampionship.addRace(*m_pRace);
          m_pGlobal->setGlobal("championship", l_cChampionship.serialize());
          printf("\n\n%s\n\n", l_cChampionship.serialize().c_str());

          if (m_pServer != nullptr)
            m_pServer->sendGlobalData("championship");
        }

        if (m_pServer != nullptr) {
          m_pServer->getOutputQueue()->removeListener(m_pDynamics->getInputQueue());
        }

        if (m_pClient != nullptr) {
          m_pClient->getOutputQueue()->removeListener(m_pDynamics->getInputQueue());
        }

        delete m_pDynamics;
        m_pDynamics = nullptr;

        m_pClient = nullptr;
        m_pServer = nullptr;
      }

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++)
        delete* it;

      m_vPlayers  .clear();
      m_vPosition .clear();
      m_mViewports.clear();

#ifdef _TOUCH_CONTROL
      m_pTouchControl = nullptr;
#endif

      if (m_pCamAnimator != nullptr) {
        m_pCamAnimator->drop();
        m_pCamAnimator = nullptr;
      }

      m_pSmgr->clear();
      m_pGui->clear();

      m_vTimerActions.clear();

      for (std::vector<messages::IMessage*>::iterator it = m_vMoveMessages.begin(); it != m_vMoveMessages.end(); it++) {
        delete* it;
      }
      m_vMoveMessages.clear();

      if (m_pShader != nullptr) {
        delete m_pShader;
        m_pShader = nullptr;
      }

      m_mCheckpoints.clear();
      m_vCameras    .clear();

      if (m_pRace != nullptr) {
        delete m_pRace;
        m_pRace = nullptr;
      }
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
    * Change the Z-Layer for the Menu Controller
    * @param a_iZLayer the new Z-Layer
    */
    void CGameState::setZLayer(int a_iZLayer) {
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

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        if ((*it)->m_pController != nullptr)
          (*it)->m_pController->update(a_cEvent);
      }

#ifdef _TOUCH_CONTROL
      if (m_pTouchControl != nullptr)
        l_bRet = m_pTouchControl->OnEvent(a_cEvent);
#endif

      if (!l_bRet && a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (!a_cEvent.KeyInput.PressedDown) {
          if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) {
            sendCancelrace(m_pOutputQueue);
            l_bRet = true;
          }
          else if (a_cEvent.KeyInput.Key == irr::KEY_PAUSE) {
            sendTogglepause(m_pOutputQueue);
            l_bRet = true;
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
            int l_iMarble = m_mViewports.begin()->second.m_pMarble->getID();
            if (l_iMarble >= 10000 && l_iMarble < 10016) {
              messages::CPlayerWithdraw l_cMessage = messages::CPlayerWithdraw(l_iMarble);
              m_pOutputQueue->postMessage(&l_cMessage);
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
          irr::core::vector3df l_cNormal   = (a_pViewPort->m_pCamera->getTarget() - a_pViewPort->m_pCamera->getAbsolutePosition()).normalize(),
                               l_cPosition = a_pViewPort->m_pCamera->getTarget() + 0.1f * (a_pViewPort->m_pCamera->getAbsolutePosition() - a_pViewPort->m_pCamera->getTarget());
          irr::core::plane3df  l_cPlane    = irr::core::plane3df(l_cPosition, l_cNormal);

          for (int i = 0; i < 16; i++) {
            if (m_aMarbles[i] != nullptr) {
              bool l_bRespawn = m_aMarbles[i]->m_eState == gameclasses::SMarbleNodes::enMarbleState::Respawn1 || m_aMarbles[i]->m_eState == gameclasses::SMarbleNodes::enMarbleState::Respawn2,
                   l_bOther   = m_aMarbles[i]->m_pPositional != a_pViewPort->m_pMarble && m_aMarbles[i]->m_pPositional != nullptr && m_aMarbles[i]->m_pRotational->getMesh()->getMeshBufferCount() > 0;

              if (l_bOther || l_bRespawn) {
                bool l_bBehind = l_cPlane.classifyPointRelation(m_aMarbles[i]->m_pPositional->getAbsolutePosition()) == irr::core::ISREL3D_BACK;
                if (l_bBehind || l_bRespawn) {
                  irr::scene::IMeshBuffer* l_pBuffer = m_aMarbles[i]->m_pRotational->getMesh()->getMeshBuffer(0);
                  m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_VERTEX_ALPHA;

                  irr::video::S3DVertex* l_pVertices = (irr::video::S3DVertex*)l_pBuffer->getVertices();

                  // The "Behind Camera" Alpha Value
                  irr::u32 l_iAlpha = 96;

                  // If the marble is respawning ..
                  if (l_bRespawn) {
                    // .. we calculate a factor ..
                    irr::f32 l_fFactor = 1.0f - (irr::f32)(m_iStep - m_aMarbles[i]->m_iRespawnStart) / 120.0f;
                    if (l_fFactor < 0.1f)
                      l_fFactor = 0.1f;

                    l_iAlpha = (irr::u32)(255.0f * l_fFactor);

                    // .. and if the marble is behind the camera we make sure
                    // the factor does not exceed "96"
                    if (l_iAlpha > 96 && l_bBehind)
                      l_iAlpha = 96;
                  }

                  for (irr::u32 j = 0; j < l_pBuffer->getVertexCount(); j++)
                    l_pVertices[j].Color.setAlpha(l_iAlpha);
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
        else {
          for (int i = 0; i < 16; i++) {
            if (m_aMarbles[i] != nullptr) {
              m_aMarbles[i]->m_pRotational->getMaterial(0).MaterialType = m_pShader == nullptr ? irr::video::EMT_SOLID : m_pShader->getMaterialType();
            }
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

#ifdef _TOUCH_CONTROL
      if (m_pCamAnimator != nullptr && m_pCamera != nullptr)
        m_pCamAnimator->animateNode(m_pCamera, 0);

      if (m_pTouchControl != nullptr) {
        irr::s8 l_iCtrlX    = 0,
                l_iCtrlY    = 0;
        bool    l_bBrake    = false,
                l_bRespawn  = false,
                l_bRearView = false;

        m_pTouchControl->getControl(l_iCtrlX, l_iCtrlY, l_bBrake, l_bRespawn, l_bRearView);

        messages::CMarbleControl l_cMessage = messages::CMarbleControl(m_mViewports.begin()->second.m_pMarble->getID(), l_iCtrlX, l_iCtrlY, l_bBrake, l_bRearView, l_bRespawn);
        m_pOutputQueue->postMessage(&l_cMessage);
      }
#endif

      do {
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
      while (l_pMsg != nullptr);

      if (!m_pDevice->run())
        return enState::Quit;

      m_pDrv->beginScene(true, true, irr::video::SColor(255, 0, 0, 0));

      if (m_pShader != nullptr)
        m_pShader->beginScene();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (m_pShader != nullptr) {
          if (it->second.m_pCamera != nullptr)
            m_pSmgr->setActiveCamera(it->second.m_pCamera);

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

              default:
                break;
            }
          }
        }
      }

      m_pGui->drawAll();

      // Fade-out of the HUD after the race must be drawn after the rest of the GUI so that the gray rectangle
      // showing the result does not overlay the fade-out
      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        if (it->second.m_pPlayer != nullptr && it->second.m_pPlayer->m_iStateChange != -1) {
          if (it->second.m_pPlayer->m_eState == gameclasses::SMarbleNodes::enMarbleState::Finished) {
            if (m_pRostrum != nullptr) {
              int l_iStepSince = m_iStep - it->second.m_pPlayer->m_iStateChange;
              irr::f32 l_fFactor = 0.0f;

              if (l_iStepSince > 180) {
                l_fFactor = 1.0f - ((irr::f32)(l_iStepSince - 180)) / 100.0f;
                if (l_fFactor < 0.0f) l_fFactor = 0.0f;
              }
              else if (l_iStepSince > 160) {
                if (!it->second.m_pHUD->isResultParentVisible())
                  it->second.m_pHUD->showResultParent();

                it->second.m_pCamera->setPosition(m_pRostrum->getCameraPosition());
                it->second.m_pCamera->setTarget(m_pRostrum->getAbsolutePosition() - irr::core::vector3df(0.0f, 15.0f, 0.0f));
                it->second.m_pCamera->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));
                l_fFactor = 1.0f;
              }
              else if (l_iStepSince > 60) {
                l_fFactor = ((irr::f32)(l_iStepSince - 60)) / 100.0f;
                if (l_fFactor > 1.0f) {
                  l_fFactor = 1.0f;
                }
              }

              m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFactor), 0, 0, 0), it->second.m_cRect);
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

        if (l_fFade > 0.0f)
          m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFade), 0, 0, 0), m_cScreen);

        if (l_fFade < 0.1f && l_fFade > 0.0f)
          m_pSoundIntf->setMenuFlag(false);

        m_pGlobal->getSoundInterface()->setSoundtrackFade(l_fFade);
      }
      else if (m_eState == enGameState::Racing) {
        if (m_iFinished != -1 && m_iStep - m_iFinished > 1800) { // Wait until the finished soundtrack has ended
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
          l_eRet = enState::Menu;
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

      for (std::vector<scenenodes::STriggerVector>::iterator it = m_vTimerActions.begin(); it != m_vTimerActions.end(); it++) {
        if ((*it).m_vActions.size() > 0) {
          if ((*(*it).m_itAction).m_iStep <= (irr::s32)a_StepNo) {
            switch ((*(*it).m_itAction).m_eAction) {
              case scenenodes::enAction::RotateSceneNode: {
                irr::scene::ISceneNode *p = m_pSmgr->getSceneNodeFromId((*(*it).m_itAction).m_iNodeId);
                if (p != nullptr) {
                  p->setRotation((*(*it).m_itAction).m_vTarget);
                }
                break;
              }

              case scenenodes::enAction::SceneNodeVisibility: {
                irr::scene::ISceneNode *p = m_pSmgr->getSceneNodeFromId((*(*it).m_itAction).m_iNodeId);
                if (p != nullptr) {
                  p->setVisible((*(*it).m_itAction).m_bVisible);
                }
                break;
              }

              default:
                break;
            }

            (*it).m_itAction++;

            if ((*it).m_itAction == (*it).m_vActions.end()) {
              (*it).m_itAction = (*it).m_vActions.begin();
            }

            (*(*it).m_itAction).m_iStep = a_StepNo + (*(*it).m_itAction).m_iTimer;
          }
        }
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
        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
          if ((*it)->m_pController != nullptr)
            (*it)->m_pController->onObjectMoved(a_ObjectId, a_Position);
        }

        m_mMoving[a_ObjectId]->setPosition(a_Position);

        if (a_Rotation.X == a_Rotation.X && a_Rotation.Y == a_Rotation.Y && a_Rotation.Z == a_Rotation.Z)
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

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
          if ((*it)->m_pController != nullptr)
            (*it)->m_pController->onMarbleMoved(a_ObjectId, a_Position, a_LinearVelocity, a_CameraPosition, a_CameraUp);
        }

        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          p->m_pPositional->setPosition(a_Position);
          p->m_pPositional->updateAbsolutePosition();
          p->m_pRotational->updateAbsolutePosition();

          if (a_Rotation.X == a_Rotation.X && a_Rotation.Y == a_Rotation.Y && a_Rotation.Z == a_Rotation.Z)
            p->m_pPositional->setRotation(a_Rotation);

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
    * This function receives messages of type "PlayerAssignMarble"
    * @param a_playerid The ID of the player
    * @param a_marbleid The ID of the marble for the player
    */
    void CGameState::onPlayerassignmarble(irr::s32 a_playerid, irr::s32 a_marbleid) {
      printf("Assign Marble %i to player %i\n", a_marbleid, a_playerid);
      for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
        if ((*it).m_iPlayerId == a_playerid) {
          gameclasses::SMarbleNodes *l_pMarble = m_pGridNode->getMarble(a_marbleid);

          gameclasses::SPlayer* l_pPlayer = new gameclasses::SPlayer(
            (*it).m_iPlayerId,
            (*it).m_sName,
            (*it).m_sTexture,
            (*it).m_sControls,
            (*it).m_eAiHelp,
            l_pMarble,
            (*it).m_eType
          );

          l_pMarble->m_pPlayer = l_pPlayer; 

          printf("Marble %i assigned to player \"%s\".\n", l_pMarble != nullptr ? l_pMarble->m_pPositional->getID() : -2, l_pPlayer->m_sName.c_str());

          m_aMarbles[l_pMarble->m_pPositional->getID() - 10000] = l_pMarble;

          assignViewport(m_fGridAngle, l_pPlayer);

          m_vPlayers .push_back(l_pPlayer);
          m_vPosition.push_back(l_pPlayer);

          threads::CInputQueue  *m_pTheInputQueue  = nullptr;
          threads::COutputQueue *m_pTheOutputQueue = nullptr;

          if (m_pDynamics != nullptr) {
            m_pTheInputQueue  = m_pDynamics->getInputQueue ();
            m_pTheOutputQueue = m_pDynamics->getOutputQueue();
          }
          else if (m_pClient != nullptr) {
            m_pTheInputQueue  = m_pClient->getInputQueue ();
            m_pTheOutputQueue = m_pClient->getOutputQueue();
          }

          if (m_pTheInputQueue != nullptr) {
            controller::CControllerFactory* l_pFactory = new controller::CControllerFactory(m_pTheInputQueue);

            if ((*it).m_eType == data::enPlayerType::Local) {
              l_pPlayer->m_pController = l_pFactory->createController(l_pPlayer->m_pMarble->m_pPositional->getID(), l_pPlayer->m_sController, l_pPlayer->m_eAiHelp, reinterpret_cast<scenenodes::CAiNode*>(m_pAiNode));
            }
            else if ((*it).m_eType == data::enPlayerType::Ai) {
              if (m_pAiThread == nullptr) {
                m_pAiThread = new controller::CAiControlThread(m_pTheOutputQueue, m_pTheInputQueue, reinterpret_cast<scenenodes::CAiNode*>(m_pAiNode));
              }

              m_pAiThread->addAiMarble(l_pPlayer->m_pMarble->m_pPositional->getID(), l_pPlayer->m_sController);
            }

            delete l_pFactory;
          }

          m_pRace->m_mAssignment[l_pMarble->m_pPositional->getID()] = l_pPlayer->m_iPlayer;
        }
      }
    }

    /**
    * This function receives messages of type "RaceSetupDone"
    */
    void CGameState::onRacesetupdone() {
      if (m_pDynamics != nullptr)
        m_pDynamics->startThread();

      if (m_pAiThread != nullptr)
        m_pAiThread->startThread();

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
        it->second.m_pHUD    = new gui::CGameHUD(it->second.m_pPlayer->m_pPlayer, it->second.m_cRect, m_cGameData.m_iLaps, m_pGui, &m_vPosition);
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
          p->m_iStateChange  = m_iStep;
          p->m_iRespawnStart = m_iStep;

          m_pSoundIntf->play2d(L"data/sounds/respawn_start.ogg", m_fSfxVolume, 0.0f);
        }
        else {
          p->m_bCamLink = true;
          p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
          p->m_iStateChange  = -1;
          p->m_iRespawnStart = -1;

          for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
            if ((*it)->m_pController != nullptr)
              (*it)->m_pController->onMarbleRespawn(a_MarbleId);
          }
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
            
            m_pSoundIntf->play2d(L"data/sounds/respawn.ogg", m_fSfxVolume, 0.0f);
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
      if (a_MarbleId >= 10000 && a_MarbleId < 10016) {
        irr::s32 l_iIndex = a_MarbleId - 10000;
        gameclasses::SMarbleNodes* p = m_aMarbles[l_iIndex];

        if (p != nullptr) {
          if (a_State == 1) {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Stunned;
            p->m_iStateChange = m_iStep;

            if (p->m_pViewport != nullptr)
              m_pSoundIntf->play2d(L"data/sounds/stunned.ogg", m_fSfxVolume, 0.0f);
          }
          else {
            p->m_eState = gameclasses::SMarbleNodes::enMarbleState::Rolling;
            p->m_iStateChange = -1;

            if (p->m_pViewport != nullptr)
              m_pSoundIntf->play2d(L"data/sounds/stunned.ogg", 0.0f, 0.0f);
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
          p->m_iStateChange = m_iStep;
          for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
            if (it->second.m_pMarble != nullptr && it->second.m_pMarble->getID() == a_MarbleId) {
              m_pSoundIntf->play2d(L"data/sounds/gameover.ogg", m_fSfxVolume, 0.0f);
            }
          }
        }

#ifdef _TOUCH_CONTROL
        if (m_pTouchControl != nullptr)
          for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
            if (it->second.m_pMarble->getID() == a_MarbleId) {
              m_pTouchControl->setVisible(false);
              break;
            }
          }
#endif
      }
    }

    /**
     * This function receives messages of type "RaceFinished"
     * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
     */
    void CGameState::onRacefinished(irr::u8 a_Cancelled) {
      if (m_iFinished == -1) {
        m_iFinished = a_Cancelled ? m_iStep - 1500 : m_iStep;
        m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStFinish);
      }
    }

    /**
     * This function receives messages of type "Checkpoint"
     * @param a_MarbleId ID of the marble
     * @param a_Checkpoint The checkpoint ID the player has passed
     */
    void CGameState::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) {
      int l_iId = a_MarbleId - 10000;

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr) {
        // printf("onCheckpoint (GameState): Marble %i, Checkpoint %i\n", a_MarbleId, a_Checkpoint);
        gfx::SViewPort* l_pViewport = m_aMarbles[l_iId]->m_pViewport;
        if (l_pViewport != nullptr) {
          m_pSoundIntf->play2d(L"data/sounds/checkpoint.ogg", m_fSfxVolume, 0.0f);

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
      int l_iId = a_MarbleId - 10000;
      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr && m_aMarbles[l_iId]->m_pViewport != nullptr) {
        m_pSoundIntf->play2d(L"data/sounds/lap.ogg", m_fSfxVolume, 0.0f);
      }
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
     * This function receives messages of type "PauseChanged"
     * @param a_Paused The current paused state
     */
    void CGameState::onPausechanged(bool a_Paused) {
      m_bPaused = a_Paused;
    }

    /**
    * This function receives messages of type "ServerDisconnect"
    */
    void CGameState::onServerdisconnect() {
      m_iFinished = m_iStep;
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
        m_aMarbles[l_iIndex]->m_pPlayer->m_iPosition      = a_Position;
        m_aMarbles[l_iIndex]->m_pPlayer->m_iLastPosUpdate = m_iStep;
        m_aMarbles[l_iIndex]->m_pPlayer->m_iDiffAhead     = a_DeficitAhead;
        m_aMarbles[l_iIndex]->m_pPlayer->m_iDiffLeader    = a_DeficitLeader;

        std::sort(m_vPosition.begin(), m_vPosition.end(), [](gameclasses::SPlayer *p1, gameclasses::SPlayer *p2) {
          if (p1->m_iPosition != p2->m_iPosition)
            return p1->m_iPosition < p2->m_iPosition;
          else
            return p1->m_iLastPosUpdate > p2->m_iLastPosUpdate;
        });

        for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++) {
          if (it->second.m_pHUD != nullptr) {
            it->second.m_pHUD->updateRanking();
          }
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
        m_aMarbles[l_iIndex]->m_pPlayer->m_bWithdrawn = true;
      }

      for (std::map<int, gfx::SViewPort>::iterator it = m_mViewports.begin(); it != m_mViewports.end(); it++)
        if (it->second.m_pHUD != nullptr)
          it->second.m_pHUD->updateRanking();
    }


#ifdef _OPENGL_ES
    /**
    * Adjust the materials of the node to get proper lighting when using
    * with OpenGL-ES on the raspberry PI
    * @param a_pNode the node to adjust
    */
    void CGameState::adjustNodeMaterials(irr::scene::ISceneNode* a_pNode) {
      for (irr::u32 i = 0; i < a_pNode->getMaterialCount(); i++) {
        a_pNode->getMaterial(i).Lighting = false;
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
        adjustNodeMaterials(*it);
    }
#endif
  }
}