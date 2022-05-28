// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <messages/CSerializer64.h>
#include <network/CNetBase.h>
#include <CGlobal.h>
#include <chrono>


namespace dustbin {
  namespace network {
    CNetBase::CNetBase(CGlobal* a_pGlobal) :
      m_pGlobal(a_pGlobal),
      m_pHost  (nullptr)
    {
      if (m_pGlobal->getGlobal("enet_initialized") != "true") {
        if (enet_initialize() != 0) {
          printf("Error while initializing ENet!\n");
        }
        else {
          m_pGlobal->setGlobal("enet_initialized", "true");
        }
      }
    }

    CNetBase::~CNetBase() {
      if (m_pGlobal->getGlobal("enet_initialized") != "") {
        enet_deinitialize();
      }
    }

    /**
    * The implementing object must implement this method. It is called when the thread start working
    */
    void CNetBase::execute() {
      printf("Network thread running...\n");

      messages::CMessageFactory l_cFactory;

      std::chrono::steady_clock::time_point l_cNextStep = std::chrono::steady_clock::now();

      bool l_bSendStep = false;

      while (!m_bStopThread && m_pHost != nullptr) {
        messages::IMessage *l_pMsg = m_pInputQueue->popMessage();

        do {
          if (l_pMsg != nullptr) {
            messages::enMessageIDs l_eMsg = l_pMsg->getMessageId();

            if (l_eMsg == messages::enMessageIDs::StepMsg) {
              messages::CStepMsg *p = reinterpret_cast<messages::CStepMsg *>(l_pMsg);
              l_bSendStep = p->getStepNo() % 4 == 0;
            }
            else l_bSendStep = true;

            // The most frequent messages are sent using a non-reliable packet
            if (l_eMsg == messages::enMessageIDs::StepMsg || l_eMsg == messages::enMessageIDs::MarbleMoved || l_eMsg == messages::enMessageIDs::ObjectMoved) {
              if (l_bSendStep) {
                broadcastMessage(l_pMsg, false);
              }
            }
            else {
              broadcastMessage(l_pMsg, true);
            }

            delete l_pMsg;
          }

          l_pMsg = m_pInputQueue->popMessage();
        }
        while (l_pMsg != nullptr);

        // Do ENet host stuff here
        ENetEvent l_cEvent;
        while (enet_host_service(m_pHost, &l_cEvent, 0) != 0) {
          if (!OnEnetEvent(&l_cEvent)) {
            switch (l_cEvent.type) {
              case ENET_EVENT_TYPE_CONNECT:
                break;

              case ENET_EVENT_TYPE_DISCONNECT:
                break;

              case ENET_EVENT_TYPE_RECEIVE: {
                std::string l_sData;
                l_sData.resize(l_cEvent.packet->dataLength + 1, 0);
                memcpy(l_sData.data(), l_cEvent.packet->data, l_cEvent.packet->dataLength);

                messages::CSerializer64 l_cSerializer = messages::CSerializer64(l_sData.c_str());
                messages::IMessage *l_pMsg = l_cFactory.createMessage(&l_cSerializer);

                if (l_pMsg != nullptr) {
                  if (!onMessageReceived(l_cEvent.peer, l_pMsg)) {
                    m_pOutputQueue->postMessage(l_pMsg);
                  }
                  delete l_pMsg;
                }
                enet_packet_destroy(l_cEvent.packet);
                break;
              }

              case ENET_EVENT_TYPE_NONE:
                break;
            }
          }
        }

        l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(5);
        std::this_thread::sleep_until(l_cNextStep);
      }

      printf("Network thread stopped.\n");
    }

    /**
    * Handle a received message in a subclass
    * @param a_pPeer the peer from which the message was received
    * @param a_pMessage the message to handle
    * @return true if the message was handled
    */
    bool CNetBase::onMessageReceived(ENetPeer* a_pPeer, messages::IMessage* a_pMessage) {
      return false;
    }


    /**
    * Broadcast a message
    * @param a_pMessage the message to broadcast
    * @param a_bReliable should the message be sent reliable?
    */
    void CNetBase::broadcastMessage(messages::IMessage* a_pMessage, bool a_bReliable) {
      messages::CSerializer64 l_cSerializer;
      a_pMessage->serialize(&l_cSerializer);
      std::string l_sMsg = l_cSerializer.getMessageAsString();

      ENetPacket *p = enet_packet_create(l_sMsg.c_str(), l_sMsg.size() + 1, a_bReliable ? ENET_PACKET_FLAG_RELIABLE : 0);
      enet_host_broadcast(m_pHost, a_bReliable ? 0 : 1, p);

      enet_host_flush(m_pHost);
    }

    /**
    * Send a message to one specific peer
    * @param a_pPeer the peer to send to
    * @param a_pMessage the message to send
    */
    void CNetBase::sendMessage(ENetPeer* a_pPeer, messages::IMessage* a_pMessage) {
      messages::CSerializer64 l_cSerializer;
      a_pMessage->serialize(&l_cSerializer);
      std::string l_sMsg = l_cSerializer.getMessageAsString();

      ENetPacket *p = enet_packet_create(l_sMsg.c_str(), l_sMsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(a_pPeer, 0, p);

      enet_host_flush(m_pHost);
    }
  }
}
