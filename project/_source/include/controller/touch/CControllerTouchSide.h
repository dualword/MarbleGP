// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/touch/ITouchController.h>
#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchSide
    * @author Christian Keimel
    * This controller has all the controls located on the left and right side of the screen
    */
    class CControllerTouchSide : public controller::ITouchController {
      private:
        enum class enControl {
          ForwardL  = 0,
          BackwardL = 1,
          ForwardR  = 0,
          BackwardR = 1,
          BrakeL    = 2,
          BrakeR    = 3,
          Left      = 4,
          Right     = 5,
          Rearview  = 6,
          Respawn   = 6,

          Count     = 8
        };

        SControl m_aControls[(int)enControl::Count];

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
