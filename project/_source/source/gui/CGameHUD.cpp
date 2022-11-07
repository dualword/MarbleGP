// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <scenenodes/CMyTextSceneNode.h>
#include <scenenodes/CAiPathSceneNode.h>
#include <gui/hud_items/CHudSpeedBar.h>
#include <gui/hud_items/CHudSteering.h>
#include <gui/hud_items/CHudLapTimes.h>
#include <gui/hud_items/CHudAiHelp.h>
#include <controller/IControllerAI.h>
#include <gui/hud_items/CHudBanner.h>
#include <scenenodes/CMyBillboard.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gameclasses/SPlayer.h>
#include <gui/CRankingElement.h>
#include <gfx/SViewPort.h>
#include <gui/CGameHUD.h>
#include <CGlobal.h>
#include <algorithm>

namespace dustbin {
  namespace gui {
    /**
    * This function receives messages of type "PlayerRespawn"
    * @param a_MarbleId ID of the marble
    * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
    */
    void CGameHUD::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (m_iGoStep != 0) {
        if (a_MarbleId == m_iMarble) {
          m_bShowSpeed = a_State == 2;
          m_bRespawn   = a_State != 2;

          if (m_pBanner != nullptr) {
            m_pBanner->setState(a_State != 2 ? CHudBanner::enBanners::Respawn : CHudBanner::enBanners::Count);
          }
        }

        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
          if ((*it)->m_iId == a_MarbleId) {
            if (a_State == 1)
              (*it)->m_iState = 2;
            else if (a_State == 2)
              (*it)->m_iState = 0;
            break;
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
    void CGameHUD::onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target) {
      if (m_iGoStep != 0) {
        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
          if ((*it)->m_iId == a_MarbleId) {
            (*it)->m_iState = 3;
            break;
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
    void CGameHUD::onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {
      int l_iId = a_MarbleId - 10000;

      printf("Player %i finished (%i)\n", l_iId, a_MarbleId);

      if (l_iId >= 0 && l_iId < 16)
        m_aFinished[l_iId] = true;

      if (a_MarbleId == m_iMarble) {
        m_bShowSpeed  = false;
        m_bFinished   = true;
        m_bRespawn    = false;
        m_bStunned    = false;

        if (m_pAiNode != nullptr)
          m_pAiNode->setVisible(false);

        if (m_pBanner != nullptr)
          m_pBanner->setState(CHudBanner::enBanners::Finished);
      }
      else {
        for (int i = 0; i < 3; i++) {
          if (m_aHiLight[i].m_iMarbleId == a_MarbleId) {
            m_aHiLight[i].m_bFinished = true;
            m_aHiLight[i].m_bVisible  = false;
          }
        }
      }

      if (m_pLapTimes != nullptr)
        m_pLapTimes->onPlayerFinished(a_RaceTime, a_MarbleId);
    }

    /**
    * This function receives messages of type "PlayerStunned"
    * @param a_MarbleId ID of the marble
    * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
    */
    void CGameHUD::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (m_iGoStep != 0) {
        if (a_MarbleId == m_iMarble) {
          m_bShowSpeed = a_State != 1;
          m_bStunned   = a_State == 1;
          if (m_pBanner != nullptr)
            m_pBanner->setState(a_State == 1 ? CHudBanner::enBanners::Stunned : CHudBanner::enBanners::Count);
        }
      
        for (std::vector<gameclasses::SPlayer*>::iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
          if ((*it)->m_iId == a_MarbleId) {
            if (a_State == 1)
              (*it)->m_iState = 1;
            else
              (*it)->m_iState = 0;
            break;
          }
        }
      }
    }

    /**
    * This function receives messages of type "RaceFinished"
    * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
    */
    void CGameHUD::onRacefinished(irr::u8 a_Cancelled) {
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_Deficit Deficit of the marble on the leader in steps
    */
    void CGameHUD::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
      if (a_MarbleId == m_iMarble) {
        m_iFinished = a_Position - 1;
        if (m_pBanner != nullptr)
          m_pBanner->setPosition(a_Position - 1);
      }
    }

    /**
    * This function receives messages of type "LapStart"
    * @param a_MarbleId ID of the marble
    * @param a_LapNo Number of the started lap
    */
    void CGameHUD::onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) {
      if (m_pLapTimes != nullptr)
        m_pLapTimes->onLapStart(m_iStep, a_MarbleId, a_LapNo);
      
      if (a_MarbleId == m_iMarble) {
        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.m_bVisible = true;

        m_mTextElements[enTextElements::Lap].m_sText = std::to_wstring(a_LapNo) + L" / " + std::to_wstring(m_iLapCnt);
      }
    }

    /**
    * This function receives messages of type "PlayerRostrum"
    * @param a_MarbleId ID of the marble sent to the rostrum
    */
    void CGameHUD::onPlayerrostrum(irr::s32 a_MarbleId) {
      int l_iIndex = a_MarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16) {
        printf("Rostrum: %i (%i)\n", l_iIndex, a_MarbleId);
        m_aRostrum[l_iIndex] = true;
        for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
          if ((*it)->m_iId == a_MarbleId) {
            l_iIndex = (*it)->m_iPosition - 1;
            if (m_aRanking[l_iIndex] != nullptr)
              m_aRanking[l_iIndex]->setVisible(true);

            break;
          }
        }

        if (a_MarbleId == m_iMarble) {
          m_cUpVector = irr::core::vector3df(0.0f, 1.0f, 0.0f);
        }
      }
    }

    /**
    * This function receives messages of type "Countdown"
    * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
    */
    void CGameHUD::onCountdown(irr::u8 a_Tick) {
      if (m_pBanner != nullptr) {
        switch (a_Tick) {
          case 4: m_pBanner->setState(CHudBanner::enBanners::CountdownReady); break;
          case 3: m_pBanner->setState(CHudBanner::enBanners::Countdown3    ); break;
          case 2: m_pBanner->setState(CHudBanner::enBanners::Countdown2    ); break;
          case 1: m_pBanner->setState(CHudBanner::enBanners::Countdown1    ); break;
          case 0: m_pBanner->setState(CHudBanner::enBanners::CountdownGo   ); break;
        }
      }

      if (a_Tick == 1) {
        m_bFadeStart = true;
        m_iFadeStart = m_iStep;
      }
      else if (a_Tick == 0) {
        m_bFadeStart = false;
        m_pRankParent->setVisible(false);

        m_cRankBack = irr::video::SColor(96, 192, 192, 192);

        for (int i = 0; i < 16; i++) {
          if (m_aRanking[i] != nullptr) {
            m_aRanking[i]->setAlpha(1.0f);
            m_aRanking[i]->highlight (false);
            m_aRanking[i]->setVisible(false);
          }
        }

        m_bShowSpeed = true;

        m_iGoStep = m_iStep;
      }
    }


    /**
    * This function receives messages of type "ConfirmWithdraw"
    * @param a_MarbleId ID of the marble
    * @param a_Timeout The number of steps defining the timeout for the withdrawal
    */
    void CGameHUD::onConfirmwithdraw(irr::s32 a_MarbleId, irr::s32 a_Timeout) {
      if (a_MarbleId == m_iMarble) {
        if (m_pBanner != nullptr)
          m_pBanner->showConfirmWithdraw(true);
        m_iWithdraw = m_iStep + a_Timeout;
      }
    }


    /**
    * This function receives messages of type "PauseChanged"
    * @param a_Paused The current paused state
    */
    void CGameHUD::onPausechanged(bool a_Paused) {
      m_bPaused = a_Paused;
      if (m_pRankParent != nullptr && !m_bFinished) {
        m_pRankParent->setVisible(a_Paused);

        for (int i = 0; i < 16; i++) {
          if (m_aRanking[i] != nullptr)
            m_aRanking[i]->setVisible(a_Paused || m_aRostrum[i]);
        }
      }

      if (m_pBanner != nullptr)
        m_pBanner->setState(a_Paused ? CHudBanner::enBanners::Paused : CHudBanner::enBanners::Count);
    }

    /**
    * This function receives messages of type "Checkpoint"
    * @param a_MarbleId ID of the marble
    * @param a_Checkpoint The checkpoint ID the player has passed
    */
    void CGameHUD::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) {
      if (m_pLapTimes != nullptr)
        m_pLapTimes->onCheckpoint(m_iStep, a_MarbleId);
    }

    /**
    * This function receives messages of type "StepMsg"
    * @param a_StepNo The current step number
    */
    void CGameHUD::onStepmsg(irr::u32 a_StepNo) {
      m_iStep = a_StepNo;

      if (m_bFadeStart) {
        irr::f32 l_fFactor = 1.0f - ((irr::f32)a_StepNo - m_iFadeStart) / 120.0f;
        m_cRankBack = irr::video::SColor((irr::u32)(96.0f * l_fFactor), 192, 192, 192);

        for (int i = 0; i < 16; i++) {
          if (m_aRanking[i] != nullptr)
            m_aRanking[i]->setAlpha(l_fFactor);
        }
      }

      if (m_bCountdown && m_iGoStep != 0 && m_iStep > m_iGoStep + 240) {
        if (m_pBanner != nullptr) {
          irr::f32 f = std::min(1.0f, ((irr::f32)(m_iGoStep - m_iStep + 300)) / 60.0f);
          if (f < 0.0f) {
            m_pBanner->setState(CHudBanner::enBanners::Count);
            m_bCountdown = false;
          }
          else m_pBanner->setFade(f);
        }
      }

      if (m_iWithdraw != -1 && m_iWithdraw < m_iStep) {
        if (m_pBanner != nullptr)
          m_pBanner->showConfirmWithdraw(false);

        m_iWithdraw = -1;
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
    void CGameHUD::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      m_mMarblePositions[a_ObjectId] = a_Position + 1.5f * a_CameraUp;

      if (a_ObjectId == m_iMarble) {
        m_fVel      = a_LinearVelocity.getLength();
        m_fSteer    = ((irr::f32)a_ControlX) / 127.0f;
        m_fThrottle = ((irr::f32)a_ControlY) / 127.0f;
        m_bBrake    = a_ControlBrake;
        m_bManRsp   = a_ControlRespawn;

        int l_iIndex = a_ObjectId - 10000;

        if (l_iIndex >= 0 && l_iIndex < 16) {
          if (!m_aRostrum[l_iIndex]) {
            m_cPosition = a_Position;
            m_cUpVector = a_CameraUp;
            m_cUpVector.normalize();
          }
        }
      }
      else {
        if (m_bHightlight) {
          for (int i = 0; i < 3; i++) {
            if (m_aHiLight[i].m_iMarbleId == a_ObjectId) {
              if (m_mMarblePositions.find(m_aHiLight[i].m_iMarbleId) != m_mMarblePositions.end() && m_aHiLight[i].m_pArrow != nullptr && m_aHiLight[i].m_pPosition != nullptr) {
                m_aHiLight[i].m_pArrow->setPosition(a_Position + 3.0f * m_cUpVector);
                
                std::wstring s = std::to_wstring(m_aHiLight[i].m_iPosition);
                
                if (m_aHiLight[i].m_iPosition == 1)
                  s += L"st";
                else if (m_aHiLight[i].m_iPosition == 2)
                  s += L"nd";
                else if (m_aHiLight[i].m_iPosition == 3)
                  s += L"rd";
                else
                  s += L"th";

                m_aHiLight[i].m_pPosition->setText(s.c_str());
                m_aHiLight[i].m_pPosition->setPosition(a_Position + 5.0f * m_cUpVector);
              }
              else {
                m_aHiLight[i].m_pArrow   ->setVisible(false);
                m_aHiLight[i].m_pPosition->setVisible(false);
              }
            }
          }
        }
      }
    }


    irr::core::dimension2du CGameHUD::getDimension(const std::wstring& s, irr::gui::IGUIFont *a_pFont) {
      irr::core::dimension2du l_cDim = a_pFont->getDimension(s.c_str());
      l_cDim.Width  = 5 * l_cDim.Width  / 4;
      l_cDim.Height = 5 * l_cDim.Height / 4;
      return l_cDim;
    }

    CGameHUD::CGameHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti& a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment* a_pGui, std::vector<gameclasses::SPlayer *> *a_vRanking) : 
      IGUIElement(irr::gui::EGUIET_COUNT, a_pGui, a_pGui->getRootGUIElement(), -1, a_cRect),
      m_iMarble       (a_pPlayer->m_pMarble->m_pPositional->getID()),
      m_iLapCnt       (a_iLapCnt),
      m_iPlayers      ((int)a_vRanking->size()),
      m_iCtrlHeight   (0),
      m_fVel          (0.0f),
      m_fThrottle     (0.0f),
      m_fSteer        (0.0f),
      m_bShowSpeed    (false),
      m_bBrake        (false),
      m_bManRsp       (false),
      m_bHightlight   (true),
      m_bShowCtrl     (true),
      m_bShowRanking  (true),
      m_bFadeStart    (false),
      m_bPaused       (false),
      m_bCountdown    (true),
      m_cRect         (a_cRect),
      m_pGui          (a_pGui),
      m_pPlayer       (a_pPlayer),
      m_cDefSize      (irr::core::dimension2du()),
      m_pDefFont      (nullptr),
      m_pTimeFont     (nullptr),
      m_pDrv          (a_pGui->getVideoDriver()),
      m_cScreen       (irr::core::dimension2du()),
      m_cUpVector     (irr::core::vector3df()),
      m_pRankParent   (nullptr),
      m_iGoStep       (0),
      m_iStep         (0),
      m_bRespawn      (false),
      m_bStunned      (false),
      m_bFinished     (false),
      m_iFadeStart    (-1),
      m_iFinished     (-1),
      m_pPosFont      (nullptr),
      m_iWithdraw     (-1),
      m_cRankBack     (irr::video::SColor(96, 192, 192, 192)),
      m_pSmgr         (CGlobal::getInstance()->getSceneManager()),
      m_pColMgr       (nullptr),
      m_vRanking      (a_vRanking),
      m_iLapTimeOffset(0),
      m_bShowLapTimes (false),
      m_pCheckered    (nullptr),
      m_pAiController (nullptr),
      m_pAiHelp       (nullptr),
      m_pSpeedBar     (nullptr),
      m_pSteering     (nullptr),
      m_pBanner       (nullptr),
      m_pLapTimes     (nullptr),
      m_pAiNode       (nullptr)
    {
      CGlobal *l_pGlobal = CGlobal::getInstance();

      m_pColMgr = m_pSmgr->getSceneCollisionManager();
      m_cScreen = m_pDrv->getScreenSize();

      irr::core::dimension2du l_cViewport = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      irr::video::SColor l_cBackground = irr::video::SColor( 128, 192, 192, 192);
      irr::video::SColor l_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

      irr::gui::IGUIFont *l_pTiny    = l_pGlobal->getFont(enFont::Tiny   , l_cViewport);
      irr::gui::IGUIFont *l_pSmall   = l_pGlobal->getFont(enFont::Small  , l_cViewport);
      irr::gui::IGUIFont *l_pRegular = l_pGlobal->getFont(enFont::Regular, l_cViewport);
      irr::gui::IGUIFont *l_pBig     = l_pGlobal->getFont(enFont::Big    , l_cViewport);
      irr::gui::IGUIFont *l_pHuge    = l_pGlobal->getFont(enFont::Huge   , l_cViewport);

      m_pPosFont = l_pHuge;

#ifdef _ANDROID
      m_pTimeFont = l_pTiny;
#else
      m_pTimeFont = l_pSmall;
#endif

      m_pDefFont = l_pSmall;
      m_cDefSize = getDimension(L"+666.6", m_pDefFont);

      m_cLabelSize.Width  = a_cRect.getWidth() / 4;
      m_cLabelSize.Height = l_pRegular->getDimension(L"666").Height;

      irr::core::dimension2du l_cSizeTop = getDimension(L"Lap"    , l_pSmall  );
      irr::core::dimension2du l_cSizeBot = getDimension(L"66 / 66", l_pRegular);

      m_iCtrlHeight = m_cLabelSize.Height / 4;
      if (m_iCtrlHeight < 2) m_iCtrlHeight = 2;

      {
        irr::core::dimension2du d = getDimension(L"Pos", l_pSmall);

        if (d.Width > l_cSizeTop.Width)
          l_cSizeTop.Width = d.Width;
      }

      if (l_cSizeBot.Width > l_cSizeTop.Width)
        l_cSizeTop.Width = l_cSizeBot.Width;
      else
        l_cSizeBot.Width = l_cSizeTop.Width;

      std::wstring l_sName = a_pPlayer->m_sWName;

      switch (a_pPlayer->m_eAiHelp) {
        case data::SPlayerData::enAiHelp::Off    : break;
        case data::SPlayerData::enAiHelp::Display: break;
        case data::SPlayerData::enAiHelp::Low    : l_sName += L" (AI help \"Low\")"   ; break;
        case data::SPlayerData::enAiHelp::Medium : l_sName += L" (AI help \"Medium\")"; break;
        case data::SPlayerData::enAiHelp::High   : l_sName += L" (AI help \"High\")"  ; break;
        case data::SPlayerData::enAiHelp::BotMgp : l_sName += L" (MarbleGP Bot)"      ; break;
        case data::SPlayerData::enAiHelp::BotMb2 : l_sName += L" (Marble2 Bot)"       ; break;
        case data::SPlayerData::enAiHelp::BotMb3 : l_sName += L" (Marble3 Bot)"       ; break;
      }

      irr::core::dimension2du l_cSizeName = getDimension(l_sName, l_pRegular);

      irr::core::position2di l_cLapHead = irr::core::position2di(a_cRect.UpperLeftCorner .X                   , a_cRect.UpperLeftCorner.Y                    );
      irr::core::position2di l_cLap     = irr::core::position2di(a_cRect.UpperLeftCorner .X                   , a_cRect.UpperLeftCorner.Y + l_cSizeTop.Height);
      irr::core::position2di l_cPosHead = irr::core::position2di(a_cRect.LowerRightCorner.X - l_cSizeTop.Width, a_cRect.UpperLeftCorner.Y                    );
      irr::core::position2di l_cPos     = irr::core::position2di(a_cRect.LowerRightCorner.X - l_cSizeTop.Width, a_cRect.UpperLeftCorner.Y + l_cSizeTop.Height);


      irr::core::position2di l_pName  = irr::core::position2di(a_cRect.getCenter().X - l_cSizeName.Width / 2, a_cRect.UpperLeftCorner.Y);

      m_mTextElements[enTextElements::LapHead] = STextElement(irr::core::recti(l_cPosHead, l_cSizeTop ), L"Lap"    , l_pSmall  , l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Lap    ] = STextElement(irr::core::recti(l_cPos    , l_cSizeBot ), L"66 / 66", l_pRegular, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::PosHead] = STextElement(irr::core::recti(l_cLapHead, l_cSizeTop ), L"Pos"    , l_pSmall  , l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Pos    ] = STextElement(irr::core::recti(l_cLap    , l_cSizeBot ), L"0"      , l_pRegular, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Name   ] = STextElement(irr::core::recti(l_pName   , l_cSizeName), l_sName   , l_pRegular, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());

      m_mTextElements[enTextElements::LapHead].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Lap    ].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::PosHead].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Pos    ].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Name   ].m_eAlignH = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::LapHead].m_eAlignV = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Lap    ].m_eAlignV = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::PosHead].m_eAlignV = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Pos    ].m_eAlignV = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Name   ].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::LapHead].m_bVisible = false;
      m_mTextElements[enTextElements::Lap    ].m_bVisible = false;
      m_mTextElements[enTextElements::PosHead].m_bVisible = false;
      m_mTextElements[enTextElements::Pos    ].m_bVisible = false;

      for (int i = 0; i < 16; i++) {
        m_aRanking[i] = nullptr;
        m_aRostrum[i] = false;
      }

      m_pRankParent = a_pGui->addTab(a_cRect, a_pGui->getRootGUIElement());
      m_pRankParent->setVisible(true);

      irr::core::dimension2du l_cRankSize = getDimension(L"ThisStringReallyIsEnough", 
#ifdef _ANDROID
        l_pRegular
#else
        l_pBig
#endif
      );
      irr::core::position2di  l_cRankPos  = irr::core::position2di(a_cRect.getWidth() / 2, a_cRect.getHeight() - 2 * l_cRankSize.Height);

      l_cRankSize.Height += l_cRankSize.Height / 8;

      l_cRankPos.Y -= 16 * 3 * l_cRankSize.Height / 4;

      int l_iOffsetY = 0;

      for (int i = 0; i < m_iPlayers; i++) {
        irr::core::position2di l_cThisPos = irr::core::position2di(
          i < m_iPlayers / 2 ? l_cRankPos.X - l_cRankSize.Width - 2 * l_pGlobal->getRasterSize() : l_cRankPos.X + 2 * l_pGlobal->getRasterSize(), 
          l_cRankPos.Y + l_iOffsetY
        );

        m_aRanking[i] = new gui::CRankingElement(
          i + 1, 
          irr::core::recti(l_cThisPos, l_cRankSize),
          i == 0 ? irr::video::SColor(224, 255, 215, 0) : i == 1 ? irr::video::SColor(224, 192, 192, 192) : i == 2 ? irr::video::SColor(224, 191, 137, 112) : irr::video::SColor(224, 128, 128, 232),
#ifdef _ANDROID
          l_pSmall, 
#else
          l_pRegular,
#endif
          m_pRankParent,
          a_pGui
        );

        if ((*m_vRanking)[i]->m_iId == m_iMarble)
          m_aRanking[i]->highlight(true);

        m_aRanking[i]->setData((*m_vRanking)[i]->m_sWName, 0, (*m_vRanking)[i]->m_bWithdrawn);
        m_aRanking[i]->drop();

        if (i == (m_iPlayers / 2) - 1)
          l_iOffsetY = 0;
        else
          l_iOffsetY += 3 * l_cRankSize.Height / 2;
      }

      for (int i = 0; i < 3; i++) {
        m_aHiLight[i].m_pArrow = new scenenodes::CMyBillboard(m_pSmgr->getRootSceneNode(), m_pSmgr, -1, irr::core::vector3df(0.0f), irr::core::dimension2df(2.5f, 2.5f));
        m_aHiLight[i].m_pArrow->setMaterialFlag(irr::video::EMF_LIGHTING, false);
        m_aHiLight[i].m_pArrow->setMaterialType(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL);
        m_aHiLight[i].m_pArrow->setMaterialTexture(0, m_pDrv->getTexture("data/images/arrow_ahead.png"));
        m_aHiLight[i].m_pArrow->setVisible(false);
        m_aHiLight[i].m_pArrow->drop();

        m_aHiLight[i].m_pPosition = new scenenodes::CMyBillboardText(
          m_pSmgr->getRootSceneNode(),
          m_pSmgr,
          -1,
          l_pGlobal->getFont(enFont::Big, l_pGlobal->getVideoDriver()->getScreenSize()), 
          L"16th",
          irr::core::vector3df(0.0f),
          irr::core::dimension2df(3.0f, 3.0f),
          irr::video::SColor(0xFF, 0, 0, 0),
          irr::video::SColor(0xFF, 0, 0, 0)
        );

        m_aHiLight[i].m_pPosition->drop();
      }

      for (int i = 0; i < 16; i++)
        m_aFinished[i] = false;

      m_cLapNoDim = m_pTimeFont->getDimension(L" Lap 66: ");
      m_cLapNoDim.Width  = 5 * m_cLapNoDim.Width  / 4;
      m_cLapNoDim.Height = 5 * m_cLapNoDim.Height / 4;

      m_cPosNameDim = m_pTimeFont->getDimension(L"66: XXXXX");
      m_cPosNameDim.Width  = 10 * m_cPosNameDim.Width  / 9;
      m_cPosNameDim.Height =  5 * m_cPosNameDim.Height / 4;

      m_iLapTimeOffset = 5 * m_cPosNameDim.Height / 4;

      m_cStartNr = irr::core::dimension2du(m_cPosNameDim.Height, m_cPosNameDim.Height);

      m_pCheckered = m_pDrv->getTexture("data/images/checkered.png");
      m_cCheckered = m_pCheckered->getOriginalSize();

      for (std::vector<gameclasses::SPlayer *>::iterator l_itRank =  a_vRanking->begin(); l_itRank != a_vRanking->end(); l_itRank++) {
        (*l_itRank)->m_iState = 0;
      }

      m_pSpeedBar = new CHudSpeedBar(m_pDrv, l_pRegular, a_cRect);
      m_pSteering = new CHudSteering(m_pDrv, a_cRect);
      m_pBanner   = new CHudBanner  (m_pDrv, m_pGui, l_pBig, l_pHuge, a_cRect);
      m_pLapTimes = new CHudLapTimes(m_pDrv, m_iMarble, irr::core::vector2di(a_cRect.LowerRightCorner.X, a_cRect.UpperLeftCorner.Y), 
#ifdef _ANDROID
        l_pTiny
#else
        l_pSmall
#endif
      );

      if (m_pPlayer->m_eAiHelp != data::SPlayerData::enAiHelp::Off && !m_pPlayer->isBot()) {
        m_pAiHelp = new CHudAiHelp(m_pDrv, m_pPlayer->m_pMarble->m_pViewport->m_cRect);
        m_pAiNode = new scenenodes::CAiPathSceneNode(l_pGlobal->getSceneManager()->getRootSceneNode(), l_pGlobal->getSceneManager(), -1, irr::core::vector3df(), irr::core::dimension2df(1.5f, 1.5f));
        m_pAiNode->drop();
      }
    }

    CGameHUD::~CGameHUD() {
      if (m_pAiController != nullptr)
        m_pAiController->setHUD(nullptr);

      if (m_pAiHelp != nullptr) {
        delete m_pAiHelp;
        m_pAiHelp = nullptr;
      }

      if (m_pSpeedBar != nullptr) {
        delete m_pSpeedBar;
        m_pSpeedBar = nullptr;
      }

      if (m_pSteering != nullptr) {
        delete m_pSteering;
        m_pSteering = nullptr;
      }

      if (m_pBanner != nullptr) {
        delete m_pBanner;
        m_pBanner = nullptr;
      }
    }

    void CGameHUD::draw() {

    if (m_pColMgr != nullptr) {
      // For Android we move these elements
      // a little less down so that it fits
      // the screen with it's larger fonts
      irr::f32 l_fFactor =
#ifdef _ANDROID
        1.5f;
#else
        3.0f;
#endif
      ;

      irr::core::vector2di l_cSpeed = m_pColMgr->getScreenCoordinatesFrom3DPosition(m_pPlayer->m_pMarble->m_pPositional->getAbsolutePosition() - l_fFactor * m_cUpVector, m_pPlayer->m_pMarble->m_pViewport->m_pCamera);

      if (m_pAiHelp != nullptr && m_bShowSpeed)
        m_pAiHelp->render(irr::core::position2di(m_cRect.getCenter().X, l_cSpeed.Y - m_cLabelSize.Height), m_pPlayer->m_pMarble->m_pViewport->m_cRect);

      l_cSpeed.X = m_cRect.UpperLeftCorner.X + m_cRect.getWidth () * l_cSpeed.X / m_cScreen.Width;
      l_cSpeed.Y = m_cRect.UpperLeftCorner.Y + m_cRect.getHeight() * l_cSpeed.Y / m_cScreen.Height;

      irr::core::recti l_cTotal = irr::core::recti(l_cSpeed - irr::core::vector2di(m_cLabelSize.Width, m_cLabelSize.Height) / 2, m_cLabelSize);

      l_cSpeed.Y = l_cTotal.UpperLeftCorner.Y;

      if (m_pSpeedBar != nullptr && m_bShowSpeed)
        l_cSpeed.Y += m_pSpeedBar->render(m_fVel, l_cSpeed, m_cRect);

      if (m_pSteering != nullptr && m_bShowCtrl && m_bShowSpeed)
        l_cSpeed.Y += m_pSteering->render(l_cSpeed, m_fSteer, m_fThrottle, m_bBrake, m_bManRsp, m_cRect);

      if (m_pBanner != nullptr)
        m_pBanner->render(m_cRect);

      if (m_bShowSpeed) {
        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.render();
          irr::core::dimension2di l_cPosSize = m_mTextElements[enTextElements::PosHead].m_cThisRect.getSize();

          m_mTextElements[enTextElements::PosHead].setPosition(l_cTotal.UpperLeftCorner - irr::core::position2di(10 * l_cPosSize.Width / 9, 0));
          m_mTextElements[enTextElements::Pos    ].setPosition(m_mTextElements[enTextElements::PosHead].m_cThisRect.UpperLeftCorner + irr::core::position2di(0, l_cPosSize.Height));

          m_mTextElements[enTextElements::LapHead].setPosition(irr::core::position2di(l_cTotal.LowerRightCorner.X + l_cPosSize.Width / 9, l_cTotal.UpperLeftCorner.Y));
          m_mTextElements[enTextElements::Lap    ].setPosition(m_mTextElements[enTextElements::LapHead].m_cThisRect.UpperLeftCorner + irr::core::position2di(0, l_cPosSize.Height));

          if (m_pPlayer->m_iPosition > 0 && m_pPlayer->m_iPosition != 99) {
            irr::s32 l_iOffset = 3 * m_cDefSize.Height / 2;

            irr::core::position2di l_cRank = l_cSpeed;
            l_cRank.X -= m_cLabelSize.Width / 2;
            l_cRank.Y  = m_mTextElements[enTextElements::Pos].m_cThisRect.LowerRightCorner.Y;

            irr::core::recti l_cRects[] = {
              irr::core::recti(l_cRank                                           , irr::core::dimension2du(m_cLabelSize.Width - m_cDefSize.Height, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0,     l_iOffset), irr::core::dimension2du(m_cLabelSize.Width - m_cDefSize.Height, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0, 2 * l_iOffset), irr::core::dimension2du(m_cLabelSize.Width - m_cDefSize.Height, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0, 3 * l_iOffset), irr::core::dimension2du(m_cLabelSize.Width - m_cDefSize.Height, m_cDefSize.Height)),
            };

            int l_iPos[] = {
              1,
              m_pPlayer->m_iPosition == 1 ? 2 : m_pPlayer->m_iPosition == 2 ? 2 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 2 : m_pPlayer->m_iPosition - 1,
              m_pPlayer->m_iPosition == 1 ? 3 : m_pPlayer->m_iPosition == 2 ? 3 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 1 : m_pPlayer->m_iPosition,
              m_pPlayer->m_iPosition == 1 ? 4 : m_pPlayer->m_iPosition == 2 ? 4 : m_pPlayer->m_iPosition == m_vRanking->size() ? m_pPlayer->m_iPosition      : m_pPlayer->m_iPosition + 1
            };

            for (int i = 0; i < 4 && i < m_vRanking->size(); i++) {
              if (l_iPos[i] == m_pPlayer->m_iPosition) {
                std::wstring l_sText = L" " + std::to_wstring(m_pPlayer->m_iPosition) + L": " + m_pPlayer->m_sWName;
                m_pDrv->draw2DRectangle(
                  irr::video::SColor(192, 232, 232, 232), 
                  l_cRects[i], &m_cRect
                );
                m_pDefFont->draw(l_sText.c_str(), l_cRects[i], irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cRect);

                irr::core::recti l_cNumber = irr::core::recti(irr::core::position2di(l_cRects[i].LowerRightCorner.X, l_cRects[i].UpperLeftCorner.Y), irr::core::dimension2du(m_cDefSize.Height, l_cRects[i].getHeight()));

                m_pDrv->draw2DRectangle(m_pPlayer->m_cBack, l_cNumber);
                m_pDrv->draw2DRectangleOutline(l_cNumber, m_pPlayer->m_cFrme);
                m_pTimeFont->draw(m_pPlayer->m_sNumber.c_str(), l_cNumber, m_pPlayer->m_cText, true, true);
              }
              else {
                for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
                  if ((*it)->m_iPosition == l_iPos[i]) {
                    m_pDrv->draw2DRectangle(
                      (*it)->m_iState == 1 ? irr::video::SColor(192,  96,  96, 255) :
                      (*it)->m_iState == 2 ? irr::video::SColor(192, 255,  96,  96) :
                      (*it)->m_iState == 3 ? irr::video::SColor(192, 255, 255,  96) : irr::video::SColor(192, 232, 232, 232), 
                      l_cRects[i], &m_cRect
                    );

                    std::wstring l_sText = L" " + std::to_wstring((*it)->m_iPosition) + L": " + (*it)->m_sWName;
                    m_pDefFont->draw(l_sText.c_str(), l_cRects[i], irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cRect);
                    
                    std::wstring l_sDeficit;

                    if (l_iPos[i] == 1) {
                      l_sDeficit = L"-" + getDeficitString(m_pPlayer->m_iDiffLeader);
                    }
                    else if (l_iPos[i] < m_pPlayer->m_iPosition) {
                      if (l_iPos[i] == m_pPlayer->m_iPosition - 1)
                        l_sDeficit = L"-" + getDeficitString(m_pPlayer->m_iDiffAhead);
                      else {
                        int l_iTotal = m_pPlayer->m_iDiffAhead;
                        for (std::vector<gameclasses::SPlayer*>::const_iterator it2 = m_vRanking->begin(); it2 != m_vRanking->end(); it2++) {
                          if ((*it2)->m_iPosition == m_pPlayer->m_iPosition - 1)
                            l_iTotal += (*it2)->m_iDiffAhead;
                        }
                        l_sDeficit = L"-" + getDeficitString(l_iTotal);
                      }
                    }
                    else {
                      if (m_pPlayer->m_iPosition == 1) {
                        l_sDeficit = L"+" + getDeficitString((*it)->m_iDiffLeader);
                      }
                      else {
                        l_sDeficit = L"+" + getDeficitString((*it)->m_iDiffAhead);
                      }
                    }

                    irr::core::dimension2du l_cSize = m_pDefFont->getDimension(l_sDeficit.c_str());
                    m_pDefFont->draw(l_sDeficit.c_str(), irr::core::recti(l_cRects[i].LowerRightCorner.X - l_cSize.Width, l_cRects[i].UpperLeftCorner.Y, l_cRects[i].LowerRightCorner.X, l_cRects[i].LowerRightCorner.Y), irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cRect);

                    irr::core::recti l_cNumber = irr::core::recti(irr::core::position2di(l_cRects[i].LowerRightCorner.X, l_cRects[i].UpperLeftCorner.Y), irr::core::dimension2du(m_cDefSize.Height, l_cRects[i].getHeight()));

                    m_pDrv->draw2DRectangle((*it)->m_cBack, l_cNumber);
                    m_pDrv->draw2DRectangleOutline(l_cNumber, (*it)->m_cFrme);
                    m_pTimeFont->draw((*it)->m_sNumber.c_str(), l_cNumber, (*it)->m_cText, true, true);

                    break;
                  }
                }
              }
            }
          }
        }
      }

      if (m_pRankParent->isVisible())
        m_pDrv->draw2DRectangle(m_cRankBack, AbsoluteClippingRect, &AbsoluteClippingRect);

      if (m_bShowLapTimes && m_pLapTimes != nullptr)
        m_pLapTimes->render(m_iStep, m_cRect);

      if (m_bShowRanking) {
        irr::core::position2di l_cPos = m_cRect.UpperLeftCorner;

        int l_iPos = 1;
        for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
          irr::video::SColor l_cColor = irr::video::SColor(128, 224, 244, 244);

          if ((*it)->m_iState == 1)
            l_cColor = irr::video::SColor(128, 0, 0, 255);
          else if ((*it)->m_iState == 2)
            l_cColor = irr::video::SColor(128, 255, 0, 0);
          else if ((*it)->m_iState == 3)
            l_cColor = irr::video::SColor(128, 255, 255, 0);

          irr::core::recti l_cNameRect = irr::core::recti(l_cPos, m_cPosNameDim);

          m_pDrv->draw2DRectangle(l_cColor, l_cNameRect);
          
          std::wstring l_sPos = l_iPos < 10 ? L" " + std::to_wstring(l_iPos) : std::to_wstring(l_iPos);

          m_pTimeFont->draw((L" " + l_sPos + L": " + helpers::s2ws((*it)->m_sShortName)).c_str(), l_cNameRect, irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cRect);

          irr::core::recti l_cNumber = irr::core::recti(irr::core::position2di(l_cNameRect.LowerRightCorner.X, l_cNameRect.UpperLeftCorner.Y), m_cStartNr);

          m_pDrv->draw2DRectangle((*it)->m_cBack, l_cNumber);
          m_pDrv->draw2DRectangleOutline(l_cNumber, (*it)->m_cFrme);
          m_pTimeFont->draw((*it)->m_sNumber.c_str(), l_cNumber, (*it)->m_cText, true, true);

          if ((*it)->m_iState == 4 && m_pCheckered != nullptr) {
            m_pDrv->draw2DImage(m_pCheckered, irr::core::recti(irr::core::position2di(l_cNumber.LowerRightCorner.X, l_cNumber.UpperLeftCorner.Y), m_cStartNr), irr::core::recti(irr::core::position2di(0, 0), m_cCheckered), nullptr, nullptr, false);
          }

          l_cPos.Y += m_iLapTimeOffset;
          l_iPos++;
        }
      }

      if (m_bPaused) {
        m_pDrv->draw2DRectangle(m_cRankBack, AbsoluteClippingRect);
      }
    }

    std::wstring CGameHUD::getDeficitString(int a_iDeficit) {
      return helpers::convertToTime(a_iDeficit) + L" ";
    }

    void CGameHUD::updateRanking() {
      for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if ((*it)->m_iId == m_iMarble && (*it)->m_iPosition > 0) {
          if (m_mTextElements.find(enTextElements::Pos) != m_mTextElements.end()) {
            m_mTextElements[enTextElements::Pos].m_sText = std::to_wstring((*it)->m_iPosition);
          }

          for (int i = 0; i < 3; i++)
            if (!m_aHiLight[i].m_bFinished) {
              m_aHiLight[i].m_iMarbleId = -1;
              m_aHiLight[i].m_bVisible  = !m_aHiLight[i].m_bFinished;
            }
            else m_aHiLight[i].m_bVisible = false;

          int l_iPos = (*it)->m_iPosition;
          
          // These positions need to be highlighted
          int l_aHighLight[3] = {
            l_iPos == 1 ? 2 : 1,
            l_iPos <= 2 ? 3 : l_iPos >= 15 ? 14 : l_iPos - 1,
            l_iPos <= 3 ? 4 : l_iPos == 16 ? 15 : l_iPos + 1
          };
          
          for (int i = 0; i < 3; i++) {
            for (std::vector<gameclasses::SPlayer*>::const_iterator it2 = m_vRanking->begin(); it2 != m_vRanking->end(); it2++) {
              if ((*it2)->m_iPosition == l_aHighLight[i]) {
                m_aHiLight[i].m_iMarbleId = (*it2)->m_iId;
                m_aHiLight[i].m_iPosition = l_aHighLight[i];
                m_aHiLight[i].m_pArrow   ->setVisible(false);
                m_aHiLight[i].m_pPosition->setVisible(false);
                
                std::wstring s = std::to_wstring(l_aHighLight[i]);

                switch (l_aHighLight[i]) {
                  case 1 : s += L"st"; break;
                  case 2 : s += L"nd"; break;
                  case 3 : s += L"rd"; break;
                  default: s += L"th"; break;
                }

                break;
              }
            }
          }

          break;
        }
      }

      int l_iIndex = 0;
      for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if (m_aRanking[l_iIndex] != nullptr) {
          m_aRanking[l_iIndex]->setData((*it)->m_sWName, (*it)->m_iDiffLeader, (*it)->m_bWithdrawn);
        }
        l_iIndex++;
      }

      for (int i = 0; i < 3; i++) {
        int l_iId = m_aHiLight[i].m_iMarbleId - 10000;
        if (l_iId >= 0 && l_iId < 16) {
          if (m_aFinished[l_iId]) {
            if (m_aHiLight[i].m_pArrow    != nullptr) m_aHiLight[i].m_pArrow   ->setVisible(false);
            if (m_aHiLight[i].m_pPosition != nullptr) m_aHiLight[i].m_pPosition->setVisible(false);

            m_aHiLight[i].m_bFinished = true;
          }
        }
        else {
          m_aHiLight[i].m_pArrow   ->setVisible(false);
          m_aHiLight[i].m_pPosition->setVisible(false);
        }
      }
    }

    void CGameHUD::setSettings(bool a_bHightlight, bool a_bShowCtrl, bool a_bShowRanking, bool a_bShowLapTimes) {
      m_bHightlight   = a_bHightlight;
      m_bShowCtrl     = a_bShowCtrl;
      m_bShowRanking  = a_bShowRanking;
      m_bShowLapTimes = a_bShowLapTimes;
    }

    bool CGameHUD::isResultParentVisible() {
      return m_pRankParent == nullptr || m_pRankParent->isVisible();
    }

    void CGameHUD::showResultParent() {
      if (m_pRankParent != nullptr)
        m_pRankParent->setVisible(true);
    }

    /**
    * This method is called to prepare for scene drawing,
    * i.e. show the necessary highlight nodes
    */
    void CGameHUD::beforeDrawScene() {
      irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->getActiveCamera();

      irr::core::vector3df l_cNormal   = (l_pCam->getTarget() - l_pCam->getAbsolutePosition()).normalize();
      irr::core::vector3df l_cPosition = l_pCam->getTarget() + 0.1f * (l_pCam->getAbsolutePosition() - l_pCam->getTarget());
      irr::core::plane3df  l_cPlane    = irr::core::plane3df(l_cPosition, l_cNormal);

      for (int i = 0; i < 3; i++) {
        bool b = true;
        
        if (m_mMarblePositions.find(m_aHiLight[i].m_iMarbleId) != m_mMarblePositions.end())
          b = l_cPlane.classifyPointRelation(m_mMarblePositions[m_aHiLight[i].m_iMarbleId]) == irr::core::ISREL3D_FRONT;

        m_aHiLight[i].m_pArrow   ->setVisible(!m_aHiLight[i].m_bFinished && b && m_aHiLight[i].m_iMarbleId >= 10000 && m_aHiLight[i].m_iMarbleId < 10016);
        m_aHiLight[i].m_pPosition->setVisible(!m_aHiLight[i].m_bFinished && b && m_aHiLight[i].m_iMarbleId >= 10000 && m_aHiLight[i].m_iMarbleId < 10016);

        m_aHiLight[i].m_bViewport = true;
      }
    }

    /**
    * This method is called when the scene was drawn to
    * hide all highlight nodes of this HUD
    */
    void CGameHUD::afterDrawScene() {
      for (int i = 0; i < 3; i++) {
        m_aHiLight[i].m_pArrow   ->setVisible(false);
        m_aHiLight[i].m_pPosition->setVisible(false);

        m_aHiLight[i].m_bViewport = false;
      }
    }

    /**
    * This method is only used to tell the HUD that it shall display AI help
    * @param a_pController the controller
    */
    void CGameHUD::setAiController(controller::IControllerAI* a_pController) {
      if (m_pAiController != a_pController) {
        m_pAiController = a_pController;
        if (m_pAiController != nullptr)
          m_pAiController->setHUD(this);
      }
    }

    /**
    * Update the AI help display of the HUD
    * @param a_bLeft does the marble steer left?
    * @param a_bRight does the marble steer right?
    * @param a_bForward does the marble accelerate?
    * @param a_bBackward does the marble decelerate?
    * @param a_bBrake is the marble braking?
    * @param a_bRespawn does the marble request manual respawn?
    * @param a_bAutomatic is the automatic control active?
    * @param a_cPoint1 the first point calculated by the AI
    * @param a_cPoint2 the second point calculated by the AI
    */
    void CGameHUD::updateAiHelp(bool a_bLeft, bool a_bRight, bool a_bForward, bool a_bBackward, bool a_bBrake, bool a_bRespawn, bool a_bAutomatic, const irr::core::vector3df &a_cPoint1, const irr::core::vector3df &a_cPoint2) {
      if (m_pAiHelp != nullptr) {
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Left     , a_bLeft      ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Right    , a_bRight     ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Forward  , a_bForward   ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Backward , a_bBackward  ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Brake    , a_bBrake     ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Respawn  , a_bRespawn   ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
        m_pAiHelp->setIconState(CHudAiHelp::enIcons::Automatic, a_bAutomatic ? CHudAiHelp::enState::Active : CHudAiHelp::enState::Inactive);
      }

      if (m_pAiNode != nullptr) {
        irr::video::SColor l_cColor = a_bForward ? irr::video::SColor(0xFF, 0, 0xFF, 0) : a_bBrake ? irr::video::SColor(0xFF, 0xFF, 0, 0) : a_bBackward ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF);
        m_pAiNode->setPosition(m_cPosition);
        m_pAiNode->setColor(l_cColor);
        m_pAiNode->setAiData(a_cPoint1, a_cPoint2);
      }
    }
  }
}
