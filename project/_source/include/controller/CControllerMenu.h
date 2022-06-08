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
        enum class enDirection {
          Up,
          Down,
          Left,
          Right,
          Any
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

        int m_iZLayer;    /**< The current Z-Layer */

        irr::gui::IGUIElement *m_pSelected;

        std::wstring m_sEditChars;    /**< String with the characters for name editing with the menu controller */
        int m_iEditTime;              /**< Timestamp when the last editing was done */

        bool m_bFirstCall;    /**< Then first called on Android we set the virtual cursor on the first element */

        irr::u32 m_aNextEvent[4];   /**< Timestamp of the last movement events */

        irr::video::ITexture *m_pArrows[2];

        std::map<int, std::vector<irr::gui::IGUIElement *>> m_mRows;    /**< The rows of items */
        std::map<int, std::vector<irr::gui::IGUIElement *>> m_mCols;    /**< The columns of items */

        std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator m_itRow;
        std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator m_itCol;

        /**
        * Get the Z-Layer of an element
        * @param p the element to check
        * @return the Z-Layer of the element
        */
        int getZLayer(irr::gui::IGUIElement *p);

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
        * @param a_eDirection
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

        /**
        * Set the cursor visibility (only relevant for Android)
        * @param a_bVisible the new visibility flag
        */
        void setVisible(bool a_bVisible);
    };

  } // namespace controller
} // namespace dustbin