/** This file was created by the Dustbin::Games Message Generator Python Script **/

#pragma once

namespace dustbin {
  namespace messages {

    /**
     * This enumeration contains all the message IDs
     * @see dustbin::messages::IMessage::getID
     */
    enum class enMessageIDs {
      ObjectMoved = 0,
      MarbleMoved = 1,
      StepMsg = 2,
      Trigger = 3,
      Controller = 4,
      PlayerRespawn = 5,
      PlayerStunned = 6,
      Checkpoint = 7,
      PlayerFinished = 8,
      RaceFinished = 9,
      Countdown = 10
    }
  }  // namespace messages
}    // namespace dustbin

