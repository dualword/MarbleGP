// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CControllerUi.h>

namespace dustbin {
  namespace gui {
    const int g_ControllerUiGameId = MAKE_IRR_ID('d', 'g', 'c', 'g');
    const irr::c8 g_ControllerUiGameName[] = "ControllerUiGame";

    /**
    * @class CControllerUi_Game
    * @author Christian Keimel
    * This class extends the controller UI for configuration of the game controls
    */
    class CControllerUi_Game : public CControllerUi {
      protected:

        /**
        * The child classes nees to implement this method for
        * creating default controls if the controller type
        * is changed
        * @param a_eCtrl the new controller type
        */
        virtual void resetToDefaultForMode(enControl a_eCtrl) override;

      public:
        CControllerUi_Game(irr::gui::IGUIElement* a_pParent);
        virtual ~CControllerUi_Game();

        /**
        * Set the controller which is to be configured
        * @param a_sCtrl the controller string the controller is constructed from
        */
        void setController(const std::string& a_sCtrl);

        virtual void deserialize(const std::string a_sData);
    };
  }
}