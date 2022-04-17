// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gameclasses/SPlayer.h>
#include <gui/CRankingElement.h>
#include <gfx/SViewPort.h>
#include <gui/CGameHUD.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    const int c_RankingPos     = 0;     /**< Index for the position text of the ranking list */
    const int c_RankingName    = 1;     /**< Index for the name text of the ranking list */
    const int c_RankingDeficit = 2;     /**< Index for the deficit to the leader text of the ranking list */
    

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

    /**
    * This function receives messages of type "PlayerRespawn"
    * @param a_MarbleId ID of the marble
    * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
    */
    void CGameHUD::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_MarbleId == m_iMarble)
        m_bShowSpeed = a_State == 2;
    }

    /**
    * This function receives messages of type "PlayerFinished"
    * @param a_MarbleId ID of the finished marble
    * @param a_RaceTime Racetime of the finished player in simulation steps
    * @param a_Laps The number of laps the player has done
    */
    void CGameHUD::onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {
      if (a_MarbleId == m_iMarble) {
        m_bShowSpeed = false;

        m_iLeader = -2;
        m_iAhead  = -2;

        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.m_bVisible = false;
      }

      if (a_MarbleId == m_iLeader)
        m_iLeader = -2;
      
      if (a_MarbleId == m_iAhead)
        m_iAhead = -2;
    }

    /**
    * This function receives messages of type "PlayerStunned"
    * @param a_MarbleId ID of the marble
    * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
    */
    void CGameHUD::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_MarbleId == m_iMarble)
        m_bShowSpeed = a_State != 1;
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
    }

    /**
    * This function receives messages of type "LapStart"
    * @param a_MarbleId ID of the marble
    * @param a_LapNo Number of the started lap
    */
    void CGameHUD::onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) {
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

      if (a_MarbleId == m_iMarble)
        m_pRankParent->setVisible(true);
    }

    /**
    * This function receives messages of type "Countdown"
    * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
    */
    void CGameHUD::onCountdown(irr::u8 a_Tick) {
      if (a_Tick == 1) {
        m_bFadeStart = true;
      }
      else if (a_Tick == 0) {
        m_bFadeStart = false;
        m_pRankParent->setVisible(false);

        m_pRankParent->setBackgroundColor(irr::video::SColor(96, 192, 192, 192));

        for (int i = 0; i < 16; i++) {
          if (m_aRanking[i] != nullptr)
            m_aRanking[i]->setAlpha(1.0f);
        }

        m_bShowSpeed = true;

        for (int i = 0; i < 16; i++)
          if (m_aRanking[i] != nullptr)
            m_aRanking[i]->setVisible(false);
      }
    }

    /**
    * This function receives messages of type "StepMsg"
    * @param a_StepNo The current step number
    */
    void CGameHUD::onStepmsg(irr::u32 a_StepNo) {
      if (m_bFadeStart) {
        irr::f32 l_fFactor = 1.0f - ((irr::f32)a_StepNo - 480) / 120.0f;
        m_pRankParent->setBackgroundColor(irr::video::SColor((irr::u32)(96.0f * l_fFactor), 192, 192, 192));

        for (int i = 0; i < 16; i++) {
          if (m_aRanking[i] != nullptr)
            m_aRanking[i]->setAlpha(l_fFactor);
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
    void CGameHUD::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      m_mMarblePositions[a_ObjectId] = a_Position;

      if (a_ObjectId == m_iMarble) {
        m_fVel      = a_LinearVelocity.getLength();
        m_cUpVector = a_CameraUp;
        m_fSteer    = ((irr::f32)a_ControlX) / 127.0f;
        m_fThrottle = ((irr::f32)a_ControlY) / 127.0f;
        m_bBrake    = a_ControlBrake;

        m_cUpVector.normalize();
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
      m_iMarble     (a_pPlayer->m_pMarble->m_pPositional->getID()),
      m_iLapCnt     (a_iLapCnt),
      m_iPosition   (0),
      m_iPlayers    ((int)a_vRanking->size()),
      m_iLeader     (-1),
      m_iAhead      (-1),
      m_iCtrlHeight (0),
      m_fVel        (0.0f),
      m_fThrottle   (0.0f),
      m_fSteer      (0.0f),
      m_bShowSpeed  (false),
      m_bBrake      (false),
      m_bHightlight (true),
      m_bShowCtrl   (true),
      m_bShowRanking(true),
      m_bFadeStart  (false),
      m_cRect       (a_cRect),
      m_pGui        (a_pGui),
      m_pPlayer     (a_pPlayer),
      m_pDefFont    (nullptr),
      m_pDrv        (a_pGui->getVideoDriver()),
      m_pSpeedFont  (nullptr),
      m_pRankParent (nullptr),
      m_pColMgr     (nullptr),
      m_vRanking    (a_vRanking)
    {
      CGlobal *l_pGlobal = CGlobal::getInstance();

      m_pColMgr = l_pGlobal->getSceneManager()->getSceneCollisionManager();
      m_cScreen = m_pDrv->getScreenSize();

      irr::core::dimension2du l_cViewport = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      m_cArrowSize = l_cViewport / 50;

      if (m_cArrowSize.Width > m_cArrowSize.Height) m_cArrowSize.Height = m_cArrowSize.Width; else m_cArrowSize.Width = m_cArrowSize.Height;

      if (m_cArrowSize.Width  < 20) m_cArrowSize.Width  = 20;
      if (m_cArrowSize.Height < 20) m_cArrowSize.Height = 20;

      irr::video::SColor l_cBackground = irr::video::SColor( 128, 192, 192, 192);
      irr::video::SColor l_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

      irr::gui::IGUIFont *l_pTiny    = l_pGlobal->getFont(enFont::Tiny   , l_cViewport);
      irr::gui::IGUIFont *l_pSmall   = l_pGlobal->getFont(enFont::Small  , l_cViewport);
      irr::gui::IGUIFont *l_pRegular = l_pGlobal->getFont(enFont::Regular, l_cViewport);
      irr::gui::IGUIFont *l_pBig     = l_pGlobal->getFont(enFont::Big    , l_cViewport);
      irr::gui::IGUIFont *l_pHuge    = l_pGlobal->getFont(enFont::Huge   , l_cViewport);

      m_pDefFont = l_pSmall;
      m_cDefSize = getDimension(L"+666.6", m_pDefFont);

      m_pSpeedFont = l_pRegular;
      m_cSpeedText = getDimension(L"666", m_pSpeedFont);

      m_cSpeedTotal.Width  = a_cRect.getWidth() / 4;
      m_cSpeedTotal.Height = m_cSpeedText.Height;

      m_cSpeedBar.Width  = m_cSpeedTotal.Width - m_cSpeedText.Width;
      m_cSpeedBar.Height = m_cSpeedTotal.Height;

      m_cSpeedOffset.X = m_cSpeedTotal.Width - m_cSpeedBar.Width;

      m_cSpeedBar *= 5;
      m_cSpeedBar /= 6;

      m_cSpeedOffset.Y = (m_cSpeedTotal.Height - m_cSpeedBar.Height) / 2;

      m_iCtrlHeight = m_cSpeedTotal.Height / 4;
      if (m_iCtrlHeight < 2) m_iCtrlHeight = 2;

      irr::core::dimension2du l_cSizeTop = getDimension(L"Lap"    , l_pSmall);
      irr::core::dimension2du l_cSizeBot = getDimension(L"66 / 66", l_pBig  );

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
      m_mTextElements[enTextElements::Lap    ] = STextElement(irr::core::recti(l_cPos    , l_cSizeBot ), L"66 / 66", l_pBig   , l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::PosHead] = STextElement(irr::core::recti(l_cLapHead, l_cSizeTop ), L"Pos"    , l_pSmall  , l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Pos    ] = STextElement(irr::core::recti(l_cLap    , l_cSizeBot ), L"0"      , l_pBig    , l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
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

      m_aArrow[0] = a_pGui->getVideoDriver()->getTexture("data/images/arrow_leader.png");
      m_aArrow[1] = a_pGui->getVideoDriver()->getTexture("data/images/arrow_ahead.png" );

      for (int i = 0; i < 2; i++)
        if (m_aArrow[i] != nullptr) {
          m_aArrowSrc [i] = irr::core::recti(irr::core::position2di(0, 0), m_aArrow[i]->getOriginalSize());
          m_aArrOffset[i] = irr::core::position2di(m_aArrow[i]->getOriginalSize().Width / 2, m_aArrow[i]->getOriginalSize().Height);
          m_aTxtOffset[i] = irr::core::position2di(m_cDefSize.Width / 2, m_aArrowSrc->getHeight() + (6 * m_cDefSize.Height / 5));
        }

      for (int i = 0; i < 16; i++) {
        m_aRanking[i] = nullptr;
        m_aRostrum[i] = false;
      }

      m_pRankParent = a_pGui->addTab(a_cRect, a_pGui->getRootGUIElement());
      m_pRankParent->setDrawBackground(true);
      m_pRankParent->setBackgroundColor(irr::video::SColor(96, 192, 192, 192));
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

      l_cRankPos.Y -= m_iPlayers * 3 * l_cRankSize.Height / 4;

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

        m_aRanking[i]->setData(helpers::s2ws((*m_vRanking)[i]->m_sName), 0);
        m_aRanking[i]->drop();

        if (i == (m_iPlayers / 2) - 1)
          l_iOffsetY = 0;
        else
          l_iOffsetY += 3 * l_cRankSize.Height / 2;
      }
    }

    CGameHUD::~CGameHUD() {
    }

    void CGameHUD::draw() {
      if (m_bShowSpeed) {
        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.render();

        if (m_pColMgr != nullptr) {
          if (m_bHightlight) {
            if (m_aArrow[0] != nullptr) {
              if (m_pPlayer->m_iDiffLeader > 60 && m_mMarblePositions.find(m_iLeader) != m_mMarblePositions.end()) {
                irr::core::vector2di l_cPos = m_pColMgr->getScreenCoordinatesFrom3DPosition(m_mMarblePositions[m_iLeader] + 2.5f * m_cUpVector, m_pPlayer->m_pMarble->m_pViewport->m_pCamera);
                l_cPos.X = m_cRect.UpperLeftCorner.X + m_cRect.getWidth () * l_cPos.X / m_cScreen.Width;
                l_cPos.Y = m_cRect.UpperLeftCorner.Y + m_cRect.getHeight() * l_cPos.Y / m_cScreen.Height;

                if (m_pPlayer->m_iDiffLeader > 180) {
                  m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), irr::core::recti(l_cPos - m_aTxtOffset[0], m_cDefSize), &m_cRect);
                }

                m_pDrv->draw2DImage(m_aArrow[0], irr::core::recti(l_cPos - m_aArrOffset[0], m_cArrowSize), m_aArrowSrc[0], &m_cRect, nullptr, true);
          
                if (m_pPlayer->m_iDiffLeader > 180) {
                  wchar_t s[0xFF];
                  swprintf(s, 0xFF, L"-%.2f", ((irr::f32)m_pPlayer->m_iDiffLeader) / 120.0f);
                  m_pDefFont->draw(s, irr::core::recti(l_cPos - m_aTxtOffset[0], m_cDefSize), irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cRect);
                }
              }
            }

            if (m_aArrow[1] != nullptr) {
              if (m_pPlayer->m_iDiffAhead > 60 && m_mMarblePositions.find(m_iAhead) != m_mMarblePositions.end()) {
                irr::core::vector2di l_cPos = m_pColMgr->getScreenCoordinatesFrom3DPosition(m_mMarblePositions[m_iAhead] + 2.5f * m_cUpVector, m_pPlayer->m_pMarble->m_pViewport->m_pCamera);
                l_cPos.X = m_cRect.UpperLeftCorner.X + m_cRect.getWidth () * l_cPos.X / m_cScreen.Width;
                l_cPos.Y = m_cRect.UpperLeftCorner.Y + m_cRect.getHeight() * l_cPos.Y / m_cScreen.Height;

                if (m_pPlayer->m_iDiffAhead > 180) {
                  m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), irr::core::recti(l_cPos - m_aTxtOffset[1], m_cDefSize), &m_cRect);
                }

                m_pDrv->draw2DImage(m_aArrow[1], irr::core::recti(l_cPos - m_aArrOffset[1], m_cArrowSize), m_aArrowSrc[1], &m_cRect, nullptr, true);

                if (m_pPlayer->m_iDiffAhead > 180) {
                  wchar_t s[0xFF];
                  swprintf(s, 0xFF, L"-%.2f", ((irr::f32)m_pPlayer->m_iDiffAhead) / 120.0f);
                  m_pDefFont->draw(s, irr::core::recti(l_cPos - m_aTxtOffset[1], m_cDefSize), irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cRect);
                }
              }
            }
          }

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

          if (m_bShowRanking && m_pPlayer->m_iPosition > 0) {
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
              m_pPlayer->m_iPosition == 1 ? 2 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 2 : m_pPlayer->m_iPosition - 1,
              m_pPlayer->m_iPosition == 1 ? 3 : m_pPlayer->m_iPosition == m_vRanking->size() ? (int)m_vRanking->size() - 1 : m_pPlayer->m_iPosition,
              m_pPlayer->m_iPosition == 1 ? 4 : m_pPlayer->m_iPosition == m_vRanking->size() ? m_pPlayer->m_iPosition      : m_pPlayer->m_iPosition + 1
            };

            for (int i = 0; i < 4 && i < m_vRanking->size(); i++) {
              m_pDrv->draw2DRectangle(irr::video::SColor(192, 232, 232, 232), l_cRects[i], &m_cRect);
              if (l_iPos[i] == m_pPlayer->m_iPosition) {
                std::wstring l_sText = L" " + std::to_wstring(m_pPlayer->m_iPosition) + L": " + helpers::s2ws(m_pPlayer->m_sName);
                wchar_t s[0xFF];
                m_pDefFont->draw(l_sText.c_str(), l_cRects[i], irr::video::SColor(0xFF, 0, 0, 0), false, true, &l_cRects[i]);
              }
              else {
                for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
                  if ((*it)->m_iPosition == l_iPos[i]) {
                    std::wstring l_sText = L" " + std::to_wstring((*it)->m_iPosition) + L": " + helpers::s2ws((*it)->m_sName).c_str();
                    m_pDefFont->draw(l_sText.c_str(), l_cRects[i], irr::video::SColor(0xFF, 0, 0, 0), false, true, & l_cRects[i]);
                    
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
                        l_sDeficit = getDeficitString((*it)->m_iDiffLeader);
                      }
                      else {
                        l_sDeficit = getDeficitString((*it)->m_iDiffAhead);
                      }
                    }

                    irr::core::dimension2du l_cSize = m_pDefFont->getDimension(l_sDeficit.c_str());
                    m_pDefFont->draw(l_sDeficit.c_str(), irr::core::recti(l_cRects[i].LowerRightCorner.X - l_cSize.Width, l_cRects[i].UpperLeftCorner.Y, l_cRects[i].LowerRightCorner.X, l_cRects[i].LowerRightCorner.Y), irr::video::SColor(0xFF, 0, 0, 0), true, true, &l_cRects[i]);
                    break;
                  }
                }
              }
            }
          }
        }
      }
    }

    std::wstring CGameHUD::getDeficitString(int a_iDeficit) {
      wchar_t s[0xFF] = L"";

      if (a_iDeficit > 0) {
        swprintf(s, 0xFF, L"%.2f ", ((irr::s32)a_iDeficit) / 120.0f);
      }
      else if (a_iDeficit < 0) {
        swprintf(s, 0xFF, L"+%i Lap%s ", -a_iDeficit, a_iDeficit == - 1 ? L"" : L"s");
      }

      return s;
    }

    void CGameHUD::updateRanking() {
      for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if ((*it)->m_iId == m_iMarble) {
          if (m_mTextElements.find(enTextElements::Pos) != m_mTextElements.end()) {
            m_mTextElements[enTextElements::Pos].m_sText = std::to_wstring((*it)->m_iPosition);
          }

          if (it != m_vRanking->begin()) {
            if (m_iLeader != -2)
              m_iLeader = (*m_vRanking->begin())->m_iId;

            if (it - 1 != m_vRanking->begin()) {
              if (m_iAhead != -2) m_iAhead = (*(it - 1))->m_iId;
            }
            else m_iAhead = -1;
          }
          else m_iLeader = -1;

          break;
        }
      }

      int l_iIndex = 0;
      for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        if (m_aRanking[l_iIndex] != nullptr) {
          m_aRanking[l_iIndex]->setData(helpers::s2ws((*it)->m_sName), (*it)->m_iDiffLeader);
        }
        l_iIndex++;
      }
    }

    void CGameHUD::setSettings(bool a_bHightlight, bool a_bShowCtrl, bool a_bShowRanking) {
      m_bHightlight  = a_bHightlight;
      m_bShowCtrl    = a_bShowCtrl;
      m_bShowRanking = a_bShowRanking;
    }
  }
}
