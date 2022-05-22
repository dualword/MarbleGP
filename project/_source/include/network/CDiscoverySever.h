// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/IThread.h>
#include <enet.h>
#include <string>

namespace dustbin {
  namespace network {
    class CDiscoveryServer : public threads::IThread {
    private:
      ENetSocket  m_cListen;  /**< The receiving socket */
      std::string m_sServer;  /**< Hostname of the server */
      int         m_iPortNo;  /**< Port the server is listening to */

      struct addrinfo *m_MulticastAddr;

      ENetSocket createMulticastServerSocket(struct addrinfo **a_pMulticastAddr);

    protected:
      /**
      * The implementing object must implement this method. It is called when the thread start working
      */
      virtual void execute();

    public:
      CDiscoveryServer(const std::string &a_sServer, int a_iPortNo);

      ~CDiscoveryServer();
    };  
  }
}