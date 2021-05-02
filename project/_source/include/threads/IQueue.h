/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#pragma once

class IMessage;

namespace dustbin {
  namespace messages {
    class IMessage;
  }

  namespace threads {
    
    /**
     * @class IQueue
     * @brief Interface for message queues
     * @author Christian Keimel / https://www.dustbin-online.de
     * This is the interface all queues must implement
     * @see CInputQueue
     * @see COutputQueue
     */
    class IQueue {
      public:
        virtual ~IQueue() { }
      /**
       * This method posts a message to the queue
       * @param a_pMsg the message to post
       */
        virtual void postMessage(dustbin::messages::IMessage *a_pMsg) = 0;
    };

  }
}