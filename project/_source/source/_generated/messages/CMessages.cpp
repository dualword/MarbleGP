/** This file was created by the Dustbin::Games Message Generator Python Script **/

#include <_generated/messages/CMessages.h>

namespace dustbin {
  namespace messages {

    // Implementation of "CCountdown"
    CCountdown::CCountdown(irr::u8 a_Tick) {
      m_Tick = a_Tick;
    }

    CCountdown::CCountdown(ISerializer *a_pSerializer) {
      m_Tick = a_pSerializer->getU8();
    }

    CCountdown::CCountdown(CCountdown *a_pOther) {
      m_Tick = a_pOther->getTick();
    }

    irr::u8 CCountdown::getTick() {
      return m_Tick;
    }

    CCountdown::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::Countdown);

      a_pSerializer->addU8(m_Tick);
    }

    IMessage *CCountdown::clone() {
      return new CCountdown(this);
    }

    enMessageIDs CCountdown::getMessageId() {
      return enMessageIDs::Countdown;
    }

    // Implementation of "CPlayerStunned"
    CPlayerStunned::CPlayerStunned(irr::s32 a_MarbleId, irr::u8 a_State) {
      m_MarbleId = a_MarbleId;
      m_State = a_State;
    }

    CPlayerStunned::CPlayerStunned(ISerializer *a_pSerializer) {
      m_MarbleId = a_pSerializer->getS32();
      m_State = a_pSerializer->getU8();
    }

    CPlayerStunned::CPlayerStunned(CPlayerStunned *a_pOther) {
      m_MarbleId = a_pOther->getMarbleId();
      m_State = a_pOther->getState();
    }

    irr::s32 CPlayerStunned::getMarbleId() {
      return m_MarbleId;
    }

    irr::u8 CPlayerStunned::getState() {
      return m_State;
    }

    CPlayerStunned::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::PlayerStunned);

      a_pSerializer->addS32(m_MarbleId);
      a_pSerializer->addU8(m_State);
    }

    IMessage *CPlayerStunned::clone() {
      return new CPlayerStunned(this);
    }

    enMessageIDs CPlayerStunned::getMessageId() {
      return enMessageIDs::PlayerStunned;
    }

    // Implementation of "CPlayerRespawn"
    CPlayerRespawn::CPlayerRespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      m_MarbleId = a_MarbleId;
      m_State = a_State;
    }

    CPlayerRespawn::CPlayerRespawn(ISerializer *a_pSerializer) {
      m_MarbleId = a_pSerializer->getS32();
      m_State = a_pSerializer->getU8();
    }

    CPlayerRespawn::CPlayerRespawn(CPlayerRespawn *a_pOther) {
      m_MarbleId = a_pOther->getMarbleId();
      m_State = a_pOther->getState();
    }

    irr::s32 CPlayerRespawn::getMarbleId() {
      return m_MarbleId;
    }

    irr::u8 CPlayerRespawn::getState() {
      return m_State;
    }

    CPlayerRespawn::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::PlayerRespawn);

      a_pSerializer->addS32(m_MarbleId);
      a_pSerializer->addU8(m_State);
    }

    IMessage *CPlayerRespawn::clone() {
      return new CPlayerRespawn(this);
    }

    enMessageIDs CPlayerRespawn::getMessageId() {
      return enMessageIDs::PlayerRespawn;
    }

    // Implementation of "CRaceFinished"
    CRaceFinished::CRaceFinished(irr::u8 a_Cancelled) {
      m_Cancelled = a_Cancelled;
    }

    CRaceFinished::CRaceFinished(ISerializer *a_pSerializer) {
      m_Cancelled = a_pSerializer->getU8();
    }

    CRaceFinished::CRaceFinished(CRaceFinished *a_pOther) {
      m_Cancelled = a_pOther->getCancelled();
    }

    irr::u8 CRaceFinished::getCancelled() {
      return m_Cancelled;
    }

    CRaceFinished::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::RaceFinished);

      a_pSerializer->addU8(m_Cancelled);
    }

    IMessage *CRaceFinished::clone() {
      return new CRaceFinished(this);
    }

    enMessageIDs CRaceFinished::getMessageId() {
      return enMessageIDs::RaceFinished;
    }

    // Implementation of "CPlayerFinished"
    CPlayerFinished::CPlayerFinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) {
      m_MarbleId = a_MarbleId;
      m_RaceTime = a_RaceTime;
      m_Laps = a_Laps;
    }

    CPlayerFinished::CPlayerFinished(ISerializer *a_pSerializer) {
      m_MarbleId = a_pSerializer->getS32();
      m_RaceTime = a_pSerializer->getU32();
      m_Laps = a_pSerializer->getS32();
    }

    CPlayerFinished::CPlayerFinished(CPlayerFinished *a_pOther) {
      m_MarbleId = a_pOther->getMarbleId();
      m_RaceTime = a_pOther->getRaceTime();
      m_Laps = a_pOther->getLaps();
    }

    irr::s32 CPlayerFinished::getMarbleId() {
      return m_MarbleId;
    }

    irr::u32 CPlayerFinished::getRaceTime() {
      return m_RaceTime;
    }

    irr::s32 CPlayerFinished::getLaps() {
      return m_Laps;
    }

    CPlayerFinished::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::PlayerFinished);

      a_pSerializer->addS32(m_MarbleId);
      a_pSerializer->addU32(m_RaceTime);
      a_pSerializer->addS32(m_Laps);
    }

    IMessage *CPlayerFinished::clone() {
      return new CPlayerFinished(this);
    }

    enMessageIDs CPlayerFinished::getMessageId() {
      return enMessageIDs::PlayerFinished;
    }

    // Implementation of "CCheckpoint"
    CCheckpoint::CCheckpoint(irr::s32 a_MarbleId, irr::u16 a_Checkpoint) {
      m_MarbleId = a_MarbleId;
      m_Checkpoint = a_Checkpoint;
    }

    CCheckpoint::CCheckpoint(ISerializer *a_pSerializer) {
      m_MarbleId = a_pSerializer->getS32();
      m_Checkpoint = a_pSerializer->getU16();
    }

    CCheckpoint::CCheckpoint(CCheckpoint *a_pOther) {
      m_MarbleId = a_pOther->getMarbleId();
      m_Checkpoint = a_pOther->getCheckpoint();
    }

    irr::s32 CCheckpoint::getMarbleId() {
      return m_MarbleId;
    }

    irr::u16 CCheckpoint::getCheckpoint() {
      return m_Checkpoint;
    }

    CCheckpoint::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::Checkpoint);

      a_pSerializer->addS32(m_MarbleId);
      a_pSerializer->addU16(m_Checkpoint);
    }

    IMessage *CCheckpoint::clone() {
      return new CCheckpoint(this);
    }

    enMessageIDs CCheckpoint::getMessageId() {
      return enMessageIDs::Checkpoint;
    }

    // Implementation of "CObjectMoved"
    CObjectMoved::CObjectMoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity) {
      m_ObjectId = a_ObjectId;
      m_Position = a_Position;
      m_Rotation = a_Rotation;
      m_LinearVelocity = a_LinearVelocity;
      m_AngularVelocity = a_AngularVelocity;
    }

    CObjectMoved::CObjectMoved(ISerializer *a_pSerializer) {
      m_ObjectId = a_pSerializer->getS32();
      m_Position = a_pSerializer->getVector3df();
      m_Rotation = a_pSerializer->getVector3df();
      m_LinearVelocity = a_pSerializer->getVector3df();
      m_AngularVelocity = a_pSerializer->getVector3df();
    }

    CObjectMoved::CObjectMoved(CObjectMoved *a_pOther) {
      m_ObjectId = a_pOther->getObjectId();
      m_Position = a_pOther->getPosition();
      m_Rotation = a_pOther->getRotation();
      m_LinearVelocity = a_pOther->getLinearVelocity();
      m_AngularVelocity = a_pOther->getAngularVelocity();
    }

    irr::s32 CObjectMoved::getObjectId() {
      return m_ObjectId;
    }

    const irr::core::vector3df &CObjectMoved::getPosition() {
      return m_Position;
    }

    const irr::core::vector3df &CObjectMoved::getRotation() {
      return m_Rotation;
    }

    const irr::core::vector3df &CObjectMoved::getLinearVelocity() {
      return m_LinearVelocity;
    }

    const irr::core::vector3df &CObjectMoved::getAngularVelocity() {
      return m_AngularVelocity;
    }

    CObjectMoved::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::ObjectMoved);

      a_pSerializer->addS32(m_ObjectId);
      a_pSerializer->addVector3df(m_Position);
      a_pSerializer->addVector3df(m_Rotation);
      a_pSerializer->addVector3df(m_LinearVelocity);
      a_pSerializer->addVector3df(m_AngularVelocity);
    }

    IMessage *CObjectMoved::clone() {
      return new CObjectMoved(this);
    }

    enMessageIDs CObjectMoved::getMessageId() {
      return enMessageIDs::ObjectMoved;
    }

    // Implementation of "CController"
    CController::CController(irr::s32 a_MarbleId, const irr::core::vector3df &a_Direction, irr::f32 a_CtrlX, irr::f32 a_CtrlY, irr::u8 a_RearView) {
      m_MarbleId = a_MarbleId;
      m_Direction = a_Direction;
      m_CtrlX = a_CtrlX;
      m_CtrlY = a_CtrlY;
      m_RearView = a_RearView;
    }

    CController::CController(ISerializer *a_pSerializer) {
      m_MarbleId = a_pSerializer->getS32();
      m_Direction = a_pSerializer->getVector3df();
      m_CtrlX = a_pSerializer->getF32();
      m_CtrlY = a_pSerializer->getF32();
      m_RearView = a_pSerializer->getU8();
    }

    CController::CController(CController *a_pOther) {
      m_MarbleId = a_pOther->getMarbleId();
      m_Direction = a_pOther->getDirection();
      m_CtrlX = a_pOther->getCtrlX();
      m_CtrlY = a_pOther->getCtrlY();
      m_RearView = a_pOther->getRearView();
    }

    irr::s32 CController::getMarbleId() {
      return m_MarbleId;
    }

    const irr::core::vector3df &CController::getDirection() {
      return m_Direction;
    }

    irr::f32 CController::getCtrlX() {
      return m_CtrlX;
    }

    irr::f32 CController::getCtrlY() {
      return m_CtrlY;
    }

    irr::u8 CController::getRearView() {
      return m_RearView;
    }

    CController::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::Controller);

      a_pSerializer->addS32(m_MarbleId);
      a_pSerializer->addVector3df(m_Direction);
      a_pSerializer->addF32(m_CtrlX);
      a_pSerializer->addF32(m_CtrlY);
      a_pSerializer->addU8(m_RearView);
    }

    IMessage *CController::clone() {
      return new CController(this);
    }

    enMessageIDs CController::getMessageId() {
      return enMessageIDs::Controller;
    }

    // Implementation of "CTrigger"
    CTrigger::CTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId) {
      m_TriggerId = a_TriggerId;
      m_ObjectId = a_ObjectId;
    }

    CTrigger::CTrigger(ISerializer *a_pSerializer) {
      m_TriggerId = a_pSerializer->getS32();
      m_ObjectId = a_pSerializer->getS32();
    }

    CTrigger::CTrigger(CTrigger *a_pOther) {
      m_TriggerId = a_pOther->getTriggerId();
      m_ObjectId = a_pOther->getObjectId();
    }

    irr::s32 CTrigger::getTriggerId() {
      return m_TriggerId;
    }

    irr::s32 CTrigger::getObjectId() {
      return m_ObjectId;
    }

    CTrigger::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::Trigger);

      a_pSerializer->addS32(m_TriggerId);
      a_pSerializer->addS32(m_ObjectId);
    }

    IMessage *CTrigger::clone() {
      return new CTrigger(this);
    }

    enMessageIDs CTrigger::getMessageId() {
      return enMessageIDs::Trigger;
    }

    // Implementation of "CStepMsg"
    CStepMsg::CStepMsg(irr::u32 a_StepNo) {
      m_StepNo = a_StepNo;
    }

    CStepMsg::CStepMsg(ISerializer *a_pSerializer) {
      m_StepNo = a_pSerializer->getU32();
    }

    CStepMsg::CStepMsg(CStepMsg *a_pOther) {
      m_StepNo = a_pOther->getStepNo();
    }

    irr::u32 CStepMsg::getStepNo() {
      return m_StepNo;
    }

    CStepMsg::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::StepMsg);

      a_pSerializer->addU32(m_StepNo);
    }

    IMessage *CStepMsg::clone() {
      return new CStepMsg(this);
    }

    enMessageIDs CStepMsg::getMessageId() {
      return enMessageIDs::StepMsg;
    }

    // Implementation of "CMarbleMoved"
    CMarbleMoved::CMarbleMoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity, irr::u8 a_Contact, const irr::core::vector3df &a_ContactPoint, irr::s8 a_ControlX, irr::s8 a_ControlY, irr::u8 a_ControlBrake, irr::u8 a_ControlRearView, irr::f32 a_Distance) {
      m_ObjectId = a_ObjectId;
      m_Position = a_Position;
      m_Rotation = a_Rotation;
      m_LinearVelocity = a_LinearVelocity;
      m_AngularVelocity = a_AngularVelocity;
      m_Contact = a_Contact;
      m_ContactPoint = a_ContactPoint;
      m_ControlX = a_ControlX;
      m_ControlY = a_ControlY;
      m_ControlBrake = a_ControlBrake;
      m_ControlRearView = a_ControlRearView;
      m_Distance = a_Distance;
    }

    CMarbleMoved::CMarbleMoved(ISerializer *a_pSerializer) {
      m_ObjectId = a_pSerializer->getS32();
      m_Position = a_pSerializer->getVector3df();
      m_Rotation = a_pSerializer->getVector3df();
      m_LinearVelocity = a_pSerializer->getVector3df();
      m_AngularVelocity = a_pSerializer->getVector3df();
      m_Contact = a_pSerializer->getU8();
      m_ContactPoint = a_pSerializer->getVector3df();
      m_ControlX = a_pSerializer->getS8();
      m_ControlY = a_pSerializer->getS8();
      m_ControlBrake = a_pSerializer->getU8();
      m_ControlRearView = a_pSerializer->getU8();
      m_Distance = a_pSerializer->getF32();
    }

    CMarbleMoved::CMarbleMoved(CMarbleMoved *a_pOther) {
      m_ObjectId = a_pOther->getObjectId();
      m_Position = a_pOther->getPosition();
      m_Rotation = a_pOther->getRotation();
      m_LinearVelocity = a_pOther->getLinearVelocity();
      m_AngularVelocity = a_pOther->getAngularVelocity();
      m_Contact = a_pOther->getContact();
      m_ContactPoint = a_pOther->getContactPoint();
      m_ControlX = a_pOther->getControlX();
      m_ControlY = a_pOther->getControlY();
      m_ControlBrake = a_pOther->getControlBrake();
      m_ControlRearView = a_pOther->getControlRearView();
      m_Distance = a_pOther->getDistance();
    }

    irr::s32 CMarbleMoved::getObjectId() {
      return m_ObjectId;
    }

    const irr::core::vector3df &CMarbleMoved::getPosition() {
      return m_Position;
    }

    const irr::core::vector3df &CMarbleMoved::getRotation() {
      return m_Rotation;
    }

    const irr::core::vector3df &CMarbleMoved::getLinearVelocity() {
      return m_LinearVelocity;
    }

    const irr::core::vector3df &CMarbleMoved::getAngularVelocity() {
      return m_AngularVelocity;
    }

    irr::u8 CMarbleMoved::getContact() {
      return m_Contact;
    }

    const irr::core::vector3df &CMarbleMoved::getContactPoint() {
      return m_ContactPoint;
    }

    irr::s8 CMarbleMoved::getControlX() {
      return m_ControlX;
    }

    irr::s8 CMarbleMoved::getControlY() {
      return m_ControlY;
    }

    irr::u8 CMarbleMoved::getControlBrake() {
      return m_ControlBrake;
    }

    irr::u8 CMarbleMoved::getControlRearView() {
      return m_ControlRearView;
    }

    irr::f32 CMarbleMoved::getDistance() {
      return m_Distance;
    }

    CMarbleMoved::serialize(ISerializer *a_pSerializer) {
      a_pSerializer->addU16((irr::u16)enMessageIDs::MarbleMoved);

      a_pSerializer->addS32(m_ObjectId);
      a_pSerializer->addVector3df(m_Position);
      a_pSerializer->addVector3df(m_Rotation);
      a_pSerializer->addVector3df(m_LinearVelocity);
      a_pSerializer->addVector3df(m_AngularVelocity);
      a_pSerializer->addU8(m_Contact);
      a_pSerializer->addVector3df(m_ContactPoint);
      a_pSerializer->addS8(m_ControlX);
      a_pSerializer->addS8(m_ControlY);
      a_pSerializer->addU8(m_ControlBrake);
      a_pSerializer->addU8(m_ControlRearView);
      a_pSerializer->addF32(m_Distance);
    }

    IMessage *CMarbleMoved::clone() {
      return new CMarbleMoved(this);
    }

    enMessageIDs CMarbleMoved::getMessageId() {
      return enMessageIDs::MarbleMoved;
    }

  }  // namespace messages
}    // namespace dustbin

