/** This file was created by the Dustbin::Games Message Generator Python Script **/

#pragma once

#include <_generated/messages/CMessageEnums.h>
#include <messages/IMessage.h>

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace messages {

    /**
     * @class CCountdown
     * @brief Countdown message before the start of the race
     */
    class CCountdown : public IMessage {
      private:
        irr::u8 m_Tick;  //*< The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)

      public:
        CCountdown(irr::u8 a_Tick);
        CCountdown(ISerializer *a_pSerializer);
        CCountdown(CCountdown *a_pOther);

        virtual ~CCountdown();

        // Getters
        irr::u8 getTick();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CPlayerStunned
     * @brief This message indicates that the stunned state of a player has changed
     */
    class CPlayerStunned : public IMessage {
      private:
        irr::s32 m_MarbleId;  //*< ID of the marble
        irr::u8 m_State;  //*< New stunned state (1 == Player stunned, 2 == Player recovered)

      public:
        CPlayerStunned(irr::s32 a_MarbleId, irr::u8 a_State);
        CPlayerStunned(ISerializer *a_pSerializer);
        CPlayerStunned(CPlayerStunned *a_pOther);

        virtual ~CPlayerStunned();

        // Getters
        irr::s32 getMarbleId();
        irr::u8 getState();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CPlayerRespawn
     * @brief This message indicates that the respawn state of a player has changed
     */
    class CPlayerRespawn : public IMessage {
      private:
        irr::s32 m_MarbleId;  //*< ID of the marble
        irr::u8 m_State;  //*< New respawn state (1 == Respawn Start, 2 == Respawn Camera, 3 == Respawn Done)

      public:
        CPlayerRespawn(irr::s32 a_MarbleId, irr::u8 a_State);
        CPlayerRespawn(ISerializer *a_pSerializer);
        CPlayerRespawn(CPlayerRespawn *a_pOther);

        virtual ~CPlayerRespawn();

        // Getters
        irr::s32 getMarbleId();
        irr::u8 getState();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CRaceFinished
     * @brief The race has ended
     */
    class CRaceFinished : public IMessage {
      private:
        irr::u8 m_Cancelled;  //*< A flag indicating whether or not the race was cancelled by a player

      public:
        CRaceFinished(irr::u8 a_Cancelled);
        CRaceFinished(ISerializer *a_pSerializer);
        CRaceFinished(CRaceFinished *a_pOther);

        virtual ~CRaceFinished();

        // Getters
        irr::u8 getCancelled();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CPlayerFinished
     * @brief A player has finished the race
     */
    class CPlayerFinished : public IMessage {
      private:
        irr::s32 m_MarbleId;  //*< ID of the finished marble
        irr::u32 m_RaceTime;  //*< Racetime of the finished player in simulation steps
        irr::s32 m_Laps;  //*< The number of laps the player has done

      public:
        CPlayerFinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps);
        CPlayerFinished(ISerializer *a_pSerializer);
        CPlayerFinished(CPlayerFinished *a_pOther);

        virtual ~CPlayerFinished();

        // Getters
        irr::s32 getMarbleId();
        irr::u32 getRaceTime();
        irr::s32 getLaps();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CCheckpoint
     * @brief A player has passed a checkpoint
     */
    class CCheckpoint : public IMessage {
      private:
        irr::s32 m_MarbleId;  //*< ID of the marble
        irr::u16 m_Checkpoint;  //*< The checkpoint ID the player has passed

      public:
        CCheckpoint(irr::s32 a_MarbleId, irr::u16 a_Checkpoint);
        CCheckpoint(ISerializer *a_pSerializer);
        CCheckpoint(CCheckpoint *a_pOther);

        virtual ~CCheckpoint();

        // Getters
        irr::s32 getMarbleId();
        irr::u16 getCheckpoint();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CObjectMoved
     * @brief This message is sent when a non-marble object has moved
     */
    class CObjectMoved : public IMessage {
      private:
        irr::s32 m_ObjectId;  //*< The ID of the object
        irr::core::vector3df m_Position;  //*< The current position
        irr::core::vector3df m_Rotation;  //*< The current rotation (Euler angles)
        irr::core::vector3df m_LinearVelocity;  //*< The linear velocity
        irr::core::vector3df m_AngularVelocity;  //*< The angualar (rotation) velocity

      public:
        CObjectMoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity);
        CObjectMoved(ISerializer *a_pSerializer);
        CObjectMoved(CObjectMoved *a_pOther);

        virtual ~CObjectMoved();

        // Getters
        irr::s32 getObjectId();
        const irr::core::vector3df &getPosition();
        const irr::core::vector3df &getRotation();
        const irr::core::vector3df &getLinearVelocity();
        const irr::core::vector3df &getAngularVelocity();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CController
     * @brief Message sent from the control thread to the physics thread for controlling a marble
     */
    class CController : public IMessage {
      private:
        irr::s32 m_MarbleId;  //*< The ID of the marble
        irr::core::vector3df m_Direction;  //*< The control direction 
        irr::f32 m_CtrlX;  //*< X-Control [-1..1]
        irr::f32 m_CtrlY;  //*< Y-Control [-1..1]
        irr::u8 m_RearView;  //*< RearView flag

      public:
        CController(irr::s32 a_MarbleId, const irr::core::vector3df &a_Direction, irr::f32 a_CtrlX, irr::f32 a_CtrlY, irr::u8 a_RearView);
        CController(ISerializer *a_pSerializer);
        CController(CController *a_pOther);

        virtual ~CController();

        // Getters
        irr::s32 getMarbleId();
        const irr::core::vector3df &getDirection();
        irr::f32 getCtrlX();
        irr::f32 getCtrlY();
        irr::u8 getRearView();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CTrigger
     * @brief Message indicating then a marble has triggered a checkpoint
     */
    class CTrigger : public IMessage {
      private:
        irr::s32 m_TriggerId;  //*< ID of the trigger
        irr::s32 m_ObjectId;  //*< ID of the marble that caused the trigger

      public:
        CTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId);
        CTrigger(ISerializer *a_pSerializer);
        CTrigger(CTrigger *a_pOther);

        virtual ~CTrigger();

        // Getters
        irr::s32 getTriggerId();
        irr::s32 getObjectId();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CStepMsg
     * @brief Message sent for every step (60 per second)
     */
    class CStepMsg : public IMessage {
      private:
        irr::u32 m_StepNo;  //*< The current step number

      public:
        CStepMsg(irr::u32 a_StepNo);
        CStepMsg(ISerializer *a_pSerializer);
        CStepMsg(CStepMsg *a_pOther);

        virtual ~CStepMsg();

        // Getters
        irr::u32 getStepNo();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

    /**
     * @class CMarbleMoved
     * @brief This message is sent when a marble is moved and contains additional information
     */
    class CMarbleMoved : public IMessage {
      private:
        irr::s32 m_ObjectId;  //*< The ID of the object
        irr::core::vector3df m_Position;  //*< The current position
        irr::core::vector3df m_Rotation;  //*< The current rotation (Euler angles)
        irr::core::vector3df m_LinearVelocity;  //*< The linear velocity
        irr::core::vector3df m_AngularVelocity;  //*< The angualar (rotation) velocity
        irr::u8 m_Contact;  //*< A Flag indicating whether or not the marble is in contact with another object
        irr::core::vector3df m_ContactPoint;  //*< The point of contact if the marble is in contact with another object
        irr::s8 m_ControlX;  //*< The marble's current controller state in X-Direction
        irr::s8 m_ControlY;  //*< The marble's current controller state in Y-Direction
        irr::u8 m_ControlBrake;  //*< Flag indicating whether or not the marble's brake is active
        irr::u8 m_ControlRearView;  //*< Flag indicating whether or not the marble's player looks behind
        irr::f32 m_Distance;  //*< The distance the marble has travelled in the current lap

      public:
        CMarbleMoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity, irr::u8 a_Contact, const irr::core::vector3df &a_ContactPoint, irr::s8 a_ControlX, irr::s8 a_ControlY, irr::u8 a_ControlBrake, irr::u8 a_ControlRearView, irr::f32 a_Distance);
        CMarbleMoved(ISerializer *a_pSerializer);
        CMarbleMoved(CMarbleMoved *a_pOther);

        virtual ~CMarbleMoved();

        // Getters
        irr::s32 getObjectId();
        const irr::core::vector3df &getPosition();
        const irr::core::vector3df &getRotation();
        const irr::core::vector3df &getLinearVelocity();
        const irr::core::vector3df &getAngularVelocity();
        irr::u8 getContact();
        const irr::core::vector3df &getContactPoint();
        irr::s8 getControlX();
        irr::s8 getControlY();
        irr::u8 getControlBrake();
        irr::u8 getControlRearView();
        irr::f32 getDistance();

        // Methods inherited from dustbin::messages::IMessage
        virtual enMessageIDs getMessageId();
        virtual IMessage *clone();
        virtual void serialize(ISerializer *a_pSerializer);
    };

  }  // namespace messages
}    // namespace dustbin

