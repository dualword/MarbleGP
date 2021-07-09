// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerMenu
    * @author Christian Keimel
    * This class handles the controls to use the menues with a game controller (or keyboard)
    */
    class CControllerMenu : public CControllerBase {
      private:
        bool m_aCtrlReady[6];

        bool wasButtonPressed(unsigned a_iIndex);

      public:
        CControllerMenu();
        ~CControllerMenu();

        /**
        * Is a "move up" requested?
        * @return a boolean
        */
        bool moveUp();

        /**
        * Is a "move down" requested?
        * @return a boolean
        */
        bool moveDown();

        /**
        * Is a "move left" requested?
        * @return a boolean
        */
        bool moveLeft();

        /**
        * Is a "move right" requested?
        * @return a boolean
        */
        bool moveRight();

        /**
        * Was the "OK / Enter" button pressed?
        * @return a boolean
        */
        bool enterPressed();

        /**
        * Was the "Cancel" button pressed?
        */
        bool cancelPressed();
    };

  } // namespace controller
} // namespace dustbin