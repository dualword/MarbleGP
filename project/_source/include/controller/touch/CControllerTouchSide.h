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
      private:
        enum class enControl {
          ForwardL  = 0,
          BackwardL = 1,
          ForwardR  = 2,
          BackwardR = 3,
          BrakeL    = 4,
          BrakeR    = 5,
          Left      = 6,
          Right     = 7,
          Rearview  = 8,
          Respawn   = 9,

          Count     = 10
        };

        SControl m_aControls[(int)enControl::Count];

        irr::s32 m_iThrottleHeight;
        irr::s32 m_iLeftID;
        irr::s32 m_iRightID;
        irr::s32 m_iCenterX;
        irr::s32 m_iSteerLeft;
        irr::s32 m_iSteerRght;
        irr::f32 m_iSteerY1;
        irr::s32 m_iSteerY2;
        irr::s32 m_iForward;
        irr::s32 m_iBackward;
        irr::f32 m_fThrottle;
        irr::f32 m_fSteer;

        bool m_bBrake;

        void addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn);
        void updateSteering(const irr::core::vector2di &a_cTouch);
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
