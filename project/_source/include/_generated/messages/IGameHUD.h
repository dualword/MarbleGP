/** This file was created by the Dustbin::Games Message Generator Python Script **/

#pragma once

#include <irrlicht.h>
#include <messages/IMessage.h>
#include <threads/IQueue.h>
#include <string>

namespace dustbin {
  namespace messages {

    /**
     * @interface IGameHUD
     * @author Dustbin::Games Message Generator Python Script
     */
    class IGameHUD {
      protected:
        /**
         * This function receives messages of type "PlayerRespawn"
         * @param a_MarbleId ID of the marble
         * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
         */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) = 0;

        /**
         * This function receives messages of type "PlayerFinished"
         * @param a_MarbleId ID of the finished marble
         * @param a_RaceTime Racetime of the finished player in simulation steps
         * @param a_Laps The number of laps the player has done
         */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) = 0;

        /**
         * This function receives messages of type "PlayerStunned"
         * @param a_MarbleId ID of the marble
         * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
         */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) = 0;

        /**
         * This function receives messages of type "RaceFinished"
         * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
         */
        virtual void onRacefinished(irr::u8 a_Cancelled) = 0;

        /**
         * This function receives messages of type "RacePosition"
         * @param a_MarbleId ID of the marble
         * @param a_Position Position of the marble
         * @param a_Laps The current lap of the marble
         * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
         * @param a_DeficitLeader Deficit of the marble on the leader in steps
         */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) = 0;

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
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) = 0;

        /**
         * This function receives messages of type "LapStart"
         * @param a_MarbleId ID of the marble
         * @param a_LapNo Number of the started lap
         */
        virtual void onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) = 0;

        /**
         * This function receives messages of type "PlayerRostrum"
         * @param a_MarbleId ID of the marble
         */
        virtual void onPlayerrostrum(irr::s32 a_MarbleId) = 0;

        /**
         * This function receives messages of type "Countdown"
         * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
         */
        virtual void onCountdown(irr::u8 a_Tick) = 0;

        /**
         * This function receives messages of type "StepMsg"
         * @param a_StepNo The current step number
         */
        virtual void onStepmsg(irr::u32 a_StepNo) = 0;

        /**
         * This function receives messages of type "ConfirmWithdraw"
         * @param a_MarbleId ID of the marble
         * @param a_Timeout The number of steps defining the timeout for the withdrawal
         */
        virtual void onConfirmwithdraw(irr::s32 a_MarbleId, irr::s32 a_Timeout) = 0;


      public:
        IGameHUD();
        virtual ~IGameHUD();

        /**
         * This function handles any message
         * @param a_pMessage The message to handle
         * @param a_bDelete Should this message be deleted?
         * @return "true" if the message was handled, "false" otherwise
         */
        bool handleMessage(dustbin::messages::IMessage *a_pMessage, bool a_bDelete = true);
    };

  }  // namespace messages
}    // namespace dustbin

