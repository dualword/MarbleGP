// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IController.h>

namespace dustbin {
  namespace controller {
    /**
    * @class CFreeCameraController
    * @author Christian Keimel
    * This is the controller class for controlling a free
    * camera, e.g. for "View Track" and "Race Replay"
    */
    class CFreeCameraController : public IController {
      public:
        CFreeCameraController(threads::IQueue* a_pQueue) : IController(a_pQueue) { }

        virtual ~CFreeCameraController() { }

        /**
        * This message must be implemented by all descendants. If called
        * it posts a control message to the queue.
        */
        virtual void postControlMessage() { }

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) { }
    };
  }
}
