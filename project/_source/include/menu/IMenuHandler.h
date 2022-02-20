// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace state {
    class IState; /**< Forward declaration of the "IState" interface */
  }

  namespace menu {
    class IMenuManager;   /**< Forward declaration of the menu manager */

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

      public:
        IMenuHandler(irr::IrrlichtDevice *a_pDevice, IMenuManager *a_pManager, state::IState *a_pState);
        virtual ~IMenuHandler();

        virtual bool OnEvent(const irr::SEvent &a_cEvent) = 0;

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual void run() { }

        /**
        * This static method creates the requested menu handler
        * @param a_sName string identifier of the menu
        * @return "true" on success, "false" otherwise
        */
        static IMenuHandler *createMenu(const std::string &a_sName, irr::IrrlichtDevice *a_pDevice, IMenuManager *a_pManager, state::IState *a_pState);

        /**
        * Change the Z-Layer. This is important for controlling the menu
        * via gamepad
        */
        void changeZLayer(int a_iZLayer);
    };

    irr::gui::IGUIElement* findElementByIdAndType(irr::s32 a_iId, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement);
    irr::gui::IGUIElement* findElementByNameAndType(const std::string &a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement);
  }
}
