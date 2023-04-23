// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/touch/CControllerTouchSide.h>

namespace dustbin {
  namespace controller {
    CControllerTouchSide::CControllerTouchSide(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) : ITouchController(a_pDrv, a_cViewport) {

    }

    CControllerTouchSide::~CControllerTouchSide() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerTouchSide::getThrottle() {
      return 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchSide::getSteer() {
      return 0.0f;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getBrake() {
      return false;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getRearView() {
      return false;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getRespawn() {
      return false;
    }

    /**
    * Draw the controller
    */
    void CControllerTouchSide::draw() {

    }

    /**
    * Handle an event
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CControllerTouchSide::handleEvent(const irr::SEvent &a_cEvent) {
      bool a_bRet = false;

#ifdef _ANDROID
#endif

      return a_bRet;
    }
  }
}