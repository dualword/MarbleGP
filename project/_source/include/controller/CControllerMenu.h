// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <vector>

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
             m_bMoved;

        irr::gui::IGUIEnvironment* m_pGui;
        irr::gui::ICursorControl * m_pCursor;

        irr::ITimer* m_pTimer;

        int m_iRaster;

        std::vector<irr::gui::IGUIElement *> m_vElements;  /**< all relevant UI elements */

        irr::gui::IGUIElement* m_pHovered,
                             * m_pSelected;

        void moveMouse(enDirection a_eDirection);

        bool isVisible(irr::gui::IGUIElement* a_pItem);

        /**
        * Fill the "m_vElements" vector
        * @param a_pParent the item to add, the children will be iterated and added as well
        */
        void fillItemList(irr::gui::IGUIElement* a_pParent);

        irr::gui::IGUIElement* findElement(int a_iStep, enDirection a_eDirection);

      public:
        CControllerMenu();
        ~CControllerMenu();

        /**
        * Event handler for this input
        * @param a_cEvent the event to handle
        */
        virtual void update(const irr::SEvent& a_cEvent);
    };

  } // namespace controller
} // namespace dustbin