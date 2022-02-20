// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <state/IState.h>
#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Forward declration of the global singleton class */

  namespace menu {
    class IMenuHandler;   /**< Forward declaration of the menu handler */
  }

  namespace controller {
    class CControllerMenu;        /**< Forward declaration of the menu controller */
    class ICustomEventReceiver;   /**< Forward declaration of the custom event receiver interface */
  }

  namespace state {
    /**
    * @class CMenuState
    * @author Christian Keimel
    * This state is used to show menu dialogs to the user
    */
    class CMenuState : public IState, public menu::IMenuManager {
      private:
        menu::IMenuHandler *m_pMenu;

        controller::CControllerMenu      *m_pController;    /**< The controller for navigating the menues with a joypad */
        controller::ICustomEventReceiver *m_pTouchCtrl;     /**< Touch controller for the menu */

        std::vector<std::string> m_vMenuStack;    /**< A stack for the menues that shall be activated */

      public:
        CMenuState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal);
        virtual ~CMenuState();

        /**
        * This method is called when the state is activated
        */
        virtual void activate();

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate();

        /**
        * Change the Z-Layer for the Menu Controller
        * @param a_iZLayer the new Z-Layer
        */
        virtual void setZLayer(int a_iZLayer);

        /**
        * Return the state's ID
        */
        virtual enState getId();

        /**
        * Event handling method. The main class passes all Irrlicht events to this method
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run();

        /**
        * Change the menu
        * @param a_pMenu the new active menu
        */
        virtual menu::IMenuHandler *changeMenu(menu::IMenuHandler *a_pMenu);

        /**
        * Push a menu to the menu stack
        * @param a_sMenu the name of the menu
        */
        virtual void pushToMenuStack(const std::string &a_sMenu);

        /**
        * Clear the menu stack
        */
        virtual void clearMenuStack();

        /**
        * Get the topmost entry of the menu stack without removing it
        * @return the topmost entry of the menu stack
        */
        virtual std::string peekMenuStack();

        /**
        * Get the topmost entry of the menu stack and remove it
        * @return the topmost entry of the menu stack
        */
        virtual std::string popMenuStack();

        /**
        * Set a controller to handle touch input
        */
        void setTouchController(controller::ICustomEventReceiver *a_pTouchCtrl);
    };
  }
}
