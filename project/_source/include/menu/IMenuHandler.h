// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace state {
    class IState; /**< Forward declaration of the "IState" interface */
  }

  namespace menu {
    class IMenuDataHandler; /**< Forward declaration of the menu handler */
    class IMenuManager;     /**< Forward declaration of the menu manager */

    /**
    * @class IMenuHandler
    * @author Christian Keimel
    * This interface must be implemented for all menues
    */
    class IMenuHandler {
      protected:
        irr::IrrlichtDevice       *m_pDevice;
        irr::io::IFileSystem      *m_pFs;
        irr::gui::IGUIEnvironment *m_pGui;
        irr::video::IVideoDriver  *m_pDrv;
        irr::scene::ISceneManager *m_pSmgr;

        IMenuManager *m_pManager;

        state::IState *m_pState;

        IMenuDataHandler *m_pDataHandler;

        /**
        * Find a gui element by name
        * @param a_sName name of the queried element
        * @param a_eType type of the element, the type doesn't matter if the type is "irr::gui::EGUIET_COUNT"
        * @return the element, nullptr if no match was found
        */
        irr::gui::IGUIElement *findElement(const std::string &a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType = irr::gui::EGUIET_COUNT);

      public:
        IMenuHandler(irr::IrrlichtDevice *a_pDevice, IMenuManager *a_pManager, state::IState *a_pState);
        virtual ~IMenuHandler();

        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        /**
        * This method is called to activate the menu.
        */
        virtual void activate() { }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        * @return true if drawing was done by the menu, false otherwise
        */
        virtual bool run();

        /**
        * This static method creates the requested menu handler
        * @param a_sName string identifier of the menu
        * @return "true" on success, "false" otherwise
        */
        static IMenuHandler *createMenu(const std::string &a_sName, irr::IrrlichtDevice *a_pDevice, IMenuManager *a_pManager, state::IState *a_pState);

        /**
        * Get the menu manager
        * @return the menu manager
        */
        IMenuManager *getMenuManager();

        /**
        * Does this menu handle the state change network message itself?
        * Might me necessary if some data needs to be written to a global
        * @return true it this menu handle the state change network message itself
        */
        virtual bool handlesNetworkStateChange();
    };
  }
}
