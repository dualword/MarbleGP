// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <network/CDiscoveryClient.h>
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
      return -1;
    }

    void CDiscoveryClient::execute() {
      printf("Server discovery thread running.\n");
      if (m_cScanner != ENET_SOCKET_NULL) {
        // We create the message once and just send it over and over again until we stop the thread
        messages::CDiscoverServerRequest l_cDiscovery = messages::CDiscoverServerRequest("MarbleGP");
        messages::CSerializer64 l_cSerializer;

        l_cDiscovery.serialize(&l_cSerializer);

        ENetAddress l_cAddress{};
        l_cAddress.host = ENET_HOST_ANY;
        l_cAddress.port = 4694;

        while (!m_bStopThread) {
          int l_iSent = sendto(m_cScanner, (const char *)l_cSerializer.getBuffer(), l_cSerializer.getBufferSize(), 0, m_pMulticastAddr->ai_addr, (int)m_pMulticastAddr->ai_addrlen);
          if (l_iSent != l_cSerializer.getBufferSize())
            printf("sendto() sent a different number of bytes than expected (%i / %i)\n", l_iSent, l_cSerializer.getBufferSize());


          std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }
      }
      printf("Server discovery thread stopped.\n");
    }

    CDiscoveryClient::CDiscoveryClient() : IThread() {
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
