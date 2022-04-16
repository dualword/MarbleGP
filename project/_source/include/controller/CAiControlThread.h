// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/IGameState.h>
#include <controller/CControllerAI.h>
#include <threads/IThread.h>
#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    class CAiNode;  /**< Forward declaration of the AI node */
  }

  namespace controller {
    /**
    * @class CAiControlThread
    * @author Christian Keimel
    * This thread calculates the controls of the AI marbles
    */
    class CAiControlThread : public threads::IThread, public messages::IGameState {
      private:
        controller::CControllerAI *m_aControllers[17];    /**< The AI controllers, one more than possible to have a terminating "nullptr" */
        scenenodes::CAiNode       *m_pAiNode;
        threads::COutputQueue     *m_pDynamicsOut;
        threads::CInputQueue      *m_pDynamicsIn;

        int m_iNumberOfBots;

      protected:
        /**
        * This function receives messages of type "StepMsg"
        * @param a_StepNo The current step number
        */
        virtual void onStepmsg(irr::u32 a_StepNo);

        /**
        * This function receives messages of type "Countdown"
        * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
        */
        virtual void onCountdown(irr::u8 a_Tick);

        /**
        * This function receives messages of type "ObjectMoved"
        * @param a_ObjectId The ID of the object
        * @param a_Position The current position
        * @param a_Rotation The current rotation (Euler angles)
        * @param a_LinearVelocity The linear velocity
        * @param a_AngularVelocity The angualar (rotation) velocity
        */
        virtual void onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity);

        /**
        * This function receives messages of type "MarbleMoved"
        * @param a_ObjectId The ID of the object
        * @param a_Position The current position
        * @param a_Rotation The current rotation (Euler angles)
        * @param a_LinearVelocity The linear velocity
        * @param a_AngularVelocity The angualar (rotation) velocity
        * @param a_CameraPosition The position of the camera
        * @param a_CameraUp The Up-Vector of the camera
        * @param a_ControlX The marble's current controller state in X-Direction
        * @param a_ControlY The marble's current controller state in Y-Direction
        * @param a_Contact A Flag indicating whether or not the marble is in contact with another object
        * @param a_ControlBrake Flag indicating whether or not the marble's brake is active
        * @param a_ControlRearView Flag indicating whether or not the marble's player looks behind
        * @param a_ControlRespawn Flag indicating whether or not the manual respawn button is pressed 
        */
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn);

        /**
        * This function receives messages of type "Trigger"
        * @param a_TriggerId ID of the trigger
        * @param a_ObjectId ID of the marble that caused the trigger
        */
        virtual void onTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId);

        /**
        * This function receives messages of type "PlayerRespawn"
        * @param a_MarbleId ID of the marble
        * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
        */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State);

        /**
        * This function receives messages of type "CameraRespawn"
        * @param a_MarbleId The ID of the marble which is respawning
        * @param a_Position The new position of the camera
        * @param a_Target The new target of the camera, i.e. the future position of the marble
        */
        virtual void onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Target);

        /**
        * This function receives messages of type "PlayerStunned"
        * @param a_MarbleId ID of the marble
        * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
        */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State);

        /**
        * This function receives messages of type "PlayerFinished"
        * @param a_MarbleId ID of the finished marble
        * @param a_RaceTime Racetime of the finished player in simulation steps
        * @param a_Laps The number of laps the player has done
        */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps);

        /**
        * This function receives messages of type "RaceFinished"
        * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
        */
        virtual void onRacefinished(irr::u8 a_Cancelled);

        /**
        * This function receives messages of type "Checkpoint"
        * @param a_MarbleId ID of the marble
        * @param a_Checkpoint The checkpoint ID the player has passed
        */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint);

        /**
        * This function receives messages of type "LapStart"
        * @param a_MarbleId ID of the marble
        * @param a_LapNo Number of the started lap
        */
        virtual void onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo);

        /**
        * This function receives messages of type "PauseChanged"
        * @param a_Paused The current paused state
        */
        virtual void onPausechanged(bool a_Paused);

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_Deficit Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader);

        /**
        * This function receives messages of type "FinishPosition"
        * @param a_Position The finish position of the marble
        * @param a_MarbleId ID of the marble
        * @param a_Deficit Deficit on the leader
        * @param a_Laps Number of laps done
        * @param a_Stunned Counter of the stuns of the marble during the race
        * @param a_Respawn Counter of the respawns of the marble during the race
        */
        virtual void onFinishposition(irr::s32 a_Position, irr::s32 a_MarbleId, irr::s32 a_Deficit, irr::s32 a_Laps, irr::s32 a_Stunned, irr::s32 a_Respawn) { }

        /**
        * The implementing object must implement this method. It is called when the thread start working
        */
        virtual void execute();

      public:
        CAiControlThread(threads::COutputQueue *a_pDynamicsOut, threads::CInputQueue *a_pDynamicsIn, scenenodes::CAiNode *a_pAiNode);
        virtual ~CAiControlThread();

        void addAiMarble(int a_iMarbleId, const std::string &a_sControls);
    };
  }
}
