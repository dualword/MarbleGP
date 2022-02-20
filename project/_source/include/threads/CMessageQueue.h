// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <mutex>
#include <vector>
#include <condition_variable>

#include <messages/IMessage.h>
#include <threads/IQueue.h>

namespace dustbin {
  namespace threads {

    class COutputQueue;

    /**
     * @class CInputQueue
     * @brief Message queue to send data to a thread
     * @author Christian Keimel / https://www.dustbin-online.de
     * This is the class that allows sending messages to a thread
     */
    class CInputQueue: public IQueue {
      private:
        std::mutex m_cMutex;					/**< Mutex to lock the message vector */
        std::vector<dustbin::messages::IMessage *> m_cMessages;	/**< Vector of messages to be processed */
        /**
         * A list of all senders posting to this queue.
         * @see COutputQueue::addListener
         */
        std::vector<COutputQueue *> m_cSenders;	
        std::condition_variable m_cCv;			/**< Condition Variable used to notify the thread about new messages */

      public:
        CInputQueue();
        virtual ~CInputQueue();
      
        /**
         * Post a message to the input queue. The thread this queue belongs to gets notified
         * @param a_pMsg the message to add to the list of pending messages
         */
        virtual void postMessage(dustbin::messages::IMessage *a_pMsg);
      
        /**
         * Removes the last (oldest) message from the message vector and returns it to the caller (i.e. the thread the queue belongs to)
         * @return the oldes message in the queue, nullptr if not messages are pending
         */
        dustbin::messages::IMessage *popMessage();
      
        /**
         * This method blocks the calling thread until a message is received or the queue is destroyed
         */
        dustbin::messages::IMessage *waitMesssage();
      
        /**
         * Add an output queue that posts it's messages to this input queue
         * @param a_pSender the queue to register
         */
        void addSender(COutputQueue *a_pSender);
      
        /**
         * Remove a sender from the list of senders
         * @param a_pSender the sender to be removed
         */
        void removeSender(COutputQueue *a_pSender);
      
        /**
         * Get the number of registered senders
         * @return the number of registered senders
         */
        int getSenderCount();
      
        /**
         * Get the number of pending messages
         * @return the number of pending messages
         */
        int getMessageCount() { return (int)m_cMessages.size(); }
      
        /**
         * Notify (wake up) the thread that this input queue belongs to
         */
        void notify();
    };

    /**
     * @class COutputQueue
     * @brief Message queue to receive data from a thread
     * @author Christian Keimel / https://www.dustbin-online.de
     * This class is used to allow threads post messages to other threads
     */
    class COutputQueue: public IQueue {
      private:
        std::vector<CInputQueue *> m_cListeners;	/**< A list of input queues that all messages posted to this queue are forewarded to */
        std::mutex m_cMutex;						/**< Mutex of the queue */

      public:
        COutputQueue();
        virtual ~COutputQueue();
      
        /**
         * This message posts a message to the queue, i.e. forewards it to all registered input queues
         * @param a_pMsg the message to be distributed
         */
        virtual void postMessage(dustbin::messages::IMessage *a_pMsg);
      
        /**
         * Add a listener to the output queue
         * @param a_pListener the new listener to be added
         */
        void addListener(CInputQueue *a_pListener);
      
        /**
         * Remove a listener from the output queue
         * @param a_pListener the listener to be removed
         */
        void removeListener(CInputQueue *a_pListener);
      
        /**
         * Get the number of registered listeners
         * @return the number of registered listeners
         */
        int getListenerCount();
    };

  }
}