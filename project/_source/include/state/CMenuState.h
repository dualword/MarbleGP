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

  namespace threads {
    class CInputQueue;    /**< Forward declaration of the input queue */
  }

  namespace network {
    class CGameClient;  /**< Forward declaration of the network game client */
  }

  namespace messages {
    class IMessage;   /**< Forward declaration of the message interface */
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

        bool m_bMenuCtrl;   /**< Menu controller enabled? */

        controller::CControllerMenu      *m_pController;    /**< The controller for navigating the menues with a joypad */
        controller::ICustomEventReceiver *m_pTouchCtrl;     /**< Touch controller for the menu */

        std::vector<std::string> m_vMenuStack;    /**< A stack for the menues that shall be activated */

      protected:
        network::CGameClient *m_pClient;
        threads::CInputQueue *m_pInputQueue;

      public:
        CMenuState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal);
        virtual ~CMenuState();

        /**
        * This method is called when the state is activated
        */
        virtual void activate() override;

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate() override;

        /**
        * Change the Z-Layer for the Menu Controller
        * @param a_iZLayer the new Z-Layer
        */
        virtual void setZLayer(int a_iZLayer) override;

        /**
        * Return the state's ID
        */
        virtual enState getId() override;

        /**
        * Event handling method. The main class passes all Irrlicht events to this method
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run() override;

        /**
        * Change the menu
        * @param a_pMenu the new active menu
        */
        virtual menu::IMenuHandler *changeMenu(menu::IMenuHandler *a_pMenu) override;

        /**
        * Callback before a menu is changed, deletes the current menu
        */
        virtual void beforeChangeMenu() override;

        /**
        * Push a menu to the menu stack
        * @param a_sMenu the name of the menu
        */
        virtual void pushToMenuStack(const std::string &a_sMenu) override;

        /**
        * Clear the menu stack
        */
        virtual void clearMenuStack() override;

        /**
        * Get the topmost entry of the menu stack without removing it
        * @return the topmost entry of the menu stack
        */
        virtual std::string peekMenuStack() override;

        /**
        * Get the topmost entry of the menu stack and remove it
        * @return the topmost entry of the menu stack
        */
        virtual std::string popMenuStack() override;

        /**
        * Set a controller to handle touch input
        */
        void setTouchController(controller::ICustomEventReceiver *a_pTouchCtrl);

        /**
        * Handle a message received by the network client
        * @param a_pMessage the message to handle
        */
        virtual void handleMessage(messages::IMessage *a_pMessage) { }

        /**
        * A callback for the menu state to get informed about a menu change
        * @param a_sMenu the loaded menu
        */
        virtual void menuChanged(const std::string &a_sMenu) override;

        /**
        * Does this state want the virtual keyboard?
        */
        virtual bool showVirtualKeyboard() override;

        /**
        * Turn the menu controller on or off
        * @param a_bFlag on / off flag
        */
        virtual void enableMenuController(bool a_bFlag);
    };
  }
}
