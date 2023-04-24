// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/touch/CControllerTouchSide.h>

namespace dustbin {
  namespace controller {
    CControllerTouchSide::CControllerTouchSide(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) : ITouchController(a_pDrv, a_cViewport),
      m_iThrottleHeight(0),
      m_fThrottle      (0.0f),
      m_fSteer         (0.0f)
    {
      m_cViewport = a_cViewport;
      m_iThrottleHeight = m_cViewport.getHeight() / 8;
    }

    CControllerTouchSide::~CControllerTouchSide() {
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerTouchSide::getThrottle() {
      return m_aControls[(int)enControl::Forward].m_bTouched ? 1.0f : (m_aControls[(int)enControl::Backward].m_bTouched || m_aControls[(int)enControl::Brake].m_bTouched) ? -1.0f : 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchSide::getSteer() {
      return m_aControls[(int)enControl::Left].m_bTouched ? -1.0f : m_aControls[(int)enControl::Right].m_bTouched ? 1.0f : 0.0f;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getBrake() {
      return m_aControls[(int)enControl::Brake].m_bTouched;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getRearView() {
      return m_aControls[(int)enControl::Rearview].m_bTouched;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getRespawn() {
      return m_aControls[(int)enControl::Respawn].m_bTouched;
    }

    /**
    * Draw the controller
    */
    void CControllerTouchSide::draw() {
      for (int i = 0; i < (int) enControl::Count; i++) {
        irr::video::ITexture *l_pTexture = m_aControls[i].m_bTouched ? m_aControls[i].m_pTextureOn
                                                                     : m_aControls[i].m_pTextureOff;

        if (l_pTexture != nullptr) {
          m_pDrv->draw2DImage(
            l_pTexture,
            m_aControls[i].m_cDestination,
            m_aControls[i].m_cSource,
            nullptr,
            nullptr,
            true
          );
        } else {
          m_pDrv->draw2DRectangle(m_aControls[i].m_cBackground, m_aControls[i].m_cDestination);
        }
      }
    }

    /**
    * Handle an event
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CControllerTouchSide::handleEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

#ifdef _TOUCH_CONTROL
      irr::core::vector2di l_cTouch = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::vector2di l_cTouch = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          enControl l_eTouch = getTouchControl(l_cTouch);

          if (l_eTouch != enControl::Count) {
            m_aControls[(int)l_eTouch].m_bTouched = true;
            m_aControls[(int)l_eTouch].m_iTouchID = a_cEvent.TouchInput.ID;
            l_bRet = true;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          enControl l_eOld = getTouched(a_cEvent.TouchInput.ID);

          if (l_eOld != enControl::Count) {
            m_aControls[(int)l_eOld].m_bTouched = false;
            m_aControls[(int)l_eOld].m_iTouchID = -1;
          }
          l_bRet = true;
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          enControl l_eTouch = getTouchControl(l_cTouch);
          enControl l_eOld   = getTouched(a_cEvent.TouchInput.ID);

          if (l_eTouch != enControl::Count && l_eOld != enControl::Count) {
            m_aControls[(int)l_eOld].m_bTouched = false;
            m_aControls[(int)l_eOld].m_iTouchID = -1;

            m_aControls[(int)l_eTouch].m_bTouched = true;
            m_aControls[(int)l_eTouch].m_iTouchID = a_cEvent.TouchInput.ID;
          }

          l_bRet = true;
        }
      }
#endif

      return l_bRet;
    }

    void CControllerTouchSide::addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn) {
      irr::video::ITexture *l_pOff = m_pDrv->getTexture(a_sOff.c_str());
      irr::video::ITexture *l_pOn  = m_pDrv->getTexture(a_sOn .c_str());

      m_aControls[(int)a_eControl] = SControl(a_cDestination, irr::core::recti(irr::core::vector2di(0, 0), l_pOff->getOriginalSize()), a_cColor, l_pOff, l_pOn);
    }

    /**
     * Get the current enum that the touch ID is touching
     * @param l_iTouchID the current touch ID
     * @return the enum of the touched element, enControl::Count if no element was found
     */
    CControllerTouchSide::enControl CControllerTouchSide::getTouched(size_t l_iTouchID) {
      for (int i = 0; i < (int)enControl::Count; i++) {
        if (m_aControls[i].m_iTouchID == l_iTouchID) {
          return (enControl)i;
        }
      }
      return enControl::Count;
    }
  }
}