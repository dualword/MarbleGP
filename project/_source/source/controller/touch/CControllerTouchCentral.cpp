// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/touch/CControllerTouchCentral.h>

namespace dustbin {
  namespace controller {
    void CControllerTouchCentral::addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn) {
      irr::video::ITexture *l_pOff = m_pDrv->getTexture(a_sOff.c_str());
      irr::video::ITexture *l_pOn  = m_pDrv->getTexture(a_sOn .c_str());

      m_aControls[(int)a_eControl] = SControl(a_cDestination, irr::core::recti(irr::core::vector2di(0, 0), l_pOff->getOriginalSize()), a_cColor, l_pOff, l_pOn);
    }

    CControllerTouchCentral::CControllerTouchCentral(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) :
      ITouchController (a_pDrv, a_cViewport),
      m_iTouchSteer    (-1),
      m_iThrottleHeight(0),
      m_fSteer         (0.0f)
    {
      m_iThrottleHeight = m_cViewport.getHeight() / 8;

      irr::core::dimension2du l_cSize = irr::core::dimension2du (m_iThrottleHeight, m_iThrottleHeight);
      irr::core::vector2di l_cCenter = a_cViewport.getCenter();

      addToControlMap(enControl::Backward, irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() -     l_cSize.Height), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_back_off.png", "data/images/ctrl_back.png");
      addToControlMap(enControl::Neutral , irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128, 255, 255, 255), "data/images/ctrl_none_off.png", "data/images/ctrl_none.png");
      addToControlMap(enControl::Forward , irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() - 3 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png");
      addToControlMap(enControl::Left    , irr::core::recti(irr::core::vector2di(   l_cCenter.X - 3 * l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_left_off.png", "data/images/ctrl_left.png");
      addToControlMap(enControl::Right   , irr::core::recti(irr::core::vector2di(   l_cCenter.X +     l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_right_off.png", "data/images/ctrl_right.png");
      addToControlMap(enControl::Rearview, irr::core::recti(irr::core::vector2di(                                     0,                                            0), l_cSize), irr::video::SColor(128, 255, 255,   0), "data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png");
      addToControlMap(enControl::Respawn , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cSize.Width,                                            0), l_cSize), irr::video::SColor(128, 255, 255,   0), "data/images/ctrl_respawn_off.png", "data/images/ctrl_respawn.png");

      m_cSteerRect.UpperLeftCorner .X = m_aControls[(int)enControl::Left    ].m_cDestination.UpperLeftCorner .X;
      m_cSteerRect.UpperLeftCorner .Y = m_aControls[(int)enControl::Forward ].m_cDestination.UpperLeftCorner .Y;
      m_cSteerRect.LowerRightCorner.X = m_aControls[(int)enControl::Right   ].m_cDestination.LowerRightCorner.X;
      m_cSteerRect.LowerRightCorner.Y = m_aControls[(int)enControl::Backward].m_cDestination.LowerRightCorner.Y;
    }

    CControllerTouchCentral::~CControllerTouchCentral() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerTouchCentral::getThrottle() {
      return m_aControls[(int)enControl::Forward].m_bTouched ? 1.0f : m_aControls[(int)enControl::Backward].m_bTouched ? -1.0f : 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchCentral::getSteer() {
      return m_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchCentral::getBrake() {
      return m_aControls[(int)enControl::Backward].m_bTouched;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchCentral::getRearView() {
      return m_aControls[(int)enControl::Rearview].m_bTouched;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchCentral::getRespawn() {
      return false;
    }

    /**
    * Draw the controller
    */
    void CControllerTouchCentral::draw() {
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
    bool CControllerTouchCentral::handleEvent(const irr::SEvent &a_cEvent) {
      bool a_bRet = false;

#ifdef _TOUCH_CONTROL
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          irr::core::vector2di l_cTouch = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

          if (m_cSteerRect.isPointInside(l_cTouch)) {
            m_iTouchSteer = a_cEvent.TouchInput.ID;
            m_cSteerPos   = l_cTouch;
          }

          for (int i = 0; i < (int)enControl::Count; i++) {
            if (m_aControls[i].m_cDestination.isPointInside(l_cTouch)) {
              m_aControls[i].m_iTouchID = a_cEvent.TouchInput.ID;
              m_aControls[i].m_bTouched = true;
              break;
            }
          }


          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (a_cEvent.TouchInput.ID == m_iTouchSteer) {
            m_iTouchSteer = -1;

            enControl l_aCtrl[] = {
              enControl::Neutral,
              enControl::Left,
              enControl::Right,
              enControl::Forward,
              enControl::Backward,
              enControl::Count
            };

            for (int i = 0; l_aCtrl[i] != enControl::Count; i++) {
              m_aControls[(int)l_aCtrl[i]].m_bTouched = false;
              m_aControls[(int)l_aCtrl[i]].m_iTouchID = -1;
            }

            if (!m_aControls[(int)enControl::Left].m_bTouched && !m_aControls[(int)enControl::Right].m_bTouched) {
              m_fSteer = 0.0f;
            }
          }
          else {
            enControl l_aCtrl[]  = {
              enControl::Respawn,
              enControl::Respawn,
              enControl::Count
            };

            for (int i = 0; l_aCtrl[i] != enControl::Count; i++) {
              if (a_cEvent.TouchInput.ID == m_aControls[(int)l_aCtrl[i]].m_iTouchID) {
                m_aControls[(int)l_aCtrl[i]].m_bTouched = false;
                m_aControls[(int)l_aCtrl[i]].m_iTouchID = -1;
              }
            }
          }

          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (a_cEvent.TouchInput.ID == m_iTouchSteer) {
            m_cSteerPos = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

            m_aControls[(int)enControl::Left    ].m_bTouched = m_cSteerPos.X <= m_aControls[(int)enControl::Left    ].m_cDestination.LowerRightCorner.X;
            m_aControls[(int)enControl::Right   ].m_bTouched = m_cSteerPos.X >= m_aControls[(int)enControl::Right   ].m_cDestination.UpperLeftCorner .X;
            m_aControls[(int)enControl::Forward ].m_bTouched = m_cSteerPos.Y <= m_aControls[(int)enControl::Forward ].m_cDestination.LowerRightCorner.Y;
            m_aControls[(int)enControl::Backward].m_bTouched = m_cSteerPos.Y >= m_aControls[(int)enControl::Backward].m_cDestination.UpperLeftCorner .Y;

            m_aControls[(int)enControl::Neutral].m_bTouched = m_aControls[(int)enControl::Neutral].m_cDestination.isPointInside(m_cSteerPos);

            calculateSteer();
          }
        }
      }
#endif

      return a_bRet;
    }

    void CControllerTouchCentral::calculateSteer() {
      if (m_aControls[(int)enControl::Left].m_bTouched || m_aControls[(int)enControl::Right].m_bTouched) {
        bool l_bLeft = false;

        if (m_cSteerPos.X < m_aControls[(int)enControl::Left].m_cDestination.LowerRightCorner.X) {
          m_fSteer = ((irr::f32)(m_aControls[(int)enControl::Left].m_cDestination.LowerRightCorner.X - m_cSteerPos.X)) / (irr::f32)m_iThrottleHeight;
          l_bLeft = true;
        } else if (m_cSteerPos.X > m_aControls[(int)enControl::Right].m_cDestination.UpperLeftCorner.X) {
          m_fSteer = ((irr::f32)(m_cSteerPos.X - m_aControls[(int)enControl::Right].m_cDestination.UpperLeftCorner.X)) / (irr::f32)m_iThrottleHeight;
        } else {
          m_fSteer = 0.0f;
        }

        m_fSteer *= m_fSteer;

        if (m_fSteer < -1.0f) m_fSteer = -1.0f;
        if (m_fSteer > 1.0f) m_fSteer = 1.0f;

        if (l_bLeft)
          m_fSteer = -m_fSteer;
      }
      else m_fSteer = 0.0f;
    }
  }
}