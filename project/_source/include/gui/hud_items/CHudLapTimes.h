// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CGameHUDHelpers.h>
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
        irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht video drive */
        irr::core::vector2di       m_cPos;    /**< The rendering position */
        irr::core::dimension2du    m_cSize;   /**< Size of the labels */
        irr::gui::IGUIFont        *m_pFont;   /**< The font to use for rendering */

        std::map<int, SPlayerRacetime> m_mLapTimes;       /**< The lap times of the marbles (key == marble id, value == lap time structure vector) */
        std::vector<int>               m_vBestSplits;     /**< The best split times */
        int                            m_iBestLapTime;    /**< The best lap time */
        int                            m_iLapTimeOffset;  /**< Vertical offset between the lap time labels */
        int                            m_iFinishStep;     /**< The simulation step when the player has finished */
        int                            m_iMarbleID;       /**< ID of the player's marble */

      public:
        /**
        * The constructor
        * @param a_pDrv the Irrlicht video driver
        * @param a_iMarbleId ID of the player's marble
        * @param a_cPos the position (upper right corner as the item is drawn in the upper right of the screen)
        * @param a_pFont the font to use for rendering
        */
        CHudLapTimes(irr::video::IVideoDriver *a_pDrv, int a_iMarbleId, const irr::core::vector2di &a_cPos, irr::gui::IGUIFont *a_pFont);
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