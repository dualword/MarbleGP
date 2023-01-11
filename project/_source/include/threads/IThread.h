/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#pragma once

#include <thread>
#include <threads/CMessageQueue.h>

namespace dustbin {
  namespace threads {

    /**
     * @class IThread
     * @brief Base class for threads
     * @author Christian Keimel / https://www.dustbin-online.de
     * This is the base class of all threads used in Stunt Marble Racers 2
     * @see CInputQueue
     * @see COutputQueue
     */
    class IThread {
      protected:
        CInputQueue  *m_pInputQueue;	/**< The input queue used to send messages to the thread */
        COutputQueue *m_pOutputQueue;	/**< The output queue used by the thread to post messages to the registered listeners */

        std::thread m_cThread;			/**< C++ std thread object */

        bool m_bStopThread;				/**< Flag to stop the thread. The implementing thread should react to this flag */
        bool m_bFinished;				  /**< Flag showing whether or not the thread has finished working */
        bool m_bStarted;          /**< Has the thread been started? */

        /**
        * The implementing object must implement this method. It is called when the thread starts working
        */
        virtual void execute() = 0;

        /**
         * This method destroys both the input and the output queue of the thread. All listeners registered in the output queue
         * will be de-registered.
         */
        void destroyQueues() {
          if (m_pInputQueue != NULL) {
            do {
              messages::IMessage* l_pMsg = m_pInputQueue->popMessage();
              if (l_pMsg != nullptr)
                delete l_pMsg;
              else
                break;
            } 
            while (true);

            delete m_pInputQueue;
            m_pInputQueue = NULL;
          }

          if (m_pOutputQueue != NULL) {
            delete m_pOutputQueue;
            m_pOutputQueue = NULL;
          }
        }

      public:
        IThread() {
          m_pInputQueue = new CInputQueue();
          m_pOutputQueue = new COutputQueue();
          m_bStopThread = false;
          m_bFinished = false;
          m_bStarted = false;
        }

        virtual ~IThread() {
          if (m_cThread.joinable()) m_cThread.join();
          destroyQueues();
        }

        /**
         * Get the input queue of the thread to post messages
         */
        CInputQueue* getInputQueue() { return m_pInputQueue; }

        /**
         * Get the output queue of the thread to register listeners
         */
        COutputQueue* getOutputQueue() { return m_pOutputQueue; }

        /**
         * Start the thread. After "execute" has finished the "m_bFinished" flag is set to "true"
         */
        virtual void startThread() {
          m_bStarted = true;
          m_bStopThread = false;
          m_cThread = std::thread([this] { execute(); m_bFinished = true; });
        }

        /**
         * Join the thread (if possible), i.e. block the calling thread until this thread has finished working
         */
        void join() { if (m_cThread.joinable()) m_cThread.join(); }

        /**
         * Set the "m_bStopThread" flag to "true" and make sure the thread processes (if using a blocking input queue
         */
        virtual void stopThread() { m_bStopThread = true; m_pInputQueue->notify(); }

        /**
         * Has the thread already finished?
         * @return "true" if the thread has finished, "false" otherwise
         */
        bool hasFinished() { return m_bFinished; }

        bool hasBeenStarted() { return m_bStarted; }
      };
  }
}