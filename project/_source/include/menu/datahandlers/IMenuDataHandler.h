#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace menu {
    /**
    * @class IMenuDataHandler
    * @author Christian Keimel
    * This interface must be implmeneted by all menu data handlers
    */
    class IMenuDataHandler {
      public:
        IMenuDataHandler() { }

        virtual ~IMenuDataHandler() { }

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) = 0;
    };
  }
}
