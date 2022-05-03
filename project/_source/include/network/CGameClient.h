// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/CMessageQueue.h>
#include <data/CDataStructs.h>
#include <network/CNetBase.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet.h>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Forward declaration of the global class */

  namespace network {
    /**
    * @class CGameClient
    * @author Christian Keimel
    * This class is the game server for network games. It will
    * run as a thread in the background
    */
    class CGameClient : public CNetBase {
      protected:
        bool m_bConnectionAllowed;

        std::vector<ENetPeer *> m_vPeers;

        data::SRacePlayers m_cPlayers;    /**< The players that this client wants to connect to the server */

        /**
        * Handle an event in a subclass
        * @return "true" if the event was handled
        */
        virtual bool OnEnetEvent(ENetEvent *a_cEvent);

        /**
        * Handle a received message in a subclass
        * @param a_pPeer the peer from which the message was received
        * @param a_pMessage the message to handle
        * @return true if the message was handled
        */
        virtual bool onMessageReceived(ENetPeer *a_pPeer, messages::IMessage *a_pMessage);

      public:
        CGameClient(const std::string &a_sAddress, int a_iPort, CGlobal *a_pGlobal);
        ~CGameClient();

        void setConnectionAllowed(bool a_bAllowed);
    };
  }
}
