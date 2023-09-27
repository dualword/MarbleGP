// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <messages/CSerializer64.h>
#include <network/CNetBase.h>
#include <CGlobal.h>
#include <chrono>


namespace dustbin {
  namespace network {
    const int c_iSendInterval = 30;

    CNetBase::CNetBase(CGlobal* a_pGlobal) :
      m_pGlobal   (a_pGlobal),
      m_pHost     (nullptr),
      m_bSendMoved(true)
    {
      if (m_pGlobal->getGlobal("enet_initialized") != "true") {
        if (enet_initialize() != 0) {
          printf("Error while initializing ENet!\n");
        }
        else {
          m_pGlobal->setGlobal("enet_initialized", "true");
        }
      }

      resetMyMarbles();
    }

    // int blub = 0;

    CNetBase::~CNetBase() {
      if (m_pGlobal->getGlobal("enet_initialized") != "") {
        enet_deinitialize();
        m_pGlobal->setGlobal("enet_initialized", "");
      }

      // blub = 0;
    }

    /**
    * The implementing object must implement this method. It is called when the thread starts working
    */
    void CNetBase::execute() {
      printf("Network thread running...\n");

      messages::CMessageFactory l_cFactory;

      while (!m_bStopThread && m_pHost != nullptr) {
        messages::IMessage *l_pMsg = m_pInputQueue->popMessage();

        do {
          if (l_pMsg != nullptr) {
            beforeSendMessage(l_pMsg);

            messages::enMessageIDs l_eMsg = l_pMsg->getMessageId();

            switch (l_eMsg) {
              case messages::enMessageIDs::StepMsg: {
                messages::CStepMsg *p = reinterpret_cast<messages::CStepMsg *>(l_pMsg);
                m_iStep = p->m_StepNo;
                m_bSendMoved = (m_iStep % c_iSendInterval) == 0;
                break;
              }

              case messages::enMessageIDs::ObjectMoved:
                if (m_bSendMoved) {
                  broadcastMessage(l_pMsg, true);
                }
                break;

              case messages::enMessageIDs::Checkpoint: 
              case messages::enMessageIDs::PlayerRespawn:
              case messages::enMessageIDs::CameraRespawn: 
              case messages::enMessageIDs::PlayerStunned: 
              case messages::enMessageIDs::LapStart: 
              case messages::enMessageIDs::PlayerFinished:
              case messages::enMessageIDs::PlayerRostrum:
              case messages::enMessageIDs::PlayerWithdraw: 
              case messages::enMessageIDs::PlayerWithdrawn: 
              case messages::enMessageIDs::ConfirmWithdraw: {
                int l_iIndex = getMarbleId(l_pMsg) - 10000;

                if (l_iIndex >= 0 && l_iIndex < 16 && std::get<0>(m_aMyMarbles[l_iIndex]) != -1) {
                  broadcastMessage(l_pMsg, true);
                }
                break;
              }

              case messages::enMessageIDs::MarbleMoved: {
                messages::CMarbleMoved *l_pMove = reinterpret_cast<messages::CMarbleMoved *>(l_pMsg);
                int l_iIndex = l_pMove->m_ObjectId - 10000;

                if (l_iIndex >= 0 && l_iIndex < 16) {
                  int l_iLast = std::get<0>(m_aMyMarbles[l_iIndex]);
                  if (marbleMessageNecessary(l_iIndex, l_pMove)) {
                    // printf("My Marble: send update (%i, %.2f | %i)\n", l_iIndex, float(std::get<0>(m_aMyMarbles[l_iIndex]) - l_iLast) / 120.0f, std::get<0>(m_aMyMarbles[l_iIndex]) - l_iLast);
                    broadcastMessage(l_pMsg, true);
                  }
                }
                break;
              }

              default:
                broadcastMessage(l_pMsg, true);
                break;
            }

            delete l_pMsg;
          }

          l_pMsg = m_pInputQueue->popMessage();
        }
        while (l_pMsg != nullptr);

        bool l_bEvent = false;

        // Do ENet host stuff here
        ENetEvent l_cEvent;
        int l_iEvents = enet_host_service(m_pHost, &l_cEvent, 5);

        while (l_iEvents > 0) {
          l_bEvent = true;

          if (!OnEnetEvent(&l_cEvent)) {
            switch (l_cEvent.type) {
              case ENET_EVENT_TYPE_CONNECT:
                break;

              case ENET_EVENT_TYPE_DISCONNECT:
                printf("Disconnected.\n");
                break;

              case ENET_EVENT_TYPE_RECEIVE: {
                messages::CSerializer64 l_cSerializer = messages::CSerializer64(l_cEvent.packet->data); //  l_sData.c_str());
                messages::IMessage *l_pMsg = l_cFactory.createMessage(&l_cSerializer);

                if (l_pMsg != nullptr) {
                  if (!onMessageReceived(l_cEvent.peer, l_pMsg)) {
                    int l_iMarble = getMarbleId(l_pMsg);

                    // if (l_pMsg->getMessageId() == messages::enMessageIDs::MarbleMoved && blub % 10 == 0) {
                      // printf("Got update for Marble %i | ", reinterpret_cast<messages::CMarbleMoved *>(l_pMsg)->m_ObjectId);

                      // for (int i = 0; i < 16; i++) {
                      //   printf("%5i ", std::get<0>(m_aMyMarbles[i]));
                      // }
                      // printf("\n");
                      // }

                      // blub++;

                    if (l_iMarble == -1 || std::get<0>(m_aMyMarbles[l_iMarble - 10000]) == -1)
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
          l_iEvents = enet_host_check_events(m_pHost, &l_cEvent);
        }

        if (l_iEvents < 0) {
          printf("Error on enet_host_service: %i\n", l_iEvents);
        }

        // if (!l_bEvent) {
        //   l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(1);
        //   std::this_thread::sleep_until(l_cNextStep);
        // }
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

      ENetPacket *p = enet_packet_create(l_sMsg.c_str(), l_sMsg.size() + 1, a_bReliable ? ENET_PACKET_FLAG_RELIABLE : ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
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

    /**
    * Mark a marble as a marble of mine
    * @param a_iMarbleId scene node ID of the marble (10000 .. 10016)
    */
    void CNetBase::setMyMarble(int a_iMarbleId) {
      int l_iIndex = a_iMarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16)
        std::get<0>(m_aMyMarbles[l_iIndex]) = 0;
    }

    /**
    * Reset the my marbles array after the game
    */
    void CNetBase::resetMyMarbles() {
      // printf("resetMyMarbles: ");
      for (int i = 0; i < 16; i++)
        std::get<0>(m_aMyMarbles[i]) = -1;

      // for (int i = 0; i < 16; i++)
      //   printf("%5i ", std::get<0>(m_aMyMarbles[i]));
      // printf("\n");
    }

    /**
    * Shall a marble moved message be sent?
    * @param a_iIndex index of the marble
    * @param a_pMsg the new marble moved message
    * @return true if the controls have changed or the last send message is more than c_iSendInterval steps in the past
    */
    bool CNetBase::marbleMessageNecessary(int a_iIndex, messages::CMarbleMoved* a_pMsg) {
      bool l_bRet = false;

      if (std::get<0>(m_aMyMarbles[a_iIndex]) == -1) {
        // Not my marble, the server just forwards the messages to all clients on receiving
      }
      if (std::get<0>(m_aMyMarbles[a_iIndex]) >= 0) {
        if (m_iStep - std::get<0>(m_aMyMarbles[a_iIndex]) > c_iSendInterval) {
          l_bRet = true;
        }
        else {
          messages::CMarbleMoved l_cOld = messages::CMarbleMoved(std::get<1>(m_aMyMarbles[a_iIndex]));

          if (a_pMsg->m_ControlX != l_cOld.m_ControlX || 
              a_pMsg->m_ControlY != l_cOld.m_ControlY ||
              a_pMsg->getControlBrake() != l_cOld.getControlBrake()
            ) 
            l_bRet = true;
        }
      }

      if (l_bRet) {
        std::get<0>(m_aMyMarbles[a_iIndex]) = m_iStep;
        std::get<1>(m_aMyMarbles[a_iIndex]) = messages::CMarbleMoved(a_pMsg);
      }

      return l_bRet;
    }

    /**
    * Get the Marble ID (10000 .. 10016) from a message (if applicable)
    * @param a_pMsg the message to check
    * @return ID of the marble of the message, -1 if message contains no marble information
    */
    int CNetBase::getMarbleId(messages::IMessage* a_pMsg) {
      switch (a_pMsg->getMessageId()) {
        case messages::enMessageIDs::Checkpoint     : return reinterpret_cast<messages::CCheckpoint      *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::PlayerRespawn  : return reinterpret_cast<messages::CPlayerRespawn   *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::CameraRespawn  : return reinterpret_cast<messages::CCameraRespawn   *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::PlayerStunned  : return reinterpret_cast<messages::CPlayerStunned   *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::MarbleMoved    : return reinterpret_cast<messages::CMarbleMoved     *>(a_pMsg)->m_ObjectId; break;
        case messages::enMessageIDs::LapStart       : return reinterpret_cast<messages::CLapStart        *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::PlayerFinished : return reinterpret_cast<messages::CPlayerFinished  *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::RacePosition   : return reinterpret_cast<messages::CRacePosition    *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::PlayerWithdraw : return reinterpret_cast<messages::CPlayerWithdraw  *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::PlayerWithdrawn: return reinterpret_cast<messages::CPlayerWithdrawn *>(a_pMsg)->m_MarbleId; break;
        case messages::enMessageIDs::ConfirmWithdraw: return reinterpret_cast<messages::CConfirmWithdraw *>(a_pMsg)->m_MarbleId; break;

        default:
          break;
      }
      return -1;
    }
  }
}
