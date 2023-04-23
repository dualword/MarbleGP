// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/touch/CControllerTouchSteerOnly.h>

namespace dustbin {
  namespace controller {
    CControllerTouchSteerOnly::CControllerTouchSteerOnly(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) : ITouchController(a_pDrv, a_cViewport), m_iThrottleHeight(0), m_fSteer(0.0f) {
      m_iThrottleHeight = m_cViewport.getHeight() / 8;
      irr::core::vector2di l_cCenter = a_cViewport.getCenter();
      irr::core::dimension2du l_cSize = irr::core::dimension2du (m_iThrottleHeight, m_iThrottleHeight);

      addToControlMap(enControl::Left    , irr::core::recti(irr::core::vector2di(                                         0, m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2), l_cSize), irr::video::SColor(128,    0,   0, 255), "data/images/ctrl_left_off.png", "data/images/ctrl_left.png");
      addToControlMap(enControl::Right   , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight, m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2), l_cSize), irr::video::SColor(128,    0,   0, 255), "data/images/ctrl_right_off.png", "data/images/ctrl_right.png");
      addToControlMap(enControl::Rearview, irr::core::recti(irr::core::vector2di(                                         0,                                                   0), l_cSize), irr::video::SColor(128, 255, 255,    0), "data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png");
      addToControlMap(enControl::Respawn , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight,                                                   0), l_cSize), irr::video::SColor(128, 255, 255,    0), "data/images/ctrl_respawn_off.png", "data/images/ctrl_respawn.png");
      addToControlMap(enControl::BrakeL  , irr::core::recti(irr::core::vector2di(                                         0, m_cViewport.getHeight()     - m_iThrottleHeight    ), l_cSize), irr::video::SColor(128, 255,   0,    0), "data/images/ctrl_brake_off.png", "data/images/ctrl_brake.png");
      addToControlMap(enControl::BrakeR  , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight, m_cViewport.getHeight()     - m_iThrottleHeight    ), l_cSize), irr::video::SColor(128, 255,   0,    0), "data/images/ctrl_brake_off.png", "data/images/ctrl_brake.png");
    }

    CControllerTouchSteerOnly::~CControllerTouchSteerOnly() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerTouchSteerOnly::getThrottle() {
      return 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchSteerOnly::getSteer() {
      return m_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSteerOnly::getBrake() {
      return m_aControls[(int)enControl::BrakeR].m_bTouched||  m_aControls[(int)enControl::BrakeL].m_bTouched;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSteerOnly::getRearView() {
      return m_aControls[(int)enControl::Rearview].m_bTouched;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSteerOnly::getRespawn() {
      return m_aControls[(int)enControl::Respawn].m_bTouched;
    }

    /**
    * Draw the controller
    */
    void CControllerTouchSteerOnly::draw() {
      for (int i = 0; i < (int)enControl::Count; i++) {
        irr::video::ITexture *l_pTexture = m_aControls[i].m_bTouched ? m_aControls[i].m_pTextureOn : m_aControls[i].m_pTextureOff;

        if (l_pTexture != nullptr) {
          m_pDrv->draw2DImage(
            l_pTexture,
            m_aControls[i].m_cDestination,
            m_aControls[i].m_cSource,
            nullptr,
            nullptr,
            true
          );
        }
        else {
          m_pDrv->draw2DRectangle(m_aControls[i].m_cBackground, m_aControls[i].m_cDestination);
        }
      }
    }

    /**
    * Handle an event
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CControllerTouchSteerOnly::handleEvent(const irr::SEvent &a_cEvent) {
      bool a_bRet = false;

#ifdef _TOUCH_CONTROL
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          irr::core::vector2di l_cTouch = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

          if (l_cTouch.X < m_cViewport.getWidth() / 2 && l_cTouch.Y > m_iThrottleHeight && l_cTouch.Y < m_cViewport.getHeight() - m_iThrottleHeight) {
            m_aControls[(int)enControl::Left].m_bTouched = true;
            m_aControls[(int)enControl::Left].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          if (l_cTouch.X > m_cViewport.getWidth() / 2 && l_cTouch.Y >  m_iThrottleHeight && l_cTouch.Y < m_cViewport.getHeight() - m_iThrottleHeight) {
            m_aControls[(int)enControl::Right].m_bTouched = true;
            m_aControls[(int)enControl::Right].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          if (l_cTouch.X < m_cViewport.getWidth() / 2 && l_cTouch.Y <= m_iThrottleHeight) {
            m_aControls[(int)enControl::Rearview].m_bTouched = true;
            m_aControls[(int)enControl::Rearview].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          if (l_cTouch.X > m_cViewport.getWidth() / 2 && l_cTouch.Y <= m_iThrottleHeight) {
            m_aControls[(int)enControl::Respawn ].m_bTouched = true;
            m_aControls[(int)enControl::Respawn ].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          if (l_cTouch.Y >= m_cViewport.getHeight() - m_iThrottleHeight) {
            if (l_cTouch.X < m_cViewport.getWidth() / 2) {
              m_aControls[(int)enControl::BrakeL].m_bTouched = true;
              m_aControls[(int)enControl::BrakeL].m_iTouchID = a_cEvent.TouchInput.ID;
            } else {
              m_aControls[(int)enControl::BrakeR].m_bTouched = true;
              m_aControls[(int)enControl::BrakeR].m_iTouchID = a_cEvent.TouchInput.ID;
            }
          }

          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          for (int i = 0; i < (int)enControl::Count; i++) {
            if (m_aControls[i].m_iTouchID == a_cEvent.TouchInput.ID) {
              m_aControls[i].m_iTouchID = -1;
              m_aControls[i].m_bTouched = false;
            }
          }
          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (m_aControls[(int)enControl::Left].m_bTouched && m_aControls[(int)enControl::Left].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::BrakeL].m_bTouched = a_cEvent.TouchInput.Y >= m_cViewport.getHeight() - m_iThrottleHeight;
            m_aControls[(int)enControl::BrakeL].m_iTouchID = a_cEvent.TouchInput.ID;
          }

          if (m_aControls[(int)enControl::Right].m_bTouched && m_aControls[(int)enControl::Right].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::BrakeR].m_bTouched = a_cEvent.TouchInput.Y >= m_cViewport.getHeight() - m_iThrottleHeight;
            m_aControls[(int)enControl::BrakeR].m_iTouchID = a_cEvent.TouchInput.ID;
          }
        }
      }
#endif

      return a_bRet;
    }

    void CControllerTouchSteerOnly::addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn) {
      irr::video::ITexture *l_pOff = m_pDrv->getTexture(a_sOff.c_str());
      irr::video::ITexture *l_pOn  = m_pDrv->getTexture(a_sOn .c_str());

      m_aControls[(int)a_eControl] = SControl(a_cDestination, irr::core::recti(irr::core::vector2di(0, 0), l_pOff->getOriginalSize()), a_cColor, l_pOff, l_pOn);
    }

    void CControllerTouchSteerOnly::calculateSteer() {
      if (m_aControls[(int)enControl::Left].m_bTouched)
        m_fSteer = -1.0f;
      else if (m_aControls[(int)enControl::Right].m_bTouched)
        m_fSteer = 1.0f;
      else
        m_fSteer = 0.0f;
    }
  }
}