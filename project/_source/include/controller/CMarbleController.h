// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/CControllerGame.h>
#include <controller/IController.h>
#include <threads/IQueue.h>
#include <string>

namespace dustbin {
  namespace controller {
    /**
    * @class CMarbleController
    * @author Christian Keimel
    * This class links the control input to the controller interface
    */
    class CMarbleController : public IController {
      protected:
        CControllerGame* m_pController;
        int              m_iMarbleId;

      public:
        CMarbleController(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue);

        virtual ~CMarbleController();

        /**
        * This message must be implemented by all descendants. If called
        * it posts a control message to the queue.
        */
        virtual void postControlMessage();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent);
    };
  }
}
