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
        struct SJoystickState {
          std::string m_sName;  /**< Name of the joystick */

          int m_iIndex;   /**< The index in the "m_aJoysticks" array */
          int m_iAxes;    /**< The number of axes*/
          int m_iButtons; /**< The number of buttons */

          bool m_bInitialized;  /**< Set to "true" after the joystick got it's first update */
          bool m_bHasPov;       /**< Is a POV present? */

          std::vector<float> m_vAxes; /**< The values of the axes */
        };

        struct SCtrlUi {
          controller::CControllerBase::SCtrlInput *m_pInput;    /**< The controller input configured by this UI item */

          std::wstring m_sName;   /**< The display name of the item */
          std::wstring m_sValue;  /**< The current value of this item */

          irr::core::recti m_cRectLabel;  /**< The rectangle of this item */
          irr::core::recti m_cRectItem;  /**< The rectangle of this item */

          SCtrlUi(controller::CControllerBase::SCtrlInput* a_pInput, const std::wstring& a_sName, const std::wstring& a_sValue, const irr::core::recti& a_cRectLabel, const irr::core::recti &a_cRectItem) :
            m_pInput    (a_pInput),
            m_sName     (a_sName),
            m_sValue    (a_sValue),
            m_cRectLabel(a_cRectLabel),
            m_cRectItem (a_cRectItem)
          {
          }
        };

        bool m_bMouseDown;
        bool m_bSelected;   /**< Is this UI element selected via gamepad input? */

        irr::gui::IGUIElement *m_pParent;

        irr::core::position2di m_cMousePos;

        irr::core::recti m_cItemRect;   /**< The inner rect for the items */

        std::string m_sConfigData;
        std::string m_sHeadline;
        std::wstring m_sReturn;

        irr::gui::IGUIFont       *m_pFont;
        irr::video::IVideoDriver *m_pDrv;
        CGlobal                  *m_pGlobal;

        std::vector<SCtrlUi> m_vItems;

        std::vector<SCtrlUi>::iterator m_itHover;
        std::vector<SCtrlUi>::iterator m_itClick;
        std::vector<SCtrlUi>::iterator m_itSelct;

        std::vector<SJoystickState> m_vJoyStates;

        menu::IMenuManager *m_pMenuManager;

        CDustbinScrollPane *m_pScrollPane;    /**< The helper for scrolling */

        irr::gui::ICursorControl *m_pCursor;  /**< The cursor control */

        void updateConfigData();

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

        virtual void draw();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin