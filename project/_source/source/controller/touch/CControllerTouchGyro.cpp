// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/touch/CControllerTouchGyro.h>

namespace dustbin {
  namespace controller {
    CControllerTouchGyro::CControllerTouchGyro(irr::video::IVideoDriver* a_pDrv, const irr::core::recti& a_cViewport) :
      CControllerTouchSteerOnly(a_pDrv, a_cViewport),
      m_fGyroX  (0.0f),
      m_pGyroOne(nullptr),
      m_pGyroTwo(nullptr)
    {
      m_pGyroOne = m_pDrv->getTexture("data/images/gyro1.png");
      m_pGyroTwo = m_pDrv->getTexture("data/images/gyro2.png");

      m_cIconSrc = irr::core::recti(irr::core::position2di(0, 0),
                                    m_pGyroOne     != nullptr ? m_pGyroOne    ->getOriginalSize() :
                                    m_pGyroTwo     != nullptr ? m_pGyroTwo    ->getOriginalSize() : irr::core::dimension2du(256, 256)
      );

      irr::core::dimension2du l_cIconSize = irr::core::dimension2du(a_cViewport.getWidth() / 16, a_cViewport.getWidth() / 16);
      irr::core::vector2di    l_cCenter   = a_cViewport.getCenter();

      m_cGyroOne = irr::core::recti(l_cCenter - irr::core::vector2di(l_cIconSize.Width / 2, l_cIconSize.Height / 2), l_cIconSize);

      m_aControls[(int)enControl::Left    ].m_bActive = false;
      m_aControls[(int)enControl::Right   ].m_bActive = false;
      m_aControls[(int)enControl::GyroRstL].m_bActive = true;
      m_aControls[(int)enControl::GyroRstR].m_bActive = true;
    }

    CControllerTouchGyro::~CControllerTouchGyro() {
    }

    /**
    * Returns the steer (-1 .. 1): left - right
    * @return the steer state
    */
    irr::f32 CControllerTouchGyro::getSteer() {
      return std::abs(m_fGyroX) < 2.5f ? 0.0f : m_fGyroX > 12.5f ? 1.0f : m_fGyroX < -12.5f ? -1.0f : m_fGyroX < 0.0f ? (m_fGyroX + 2.5f) / 10.0f : (m_fGyroX - 2.5f) / 10.0f;;
    }

    /**
    * Handle an event
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CControllerTouchGyro::handleEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_GYROSCOPE_EVENT) {
        m_fGyroX -= a_cEvent.GyroscopeEvent.Z;

        l_bRet = true;
      }
#endif

      if (!l_bRet) {
        l_bRet = CControllerTouchSteerOnly::handleEvent(a_cEvent);
      }

      return l_bRet;
    }

    /**
    * Draw the controller
    */
    void CControllerTouchGyro::draw() {
      CControllerTouchSteerOnly::draw();

      if (m_pGyroOne != nullptr) {
        m_pDrv->draw2DImage(m_pGyroOne, m_cGyroOne, m_cIconSrc, &m_cViewport, nullptr, true);
      }

      if (m_pGyroTwo != nullptr) {
        irr::s32 l_iX = m_cViewport.getCenter().X + (irr::s32)(getSteer() * (irr::f32)m_cViewport.getWidth() / 8.0f);
        irr::s32 l_iY = m_cViewport.getCenter().Y;

        l_iX -= m_cGyroOne.getWidth () / 2;
        l_iY -= m_cGyroOne.getHeight() / 2;

        irr::core::recti l_cRect = irr::core::recti(irr::core::position2di(l_iX, l_iY), m_cGyroOne.getSize());

        m_pDrv->draw2DImage(m_pGyroTwo, l_cRect, m_cIconSrc, &m_cViewport, nullptr, true);
      }

      if (m_aControls[(int)enControl::GyroRstL].m_bTouched || m_aControls[(int)enControl::GyroRstR].m_bTouched) {
        m_fGyroX = 0.0f;
      }
    }
  }
}