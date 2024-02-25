// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessageFactory.h>
#include <_generated/messages/CMessages.h>
#include <network/CDiscoverySever.h>
#include <network/CNetworkDefines.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>

namespace dustbin {
  namespace network {
#ifdef WIN32
    void printSocketError() {
      printf("Error: ");

      switch (WSAGetLastError()) {
        case WSANOTINITIALISED: printf("WSANOTINITIALISED"); break;
        case WSAENETDOWN      : printf("WSAENETDOWN"      ); break;
        case WSAEFAULT        : printf("WSAEFAULT"        ); break;
        case WSAENOTCONN      : printf("WSAENOTCONN"      ); break;
        case WSAEINTR         : printf("WSAEINTR"         ); break;
        case WSAEINPROGRESS   : printf("WSAEINPROGRESS"   ); break;
        case WSAENETRESET     : printf("WSAENETRESET"     ); break;
        case WSAENOTSOCK      : printf("WSAENOTSOCK"      ); break;
        case WSAEOPNOTSUPP    : printf("WSAEOPNOTSUPP"    ); break;
        case MSG_OOB          : printf("MSG_OOB"          ); break;
        case WSAESHUTDOWN     : printf("WSAESHUTDOWN"     ); break;
        case WSAEWOULDBLOCK   : printf("WSAEWOULDBLOCK"   ); break;
        case WSAEMSGSIZE      : printf("WSAEMSGSIZE"      ); break;
        case WSAEINVAL        : printf("WSAEINVAL"        ); break;
        case WSAECONNABORTED  : printf("WSAECONNABORTED"  ); break;
        case WSAETIMEDOUT     : printf("WSAETIMEDOUT"     ); break;
        case WSAECONNRESET    : printf("WSAECONNRESET"    ); break;
      }

      printf("\n");
    }
#endif

    ENetSocket CDiscoveryServer::createMulticastServerSocket(ENetAddress *a_pMulticastAddr) {
      ENetSocket l_cListen = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
      // Allow the port to be reused by other applications - this means we can run several servers at once
      enet_socket_set_option(l_cListen, ENET_SOCKOPT_REUSEADDR, 1);
      a_pMulticastAddr->host = ENET_HOST_ANY;
      a_pMulticastAddr->port = c_iScanPort;
      enet_socket_bind(l_cListen, a_pMulticastAddr);

      return l_cListen;
    }

    void CDiscoveryServer::execute() {
      printf("Server discovery thread running.\n");

      std::string l_sResponse = "";

      {
        messages::CDiscoverServerResponse l_cMsg = messages::CDiscoverServerResponse(m_iPortNo, m_sServer);
        messages::CSerializer64 l_cSerializer;
        l_cMsg.serialize(&l_cSerializer);
        l_sResponse = l_cSerializer.getMessageAsString();
      }

      printf("Response Message: \"%s\"\n", l_sResponse.c_str());

      while (!m_bStopThread && m_cListen != ENET_SOCKET_NULL) {
        ENetSocketSet set{};
        ENET_SOCKETSET_EMPTY(set);
        ENET_SOCKETSET_ADD(set, m_cListen);
        int l_iSelect = enet_socketset_select(m_cListen, &set, NULL, 0);

        if (l_iSelect > 0) {
          ENetAddress l_cAddr;
          char l_aBuf[1024]{};
          ENetBuffer l_cRecvBuf{};
          l_cRecvBuf.data = l_aBuf;
          l_cRecvBuf.dataLength = 1024;

          int l_iRead = enet_socket_receive(m_cListen, &l_cAddr, &l_cRecvBuf, 1);
          if (l_iRead > 0) {
            messages::CMessageFactory l_cFactory;
            messages::CSerializer64 l_cDeSerializer = messages::CSerializer64(l_aBuf);
            messages::IMessage *l_pMsg = l_cFactory.createMessage(&l_cDeSerializer);

            if (l_pMsg != nullptr) {
              if (l_pMsg->getMessageId() == messages::enMessageIDs::DiscoverServerRequest) {
                messages::CDiscoverServerRequest *p = reinterpret_cast<messages::CDiscoverServerRequest *>(l_pMsg);

                if (p->getgame() == c_sTheNetGame) {
                  printf("Correct Game requested.\n");
                  ENetBuffer replybuf{};
                  replybuf.data = (void *)l_sResponse.data();
                  replybuf.dataLength = l_sResponse.length();
                  int l_iSent = enet_socket_send(m_cListen, &l_cAddr, &replybuf, 1);
#ifdef WIN32
                  if (l_iSent == SOCKET_ERROR) {
                    printSocketError();
                  }
#endif
                }
              }

              delete l_pMsg;
            }
          }
#ifdef WIN32
          else if (l_iRead == SOCKET_ERROR) {
            printSocketError();
          }
#endif
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      printf("Server discovery thread stopped.\n");
    }

    CDiscoveryServer::CDiscoveryServer(const std::string& a_sServer, int a_iPortNo) : IThread(), m_cListen(ENET_SOCKET_NULL), m_sServer(a_sServer), m_iPortNo(a_iPortNo) {
      m_cListen = createMulticastServerSocket(&m_cMulticastAddress);
    }

    CDiscoveryServer::~CDiscoveryServer() {
#ifdef WIN32
      enet_socket_shutdown(m_cListen, ENET_SOCKET_SHUTDOWN_READ_WRITE);
#endif
      enet_socket_destroy(m_cListen);
    }
  }
}