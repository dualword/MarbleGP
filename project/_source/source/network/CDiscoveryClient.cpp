// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <network/CDiscoveryClient.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <network/CPorts.h>
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
          messages::CDiscoverServerRequest l_cDiscovery = messages::CDiscoverServerRequest("MarbleGP");
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
          printf("%i Bytes sent.\n", l_iSent);

          std::this_thread::sleep_for(std::chrono::milliseconds(1500));
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

      m_cScanner = createMulticastClientSocket(&m_pMulticastAddr);
    }

    CDiscoveryClient::~CDiscoveryClient() {
#ifdef WIN32
      enet_socket_shutdown(m_cScanner, ENET_SOCKET_SHUTDOWN_READ_WRITE);
#endif
      enet_socket_destroy(m_cScanner);
    }
  }
}
