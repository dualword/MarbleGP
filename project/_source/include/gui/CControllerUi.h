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

    class CDustbinScrollPane; /**< Forward declaration of the scroll pane */

    /**
    * @class CControllerUi
    * @author Christian Keimel
    * This class provides a UI interface to configure controls
    */
    class CControllerUi : public gui::CMenuBackground, public controller::CControllerBase, public controller::IJoystickEventHandler, public gui::IGuiMoveOptionElement {
      protected:
        irr::gui::ICursorControl *m_pCursor;
        irr::gui::IGUIFont       *m_pFont;
        irr::gui::IGUIFont       *m_pSmall;
        menu::IMenuManager       *m_pMenuMgr;
        irr::core::position2di    m_cMousePos;    /**< Position of the mouse, filled with "mouse moved" events as Android has no ICursor instance */
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

        /**
        * Search for the nearest collision of a label with a rectangle
        * @param a_cRect the rect to collide with
        * @param a_cOffset offset for the rectangle (will be used to enlarge the rect)
        * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
        * @param a_cOut [out] the result rectangle
        * @return true if a collision was found, false otherwise
        */
        bool checkForRectCollision(const irr::core::recti &a_cRect, const irr::core::dimension2du &a_cOffset, int a_iDirection, irr::core::recti &a_cOut, irr::core::recti &a_cCheck);

        /**
        * Search for the nearest label to the mouse cursor in a particular direction.
        * Only called if no label was found using checkForRectCollision
        * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
        * @param a_cOut [out] the result rectangle
        * @return true if a collision was found, false otherwise
        */
        bool checkForNearestLabel(int a_iDirection, irr::core::recti &a_cOut, irr::core::recti &a_cCheck);

      public:
        CControllerUi(irr::gui::IGUIElement *a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType);
        ~CControllerUi();

        /**
        * This method builds the UI for the controller
        * @param a_pParent the parent element. The outbox of this element will be used for the UI
        */
        virtual void buildUi(irr::gui::IGUIElement *a_pParent) = 0;

        virtual bool update(const irr::SEvent& a_cEvent) override;

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

        /**
        * Get a position to move to depending on the direction and the given mouse position
        * @param a_cMousePos the mouse position
        * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
        * @param a_cOut the position to move to
        * @return true if a position was found, false otherwise
        */
        virtual bool getMoveOption(const irr::core::position2di &a_cMousePos, int a_iDirection, irr::core::position2di &a_cOut) override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace controller 
} // namespace dustbin