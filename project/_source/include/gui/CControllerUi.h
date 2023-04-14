// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/IJoystickEventHandler.h>
#include <controller/CControllerBase.h>
#include <gui/IGuiMoveOptionElement.h>
#include <gui/CMenuBackground.h>
#include <state/IState.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace menu {
    class IMenuManager;   /**< Forward declaration of the menu manager */
  }


  namespace gui {

    class CDustbinScrollPane;       /**< Forward declaration of the scroll pane */
    class IGuiControllerUiCallback; /**< Forward declaration of the callback */
    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public irr::gui::IGUIElement, public controller::IJoystickEventHandler {
      public:
        enum class enMode {
          Display,          /**< Just show the controls */
          Wizard,           /**< Control wizard to step through the control items and initialize them */
          Test              /**< Test the controls */
        };

      protected:
        struct SGuiElement {
          irr::core::vector2di m_cPosHead;  /**< The headline position */
          irr::core::vector2di m_cPosColn;  /**< The colon position */
          irr::core::vector2di m_cPosCtrl;  /**< The control text position */
          irr::core::vector2di m_cPosBack;  /**< Position of the background rectangle */

          irr::core::dimension2di m_cDimHead;   /**< Size of the headline */
          irr::core::dimension2di m_cDimColn;   /**< Size of the colon */

          SGuiElement();
        };

        controller::CControllerBase *m_pController;
        irr::video::IVideoDriver    *m_pDrv;
        irr::gui::IGUIFont          *m_pFont;
        enMode                       m_eMode;

        std::vector<SGuiElement> m_vGui;

        void calculateGui();

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType);
        ~CControllerUi();

        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get the configuration of the controller as XML string
        * @return the configuration of the controller
        */
        std::string getControllerString();

        /**
        * The "setText" method is abused to pass the configuration XML string to the UI
        * @param a_pText the new XML string
        */
        virtual void setText(const wchar_t* a_pText) override;

        /**
        * Set the controller which is to be configured
        * @param a_sCtrl the controller string the controller is constructed from
        */
        virtual void setController(const std::string &a_sCtrl);

        virtual bool OnJoystickEvent(const irr::SEvent& a_cEvent) override;

        virtual void draw() override;

        virtual std::string serialize();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin