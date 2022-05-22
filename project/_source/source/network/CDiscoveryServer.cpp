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
      SOCKET l_pSocket;
      struct addrinfo   l_cHints  = { 0 };    /* Hints for name lookup */
      struct addrinfo*  l_cLocalAddr = 0;         /* Local address to bind to */
      int yes=1;

      char l_cMulticastIP[] = "ff02::1";
      char l_cMulticastPort[] = "4694";

#ifdef WIN32
      WSADATA trash;
      if(WSAStartup(MAKEWORD(2,0),&trash) != 0)
        return -1;
#endif


      /* Resolve the multicast group address */
      l_cHints.ai_family = PF_UNSPEC;
      l_cHints.ai_flags  = AI_NUMERICHOST;
      int status;
      if ((status = getaddrinfo(l_cMulticastIP, NULL, &l_cHints, a_pMulticastAddr)) != 0) {
        // fprintf(stderr, "getaddrinfo: %s\n", helpers::ws2s(gai_strerror(status)).c_str());
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
      }


      /* 
      Get a local address with the same family (IPv4 or IPv6) as our multicast group
      This is for receiving on a certain port.
      */
      l_cHints.ai_family   = (*a_pMulticastAddr)->ai_family;
      l_cHints.ai_socktype = SOCK_DGRAM;
      l_cHints.ai_flags    = AI_PASSIVE; /* Return an address we can bind to */
      if ((status = getaddrinfo(NULL, l_cMulticastPort, &l_cHints, &l_cLocalAddr)) != 0 ) {
        // fprintf(stderr, "getaddrinfo: %s\n", helpers::ws2s(gai_strerror(status)).c_str());
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
      }


      /* Create socket for receiving datagrams */
      if ((l_pSocket = socket(l_cLocalAddr->ai_family, l_cLocalAddr->ai_socktype, 0)) < 0) {
        perror("socket() failed");
        return -1;
      }



      /*
      * Enable SO_REUSEADDR to allow multiple instances of this
      * application to receive copies of the multicast datagrams.
      */
      if (setsockopt(l_pSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int)) == -1) {
        perror("setsockopt");
        return -1;
      }

      /* Bind the local address to the multicast port */
      if ( bind(l_pSocket, l_cLocalAddr->ai_addr, (int)l_cLocalAddr->ai_addrlen) != 0 ) {
        perror("bind() failed");
        return -1;
      }

      /* get/set socket receive buffer */
      int l_iOptval=0;
      socklen_t l_iOptval_len = sizeof(l_iOptval);
      int l_iDfltrcvbuf;
      if(getsockopt(l_pSocket, SOL_SOCKET, SO_RCVBUF,(char*)&l_iOptval, &l_iOptval_len) !=0) {
        perror("getsockopt");
        return -1;
      }

      l_iDfltrcvbuf = l_iOptval;
      l_iOptval = 4096;

      if(setsockopt(l_pSocket,SOL_SOCKET,SO_RCVBUF,(char*)&l_iOptval,sizeof(l_iOptval)) != 0) {
        perror("setsockopt");
        return -1;
      }
      if(getsockopt(l_pSocket, SOL_SOCKET, SO_RCVBUF,(char*)&l_iOptval, &l_iOptval_len) != 0) {
        perror("getsockopt");
        return -1;
      }
      printf("tried to set socket receive buffer from %d to %d, got %d\n",
        l_iDfltrcvbuf, 4096, l_iOptval);

      /* Join the multicast group. We do this seperately depending on whether we
      * are using IPv4 or IPv6. 
      */
      if ( (*a_pMulticastAddr)->ai_family  == PF_INET &&  
        (*a_pMulticastAddr)->ai_addrlen == sizeof(struct sockaddr_in)) /* IPv4 */
      {
        struct ip_mreq l_cMulticastRequest {};  /* Multicast address join structure */

                                          /* Specify the multicast group */
        memcpy(&l_cMulticastRequest.imr_multiaddr,
          &((struct sockaddr_in*)((*a_pMulticastAddr)->ai_addr))->sin_addr,
          sizeof(l_cMulticastRequest.imr_multiaddr));

        /* Accept multicast from any interface */
        l_cMulticastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

        /* Join the multicast address */
        if ( setsockopt(l_pSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &l_cMulticastRequest, sizeof(l_cMulticastRequest)) != 0 ) {
          perror("setsockopt() failed");
          return -1;
        }
      }
      else if ( (*a_pMulticastAddr)->ai_family  == PF_INET6 &&
        (*a_pMulticastAddr)->ai_addrlen == sizeof(struct sockaddr_in6) ) /* IPv6 */
      {
        struct ipv6_mreq l_iMulticastRequest {};  /* Multicast address join structure */

                                            /* Specify the multicast group */
        memcpy(&l_iMulticastRequest.ipv6mr_multiaddr,
          &((struct sockaddr_in6*)((*a_pMulticastAddr)->ai_addr))->sin6_addr,
          sizeof(l_iMulticastRequest.ipv6mr_multiaddr));

        /* Accept multicast from any interface */
        l_iMulticastRequest.ipv6mr_interface = 0;

        /* Join the multicast address */
        if ( setsockopt(l_pSocket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*) &l_iMulticastRequest, sizeof(l_iMulticastRequest)) != 0 ) {
          perror("setsockopt() failed");
          return -1;
        }
      }
      else {
        perror("Neither IPv4 or IPv6"); 
        return -1;
      }



      if(l_cLocalAddr)
        freeaddrinfo(l_cLocalAddr);

      return l_pSocket;
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
      enet_socket_shutdown(m_cListen, ENET_SOCKET_SHUTDOWN_READ_WRITE);
      enet_socket_destroy(m_cListen);
    }
  }
}