// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gameclasses/SPlayer.h>
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
      m_eAlignH    (irr::gui::EGUIA_UPPERLEFT),
      m_eAlignV    (irr::gui::EGUIA_UPPERLEFT),
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
    }

    /**
    * This function receives messages of type "PlayerFinished"
    * @param a_MarbleId ID of the finished marble
    * @param a_RaceTime Racetime of the finished player in simulation steps
    * @param a_Laps The number of laps the player has done
    */
    void CGameHUD::onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {
      if (a_MarbleId == m_iMarble) {
        for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
          it->second.m_bVisible = false;
      }
    }

    /**
    * This function receives messages of type "PlayerStunned"
    * @param a_MarbleId ID of the marble
    * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
    */
    void CGameHUD::onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) {
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
    }


    irr::core::dimension2du CGameHUD::getDimension(const std::wstring& s, irr::gui::IGUIFont *a_pFont) {
      irr::core::dimension2du l_cDim = a_pFont->getDimension(s.c_str());
      l_cDim.Width  = 3 * l_cDim.Width  / 2;
      l_cDim.Height = 3 * l_cDim.Height / 2;
      return l_cDim;
    }

    CGameHUD::CGameHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti& a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment* a_pGui, std::vector<gameclasses::SPlayer *> *a_vRanking) : 
      IGUIElement(irr::gui::EGUIET_COUNT, a_pGui, a_pGui->getRootGUIElement(), -1, a_cRect),
      m_iMarble  (a_pPlayer->m_pMarble->m_pPositional->getID()),
      m_iLapCnt  (a_iLapCnt),
      m_iPosition(0),
      m_iPlayers ((int)a_vRanking->size()),
      m_cRect    (a_cRect),
      m_pGui     (a_pGui),
      m_pPlayer  (a_pPlayer),
      m_vRanking (a_vRanking)
    {
      CGlobal *l_pGlobal = CGlobal::getInstance();

      irr::core::dimension2du l_cViewport = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      irr::gui::IGUIFont *l_pTiny    = l_pGlobal->getFont(enFont::Tiny   , l_cViewport);
      irr::gui::IGUIFont *l_pSmall   = l_pGlobal->getFont(enFont::Small  , l_cViewport);
      irr::gui::IGUIFont *l_pRegular = l_pGlobal->getFont(enFont::Regular, l_cViewport);
      irr::gui::IGUIFont *l_pBig     = l_pGlobal->getFont(enFont::Big    , l_cViewport);
      irr::gui::IGUIFont *l_pHuge    = l_pGlobal->getFont(enFont::Huge   , l_cViewport);

      std::wstring s = helpers::s2ws(a_pPlayer->m_sName);
      irr::core::dimension2du l_cDim = getDimension(s, l_pRegular);

      irr::video::SColor l_cBackground = irr::video::SColor( 128, 192, 192, 192);
      irr::video::SColor l_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

      m_mTextElements[enTextElements::Name] = STextElement(irr::core::recti(a_cRect.UpperLeftCorner, l_cDim), s, l_pRegular, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Name].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Name].m_eAlignV = irr::gui::EGUIA_CENTER;

      irr::core::dimension2du d1 = getDimension(L"66/66"               , l_pBig  );
      irr::core::dimension2du d2 = getDimension(L"Position"            , l_pTiny );
      irr::core::dimension2du d3 = getDimension(L"ReallyLongPlayerName", l_pSmall);
      irr::core::dimension2du d4 = getDimension(L"+666.66 sec"         , l_pSmall);

      if (d1.Width < d2.Width)
        d1.Width = d2.Width;
      else
        d2.Width = d1.Width;

      if (d1.Height + d2.Height < 2 * d3.Height) {
        d2.Height = 2 * d3.Height - d2.Height;
      }
      else {
        d3.Height = (d1.Height + d2.Height) / 2;
      }

      irr::core::position2di l_cPos = irr::core::position2di(a_cRect.getCenter().X, a_cRect.UpperLeftCorner.Y);
      l_cPos.X -= (d3.Width + d2.Width) / 2 + d1.Width;

      irr::core::recti l_cRects[] = {
        irr::core::recti(l_cPos                                                                    , d2), // 0: Position Header
        irr::core::recti(l_cPos + irr::core::position2di(                             0, d2.Height), d1), // 1: Position
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width                      ,         0), d3), // 2: Marble Ahead
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width                      , d3.Height), d3), // 3: Marble Behind
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width + d3.Width           , 0        ), d4), // 4: Time diff ahead
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width + d3.Width           , d3.Height), d4), // 5: Time diff behind
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width + d3.Width + d4.Width, 0        ), d2), // 6: Lap Header
        irr::core::recti(l_cPos + irr::core::position2di(d1.Width + d3.Width + d4.Width, d2.Height), d1)  // 7: Lap
      };

      // Adjust the lower Y values to match
      if (l_cRects[3].LowerRightCorner.Y < l_cRects[1].LowerRightCorner.Y)
        l_cRects[3].LowerRightCorner.Y = l_cRects[1].LowerRightCorner.Y;
      else
        l_cRects[1].LowerRightCorner.Y = l_cRects[3].LowerRightCorner.Y;

      if (l_cRects[4].LowerRightCorner.Y < l_cRects[2].LowerRightCorner.Y)
        l_cRects[4].LowerRightCorner.Y = l_cRects[2].LowerRightCorner.Y;
      else
        l_cRects[2].LowerRightCorner.Y = l_cRects[4].LowerRightCorner.Y;

      if (l_cRects[5].LowerRightCorner.Y < l_cRects[1].LowerRightCorner.Y)
        l_cRects[5].LowerRightCorner.Y = l_cRects[1].LowerRightCorner.Y;
      else
        l_cRects[1].LowerRightCorner.Y = l_cRects[5].LowerRightCorner.Y;

      m_mTextElements[enTextElements::PosHead] = STextElement(l_cRects[0], L"Position", l_pTiny, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::PosHead].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::PosHead].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::Pos] = STextElement(l_cRects[1], L"1", l_pBig, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Pos].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Pos].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::Ahead] = STextElement(l_cRects[2], L"Marble in front", l_pSmall, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Ahead].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::Behind] = STextElement(l_cRects[3], L"Marble behind", l_pSmall, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Behind].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::TimeAhead] = STextElement(l_cRects[4], L"-666.66 sec", l_pSmall, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::TimeAhead].m_eAlignH = irr::gui::EGUIA_LOWERRIGHT;
      m_mTextElements[enTextElements::TimeAhead].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::TimeBehind] = STextElement(l_cRects[5], L"+666.66 sec", l_pSmall, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::TimeBehind].m_eAlignH = irr::gui::EGUIA_LOWERRIGHT;
      m_mTextElements[enTextElements::TimeBehind].m_eAlignV = irr::gui::EGUIA_CENTER;


      m_mTextElements[enTextElements::LapHead] = STextElement(l_cRects[6], L"Lap", l_pTiny, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::LapHead].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::LapHead].m_eAlignV = irr::gui::EGUIA_CENTER;

      m_mTextElements[enTextElements::Lap] = STextElement(l_cRects[7], L"66/66", l_pBig, l_cBackground, l_cTextColor, a_pGui->getVideoDriver());
      m_mTextElements[enTextElements::Lap].m_eAlignH = irr::gui::EGUIA_CENTER;
      m_mTextElements[enTextElements::Lap].m_eAlignV = irr::gui::EGUIA_CENTER;

      for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
        it->second.m_bVisible = false;

      irr::core::dimension2du l_cRankNr = l_pBig->getDimension(L"66");
      if (l_cRankNr.Width > l_cRankNr.Height) l_cRankNr.Height = l_cRankNr.Width; else l_cRankNr.Width = l_cRankNr.Height;

      irr::core::dimension2du l_cRankName = l_pRegular->getDimension(L"ThisNameIsLongEngough");
      irr::core::dimension2du l_cRankDiff = l_pTiny   ->getDimension(L"+1234 sec"            );

      l_cRankDiff.Width = l_cRankName.Width;

      if (l_cRankNr.Height > l_cRankName.Height + l_cRankDiff.Height) {
        l_cRankName.Height = l_cRankNr.Height - l_cRankDiff.Height;
      }
      else {
        l_cRankNr.Height = l_cRankName.Height + l_cRankDiff.Height;
        if (l_cRankNr.Width > l_cRankNr.Height) l_cRankNr.Height = l_cRankNr.Width; else l_cRankNr.Width = l_cRankNr.Height;
      }

      irr::core::dimension2du l_cRankTotal = irr::core::dimension2du(l_cRankNr.Width + l_cRankName.Width, l_cRankNr.Height);

      irr::core::position2di l_cPosition = irr::core::position2di(
        a_cRect.LowerRightCorner.X - l_cRankTotal.Width,
        a_cRect.getCenter().Y - (m_iPlayers * (5 * l_cRankTotal.Height / 8))
      );
    }

    CGameHUD::~CGameHUD() {
    }

    void CGameHUD::draw() {
      for (std::map<enTextElements, STextElement>::iterator it = m_mTextElements.begin(); it != m_mTextElements.end(); it++)
        it->second.render();
    }

    void CGameHUD::updateRanking() {
      // printf("**************\n");
      // printf("Ranking update\n");
      // printf("**************\n");
      for (std::vector<gameclasses::SPlayer*>::const_iterator it = m_vRanking->begin(); it != m_vRanking->end(); it++) {
        // printf("%i: %s (%.2f, %.2f)\n", (*it)->m_iPosition, (*it)->m_sName.c_str(), (irr::f32)((*it)->m_iDiffLeader) / 120.0f, (irr::f32)((*it)->m_iDiffAhead) / 120.0f);
        if ((*it)->m_iId == m_iMarble) {
          m_mTextElements[enTextElements::Pos].m_sText = std::to_wstring((*it)->m_iPosition);
          m_iPosition = (*it)->m_iPosition;

          if (it == m_vRanking->begin()) {
            m_mTextElements[enTextElements::Ahead].m_sText       = L"";
            m_mTextElements[enTextElements::Ahead].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::Ahead].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

            m_mTextElements[enTextElements::TimeAhead].m_sText = L"";
            m_mTextElements[enTextElements::TimeAhead].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::TimeAhead].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);
          }
          else {
            m_mTextElements[enTextElements::Ahead].m_sText       = helpers::s2ws((*(it - 1))->m_sName);
            m_mTextElements[enTextElements::Ahead].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::Ahead].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

            wchar_t s[0xFF];
            // swprintf_s<0xFF>(s, L"-%.2f sec.", (irr::f32)((*it)->m_iDiffAhead) / 120.0f);
            swprintf(s, 0xFF, L"-%.2f sec.", (irr::f32)((*it)->m_iDiffAhead) / 120.0f);

            m_mTextElements[enTextElements::TimeAhead].m_sText = s;
            m_mTextElements[enTextElements::TimeAhead].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::TimeAhead].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);
          }

          if (it + 1 == m_vRanking->end()) {
            m_mTextElements[enTextElements::Behind].m_sText       = L"";
            m_mTextElements[enTextElements::Behind].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::Behind].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

            m_mTextElements[enTextElements::TimeBehind].m_sText = L"";
            m_mTextElements[enTextElements::TimeBehind].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::TimeBehind].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);
          }
          else {
            m_mTextElements[enTextElements::Behind].m_sText       = helpers::s2ws((*(it + 1))->m_sName);
            m_mTextElements[enTextElements::Behind].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::Behind].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);

            wchar_t s[0xFF] = L"";
            // swprintf_s<0xFF>(s, L"+%.2f sec.", (irr::f32)((*(it + 1))->m_iDiffAhead) / 120.0f);
            swprintf(s, 0xFF, L"+%.2f sec.", (irr::f32)((*(it + 1))->m_iDiffAhead) / 120.0f);

            m_mTextElements[enTextElements::TimeBehind].m_sText = s;
            m_mTextElements[enTextElements::TimeBehind].m_cBackground = irr::video::SColor( 128, 192, 192, 192);
            m_mTextElements[enTextElements::TimeAhead].m_cTextColor  = irr::video::SColor(0xFF,   0,   0,   0);
          }
        }
      }
    }
  }
}
