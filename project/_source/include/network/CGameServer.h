// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/CMessageQueue.h>
#include <network/CNetBase.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet.h>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Forward declaration of the global class */

  namespace network {
    /**
    * @class CGameServer
    * @author Christian Keimel
    * This class is the game server for network games. It will
    * run as a thread in the background
    */
    class CGameServer : public CNetBase {
      protected:
        bool m_bConnectionAllowed;  /**< If the server open for new connections? Will be set to "false" once the gaming starts */
        int  m_iOpenPlayerSlots;    /**< The number of players that can still connect */

        std::vector<ENetPeer *> m_vPeers;

        /**
        * Handle an event in a subclass
        * @return "true" if the event was handled
        */
        virtual bool OnEvent(ENetEvent *a_cEvent);

      public:
        CGameServer(int a_iOpenSlots, CGlobal *a_pGlobal);
        ~CGameServer();

        void setConnectionAllowed(bool a_bAllowed);
    };
  }
}
