// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame.h>
#include <controller/IController.h>
#include <helpers/CStringHelpers.h>

namespace dustbin {
  namespace controller {
    CControllerGame::CControllerGame() : IControllerGame(IControllerGame::enType::KeyJoy), m_bWithdrawDown(false), m_bPauseDown(false) {
      setupControls();
    }

    void CControllerGame::setupControls() {
      SCtrlInput l_cInput;

#ifdef _ANDROID
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 1;                             l_cInput.m_sName = "Forward"    ; m_vControls.push_back(l_cInput);  // 0 == forward
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 2;                             l_cInput.m_sName = "Backward"   ; m_vControls.push_back(l_cInput);  // 1 == backward
      l_cInput.m_eType = enInputType::JoyAxis  ; l_cInput.m_iAxis   = 0; l_cInput.m_iDirection = -1; l_cInput.m_sName = "Left"       ; m_vControls.push_back(l_cInput);  // 2 == left
      l_cInput.m_eType = enInputType::JoyAxis  ; l_cInput.m_iAxis   = 0; l_cInput.m_iDirection =  1; l_cInput.m_sName = "Right"      ; m_vControls.push_back(l_cInput);  // 3 == right
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 0;                             l_cInput.m_sName = "Brake"      ; m_vControls.push_back(l_cInput);  // 4 == brake
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 4;                             l_cInput.m_sName = "Rearview"   ; m_vControls.push_back(l_cInput);  // 5 == rearview
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 5;                             l_cInput.m_sName = "Respawn"    ; m_vControls.push_back(l_cInput);  // 6 == respawn
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 7;                             l_cInput.m_sName = "Pause"      ; m_vControls.push_back(l_cInput);  // 7 == pause
      l_cInput.m_eType = enInputType::JoyButton; l_cInput.m_iButton = 6;                             l_cInput.m_sName = "Cancel Race"; m_vControls.push_back(l_cInput);  // 8 == cancel race
#else
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP    ; l_cInput.m_sName = "Forward"    ; m_vControls.push_back(l_cInput);  // 0 == forward
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN  ; l_cInput.m_sName = "Backward"   ; m_vControls.push_back(l_cInput);  // 1 == backward
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT  ; l_cInput.m_sName = "Left"       ; m_vControls.push_back(l_cInput);  // 2 == left
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT ; l_cInput.m_sName = "Right"      ; m_vControls.push_back(l_cInput);  // 3 == right
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE ; l_cInput.m_sName = "Brake"      ; m_vControls.push_back(l_cInput);  // 4 == brake
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_TAB   ; l_cInput.m_sName = "Rearview"   ; m_vControls.push_back(l_cInput);  // 5 == rearview
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RETURN; l_cInput.m_sName = "Respawn"    ; m_vControls.push_back(l_cInput);  // 6 == respawn
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_PAUSE ; l_cInput.m_sName = "Pause"      ; m_vControls.push_back(l_cInput);  // 7 == pause
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_ESCAPE; l_cInput.m_sName = "Cancel Race"; m_vControls.push_back(l_cInput);  // 8 == cancel race
#endif
    }

    CControllerGame::~CControllerGame() {

    }

    /**
    * Returns the throttle control (-1 .. 1): forward - backward
    * @return the throttle state
    */
    irr::f32 CControllerGame::getThrottle() {
      return m_vControls[0].m_fValue - m_vControls[1].m_fValue;
    }

    /**
    * Returns the steer (-1 .. 1): right - left
    * @return the steer state
    */
    irr::f32 CControllerGame::getSteer() {
      irr::f32 l_fSteer = m_vControls[3].m_fValue - m_vControls[2].m_fValue;

      return abs(l_fSteer) > 0.1f ? l_fSteer : 0.0f;
    }

    /**
    * Returns the state of the brake control
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame::getBrake() {
      return m_vControls[4].m_fValue >= 0.5f;
    }

    /**
     * Return the rearview control
     * @return true if control >= 0.5, false otherwise
     */
    bool CControllerGame::getRearView() {
      return m_vControls[5].m_fValue >= 0.5f;
    }

    /**
    * Get the respawn control state
    * @return true if control >= 0.5, false otherwise
    */
    bool CControllerGame::getRespawn() {
      return m_vControls[6].m_fValue >= 0.5f;
    }

    /**
    * Get the cancel button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame::withdrawFromRace() {
      if (!m_bWithdrawDown) {
        if (m_vControls[8].m_fValue >= 0.5f) {
          m_bWithdrawDown = true;
          return true;
        }
      }
      else {
        if (m_vControls[8].m_fValue < 0.5f) {
          m_bWithdrawDown = false;
        }
      }

      return false;
    }

    /**
    * Get the pause button state
    * @return true if the button was pressed since the last query, false otherwise
    */
    bool CControllerGame::pause() {
      if (!m_bPauseDown) {
        if (m_vControls[7].m_fValue >= 0.5f) {
          m_bPauseDown = true;
          return true;
        }
      }
      else {
        if (m_vControls[7].m_fValue < 0.5f) {
          m_bPauseDown = false;
        }
      }

      return false;
    }


    /**
    * Process Irrlicht evnts to update the controls
    * @param a_cEvent event to process
    */
    void CControllerGame::updateControls(const irr::SEvent& a_cEvent) {
      // In this case just pass the event to the base class
      CControllerBase::update(a_cEvent);
    }

    /**
    * The player has finished, hide the UI elements if necessary
    */
    void CControllerGame::playerFinished() {
      // Nothing to do in this case
    }

    /**
     * If this controller has an UI this method will move it to the front.
     * The Android touch and gyroscope controllers have an UI
     */
    void CControllerGame::moveGuiToFront() {
      // Nothing to do here
    }

    /**
    * Get the text shown in the tutorial
    * @param a_bFirst true if this is the first help point (controls), false if it's the fourth (respawn)
    * @return the text shown in the tutorial
    */
    std::wstring CControllerGame::getTutorialText(bool a_bFirst) {
      std::wstring s = L"";

      if (a_bFirst) {
        s += L"Steer the marble left and right using " + createDoubleControlString(m_vControls[2], m_vControls[3]) + L", ";
        s += L"accelerate the marble forward and backward using " + createDoubleControlString(m_vControls[0], m_vControls[1]) + L", ";
        s += L"the brake is activated using " + helpers::s2ws(m_vControls[4].toString()) + L".\n";
      }
      else {
        s = L"You can manually respawn the marble by pressing " + createSingleControlString(m_vControls[6]) + L" for two seconds.\n";
      }

      return s;
    }


    /**
    * Create the control string for the hints for a single control
    * @param a_cCtrl the control
    * @return a string with the description of the control
    */
    std::wstring CControllerGame::createSingleControlString(SCtrlInput& a_cCtrl) {
      std::wstring s = L"";

      switch (a_cCtrl.m_eType) {
        case enInputType::Key:
          s = L"Key \"" + helpers::keyCodeToString(a_cCtrl.m_eKey) + L"\"";
          break;

        case enInputType::JoyAxis:
          s = L"Gamepad axis " + std::to_wstring(a_cCtrl.m_iAxis) + (a_cCtrl.m_iDirection > 0 ? L"+" : L"-");
          break;

        case enInputType::JoyButton:
          s = L"Gamepad button " + std::to_wstring(a_cCtrl.m_iButton);
          break;

        case enInputType::JoyPov:
          s = L"Gamepad POV ";
          switch (a_cCtrl.m_iPov) {
            case     0: s += L"Up"   ; break;
            case 18000: s += L"Down" ; break;
            case  9000: s += L"Left" ; break;
            case 27000: s += L"Right"; break;
            default:
              s += std::to_wstring(a_cCtrl.m_iPov);
              break;
          }
          break;
      }

      return s;
    }

    /**
    * Create the control string for the hints for two linked controls
    * @param a_cCtrl1 the first control
    * @param a_cCtrl2 the second control
    * @return a string with the description of the controls
    */
    std::wstring CControllerGame::createDoubleControlString(SCtrlInput& a_cCtrl1, SCtrlInput& a_cCtrl2) {
      std::wstring s = L"";
      if (a_cCtrl1.m_eType != a_cCtrl2.m_eType)
        s = createSingleControlString(a_cCtrl1) + L" and " + createSingleControlString(a_cCtrl2);
      else {
        switch (a_cCtrl1.m_eType) {
          case enInputType::Key:
            s = L"Keys \"" + helpers::keyCodeToString(a_cCtrl1.m_eKey) + L" and " + helpers::keyCodeToString(a_cCtrl2.m_eKey);
            break;

          case enInputType::JoyAxis:
            if (a_cCtrl1.m_iAxis == a_cCtrl2.m_iAxis) {
              s = L"Gamepad Axis " + std::to_wstring(a_cCtrl1.m_iAxis);
            }
            else {
              s = createSingleControlString(a_cCtrl1) + L" and " + createSingleControlString(a_cCtrl2);
            }
            break;

          case enInputType::JoyPov:
            s = L"Gamepad POV ";
            switch (a_cCtrl1.m_iPov) {
              case     0: s += L"Up"   ; break;
              case  9000: s += L"Right"; break;
              case 18000: s += L"Down" ; break;
              case 27000: s += L"Left" ; break;
              default:
                s += std::to_wstring(a_cCtrl1.m_iPov);
                break;
            }

            s += L" and ";
            switch (a_cCtrl2.m_iPov) {
              case     0: s += L"Up"   ; break;
              case  9000: s += L"Right"; break;
              case 18000: s += L"Down" ; break;
              case 27000: s += L"Left" ; break;
              default:
                s += std::to_wstring(a_cCtrl2.m_iPov);
                break;
            }
            break;
        }
      }
      
      return s;
    }

  } // namespace controller
} // namespace dustbin