// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/IJoystickEventHandler.h>
#include <controller/CControllerBase.h>
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
    const int g_ControllerUiId = MAKE_IRR_ID('d', 'g', 'c', 'u');
    const irr::c8 g_ControllerUiName[] = "ControllerUi";

    class CDustbinScrollPane; /**< Forward declaration of the scroll pane */

    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public gui::CMenuBackground, public controller::CControllerBase, public controller::IJoystickEventHandler {
      private:
        irr::gui::ICursorControl *m_pCursor;
        irr::gui::IGUIFont       *m_pFont;
        irr::gui::IGUIFont       *m_pSmall;
        menu::IMenuManager       *m_pMenuMgr;
        std::string               m_sSelected;    /**< The selected controller type */
        std::string               m_sConfigData;  /**< The serialized controller config string */
        irr::core::recti          m_cDraw;        /**< Draw rect for the image */
        irr::s32                  m_iFontHeight;  /**< Height of the font */
        irr::SEvent               m_cOld;         /**< The old event, we want the release events to set the control */
        bool                      m_bJoyOld;      /**< Is the old event a joystick input event? */
        irr::video::ITexture     *m_pBackground;  /**< The background image */
        irr::core::recti          m_cSource;      /**< Source rectangle for the background image */

        std::map<std::string, std::tuple<irr::core::recti, std::wstring, bool, bool, bool>> m_mLabels;    /**< Map with the control labels (Key == Name of the control, Value = tuple(0 == rect of the label, 1 == text of the label 2 == hovered, 3 == clicked, 4 == selected) */


        /**
        * Get a readable string of the set controls
        * @return a readable string of the set controls
        */
        std::wstring getControlText(CControllerBase::SCtrlInput *a_pCtrl);

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent);
        ~CControllerUi();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        void buildUi(irr::gui::IGUIElement *a_pParent);

        virtual bool update(const irr::SEvent& a_cEvent) override;

        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get the configuration of the controller as XML string
        * @return the configuration of the controller
        */
        std::string getControllerString();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType() override;

        /**
        * The "setText" method is abused to pass the configuration XML string to the UI
        * @param a_pText the new XML string
        */
        virtual void setText(const wchar_t* a_pText) override;

        /**
        * Change the font for the configuration dialog
        */
        void setFont(irr::gui::IGUIFont* a_pFont);

        /**
        * Is currently editing an item active? This will block the menu controller
        */
        bool isEditing();

        void setMenuManager(menu::IMenuManager *a_pMenuManager);

        virtual bool OnJoystickEvent(const irr::SEvent& a_cEvent) override;

        virtual void draw() override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin