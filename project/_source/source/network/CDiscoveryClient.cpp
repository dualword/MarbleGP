// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <network/CDiscoveryClient.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
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
      SOCKET l_pSocket;
      struct addrinfo l_cHints = { 0 };    /* Hints for name lookup */

      char l_sMulticastIP[] = "ff02::1";
      char l_sMulticastPort[] = "4694";

      int l_iMulticastTTL = 2;

#ifdef WIN32
      WSADATA l_cTrash;
      if(WSAStartup(MAKEWORD(2,0),&l_cTrash) != 0)
        return -1;
#endif


      /*
      Resolve destination address for multicast datagrams 
      */
      l_cHints.ai_family   = PF_UNSPEC;
      l_cHints.ai_socktype = SOCK_DGRAM;
      l_cHints.ai_flags    = AI_NUMERICHOST;
      int status;
      if ((status = getaddrinfo(l_sMulticastIP, l_sMulticastPort, &l_cHints, a_pMulticastAddr)) != 0 )
      {
        // fprintf(stderr, "getaddrinfo: %s\n", helpers::ws2s(gai_strerror(status)).c_str());
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
      }



      /* 
      Create socket for sending multicast datagrams 
      */
      if ((l_pSocket = socket((*a_pMulticastAddr)->ai_family, (*a_pMulticastAddr)->ai_socktype, 0)) < 0) {
        perror("socket() failed");
        freeaddrinfo(*a_pMulticastAddr);
        return -1;
      }

      /* 
      Set TTL of multicast packet 
      */
      if (setsockopt(l_pSocket,
        (*a_pMulticastAddr)->ai_family == PF_INET6 ? IPPROTO_IPV6        : IPPROTO_IP,
        (*a_pMulticastAddr)->ai_family == PF_INET6 ? IPV6_MULTICAST_HOPS : IP_MULTICAST_TTL,
        (char*) &l_iMulticastTTL, sizeof(l_iMulticastTTL)) != 0 ) {
        perror("setsockopt() failed");
        freeaddrinfo(*a_pMulticastAddr);
        return -1;
      }


      /* 
      set the sending interface 
      */
      if((*a_pMulticastAddr)->ai_family == PF_INET) {
        in_addr_t iface = INADDR_ANY; /* well, yeah, any */
        if(setsockopt (l_pSocket, 
          IPPROTO_IP,
          IP_MULTICAST_IF,
          (char*)&iface, sizeof(iface)) != 0) { 
          perror("interface setsockopt() sending interface");
          freeaddrinfo(*a_pMulticastAddr);
          return -1;
        }

      }
      if((*a_pMulticastAddr)->ai_family == PF_INET6) {
        unsigned int ifindex = 0; /* 0 means 'default interface'*/
        if(setsockopt (l_pSocket, 
          IPPROTO_IPV6,
          IPV6_MULTICAST_IF,
          (char*)&ifindex, sizeof(ifindex)) != 0) { 
          perror("interface setsockopt() sending interface");
          freeaddrinfo(*a_pMulticastAddr);
          return -1;
        }   

      }

      return l_pSocket;
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
      enet_socket_shutdown(m_cScanner, ENET_SOCKET_SHUTDOWN_READ_WRITE);
      enet_socket_destroy(m_cScanner);
    }
  }
}
