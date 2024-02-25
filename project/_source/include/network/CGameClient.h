// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/CMessageQueue.h>
#include <gameclasses/SPlayer.h>
#include <data/CDataStructs.h>
#include <network/CNetBase.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet/enet.h>
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
        bool m_bConnected;

        std::vector<ENetPeer *> m_vPeers;

        std::vector<gameclasses::SPlayer> m_vPlayers;   /**< The players of this client */

        /**
        * Handle an event in a subclass
        * @return "true" if the event was handled
        */
        virtual bool OnEnetEvent(ENetEvent *a_cEvent) override;

        /**
        * Handle a received message in a subclass
        * @param a_pPeer the peer from which the message was received
        * @param a_pMessage the message to handle
        * @return true if the message was handled
        */
        virtual bool onMessageReceived(ENetPeer *a_pPeer, messages::IMessage *a_pMessage) override;


        /**
        * React to a message before it's sent to all clients
        * @param a_pMsg the message the will be sent
        * @return true if the message shall be sent, false otherwise
        */
        virtual bool beforeSendMessage(messages::IMessage *a_pMsg) override;

    public:
        CGameClient(irr::u32 a_iHostIP, int a_iPort, CGlobal *a_pGlobal, const std::vector<gameclasses::SPlayer> a_vPlayers);
        ~CGameClient();

        /**
        * Notify the server about a state change
        * @param a_sState the new active state
        */
        void stateChanged(const std::string &a_sState);
    };
  }
}
