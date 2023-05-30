// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IController.h>
#include <data/CDataStructs.h>
#include <threads/IQueue.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    class CAiNode;
  }

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
        IController* createController(
          int                          a_iMarbleId, 
          const std::string           &a_sControls, 
          bool                         a_bAutoThrottle,
          data::SPlayerData::enAiHelp  a_eAiHelp, 
          scenenodes::CAiNode         *a_pAiNode, 
          const std::string           &a_sAiScript,
          const irr::core::recti      &a_cViewport,
          bool                        &a_bShowRaking
        );
    };
  }
}
