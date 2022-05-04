// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <messages/CSerializer64.h>
#include <network/CGameServer.h>
#include <CGlobal.h>
#include <chrono>


namespace dustbin {
  namespace network {
    CGameServer::CGameServer(const std::vector<int> &a_vAvailableIDs, CGlobal* a_pGlobal) :
      CNetBase            (a_pGlobal),
      m_bConnectionAllowed(true)
    {
      m_vAvailableSlots = a_vAvailableIDs;

      if (m_pGlobal->getGlobal("enet_initialized") == "true") {
        m_cAddress.host = ENET_HOST_ANY;
        m_cAddress.port = 4693;

        m_pHost = enet_host_create(&m_cAddress, 16, 2, 0, 0);

        if (m_pHost == nullptr) {
          printf("Starting server failed.\n");
        }

        m_pGlobal->setGlobal("enet_initialized", "true");
      }

      m_cPlayers.deserialize(m_pGlobal->getGlobal("raceplayers"));
    }

    CGameServer::~CGameServer() {
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

    void CGameServer::setConnectionAllowed(bool a_bAllowed) {
      m_bConnectionAllowed = a_bAllowed;
    }


    /**
    * Handle an event in a subclass
    * @return "true" if the event was handled
    */
    bool CGameServer::OnEnetEvent(ENetEvent* a_cEvent) {
      switch (a_cEvent->type) {
        case ENET_EVENT_TYPE_CONNECT:
          if (m_bConnectionAllowed && m_vPeers.size() < 16) {
            printf("Client Connected.\n");
            m_vPeers.push_back(a_cEvent->peer);

            messages::CServerIdentifier l_cMsg = messages::CServerIdentifier("MarbleGP Server", (irr::s32)m_vAvailableSlots.size());
            sendMessage(a_cEvent->peer, &l_cMsg);
          }
          else {
            printf("Sorry, we are full.\n");
            enet_peer_disconnect_now(a_cEvent->peer, 0);
          }
          return true;

        case ENET_EVENT_TYPE_DISCONNECT:
        case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
          printf("Peer disconnected.\n");
          if (m_mPlayerMap.find(a_cEvent->peer) != m_mPlayerMap.end()) {
            for (std::vector<int>::iterator it = m_mPlayerMap[a_cEvent->peer].begin(); it != m_mPlayerMap[a_cEvent->peer].end(); it++) {
              printf("Remove player #%i\n", *it);
              messages::CPlayerRemoved l_cRemove = messages::CPlayerRemoved(*it);
              broadcastMessage(&l_cRemove, true);
              m_pOutputQueue->postMessage(&l_cRemove);
            }
            m_mPlayerMap.erase(a_cEvent->peer);
          }

          for (std::vector<ENetPeer*>::iterator it = m_vPeers.begin(); it != m_vPeers.end(); it++) {
            if ((*it) == a_cEvent->peer) {
              printf("Remove peer from list.\n");
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
    bool CGameServer::onMessageReceived(ENetPeer* a_pPeer, messages::IMessage* a_pMessage) {
      // do here: handle all the messages of a newly connecting client
      if (a_pMessage != nullptr) {
        switch (a_pMessage->getMessageId()) {
          case messages::enMessageIDs::ClientRequest: {
            messages::CClientRequest *p = reinterpret_cast<messages::CClientRequest *>(a_pMessage);

            if (p->getplayerslots() <= m_vAvailableSlots.size()) {
              printf("Client has requested %i of %i slots.\n", p->getplayerslots(), (int)m_vAvailableSlots.size());
              m_mAssignedSlots[a_pPeer] = std::vector<std::tuple<int, bool>>();

              while (m_mAssignedSlots[a_pPeer].size() < p->getplayerslots()) {
                m_mAssignedSlots[a_pPeer].push_back(std::make_tuple(*m_vAvailableSlots.begin(), false));
                printf("%5i", std::get<0>(m_mAssignedSlots[a_pPeer].back()));
                m_vAvailableSlots.erase(m_vAvailableSlots.begin());
              }

              printf("\n%i slots left.\n", (int)m_vAvailableSlots.size());

              messages::CClientApproval l_cMsg = messages::CClientApproval((irr::s32)m_mAssignedSlots[a_pPeer].size());
              sendMessage(a_pPeer, &l_cMsg);
            }

            return true;
          }

          case messages::enMessageIDs::RegisterPlayer: {
            messages::CRegisterPlayer *p = reinterpret_cast<messages::CRegisterPlayer *>(a_pMessage);
            if (m_mAssignedSlots.find(a_pPeer) != m_mAssignedSlots.end()) {
              for (std::vector<std::tuple<int, bool>>::iterator it = m_mAssignedSlots[a_pPeer].begin(); it != m_mAssignedSlots[a_pPeer].end(); it++) {
                if (std::get<1>(*it) == false) {
                  std::get<1>(*it) = true;
                  printf("Slot %i assigned to player \"%s\"\n", std::get<0>(*it), p->getname().c_str());

                  int l_iNextPlayerId = 0;
                  for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
                    if (l_iNextPlayerId <= (*it).m_iPlayerId)
                      l_iNextPlayerId = (*it).m_iPlayerId + 1;
                  }

                  data::SPlayerData l_cPlayer;

                  l_cPlayer.m_sName     = p->getname   ();
                  l_cPlayer.m_sTexture  = p->gettexture();
                  l_cPlayer.m_iPlayerId = l_iNextPlayerId;

                  m_cPlayers.m_vPlayers.push_back(l_cPlayer);

                  if (m_mPlayerMap.find(a_pPeer) == m_mPlayerMap.end()) {
                    m_mPlayerMap[a_pPeer] = std::vector<int>();
                  }

                  m_mPlayerMap[a_pPeer].push_back(l_iNextPlayerId);

                  messages::CRegisterPlayer l_cNewMsg = messages::CRegisterPlayer(p->getname(), p->gettexture(), l_iNextPlayerId);

                  // ToDo: Maybe create a new message with further information
                  m_pOutputQueue->postMessage(&l_cNewMsg);

                  messages::CUpdatePlayerId l_cReply = messages::CUpdatePlayerId(p->getident(), l_iNextPlayerId);
                  sendMessage(a_pPeer, &l_cReply);

                  return true;
                }
              }
            }
            else printf("No slots assigned to peer.\n");

            return true;
          }

          case messages::enMessageIDs::PlayerListEnd: {
            for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
              messages::CRacePlayer l_cPlayer = messages::CRacePlayer((*it).m_iPlayerId, (*it).m_sName, (*it).m_sTexture);
              broadcastMessage(&l_cPlayer, true);
            }

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
