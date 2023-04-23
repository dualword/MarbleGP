// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/touch/CControllerTouchSide.h>

namespace dustbin {
  namespace controller {
    CControllerTouchSide::CControllerTouchSide(irr::video::IVideoDriver* a_pDrv, const irr::core::recti &a_cViewport) : ITouchController(a_pDrv, a_cViewport),
      m_iThrottleHeight(0),
      m_iLeftID        (-1),
      m_iRightID       (-1),
      m_iCenterX       (a_cViewport.getCenter().X),
      m_iSteerLeft     (0),
      m_iSteerRght     (0),
      m_iSteerY1       (0),
      m_iSteerY2       (0),
      m_iForward       (a_cViewport.getHeight() / 3),
      m_iBackward      (2 * a_cViewport.getHeight() / 3),
      m_fThrottle      (0.0f),
      m_fSteer         (0.0f),
      m_bBrake         (false)
    {
      m_cViewport = a_cViewport;

      irr::core::vector2di l_cCenter = a_cViewport.getCenter();

      m_iThrottleHeight = m_cViewport.getHeight() / 8;

      irr::core::dimension2du l_cSize = irr::core::dimension2du (m_iThrottleHeight, m_iThrottleHeight);

      m_iSteerLeft = m_iThrottleHeight;
      m_iSteerRght = m_cViewport.getWidth() - m_iThrottleHeight;

      irr::s32 l_iRight  = m_cViewport.getWidth() - m_iThrottleHeight;
      irr::s32 l_iCenter = m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2;

      addToControlMap(enControl::ForwardL , irr::core::recti(irr::core::vector2di(       0,                               m_iThrottleHeight), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png");
      addToControlMap(enControl::BackwardL, irr::core::recti(irr::core::vector2di(       0, m_cViewport.getHeight() - 2 * m_iThrottleHeight), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_back_off.png"      , "data/images/ctrl_back.png");
      addToControlMap(enControl::ForwardR , irr::core::recti(irr::core::vector2di(l_iRight,                               m_iThrottleHeight), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png");
      addToControlMap(enControl::BackwardR, irr::core::recti(irr::core::vector2di(l_iRight, m_cViewport.getHeight() - 2 * m_iThrottleHeight), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_back_off.png"      , "data/images/ctrl_back.png");
      addToControlMap(enControl::BrakeL   , irr::core::recti(irr::core::vector2di(       0, m_cViewport.getHeight() -     m_iThrottleHeight), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_brake_off.png"     , "data/images/ctrl_brake.png");
      addToControlMap(enControl::BrakeR   , irr::core::recti(irr::core::vector2di(l_iRight, m_cViewport.getHeight() -     m_iThrottleHeight), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_brake_off.png"     , "data/images/ctrl_brake.png");
      addToControlMap(enControl::Left     , irr::core::recti(irr::core::vector2di(       0,                                       l_iCenter), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_left_off.png"      , "data/images/ctrl_left.png");
      addToControlMap(enControl::Right    , irr::core::recti(irr::core::vector2di(l_iRight,                                       l_iCenter), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_right_off.png"     , "data/images/ctrl_right.png");
      addToControlMap(enControl::Rearview , irr::core::recti(irr::core::vector2di(       0,                                               0), l_cSize), irr::video::SColor(128, 255,   0, 255), "data/images/ctrl_rearview_off.png"  , "data/images/ctrl_rearview.png");
      addToControlMap(enControl::Respawn  , irr::core::recti(irr::core::vector2di(l_iRight,                                               0), l_cSize), irr::video::SColor(128, 255,   0, 255), "data/images/ctrl_respawn_off.png"   , "data/images/ctrl_respawn.png");

      m_iSteerY1 = m_aControls[(int)enControl::ForwardL ].m_cDestination.LowerRightCorner.Y;
      m_iSteerY2 = m_aControls[(int)enControl::BackwardL].m_cDestination.UpperLeftCorner .Y;
    }

    CControllerTouchSide::~CControllerTouchSide() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerTouchSide::getThrottle() {
      return m_fThrottle;
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchSide::getSteer() {
      return m_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerTouchSide::getBrake() {
      return m_bBrake || m_aControls[(int)enControl::BrakeL].m_bTouched || m_aControls[(int)enControl::BrakeR].m_bTouched;
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
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {


          if (m_aControls[(int)enControl::Respawn].m_cDestination.isPointInside(l_cTouch)) {
            m_aControls[(int)enControl::Respawn].m_bTouched = true;
            m_aControls[(int)enControl::Respawn].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          else if (m_aControls[(int)enControl::Rearview].m_cDestination.isPointInside(l_cTouch)) {
            m_aControls[(int)enControl::Rearview].m_bTouched = true;
            m_aControls[(int)enControl::Rearview].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          else if (m_aControls[(int)enControl::BrakeL].m_cDestination.isPointInside(l_cTouch)) {
            m_aControls[(int)enControl::BrakeL].m_bTouched = true;
            m_aControls[(int)enControl::BrakeL].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          else if (m_aControls[(int)enControl::BrakeR].m_cDestination.isPointInside(l_cTouch)) {
            m_aControls[(int)enControl::BrakeR].m_bTouched = true;
            m_aControls[(int)enControl::BrakeR].m_iTouchID = a_cEvent.TouchInput.ID;
          }
          else {
            if (l_cTouch.X < m_iCenterX) {
              m_iLeftID = a_cEvent.TouchInput.ID;
              updateSteering(l_cTouch);
            }
            else {
              m_iRightID = a_cEvent.TouchInput.ID;
              updateSteering(l_cTouch);
            }
          }
          l_bRet = true;
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (m_iLeftID == a_cEvent.TouchInput.ID) {
            m_iLeftID = -1;
            l_bRet = true;
            updateSteering(l_cTouch);
          }
          else if (m_iRightID == a_cEvent.TouchInput.ID) {
            m_iRightID = -1;
            updateSteering(l_cTouch);
            l_bRet = true;
          }
          else if (m_aControls[(int)enControl::Rearview].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::Rearview].m_bTouched = false;
            m_aControls[(int)enControl::Rearview].m_iTouchID = -1;
            l_bRet = true;
          }
          else if (m_aControls[(int)enControl::Respawn].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::Respawn].m_bTouched = false;
            m_aControls[(int)enControl::Respawn].m_iTouchID = -1;
            l_bRet = true;
          }
          else if (m_aControls[(int)enControl::BrakeL].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::BrakeL].m_bTouched = false;
            m_aControls[(int)enControl::BrakeL].m_iTouchID = -1;
          }
          else if (m_aControls[(int)enControl::BrakeR].m_iTouchID == a_cEvent.TouchInput.ID) {
            m_aControls[(int)enControl::BrakeR].m_bTouched = false;
            m_aControls[(int)enControl::BrakeR].m_iTouchID = -1;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          updateSteering(l_cTouch);
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

    void CControllerTouchSide::updateSteering(const irr::core::vector2di &a_cTouch) {
      m_fSteer = 0.0f;

      if (m_iLeftID != -1 && a_cTouch.X < m_iSteerLeft && a_cTouch.Y > m_iForward && a_cTouch.Y < m_iBackward) {
        m_fSteer = -1.0f;
      }

      if (m_iRightID != -1 && a_cTouch.X > m_iSteerRght && a_cTouch.Y > m_iForward && a_cTouch.Y < m_iBackward) {
        m_fSteer += 1.0f;
      }

      m_fThrottle = 0.0f;

      if (m_iLeftID != -1 || m_iRightID != -1) {
        if (a_cTouch.Y < m_iForward)
          m_fThrottle = 1.0f;

        if (a_cTouch.Y > m_iBackward) {
          m_fThrottle -= 1.0f;
        }

        m_bBrake = a_cTouch.Y > m_aControls[(int)enControl::BackwardL].m_cDestination.getCenter().Y;
      }

      m_aControls[(int)enControl::Left     ].m_bTouched = m_fSteer < 0.0f;
      m_aControls[(int)enControl::Right    ].m_bTouched = m_fSteer > 0.0f;
      m_aControls[(int)enControl::ForwardL ].m_bTouched = m_fThrottle > 0.0f;
      m_aControls[(int)enControl::ForwardR ].m_bTouched = m_fThrottle > 0.0f;
      m_aControls[(int)enControl::BackwardL].m_bTouched = m_fThrottle < 0.0f;
      m_aControls[(int)enControl::BackwardR].m_bTouched = m_fThrottle < 0.0f;
    }
  }
}