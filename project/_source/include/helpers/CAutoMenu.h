#pragma once

#include <data/CDataStructs.h>
#include <irrlicht.h>

namespace dustbin {
  namespace menu {
    class IMenuHandler;
  }

  namespace helpers {
    /**
    * @class CAutoMenu
    * @author Christian Keimel
    * This class does automatically send "Button OK clicked" events to the recveiver
    * after 5 seconds if only bots are racing
    */
    class CAutoMenu {
      private:
        irr::ITimer *m_pTimer;    /**< The Irrlicht Timer */
        irr::u32     m_iTime;     /**< The start time */
        bool         m_bBotsOnly; /**< Only bots in the race */

        menu::IMenuHandler *m_pReciever;   /**< the event receiver to send the event to */

      public:
        CAutoMenu(irr::IrrlichtDevice *a_pDevice, menu::IMenuHandler *a_pRecveiver);

        virtual ~CAutoMenu();

        /**
        * This method is called every "run" event of the menu and
        * sends a "button OK clicked" event to the receiver if
        * only bots are racing and 5 seconds have passed
        */
        void process();
    };
  }
}
