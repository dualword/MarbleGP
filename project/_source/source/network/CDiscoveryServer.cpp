// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <network/CDiscoverySever.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>

#ifndef SOCKET
#define SOCKET int
#endif

namespace dustbin {
  namespace network {
    ENetSocket CDiscoveryServer::createMulticastServerSocket(struct addrinfo **a_pMulticastAddr) {
      return -1;
    }

    void CDiscoveryServer::execute() {
      printf("Server discovery thread running.\n");

      char recvBuf[0xFF];

      while (!m_bStopThread) {
        sockaddr l_cFrom {};
        int l_iFrom = (int)sizeof(l_cFrom);

        memset(recvBuf, 0, 0xFF);
        int l_iRead = recvfrom(m_cListen, recvBuf, 255, 0, nullptr, 0);

        printf("%i Bytes read: %s\n", l_iRead, recvBuf);

        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
      }
      printf("Server discovery thread stopped.\n");
    }

    CDiscoveryServer::CDiscoveryServer(const std::string& a_sServer, int a_iPortNo) : IThread(), m_sServer(a_sServer), m_iPortNo(a_iPortNo), m_MulticastAddr(nullptr) {
      m_cListen = createMulticastServerSocket(&m_MulticastAddr);
    }

    CDiscoveryServer::~CDiscoveryServer() {
#ifdef WIN32
      enet_socket_shutdown(m_cListen, ENET_SOCKET_SHUTDOWN_READ_WRITE);
#endif
      enet_socket_destroy(m_cListen);
    }
  }
}