// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <messages/CSerializer64.h>
#include <network/CGameClient.h>
#include <CGlobal.h>
#include <chrono>


namespace dustbin {
  namespace network {
    CGameClient::CGameClient(const std::string &a_sAddress, int a_iPort, CGlobal* a_pGlobal) :
      CNetBase            (a_pGlobal),
      m_bConnectionAllowed(true)
    {
      if (m_pGlobal->getGlobal("enet_initialized") == "true") {
        m_pHost = enet_host_create(nullptr, 1, 2, 0, 0);
        enet_address_set_host(&m_cAddress, a_sAddress.c_str());
        m_cAddress.port = a_iPort;

        ENetPeer *p = enet_host_connect(m_pHost, &m_cAddress, 2, 0);

        if (p == nullptr) {
          printf("Error while connecting to host.\n");
        }
        else {
          m_vPeers.push_back(p);
        }

        m_pGlobal->setGlobal("enet_initialized", "true");
      }
    }

    CGameClient::~CGameClient() {
      for (std::vector<ENetPeer *>::iterator it = m_vPeers.begin(); it != m_vPeers.end(); it++)
        enet_peer_disconnect_now(*it, 0);

      if (m_pHost != nullptr) {
        enet_host_destroy(m_pHost);
        m_pHost = nullptr;
      }

      if (m_pGlobal->getGlobal("enet_initialized") != "") {
        enet_deinitialize();
      }
    }

    void CGameClient::setConnectionAllowed(bool a_bAllowed) {
      m_bConnectionAllowed = a_bAllowed;
    }


    /**
    * Handle an event in a subclass
    * @return "true" if the event was handled
    */
    bool CGameClient::OnEnetEvent(ENetEvent* a_cEvent) {
      switch (a_cEvent->type) {
      case ENET_EVENT_TYPE_CONNECT:
        if (m_bConnectionAllowed && m_vPeers.size() < 16) {
          printf("Client Connected.\n");
          m_vPeers.push_back(a_cEvent->peer);
        }
        else {
          printf("Sorry, we are full.\n");
          enet_peer_disconnect_now(a_cEvent->peer, 0);
        }
        return true;

      case ENET_EVENT_TYPE_DISCONNECT:
      case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        for (std::vector<ENetPeer*>::iterator it = m_vPeers.begin(); it != m_vPeers.end(); it++) {
          if ((*it) == a_cEvent->peer) {
            m_vPeers.erase(it);
            return true;
          }
        }
        break;

      case ENET_EVENT_TYPE_RECEIVE: {
        break;
      }

      case ENET_EVENT_TYPE_NONE:
        break;
      }

      return false;
    }

    /**
    * Handle a received message in a subclass
    * @param a_pPeer the peer from which the message was received
    * @param a_pMessage the message to handle
    * @return true if the message was handled
    */
    bool CGameClient::onMessageReceived(ENetPeer* a_pPeer, messages::IMessage* a_pMessage) {
      // do here: handle all the messages of a newly connecting client
      if (a_pMessage != nullptr) {
        switch (a_pMessage->getMessageId()) {
          case messages::enMessageIDs::ServerIdentifier: {
            messages::CServerIdentifier *p = reinterpret_cast<messages::CServerIdentifier *>(a_pMessage);
            printf("Server identifier: %s (%i free slots)\n", p->getidentify().c_str(), p->getfreeslots());
            if (p->getidentify() == "MarbleGP Server") {
              m_cPlayers.deserialize(m_pGlobal->getGlobal("raceplayers"));

              messages::CClientRequest p = messages::CClientRequest((irr::s32)m_cPlayers.m_vPlayers.size());
              sendMessage(a_pPeer, &p);
            }
            else {
              enet_peer_disconnect(a_pPeer, 0);
            }

            return true;
          }

          case messages::enMessageIDs::ClientApproval: {
            messages::CClientApproval *p = reinterpret_cast<messages::CClientApproval *>(a_pMessage);
            printf("Server has assigned %i slots to me.\n", p->getslotsassigned());

            irr::s32 l_iSlots = p->getslotsassigned();
            std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin();

            while (l_iSlots > 0 && it != m_cPlayers.m_vPlayers.end()) {
              printf("Register player \"%s\"...\n", (*it).m_sName.c_str());

              messages::CRegisterPlayer l_cPlayer = messages::CRegisterPlayer((*it).m_sName, (*it).m_sTexture, (*it).m_iPlayerId);
              sendMessage(a_pPeer, &l_cPlayer);
              it++;
              l_iSlots--;
            }

            messages::CPlayerListEnd l_cReady;
            sendMessage(a_pPeer, &l_cReady);

            return true;
          }

          case messages::enMessageIDs::UpdatePlayerId: {
            messages::CUpdatePlayerId *p = reinterpret_cast<messages::CUpdatePlayerId *>(a_pMessage);

            for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
              if ((*it).m_iPlayerId == p->getoriginal_id()) {
                printf("ID of player %s changed from %i to %i\n", (*it).m_sName.c_str(), p->getoriginal_id(), p->getnetgame_id());
                (*it).m_iPlayerId = p->getnetgame_id();
              }
            }

            m_pOutputQueue->postMessage(a_pMessage);
            return true;
          }

          case messages::enMessageIDs::RacePlayer: {
            messages::CRacePlayer *p = reinterpret_cast<messages::CRacePlayer *>(a_pMessage);

            printf("Race player %i: \"%s\"\n", p->getplayerid(), p->getname().c_str());
            m_pOutputQueue->postMessage(a_pMessage);

            return true;
          }

          default:
            break;
        }
      }
      return false;
    }
  }
}
