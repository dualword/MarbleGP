// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudLapTimes.h>
#include <helpers/CStringHelpers.h>

namespace dustbin {
  namespace gui {
    /**
    * The constructor
    * @param a_cPos the position (upper right corner as the item is drawn in the upper right of the screen)
    * @param a_pDrv the Irrlicht video driver
    * @param a_pGui the Irrlicht GUI environment
    * @param a_pFont the font to use for rendering
    * @param a_pPlayers the players
    */
    CHudLapTimes::CHudLapTimes(
      irr::video::IVideoDriver *a_pDrv, 
      int a_iMarbleId, 
      const irr::core::vector2di &a_cPos, 
      irr::gui::IGUIFont *a_pFont, 
      const std::vector<gameclasses::SRaceData *> &a_vPlayers
    ) : 
      m_pDrv     (a_pDrv),
      m_pFont    (a_pFont),
      m_iOffset  (-1),
      m_iLastCp  (-1),
      m_iStartLap(-1),
      m_iLastLap (-1),
      m_iBestLap (-1),
      m_iBestOld (-1),
      m_iFinished(-1),
      m_bFirstLap(true),
      m_sBest    (L"-"),
      m_iMarble  (a_iMarbleId),
      m_pPlayer  (nullptr),
      m_pBest    (nullptr)
    {
      for (int i = 0; i < 16; i++)
        m_aLapStart[i] = -1;

      // Checkpoint:
      // Racetime:
      // Last Lap:
      // Best Lap:
      m_cSizeCol1 = m_pFont->getDimension(L" Checkpoint: ");
      m_cSizeCol1.Height = 5 * m_cSizeCol1.Height / 4;

      m_cSizeCol2 = m_pFont->getDimension(L"666:66.666 ");
      m_cSizeCol2.Height = 5 * m_cSizeCol2.Height / 4;

      m_cSizeTotal.Width  = m_cSizeCol1.Width + m_cSizeCol2.Width;
      m_cSizeTotal.Height = m_cSizeCol1.Height > m_cSizeCol2.Height ? m_cSizeCol1.Height : m_cSizeCol2.Height;

      m_cPos  = a_cPos;
      m_cPos.X -= m_cSizeTotal.Width;

      m_iOffset = 5 * m_cSizeTotal.Height / 4;

      for (int i = 0; i < 16; i++)
        m_aPlayers[i] = nullptr;

      for (auto l_pPlayer : a_vPlayers) {
        m_aPlayers[l_pPlayer->m_iMarble - 10000] = l_pPlayer;

        if (l_pPlayer->m_iMarble == a_iMarbleId)
          m_pPlayer = l_pPlayer;
      }
    }

    CHudLapTimes::~CHudLapTimes() {
    }

    /**
    * Convert a number of steps to a time string
    * @param a_iSteps the number of steps to convert
    * @return string representation of the time
    */
    std::wstring CHudLapTimes::convertToTime(irr::s32 a_iSteps, bool a_bSign) {
      irr::s32 l_iTime = (irr::s32)(a_iSteps / 1.2f);

      bool l_bNegative = l_iTime < 0;

      irr::s32 l_iHundrts = std::abs(l_iTime % 100); l_iTime /= 100;
      irr::s32 l_iSeconds = std::abs(l_iTime %  60); l_iTime /=  60;
      irr::s32 l_iMinutes = std::abs(l_iTime);

      std::wstring l_sHundrts = std::to_wstring(l_iHundrts); while (l_sHundrts.size() < 2) l_sHundrts = L"0" + l_sHundrts;
      std::wstring l_sSeconds = std::to_wstring(l_iSeconds); if (l_iMinutes > 0) while (l_sSeconds.size() < 2) l_sSeconds = L"0" + l_sSeconds;
      std::wstring l_sMinutes = std::to_wstring(l_iMinutes);

      std::wstring l_sRet = L"";

      if (l_iMinutes > 0)
        l_sRet = l_sMinutes + L":";

      l_sRet = l_sRet + l_sSeconds + L"." + l_sHundrts;

      if (l_bNegative)
        l_sRet = L"-" + l_sRet;
      else if (a_bSign)
        l_sRet = L"+" + l_sRet;

      return l_sRet;
    }

    /**
    * A marble has started a new lap
    * @param a_iStep the simulation step
    * @param a_iMarble ID of the marble
    * @param a_iLapNo the current lap
    */
    void CHudLapTimes::onLapStart(int a_iStep, int a_iMarble, int a_iLapNo) {
      int l_iIndex = a_iMarble - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16) {
        if (a_iMarble == m_iMarble) {
          if (m_aLapStart[l_iIndex] != -1) {
            m_iLastLap  = a_iStep - m_aLapStart[l_iIndex];

            // if (m_iBestLap != -1) {
            //   wprintf(L"%s / %s ==> %s\n", convertToTime(m_iLastLap, false).c_str(), convertToTime(m_iBestLap, false).c_str(), convertToTime(m_iLastLap - m_iBestLap, true).c_str());
            // }
          }
          m_iStartLap = a_iStep;
        }

        if ((m_bFirstLap && a_iLapNo > 1) || a_iStep - m_aLapStart[l_iIndex] < m_iBestLap) {
          m_vBest = m_aSplits[l_iIndex];
          m_iBestOld = m_iBestLap;
          m_iBestLap = a_iStep - m_aLapStart[l_iIndex];
          m_pBest = m_aPlayers[l_iIndex]->m_pPlayer;
          m_sBest = m_pBest->m_sWName;

          if (m_sBest.find_last_of(L'|') != std::wstring::npos)
            m_sBest = m_sBest.substr(0, m_sBest.find_last_of(L'|'));
        }

        m_aLapStart[l_iIndex] = a_iStep;
        m_aSplits  [l_iIndex].clear();
        m_bFirstLap = a_iLapNo <= 1;
      }
    }

    /**
    * A player has finished
    * @param a_iRaceTime the marble's race time in steps
    * @param a_iMarble ID of the marble
    */
    void CHudLapTimes::onPlayerFinished(int a_iRaceTime, int a_iMarble) {
      if (a_iMarble == m_iMarble)
        m_iFinished = a_iRaceTime;
    }

    /**
    * A marble has passed a checkpoint
    * @param a_iStep the simulation step
    * @param a_iMarble ID of the marble
    */
    void CHudLapTimes::onCheckpoint(int a_iStep, int a_iMarble) {
      if (a_iMarble == m_iMarble)
        m_iLastCp = a_iStep;

      int l_iIndex = a_iMarble - 10000;
      if (m_aLapStart[l_iIndex] != -1 && m_aLapStart[l_iIndex] != a_iStep) {
        m_aSplits[l_iIndex].push_back(a_iStep - m_aLapStart[l_iIndex]);
        if (a_iMarble == m_iMarble) {
        }

        if (m_bFirstLap) {
          if (m_aSplits[l_iIndex].size() > m_vBest.size()) {
            m_vBest.push_back(m_aSplits[l_iIndex].back());
          }
        }
      }
    }

    /**
    * Render the lap times list
    * @param a_iStep the simulation step
    * @param a_cClip the clipping rectangle
    */
    void CHudLapTimes::render(int a_iStep, const irr::core::recti &a_cClip) {
      irr::core::position2di l_cLapTimePos = m_cPos;

#ifdef _ANDROID
      l_cLapTimePos.Y += m_iOffset;
#endif

      std::vector<std::tuple<irr::core::position2di, std::wstring, std::wstring, irr::video::SColor, irr::video::SColor>> l_vRows;

      l_vRows.push_back(std::make_tuple(
        l_cLapTimePos, 
        L" Racetime: ", 
        convertToTime(m_iFinished == -1 ? a_iStep : m_iFinished, false), 
        irr::video::SColor(128, 255, 255, 192), 
        irr::video::SColor(255,   0,   0,   0)
      ));

      int l_iIndex = m_iMarble - 10000;

      if (m_pPlayer != nullptr && l_iIndex >= 0 && l_iIndex < 16) {
        int l_iTime    = m_aSplits[l_iIndex].size() > 0 ? m_aSplits[l_iIndex].back() : 0;
        int l_iLapTime = a_iStep - m_iStartLap;

        if (m_iStartLap != -1) {
          l_cLapTimePos.Y += m_iOffset;

          if (l_iTime > 1 && m_iLastCp >= 0 && a_iStep - m_iLastCp < 120) {
            int l_iDiff = m_vBest.size() >= m_aSplits[l_iIndex].size() ? m_aSplits[l_iIndex].back() - m_vBest[m_aSplits[l_iIndex].size() - 1] : 0;
            
            irr::video::SColor l_cText = l_iDiff > 0 ? irr::video::SColor(255, 255, 255, 128) : l_iDiff < 0 ? irr::video::SColor(255, 128, 255, 128) : irr::video::SColor(255, 32, 32, 32);

            l_vRows.push_back(std::make_tuple(
              l_cLapTimePos,
              L" Checkpoint: ",
              convertToTime(l_iTime, false),
              irr::video::SColor(128, 192, 192, 255),
              l_cText
            ));
          }
          else if (l_iTime > 1 && m_iLastCp >= 0 && a_iStep - m_iLastCp < 240) {
            int l_iDiff = m_vBest.size() >= m_aSplits[l_iIndex].size() ? m_aSplits[l_iIndex].back() - m_vBest[m_aSplits[l_iIndex].size() - 1] : 0;

            irr::video::SColor l_cText = l_iDiff > 0 ? irr::video::SColor(255, 255, 255, 128) : l_iDiff < 0 ? irr::video::SColor(255, 128, 255, 128) : irr::video::SColor(255, 32, 32, 32);

            l_vRows.push_back(std::make_tuple(
              l_cLapTimePos,
              L" Deficit: ",
              convertToTime(l_iDiff, true),
              irr::video::SColor(128, 192, 192, 255),
              l_cText
            ));
          }
          else {
            l_vRows.push_back(std::make_tuple(
              l_cLapTimePos,
              L" This Lap: ",
              m_iFinished == -1 ?  convertToTime(l_iLapTime, false) : L"Finished",
              irr::video::SColor(128, 192, 192, 255),
              irr::video::SColor(255,   0,   0,   0)
            ));
          }

          l_cLapTimePos.Y += m_iOffset;

          if (a_iStep - m_iStartLap < 240 && (m_iLastLap > m_iBestLap || m_iBestOld != -1)) {
            int l_iDiff = m_iLastLap > m_iBestLap ? m_iLastLap - m_iBestLap : m_iLastLap - m_iBestOld;

            irr::video::SColor l_cText = l_iDiff > 0 ? irr::video::SColor(255, 255, 255, 128) : l_iDiff < 0 ? irr::video::SColor(255, 128, 255, 128) : irr::video::SColor(255, 32, 32, 32);

            l_vRows.push_back(std::make_tuple(
              l_cLapTimePos,
              L" Deficit: ",
              convertToTime(l_iDiff, true),
              irr::video::SColor(128, 192, 192, 255),
              l_cText
            ));
          }
          else {
            l_vRows.push_back(std::make_tuple(
              l_cLapTimePos,
              L" Last Lap: ",
              m_iLastLap == -1 ? L"-" : convertToTime(m_iLastLap, false),
              irr::video::SColor(128, 192, 192, 255),
              (m_iLastLap == m_iBestLap && m_iLastLap != -1) ? irr::video::SColor(255, 128, 255, 128) : irr::video::SColor(255, 0, 0, 0)
            ));
          }
        }
      }

      for (auto l_tRow : l_vRows) {
        m_pDrv->draw2DRectangle(std::get<3>(l_tRow), irr::core::recti(std::get<0>(l_tRow), m_cSizeTotal));

        irr::core::dimension2du l_cSize = m_pFont->getDimension(std::get<1>(l_tRow).c_str());
        irr::core::recti l_cRect = irr::core::recti(
          std::get<0>(l_tRow) + irr::core::vector2di(m_cSizeCol1.Width - l_cSize.Width, 0), irr::core::dimension2du(l_cSize.Width, m_cSizeTotal.Height)
        );
        m_pFont->draw(std::get<1>(l_tRow).c_str(), l_cRect, std::get<4>(l_tRow), false, true);

        l_cSize = m_pFont->getDimension(std::get<2>(l_tRow).c_str());
        l_cSize.Width += m_cSizeTotal.Height / 4;

        l_cRect = irr::core::recti(
          std::get<0>(l_tRow) + irr::core::vector2di(m_cSizeTotal.Width - l_cSize.Width, 0), irr::core::dimension2du(l_cSize.Width, m_cSizeTotal.Height)
        );

        m_pFont->draw(std::get<2>(l_tRow).c_str(), l_cRect, std::get<4>(l_tRow), false, true);
      }

      if (m_iBestLap != -1 && m_pBest != nullptr) {
        l_cLapTimePos.Y += m_iOffset;

        irr::video::SColor l_cText = m_pBest->m_cText;
        irr::video::SColor l_cBack = m_pBest->m_cBack; l_cBack.setAlpha(192);
        irr::video::SColor l_cFrme = m_pBest->m_cFrme;

        m_pDrv->draw2DRectangle(l_cBack, irr::core::recti(l_cLapTimePos, m_cSizeTotal));
        m_pDrv->draw2DRectangleOutline(irr::core::recti(l_cLapTimePos, m_cSizeTotal), l_cFrme);

        irr::core::dimension2du l_cSize = m_pFont->getDimension(L" Best Lap: ");

        m_pFont->draw(
          L" Best Lap: ", 
          irr::core::recti(l_cLapTimePos + irr::core::vector2di(m_cSizeCol1.Width - l_cSize.Width, 0), m_cSizeCol1), 
          l_cText, 
          false, 
          true
        );

        std::wstring s = convertToTime(m_iBestLap, false);
        l_cSize = m_pFont->getDimension(s.c_str());
        l_cSize.Width += m_cSizeTotal.Height / 4;

        m_pFont->draw(
          s.c_str(), 
          irr::core::recti(l_cLapTimePos + irr::core::vector2di(m_cSizeTotal.Width - l_cSize.Width, 0), m_cSizeCol2),
          l_cText,
          false,
          true
        );
        

        l_cLapTimePos.Y += m_iOffset;

        irr::core::recti l_cRectNumber = irr::core::recti(l_cLapTimePos, irr::core::dimension2du(m_cSizeTotal.Height, m_cSizeTotal.Height));
        irr::core::recti l_cRectName   = irr::core::recti(
          l_cLapTimePos + irr::core::vector2di(m_cSizeTotal.Height, 0),
          irr::core::dimension2du(m_cSizeTotal.Width - m_cSizeTotal.Height, m_cSizeTotal.Height)
        );

        m_pDrv->draw2DRectangle(l_cBack, l_cRectNumber);
        m_pDrv->draw2DRectangle(l_cBack, l_cRectName);
        
        m_pDrv->draw2DRectangleOutline(l_cRectNumber, l_cFrme);
        m_pDrv->draw2DRectangleOutline(l_cRectName  , l_cFrme);

        m_pFont->draw(m_pBest->m_sNumber.c_str(), l_cRectNumber, l_cText, true, true);
        m_pFont->draw(m_sBest           .c_str(), l_cRectName  , l_cText, true, true);
      }
    }
  }
}