// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <controller/IControllerGame.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerGame
    * @author Christian Keimel
    * This class handles handles controls for the actual game
    */
    class CControllerGame : public CControllerBase, public IControllerGame {
      private:
        bool m_bWithdrawDown;   /**< Cancel button pressed? */
        bool m_bPauseDown;    /**< Pause button pressed? */

      public:
        CControllerGame();
        virtual ~CControllerGame();

        /**
        * Returns the throttle control (-1 .. 1): forward - backward
        * @return the throttle state
        */
        irr::f32 getThrottle();

        /**
        * Returns the steer (-1 .. 1): right - left
        * @return the steer state
        */
        irr::f32 getSteer();

        /**
        * Returns the state of the brake control
        * @return true if control >= 0.5, false otherwise
        */
        bool getBrake();

        /**
         * Return the rearview control
         * @return true if control >= 0.5, false otherwise
         */
        bool getRearView();

        /**
        * Get the respawn control state
        * @return true if control >= 0.5, false otherwise
        */
        bool getRespawn();

        /**
        * Get the "withdraw from race" button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        bool withdrawFromRace();

        /**
        * Get the pause button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        bool pause();

        /**
        * Process Irrlicht evnts to update the controls
        * @param a_cEvent event to process
        */
        virtual void updateControls(const irr::SEvent &a_cEvent);
    };

  } // namespace controller
} // namespace dustbin