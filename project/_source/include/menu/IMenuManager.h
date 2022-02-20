// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <string>

namespace dustbin {
  namespace menu {
    class IMenuHandler;   /**< Forward declaration of the menu handler */
    /**
    * @class IMenuManager
    * @author Christian Keimel
    * This small interface needs to be implemented by the main class
    * @see dustbin::CMainClass
    */
    class IMenuManager {
      public:
        virtual ~IMenuManager() { }

        /**
        * Change the menu
        * @param a_pMenu the new active menu
        */
        virtual IMenuHandler *changeMenu(IMenuHandler *a_pMenu) = 0;

        /**
        * Push a menu to the menu stack
        * @param a_sMenu the name of the menu
        */
        virtual void pushToMenuStack(const std::string &a_sMenu) = 0;

        /**
        * Clear the menu stack
        */
        virtual void clearMenuStack() = 0;

        /**
        * Get the topmost entry of the menu stack without removing it
        * @return the topmost entry of the menu stack
        */
        virtual std::string peekMenuStack() = 0;

        /**
        * Get the topmost entry of the menu stack and remove it
        * @return the topmost entry of the menu stack
        */
        virtual std::string popMenuStack() = 0;
    };

  }
}
