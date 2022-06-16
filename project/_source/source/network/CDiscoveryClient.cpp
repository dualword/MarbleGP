// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <network/CDiscoveryClient.h>
#include <network/CNetworkDefines.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <enet/enet.h>
#include <CGlobal.h>

#ifdef WIN32
typedef unsigned int in_addr_t;
#endif

#ifndef SOCKET
#define SOCKET int
#endif

namespace dustbin {
  namespace network {
    ENetSocket CDiscoveryClient::createMulticastClientSocket(struct addrinfo **a_pMulticastAddr) {
      ENetSocket l_cScanner = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
      // We need to set a socket option in order to send to the broadcast address
      enet_socket_set_option(l_cScanner, ENET_SOCKOPT_BROADCAST, 1);
      return l_cScanner;
    }

    void CDiscoveryClient::execute() {
      printf("Server discovery thread running.\n");
      if (m_cScanner != ENET_SOCKET_NULL) {
        std::string l_sMessage = "";

        {
        // We create the message once and just send it over and over again until we stop the thread
          messages::CDiscoverServerRequest l_cDiscovery = messages::CDiscoverServerRequest(c_sTheNetGame);
          messages::CSerializer64 l_cSerializer;
          l_cDiscovery.serialize(&l_cSerializer);
          l_sMessage = l_cSerializer.getMessageAsString();
        }


        ENetAddress l_cAddress{};
        l_cAddress.host = 0xFFFFFFFF; // ENET_HOST_BROADCAST;
        l_cAddress.port = c_iScanPort;

        while (!m_bStopThread) {
          ENetBuffer sendbuf{};
          sendbuf.data = (void *)l_sMessage.data();
          sendbuf.dataLength = l_sMessage.length();
          int l_iSent = enet_socket_send(m_cScanner, &l_cAddress, &sendbuf, 1);

          int l_iSelect = 0;

          do {
            ENetSocketSet set{};
            ENET_SOCKETSET_EMPTY(set);
            ENET_SOCKETSET_ADD(set, m_cScanner);
            l_iSelect = enet_socketset_select(m_cScanner, &set, NULL, 0);

            if (l_iSelect > 0) {
              char l_aBuffer[1024]{};
              ENetAddress l_cAddr;
              ENetBuffer l_cRecvBuf{};
              l_cRecvBuf.data = l_aBuffer;
              l_cRecvBuf.dataLength = 1024;
              int l_iRead = enet_socket_receive(m_cScanner, &l_cAddr, &l_cRecvBuf, 1);

              enet_uint32 l_iServer = l_cAddr.host;
              std::string l_sServerIP = std::to_string(l_iServer & 0xFF) + "." + std::to_string((l_iServer & 0xFF00) / 256) + "." + std::to_string((l_iServer & 0xFF0000) / 65536) + "." + std::to_string((l_iServer & 0xFF000000) / 16777216);
              printf("%i Bytes read from %s (%s)\n", l_iRead, l_sServerIP.c_str(), l_aBuffer);

              if (l_iRead > 0) {
                messages::CMessageFactory l_cFactory;
                messages::CSerializer64 l_cSerializer = messages::CSerializer64(l_aBuffer);
                messages::IMessage *l_pMsg = l_cFactory.createMessage(&l_cSerializer);
                if (l_pMsg != nullptr) {
                  if (l_pMsg->getMessageId() == messages::enMessageIDs::DiscoverServerResponse) {
                    messages::CDiscoverServerResponse *p = reinterpret_cast<messages::CDiscoverServerResponse *>(l_pMsg);

                    if (std::find(m_vServersFound.begin(), m_vServersFound.end(), std::make_tuple(l_iServer, p->getport())) == m_vServersFound.end()) {
                      printf("Game Server Found: %s:%i\n", p->getserver().c_str(), p->getport());
                      m_vServersFound.push_back(std::make_tuple(l_iServer, p->getport()));

                      if (p->getserver() != "0.0.0.0")
                        l_sServerIP = p->getserver();

                      messages::CGameServerFound l_cMsg = messages::CGameServerFound(l_iServer, p->getport(), l_sServerIP);
                      m_pOutputQueue->postMessage(&l_cMsg);
                    }
                  }
                  delete l_pMsg;
                }
              }
            }
          }
          while (l_iSelect > 0);

          messages::CDiscoveryStep l_cDiscovery;
          m_pOutputQueue->postMessage(&l_cDiscovery);

          std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
      }
      printf("Server discovery thread stopped.\n");
    }

    CDiscoveryClient::CDiscoveryClient() : IThread(), m_cScanner(ENET_SOCKET_NULL) {
      if (CGlobal::getInstance()->getGlobal("enet_initialized") != "true") {
        if (enet_initialize() != 0) {
          printf("Error while initializing ENet!\n");
        }
        else {
          CGlobal::getInstance()->setGlobal("enet_initialized", "true");
        }
      }

      m_cScanner = createMulticastClientSocket(&m_cMulticastAddr);
    }

    CDiscoveryClient::~CDiscoveryClient() {
#ifdef WIN32
      enet_socket_shutdown(m_cScanner, ENET_SOCKET_SHUTDOWN_READ_WRITE);
#endif
      enet_socket_destroy(m_cScanner);
    }
  }
}
