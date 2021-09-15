// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <messages/IMessage.h>
#include <threads/IQueue.h>

namespace dustbin {
  namespace controller {
    /**
    * @class IController
    * @author Christian Keimel
    * This is the interface for all game controllers. Unlike
    * "CControllerBase" and it's descandents it just encapsulates
    * the controller input and makes it available to the game
    */
    class IController {
      protected:
        threads::IQueue* m_pQueue;

      public:
        IController(threads::IQueue* a_pQueue) {
          m_pQueue = a_pQueue;
        }

        virtual ~IController() { }

        /**
        * This message must be implemented by all descendants. If called
        * it posts a control message to the queue.
        */
        virtual void postControlMessage() = 0;

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) = 0;
    };
  }
}
