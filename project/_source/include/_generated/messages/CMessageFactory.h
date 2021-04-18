/** This file was created by the Dustbin::Games Message Generator Python Script **/

#pragma once

#include <_generated/messages/CMessages.h>

namespace dustbin {
  namespace messages {

    class ISerializer;
    class IMessage;

    class CMessageFactory {
      public:
        static IMessage *createMessage(ISerializer *a_pSerializer);
    };
  }  // namespace messages
}    // namespace dustbin

