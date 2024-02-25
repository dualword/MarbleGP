// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <_generated/messages/IDynamicThread.h>
#include <gameclasses/ITriggerHandler.h>
#include <gameclasses/COdeNodes.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/SPlayer.h>
#include <data/CDataStructs.h>
#include <threads/IThread.h>
#include <vector>
#include <chrono>
#include <map>

namespace dustbin {
  namespace scenenodes {
    class CRostrumNode;
    class CStartingGridSceneNode;
  }

  namespace lua {
    class CLuaScript_physics;   /**< Forward declaration of the LUA physics script */
  }

  namespace gameclasses {

    /**
    * Foreward declaration of the CWorld object which
    * contains all physics objects and is defined in
    * the .cpp file
    */
    class CWorld;
    class IGameLogic;
    class CObjectMarble;
    class COfftrackDetector;

    /**
    * @class CDynamicThread
    * @author Christian Keimel
    * This class computes all the dynamics of the actual game
    */
    class CDynamicThread : public threads::IThread, public messages::IDynamicThread, public ITriggerHandler {
      private:
        enum class enGameState {
          Countdown,
          Racing,
          Finished,
          Cancelled
        };

        enGameState                       m_eGameState;
        data::SGameSettings::enAutoFinish m_eAutoFinish;

        CWorld        *m_pWorld;
        CObjectMarble *m_aMarbles[16];

        bool m_bPaused;

        irr::f32 m_fGridAngle;
        irr::s32 m_iPlayers;      /**< Number of app players (incl. AI) */
        irr::s32 m_iHuman;        /**< Number of all human (non-AI) players */

        std::chrono::high_resolution_clock::time_point m_cNextStep;

        IGameLogic *m_pGameLogic;

        scenenodes::CRostrumNode *m_pRostrumNode;

        lua::CLuaScript_physics *m_pLuaScript;
        std::string              m_sLuaError;

        int m_iCountDown;

        bool m_bNetworkClient;

        COfftrackDetector *m_pOfftrack;

        void createPhysicsObjects(irr::scene::ISceneNode* a_pNode);

        void run();

      protected:
        /**
         * This function receives messages of type "MarbleControl"
         * @param a_ObjectId The ID of the Marble
         * @param a_CtrlX The X Control (steer)
         * @param a_CtrlY The Y Control (throttle)
         * @param a_Brake Is the brake active?
         * @param a_RearView Does the player want to look back?
         * @param a_Respawn Is the manual respawn button pressed?
         */
        virtual void onMarblecontrol(irr::s32 a_ObjectId, irr::s8 a_CtrlX, irr::s8 a_CtrlY, bool a_Brake, bool a_RearView, bool a_Respawn) override;

        /**
         * This function receives messages of type "CancelRace"
         */
        virtual void onCancelrace() override;

        /**
         * This function receives messages of type "TogglePause"
         * @param a_MarbleID ID of the marble requesting the pause change
         */
        virtual void onTogglepause(irr::s32 a_MarbleID) override;

        /**
        * This function receives messages of type "PlayerWithdraw"
        * @param a_MarbleId ID of the marble
        */
        virtual void onPlayerwithdraw(irr::s32 a_MarbleId) override;

        /**
        * This function receives messages of type "PlayerRemoved"
        * @param a_playerid ID of the removed player
        */
        virtual void onPlayerremoved(irr::s32 a_playerid) override;

        /**
        * This function receives messages of type "ObjectMoved"
        * @param a_ObjectId The ID of the object
        * @param a_Position The current position
        * @param a_Rotation The current rotation (Euler angles)
        * @param a_LinearVelocity The linear velocity
        * @param a_AngularVelocity The angualar (rotation) velocity
        */
        virtual void onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity) override;

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
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) override;

        /**
        * This function receives messages of type "StepUpdate"
        * @param a_StepNo The step received from the server
        */
        virtual void onStepupdate(irr::s32 a_StepNo) override;

        /**
        * This function receives messages of type "JointSetAxis"
        * @param a_NodeId ID of the node the joint is attached to 
        * @param a_Axis New axis for the joint
        */
        virtual void onJointsetaxis(irr::s32 a_NodeId, const irr::core::vector3df &a_Axis) override;

        /**
        * This function receives messages of type "JointSetHiStop"
        * @param a_NodeId ID of the node the joint is attached to 
        * @param a_HiStop New value for high stop of the joint
        */
        virtual void onJointsethistop(irr::s32 a_NodeId, irr::f64 a_HiStop) override;

        /**
        * This function receives messages of type "JointSetLoStop"
        * @param a_NodeId ID of the node the joint is attached to 
        * @param a_LoStop New value for low stop of the joint
        */
        virtual void onJointsetlostop(irr::s32 a_NodeId, irr::f64 a_LoStop) override;

        /**
        * This function receives messages of type "JointStartMotor"
        * @param a_NodeId ID of the node the joint is attached to 
        * @param a_Speed Desired speed for the joint
        * @param a_Force Force used to get to the speed
        */
        virtual void onJointstartmotor(irr::s32 a_NodeId, irr::f64 a_Speed, irr::f64 a_Force) override;

        /**
        * This function receives messages of type "JointSetPosition"
        * @param a_NodeId ID of the node the joint is attached to 
        * @param a_Position New position of the joint
        */
        virtual void onJointsetposition(irr::s32 a_NodeId, irr::f64 a_Position) override;


        /**
        * This function receives messages of type "ServerDisconnect"
        */
        virtual void onServerdisconnect() override;

        /**
        * This function receives messages of type "PlayerRespawn"
        * @param a_MarbleId ID of the marble
        * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
        */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
        * This function receives messages of type "CameraRespawn"
        * @param a_MarbleId The ID of the marble which is respawning
        * @param a_Position The new position of the camera
        * @param a_Target The new target of the camera, i.e. the future position of the marble
        */
        virtual void onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Target) override;

        /**
        * This function receives messages of type "PlayerStunned"
        * @param a_MarbleId ID of the marble
        * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
        */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
        * This function receives messages of type "Checkpoint"
        * @param a_MarbleId ID of the marble
        * @param a_Checkpoint The checkpoint ID the player has passed
        */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint, irr::s32 a_StepNo) override;

        virtual void execute() override;

      public:
        CDynamicThread(bool a_bNetworkClient, COfftrackDetector *a_pOfftrack);

        virtual ~CDynamicThread();

        bool setupGame(
          scenenodes::CWorldNode *a_pWorld,
          scenenodes::CStartingGridSceneNode *a_pGrid,
          const std::vector<gameclasses::SPlayer *> &a_vPlayers, 
          int a_iLaps,
          const std::string &a_sLuaScript,
          data::SGameSettings::enAutoFinish a_eAutoFinish);

        /**
         * Callback to handle a trigger
         * @param a_iTrigger trigger id
         * @param a_iMarble Id of the marble
         * @param a_vPosition position of the marble that triggered
         * @param a_bBroadcast shall we send the trigger to the output queue?
         */
        virtual void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition, bool a_bBroadcast) override;

        /**
        * Callback to start respawn of a marble
        * @param a_iMarble Id of the marble to respawn
        */
        virtual void handleRespawn(int a_iMarble) override;

        /**
        * Callback for sending a "Checkpoint" message
        * @param a_iMarble Id of the marble
        * @param a_iCheckpoint Checkpoint id
        * @param a_iStepNo The simulation step when the checkpoint was triggered
        */
        virtual void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint, int a_iStepNo) override;

        /**
        * Callback for sending a "LapStart" message
        * @param a_iMarbleId Id of the marble
        * @param a_iLapNo Number of the started lap
        */
        virtual void handleLapStart(int a_iMarbleId, int a_iLapNo) override;

        /**
        * Callback for "Marble Touch" Triggers
        * @param a_iMarbleId the ID of the marble
        * @param a_iTouchId the ID of the touched trigger
        */
        virtual void handleMarbleTouch(int a_iMarbleId, int a_iTouchId) override;

        /**
        * LUA callback for finishing a player
        * @param a_iMarbleId the id of the marbles that has finished the race
        * @param a_iLaps the number of laps the marble has raced
        */
        void finishPlayer(int a_iMarbleId, int a_iRaceTime, int a_iLaps);

        /**
        * LUA callback for starting a player
        * @param a_iMarble the marble id
        */
        void startPlayer(int a_iMarble);

        /**
        * A function for the physics LUA script to send data to the AI and scene scripts
        * @param a_iNumberOne first number
        * @param a_iNumberTwo other number
        * @param a_sData string data
        */
        void sendMessageFromLUA(int a_iNumberOne, int a_iNumbetTwo, const std::string &a_sData);

        /**
        * Send a trigger message to the UI thread
        * @param a_iTriggerID ID of the trigger
        * @param a_iObjectID the object that has triggered
        */
        void sendTriggerToUI(int a_iTriggerID, int a_iObjectID);

        /**
        * Assign a player to a marble
        * @param a_pPlayer the player
        * @param a_pMarbleNode scene node of the marble
        */
        void assignPlayerToMarble(gameclasses::SPlayer *a_pPlayer, irr::scene::ISceneNode *a_pMarbleNode);

        /**
        * Retrieve the world of the race
        * @return the world of the race
        */
        CWorld* getWorld();

        /**
        * Get the LUA error
        * @return the LUA error
        */
        const std::string &getLuaError();

        /**
        * Send "joint set hi stop" message
        * @param a_iBody ID of the body the joint is attached to
        * @param a_fHiStop New hi-stop value
        */
        void setJointHiStop(irr::s32 a_iBody, irr::f64 a_fHiStop);

        /**
        * Send "joint set Lo stop" message
        * @param a_iBody ID of the body the joint is attached to
        * @param a_fLoStop New lo-stop value
        */
        void setJointLoStop(irr::s32 a_iBody, irr::f64 a_fLoStop);

        /**
        * Send "joint axis" message
        * @param a_iBody ID of the body the joint is attached to
        * @param a_cAxis the new axis of the joint
        */
        void setJointAxis(irr::s32 a_iBody, const irr::core::vector3df &a_cAxis);

        /**
        * Send the "joint start motor" message
        * @param a_iBody ID of the body the joint is attached to
        * @param a_fSpeed speed value for the motor
        * @param a_fForce force value for the motor
        * @param e_eJoint the joint type
        */
        void jointStartMotor(irr::s32 a_iBody, irr::f64 a_fSpeed, irr::f64 a_fForce, gameclasses::CObject::enJointType a_eJoint);

        /**
        * Send the "Joint set position" message
        * @param a_iBody ID of the body the joint is attached to
        * @param a_fPosition new position (slider joint) or angle (hinge joint)
        */
        void jointSetPosition(irr::s32 a_iBody, irr::f64 a_fPosition);
    };
  }
}