// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame.h>

namespace dustbin {
  namespace controller {

    CControllerGame::CControllerGame() : m_bWithdrawDown(false), m_bPauseDown(false) {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP    ; l_cInput.m_sName = "Forward"    ; m_vControls.push_back(l_cInput);  // 0 == forward
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN  ; l_cInput.m_sName = "Backward"   ; m_vControls.push_back(l_cInput);  // 1 == backward
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT  ; l_cInput.m_sName = "Left"       ; m_vControls.push_back(l_cInput);  // 2 == left
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT ; l_cInput.m_sName = "Right"      ; m_vControls.push_back(l_cInput);  // 3 == right
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE ; l_cInput.m_sName = "Brake"      ; m_vControls.push_back(l_cInput);  // 4 == brake
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_TAB   ; l_cInput.m_sName = "Rearview"   ; m_vControls.push_back(l_cInput);  // 5 == rearview
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RETURN; l_cInput.m_sName = "Respawn"    ; m_vControls.push_back(l_cInput);  // 6 == respawn
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_PAUSE ; l_cInput.m_sName = "Pause"      ; m_vControls.push_back(l_cInput);  // 7 == pause
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_ESCAPE; l_cInput.m_sName = "Cancel Race"; m_vControls.push_back(l_cInput);  // 8 == cancel race
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
      return m_vControls[3].m_fValue - m_vControls[2].m_fValue;
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

  } // namespace controller
} // namespace dustbin