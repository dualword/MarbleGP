// (w) 2021 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <controller/CMarbleController.h>
#include <messages/CSerializer64.h>

namespace dustbin {
  namespace controller {
    CMarbleController::CMarbleController(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue) : IController(a_pQueue), m_pController(nullptr), m_iMarbleId(a_iMarbleId) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sControls.c_str());

      std::string l_sHead = l_cSerializer.getString();

      if (l_sHead == "DustbinController") {
        m_pController = new controller::CControllerGame();
        m_pController->deserialize(a_sControls);
      }
    }

    CMarbleController::~CMarbleController() {
      if (m_pController != nullptr)
        delete m_pController;
    }

    /**
    * This message must be implemented by all descendants. If called
    * it posts a control message to the queue.
    */
    void CMarbleController::postControlMessage() {
      if (m_pQueue != nullptr && m_pController != nullptr) {
        irr::f32 l_fCtrlX = m_pController->getSteer(),
                 l_fCtrlY = m_pController->getThrottle();

        irr::s8 l_iCtrlX = (irr::s8)(127.0f * (l_fCtrlX > 1.0f ? 1.0f : l_fCtrlX < -1.0f ? -1.0f : l_fCtrlX)),
                l_iCtrlY = (irr::s8)(127.0f * (l_fCtrlY > 1.0f ? 1.0f : l_fCtrlY < -1.0f ? -1.0f : l_fCtrlY));

        messages::CMarbleControl l_cMessage = messages::CMarbleControl(m_iMarbleId, l_iCtrlX, l_iCtrlY, m_pController->getBrake(), m_pController->getRearView(), m_pController->getRespawn());
        m_pQueue->postMessage(&l_cMessage);
      }
    }

    /**
    * Update the controller with the Irrlicht event
    * @param a_cEvent the Irrlicht event
    */
    void CMarbleController::update(const irr::SEvent& a_cEvent) {
      if (m_pController != nullptr)
        m_pController->update(a_cEvent);
    }
  }
}