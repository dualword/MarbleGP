// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/IThread.h>
#include <enet/enet.h>
#include <string>
#include <vector>
#include <tuple>

#ifdef _ANDROID
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

namespace dustbin {
  namespace network {
    /**
    * @class CDiscoveryClient
    * @author Christian Keimel
    * This thread broadcasts messages to the network every second
    * in search of servers and waits for a response
    */
    class CDiscoveryClient : public threads::IThread {
      private:
        ENetSocket  m_cScanner;   /**< The socket used to broadcast search messages */

        struct addrinfo *m_cMulticastAddr;

        std::vector<std::tuple<int, int>> m_vServersFound;  /**< List of the found servers */

        ENetSocket createMulticastClientSocket(struct addrinfo **a_pMulticastAddr);

      protected:
        /**
        * The implementing object must implement this method. It is called when the thread start working
        */
        virtual void execute();

      public:
        CDiscoveryClient();

        ~CDiscoveryClient();
    };
  }
}
