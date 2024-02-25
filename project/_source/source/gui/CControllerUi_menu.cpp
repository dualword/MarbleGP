// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <controller/CControllerMenu.h>
#include <controller/CControllerBase.h>
#include <gui/CControllerUi_Menu.h>

namespace dustbin {
  namespace gui {
    CControllerUi_Menu::CControllerUi_Menu(irr::gui::IGUIElement* a_pParent) : CControllerUi(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiMenuId) {
    }

    CControllerUi_Menu::~CControllerUi_Menu() {
    }

    /**
    * Set the controller which is to be configured
    * @param a_sCtrl the controller string the controller is constructed from
    */
    void CControllerUi_Menu::setController(const std::string& a_sCtrl) {
      CControllerUi::setController(a_sCtrl);

      controller::CControllerMenu l_pCtrl = controller::CControllerMenu();
      
      if (!m_pController->compareTo(&l_pCtrl)) {
        m_pController->getInputs().clear();
        m_pController->deserialize(l_pCtrl.serialize());
      }

      calculateGui();
    }

    /**
    * The child classes nees to implement this method for
    * creating default controls if the controller type
    * is changed
    * @param a_eCtrl the new controller type
    */
    void CControllerUi_Menu::resetToDefaultForMode(enControl a_eCtrl) {
      if (a_eCtrl == enControl::Keyboard) {
        m_pController->deserialize("DustbinController;control;f%3bl%3bUp%3bh%3ba%3bn%3bM%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3ba;control;f%3bl%3bDown%3bh%3ba%3bn%3bO%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bqze;control;f%3bl%3bLeft%3bh%3ba%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3b4Lg;control;f%3bl%3bRight%3bh%3ba%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bOmc;control;f%3bl%3bEnter%3bh%3ba%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp;control;f%3bl%3bOk%3bh%3ba%3bn%3bn%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp;control;f%3bl%3bCancel%3bh%3ba%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp");
      }
      else if (a_eCtrl == enControl::Joystick) {
        m_pController->deserialize("DustbinController;control;f%3bl%3bUp%3bh%3bd%3bn%3bM%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3ba;control;f%3bl%3bDown%3bh%3bd%3bn%3bO%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bqze;control;f%3bl%3bLeft%3bh%3bd%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3b4Lg;control;f%3bl%3bRight%3bh%3bd%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3bOmc;control;f%3bl%3bEnter%3bh%3bc%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp;control;f%3bl%3bOk%3bh%3bc%3bn%3bn%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp;control;f%3bl%3bCancel%3bh%3bc%3bn%3bB%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb%3bF%3b%3d%3dp");
      }
    }
  }
}