// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    /**
    * @class IControllerGame
    * @author Christian Keimel
    * This interface must be implemented by
    * all game controllers (key/joy, touch, gyroscope)
    */
    class IControllerGame {
      public:
        enum class enType {
          KeyJoy,           /**< Keyboard and gamepad control */
          Touch,            /**< Touch controls, steering left (Android only) */
          Gyroscope,        /**< Gyroscope controls (Android only) */

          Count
        };

      private:
        enType m_eType;

      public:
        IControllerGame(IControllerGame::enType a_eType) : m_eType(a_eType) { }
        virtual ~IControllerGame() { }

        /**
        * Returns the throttle control (-1 .. 1): forward - backward
        * @return the throttle state
        */
        virtual irr::f32 getThrottle() = 0;

        /**
        * Returns the steer (-1 .. 1): right - left
        * @return the steer state
        */
        virtual irr::f32 getSteer() = 0;

        /**
        * Returns the state of the brake control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getBrake() = 0;

        /**
        * Return the rearview control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRearView() = 0;

        /**
        * Get the respawn control state
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRespawn() = 0;

        /**
        * Get the "withdraw from race" button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool withdrawFromRace() = 0;

        /**
        * Get the pause button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool pause() = 0;

        /**
        * Process Irrlicht evnts to update the controls
        * @param a_cEvent event to process
        */
        virtual void updateControls(const irr::SEvent &a_cEvent) = 0;

        /**
        * Get the selected controller type
        * @return the selected controller type
        */
        IControllerGame::enType getType() { return m_eType; }

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() = 0;
    };
  }
}
