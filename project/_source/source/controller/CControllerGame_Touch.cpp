// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Touch.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
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
        m_iTouchThrottle (-1),
        m_iTouchSteer    (-1),
        m_iTouchBrake    (-1),
        m_iTouchRearview (-1),
        m_iTouchRespawn  (-1),
        m_iThrottleY1    (0),
        m_iThrottleY2    (0),
        m_iThrottleY3    (0),
        m_iSteerX1       (0),
        m_iSteerX2       (0),
        m_iThrottleHeight(0),
        m_iSteeringWidth (0),
        m_fThrottle      (0.0f),
        m_fSteering      (0.0f),
        m_bBrkThrottle   (false),
        m_pDrv           (CGlobal::getInstance()->getVideoDriver()),
        m_pSpeedForward  (nullptr),
        m_pSpeedBackward (nullptr),
        m_pSteerLeft     (nullptr),
        m_pSteerRight    (nullptr)
    {
      m_cViewport = a_cViewport;

      irr::core::dimension2du l_cImage = irr::core::dimension2du(m_cViewport.getHeight() / 8, m_cViewport.getHeight() / 8);

      m_pSpeedForward    = m_pDrv->getTexture("data/images/ctrl_accelerate.png");
      m_pSpeedBackward   = m_pDrv->getTexture("data/images/ctrl_back.png");
      m_pSteerLeft       = m_pDrv->getTexture("data/images/ctrl_left.png");
      m_pSteerRight      = m_pDrv->getTexture("data/images/ctrl_right.png");
      m_pSpeedNeutral[0] = m_pDrv->getTexture("data/images/ctrl_nothrottle.png");
      m_pSpeedNeutral[1] = m_pDrv->getTexture("data/images/ctrl_nothrottle_on.png");
      m_pSteerNeutral[0] = m_pDrv->getTexture("data/images/ctrl_none_off.png");
      m_pSteerNeutral[1] = m_pDrv->getTexture("data/images/ctrl_none.png");
      m_pBrake       [0] = m_pDrv->getTexture("data/images/ctrl_brake_off.png");
      m_pBrake       [1] = m_pDrv->getTexture("data/images/ctrl_brake.png");
      m_pRespawn     [0] = m_pDrv->getTexture("data/images/ctrl_respawn_off.png");
      m_pRespawn     [1] = m_pDrv->getTexture("data/images/ctrl_respawn.png");
      m_pRearview    [0] = m_pDrv->getTexture("data/images/ctrl_rearview_off.png");
      m_pRearview    [1] = m_pDrv->getTexture("data/images/ctrl_rearview.png");

      m_cImage = irr::core::recti(irr::core::vector2di(0, 0), m_pSpeedBackward->getOriginalSize());

      m_cSpeedNeutral[0] = irr::core::recti(irr::core::vector2di(                                      0, m_cViewport.getHeight() / 2 - l_cImage.Height / 2), l_cImage);
      m_cSpeedNeutral[1] = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cImage.Width, m_cViewport.getHeight() / 2 - l_cImage.Height / 2), l_cImage);

      m_cBrake[0] = irr::core::recti(irr::core::vector2di(                                      0, m_cViewport.getHeight() - l_cImage.Height), l_cImage);
      m_cBrake[1] = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cImage.Width, m_cViewport.getHeight() - l_cImage.Height), l_cImage);

      m_cSteerNeutral = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() / 2 - l_cImage.Width / 2, m_cViewport.getHeight() - l_cImage.Height), l_cImage);
      m_cRearView     = irr::core::recti(irr::core::vector2di(0, 0), l_cImage);
      m_cRespawn      = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cImage.Width, 0), l_cImage);

      m_cTouchSteer = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() / 4, 0), irr::core::dimension2du(m_cViewport.getWidth() / 2, m_cViewport.getHeight()));

      m_cTouchSpeed[0] = irr::core::recti(irr::core::vector2di(0                                      , l_cImage.Height), irr::core::dimension2du(m_cViewport.getWidth() / 4, m_cViewport.getHeight() - 2 * l_cImage.Height));
      m_cTouchSpeed[1] = irr::core::recti(irr::core::vector2di(m_cViewport.getWidth() - l_cImage.Width, l_cImage.Height), irr::core::dimension2du(m_cViewport.getWidth() / 4, m_cViewport.getHeight() - 2 * l_cImage.Height));

      m_iThrottleY1 = m_cSpeedNeutral[0].UpperLeftCorner .Y;
      m_iThrottleY2 = m_cSpeedNeutral[0].LowerRightCorner.Y;
      m_iThrottleY3 = m_cViewport.getHeight() - 3 * m_cBrake[0].getHeight();

      m_iSteerX1 = m_cSteerNeutral.UpperLeftCorner .X;
      m_iSteerX2 = m_cSteerNeutral.LowerRightCorner.X;

      m_iSteeringWidth  = m_cViewport.getWidth () / 10;
      m_iThrottleHeight = m_cViewport.getHeight() / 10;
    }

    CControllerGame_Touch::~CControllerGame_Touch() {
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Touch::getThrottle() {
      return m_fThrottle;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Touch::getSteer() {
      return m_fSteering;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getBrake() {
      return m_iTouchBrake != -1 || m_bBrkThrottle;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRearView() {
      return m_iTouchRearview != -1;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRespawn() {
      return m_iTouchRespawn != -1;
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

          if (m_cBrake[0].isPointInside(l_cTouch) || m_cBrake[1].isPointInside(l_cTouch))
            m_iTouchBrake = a_cEvent.TouchInput.ID;
          else if (m_cRearView.isPointInside(l_cTouch))
            m_iTouchRearview = a_cEvent.TouchInput.ID;
          else if (m_cRespawn.isPointInside(l_cTouch))
            m_iTouchRespawn = a_cEvent.TouchInput.ID;
          else if (m_cTouchSpeed[0].isPointInside(l_cTouch) || m_cTouchSpeed[1].isPointInside(l_cTouch))
            m_iTouchThrottle = a_cEvent.TouchInput.ID;
          else if (m_cTouchSteer.isPointInside(l_cTouch))
            m_iTouchSteer = a_cEvent.TouchInput.ID;
        } else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (m_iTouchBrake == a_cEvent.TouchInput.ID)
            m_iTouchBrake = -1;
          else if (m_iTouchRearview == a_cEvent.TouchInput.ID)
            m_iTouchRearview = -1;
          else if (m_iTouchRespawn == a_cEvent.TouchInput.ID)
            m_iTouchRespawn = -1;
          else if (m_iTouchThrottle == a_cEvent.TouchInput.ID) {
            m_iTouchThrottle = -1;
            m_fThrottle = 0.0f;
          }
          else if (m_iTouchSteer == a_cEvent.TouchInput.ID) {
            m_iTouchSteer = -1;
            m_fSteering = 0.0f;
          }
        } else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (a_cEvent.TouchInput.ID == m_iTouchThrottle) {
            irr::s32 y = a_cEvent.TouchInput.Y;

            if (y < m_iThrottleY1) {
              m_fThrottle = ((irr::f32)m_iThrottleY1 - (irr::f32)y) / (irr::f32)m_iThrottleHeight;
              if (m_fThrottle > 1.0f)
                m_fThrottle = 1.0f;
            }
            else if (y > m_iThrottleY2) {
              m_fThrottle = -(((irr::f32)y - (irr::f32)m_iThrottleY2) / (irr::f32)m_iThrottleHeight);
              if (m_fThrottle < -1.0f)
                m_fThrottle = -1.0f;
            }

            m_bBrkThrottle = y > m_iThrottleY3;
          }
          else if (a_cEvent.TouchInput.ID == m_iTouchSteer) {
            irr::s32 x = a_cEvent.TouchInput.X;

            if (x < m_iSteerX1) {
              m_fSteering = -((irr::f32)m_iSteerX1 - (irr::f32)x) / (irr::f32)m_iSteeringWidth;
              if (m_fSteering < -1.0f)
                m_fSteering = -1.0f;
            }
            else if (x > m_iSteerX2) {
              m_fSteering = ((irr::f32)x - (irr::f32)m_iSteerX2) / (irr::f32)m_iSteeringWidth;
              if (m_fSteering > 1.0f)
                m_fSteering = 1.0f;
            }
          }
        }
      }
#endif

      return l_bRet;
    }

    void CControllerGame_Touch::draw() {
      if (IsVisible) {
        m_pDrv->draw2DImage(m_iTouchThrottle != -1 ? m_pSpeedNeutral[1] : m_pSpeedNeutral[0], m_cSpeedNeutral[0], m_cImage, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_iTouchThrottle != -1 ? m_pSpeedNeutral[1] : m_pSpeedNeutral[0], m_cSpeedNeutral[1], m_cImage, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_iTouchSteer    != -1 ? m_pSteerNeutral[1] : m_pSteerNeutral[0], m_cSteerNeutral   , m_cImage, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_iTouchRearview != -1 ?  m_pRearview   [1] : m_pRearview    [0], m_cRearView       , m_cImage, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_iTouchRespawn  != -1 ? m_pRespawn     [1] : m_pRespawn     [0], m_cRespawn        , m_cImage, nullptr, nullptr, true);

        m_pDrv->draw2DImage(m_iTouchBrake != -1 || m_bBrkThrottle ? m_pBrake[1] : m_pBrake[0], m_cBrake[0], m_cImage, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_iTouchBrake != -1 || m_bBrkThrottle ? m_pBrake[1] : m_pBrake[0], m_cBrake[1], m_cImage, nullptr, nullptr, true);
      }
    }
  }
}