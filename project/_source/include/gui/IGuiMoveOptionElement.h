// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace gui {
    /**
    * @class IGuiMoveOptionElement
    * @author Christian Keimel
    * This interface needs to be implemented by GUI elements
    * that want to react to cursor control
    */
    class IGuiMoveOptionElement {
      public:
        virtual ~IGuiMoveOptionElement() { }

        /**
        * Get a position to move to depending on the direction and the given mouse position
        * @param a_cMousePos the mouse position
        * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
        * @param a_cOut the position to move to
        * @return true if a position was found, false otherwise
        */
        virtual bool getMoveOption(const irr::core::position2di &a_cMousePos, int a_iDirection, irr::core::position2di &a_cOut) = 0;
    };
  }
}
