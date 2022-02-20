// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    /**
    * @class ICustomEventReceiver
    * @author Christian Keimel
    * This is the interface that should be implemented for special
    * input handling, e.g. touch input. The main class accepts two
    * of these, one for menu with emulated mouse input, one for the
    * game where the touch creates controller messages
    */
    class ICustomEventReceiver {
      protected:
        irr::IrrlichtDevice *m_pDevice;

      public:
        ICustomEventReceiver(irr::IrrlichtDevice *a_pDevice) : m_pDevice(a_pDevice) { }
        virtual ~ICustomEventReceiver() { }

        virtual bool handleEvent(const irr::SEvent &a_cEvent) = 0;
    };
  }
}

