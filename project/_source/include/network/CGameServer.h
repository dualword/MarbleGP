// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/CMessageQueue.h>
#include <data/CDataStructs.h>
#include <network/CNetBase.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet.h>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  class CGlobal;  /**< Forward declaration of the global class */

  namespace network {
    class CDiscoveryServer; /**< Forward declaration of the discovery server thread */
    /**
    * @class CGameServer
    * @author Christian Keimel
    * This class is the game server for network games. It will
    * run as a thread in the background
    */
    class CGameServer : public CNetBase {
      protected:
        bool m_bConnectionAllowed;  /**< If the server open for new connections? Will be set to "false" once the gaming starts */

        std::string m_sHostName;    /**< Name of the host */

        std::vector<ENetPeer *> m_vPeers;

        std::vector<int> m_vAvailableSlots;   /**< The available client slots */

        std::map<ENetPeer *, std::vector<std::tuple<int, bool>>> m_mAssignedSlots;  /**< The slots that were assigned to a specific peer */

        std::map<ENetPeer *, std::vector<int>> m_mMarbleMap;      /**< The marble IDs that were assigned to a specific peer */
        std::map<ENetPeer *, std::vector<int>> m_mPlayerMap;      /**< The player IDs that were assigned to a specific peer */

        std::map<ENetPeer *, std::string> m_mCurrentStates;    /**< The current states the clients are in */

        data::SRacePlayers m_cPlayers;    /**< The players */

        CDiscoveryServer *m_pDiscovery;   /**< The discovery server thread */

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
        CGameServer(const std::vector<int> &a_vAvailableIDs, CGlobal *a_pGlobal);
        ~CGameServer();

        void setConnectionAllowed(bool a_bAllowed);

        /**
        * Check whether or not all of the clients are in a specific state
        * @param a_sState the state to check for
        * @return true if all clients have reached the requested state, false otherwise
        */
        bool allClientsAreInState(const std::string &a_sState);

        /**
        * Transfer a global entry to all clients
        * @param a_sKey the key to transfer
        */
        void sendGlobalData(const std::string &a_sKey);

        /**
        * Notify all clients about a state change
        * @param a_sNewState the state to change to
        */
        void changeState(const std::string &a_sNewState);

        /**
        * Get the host name
        * @return the host name
        */
        const std::string &getHostName();
    };
  }
}
