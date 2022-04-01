// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <messages/IMessage.h>
#include <threads/IQueue.h>
#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    class CGuiAiDebug;
  }

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

        virtual void onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cNewPos) { }

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp) { }

        virtual void onMarbleRespawn(int a_iMarbleId) { }

        virtual void setDebugGui(gui::CGuiAiDebug *a_pDebug) { }
    };
  }
}
