// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/IJoystickEventHandler.h>
#include <controller/CControllerBase.h>
#include <gui/CMenuBackground.h>
#include <state/IState.h>
#include <string>
#include <map>

namespace dustbin {
  namespace gui {
    const int g_ControllerUiId = MAKE_IRR_ID('d', 'g', 'c', 'u');
    const irr::c8 g_ControllerUiName[] = "ControllerUi";

    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public gui::CMenuBackground, public controller::CControllerBase, public controller::IJoystickEventHandler {
      private:
        struct SJoystickState {
          std::string m_sName;  /**< Name of the joystick */

          int m_iIndex,   /**< The index in the "m_aJoysticks" array */
              m_iAxes,    /**< The number of axes*/
              m_iButtons; /**< The number of buttons */

          bool m_bInitialized,  /**< Set to "true" after the joystick got it's first update */
               m_bHasPov;       /**< Is a POV present? */

          std::vector<float> m_vAxes; /**< The values of the axes */
        };

        irr::gui::IGUIElement *m_pParent;

        irr::core::position2di m_cMousePos;

        std::string m_sConfigData;
        std::string m_sHeadline;
        std::wstring m_sReturn;

        irr::gui::IGUIElement *m_pSelectedOne,  /**< The first selected control static text */
                              *m_pSelectedTwo,  /**< The second selected control static text*/
                              *m_pHoveredOne,   /**< The first hovered control static text */
                              *m_pHoveredTwo;   /**< The second hovered control static text */

        bool m_bMouseDown;

        std::map<irr::gui::IGUIElement*, irr::gui::IGUIElement*> m_mTextPairs;  /**< A map that links the first and second control static texts */

        std::vector<irr::gui::IGUIElement*> m_vElements;  /**< A vector that contains all elements of the UI */

        std::map<irr::gui::IGUIElement*, std::vector<controller::CControllerBase::SCtrlInput>::iterator> m_mTextControls; /**< A map that links static texts to control items */

        std::map<std::vector<controller::CControllerBase::SCtrlInput>::iterator, irr::gui::IGUIStaticText*> m_mControlText; /**< The label for the controllers */

        irr::gui::IGUIFont* m_pFont;

        std::vector<controller::CControllerBase::SCtrlInput>::iterator m_itHovered,   /**< The hovered control */
                                                                       m_itSelected;  /**< The selected control */

        std::vector<SJoystickState> m_vJoyStates;

        void elementEvent(irr::gui::IGUIElement* a_pElement, bool a_bEnter);
        void resetSelected();

        void updateConfigData();

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent);
        ~CControllerUi();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        void buildUi(irr::gui::IGUIElement *a_pParent);

        virtual bool update(const irr::SEvent& a_cEvent);

        /**
        * Get the configuration of the controller as XML string
        * @return the configuration of the controller
        */
        std::string getControllerString();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        /**
        * The "setText" method is abused to pass the configuration XML string to the UI
        * @param a_pText the new XML string
        */
        virtual void setText(const wchar_t* a_pText);

        /**
        * Change the font for the configuration dialog
        */
        void setFont(irr::gui::IGUIFont* a_pFont);

        virtual void OnJoystickEvent(const irr::SEvent& a_cEvent);

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  } // namespace controller 
} // namespace dustbin