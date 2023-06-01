// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/touch/CControllerTouchSteerOnly.h>
#include <controller/touch/CControllerTouchGyro.h>
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
      irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
    ),
      m_pController(nullptr)
    {
      irr::core::recti l_cViewport = irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize());

      switch (a_eType) {
        case IControllerGame::enType::Touch:
          m_pController = new CControllerTouchSteerOnly(CGlobal::getInstance()->getVideoDriver(), l_cViewport);
          break;

        case IControllerGame::enType::Gyroscope:
          m_pController = new CControllerTouchGyro(CGlobal::getInstance()->getVideoDriver(), l_cViewport);
          break;

        default:
          // No touch controller specified, we do nothing here
          break;
      }
    }

    CControllerGame_Touch::~CControllerGame_Touch() {
    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame_Touch::getThrottle() {
      if (m_pController != nullptr)
        return m_pController->getThrottle();
      else
        return 0.0f;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame_Touch::getSteer() {
      if (m_pController != nullptr)
        return m_pController->getSteer();
      else
        return 0.0f;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getBrake() {
      if (m_pController != nullptr)
        return m_pController->getBrake();
      else
        return false;
    }

    /**
    * Return the rearview control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRearView() {
      if (m_pController != nullptr)
        return m_pController->getRearView();
      else
        return false;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame_Touch::getRespawn() {
      if (m_pController != nullptr)
        return m_pController->getRespawn();
      else
        return false;
    }

    /**
    * Get the "withdraw from race" button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::withdrawFromRace() {
      if (m_pController != nullptr)
        return m_pController->withdrawFromRace();
      else
        return false;
    }

    /**
    * Get the pause button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame_Touch::pause() {
      if (m_pController != nullptr)
        return m_pController->pause();
      else
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
     * If this controller has an UI this method will move it to the front.
     * The Android touch and gyroscope controllers have an UI
     */
    void CControllerGame_Touch::moveGuiToFront() {
      CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement()->bringToFront(this);
    }

    /**
    * Get the text shown in the tutorial
    * @param a_bFirst true if this is the first help point (controls), false if it's the fourth (respawn)
    * @return the text shown in the tutorial
    */
    std::wstring CControllerGame_Touch::getTutorialText(bool a_bFirst) {
      return m_pController != nullptr ? m_pController->getTutorialText(a_bFirst) : L"";
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CControllerGame_Touch::OnEvent(const irr::SEvent &a_cEvent) {
      if (m_pController != nullptr)
        return m_pController->handleEvent(a_cEvent);
      else
        return false;
    }

    void CControllerGame_Touch::draw() {
      if (IsVisible && m_pController != nullptr) {
        m_pController->draw();
      }
    }
  }
}