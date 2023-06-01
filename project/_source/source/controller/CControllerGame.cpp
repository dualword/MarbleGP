// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame.h>
#include <controller/IController.h>
#include <helpers/CStringHelpers.h>

namespace dustbin {
  namespace controller {
    CControllerGame::CControllerGame() : IControllerGame(IControllerGame::enType::KeyJoy), m_bWithdrawDown(false), m_bPauseDown(false), m_bAutoThrottle(true) {
      setupControls();
    }

    CControllerGame::CControllerGame(bool a_bAutoThrottle) : IControllerGame(IControllerGame::enType::KeyJoy), m_bWithdrawDown(false), m_bPauseDown(false), m_bAutoThrottle(a_bAutoThrottle) {
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
      if (m_bAutoThrottle)
        return m_vControls[4].m_fValue > 0.5f ? -1.0f : 1.0f;
      else
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
        s += L"Steer the marble left and right using the following controls:\n";

        s += helpers::s2ws(m_vControls[2].toString()) + L"\n" + helpers::s2ws(m_vControls[3].toString()) + L".\n\n";

        if (m_bAutoThrottle) {
          s += L"The marble accelerated automatically unless you activate the brake using " + helpers::s2ws(m_vControls[4].toString()) + L". ";
        }
        else {
          s += L"Accelerate the marble forward using " + helpers::s2ws(m_vControls[0].toString()) + L" and backward with " + helpers::s2ws(m_vControls[1].toString()) + L".\n";
          s += L"The brake is activated using " + helpers::s2ws(m_vControls[4].toString()) + L". ";
        }
      }
      else {
        s = L"You can manually respawn the marble by pressing " + helpers::s2ws(m_vControls[6].toString()) + L" for two seconds.";
      }

      return s;
    }
  } // namespace controller
} // namespace dustbin