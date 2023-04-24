// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/touch/ITouchController.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchSide
    * @author Christian Keimel
    * This controller has all the controls located on the left and right side of the screen
    */
    class CControllerTouchSide : public controller::ITouchController {
      protected:
        enum class enControl {
          Forward   = 0,
          NeutralV  = 1,
          Backward  = 2,
          Left      = 3,
          NeutralS  = 4,
          Right     = 5,
          Brake     = 6,
          Rearview  = 7,
          Respawn   = 8,

          Count     = 9
        };

        SControl m_aControls[(int)enControl::Count];

        irr::s32 m_iThrottleHeight;
        irr::f32 m_fThrottle;
        irr::f32 m_fSteer;

        void addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn);

        /**
         * Get the control that is touched
         * @param a_cTouch position of the touch event
         * @return the enum of the touched element, enControl::Count if no element was touched
         */
        virtual enControl getTouchControl(const irr::core::vector2di &a_cTouch) = 0;

        /**
         * Get the current enum that the touch ID is touching
         * @param l_iTouchID the current touch ID
         * @return the enum of the touched element, enControl::Count if no element was found
         */
        enControl getTouched(size_t l_iTouchID);
      public:
        CControllerTouchSide(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~CControllerTouchSide();

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
