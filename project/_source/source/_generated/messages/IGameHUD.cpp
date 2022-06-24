/** This file was created by the Dustbin::Games Message Generator Python Script **/

#include <_generated/messages/IGameHUD.h>
#include <_generated/messages/CMessageEnums.h>
#include <_generated/messages/CMessages.h>

namespace dustbin {
  namespace messages {

      IGameHUD::IGameHUD() {
      }

      IGameHUD::~IGameHUD() {
      }

      bool IGameHUD::handleMessage(dustbin::messages::IMessage *a_pMessage, bool a_bDelete) {
        bool l_bRet = false;
        if (a_pMessage != nullptr) {
          dustbin::messages::enMessageIDs l_eMsgId = a_pMessage->getMessageId();

          switch (l_eMsgId) {
            case dustbin::messages::enMessageIDs::PlayerRespawn: {
              dustbin::messages::CPlayerRespawn *p = reinterpret_cast<dustbin::messages::CPlayerRespawn *>(a_pMessage);
              onPlayerrespawn(p->getMarbleId(), p->getState());
              break;
            }
            case dustbin::messages::enMessageIDs::PlayerFinished: {
              dustbin::messages::CPlayerFinished *p = reinterpret_cast<dustbin::messages::CPlayerFinished *>(a_pMessage);
              onPlayerfinished(p->getMarbleId(), p->getRaceTime(), p->getLaps());
              break;
            }
            case dustbin::messages::enMessageIDs::PlayerStunned: {
              dustbin::messages::CPlayerStunned *p = reinterpret_cast<dustbin::messages::CPlayerStunned *>(a_pMessage);
              onPlayerstunned(p->getMarbleId(), p->getState());
              break;
            }
            case dustbin::messages::enMessageIDs::RaceFinished: {
              dustbin::messages::CRaceFinished *p = reinterpret_cast<dustbin::messages::CRaceFinished *>(a_pMessage);
              onRacefinished(p->getCancelled());
              break;
            }
            case dustbin::messages::enMessageIDs::RacePosition: {
              dustbin::messages::CRacePosition *p = reinterpret_cast<dustbin::messages::CRacePosition *>(a_pMessage);
              onRaceposition(p->getMarbleId(), p->getPosition(), p->getLaps(), p->getDeficitAhead(), p->getDeficitLeader());
              break;
            }
            case dustbin::messages::enMessageIDs::MarbleMoved: {
              dustbin::messages::CMarbleMoved *p = reinterpret_cast<dustbin::messages::CMarbleMoved *>(a_pMessage);
              onMarblemoved(p->getObjectId(), p->getPosition(), p->getRotation(), p->getLinearVelocity(), p->getAngularVelocity(), p->getCameraPosition(), p->getCameraUp(), p->getControlX(), p->getControlY(), p->getContact(), p->getControlBrake(), p->getControlRearView(), p->getControlRespawn());
              break;
            }
            case dustbin::messages::enMessageIDs::LapStart: {
              dustbin::messages::CLapStart *p = reinterpret_cast<dustbin::messages::CLapStart *>(a_pMessage);
              onLapstart(p->getMarbleId(), p->getLapNo());
              break;
            }
            case dustbin::messages::enMessageIDs::PlayerRostrum: {
              dustbin::messages::CPlayerRostrum *p = reinterpret_cast<dustbin::messages::CPlayerRostrum *>(a_pMessage);
              onPlayerrostrum(p->getMarbleId());
              break;
            }
            case dustbin::messages::enMessageIDs::Countdown: {
              dustbin::messages::CCountdown *p = reinterpret_cast<dustbin::messages::CCountdown *>(a_pMessage);
              onCountdown(p->getTick());
              break;
            }
            case dustbin::messages::enMessageIDs::StepMsg: {
              dustbin::messages::CStepMsg *p = reinterpret_cast<dustbin::messages::CStepMsg *>(a_pMessage);
              onStepmsg(p->getStepNo());
              break;
            }
            case dustbin::messages::enMessageIDs::ConfirmWithdraw: {
              dustbin::messages::CConfirmWithdraw *p = reinterpret_cast<dustbin::messages::CConfirmWithdraw *>(a_pMessage);
              onConfirmwithdraw(p->getMarbleId(), p->getTimeout());
              break;
            }
            case dustbin::messages::enMessageIDs::PauseChanged: {
              dustbin::messages::CPauseChanged *p = reinterpret_cast<dustbin::messages::CPauseChanged *>(a_pMessage);
              onPausechanged(p->getPaused());
              break;
            }
            default:
              break;
          }

          if (a_bDelete) {
            delete a_pMessage;
            l_bRet = true;
          }
        }
        return l_bRet;
      }

  }  // namespace messages
}    // namespace dustbin

