// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    /**
    * @class IJoystickEventHandler
    * @author Christian Keimel
    * This class is used to pass joystick events to CControllerUi instances as Irrlicht
    * does not pass joystick events to UI elements
    */
    class IJoystickEventHandler {
      public:
        virtual void OnJoystickEvent(const irr::SEvent& a_cEvent) = 0;
    };
  }
}