// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
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
        /**
        * The mode of the controller UI
        */
        enum class enMode {
          Display,          /**< Just show the controls */
          Wizard,           /**< Control wizard to step through the control items and initialize them */
          Test              /**< Test the controls */
        };

        /**
        * The control type of the UI
        */
        enum class enControl {
          Off,                  /**< Controller deactivated */
          Keyboard,             /**< Only receive keyboard events */
          Joystick              /**< Only receive joystick events */
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

        struct SJoystickState {
          irr::s16 m_iAxis    [irr::SEvent::SJoystickEvent::NUMBER_OF_AXES   ];
          bool     m_bButton  [irr::SEvent::SJoystickEvent::NUMBER_OF_BUTTONS];
          irr::u16 m_iPov;
          
          SJoystickState();

          void update(const irr::SEvent &a_cEvent);

          bool isNeutral();
          bool hasChanged(const irr::SEvent &a_cEvent);
        };

        controller::CControllerBase *m_pController;
        irr::video::IVideoDriver    *m_pDrv;
        irr::gui::IGUIFont          *m_pFont;
        enMode                       m_eMode;
        enControl                    m_eCtrl;
        int                          m_iWizard;     /**< The item index of the control item the wizard is currently editing */
        bool                         m_bSet;        /**< Was a value set? */
        irr::u8                      m_iJoystick;   /**< The joystick index to listen for (for the wizard) */
        int                          m_iErrorCnt;   /**< The error counter (conflicting inputs will flash four times) */
        int                          m_iErrorTime;  /**< The last time an error has flased */
        bool                         m_bError;      /**< Is there an error, i.e. conflicting inputs */
        int                          m_iErrorCtrl;  /**< Index of the conflicting control if an error was found */
        int                          m_iNextSet;    /**< Hack as my gamepad on Android sends a button pressed after the shoulder axis are used */
        irr::ITimer                 *m_pTimer;      /**< The Irrlicht timer */

        std::vector<SGuiElement> m_vGui;

        std::map<irr::u8, SJoystickState> m_mJoysticks;
        std::map<irr::u8, SJoystickState> m_mNeutral;   /**< We assume that the joysticks are in neutral state */

        void calculateGui();

        /**
        * The child classes nees to implement this method for
        * creating default controls if the controller type
        * is changed
        * @param a_eCtrl the new controller type
        */
        virtual void resetToDefaultForMode(enControl a_eCtrl) = 0;

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType);
        virtual ~CControllerUi();

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

        /**
        * Set the mode of the controller UI
        * @param a_eMode the new mode
        */
        void setMode(enMode a_eMode);

        /**
        * Get the current mode
        * @return the current mode
        */
        enMode getMode();

        /**
        * Set the control type of the UI
        * @param a_eCtrl the new control type
        */
        void setControlType(enControl a_eCtrl);

        /**
        * Get the control type
        * @return the control type
        */
        enControl getControlType();

        /**
        * Start the controller configuration wizard
        */
        void startWizard();

        /**
        * Start the test of the controller configuration
        */
        void startTest();

        /**
        * Get the actual controller
        * @return the actual controller
        */
        controller::CControllerBase *getController();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin