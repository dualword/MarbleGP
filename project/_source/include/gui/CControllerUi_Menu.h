// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
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
      public:
        CControllerUi_Menu(irr::gui::IGUIElement* a_pParent);
        virtual ~CControllerUi_Menu();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        virtual void buildUi(irr::gui::IGUIElement *a_pParent) override;

        virtual irr::gui::EGUI_ELEMENT_TYPE getType() override;
    };
  }
}