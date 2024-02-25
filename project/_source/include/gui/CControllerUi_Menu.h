// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CControllerUi.h>

namespace dustbin {
  namespace gui {
    const int g_ControllerUiMenuId = MAKE_IRR_ID('d', 'g', 'c', 'm');
    const irr::c8 g_ControllerUiMenuName[] = "ControllerUiMenu";

    /**
    * @class CControllerUi_Menu
    * @author Christian Keimel
    * This class extends the controller UI for configuration of the game controls
    */
    class CControllerUi_Menu : public CControllerUi {
      protected:

        /**
        * The child classes nees to implement this method for
        * creating default controls if the controller type
        * is changed
        * @param a_eCtrl the new controller type
        */
        virtual void resetToDefaultForMode(enControl a_eCtrl) override;

      public:
        CControllerUi_Menu(irr::gui::IGUIElement* a_pParent);
        virtual ~CControllerUi_Menu();

        /**
        * Set the controller which is to be configured
        * @param a_sCtrl the controller string the controller is constructed from
        */
        virtual void setController(const std::string &a_sCtrl) override;
    };
  }
}