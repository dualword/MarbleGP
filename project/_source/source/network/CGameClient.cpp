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
    bool CGameClient::OnEvent(ENetEvent* a_cEvent) {
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
  }
}
