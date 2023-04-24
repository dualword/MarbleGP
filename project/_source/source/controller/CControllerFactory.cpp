
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
      const irr::core::recti      &a_cViewport,
      bool                        &a_bShowRaking
    ) {

      bool l_bDataValid = false;

      std::string l_aCtrl[] = {
        "DustbinController"    ,
        "DustbinTouchControl"  ,
        "DustbinTouchSteerLeft",
        "DustbinTouchSteerRight",
        "DustbinTouchSteerOnly",
        "DustbinGyroscope"     ,
        ""                      
      };

      for (std::string l_sCtrl : l_aCtrl) {
        std::string l_sSub = a_sControls.substr(0, l_sCtrl.size());
        if (l_sSub == l_sCtrl) {
          l_bDataValid = true;

          a_bShowRaking = l_sSub == "DustbinController" || l_sSub == "DustbinGyroscope";

          break;
        }
      }

      if (l_bDataValid) {
        return new CMarbleController(a_iMarbleId, a_sControls, a_pAiNode, a_eAiHelp, m_pQueue, a_sAiScript, a_cViewport);
      }

      return nullptr;
    }
  }
}
