// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Gyro.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CControllerGame_Gyro::CControllerGame_Gyro(IControllerGame::enType a_eType, const irr::core::recti &a_cViewport, bool a_bSteerOnly)  : IControllerGame(a_eType), irr::gui::IGUIElement(
      (irr::gui::EGUI_ELEMENT_TYPE)g_GyroControlId,
      CGlobal::getInstance()->getGuiEnvironment(), 
      CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
      -1, 
      irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
    ),
      m_pDrv      (CGlobal::getInstance()->getVideoDriver()),
      m_pGyroOne  (nullptr),
      m_pGyroTwo  (nullptr),
      m_pReset    (nullptr),
      m_fGyroX    (0.0f),
      m_fGyroY    (0.0f),
      m_iRespawn  (-1),
      m_iRearview (-1),
      m_bSteerOnly(a_bSteerOnly)
    {
      m_cViewport = a_cViewport;

      m_pGyroOne = m_pDrv->getTexture("data/images/gyro1.png");
      m_pGyroTwo = m_pDrv->getTexture("data/images/gyro2.png");

      m_pReset   = m_pDrv->getTexture("data/images/gyro_reset.png");

      m_pRespawn[0] = m_pDrv->getTexture("data/images/ctrl_respawn_off.png");
      m_pRespawn[1] = m_pDrv->getTexture("data/images/ctrl_respawn.png");

      m_pRearview[0] = m_pDrv->getTexture("data/images/ctrl_rearview_off.png");
      m_pRearview[1] = m_pDrv->getTexture("data/images/ctrl_rearview.png");

      m_cIconSrc = irr::core::recti(irr::core::position2di(0, 0),
        m_pGyroOne     != nullptr ? m_pGyroOne    ->getOriginalSize() :
        m_pGyroTwo     != nullptr ? m_pGyroTwo    ->getOriginalSize() :
        m_pReset       != nullptr ? m_pReset      ->getOriginalSize() :
        m_pRespawn [0] != nullptr ? m_pRespawn [0]->getOriginalSize() :
        m_pRespawn [1] != nullptr ? m_pRespawn [1]->getOriginalSize() :
        m_pRearview[0] != nullptr ? m_pRearview[0]->getOriginalSize() :
        m_pRearview[1] != nullptr ? m_pRearview[1]->getOriginalSize() : irr::core::dimension2du(256, 256)
      );

      irr::core::dimension2du l_cIconSize = irr::core::dimension2du(a_cViewport.getWidth() / 16, a_cViewport.getWidth() / 16);
      irr::core::vector2di    l_cCenter   = a_cViewport.getCenter();

      m_cGyroOne = irr::core::recti(l_cCenter - irr::core::vector2di(l_cIconSize.Width / 2, l_cIconSize.Height / 2), l_cIconSize);

      irr::core::vector2di l_cLeft = irr::core::vector2di(                                         0, a_cViewport.getHeight() / 2 - l_cIconSize.Height / 2);
      irr::core::vector2di l_cRght = irr::core::vector2di(a_cViewport.getWidth() - l_cIconSize.Width, a_cViewport.getHeight() / 2 - l_cIconSize.Height / 2);

      m_cReset[0] = irr::core::recti(l_cLeft, l_cIconSize);
      m_cReset[1] = irr::core::recti(l_cRght, l_cIconSize);

      l_cLeft.Y = 3 * (a_cViewport.getHeight() - l_cIconSize.Height) / 4;
      l_cRght.Y = 3 * (a_cViewport.getHeight() - l_cIconSize.Height) / 4;

      m_cRespawn[0] = irr::core::recti(l_cLeft, l_cIconSize);
      m_cRespawn[1] = irr::core::recti(l_cRght, l_cIconSize);

      l_cLeft.Y = a_cViewport.getHeight() - l_cIconSize.Height;
      l_cRght.Y = a_cViewport.getHeight() - l_cIconSize.Height;

      m_cRearview[0] = irr::core::recti(l_cLeft, l_cIconSize);
      m_cRearview[1] = irr::core::recti(l_cRght, l_cIconSize);
    }

    CControllerGame_Gyro::~CControllerGame_Gyro() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Gyro::getThrottle() {
      return std::abs(m_fGyroY) < 2.5f ? 0.0f : m_fGyroY > 12.5f ? 1.0f : m_fGyroY < -12.5f ? -1.0f : m_fGyroY < 0.0f ? (m_fGyroY + 2.5f) / 10.0f : (m_fGyroY - 2.5f) / 10.0f;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Gyro::getSteer() {
      return std::abs(m_fGyroX) < 2.5f ? 0.0f : m_fGyroX > 12.5f ? 1.0f : m_fGyroX < -12.5f ? -1.0f : m_fGyroX < 0.0f ? (m_fGyroX + 2.5f) / 10.0f : (m_fGyroX - 2.5f) / 10.0f;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Gyro::getBrake() {
      return m_fGyroY > 7.5f;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Gyro::getRearView() {
      return m_iRearview != -1;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Gyro::getRespawn() {
      return m_iRespawn != -1;
    }

    /**
    * Get the "withdraw from race" button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Gyro::withdrawFromRace() {
      return false;
    }

    /**
    * Get the pause button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Gyro::pause() {
      return false;
    }

    /**
    * Process Irrlicht evnts to update the controls
    * @param a_cEvent event to process
    */
    void CControllerGame_Gyro::updateControls(const irr::SEvent& a_cEvent) {
#ifdef _TOUCH_CONTROL
      if (a_cEvent.EventType == irr::EET_GYROSCOPE_EVENT) {
        m_fGyroX -= a_cEvent.GyroscopeEvent.Z;
        m_fGyroY += a_cEvent.GyroscopeEvent.Y;
      }
      else if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          irr::core::vector2di l_cPos = irr::core::vector2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

          for (int i = 0; i < 2; i++) {
            if (m_cRearview[i].isPointInside(l_cPos)) {
              m_iRearview = a_cEvent.TouchInput.ID;
              break;
            }
            else if (m_cRespawn[i].isPointInside(l_cPos)) {
              m_iRespawn = a_cEvent.TouchInput.ID;
              break;
            }
            else if (m_cReset[i].isPointInside(l_cPos)) {
              m_fGyroX = 0.0f;
              m_fGyroY = 0.0f;
              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (a_cEvent.TouchInput.ID == m_iRearview)
            m_iRearview = -1;
          else if (a_cEvent.TouchInput.ID == m_iRespawn)
            m_iRespawn = -1;
        }
      }
#else
#endif
    }

    /**
    * Implementation of the serialization method which does nothing in this case
    */
    void CControllerGame_Gyro::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {

    }

    /**
    * Implementation of the deserialization method which does nothing in this case
    */
    void CControllerGame_Gyro::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {

    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CControllerGame_Gyro::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;


      return l_bRet;
    }

    /**
    * The player has finished, hide the UI elements if necessary
    */
    void CControllerGame_Gyro::playerFinished() {
      setVisible(false);
    }

    /**
     * If this controller has an UI this method will move it to the front.
     * The Android touch and gyroscope controllers have an UI
     */
    void CControllerGame_Gyro::moveGuiToFront() {
      CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement()->bringToFront(this);
    }

    void CControllerGame_Gyro::draw() {
      if (IsVisible) {
        irr::video::ITexture *l_pImage = m_pReset;

        if (m_pReset != nullptr) {
          m_pDrv->draw2DImage(l_pImage, m_cReset[0], m_cIconSrc, &m_cViewport, nullptr, true);
          m_pDrv->draw2DImage(l_pImage, m_cReset[1], m_cIconSrc, &m_cViewport, nullptr, true);
        }

        l_pImage = m_iRespawn == -1 ? m_pRespawn[0] : m_pRespawn[1];

        if (l_pImage != nullptr) {
          m_pDrv->draw2DImage(l_pImage, m_cRespawn[0], m_cIconSrc, &m_cViewport, nullptr, true);
          m_pDrv->draw2DImage(l_pImage, m_cRespawn[1], m_cIconSrc, &m_cViewport, nullptr, true);
        }

        l_pImage = m_iRearview == -1 ? m_pRearview[0] : m_pRearview[1];

        if (l_pImage != nullptr) {
          m_pDrv->draw2DImage(l_pImage, m_cRearview[0], m_cIconSrc, &m_cViewport, nullptr, true);
          m_pDrv->draw2DImage(l_pImage, m_cRearview[1], m_cIconSrc, &m_cViewport, nullptr, true);
        }

        if (m_pGyroOne != nullptr) {
          m_pDrv->draw2DImage(m_pGyroOne, m_cGyroOne, m_cIconSrc, &m_cViewport, nullptr, true);
        }

        if (m_pGyroTwo != nullptr) {
          irr::s32 l_iX = m_cViewport.getCenter().X + (irr::s32)(getSteer() * (irr::f32)m_cViewport.getWidth() / 8.0f);
          irr::s32 l_iY = m_cViewport.getCenter().Y;

          if (!m_bSteerOnly) {
            l_iY -= (irr::s32)(getThrottle() * (irr::f32)m_cViewport.getHeight() / 8.0f);
          }

          l_iX -= m_cGyroOne.getWidth () / 2;
          l_iY -= m_cGyroOne.getHeight() / 2;

          irr::core::recti l_cRect = irr::core::recti(irr::core::position2di(l_iX, l_iY), m_cGyroOne.getSize());

          m_pDrv->draw2DImage(m_pGyroTwo, l_cRect, m_cIconSrc, &m_cViewport, nullptr, true);
        }
      }
    }
  }
}