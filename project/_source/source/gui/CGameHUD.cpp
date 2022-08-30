// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <scenenodes/CMyTextSceneNode.h>
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
    CGameHUD::SHighLight::SHighLight() : m_iMarbleId(-1), m_iPosition(0), m_bVisible(false), m_bFinished(false), m_bViewport(false), m_pArrow(nullptr), m_pPosition(nullptr) {
    }

    CGameHUD::SLapTime::SLapTime() : m_iStart(0), m_iEnd(0), m_iLapNo(0), m_iLapTime(-1) {
    }

    CGameHUD::SPlayerRacetime::SPlayerRacetime() : m_iFastest(-1), m_iLastSplit(-1) {
    }

    CGameHUD::STextElement::STextElement(const irr::core::recti &a_cRect, const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont, const irr::video::SColor &a_cBackground, const irr::video::SColor &a_cTextColor, irr::video::IVideoDriver *a_pDrv) :
      m_cThisRect  (a_cRect),
      m_sText      (a_sText),
      m_pFont      (a_pFont),
      m_cBackground(a_cBackground),
      m_cTextColor (a_cTextColor),
      m_pDrv       (a_pDrv),
      m_eAlignV    (irr::gui::EGUIA_UPPERLEFT),
      m_eAlignH    (irr::gui::EGUIA_UPPERLEFT),
      m_bVisible   (true)
    {
    }

    CGameHUD::STextElement::STextElement() :
      m_sText(L""),
      m_pFont(nullptr),
      m_pDrv (nullptr)
    {
    }

    void CGameHUD::STextElement::render() {
      if (m_bVisible) {
        m_pDrv->draw2DRectangle(m_cBackground, m_cThisRect);

        irr::core::recti        l_cTextRect = m_cThisRect;
        irr::core::dimension2du l_cTextDim  = m_pFont->getDimension(m_sText.c_str());
        
        if (m_eAlignH == irr::gui::EGUIA_LOWERRIGHT) {
          l_cTextRect.UpperLeftCorner.X = m_cThisRect.LowerRightCorner.X - l_cTextDim.Width;
        }

        if (m_eAlignV == irr::gui::EGUIA_LOWERRIGHT) {
          l_cTextRect.UpperLeftCorner.Y = m_cThisRect.LowerRightCorner.Y - l_cTextDim.Height;
        }

        m_pFont->draw(m_sText.c_str(), l_cTextRect, m_cTextColor, m_eAlignH == irr::gui::EGUIA_CENTER, m_eAlignV == irr::gui::EGUIA_CENTER);
      }
    }

    void CGameHUD::STextElement::setPosition(const irr::core::position2di& a_cPos) {
      irr::core::dimension2di l_cSize = m_cThisRect.getSize();
      m_cThisRect.UpperLeftCorner  = a_cPos;
      m_cThisRect.LowerRightCorner = a_cPos + l_cSize;
    }

    /**
    * This function receives messages of type "PlayerRespawn"
    * @param a_MarbleId ID of the marble
    * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
    */
    void CGameHUD::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_MarbleId == m_iMarble) {
        m_bShowSpeed = a_State == 2;
        m_bRespawn   = a_State != 2;
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

    /**
    * This function receives messages of type "CameraRespawn"
    * @param a_MarbleId The ID of the marble which is respawning
    * @param a_Position The new position of the camera
    * @param a_Target The new target of the camera, i.e. the future position of the marble
    */
    void CGameHUD::onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target) {
      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if ((*it)->m_iId == a_MarbleId) {
          (*it)->m_iState = 3;
          break;
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
        m_bShowSpeed = false;
        m_bFinished  = true;
        m_bRespawn   = false;
        m_bStunned   = false;
      }
      else {
        for (int i = 0; i < 3; i++) {
          if (m_aHiLight[i].m_iMarbleId == a_MarbleId) {
            m_aHiLight[i].m_bFinished = true;
            m_aHiLight[i].m_bVisible  = false;
          }
        }
      }

      // The player has finished so we remove the currently running lap. I have chosen to do it this way because this
      // callback is called *after* the lap has started, and using the number of laps doesn't work either because the
      // player may already lapped.
      if (m_mLapTimes.find(a_MarbleId) != m_mLapTimes.end() && m_mLapTimes[a_MarbleId].m_vLapTimes.size() > 0) {
        m_mLapTimes[a_MarbleId].m_vLapTimes.erase(m_mLapTimes[a_MarbleId].m_vLapTimes.end() - 1);
      }

      for (std::vector<gameclasses::SPlayer*>::iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if ((*it)->m_iId == a_MarbleId) {
          (*it)->m_iState = 4;
          break;
        }
      }
    }

    /**
    * This function receives messages of type "PlayerStunned"
    * @param a_MarbleId ID of the marble
    * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
    */
    void CGameHUD::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_MarbleId == m_iMarble) {
        m_bShowSpeed = a_State != 1;
        m_bStunned   = a_State == 1;
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
      if (a_MarbleId == m_iMarble)
        m_iFinished = a_Position - 1;
    }

    /**
    * This function receives messages of type "LapStart"
    * @param a_MarbleId ID of the marble
    * @param a_LapNo Number of the started lap
    */
    void CGameHUD::onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) {
      if (m_mLapTimes.find(a_MarbleId) == m_mLapTimes.end())
        m_mLapTimes[a_MarbleId] = SPlayerRacetime();

      if (m_mLapTimes[a_MarbleId].m_vLapTimes.size() > 0) {
        int l_iTime = m_iStep - m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iStart;
        m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iEnd     = m_iStep;
        m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iLapTime = l_iTime;

        if (m_iBestLapTime == -1 || l_iTime < m_iBestLapTime) {
          m_iBestLapTime = l_iTime;
        }

        if (l_iTime < m_mLapTimes[a_MarbleId].m_iFastest || m_mLapTimes[a_MarbleId].m_iFastest == -1) {
          m_mLapTimes[a_MarbleId].m_iFastest = l_iTime;
        }
      }

      m_mLapTimes[a_MarbleId].m_vLapTimes.push_back(SLapTime());
      m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iStart = m_iStep;
      m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iLapNo = a_LapNo;
      
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
      }
    }

    /**
    * This function receives messages of type "Countdown"
    * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
    */
    void CGameHUD::onCountdown(irr::u8 a_Tick) {
      m_iCountDown = a_Tick;

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
        m_iWithdraw = m_iStep + a_Timeout;
        m_pWithdraw->setVisible(true);
      }
    }


    /**
    * This function receives messages of type "PauseChanged"
    * @param a_Paused The current paused state
    */
    void CGameHUD::onPausechanged(bool a_Paused) {
      m_bPaused = a_Paused;
      if (m_pRankParent != nullptr && !m_bFinished)
        m_pRankParent->setVisible(a_Paused);

      for (int i = 0; i < 16; i++) {
        m_aRanking[i]->setVisible(a_Paused || m_aRostrum[i]);
      }
    }

    /**
    * This function receives messages of type "Checkpoint"
    * @param a_MarbleId ID of the marble
    * @param a_Checkpoint The checkpoint ID the player has passed
    */
    void CGameHUD::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) {
      if (m_mLapTimes.find(a_MarbleId) != m_mLapTimes.end()) {
        int l_iSplitTime = m_iStep - m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_iStart;

        if (l_iSplitTime != 0) {
          m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_vSplitTimes.push_back(l_iSplitTime);
          m_mLapTimes[a_MarbleId].m_iLastSplit = m_iStep;

          size_t l_iSplitIdx = m_mLapTimes[a_MarbleId].m_vLapTimes.back().m_vSplitTimes.size();

          if (m_vBestSplits.size() < l_iSplitIdx) {
            m_vBestSplits.push_back(l_iSplitTime);
          }
          else {
            if (l_iSplitTime < m_vBestSplits[l_iSplitIdx - 1]) {
              m_vBestSplits[l_iSplitIdx - 1] = l_iSplitTime;
            }
          }

          if (m_mLapTimes[a_MarbleId].m_vPlayerBestSplit.size() < l_iSplitIdx) {
            m_mLapTimes[a_MarbleId].m_vPlayerBestSplit.push_back(l_iSplitTime);
          }
          else {
            if (l_iSplitTime < m_mLapTimes[a_MarbleId].m_vPlayerBestSplit[l_iSplitIdx - 1])
              m_mLapTimes[a_MarbleId].m_vPlayerBestSplit[l_iSplitIdx - 1] = l_iSplitTime;
          }
        }
      }
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

      if (m_iCountDown == 0 && m_iStep > m_iGoStep + 240) {
        m_fCdAlpha = std::min(1.0f, ((irr::f32)(m_iGoStep - m_iStep + 300)) / 60.0f);
        if (m_fCdAlpha < 0.0f)
          m_iCountDown = -1;
      }

      if (m_iWithdraw != -1 && m_iWithdraw < m_iStep) {
        m_pWithdraw->setVisible(false);
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
        m_cUpVector = a_CameraUp;
        m_fSteer    = ((irr::f32)a_ControlX) / 127.0f;
        m_fThrottle = ((irr::f32)a_ControlY) / 127.0f;
        m_bBrake    = a_ControlBrake;

        m_cUpVector.normalize();
      }
      else {
        if (m_bHightlight) {
          for (int i = 0; i < 3; i++) {
            if (m_aHiLight[i].m_iMarbleId == a_ObjectId) {
              if (m_mMarblePositions.find(m_aHiLight[i].m_iMarbleId) != m_mMarblePositions.end() && m_aHiLight[i].m_pArrow != nullptr && m_aHiLight[i].m_pPosition != nullptr) {
                m_aHiLight[i].m_pArrow->setPosition(m_mMarblePositions[a_ObjectId]);
                
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
                m_aHiLight[i].m_pPosition->setPosition(a_Position + 2.5f * a_CameraUp);
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
      m_bHightlight   (true),
      m_bShowCtrl     (true),
      m_bShowRanking  (true),
      m_bFadeStart    (false),
      m_bPaused       (false),
      m_cRect         (a_cRect),
      m_pGui          (a_pGui),
      m_pPlayer       (a_pPlayer),
      m_cDefSize      (irr::core::dimension2du()),
      m_pDefFont      (nullptr),
      m_pTimeFont     (nullptr),
      m_pDrv          (a_pGui->getVideoDriver()),
      m_cScreen       (irr::core::dimension2du()),
      m_pSpeedFont    (nullptr),
      m_cSpeedTotal   (irr::core::dimension2du()),
      m_cSpeedText    (irr::core::dimension2du()),
      m_cSpeedOffset  (irr::core::position2di()),
      m_cSpeedBar     (irr::core::dimension2du()),
      m_cUpVector     (irr::core::vector3df()),
      m_pRankParent   (nullptr),
      m_pPaused       (nullptr),
      m_iCountDown    (4),
      m_fCdAlpha      (1.0f),
      m_iGoStep       (0),
      m_iStep         (0),
      m_pStunned      (nullptr),
      m_pRespawn      (nullptr),
      m_pFinished     (nullptr),
      m_bRespawn      (false),
      m_bStunned      (false),
      m_bFinished     (false),
      m_iFadeStart    (-1),
      m_iFinished     (-1),
      m_pPosFont      (nullptr),
      m_pWithdraw     (nullptr),
      m_iWithdraw     (-1),
      m_cRankBack     (irr::video::SColor(96, 192, 192, 192)),
      m_pSmgr         (CGlobal::getInstance()->getSceneManager()),
      m_pColMgr       (nullptr),
      m_vRanking      (a_vRanking),
      m_iBestLapTime  (-1),
      m_iLapTimeOffset(0),
      m_bShowLapTimes (false),
      m_pCheckered    (nullptr)
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

      m_pSpeedFont = l_pRegular;
      m_cSpeedText = getDimension(L"666", m_pSpeedFont);

      m_cSpeedTotal.Width  = a_cRect.getWidth() / 4;
      m_cSpeedTotal.Height = m_cSpeedText.Height;

      m_cSpeedBar.Width  = m_cSpeedTotal.Width - m_cSpeedText.Width;
      m_cSpeedBar.Height = m_cSpeedTotal.Height;

      m_cSpeedOffset.X = m_cSpeedTotal.Width - m_cSpeedBar.Width;

      int l_iSpeedOffset = m_cSpeedBar.Height / 8;
      if (l_iSpeedOffset < 2)
        l_iSpeedOffset = 2;

      m_cSpeedOffset.Y += l_iSpeedOffset;
      m_cSpeedBar.Width -= l_iSpeedOffset;
      m_cSpeedBar.Height -= 2 * l_iSpeedOffset;

      m_cSpeedOffset.Y = (m_cSpeedTotal.Height - m_cSpeedBar.Height) / 2;

      m_iCtrlHeight = m_cSpeedTotal.Height / 4;
      if (m_iCtrlHeight < 2) m_iCtrlHeight = 2;

      irr::core::dimension2du l_cSizeTop = getDimension(L"Lap"    , l_pSmall  );
      irr::core::dimension2du l_cSizeBot = getDimension(L"66 / 66", l_pRegular);

      {
        irr::core::dimension2du d = getDimension(L"Pos", l_pSmall);

        if (d.Width > l_cSizeTop.Width)
          l_cSizeTop.Width = d.Width;
      }

      if (l_cSizeBot.Width > l_cSizeTop.Width)
        l_cSizeTop.Width = l_cSizeBot.Width;
      else
        l_cSizeBot.Width = l_cSizeTop.Width;

      std::wstring l_sName = helpers::s2ws(a_pPlayer->m_sName);
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

        m_aRanking[i]->setData(helpers::s2ws((*m_vRanking)[i]->m_sName), 0, (*m_vRanking)[i]->m_bWithdrawn);
        m_aRanking[i]->drop();

        if (i == (m_iPlayers / 2) - 1)
          l_iOffsetY = 0;
        else
          l_iOffsetY += 3 * l_cRankSize.Height / 2;
      }

      m_pStunned  = m_pDrv->getTexture("data/images/text_stunned.png");
      m_pRespawn  = m_pDrv->getTexture("data/images/text_respawn.png");
      m_pFinished = m_pDrv->getTexture("data/images/text_finished.png");

      m_pCountDown[4] = m_pDrv->getTexture("data/images/countdown_ready.png");
      m_pCountDown[3] = m_pDrv->getTexture("data/images/countdown_three.png");
      m_pCountDown[2] = m_pDrv->getTexture("data/images/countdown_two.png");
      m_pCountDown[1] = m_pDrv->getTexture("data/images/countdown_one.png");
      m_pCountDown[0] = m_pDrv->getTexture("data/images/countdown_go.png");

      m_pLaurel[0] = m_pDrv->getTexture("data/images/laurel_gold.png");
      m_pLaurel[1] = m_pDrv->getTexture("data/images/laurel_silver.png");
      m_pLaurel[2] = m_pDrv->getTexture("data/images/laurel_bronze.png");
      m_pLaurel[3] = m_pDrv->getTexture("data/images/laurel_rest.png");

      m_pPaused = m_pDrv->getTexture("data/images/pause.png");

      if (m_pCountDown[0] != nullptr) {
        irr::core::dimension2du l_cSize   = m_pCountDown[0]->getOriginalSize();
        irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());
        irr::core::position2di  l_cPos    = a_cRect.UpperLeftCorner;

        irr::f32 l_fRatio = ((irr::f32)l_cScreen.Width) / ((irr::f32)l_cScreen.Height);

        printf("Ratio: %.2f\n", l_fRatio);

        irr::core::dimension2du l_cCntSize = irr::core::dimension2du(
          l_cSize.Width  * l_cScreen.Width  / 3840,
          l_cSize.Height * l_cScreen.Height / 2160
        );

        l_cCntSize.Height = 250 * l_cCntSize.Width / 2560;

        irr::core::position2di l_cCntPos = irr::core::position2di(
          l_cPos.X + l_cScreen .Width / 2 - l_cCntSize.Width  / 2,
          l_cPos.Y + l_cRankPos.Y     / 2 - l_cCntSize.Height / 2
        );

        m_cCountDown = irr::core::recti(l_cCntPos, l_cCntSize);
        m_cCntSource = irr::core::recti(irr::core::vector2di(0, 0), m_pCountDown[0]->getOriginalSize());

        if (m_pLaurel[0] != nullptr) {
          irr::core::dimension2di l_cDim = irr::core::dimension2di(m_cCountDown.getHeight(), m_cCountDown.getHeight());

          m_cLaurelLft = irr::core::recti(m_cCountDown.UpperLeftCorner                                                                        , l_cDim);
          m_cLaurelRgt = irr::core::recti(irr::core::vector2di(m_cCountDown.LowerRightCorner.X - l_cDim.Width, m_cCountDown.UpperLeftCorner.Y), l_cDim);

          m_cLaurelSrc = irr::core::recti(irr::core::vector2di(0, 0), m_pLaurel[0]->getOriginalSize());
        }

        m_pWithdraw = m_pGui->addStaticText(L"Withdraw from Race? Click again!", m_cCountDown);
        m_pWithdraw->setOverrideFont(l_pBig);
        m_pWithdraw->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
        m_pWithdraw->setVisible(false);
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

      m_cLapTotalDim = m_pTimeFont->getDimension(L"Lap #66: 666.66");
      m_cLapTotalDim.Width  = 5 * m_cLapTotalDim.Width  / 4;
      m_cLapTotalDim.Height = 5 * m_cLapTotalDim.Height / 4;
      
      m_cLapNoDim = m_pTimeFont->getDimension(L" Lap 66: ");
      m_cLapNoDim.Width  = 5 * m_cLapNoDim.Width  / 4;
      m_cLapNoDim.Height = 5 * m_cLapNoDim.Height / 4;

      m_cPosNameDim = m_pTimeFont->getDimension(L"66: XXXXX");
      m_cPosNameDim.Width  = 10 * m_cPosNameDim.Width  / 9;
      m_cPosNameDim.Height =  5 * m_cPosNameDim.Height / 4;

      m_cStartNr = irr::core::dimension2du(m_cPosNameDim.Height, m_cPosNameDim.Height);

      m_iLapTimeOffset = 5 * m_cLapTotalDim.Height / 4;

      m_cLapTimePos.X = a_cRect.LowerRightCorner.X - m_cLapTotalDim.Width;

      m_pCheckered = m_pDrv->getTexture("data/images/checkered.png");
    }

    CGameHUD::~CGameHUD() {
    }

    void CGameHUD::draw() {
      if (m_bShowSpeed) {
        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.render();

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
          l_cSpeed.X = m_cRect.UpperLeftCorner.X + m_cRect.getWidth () * l_cSpeed.X / m_cScreen.Width;
          l_cSpeed.Y = m_cRect.UpperLeftCorner.Y + m_cRect.getHeight() * l_cSpeed.Y / m_cScreen.Height;

          irr::core::recti l_cTotal = irr::core::recti(l_cSpeed - irr::core::vector2di(m_cSpeedTotal.Width, m_cSpeedTotal.Height) / 2, m_cSpeedTotal);

          irr::core::dimension2di l_cPosSize = m_mTextElements[enTextElements::PosHead].m_cThisRect.getSize();

          m_mTextElements[enTextElements::PosHead].setPosition(l_cTotal.UpperLeftCorner - irr::core::position2di(10 * l_cPosSize.Width / 9, 0));
          m_mTextElements[enTextElements::Pos    ].setPosition(m_mTextElements[enTextElements::PosHead].m_cThisRect.UpperLeftCorner + irr::core::position2di(0, l_cPosSize.Height));

          m_mTextElements[enTextElements::LapHead].setPosition(irr::core::position2di(l_cTotal.LowerRightCorner.X + l_cPosSize.Width / 9, l_cTotal.UpperLeftCorner.Y));
          m_mTextElements[enTextElements::Lap    ].setPosition(m_mTextElements[enTextElements::LapHead].m_cThisRect.UpperLeftCorner + irr::core::position2di(0, l_cPosSize.Height));

          m_pDrv->draw2DRectangle(irr::video::SColor(160, 192, 192, 192), l_cTotal, &m_cRect);
          wchar_t s[0xFF];
          swprintf(s, 0xFF, L"%.0f", m_fVel);
          m_pSpeedFont->draw(s, irr::core::recti(l_cTotal.UpperLeftCorner, m_cSpeedText), irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cRect);

          int l_iVel = m_fVel > 150.0f ? 150 : (int)m_fVel;

          irr::f32 l_fRd = m_fVel < 75.0f ? 0.0f : m_fVel > 125.0f ? 1.0f :        (m_fVel - 75.0f) / 50.0f;
          irr::f32 l_fGr = m_fVel < 75.0f ? 1.0f : m_fVel > 125.0f ? 0.0f : 1.0f - (m_fVel - 75.0f) / 50.0f;

          l_fGr = l_fGr * l_fGr;

          m_pDrv->draw2DRectangle(irr::video::SColor(128, (irr::u32)(255.0f * l_fRd), (irr::u32)(255.0f * l_fGr), 0), irr::core::recti(l_cTotal.UpperLeftCorner + m_cSpeedOffset, irr::core::dimension2di(l_iVel * m_cSpeedBar.Width / 150, m_cSpeedBar.Height)), &m_cRect);

          l_cSpeed.Y = l_cTotal.LowerRightCorner.Y + m_iCtrlHeight;

          if (m_bShowCtrl) {
            irr::core::recti l_cSteer = irr::core::recti(l_cTotal.getCenter().X, l_cTotal.LowerRightCorner.Y + m_iCtrlHeight, l_cTotal.getCenter().X + (irr::s32)(m_fSteer * (irr::f32)l_cTotal.getWidth() / 2.0f), l_cTotal.LowerRightCorner.Y + 2 * m_iCtrlHeight);
            if (l_cSteer.UpperLeftCorner.X > l_cSteer.LowerRightCorner.X) {
              irr::s32 l_iDummy = l_cSteer.UpperLeftCorner.X;
              l_cSteer.UpperLeftCorner.X = l_cSteer.LowerRightCorner.X;
              l_cSteer.LowerRightCorner.X = l_iDummy;
            }

            m_pDrv->draw2DRectangle(irr::video::SColor(128, 0, 0, 255), l_cSteer, &m_cRect);

            irr::core::recti l_cThrottle = irr::core::recti(l_cTotal.getCenter().X, l_cTotal.LowerRightCorner.Y + 3 * m_iCtrlHeight, l_cTotal.getCenter().X + (irr::s32)(m_fThrottle * (irr::f32)l_cTotal.getWidth() / 2.0f), l_cTotal.LowerRightCorner.Y + 4 * m_iCtrlHeight);

            if (l_cThrottle.UpperLeftCorner.X > l_cThrottle.LowerRightCorner.X) {
              irr::s32 l_iDummy = l_cThrottle.UpperLeftCorner.X;
              l_cThrottle.UpperLeftCorner.X = l_cThrottle.LowerRightCorner.X;
              l_cThrottle.LowerRightCorner.X = l_iDummy;
            }

            m_pDrv->draw2DRectangle(m_fThrottle < 0 ? irr::video::SColor(128, 255, 255, 0) : irr::video::SColor(128, 0, 255, 0), l_cThrottle, &m_cRect);

            if (m_bBrake) {
              m_pDrv->draw2DRectangle(irr::video::SColor(128, 255, 0, 0), irr::core::recti(l_cTotal.UpperLeftCorner.X, l_cTotal.LowerRightCorner.Y + 2 * m_iCtrlHeight, l_cTotal.LowerRightCorner.X, l_cTotal.LowerRightCorner.Y + 3 * m_iCtrlHeight), &m_cRect);
            }

            l_cSpeed.Y += 4 * m_iCtrlHeight;
          }

          if (m_pPlayer->m_iPosition > 0 && m_pPlayer->m_iPosition != 99) {
            irr::s32 l_iOffset = 3 * m_cDefSize.Height / 2;

            irr::core::position2di l_cRank = l_cSpeed;
            l_cRank.X -= m_cSpeedTotal.Width / 2;

            irr::core::recti l_cRects[] = {
              irr::core::recti(l_cRank                                           , irr::core::dimension2du(m_cSpeedTotal.Width, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0,     l_iOffset), irr::core::dimension2du(m_cSpeedTotal.Width, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0, 2 * l_iOffset), irr::core::dimension2du(m_cSpeedTotal.Width, m_cDefSize.Height)),
              irr::core::recti(l_cRank + irr::core::position2di(0, 3 * l_iOffset), irr::core::dimension2du(m_cSpeedTotal.Width, m_cDefSize.Height)),
            };

            int l_iPos[] = {
              1,
              m_pPlayer->m_iPosition == 1 ? 2 : m_pPlayer->m_iPosition == 2 ? 2 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 2 : m_pPlayer->m_iPosition - 1,
              m_pPlayer->m_iPosition == 1 ? 3 : m_pPlayer->m_iPosition == 2 ? 3 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 1 : m_pPlayer->m_iPosition,
              m_pPlayer->m_iPosition == 1 ? 4 : m_pPlayer->m_iPosition == 2 ? 4 : m_pPlayer->m_iPosition == m_vRanking->size() ? m_pPlayer->m_iPosition      : m_pPlayer->m_iPosition + 1
            };

            for (int i = 0; i < 4 && i < m_vRanking->size(); i++) {
              m_pDrv->draw2DRectangle(irr::video::SColor(192, 232, 232, 232), l_cRects[i], &m_cRect);
              if (l_iPos[i] == m_pPlayer->m_iPosition) {
                std::wstring l_sText = L" " + std::to_wstring(m_pPlayer->m_iPosition) + L": " + helpers::s2ws(m_pPlayer->m_sName);
                m_pDefFont->draw(l_sText.c_str(), l_cRects[i], irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cRect);
              }
              else {
                for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
                  if ((*it)->m_iPosition == l_iPos[i]) {
                    std::wstring l_sText = L" " + std::to_wstring((*it)->m_iPosition) + L": " + helpers::s2ws((*it)->m_sName).c_str();
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

      if (!m_pWithdraw->isVisible()) {
        if (m_iCountDown >= 0 && m_iCountDown < 5 && m_pCountDown[m_iCountDown] != nullptr) {
          irr::video::SColor l_aColor[] = {
            irr::video::SColor((irr::u32)(255.0f * m_fCdAlpha), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fCdAlpha), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fCdAlpha), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fCdAlpha), 255, 255, 255)
          };

          m_pDrv->draw2DImage(m_pCountDown[m_iCountDown], m_cCountDown, m_cCntSource, nullptr, l_aColor, true);
        }

        if (m_bRespawn && m_pRespawn != nullptr)
          m_pDrv->draw2DImage(m_pRespawn, m_cCountDown, m_cCntSource, nullptr, nullptr, true);

        if (m_bStunned && m_pStunned != nullptr)
          m_pDrv->draw2DImage(m_pStunned, m_cCountDown, m_cCntSource, nullptr, nullptr, true);

        if (m_bFinished) {
          if (m_pFinished != nullptr) {
            m_pDrv->draw2DImage(m_pFinished, m_cCountDown, m_cCntSource, nullptr, nullptr, true);
          }

          if (m_iFinished >= 0) {
            int l_iIndex = m_iFinished < 3 ? m_iFinished : 3;
            m_pDrv->draw2DImage(m_pLaurel[l_iIndex], m_cLaurelLft, m_cLaurelSrc, nullptr, nullptr, true);
            m_pDrv->draw2DImage(m_pLaurel[l_iIndex], m_cLaurelRgt, m_cLaurelSrc, nullptr, nullptr, true);

            if (m_pPosFont != nullptr) {
              m_pPosFont->draw(std::to_wstring(m_iFinished + 1).c_str(), m_cLaurelLft, irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pPosFont->draw(std::to_wstring(m_iFinished + 1).c_str(), m_cLaurelRgt, irr::video::SColor(0xFF, 0, 0, 0), true, true);
            }
          }
        }
      }

      irr::core::position2di l_cLapTimePos = m_cLapTimePos;
      if (m_bShowLapTimes && m_mLapTimes.find(m_iMarble) != m_mLapTimes.end()) {
        for (std::vector<SLapTime>::iterator it = m_mLapTimes[m_iMarble].m_vLapTimes.begin(); it != m_mLapTimes[m_iMarble].m_vLapTimes.end(); it++) {
          std::wstring l_sLap  = L" Lap " + std::to_wstring((*it).m_iLapNo) + L": ";
          std::wstring l_sTime = L"";

          irr::video::SColor l_cColor = irr::video::SColor(128, 224, 224, 244);
          irr::video::SColor l_cText  = irr::video::SColor(255,   0,   0,   0);

          if ((*it).m_iLapTime != -1) {
            l_sTime = helpers::convertToTime((*it).m_iLapTime);

            if ((*it).m_iLapTime == m_iBestLapTime) {
              l_cColor = irr::video::SColor(128, 255, 96, 96);
            }
            else if ((*it).m_iLapTime == m_mLapTimes[m_iMarble].m_iFastest) {
              l_cColor = irr::video::SColor(128, 255, 255, 96);
            }
          }
          else {
            if (m_mLapTimes[m_iMarble].m_iLastSplit != -1 && m_iStep - m_mLapTimes[m_iMarble].m_iLastSplit <= 180) {
              irr::s32 l_iSplit = m_mLapTimes[m_iMarble].m_vLapTimes.back().m_vSplitTimes.back();
              l_sTime = helpers::convertToTime(l_iSplit);
              l_cColor = irr::video::SColor(128, 192, 192, 192);

              if (l_iSplit == m_vBestSplits[m_mLapTimes[m_iMarble].m_vLapTimes.back().m_vSplitTimes.size() - 1])
                l_cText = irr::video::SColor(0xFF, 255, 0, 0);
              else if (l_iSplit == m_mLapTimes[m_iMarble].m_vPlayerBestSplit[m_mLapTimes[m_iMarble].m_vLapTimes.back().m_vSplitTimes.size() - 1])
                l_cText = irr::video::SColor(0xFF, 255, 255, 0);
            }
            else l_sTime = helpers::convertToTime(m_iStep - (*it).m_iStart);
          }

          irr::core::dimension2du l_cSize = m_pTimeFont->getDimension(l_sTime.c_str());

          irr::core::recti l_cRect = irr::core::recti(l_cLapTimePos, m_cLapTotalDim);

          m_pDrv->draw2DRectangle(l_cColor, l_cRect, &m_cRect);
          m_pTimeFont->draw(l_sLap.c_str(), irr::core::recti(l_cLapTimePos, m_cLapTotalDim), irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cRect);

          irr::core::recti l_cRectTime = irr::core::recti(
            l_cRect.LowerRightCorner.X - m_cLapTotalDim.Height / 4 - l_cSize.Width,
            l_cRect.UpperLeftCorner.Y,
            l_cRect.LowerRightCorner.X,
            l_cRect.LowerRightCorner.Y
          );

          m_pTimeFont->draw(l_sTime.c_str(), l_cRectTime, l_cText, false, true);

          l_cLapTimePos.Y += m_iLapTimeOffset;
        }
      }

      if (m_bShowLapTimes && !m_bFinished && m_iGoStep != 0) {
        irr::core::recti l_cRect = irr::core::recti(l_cLapTimePos, m_cLapTotalDim);
        m_pDrv->draw2DRectangle(irr::video::SColor(128, 224, 224, 224), l_cRect);
        m_pTimeFont->draw(L" Race: ", l_cRect, irr::video::SColor(0xFF, 0, 0, 0), false, true, &l_cRect);

        std::wstring l_sTime = helpers::convertToTime(m_iStep - m_iGoStep);
        irr::core::dimension2du l_cSize = m_pTimeFont->getDimension(l_sTime.c_str());

        irr::core::recti l_cRectTime = irr::core::recti(
          l_cRect.LowerRightCorner.X - m_cLapTotalDim.Height / 4 - l_cSize.Width,
          l_cRect.UpperLeftCorner.Y,
          l_cRect.LowerRightCorner.X,
          l_cRect.LowerRightCorner.Y
        );

        m_pTimeFont->draw(l_sTime.c_str(), l_cRectTime, irr::video::SColor(0xFF, 0, 0, 0), false, true);
      }

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
            m_pDrv->draw2DImage(m_pCheckered, irr::core::recti(irr::core::position2di(l_cNumber.LowerRightCorner.X, l_cNumber.UpperLeftCorner.Y), m_cStartNr), irr::core::recti(irr::core::position2di(0, 0), m_cStartNr));
          }

          l_cPos.Y += m_iLapTimeOffset;
          l_iPos++;
        }
      }

      if (m_bPaused) {
        m_pDrv->draw2DRectangle(m_cRankBack, AbsoluteClippingRect);
        if (m_pPaused != nullptr)
          m_pDrv->draw2DImage(m_pPaused, m_cCountDown, m_cCntSource, &AbsoluteClippingRect, nullptr, true);
      }
    }

    std::wstring CGameHUD::getDeficitString(int a_iDeficit) {
      return helpers::convertToTime(a_iDeficit);
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
          m_aRanking[l_iIndex]->setData(helpers::s2ws((*it)->m_sName), (*it)->m_iDiffLeader, (*it)->m_bWithdrawn);
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
  }
}
