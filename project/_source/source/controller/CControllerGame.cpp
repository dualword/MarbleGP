// (w) 2021 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame.h>

namespace dustbin {
  namespace controller {

    CControllerGame::CControllerGame() {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP    ; l_cInput.m_sName = "Forward" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN  ; l_cInput.m_sName = "Backward"; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT  ; l_cInput.m_sName = "Left"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT ; l_cInput.m_sName = "Right"   ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE ; l_cInput.m_sName = "Brake"   ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_TAB   ; l_cInput.m_sName = "Rearview"; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RETURN; l_cInput.m_sName = "Respawn" ; m_vControls.push_back(l_cInput);
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

  } // namespace controller
} // namespace dustbin