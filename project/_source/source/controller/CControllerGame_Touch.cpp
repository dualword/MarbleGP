// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Touch.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CControllerGame_Touch::CControllerGame_Touch(IControllerGame::enType a_eType, const irr::core::recti &a_cViewport) : irr::gui::IGUIElement(
        (irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId,
        CGlobal::getInstance()->getGuiEnvironment(), 
        CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
        -1, 
        irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
      ),
      IControllerGame(a_eType),
      m_pDrv         (CGlobal::getInstance()->getVideoDriver())
    {
      m_cViewport = a_cViewport;

      for (int i = 0; i < (int)enControls::Count; i++)
        m_aControls[i] = nullptr;

      irr::core::dimension2du l_cTouchSize = irr::core::dimension2du(
        a_cViewport.getSize().Width / 16,
        a_cViewport.getSize().Width / 16
      );

      irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();


      if (a_eType == IControllerGame::enType::TouchLeft) {
        createSteeringControls(true);
        createThrottleControls(false);
      }
      else if (a_eType == IControllerGame::enType::TouchRight) {
        createSteeringControls(false);
        createThrottleControls(true);
      }
      else if (a_eType == IControllerGame::enType::TouchSteer) {
        int l_iHeightOffset = 2 * l_cTouchSize.Height;

        irr::core::recti l_cScreenRect = irr::core::recti(irr::core::vector2di(0, (l_cScreen.Height - l_cTouchSize.Height) / 2), l_cTouchSize);
        irr::core::recti l_cTouchRect  = irr::core::recti(irr::core::vector2di(0, l_iHeightOffset), irr::core::dimension2du(l_cScreen.Width / 3, l_cScreen.Height - l_iHeightOffset));

        m_aControls[(int)enControls::Left] = new STouchInput("data/images/ctrl_left_off.png", "data/images/ctrl_left.png", l_cScreenRect, l_cTouchRect, m_pDrv);

        l_cScreenRect = irr::core::recti(irr::core::vector2di(l_cScreen.Width - l_cTouchSize.Width, (l_cScreen.Height - l_cTouchSize.Height) / 2), l_cTouchSize);
        l_cTouchRect  = irr::core::recti(irr::core::vector2di(2 * l_cScreen.Width / 3, l_iHeightOffset), irr::core::dimension2du(l_cScreen.Width / 3, l_cScreen.Height - l_iHeightOffset));

        m_aControls[(int)enControls::Right] = new STouchInput("data/images/ctrl_right_off.png", "data/images/ctrl_right.png", l_cScreenRect, l_cTouchRect, m_pDrv);

        l_cScreenRect = irr::core::recti(irr::core::vector2di(0, 0), l_cTouchSize);
        m_aControls[(int)enControls::Rearview] = new STouchInput("data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png", l_cScreenRect, l_cScreenRect, m_pDrv);

        l_cScreenRect = irr::core::recti(irr::core::vector2di(l_cScreen.Width - l_cTouchSize.Width, 0), l_cTouchSize);
        m_aControls[(int)enControls::Respawn] = new STouchInput("data/images/ctrl_respawn_off.png", "data/images/ctrl_respawn.png", l_cScreenRect, l_cScreenRect, m_pDrv);
      }
    }

    CControllerGame_Touch::~CControllerGame_Touch() {
      for (int i = 0; i < (int)enControls::Count; i++)
        if (m_aControls[i] != nullptr)
          delete m_aControls[i];
    }

    /**
    * Create the throttle controls
    * @param a_bLeft throttle controls on the left?
    */
    void CControllerGame_Touch::createThrottleControls(bool a_bLeft) {
      irr::core::dimension2du l_cTouchSize = irr::core::dimension2du(
        m_cViewport.getSize().Width / 16,
        m_cViewport.getSize().Width / 16
      );

      irr::core::vector2di l_cPos = irr::core::vector2di(
        a_bLeft ? l_cTouchSize.Width / 2 : m_cViewport.getWidth() - l_cTouchSize.Width - l_cTouchSize.Width / 2,
        m_cViewport.getHeight() - 4 * l_cTouchSize.Height - l_cTouchSize.Height / 2
      );

      m_aControls[(int)enControls::Respawn   ] = new STouchInput("data/images/ctrl_respawn_off.png"   , "data/images/ctrl_respawn.png"   , irr::core::recti(irr::core::vector2di(0, 0), l_cTouchSize), irr::core::recti(irr::core::vector2di(0, 0), l_cTouchSize), m_pDrv);

      m_aControls[(int)enControls::Forward   ] = new STouchInput("data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png", irr::core::recti(l_cPos, l_cTouchSize), irr::core::recti(l_cPos, l_cTouchSize), m_pDrv); l_cPos.Y += l_cTouchSize.Height;
      m_aControls[(int)enControls::NoThrottle] = new STouchInput("data/images/ctrl_none_off.png"      , "data/images/ctrl_none.png"      , irr::core::recti(l_cPos, l_cTouchSize), irr::core::recti(l_cPos, l_cTouchSize), m_pDrv); l_cPos.Y += l_cTouchSize.Height;
      m_aControls[(int)enControls::Backward  ] = new STouchInput("data/images/ctrl_back_off.png"      , "data/images/ctrl_back.png"      , irr::core::recti(l_cPos, l_cTouchSize), irr::core::recti(l_cPos, l_cTouchSize), m_pDrv); l_cPos.Y += l_cTouchSize.Height;
      m_aControls[(int)enControls::Brake     ] = new STouchInput("data/images/ctrl_brake_off.png"     , "data/images/ctrl_brake.png"     , irr::core::recti(l_cPos, l_cTouchSize), irr::core::recti(l_cPos, l_cTouchSize), m_pDrv);

      enControls l_aAdjust[] = {
        enControls::Forward,
        enControls::NoThrottle,
        enControls::Backward,
        enControls::Brake
      };

      if (a_bLeft) {
        for (enControls l_cCtrl: l_aAdjust) {
          m_aControls[(int)l_cCtrl]->m_cTouch.UpperLeftCorner .X  = 0;
          m_aControls[(int)l_cCtrl]->m_cTouch.LowerRightCorner.X += 2 * l_cTouchSize.Width;
        }
      }
      else {
        for (enControls l_cCtrl: l_aAdjust) {
          m_aControls[(int)l_cCtrl]->m_cTouch.UpperLeftCorner .X -= 2 * l_cTouchSize.Width;
          m_aControls[(int)l_cCtrl]->m_cTouch.LowerRightCorner.X  = m_cViewport.getWidth();
        }
      }

      m_aControls[(int)enControls::Forward]->m_cTouch.UpperLeftCorner .Y = 2 * l_cTouchSize.Height;
      m_aControls[(int)enControls::Brake  ]->m_cTouch.LowerRightCorner.Y = m_cViewport.getHeight();
    }

    /**
    * Create the steering controls
    * @param a_bLeft steering controls on the left?
    */
    void CControllerGame_Touch::createSteeringControls(bool a_bLeft) {
      irr::core::dimension2du l_cTouchSize = irr::core::dimension2du(
        m_cViewport.getSize().Width / 16,
        m_cViewport.getSize().Width / 16
      );

      irr::core::vector2di l_cPos = irr::core::vector2di(
        a_bLeft ? l_cTouchSize.Width / 2 : m_cViewport.getWidth() - 3 * l_cTouchSize.Width - l_cTouchSize.Width / 2,
        m_cViewport.getHeight() - l_cTouchSize.Height - l_cTouchSize.Height / 2
      );

      irr::core::recti l_cScreen = irr::core::recti(
        irr::core::vector2di(a_bLeft ? 0 : m_cViewport.getWidth() - 3 * l_cTouchSize.Width, m_cViewport.getHeight() - l_cTouchSize.Height),
        l_cTouchSize
      );

      irr::core::recti l_cTouch = irr::core::recti(
        irr::core::vector2di(a_bLeft ? 0 : 2 * m_cViewport.getWidth() / 3, 2 * m_cViewport.getHeight() / 3),
        irr::core::dimension2du(a_bLeft ? l_cTouchSize.Width : m_cViewport.getWidth() / 3 - 2 * l_cTouchSize.Width, m_cViewport.getHeight() / 3)
      );

      m_aControls[(int)enControls::Left   ] = new STouchInput("data/images/ctrl_left_off.png" , "data/images/ctrl_left.png" , l_cScreen, l_cTouch, m_pDrv);

      l_cScreen = irr::core::recti(
        irr::core::vector2di(a_bLeft ? l_cTouchSize.Width : m_cViewport.getWidth() - 2 * l_cTouchSize.Width, m_cViewport.getHeight() - l_cTouchSize.Height),
        l_cTouchSize
      );

      l_cTouch = irr::core::recti(
        irr::core::vector2di(l_cScreen.UpperLeftCorner.X, m_cViewport.getHeight() / 3),
        irr::core::dimension2du(l_cTouchSize.Width, m_cViewport.getHeight() / 3)
      );

      m_aControls[(int)enControls::NoSteer] = new STouchInput("data/images/ctrl_none_off.png" , "data/images/ctrl_none.png" , l_cScreen, l_cTouch, m_pDrv);

      l_cScreen = irr::core::recti(
        irr::core::vector2di(a_bLeft ? 2 * l_cTouchSize.Width : m_cViewport.getWidth() - l_cTouchSize.Width, m_cViewport.getHeight() - l_cTouchSize.Height),
        l_cTouchSize
      );

      l_cTouch = irr::core::recti(
        irr::core::vector2di(l_cScreen.UpperLeftCorner.X, m_cViewport.getHeight() / 3),
        irr::core::dimension2du(a_bLeft ? m_cViewport.getWidth() - 2 * l_cTouchSize.Width : l_cTouchSize.Width, l_cTouchSize.Height)
      );

      m_aControls[(int)enControls::Right  ] = new STouchInput("data/images/ctrl_right_off.png", "data/images/ctrl_right.png", l_cScreen, l_cTouch, m_pDrv);

      m_aControls[(int)enControls::Rearview] = new STouchInput("data/images/ctrl_rearview_off.png", "data/images/ctrl_rearview.png", irr::core::recti(irr::core::vector2di(a_bLeft ? 0 : m_cViewport.getWidth() - l_cTouchSize.Width, 0), l_cTouchSize), irr::core::recti(irr::core::vector2di(a_bLeft ? 0 : m_cViewport.getWidth() - l_cTouchSize.Width, 0), l_cTouchSize), m_pDrv);

      enControls l_aAdjust[] = {
        enControls::Left,
        enControls::NoSteer,
        enControls::Right
      };

      for (enControls l_eCtrl: l_aAdjust) {
        m_aControls[(int)l_eCtrl]->m_cTouch.UpperLeftCorner .Y -= 3 * l_cTouchSize.Height;
        m_aControls[(int)l_eCtrl]->m_cTouch.LowerRightCorner.Y  = m_cViewport.getHeight();
      }

      if (a_bLeft) {
        m_aControls[(int)enControls::Right]->m_cTouch.LowerRightCorner.X = m_cViewport.getWidth() / 2;
      }
      else {
        m_aControls[(int)enControls::Left]->m_cTouch.UpperLeftCorner.X = m_cViewport.getWidth() / 2;
      }
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Touch::getThrottle() {
      irr::f32 l_fThrottle = 0.0f;

      if (m_aControls[(int)enControls::Forward ] != nullptr && m_aControls[(int)enControls::Forward ]->m_bActive) l_fThrottle += 1.0f;

      if (
        (m_aControls[(int)enControls::Backward] != nullptr && m_aControls[(int)enControls::Backward]->m_bActive) || 
        (m_aControls[(int)enControls::Brake   ] != nullptr && m_aControls[(int)enControls::Brake   ]->m_bActive)
      ) 
        l_fThrottle -= 1.0f;

      return l_fThrottle;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Touch::getSteer() {
      irr::f32 l_fSteer = 0.0f;

      if (m_aControls[(int)enControls::Left ] != nullptr && m_aControls[(int)enControls::Left ]->m_bActive) l_fSteer -= 1.0f;
      if (m_aControls[(int)enControls::Right] != nullptr && m_aControls[(int)enControls::Right]->m_bActive) l_fSteer += 1.0f;

      return l_fSteer;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getBrake() {
      return m_aControls[(int)enControls::Brake] != nullptr ? m_aControls[(int)enControls::Brake]->m_bActive : false;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRearView() {
      return m_aControls[(int)enControls::Rearview] != nullptr ? m_aControls[(int)enControls::Rearview]->m_bActive : false;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRespawn() {
      return m_aControls[(int)enControls::Respawn] != nullptr ? m_aControls[(int)enControls::Respawn]->m_bActive : false;
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
    * Process Irrlicht evnts to update the controls
    * @param a_cEvent event to process
    */
    void CControllerGame_Touch::updateControls(const irr::SEvent& a_cEvent) {
      OnEvent(a_cEvent);
    }

    /**
    * Implementation of the serialization method which does nothing in this case
    */
    void CControllerGame_Touch::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
    }

    /**
    * Implementation of the deserialization method which does nothing in this case
    */
    void CControllerGame_Touch::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CControllerGame_Touch::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

#ifdef _TOUCH_CONTROL
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == -1) {
              m_aTouch[i].m_iIndex = a_cEvent.TouchInput.ID;

              irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

              for (int j = 0; j < (int)enControls::Count; j++) {
                if (m_aControls[j] != nullptr && m_aControls[j]->m_cTouch.isPointInside(l_cPos)) {
                  m_aControls[j]->m_bActive = true;
                  m_aTouch   [i].m_eControl = (enControls)j;

                  if (m_aControls[j] != nullptr) {
                    m_aControls[j]->m_bActive = true;
                  }

                  break;
                }
              }

              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == a_cEvent.TouchInput.ID) {
              if (m_aTouch[i].m_eControl != enControls::Count && m_aControls[(int)m_aTouch[i].m_eControl] != nullptr) {
                m_aControls[(int)m_aTouch[i].m_eControl]->m_bActive = false;
              }

              m_aTouch[i].m_iIndex   = -1;
              m_aTouch[i].m_eControl = enControls::Count;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == a_cEvent.TouchInput.ID) {
              enControls l_eCtrl = enControls::Count;

              irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

              for (int j = 0; j < (int)enControls::Count; j++) {
                if (m_aControls[j] != nullptr && m_aControls[j]->m_cTouch.isPointInside(l_cPos)) {
                  l_eCtrl = (enControls)j;
                }
              }

              if (l_eCtrl != m_aTouch[i].m_eControl) {
                if (m_aTouch[i].m_eControl != enControls::Count && m_aControls[(int)m_aTouch[i].m_eControl] != nullptr)
                  m_aControls[(int)m_aTouch[i].m_eControl]->m_bActive = false;

                if (l_eCtrl != enControls::Count && m_aControls[(int)l_eCtrl] != nullptr)
                  m_aControls[(int)l_eCtrl]->m_bActive = true;
              }

              m_aTouch[i].m_eControl = l_eCtrl;

              break;
            }
          }
        }
      }
#else
      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        std::vector<std::tuple<enControls, irr::EKEY_CODE>> a_vControls = {
          std::make_tuple(enControls::Left    , irr::KEY_LEFT),
          std::make_tuple(enControls::Right   , irr::KEY_RIGHT),
          std::make_tuple(enControls::Forward , irr::KEY_UP),
          std::make_tuple(enControls::Backward, irr::KEY_DOWN),
          std::make_tuple(enControls::Brake   , irr::KEY_SPACE),
          std::make_tuple(enControls::Rearview, irr::KEY_TAB),
          std::make_tuple(enControls::Respawn , irr::KEY_RETURN)
        };

        for (auto& l_cCtrl : a_vControls) {
          if (m_aControls[(int)std::get<0>(l_cCtrl)] != nullptr) {
            if (a_cEvent.KeyInput.Key == std::get<1>(l_cCtrl)) {
              m_aControls[(int)std::get<0>(l_cCtrl)]->m_bActive = a_cEvent.KeyInput.PressedDown;
            }
          }
        }
      }

#endif

      return l_bRet;
    }

    void CControllerGame_Touch::draw()  {
      for (auto& l_pControl : m_aControls) {
        if (l_pControl != nullptr)
          l_pControl->draw(m_cViewport);
      }
    }

    CControllerGame_Touch::STouchInput::STouchInput() :
      m_pDrv   (nullptr),
      m_pOff   (nullptr),
      m_pOn    (nullptr),
      m_bActive(false  )
    {
    }

    CControllerGame_Touch::STouchInput::STouchInput(const std::string& a_sPathOff, const std::string& a_sPathOn, const irr::core::recti& a_cScreen, const irr::core::recti &a_cTouch, irr::video::IVideoDriver* a_pDrv) :
      m_pDrv   (a_pDrv),
      m_pOff   (nullptr),
      m_pOn    (nullptr),
      m_bActive(false)
    {
      m_pOff = m_pDrv->getTexture(a_sPathOff.c_str());
      m_pOn  = m_pDrv->getTexture(a_sPathOn .c_str());

      m_cTouch = a_cTouch;
      
      irr::s32 l_iOffset = a_cScreen.getWidth() / 16;   // The rect to draw the icon will be offset a little

      m_cScreen = irr::core::recti(a_cScreen.UpperLeftCorner + irr::core::vector2di(l_iOffset), a_cScreen.LowerRightCorner - irr::core::vector2di(l_iOffset));

      if (m_pOff != nullptr) m_cSourceOff = irr::core::recti(irr::core::position2di(), m_pOff->getSize());
      if (m_pOn  != nullptr) m_cSourceOn  = irr::core::recti(irr::core::position2di(), m_pOn ->getSize());
    }

    void CControllerGame_Touch::STouchInput::draw(const irr::core::recti &a_cClip) {
      if (m_bActive) {
        if (m_pOn != nullptr)
          m_pDrv->draw2DImage(m_pOn, m_cScreen, m_cSourceOn, &a_cClip, nullptr, true);
      }
      else {
        if (m_pOff != nullptr)
          m_pDrv->draw2DImage(m_pOff, m_cScreen, m_cSourceOff, &a_cClip, nullptr, true);
      }
    }

    CControllerGame_Touch::STouchState::STouchState() : m_iIndex(-1), m_eControl(enControls::Count) {
    }
  }
}