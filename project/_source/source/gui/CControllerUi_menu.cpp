// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

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
  }
}