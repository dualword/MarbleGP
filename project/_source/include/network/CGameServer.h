// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <threads/CMessageQueue.h>
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
    /**
    * @class CGameServer
    * @author Christian Keimel
    * This class is the game server for network games. It will
    * run as a thread in the background
    */
    class CGameServer : public CNetBase {
      protected:
        bool m_bConnectionAllowed;  /**< If the server open for new connections? Will be set to "false" once the gaming starts */

        std::vector<ENetPeer *> m_vPeers;

        std::vector<int> m_vAvailableSlots;   /**< The available client slots */

        std::map<ENetPeer *, std::vector<std::tuple<int, bool>>> m_mAssignedSlots;  /**< The slots that were assigned to a specific peer */

        std::map<ENetPeer *, std::vector<int>> m_mMarbleMap;      /**< The marble IDs that were assigned to a specific peer */

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
    };
  }
}
