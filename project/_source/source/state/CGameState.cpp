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
#include <scenenodes/CMyCameraAnimator.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CCheckpointNode.h>
#include <shader/CShaderHandlerBase.h>
#include <scenenodes/CSkyBoxFix.h>
#include <scenenodes/CWorldNode.h>
#include <sound/ISoundInterface.h>
#include <shader/CMyShaderNone.h>
#include <sound/CSoundEnums.h>
#include <data/CDataStructs.h>
#include <state/CGameState.h>
#include <CGlobal.h>
#include <string>
#include <map>

#ifdef _TOUCH_CONTROL
#include <gui/CGuiTouchControl.h>
#endif


namespace dustbin {
  namespace state {
    /**
    * @class SGameViewports
    * @author Christian Keimel
    * This struct maps the number of local players
    * to the viewport distribution
    */
    struct SGameViewports {
      /**
      * Enumeration for the type of viewport
      */
      enum class enType {
        Player,   /**< A viewport for a player */
        Racedata, /**< A viewport for the race data, i.e. ranking, racetime, leader... */
        Logo      /**< A viewport for a logo, just to fill the empty space */
      };

      /**
      * @class SViewportDef
      * @author Christian Keimel
      * This struct holds information about how a viewport looks like
      */
      struct SViewportDef {
        int    m_iColumn,   /**< The column of the viewport*/
               m_iRow;      /**< The row of the viewport*/
        enType m_eType;     /**< The type of viewport */

        SViewportDef() : m_iColumn(0), m_iRow(0), m_eType(enType::Player) {
        }

        SViewportDef(int a_iColumn, int a_iRow, enType a_eType) : m_iColumn(a_iColumn), m_iRow(a_iRow), m_eType(a_eType) {
        }
      };

      /**
      * @class SViewportDistribution
      * @author Christian Keimel
      * This struct is responsible for distributing the viewports
      * across the window depending on the number of players
      */
      struct SViewportDistribution {
        int m_iColumns,   /**< The number of columns for this distribution */
            m_iRows;      /**< The number of rows for this distribution */

        std::vector<SViewportDef> m_vViewports;  /**< The actual viewports for the players */

        SViewportDistribution() : m_iColumns(0), m_iRows(0) {
        }

        SViewportDistribution(int a_iColumns, int a_iRows) : m_iColumns(a_iColumns), m_iRows(a_iRows) {
        }
      };

      /**
      * This map hods the data for the distribution per number of local players
      * key == number of local players, value == actual distribution
      */
      std::map<int, SViewportDistribution> m_mDistribution;
    };


    CGameState::CGameState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) :
      IState(a_pDevice, a_pGlobal),
      m_eState       (enGameState::Countdown),
      m_pInputQueue  (nullptr),
      m_pOutputQueue (nullptr),
      m_iStep        (-1),
      m_iFinished    (-1),
      m_iFadeOut     (-1),
      m_fSfxVolume   (1.0f),
      m_bPaused      (false),
      m_pSoundIntf   (nullptr),
      m_pDynamics    (nullptr),
      m_pShader      (nullptr)
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
    }

    /**
     * This method is called when the state is activated
     */
    void CGameState::activate() {
      m_iStep      = 0;
      m_iFadeOut   = -1;
      m_iFinished  = -1;
      m_eState     = enGameState::Countdown;
      m_bPaused    = false;
      m_fSfxVolume = m_pGlobal->getSettingData().m_fSfxGame;

      m_pGui->clear();

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      // Load the track, and don't forget to run the skybox fix beforehands
      std::string l_sTrack = "data/levels/" + m_pGlobal->getGlobal("track") + "/track.xml";

      if (m_pFs->existFile(l_sTrack.c_str())) {
        scenenodes::CSkyBoxFix* l_pFix = new scenenodes::CSkyBoxFix(m_pDrv, m_pSmgr, m_pFs, l_sTrack.c_str());
        m_pSmgr->loadScene(l_sTrack.c_str());
        l_pFix->hideOriginalSkybox(m_pSmgr->getRootSceneNode());
        delete l_pFix;

        m_pSmgr->addCameraSceneNode(nullptr, irr::core::vector3df(250, 250.0f, 250.0f));
      }
      else {
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
        // throw std::exception();
      }

      scenenodes::CStartingGridSceneNode* l_pGrid = reinterpret_cast<scenenodes::CStartingGridSceneNode*>(l_pNode);

      if (l_pGrid == nullptr) {
        printf("Grid node not found.\n");
        return;
      }

      data::SRacePlayers l_cPlayers;
      std::string l_sPlayers = m_pGlobal->getGlobal("raceplayers");
      l_cPlayers.deserialize(l_sPlayers);

      // First we sort the player vector by the grid position as the first step will be marble assignment ..
      std::sort(l_cPlayers.m_vPlayers.begin(), l_cPlayers.m_vPlayers.end(), [](const data::SPlayerData &a_cPlayer1, const data::SPlayerData &a_cPlayer2) {
        return a_cPlayer1.m_iGridPos < a_cPlayer2.m_iGridPos;
      });

      SGameViewports l_cViewports;

      {
        SGameViewports::SViewportDistribution l_cOnePlayer = SGameViewports::SViewportDistribution(1, 1);
        l_cOnePlayer.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
        l_cViewports.m_mDistribution[1] = l_cOnePlayer;

        SGameViewports::SViewportDistribution l_cTwoPlayers = SGameViewports::SViewportDistribution(1, 2);
        l_cTwoPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
        l_cTwoPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
        l_cViewports.m_mDistribution[2] = l_cTwoPlayers;

        SGameViewports::SViewportDistribution l_cThreePlayers = SGameViewports::SViewportDistribution(2, 2);
        l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
        l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
        l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
        l_cThreePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
        l_cViewports.m_mDistribution[3] = l_cThreePlayers;

        SGameViewports::SViewportDistribution l_cFourPlayers = SGameViewports::SViewportDistribution(2, 2);
        l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
        l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
        l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
        l_cFourPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player));
        l_cViewports.m_mDistribution[4] = l_cFourPlayers;

        SGameViewports::SViewportDistribution l_cFivePlayers = SGameViewports::SViewportDistribution(2, 3);
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player  ));
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player  ));
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player  ));
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player  ));
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Racedata));
        l_cFivePlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player  ));
        l_cViewports.m_mDistribution[5] = l_cFivePlayers;

        SGameViewports::SViewportDistribution l_cSixPlayers = SGameViewports::SViewportDistribution(2, 3);
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 0, SGameViewports::enType::Player));
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 0, SGameViewports::enType::Player));
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 0, SGameViewports::enType::Player));
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(0, 1, SGameViewports::enType::Player));
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(1, 1, SGameViewports::enType::Player));
        l_cSixPlayers.m_vViewports.push_back(SGameViewports::SViewportDef(2, 1, SGameViewports::enType::Player));
        l_cViewports.m_mDistribution[6] = l_cSixPlayers;

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
        l_cViewports.m_mDistribution[7] = l_cSevenPlayers;

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
        l_cViewports.m_mDistribution[8] = l_cEightPlayers;
      }

      int l_iNumOfViewports = 0;  // The number of necessary viewports, aka local players

      printf("******** Marble assignment:\n");
      // .. fill the player vector and assign the marbles to the players (depending on the grid positions) ...
      for (size_t i = 0; i < l_cPlayers.m_vPlayers.size(); i++) {
        gameclasses::SMarbleNodes *l_pMarble = l_pGrid->getNextMarble();
        gameclasses::SPlayer* l_pPlayer = new gameclasses::SPlayer(
          l_cPlayers.m_vPlayers[i].m_iPlayerId,
          l_cPlayers.m_vPlayers[i].m_sName,
          l_cPlayers.m_vPlayers[i].m_sTexture,
          l_cPlayers.m_vPlayers[i].m_sControls,
          l_pMarble
        );

        printf("Marble %i assigned to player \"%s\".\n", l_pMarble != nullptr ? l_pMarble->m_pPositional->getID() : -2, l_pPlayer->m_sName.c_str());

        m_aMarbles[l_pMarble->m_pPositional->getID() - 10000] = l_pMarble;

        if (l_cPlayers.m_vPlayers[i].m_eType == data::SPlayerData::enPlayerType::Local)
          l_iNumOfViewports++;

        m_vPlayers.push_back(l_pPlayer);
      }

      // .. next we sort the players by the player IDs to give them the correct viewports
      std::sort(l_cPlayers.m_vPlayers.begin(), l_cPlayers.m_vPlayers.end(), [](const data::SPlayerData &a_cPlayer1, const data::SPlayerData &a_cPlayer2) {
        return a_cPlayer1.m_iPlayerId < a_cPlayer2.m_iPlayerId;
      });

      l_pGrid->removeUnusedMarbles();
      fillMovingMap(findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode()));

      irr::f32 l_fAngle = l_pGrid->getAngle();

      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 5.0f, 7.5f);
      l_vOffset.rotateXZBy(l_fAngle);

      if (l_cViewports.m_mDistribution.find(l_iNumOfViewports) == l_cViewports.m_mDistribution.end()) {
        // ToDo: Error message
      }

      irr::core::dimension2di l_cViewportSize = m_cScreen.getSize();

      if (l_iNumOfViewports > 0) {
        l_cViewportSize.Width  /= l_cViewports.m_mDistribution[l_iNumOfViewports].m_iColumns;
        l_cViewportSize.Height /= l_cViewports.m_mDistribution[l_iNumOfViewports].m_iRows;

        if (m_pDevice->getCursorControl() != nullptr)
          m_pDevice->getCursorControl()->setVisible(false);
      }

      printf("******** Viewport Assignment:\n");
      int l_iViewport = 0;
      for (size_t i = 0; i < l_cPlayers.m_vPlayers.size(); i++) {
        gameclasses::SPlayer *l_pPlayer = nullptr;

        if (l_cPlayers.m_vPlayers[i].m_eType == data::SPlayerData::enPlayerType::Local) {
          for (std::vector<gameclasses::SPlayer*>::iterator l_itPlayer = m_vPlayers.begin(); l_itPlayer != m_vPlayers.end(); l_itPlayer++) {
            if ((*l_itPlayer)->m_iPlayer == l_cPlayers.m_vPlayers[i].m_iPlayerId) {
              l_pPlayer = *l_itPlayer;
              break;
            }
          }

          if (l_pPlayer != nullptr && l_pPlayer->m_pMarble->m_pPositional) {
            std::vector<SGameViewports::SViewportDef>::iterator it = l_cViewports.m_mDistribution[l_iNumOfViewports].m_vViewports.begin();

            while (true) {
              if ((*it).m_eType == SGameViewports::enType::Player) {
                break;
              }
              it++;
            }

            if (it != l_cViewports.m_mDistribution[l_iNumOfViewports].m_vViewports.end()) {
              printf("Viewport %i / %i assigned to player \"%s\".\n", l_iViewport++, l_iNumOfViewports, l_cPlayers.m_vPlayers[i].m_sName.c_str());

              irr::core::recti l_cRect = irr::core::recti(
                 (*it).m_iColumn      * l_cViewportSize.Width,
                 (*it).m_iRow         * l_cViewportSize.Height,
                ((*it).m_iColumn + 1) * l_cViewportSize.Width,
                ((*it).m_iRow    + 1) * l_cViewportSize.Height
              );

              l_cViewports.m_mDistribution[l_iNumOfViewports].m_vViewports.erase(it);

              irr::scene::ICameraSceneNode* l_pCam = m_pSmgr->addCameraSceneNode(m_pSmgr->getRootSceneNode(), l_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition() + l_vOffset, l_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition());
              l_pCam->setAspectRatio((((irr::f32)l_cRect.LowerRightCorner.X) - ((irr::f32)l_cRect.UpperLeftCorner.X)) / (((irr::f32)l_cRect.LowerRightCorner.Y) - ((irr::f32)l_cRect.UpperLeftCorner.Y)));
              l_pCam->updateAbsolutePosition();

              gfx::SViewPort l_cViewport = gfx::SViewPort(l_cRect, l_cPlayers.m_vPlayers[i].m_iPlayerId, l_pPlayer->m_pMarble->m_pPositional, l_pCam);

              for (std::map<irr::s32, scenenodes::CCheckpointNode*>::iterator it = m_mCheckpoints.begin(); it != m_mCheckpoints.end(); it++) {
                if (it->second->m_bFirstInLap && it->second->getParent()->getType() == irr::scene::ESNT_MESH) {
                  l_cViewport.m_vNextCheckpoints.push_back(reinterpret_cast<irr::scene::IMeshSceneNode *>(it->second->getParent()));
                }
              }

              l_cViewport.m_pPlayer = l_pPlayer->m_pMarble;
              m_mViewports[l_cPlayers.m_vPlayers[i].m_iPlayerId] = l_cViewport;
              l_pPlayer->m_pMarble->m_pViewport = &m_mViewports[l_cPlayers.m_vPlayers[i].m_iPlayerId];
            }
            else {
              // ToDo: error message "not enough viewport available"
            }
          }
        }
      }

      if (m_mViewports.size() == 0) {
        // No viewport ==> view track, create a viewport
        scenenodes::CMyCameraAnimator *l_pAnimator = new scenenodes::CMyCameraAnimator(m_pDevice);
        irr::scene::ICameraSceneNode* l_pCam = m_pSmgr->addCameraSceneNode(m_pSmgr->getRootSceneNode(), l_vOffset);
        l_pCam->addAnimator(l_pAnimator);
        l_pAnimator->drop();
        gfx::SViewPort l_cViewport = gfx::SViewPort(irr::core::recti(irr::core::position2di(0, 0), m_pDrv->getScreenSize()), 0, nullptr, l_pCam);
        m_mViewports[0] = l_cViewport;
      }

      printf("******** Ready.\n");

      data::SSettings l_cSettings= m_pGlobal->getSettingData();

      irr::u32 l_iAmbient = 196;

      switch (l_cSettings.m_iAmbient) {
        case 0: l_iAmbient = 32; break;
        case 1: l_iAmbient = 64; break;
        case 2: l_iAmbient = 96; break;
        case 3: l_iAmbient = 128; break;
        case 4: l_iAmbient = 160; break;
      }

      irr::core::dimension2du l_cDim = irr::core::dimension2du(m_mViewports.begin()->second.m_cRect.getSize().Width, m_mViewports.begin()->second.m_cRect.getSize().Height);

      switch (l_cSettings.m_iShadows) {
        case 3:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 8096, l_cSettings.m_iAmbient);
          }
          break;
#endif
          
        case 2:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 4096, l_cSettings.m_iAmbient);
          }
          break;
#endif

        case 1:
#ifndef NO_XEFFECT
          if (m_mViewports.size() == 1) {
            m_pShader = new shader::CShaderHandlerXEffect(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, l_iAmbient);
          }
          else {
            m_pShader = new shader::CShaderHandleXEffectSplitscreen(m_pGlobal->getIrrlichtDevice(), l_cDim, 2048, l_cSettings.m_iAmbient);
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
      
      l_pNode = findSceneNodeByType((irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId, m_pSmgr->getRootSceneNode());
      
      if (l_pNode != nullptr) {
        m_pDynamics = new gameclasses::CDynamicThread(reinterpret_cast<scenenodes::CWorldNode*>(l_pNode), m_vPlayers, std::atoi(m_pGlobal->getSetting("laps").c_str()), m_vTimerActions, m_vMarbleCounters);

        if (m_pInputQueue  == nullptr) m_pInputQueue  = new threads::CInputQueue ();
        if (m_pOutputQueue == nullptr) m_pOutputQueue = new threads::COutputQueue();

        m_pDynamics->getOutputQueue()->addListener(m_pInputQueue);
        m_pOutputQueue->addListener(m_pDynamics->getInputQueue());

        controller::CControllerFactory* l_pFactory = new controller::CControllerFactory(m_pDynamics->getInputQueue());

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
          (*it)->m_pController = l_pFactory->createController((*it)->m_pMarble->m_pPositional->getID(), (*it)->m_sController);
        }

        delete l_pFactory;

        m_pDynamics->startThread();
      }
      else {
        m_pGlobal->setGlobal("ERROR_MESSAGE", "No world node found.");
        m_pGlobal->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
        // throw std::exception();
      }

#ifdef _TOUCH_CONTROL
      if (m_pGlobal->getSettingData().m_bTouchControl) {
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

#ifdef _TOUCH_CONTROL
      m_pTouchControl = nullptr;
#endif

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
      }

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
      if (m_pTouchControl != nullptr) {
        irr::s8 l_iCtrlX    = 0,
                l_iCtrlY    = 0;
        bool    l_bBrake    = false,
                l_bRespawn  = false,
                l_bRearView = false;

        m_pTouchControl->getControl(l_iCtrlX, l_iCtrlY, l_bBrake, l_bRespawn, l_bRearView);

        messages::CMarbleControl l_cMessage = messages::CMarbleControl((*m_vPlayers.begin())->m_pMarble->m_pPositional->getID(), l_iCtrlX, l_iCtrlY, l_bBrake, l_bRearView, l_bRespawn);
        m_pDynamics->getInputQueue()->postMessage(&l_cMessage);
      }
#endif

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
      printf("onPlayerStunned: %i, %i\n", a_MarbleId, a_State);
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
          m_pSoundIntf->play2d(L"data/sounds/gameover.ogg", m_fSfxVolume, 0.0f);
        }
      }
    }

    /**
     * This function receives messages of type "RaceFinished"
     * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
     */
    void CGameState::onRacefinished(irr::u8 a_Cancelled) {
      m_iFinished = a_Cancelled ? m_iStep - 550 : m_iStep;
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
        printf("onCheckpoint (GameState): Marble %i, Checkpoint %i\n", a_MarbleId, a_Checkpoint);
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
      printf("onLapstart (GameState): Marble %i, Lap %i\n", a_MarbleId, a_LapNo);
      int l_iId = a_MarbleId - 10000;
      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr && m_aMarbles[l_iId]->m_pViewport != nullptr) {
        m_pSoundIntf->play2d(L"data/sounds/lap.ogg", m_fSfxVolume, 0.0f);
      }
    }

    /**
     * This function receives messages of type "PauseChanged"
     * @param a_Paused The current paused state
     */
    void CGameState::onPausechanged(bool a_Paused) {
      m_bPaused = a_Paused;
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