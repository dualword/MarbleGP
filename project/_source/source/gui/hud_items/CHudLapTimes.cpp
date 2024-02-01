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
      m_pDrv          (a_pDrv),
      m_pFont         (a_pFont),
      m_iBestLapTime  (-1),
      m_iPersonalBest (-1),
      m_iLastLapTime  (-1),
      m_iLapTimeOffset(-1),
      m_iFinishStep   (-1),
      m_iMarbleID     (a_iMarbleId),
      m_iLastCpTime   (-1),
      m_iLapStart     (-1),
      m_iCpNo         (-1),
      m_sBestLap      (L""),
      m_pBestLap      (nullptr)
    {
      // Race:
      // Lap:
      // Last:
      // Fastest:
      m_cSizeCol1 = m_pFont->getDimension(L" Fastest: ");
      m_cSizeCol1.Width  = 5 * m_cSizeCol1.Width  / 4;
      m_cSizeCol1.Height = 5 * m_cSizeCol1.Height / 4;

      m_cSizeCol2 = m_pFont->getDimension(L" 66:66.66 ");
      m_cSizeCol2.Width  = 5 * m_cSizeCol2.Width  / 4;
      m_cSizeCol2.Height = 5 * m_cSizeCol2.Height / 4;

      m_cSizeTotal.Width  = m_cSizeCol1.Width + m_cSizeCol2.Width;
      m_cSizeTotal.Height = m_cSizeCol1.Height > m_cSizeCol2.Height ? m_cSizeCol1.Height : m_cSizeCol2.Height;

      m_cPos  = a_cPos;
      m_cPos.X -= m_cSizeTotal.Width;

      m_iLapTimeOffset = 5 * m_cSizeTotal.Height / 4;

      for (auto l_pPlayer : a_vPlayers) {
        std::wstring l_sName = l_pPlayer->m_pPlayer->m_sWName;

        if (l_sName.find_last_not_of(L'|') != std::wstring::npos) {
          l_sName = l_sName.substr(0, l_sName.find_last_of(L'|'));
        }

        m_mPlayers[l_pPlayer->m_iMarble] = l_sName;
      }

      printf("Ready.\n");
    }

    CHudLapTimes::~CHudLapTimes() {
    }

    /**
    * Convert a number of steps to a time string
    * @param a_iSteps the number of steps to convert
    * @return string representation of the time
    */
    std::wstring CHudLapTimes::convertToTime(irr::s32 a_iSteps) {
      irr::s32 l_iTime = (irr::s32)(a_iSteps / 1.2f);

      irr::s32 l_iHundrts = l_iTime % 100; l_iTime /= 100;
      irr::s32 l_iSeconds = l_iTime %  60; l_iTime /=  60;
      irr::s32 l_iMinutes = l_iTime;

      std::wstring l_sHundrts = std::to_wstring(l_iHundrts); while (l_sHundrts.size() < 2) l_sHundrts = L"0" + l_sHundrts;
      std::wstring l_sSeconds = std::to_wstring(l_iSeconds); if (l_iMinutes > 0) while (l_sSeconds.size() < 2) l_sSeconds = L"0" + l_sSeconds;
      std::wstring l_sMinutes = std::to_wstring(l_iMinutes);

      std::wstring l_sRet = L"";

      if (l_iMinutes > 0)
        l_sRet = l_sMinutes + L":";

      return l_sRet + l_sSeconds + L"." + l_sHundrts;
    }

    /**
    * A marble has started a new lap
    * @param a_iStep the simulation step
    * @param a_iMarble ID of the marble
    * @param a_iLapNo the current lap
    */
    void CHudLapTimes::onLapStart(int a_iStep, int a_iMarble, int a_iLapNo) {
      if (m_mLapTimes.find(a_iMarble) == m_mLapTimes.end())
        m_mLapTimes[a_iMarble] = SPlayerRacetime();

      if (m_mLapTimes[a_iMarble].m_vLapTimes.size() > 0) {
        int l_iLastLapTime = a_iStep - m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iStart;
        m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iEnd     = a_iStep;
        m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iLapTime = l_iLastLapTime;

        if (m_iBestLapTime == -1 || l_iLastLapTime < m_iBestLapTime) {
          m_iBestLapTime = l_iLastLapTime;

          m_pBestLap = &(m_mLapTimes[a_iMarble].m_vLapTimes.back());

          if (m_mPlayers.find(a_iMarble) != m_mPlayers.end())
            m_sBestLap = m_mPlayers[a_iMarble];
          else
            m_sBestLap = L"-";
        }

        if (a_iMarble == m_iMarbleID && (m_iPersonalBest == -1 || l_iLastLapTime < m_iPersonalBest))
          m_iPersonalBest = l_iLastLapTime;

        if (l_iLastLapTime < m_mLapTimes[a_iMarble].m_iFastest || m_mLapTimes[a_iMarble].m_iFastest == -1) {
          m_mLapTimes[a_iMarble].m_iFastest = l_iLastLapTime;
        }

        if (a_iMarble == m_iMarbleID)
          m_iLastLapTime = l_iLastLapTime;
      }

      m_mLapTimes[a_iMarble].m_vLapTimes.push_back(SLapTime());
      m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iStart = a_iStep;
      m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iLapNo = a_iLapNo;

      if (a_iMarble == m_iMarbleID) {
        m_iLapStart = a_iStep;
        m_iCpNo     = 0;
      }
    }

    /**
    * A player has finished
    * @param a_iRaceTime the marble's race time in steps
    * @param a_iMarble ID of the marble
    */
    void CHudLapTimes::onPlayerFinished(int a_iRaceTime, int a_iMarble) {
      // The player has finished so we remove the currently running lap. I have chosen to do it this way because this
      // callback is called *after* the lap has started, and using the number of laps doesn't work either because the
      // player may already lapped.
      if (m_mLapTimes.find(a_iMarble) != m_mLapTimes.end() && m_mLapTimes[a_iMarble].m_vLapTimes.size() > 0) {
        m_mLapTimes[a_iMarble].m_vLapTimes.erase(m_mLapTimes[a_iMarble].m_vLapTimes.end() - 1);
      }

      if (m_iMarbleID == a_iMarble)
        m_iFinishStep = a_iRaceTime;
    }

    /**
    * A marble has passed a checkpoint
    * @param a_iStep the simulation step
    * @param a_iMarble ID of the marble
    */
    void CHudLapTimes::onCheckpoint(int a_iStep, int a_iMarble) {
      if (m_mLapTimes.find(a_iMarble) != m_mLapTimes.end() && m_mLapTimes[a_iMarble].m_vLapTimes.size() > 0) {
        int l_iSplitTime = a_iStep - m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iStart;

        if (l_iSplitTime != 0) {
          m_mLapTimes[a_iMarble].m_vLapTimes.back().m_vSplitTimes.push_back(l_iSplitTime);
          m_mLapTimes[a_iMarble].m_iLastSplit = a_iStep;
        }

        int l_iIndex = (int)m_mLapTimes[a_iMarble].m_vLapTimes.back().m_vSplitTimes.size();
        if (m_mSplits.find(l_iIndex) == m_mSplits.end() || m_mSplits[l_iIndex] > l_iSplitTime) {
          m_mSplits[l_iIndex] = l_iSplitTime;
        }
      }

      if (a_iMarble == m_iMarbleID) {
        m_iLastCpTime = a_iStep;
        m_iCpNo++;
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
      l_cLapTimePos.Y += m_iLapTimeOffset;
#endif

      std::vector<std::tuple<irr::core::position2di, std::wstring, std::wstring, irr::video::SColor, irr::video::SColor, bool>> l_vRows;

      l_vRows.push_back(std::make_tuple(
        l_cLapTimePos, 
        L" Race: ", 
        convertToTime(m_iFinishStep > 0 ? m_iFinishStep : a_iStep), 
        irr::video::SColor( 128, 255, 255, 192), 
        irr::video::SColor(0xFF,   0,   0,   0),
        false
      ));

      if (m_mLapTimes.find(m_iMarbleID) != m_mLapTimes.end()) {
        std::wstring s = L"-";

        irr::video::SColor l_cColor = irr::video::SColor(0xFF, 0, 0, 0);

        if (m_iLapStart > 0) {
          if (m_iLastCpTime > 0 && a_iStep - m_iLastCpTime < 180 && m_iCpNo > 1) {
            irr::s32 l_iTime = m_iLastCpTime - m_iLapStart;

            if (m_mSplits.find(m_iCpNo) == m_mSplits.end() || m_mSplits[m_iCpNo] >= l_iTime)
              l_cColor = irr::video::SColor(0xFF, 255, 64, 0);

            s = convertToTime(l_iTime);
          }
          else {
            s = convertToTime(a_iStep - m_iLapStart);
          }
        }
        else s = L"-";

        l_cLapTimePos.Y += m_iLapTimeOffset;
        l_vRows.push_back(std::make_tuple(
          l_cLapTimePos, L" Lap: ", s, irr::video::SColor(128, 192, 192, 255), l_cColor, false
        ));

        if (m_iLastLapTime > 0) s = convertToTime(m_iLastLapTime); else s = L"-";

        l_cLapTimePos.Y += m_iLapTimeOffset;
        l_vRows.push_back(std::make_tuple(
          l_cLapTimePos, L" Last: ", s, irr::video::SColor(128, 192, 192, 255), irr::video::SColor(0xFF, 0, 0, 0), false
        ));

        l_cColor = irr::video::SColor(0xFF, 0, 0, 0);

        if (m_iBestLapTime > 0) {
          s = convertToTime(m_iBestLapTime); 
          if (m_iBestLapTime == m_iPersonalBest)
            l_cColor = irr::video::SColor(0xFF, 255, 255, 0);
        }
        else s = L"-";

        l_cLapTimePos.Y += m_iLapTimeOffset;
        l_vRows.push_back(std::make_tuple(
          l_cLapTimePos, L" Fastest: ", s, irr::video::SColor(128, 192, 192, 255), l_cColor, false
        ));

        l_cLapTimePos.Y += m_iLapTimeOffset;
        l_vRows.push_back(std::make_tuple(
          l_cLapTimePos, L"", m_sBestLap, irr::video::SColor(128, 192, 192, 255), irr::video::SColor(0xFF, 0, 0, 0), true
        ));
      }

      for (auto l_tRow : l_vRows) {
        m_pDrv->draw2DRectangle(std::get<3>(l_tRow), irr::core::recti(std::get<0>(l_tRow), m_cSizeTotal));

        if (!std::get<5>(l_tRow)) {
          irr::core::dimension2du l_cSize = m_pFont->getDimension(std::get<1>(l_tRow).c_str());
          irr::core::recti l_cRect = irr::core::recti(
            std::get<0>(l_tRow) + irr::core::vector2di(m_cSizeCol1.Width - l_cSize.Width, 0), irr::core::dimension2du(l_cSize.Width, m_cSizeTotal.Height)
          );
          m_pFont->draw(std::get<1>(l_tRow).c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), false, true);

          l_cSize = m_pFont->getDimension(std::get<2>(l_tRow).c_str());
          l_cSize.Width += m_cSizeTotal.Height / 4;

          l_cRect = irr::core::recti(
            std::get<0>(l_tRow) + irr::core::vector2di(m_cSizeTotal.Width - l_cSize.Width, 0), irr::core::dimension2du(l_cSize.Width, m_cSizeTotal.Height)
          );

          m_pFont->draw(std::get<2>(l_tRow).c_str(), l_cRect, std::get<4>(l_tRow), false, true);
        }
        else {
          irr::core::recti l_cRect = irr::core::recti(std::get<0>(l_tRow), m_cSizeTotal);
          m_pFont->draw(std::get<2>(l_tRow).c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), true, true);
        }
      }
    }
  }
}