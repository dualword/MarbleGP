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

        int m_iOffset;      /**< Vertical offset between the items */
        int m_iLastCp;      /**< The player's last checkpoint time */
        int m_iMarble;      /**< The player's marble ID */
        int m_iStartLap;    /**< The player's last lap start */
        int m_iLastLap;     /**< The player's last lap time */
        int m_iBestLap;     /**< The best lap time */
        int m_iBestOld;     /**< The previous best lap used for calculating the deficit of the laptimes */
        int m_iFinished;    /**< Finish time of the HUD player */

        int m_aLapStart[16];  /**< Lap start for every player */

        bool m_bFirstLap;   /**< Is this the first lap? */

        std::wstring m_sBest;   /**< Name of the player with the best race lap */
         
        std::vector<int> m_aSplits[16];   /**< Current lap split times for all players */
        std::vector<int> m_vBest;         /**< Split times of the best lap */

        gameclasses::SRaceData *m_aPlayers[16];   /**< Array of the players */

        gameclasses::SRaceData *m_pPlayer;    /**< The HUD player's race data */

        gameclasses::SPlayer *m_pBest;    /**< The player with the best lap */

        /**
        * Convert a number of steps to a time string
        * @param a_iSteps the number of steps to conver
        * @param a_bSign always add a sign, even for positive numbers
        * @return string representation of the time
        */
        std::wstring convertToTime(irr::s32 a_iSteps, bool a_bSign);

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