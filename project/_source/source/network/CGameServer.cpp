// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <messages/CSerializer64.h>
#include <network/CGameServer.h>
#include <CGlobal.h>
#include <chrono>


namespace dustbin {
  namespace network {
    CGameServer::CGameServer(int a_iOpenSlots, CGlobal* a_pGlobal) :
      CNetBase            (a_pGlobal),
      m_iOpenPlayerSlots  (a_iOpenSlots),
      m_bConnectionAllowed(true)
    {
      if (m_pGlobal->getGlobal("enet_initialized") == "true") {
        m_cAddress.host = ENET_HOST_ANY;
        m_cAddress.port = 4693;

        m_pHost = enet_host_create(&m_cAddress, 16, 2, 0, 0);

        if (m_pHost == nullptr) {
          printf("Starting server failed.\n");
        }

        m_pGlobal->setGlobal("enet_initialized", "true");
      }
    }

    CGameServer::~CGameServer() {
      if (m_pHost != nullptr) {
        enet_host_destroy(m_pHost);
        m_pHost = nullptr;
      }

      if (m_pGlobal->getGlobal("enet_initialized") != "") {
        enet_deinitialize();
      }
    }

    void CGameServer::setConnectionAllowed(bool a_bAllowed) {
      m_bConnectionAllowed = a_bAllowed;
    }


    /**
    * Handle an event in a subclass
    * @return "true" if the event was handled
    */
    bool CGameServer::OnEvent(ENetEvent* a_cEvent) {
      switch (a_cEvent->type) {
        case ENET_EVENT_TYPE_CONNECT:
          if (m_bConnectionAllowed && m_vPeers.size() < 16) {
            printf("Client Connected.\n");
            m_vPeers.push_back(a_cEvent->peer);

            messages::CServerIdentifier l_cMsg = messages::CServerIdentifier("MarbleGP Server", m_iOpenPlayerSlots);
            messages::CSerializer64 l_cSerializer = messages::CSerializer64();

            l_cMsg.serialize(&l_cSerializer);
            std::string l_sMsg = l_cSerializer.getMessageAsString();

            ENetPacket *p = enet_packet_create(l_sMsg.c_str(), l_sMsg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(a_cEvent->peer, 0, p);
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

  }
}
