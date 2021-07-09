// (w) 2021 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu() {
      for (unsigned i = 0; i < 6; i++)
        m_aCtrlReady[i] = true;

      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP   ; l_cInput.m_sName = "Up"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN ; l_cInput.m_sName = "Down"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT ; l_cInput.m_sName = "Left"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT; l_cInput.m_sName = "Right" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE; l_cInput.m_sName = "Enter" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_TAB  ; l_cInput.m_sName = "Cancel"; m_vControls.push_back(l_cInput);
    }

    CControllerMenu::~CControllerMenu() {
    }

    bool CControllerMenu::wasButtonPressed(unsigned a_iIndex) {
      irr::f32 l_fValue = m_vControls[a_iIndex].m_fValue;

      if (!m_aCtrlReady[a_iIndex]) {
        if (l_fValue < 0.5f) m_aCtrlReady[a_iIndex] = true;
        return false;
      }
      else {
        if (l_fValue > 0.5f) {
          m_aCtrlReady[a_iIndex] = false;
          return true;
        }
        else return false;
      }
    }
    
    /**
    * Is a "move up" requested?
    * @return a boolean
    */
    bool CControllerMenu::moveUp() {
      return wasButtonPressed(0);
    }

    /**
    * Is a "move down" requested?
    * @return a boolean
    */
    bool CControllerMenu::moveDown() {
      return wasButtonPressed(1);
    }

    /**
    * Is a "move left" requested?
    * @return a boolean
    */
    bool CControllerMenu::moveLeft() {
      return wasButtonPressed(2);
    }

    /**
    * Is a "move right" requested?
    * @return a boolean
    */
    bool CControllerMenu::moveRight() {
      return wasButtonPressed(3);
    }

    /**
    * Was the "OK / Enter" button pressed?
    * @return a boolean
    */
    bool CControllerMenu::enterPressed() {
      return wasButtonPressed(4);
    }

    /**
    * Was the "Cancel" button pressed?
    */
    bool CControllerMenu::cancelPressed() {
      return wasButtonPressed(5);
    }

  } // namespace controller
} // namespace dustbin