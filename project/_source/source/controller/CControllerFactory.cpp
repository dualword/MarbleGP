
#include <controller/CControllerFactory.h>
#include <controller/CMarbleController.h>
#include <controller/CControllerAI.h>
#include <messages/CSerializer64.h>

namespace dustbin {
  namespace controller {
    CControllerFactory::CControllerFactory(threads::IQueue* a_pQueue) : m_pQueue(a_pQueue) {
    }

    /**
    * Create a controller from the configuration string
    * @returns a new controller instance
    */
    IController* CControllerFactory::createController(
      int                          a_iMarbleId, 
      const std::string           &a_sControls, 
      data::SPlayerData::enAiHelp  a_eAiHelp, 
      scenenodes::CAiNode         *a_pAiNode, 
      const std::string           &a_sAiScript,
      const irr::core::recti      &a_cViewport
    ) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sControls.c_str());

      std::string l_sHead = l_cSerializer.getString();

      if (l_sHead == "DustbinController") {
        return new CMarbleController(a_iMarbleId, a_sControls, a_pAiNode, a_eAiHelp, m_pQueue, a_sAiScript, a_cViewport);
      }

      return nullptr;
    }
  }
}
