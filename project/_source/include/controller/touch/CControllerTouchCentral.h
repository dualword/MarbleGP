// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/touch/ITouchController.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchCentral
    * @author Christian Keimel
    * This is the controller with the controls in the center, right below the marble
    */
    class CControllerTouchCentral : public controller::ITouchController {
      private:
        enum class enControl {
          Neutral  = 0,
          Forward  = 1,
          Backward = 2,
          Left     = 3,
          Right    = 4,
          Rearview = 5,
          Respawn  = 6,

          Count    = 7
        };

        SControl m_aControls[(int)enControl::Count];

        irr::s32 m_iTouchSteer;     /**< The X coordinate of the controller touch */
        irr::s32 m_iThrottleHeight;
        irr::f32 m_fSteer;

        irr::core::vector2di m_cSteerPos;  /**< The position of the controller touch */

        irr::core::recti m_cSteerRect;

        void addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn);
        void calculateSteer();

      public:
        CControllerTouchCentral(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~CControllerTouchCentral();

        /**
        * Returns the throttle control (-1 .. 1): forward - backward
        * @return the throttle state
        */
        virtual irr::f32 getThrottle();

        /**
        * Returns the steer (-1 .. 1): left - right
        * @return the steer state
        */
        virtual irr::f32 getSteer();

        /**
        * Returns the state of the brake control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getBrake();

        /**
        * Return the rearview control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRearView();

        /**
        * Get the respawn control state
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRespawn();

        /**
        * Handle an event
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleEvent(const irr::SEvent &a_cEvent);

        /**
        * Draw the controller
        */
        virtual void draw();
    };
  }
}
