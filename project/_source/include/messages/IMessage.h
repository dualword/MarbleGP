/*
  (w) 2021 by Christian Keimel / https://www.dustbin-online.de
*/
#pragma once

#include <_generated/messages/CMessageEnums.h>

namespace dustbin {
  namespace messages {
    /**
     * Foreward declation of the serializer
     */
    class ISerializer;
    
    /**
     * @class IMessage
     * @brief Base class for all messages. The messages are auto-generated
     */
    class IMessage {
      public:
        /**
         * Virtual destructor
         */
        virtual ~IMessage() { }
        
        /**
         * Retrieve the message ID of this message
         * @return the ID of the message (internally a 16 bit unsigned integer)
         */
        virtual enMessageIDs getMessageId() = 0;
        
        /**
         * Clone this message
         * @return a clone of the message
         */
        virtual IMessage *clone() = 0;
        
        /**
         * Serialize the message using the serializer
         * @param a_pSerializer the serializer which will be filled with the message content
         */
        virtual void serialize(ISerializer *a_pSerializer) = 0;
    };
  } // namespace messages
}   // namespace dustbin
