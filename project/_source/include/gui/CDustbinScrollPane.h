// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace gui {
    /**
    * @class CDustbinScrollPane
    * @author Christian Keimel
    * This class helps scrolling items in a menu
    */
    class CDustbinScrollPane {
      private:
        bool m_bMouseDown;

        irr::core::recti m_cRectOuter;    /**< The rectangle of this scroll pane */
        irr::core::recti m_cRectInner;    /**< The inner rectangle, i.e. where all the items are placed */
        irr::core::recti m_cScrollBar;    /**< The visible element showing the scroll position */

        irr::core::position2di m_cScrollPos;    /**< The scroll position */
        irr::core::position2di m_cScrollMax;    /**< The maximum scroll values (minimum is always 0) */
        irr::core::position2di m_cMouseDown;    /**< The mouse position where the left button was pressed down */
        

        std::vector<irr::core::recti> m_vItems;   /**< The registered rectangles */

      public:
        /**
        * The contructor
        * @param a_cRect the outer rectangle to be scrolled
        */
        CDustbinScrollPane(const irr::core::recti &a_cRect);

        /**
        * Add an item to the scroll pane
        * @param a_cRect the rectangle to add
        */
        void addRectangle(const irr::core::recti &a_cRect);

        /**
        * Get the current scroll position
        * @return the current scroll position
        */
        const irr::core::position2di &getScrollPosition();

        /**
        * Try to put an UI element into the center of the pane, or at least make it visilbe
        * @param a_cRect the rectangle to center
        */
        void centerElement(const irr::core::recti &a_cRect);

        /**
        * Handle Irrlicht events. Mouse clicks will only be taken notice of, mouse movements
        * will be consumed if the left mouse button is down
        * @param a_cEvent the event to handle
        */
        bool OnEvent(const irr::SEvent &a_cEvent);
    };
  }
}
