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
    */
    CHudLapTimes::CHudLapTimes(irr::video::IVideoDriver *a_pDrv, int a_iMarbleId, const irr::core::vector2di &a_cPos, irr::gui::IGUIFont *a_pFont) : 
      m_pDrv          (a_pDrv),
      m_pFont         (a_pFont),
      m_iBestLapTime  (-1),
      m_iLapTimeOffset(0),
      m_iFinishStep   (0),
      m_iMarbleID     (a_iMarbleId)
    {
      m_cSize = m_pFont->getDimension(L"Lap #66: 666.66");
      m_cSize.Width  = 5 * m_cSize.Width  / 4;
      m_cSize.Height = 5 * m_cSize.Height / 4;

      m_cPos  = a_cPos;
      m_cPos.X -= m_cSize.Width;

      m_iLapTimeOffset = 5 * m_cSize.Height / 4;
    }

    CHudLapTimes::~CHudLapTimes() {
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
        int l_iTime = a_iStep - m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iStart;
        m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iEnd     = a_iStep;
        m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iLapTime = l_iTime;

        if (m_iBestLapTime == -1 || l_iTime < m_iBestLapTime) {
          m_iBestLapTime = l_iTime;
        }

        if (l_iTime < m_mLapTimes[a_iMarble].m_iFastest || m_mLapTimes[a_iMarble].m_iFastest == -1) {
          m_mLapTimes[a_iMarble].m_iFastest = l_iTime;
        }
      }

      m_mLapTimes[a_iMarble].m_vLapTimes.push_back(SLapTime());
      m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iStart = a_iStep;
      m_mLapTimes[a_iMarble].m_vLapTimes.back().m_iLapNo = a_iLapNo;
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

          size_t l_iSplitIdx = m_mLapTimes[a_iMarble].m_vLapTimes.back().m_vSplitTimes.size();

          if (m_vBestSplits.size() < l_iSplitIdx) {
            m_vBestSplits.push_back(l_iSplitTime);
          }
          else {
            if (l_iSplitTime < m_vBestSplits[l_iSplitIdx - 1]) {
              m_vBestSplits[l_iSplitIdx - 1] = l_iSplitTime;
            }
          }

          if (m_mLapTimes[a_iMarble].m_vPlayerBestSplit.size() < l_iSplitIdx) {
            m_mLapTimes[a_iMarble].m_vPlayerBestSplit.push_back(l_iSplitTime);
          }
          else {
            if (l_iSplitTime < m_mLapTimes[a_iMarble].m_vPlayerBestSplit[l_iSplitIdx - 1])
              m_mLapTimes[a_iMarble].m_vPlayerBestSplit[l_iSplitIdx - 1] = l_iSplitTime;
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

      if (m_mLapTimes.find(m_iMarbleID) != m_mLapTimes.end()) {
        for (std::vector<SLapTime>::iterator it = m_mLapTimes[m_iMarbleID].m_vLapTimes.begin(); it != m_mLapTimes[m_iMarbleID].m_vLapTimes.end(); it++) {
          std::wstring l_sLap  = L" Lap " + std::to_wstring((*it).m_iLapNo) + L": ";
          std::wstring l_sTime = L"";

          irr::video::SColor l_cColor = irr::video::SColor(128, 224, 224, 244);
          irr::video::SColor l_cText  = irr::video::SColor(255,   0,   0,   0);

          if ((*it).m_iLapTime != -1) {
            l_sTime = helpers::convertToTime((*it).m_iLapTime);

            if ((*it).m_iLapTime == m_iBestLapTime) {
              l_cColor = irr::video::SColor(128, 255, 96, 96);
            }
            else if ((*it).m_iLapTime == m_mLapTimes[m_iMarbleID].m_iFastest) {
              l_cColor = irr::video::SColor(128, 255, 255, 96);
            }
          }
          else {
            if (m_mLapTimes[m_iMarbleID].m_iLastSplit != -1 && a_iStep - m_mLapTimes[m_iMarbleID].m_iLastSplit <= 180) {
              irr::s32 l_iSplit = m_mLapTimes[m_iMarbleID].m_vLapTimes.back().m_vSplitTimes.back();
              l_sTime = helpers::convertToTime(l_iSplit);
              l_cColor = irr::video::SColor(128, 192, 192, 192);

              if (l_iSplit == m_vBestSplits[m_mLapTimes[m_iMarbleID].m_vLapTimes.back().m_vSplitTimes.size() - 1])
                l_cText = irr::video::SColor(0xFF, 255, 0, 0);
              else if (l_iSplit == m_mLapTimes[m_iMarbleID].m_vPlayerBestSplit[m_mLapTimes[m_iMarbleID].m_vLapTimes.back().m_vSplitTimes.size() - 1])
                l_cText = irr::video::SColor(0xFF, 255, 255, 0);
            }
            else l_sTime = helpers::convertToTime(a_iStep - (*it).m_iStart);
          }

          irr::core::dimension2du l_cSize = m_pFont->getDimension(l_sTime.c_str());

          irr::core::recti l_cRect = irr::core::recti(l_cLapTimePos, m_cSize);

          m_pDrv->draw2DRectangle(l_cColor, l_cRect, &a_cClip);
          m_pFont->draw(l_sLap.c_str(), irr::core::recti(l_cLapTimePos, m_cSize), irr::video::SColor(0xFF, 0, 0, 0), false, true, &a_cClip);

          irr::core::recti l_cRectTime = irr::core::recti(
            l_cRect.LowerRightCorner.X - m_cSize.Height / 4 - l_cSize.Width,
            l_cRect.UpperLeftCorner.Y,
            l_cRect.LowerRightCorner.X,
            l_cRect.LowerRightCorner.Y
          );

          m_pFont->draw(l_sTime.c_str(), l_cRectTime, l_cText, false, true);

          l_cLapTimePos.Y += m_iLapTimeOffset;
        }
      }

      irr::core::recti l_cRect = irr::core::recti(l_cLapTimePos, m_cSize);
      m_pDrv->draw2DRectangle(irr::video::SColor(128, 192, 255, 192), l_cRect);
      m_pFont->draw(L" Race: ", l_cRect, irr::video::SColor(0xFF, 0, 0, 0), false, true, &l_cRect);

      std::wstring l_sTime = helpers::convertToTime(m_iFinishStep != 0 ? m_iFinishStep : a_iStep);
      irr::core::dimension2du l_cSize = m_pFont->getDimension(l_sTime.c_str());

      irr::core::recti l_cRectTime = irr::core::recti(
        l_cRect.LowerRightCorner.X - m_cSize.Height / 4 - l_cSize.Width,
        l_cRect.UpperLeftCorner.Y,
        l_cRect.LowerRightCorner.X,
        l_cRect.LowerRightCorner.Y
      );

      m_pFont->draw(l_sTime.c_str(), l_cRectTime, irr::video::SColor(0xFF, 0, 0, 0), false, true);
    }
  }
}