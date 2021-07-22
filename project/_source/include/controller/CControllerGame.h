// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerGame
    * @author Christian Keimel
    * This class handles handles controls for the actual game
    */
    class CControllerGame : public CControllerBase {
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
    };

  } // namespace controller
} // namespace dustbin