// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerMenu
    * @author Christian Keimel
    * This class handles the controls to use the menues with a game controller (or keyboard)
    */
    class CControllerMenu : public CControllerBase {
      private:
        enum class enDirection {
          Up,
          Down,
          Left,
          Right
        };

        bool m_bButtonDown,
             m_bMoved,
             m_bActive,
             m_bEvent,
             m_bCancelDown,
             m_bOkDown;

        irr::gui::IGUIEnvironment *m_pGui;
        irr::video::IVideoDriver  *m_pDrv;
        irr::gui::ICursorControl  *m_pCursor;
        irr::IrrlichtDevice       *m_pDevice;

        irr::core::position2di m_cMousePos;   /**< The mouse position (mainly for Android) */

        irr::core::dimension2du m_cEditSize;

        irr::ITimer* m_pTimer;

        int m_iZLayer;    /**< The current Z-Layer */

        std::vector<irr::gui::IGUIElement *> m_vElements;  /**< all relevant UI elements */

        std::vector<int> m_vColumns;    /**< Store the columns that contain UI elements */
        std::vector<int> m_vRows;       /**< Store the rows that contain UI elements */

        irr::gui::IGUIElement *m_pSelected;

        std::wstring m_sEditChars;    /**< String with the characters for name editing with the menu controller */
        int m_iEditTime;              /**< Timestamp when the last editing was done */

        bool m_bFirstCall;    /**< Then first called on Android we set the virtual cursor on the first element */

        irr::video::ITexture *m_pArrows[2];

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

        /**
        * Fill the "m_vElements" vector
        * @param a_pParent the item to add, the children will be iterated and added as well
        * @param a_iZLayer the current Z-Layer
        */
        void fillItemList(irr::gui::IGUIElement* a_pParent, int a_iZLayer);

        /**
        * Get the Z-Layer of an item. Iterates through all ancestors until either a "MenuBackground" element or the root element
        * is found. If a "MenuBackground" is found it's "Z-Layer" property is returned, for the root element "0" is returned
        * @param a_pItem the item to get the Z-Layer
        * @return the Z-Layer of the item
        */
        int getElementZLayer(irr::gui::IGUIElement* a_pItem);

      public:
        CControllerMenu(int a_iZLayer);
        virtual ~CControllerMenu();

        /**
        * Event handler for this input
        * @param a_cEvent the event to handle
        */
        virtual bool update(const irr::SEvent& a_cEvent);

        /**
        * Change the Z-Layer the controller controls
        * @param a_iZLayer the new Z-Layer
        */
        void setZLayer(int a_iZLayer);

        /**
        * Reset all necessary members as a new menu was loaded
        */
        void reset();

        void draw();
    };

  } // namespace controller
} // namespace dustbin