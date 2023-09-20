// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace gui {
    class CVirtualKeyboard;
  }

  namespace controller {
    /**
    * @class CControllerMenu
    * @author Christian Keimel
    * This class handles the controls to use the menues with a game controller (or keyboard)
    */
    class CControllerMenu : public CControllerBase {
      private:
        /**
        * @class SOldAxisPos
        * @author Christian Keimel
        * This structure is necessary for the map with the old axis
        * values for selecting the gamepad to control the menu
        * @see m_mOldAxisPos
        */
        typedef struct SOldAxisPos {
          int m_iAxis[18];
        } SOldAxisPos;

        enum class enDirection {
          Up    = 0,
          Down  = 1,
          Left  = 2,
          Right = 3,
          Any   = 4
        };

        bool m_bButtonDown;
        bool m_bActive;
        bool m_bEvent;
        bool m_bCancelDown;
        bool m_bOkDown;
        bool m_bVisible;
        bool m_bDebug;

        int m_iRaster;

        irr::gui::IGUIEnvironment *m_pGui;
        irr::video::IVideoDriver  *m_pDrv;
        irr::gui::ICursorControl  *m_pCursor;
        irr::IrrlichtDevice       *m_pDevice;
        
        irr::core::position2di m_cMousePos;   /**< The mouse position (mainly for Android) */

        irr::core::dimension2du m_cEditSize;

        irr::core::dimension2du m_cScreen;

        irr::ITimer* m_pTimer;

        irr::gui::IGUIElement *m_pSelected;

        std::wstring m_sEditChars;    /**< String with the characters for name editing with the menu controller */

        irr::u32 m_aNextEvent[4];   /**< Timestamp of the last movement events */
        irr::s32 m_iGamepad;        /**< The menu control gamepad (if wanted) */
        bool     m_bGamepad;        /**< Does the control use a gampad? */

        std::map<int, SOldAxisPos> m_mOldAxisPos;   /**< The old axis position of the joysticks to select the menu controller gamepad */

        irr::video::ITexture *m_pArrows[2];
        irr::video::ITexture *m_pCursorTexture;   /**< The cursor for Android */

        std::map<int, std::vector<irr::gui::IGUIElement *>> m_mRows;    /**< The rows of items */
        std::map<int, std::vector<irr::gui::IGUIElement *>> m_mCols;    /**< The columns of items */

        std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator m_itRow;
        std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator m_itCol;

        /**
        * Is this element really visible?
        * @param p the element to check
        * @return true if the element and all ancestors are visible
        */
        bool isElementVisible(irr::gui::IGUIElement *p);

        /**
        * Fill the vector with all supported editable UI elements
        * @param a_vElements the vector filled with the options
        * @param a_pThis the current UI element
        */
        void fillElementVector(std::vector<irr::gui::IGUIElement *> &a_vElements, irr::gui::IGUIElement *a_pThis);

        /**
        * Find the possible next UI elements when moving the mouse pointer
        * @param a_eDirection the direction to move to
        */
        std::vector<irr::gui::IGUIElement *> findMoveOptions(enDirection a_eDirection);

        /**
        * Find the best option for movement
        * @param a_eDirection the direction to search for an option
        * @return the GUI element that is the best option for the movement
        */
        irr::gui::IGUIElement *findBestOption(enDirection a_eDirection);

        /**
        * Find the correct iterator for the current column
        */
        void findColumnIterator();

        /**
        * Find the correct iterator for the current row
        */
        void findRowIterator();

        /**
        * Change the mouse position after controller input
        * @param a_eDirection the controller input
        */
        void moveMouse(enDirection a_eDirection);

        /**
        * Check an item and all it's parents for visibility
        * @param a_pItem the item to check
        * @return true if the item and all parents are visible
        */
        bool isVisible(irr::gui::IGUIElement* a_pItem);

      public:
        CControllerMenu();
        virtual ~CControllerMenu();

        /**
        * Event handler for this input
        * @param a_cEvent the event to handle
        */
        virtual bool update(const irr::SEvent& a_cEvent) override;

        /**
        * Reset all necessary members as a new menu was loaded
        */
        void reset();

        void draw();

        /**
        * Set the cursor visibility (only relevant for Android)
        * @param a_bVisible the new visibility flag
        */
        void setVisible(bool a_bVisible);

        /**
        * Check if the joystick is already assigned
        * @return true if the joystick is already assigned, false otherwise
        */
        virtual bool isJoystickAssigned() override;
    };

  } // namespace controller
} // namespace dustbin