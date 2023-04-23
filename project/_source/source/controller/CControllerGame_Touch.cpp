// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Touch.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CControllerGame_Touch::SControl::SControl() : m_pTextureOff(nullptr), m_pTextureOn(nullptr), m_bTouched(false), m_iTouchID(-1) {
    }

    CControllerGame_Touch::SControl::SControl(
      const irr::core::recti &a_cDestination,
      const irr::core::recti &a_cSource,
      const irr::video::SColor &a_cBackground,
      irr::video::ITexture *a_pOff,
      irr::video::ITexture *a_pOn
    ) :
      m_pTextureOff(a_pOff),
      m_pTextureOn (a_pOn),
      m_bTouched   (false),
      m_iTouchID   (-1)
    {
      m_cDestination = a_cDestination;
      m_cSource      = a_cSource;
      m_cBackground  = a_cBackground;
    }

    CControllerGame_Touch::CControllerGame_Touch(IControllerGame::enType a_eType,
                                                 const irr::core::recti &a_cViewport)
      : IControllerGame(a_eType), irr::gui::IGUIElement(
      (irr::gui::EGUI_ELEMENT_TYPE) g_TouchControlId,
      CGlobal::getInstance()->getGuiEnvironment(),
      CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(),
      -1,
      irr::core::recti(irr::core::position2di(0, 0),
                       CGlobal::getInstance()->getVideoDriver()->getScreenSize())
    ),
        m_iTouchSteer    (-1),
        m_iThrottleHeight(0),
        m_fSteer         (0.0f),
        m_pDrv           (CGlobal::getInstance()->getVideoDriver())
    {
      m_cViewport = a_cViewport;

      irr::core::vector2di l_cCenter = a_cViewport.getCenter();


      m_iThrottleHeight = m_cViewport.getHeight() / 8;

      irr::core::dimension2du l_cSize = irr::core::dimension2du (m_iThrottleHeight, m_iThrottleHeight);

      if (a_eType == IControllerGame::enType::Touch) {
        addToControlMap(enControl::Backward, irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() -     l_cSize.Height), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_back_off.png", "data/images/ctrl_back.png");
        addToControlMap(enControl::Neutral , irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128, 255, 255, 255), "data/images/ctrl_none_off.png", "data/images/ctrl_none.png");
        addToControlMap(enControl::Forward , irr::core::recti(irr::core::vector2di(   l_cCenter.X -     l_cSize.Width / 2, m_cViewport.getHeight() - 3 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png");
        addToControlMap(enControl::Left    , irr::core::recti(irr::core::vector2di(   l_cCenter.X - 3 * l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_left_off.png", "data/images/ctrl_left.png");
        addToControlMap(enControl::Right   , irr::core::recti(irr::core::vector2di(   l_cCenter.X +     l_cSize.Width / 2, m_cViewport.getHeight() - 2 * l_cSize.Height), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_right_off.png", "data/images/ctrl_right.png");
        addToControlMap(enControl::Rearview, irr::core::recti(irr::core::vector2di(                                     0,                                            0), l_cSize), irr::video::SColor(128, 255, 255,   0), "data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png");
        addToControlMap(enControl::Respawn , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cSize.Width,                                            0), l_cSize), irr::video::SColor(128, 255, 255,   0), "data/images/ctrl_respawn_off.png", "data/images/ctrl_respawn.png");

        m_cSteerRect.UpperLeftCorner .X = m_mControls[enControl::Left    ].m_cDestination.UpperLeftCorner .X;
        m_cSteerRect.UpperLeftCorner .Y = m_mControls[enControl::Forward ].m_cDestination.UpperLeftCorner .Y;
        m_cSteerRect.LowerRightCorner.X = m_mControls[enControl::Right   ].m_cDestination.LowerRightCorner.X;
        m_cSteerRect.LowerRightCorner.Y = m_mControls[enControl::Backward].m_cDestination.LowerRightCorner.Y;
      }
      else {
        addToControlMap(enControl::Left    , irr::core::recti(irr::core::vector2di(                                         0, m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2), l_cSize), irr::video::SColor(128,    0,   0, 255), "data/images/ctrl_left_off.png", "data/images/ctrl_left.png");
        addToControlMap(enControl::Right   , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight, m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2), l_cSize), irr::video::SColor(128,    0,   0, 255), "data/images/ctrl_right_off.png", "data/images/ctrl_right.png");
        addToControlMap(enControl::Rearview, irr::core::recti(irr::core::vector2di(                                         0,                                                   0), l_cSize), irr::video::SColor(128, 255, 255,    0), "data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png");
        addToControlMap(enControl::Respawn , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight,                                                   0), l_cSize), irr::video::SColor(128, 255, 255,    0), "data/images/ctrl_respawn_off.png", "data/images/ctrl_respawn.png");
        addToControlMap(enControl::BrakeL  , irr::core::recti(irr::core::vector2di(                                         0, m_cViewport.getHeight()     - m_iThrottleHeight    ), l_cSize), irr::video::SColor(128, 255,   0,    0), "data/images/ctrl_brake_off.png", "data/images/ctrl_brake.png");
        addToControlMap(enControl::BrakeR  , irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - m_iThrottleHeight, m_cViewport.getHeight()     - m_iThrottleHeight    ), l_cSize), irr::video::SColor(128, 255,   0,    0), "data/images/ctrl_brake_off.png", "data/images/ctrl_brake.png");
      }
    }

    CControllerGame_Touch::~CControllerGame_Touch() {
    }

    void CControllerGame_Touch::addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn) {
      irr::video::ITexture *l_pOff = m_pDrv->getTexture(a_sOff.c_str());
      irr::video::ITexture *l_pOn  = m_pDrv->getTexture(a_sOn .c_str());

      m_mControls[a_eControl] = SControl(a_cDestination, irr::core::recti(irr::core::vector2di(0, 0), l_pOff->getOriginalSize()), a_cColor, l_pOff, l_pOn);
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Touch::getThrottle() {
      if (IControllerGame::getType() == IControllerGame::enType::TouchSteer)
        return 0.0f;
      else
        return m_mControls[enControl::Forward].m_bTouched ? 1.0f : m_mControls[enControl::Backward].m_bTouched ? -1.0f : 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Touch::getSteer() {
      return m_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getBrake() {
      if (IControllerGame::getType() == IControllerGame::enType::Touch)
        return m_mControls[enControl::Backward].m_bTouched;
      else
        return m_mControls[enControl::BrakeL].m_bTouched || m_mControls[enControl::BrakeR].m_bTouched;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRearView() {
      return m_mControls.find(enControl::Rearview) != m_mControls.end() && m_mControls[enControl::Rearview].m_bTouched;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRespawn() {
      return m_mControls.find(enControl::Respawn) != m_mControls.end() && m_mControls[enControl::Respawn].m_bTouched;
    }

    /**
    * Get the "withdraw from race" button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::withdrawFromRace() {
      return false;
    }

    /**
    * Get the pause button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::pause() {
      return false;
    }

    /**
    * Process Irrlicht events to update the controls
    * @param a_cEvent event to process
    */
    void CControllerGame_Touch::updateControls(const irr::SEvent &a_cEvent) {
      OnEvent(a_cEvent);
    }

    /**
    * The player has finished, hide the UI elements if necessary
    */
    void CControllerGame_Touch::playerFinished() {
      setVisible(false);
    }

    /**
    * Implementation of the serialization method which does nothing in this case
    */
    void CControllerGame_Touch::serializeAttributes(irr::io::IAttributes *a_pOut,
                                                    irr::io::SAttributeReadWriteOptions *a_pOptions) const {
    }

    /**
    * Implementation of the deserialization method which does nothing in this case
    */
    void CControllerGame_Touch::deserializeAttributes(irr::io::IAttributes *a_pIn,
                                                      irr::io::SAttributeReadWriteOptions *a_pOptions) {
    }

    void CControllerGame_Touch::calculateSteer() {
      if (IControllerGame::getType() == IControllerGame::enType::Touch) {
        if (m_mControls[enControl::Left].m_bTouched || m_mControls[enControl::Right].m_bTouched) {
          bool l_bLeft = false;

          if (m_cSteerPos.X < m_mControls[enControl::Left].m_cDestination.LowerRightCorner.X) {
            m_fSteer = ((irr::f32)(m_mControls[enControl::Left].m_cDestination.LowerRightCorner.X - m_cSteerPos.X)) / (irr::f32)m_iThrottleHeight;
            l_bLeft = true;
          } else if (m_cSteerPos.X > m_mControls[enControl::Right].m_cDestination.UpperLeftCorner.X) {
            m_fSteer = ((irr::f32)(m_cSteerPos.X - m_mControls[enControl::Right].m_cDestination.UpperLeftCorner.X)) / (irr::f32)m_iThrottleHeight;
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
      else {
        if (m_mControls[enControl::Left].m_bTouched)
          m_fSteer = -1.0f;
        else if (m_mControls[enControl::Right].m_bTouched)
          m_fSteer = 1.0f;
        else
          m_fSteer = 0.0f;
      }
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CControllerGame_Touch::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

#ifdef _TOUCH_CONTROL
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          irr::core::vector2di l_cTouch = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

          if (IControllerGame::getType() == IControllerGame::enType::Touch) {
            if (m_cSteerRect.isPointInside(l_cTouch)) {
              m_iTouchSteer = a_cEvent.TouchInput.ID;
              m_cSteerPos   = l_cTouch;
            }

            for (std::map<enControl, SControl>::iterator l_itCtrl = m_mControls.begin(); l_itCtrl != m_mControls.end(); l_itCtrl++) {
              if ((*l_itCtrl).second.m_cDestination.isPointInside(l_cTouch)) {
                (*l_itCtrl).second.m_iTouchID = a_cEvent.TouchInput.ID;
                (*l_itCtrl).second.m_bTouched = true;
                break;
              }
            }
          }
          else {
            if (l_cTouch.X < m_cViewport.getWidth() / 2 && l_cTouch.Y > m_iThrottleHeight && l_cTouch.Y < m_cViewport.getHeight() - m_iThrottleHeight) {
              m_mControls[enControl::Left].m_bTouched = true;
              m_mControls[enControl::Left].m_iTouchID = a_cEvent.TouchInput.ID;
            }
            if (l_cTouch.X > m_cViewport.getWidth() / 2 && l_cTouch.Y >  m_iThrottleHeight && l_cTouch.Y < m_cViewport.getHeight() - m_iThrottleHeight) {
              m_mControls[enControl::Right].m_bTouched = true;
              m_mControls[enControl::Right].m_iTouchID = a_cEvent.TouchInput.ID;
            }
            if (l_cTouch.X < m_cViewport.getWidth() / 2 && l_cTouch.Y <= m_iThrottleHeight) {
              m_mControls[enControl::Rearview].m_bTouched = true;
              m_mControls[enControl::Rearview].m_iTouchID = a_cEvent.TouchInput.ID;
            }
            if (l_cTouch.X > m_cViewport.getWidth() / 2 && l_cTouch.Y <= m_iThrottleHeight) {
              m_mControls[enControl::Respawn ].m_bTouched = true;
              m_mControls[enControl::Respawn ].m_iTouchID = a_cEvent.TouchInput.ID;
            }
            if (l_cTouch.Y >= m_cViewport.getHeight() - m_iThrottleHeight) {
              if (l_cTouch.X < m_cViewport.getWidth() / 2) {
                m_mControls[enControl::BrakeL].m_bTouched = true;
                m_mControls[enControl::BrakeL].m_iTouchID = a_cEvent.TouchInput.ID;
              } else {
                m_mControls[enControl::BrakeR].m_bTouched = true;
                m_mControls[enControl::BrakeR].m_iTouchID = a_cEvent.TouchInput.ID;
              }
            }
          }

          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (IControllerGame::getType() == IControllerGame::enType::Touch) {
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
                m_mControls[l_aCtrl[i]].m_bTouched = false;
                m_mControls[l_aCtrl[i]].m_iTouchID = -1;
              }

              if (!m_mControls[enControl::Left].m_bTouched && !m_mControls[enControl::Right].m_bTouched) {
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
                if (a_cEvent.TouchInput.ID == m_mControls[l_aCtrl[i]].m_iTouchID) {
                  m_mControls[l_aCtrl[i]].m_bTouched = false;
                  m_mControls[l_aCtrl[i]].m_iTouchID = -1;
                }
              }
            }
          }
          else {
            for (std::map<enControl, SControl>::iterator l_itCtrl = m_mControls.begin(); l_itCtrl != m_mControls.end(); l_itCtrl++) {
              if ((*l_itCtrl).second.m_iTouchID == a_cEvent.TouchInput.ID) {
                (*l_itCtrl).second.m_iTouchID = -1;
                (*l_itCtrl).second.m_bTouched = false;
              }
            }
          }
          calculateSteer();
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (IControllerGame::getType() == IControllerGame::enType::Touch) {
            if (a_cEvent.TouchInput.ID == m_iTouchSteer) {
              m_cSteerPos = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

              m_mControls[enControl::Left    ].m_bTouched = m_cSteerPos.X <= m_mControls[enControl::Left    ].m_cDestination.LowerRightCorner.X;
              m_mControls[enControl::Right   ].m_bTouched = m_cSteerPos.X >= m_mControls[enControl::Right   ].m_cDestination.UpperLeftCorner .X;
              m_mControls[enControl::Forward ].m_bTouched = m_cSteerPos.Y <= m_mControls[enControl::Forward ].m_cDestination.LowerRightCorner.Y;
              m_mControls[enControl::Backward].m_bTouched = m_cSteerPos.Y >= m_mControls[enControl::Backward].m_cDestination.UpperLeftCorner .Y;

              m_mControls[enControl::Neutral].m_bTouched = m_mControls[enControl::Neutral].m_cDestination.isPointInside(m_cSteerPos);

              calculateSteer();
            }
          }
          else {
            if (m_mControls[enControl::Left].m_bTouched && m_mControls[enControl::Left].m_iTouchID == a_cEvent.TouchInput.ID) {
              m_mControls[enControl::BrakeL].m_bTouched = a_cEvent.TouchInput.Y >= m_cViewport.getHeight() - m_iThrottleHeight;
              m_mControls[enControl::BrakeL].m_iTouchID = a_cEvent.TouchInput.ID;
            }

            if (m_mControls[enControl::Right].m_bTouched && m_mControls[enControl::Right].m_iTouchID == a_cEvent.TouchInput.ID) {
              m_mControls[enControl::BrakeR].m_bTouched = a_cEvent.TouchInput.Y >= m_cViewport.getHeight() - m_iThrottleHeight;
              m_mControls[enControl::BrakeR].m_iTouchID = a_cEvent.TouchInput.ID;
            }
          }
        }
      }
#endif

      return l_bRet;
    }

    void CControllerGame_Touch::draw() {
      if (IsVisible) {
        for (std::map<enControl, SControl>::iterator l_itCtrl = m_mControls.begin(); l_itCtrl != m_mControls.end(); l_itCtrl++) {
          irr::video::ITexture *l_pTexture = (*l_itCtrl).second.m_bTouched ? (*l_itCtrl).second.m_pTextureOn : (*l_itCtrl).second.m_pTextureOff;

          if (l_pTexture != nullptr) {
            m_pDrv->draw2DImage(
              l_pTexture,
              (*l_itCtrl).second.m_cDestination,
              (*l_itCtrl).second.m_cSource,
              nullptr,
              nullptr,
              true
            );
          }
          else {
            m_pDrv->draw2DRectangle((*l_itCtrl).second.m_cBackground, (*l_itCtrl).second.m_cDestination);
          }
        }
      }
    }
  }
}