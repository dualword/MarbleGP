// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/CMessageFactory.h>
#include <threads/CMessageQueue.h>
#include <threads/IThread.h>
#include <irrlicht.h>
#include <enet/enet.h>
#include <vector>
#include <tuple>

namespace dustbin {
  class CGlobal;  /**< Forward declaration of the global class */

  namespace network {
    /**
    * @class CNetBase
    * @author Christian Keimel
    * The base class for network client and server
    */
    class CNetBase : public threads::IThread {
      protected:
        CGlobal     *m_pGlobal;
        ENetHost    *m_pHost;
        ENetAddress  m_cAddress;
        bool         m_bSendMoved;
        int          m_iStep;

        messages::CMessageFactory l_cFactory;

        std::tuple<int, messages::CMarbleMoved> m_aMyMarbles[16];   /**< Data of all marbles (0 == last update time (-1 == not my marble), 1 == last sent message) */

        /**
        * Handle an event in a subclass
        * @return "true" if the event was handled
        */
        virtual bool OnEnetEvent(ENetEvent *a_cEvent) = 0;

        /**
        * Handle a received message in a subclass
        * @param a_pPeer the peer from which the message was received
        * @param a_pMessage the message to handle
        * @return true if the message was handled
        */
        virtual bool onMessageReceived(ENetPeer *a_pPeer, messages::IMessage *a_pMessage);

        /**
        * React to a message before it's sent to all clients
        * @param a_pMsg the message the will be sent
        */
        virtual void beforeSendMessage(messages::IMessage *a_pMsg) { }

        /**
        * Shall a marble moved message be sent?
        * @param a_iIndex index of the marble
        * @param a_pMsg the new marble moved message
        * @return true if the controls have changed or the last send message is more than c_iSendInterval steps in the past
        */
        bool marbleMessageNecessary(int a_iIndex, messages::CMarbleMoved *a_pMsg);

        /**
        * Get the Marble ID (10000 .. 10016) from a message (if applicable)
        * @param a_pMsg the message to check
        * @return ID of the marble of the message, -1 if message contains no marble information
        */
        int getMarbleId(messages::IMessage *a_pMsg);

      public:
        CNetBase(CGlobal *a_pGlobal);
        ~CNetBase();

        /**
        * The implementing object must implement this method. It is called when the thread starts working
        */
        virtual void execute();

        /**
        * Broadcast a message
        * @param a_pMessage the message to broadcast
        * @param a_bReliable should the message be sent reliable?
        */
        void broadcastMessage(messages::IMessage *a_pMessage, bool a_bReliable);

        /**
        * Send a message to one specific peer
        * @param a_pPeer the peer to send to
        * @param a_pMessage the message to send
        */
        void sendMessage(ENetPeer *a_pPeer, messages::IMessage *a_pMessage);

        /**
        * Mark a marble as a marble of mine
        * @param a_iMarbleId scene node ID of the marble (10000 .. 10016)
        */
        void setMyMarble(int a_iMarbleId);

        /**
        * Reset the my marbles array after the game
        */
        void resetMyMarbles();
    };
  }
}
