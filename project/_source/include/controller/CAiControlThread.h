// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/IAiController.h>
#include <controller/IControllerAI.h>
#include <data/CDataStructs.h>
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
    class CAiControlThread : public threads::IThread, public messages::IAiController {
      private:
        controller::IControllerAI *m_aControllers[17];    /**< The AI controllers, one more than possible to have a terminating "nullptr" */
        scenenodes::CAiNode       *m_pAiNode;
        threads::COutputQueue     *m_pDynamicsOut;
        threads::CInputQueue      *m_pDynamicsIn;

        data::SMarblePosition m_aMarbles[16];   /**< All necessary marble data */

        int m_iNumberOfBots;

      protected:
        /**
        * This function receives messages of type "StepMsg"
        * @param a_StepNo The current step number
        */
        virtual void onStepmsg(irr::u32 a_StepNo) override;

        /**
        * This function receives messages of type "ObjectMoved"
        * @param a_ObjectId The ID of the object
        * @param a_Position The current position
        * @param a_Rotation The current rotation (Euler angles)
        * @param a_LinearVelocity The linear velocity
        * @param a_AngularVelocity The angualar (rotation) velocity
        */
        virtual void onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity) override;

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
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) override;

        /**
        * This function receives messages of type "PlayerRespawn"
        * @param a_MarbleId ID of the marble
        * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
        */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
        * This function receives messages of type "Checkpoint"
        * @param a_MarbleId ID of the marble
        * @param a_Checkpoint The checkpoint ID the player has passed
        */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) override;

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
        * @param a_DeficitLeader Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) override;

        /**
        * This function receives messages of type "PlayerFinished"
        * @param a_MarbleId ID of the finished marble
        * @param a_RaceTime Racetime of the finished player in simulation steps
        * @param a_Laps The number of laps the player has done
        */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) override;

        /**
        * The implementing object must implement this method. It is called when the thread start working
        */
        virtual void execute() override;

      public:
        CAiControlThread(threads::COutputQueue *a_pDynamicsOut, threads::CInputQueue *a_pDynamicsIn, scenenodes::CAiNode *a_pAiNode);
        virtual ~CAiControlThread();

        void addAiMarble(int a_iMarbleId, const std::string &a_sControls);
    };
  }
}
