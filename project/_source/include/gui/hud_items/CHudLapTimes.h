// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CGameHUDHelpers.h>
#include <gameclasses/SPlayer.h>
#include <irrlicht.h>
#include <map>

namespace dustbin {
  namespace gui {
    /**
    * @class CHudLapTimes
    * @author Christian Keimel
    * This class takes care of showing the lap times (if wanted)
    */
    class CHudLapTimes {
      private:
        irr::video::IVideoDriver  *m_pDrv;        /**< The Irrlicht video drive */
        irr::core::vector2di       m_cPos;        /**< The rendering position */
        irr::core::dimension2du    m_cSizeCol1;   /**< Size of the first part of the display (LapXX...) */
        irr::core::dimension2du    m_cSizeCol2;   /**< Size of the second part of the display (Time) */
        irr::core::dimension2du    m_cSizeTotal;  /**< Total size of the labels */
        irr::gui::IGUIFont        *m_pFont;       /**< The font to use for rendering */

        std::map<int, SPlayerRacetime> m_mLapTimes;       /**< The lap times of the marbles (key == marble id, value == lap time structure vector) */
        int                            m_iBestLapTime;    /**< The best lap time */
        int                            m_iPersonalBest;   /**< The player's best lap time */
        int                            m_iLastLapTime;    /**< Lap time of the player's last lap */
        int                            m_iLapTimeOffset;  /**< Vertical offset between the lap time labels */
        int                            m_iFinishStep;     /**< The simulation step when the player has finished */
        int                            m_iMarbleID;       /**< ID of the player's marble */
        int                            m_iLastCpTime;     /**< The player's last checkpoint time*/
        int                            m_iLapStart;       /**< Start of the player's lap */
        int                            m_iCpNo;           /**< Number of the last checkpoint the player has passede */
        std::map<int, std::wstring>    m_mPlayers;        /**< Assignment of the marble IDs to the player names */
        std::map<int, int>             m_mSplits;         /**< Best split times */

        std::wstring m_sBestLap;    /**< Name of the player with the best lap time */

        SLapTime *m_pBestLap;   /**< Data of the best lap */

        /**
        * Convert a number of steps to a time string
        * @param a_iSteps the number of steps to convert
        * @return string representation of the time
        */
        std::wstring convertToTime(irr::s32 a_iSteps);

      public:
        /**
        * The constructor
        * @param a_pDrv the Irrlicht video driver
        * @param a_iMarbleId ID of the player's marble
        * @param a_cPos the position (upper right corner as the item is drawn in the upper right of the screen)
        * @param a_pFont the font to use for rendering
        * @param a_pPlayers the players
        */
        CHudLapTimes(
          irr::video::IVideoDriver *a_pDrv, 
          int a_iMarbleId, 
          const irr::core::vector2di &a_cPos, 
          irr::gui::IGUIFont *a_pFont, 
          const std::vector<gameclasses::SRaceData *> &a_vPlayers
        );
        ~CHudLapTimes();

        /**
        * A marble has started a new lap
        * @param a_iStep the simulation step
        * @param a_iLapNo the current lap
        */
        void onLapStart(int a_iStep, int a_iMarble, int a_iLapNo);

        /**
        * A player has finished
        * @param a_iRaceTime the marble's race time in steps
        * @param a_iMarble ID of the marble
        */
        void onPlayerFinished(int a_iRaceTime, int a_iMarble);

        /**
        * A marble has passed a checkpoint
        * @param a_iStep the simulation step
        * @param a_iMarble ID of the marble
        */
        void onCheckpoint(int a_iStep, int a_iMarble);

        /**
        * Render the lap times list
        * @param a_iStep the simulation step
        * @param a_cClip the clipping rectangle
        */
        void render(int a_iStep, const irr::core::recti &a_cClip);
    };
  }
}