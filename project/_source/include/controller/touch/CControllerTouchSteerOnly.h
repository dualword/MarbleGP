// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/touch/ITouchController.h>
#include <irrlicht.h>
#include <string>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchSteerOnly
    * @author Christian Keimel
    * This controller is for AI help marbles, only steering, brake, rearview and respawn are available
    */
    class CControllerTouchSteerOnly : public controller::ITouchController {
      private:
        enum class enControl {
          Left     = 0,
          Right    = 1,
          BrakeL   = 2,
          BrakeR   = 3,
          Rearview = 4,
          Respawn  = 5,

          Count    = 6
        };

        enum class enControlAreas {
          Left = 0,
          Right = 1,
          Brake = 2,
          Rearview = 3,
          Respawn = 4,

          Count = 5
        };

        SControl m_aControls[(int)enControl::Count];

        irr::core::recti m_aCtrlRects[(int)enControlAreas::Count];

        std::map<size_t, irr::core::vector2di> m_mTouch;

        irr::s32 m_iThrottleHeight;
        irr::f32 m_fSteer;
        irr::f32 m_fThrottle;

        bool m_bRearView;
        bool m_bRespawn;

        void addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn);
        void calculateSteer();

      public:
        CControllerTouchSteerOnly(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~CControllerTouchSteerOnly();

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
