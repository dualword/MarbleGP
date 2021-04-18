/** This file was created by the Dustbin::Games Message Generator Python Script **/

#include <_generated/messages/CMessageFactory.h>

namespace dustbin {
  namespace messages {

    IMessage *CMessageFactory::createMessage(ISerializer *a_pSerializer) {
      switch (a_pSerializer->getMessageType()) {
        case (irr::u16)enMessageIDs::Countdown: return new CCountdown(a_pSerializer); break;
        case (irr::u16)enMessageIDs::PlayerStunned: return new CPlayerStunned(a_pSerializer); break;
        case (irr::u16)enMessageIDs::PlayerRespawn: return new CPlayerRespawn(a_pSerializer); break;
        case (irr::u16)enMessageIDs::RaceFinished: return new CRaceFinished(a_pSerializer); break;
        case (irr::u16)enMessageIDs::PlayerFinished: return new CPlayerFinished(a_pSerializer); break;
        case (irr::u16)enMessageIDs::Checkpoint: return new CCheckpoint(a_pSerializer); break;
        case (irr::u16)enMessageIDs::ObjectMoved: return new CObjectMoved(a_pSerializer); break;
        case (irr::u16)enMessageIDs::Controller: return new CController(a_pSerializer); break;
        case (irr::u16)enMessageIDs::Trigger: return new CTrigger(a_pSerializer); break;
        case (irr::u16)enMessageIDs::StepMsg: return new CStepMsg(a_pSerializer); break;
        case (irr::u16)enMessageIDs::MarbleMoved: return new CMarbleMoved(a_pSerializer); break;
      }
      return nullptr;
    }
  }  // namespace messages
}    // namespace dustbin

