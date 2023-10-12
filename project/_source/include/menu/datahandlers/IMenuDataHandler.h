#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace menu {
    /**
    * @class IMenuDataHandler
    * @author Christian Keimel
    * This interface must be implmeneted by all menu data handlers
    */
    class IMenuDataHandler {
      protected:
        irr::IrrlichtDevice       *m_pDevice;   /**< The Irrlicht device */
        irr::video::IVideoDriver  *m_pDrv;      /**< The Irrlicht video driver */
        irr::gui::IGUIEnvironment *m_pGui;      /**< The Irrlicht GUI environment */
        irr::io::IFileSystem      *m_pFs;       /**< The Irrlicht file system */

        /**
        * Find a GUi element by name and optionally type
        * @param a_sName the name of the UI element
        * @param a_eType the type of the UI element (if irr::gui::EGUIET_COUNT (default) is passed the type is not considered)
        */
        irr::gui::IGUIElement *findElement(const std::string &a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType = irr::gui::EGUIET_COUNT);

      public:
        IMenuDataHandler();

        virtual ~IMenuDataHandler();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) = 0;

        /**
        * Gets called every frame, maybe it's necessary to implement something
        */
        virtual void runDataHandler();
    };
  }
}
