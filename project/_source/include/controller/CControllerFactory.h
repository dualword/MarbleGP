// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IController.h>
#include <threads/IQueue.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerFactory
    * @author Christian Keimel
    * This class generates a controller instance out of the
    * controller configuration string
    */
    class CControllerFactory {
      private:
        threads::IQueue* m_pQueue;

      public:
        CControllerFactory(threads::IQueue* a_pQueue);

        /**
        * Create a controller from the configuration string
        * @returns a new controller instance
        */
        IController* createController(int a_iMarbleId, const std::string& a_sControls);
    };
  }
}
