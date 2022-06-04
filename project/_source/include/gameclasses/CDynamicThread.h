// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <_generated/messages/IDynamicThread.h>
#include <scenenodes/CTriggerTimerNode.h>
#include <gameclasses/ITriggerHandler.h>
#include <gameclasses/CMarbleCounter.h>
#include <scenenodes/STriggerAction.h>
#include <scenenodes/STriggerVector.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/SPlayer.h>
#include <threads/IThread.h>
#include <vector>
#include <chrono>
#include <map>

namespace dustbin {
  namespace scenenodes {
    class CRostrumNode;
    class CStartingGridSceneNode;
  }

  namespace gameclasses {
    /**
    * Foreward declaration of the CWorld object which
    * contains all physics objects and is defined in
    * the .cpp file
    */
    class CWorld;
    class CObjectMarble;
    class IGameLogic;

    /**
    * @class CDynamicThread
    * @author Christian Keimel
    * This class computes all the dynamics of the actual game
    */
    class CDynamicThread : public threads::IThread, public messages::IDynamicThread, public ITriggerHandler {
      public:
        enum class enAutoFinish {
          AllPlayers,
          SecondToLast,
          FirstPlayer,
          PlayersAndAI
        };

      private:
        enum class enGameState {
          Countdown,
          Racing,
          Finished,
          Cancelled
        };

        enGameState  m_eGameState;
        enAutoFinish m_eAutoFinish;

        CWorld        *m_pWorld;
        CObjectMarble *m_aMarbles[16];

        bool m_bPaused;

        irr::f32 m_fGridAngle;
        irr::s32 m_iPlayers;      /**< Number of app players (incl. AI) */
        irr::s32 m_iHuman;        /**< Number of all human (non-AI) players */

        std::chrono::high_resolution_clock::time_point m_cNextStep;

        IGameLogic *m_pGameLogic;

        scenenodes::CRostrumNode *m_pRostrumNode;

        std::vector<scenenodes::STriggerVector> m_vTimerActions;
        std::vector<gameclasses::CMarbleCounter> m_vMarbleCounters; /**< A list of marble counters */

        std::map<int, std::vector<scenenodes::STriggerAction>> m_mTouchMap;    /**< A map of the trigger actions for marble touch actions */

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
         */
        virtual void onTogglepause() override;

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

        virtual void execute() override;

      public:
        CDynamicThread();

        virtual ~CDynamicThread();

        void setupGame(
          scenenodes::CWorldNode *a_pWorld,
          scenenodes::CStartingGridSceneNode *a_pGrid,
          const std::vector<data::SPlayerData> &a_vPlayers, 
          int a_iLaps, 
          std::vector<scenenodes::STriggerVector> a_vTimerActions, 
          std::vector<gameclasses::CMarbleCounter> a_vMarbleCounters, 
          enAutoFinish a_eAutoFinish);

        /**
         * Callback to handle a trigger
         * @param a_iTrigger trigger id
         * @param a_iMarble Id of the marble
         * @param a_vPosition position of the marble that triggered
         */
        virtual void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition) override;

        /**
        * Callback to start respawn of a marble
        * @param a_iMarble Id of the marble to respawn
        */
        virtual void handleRespawn(int a_iMarble) override;

        /**
        * Callback for sending a "Checkpoint" message
        * @param a_iMarble Id of the marble
        * @param a_iCheckpoint Checkpoint id
        */
        virtual void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint) override;

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
        * Get the race result
        * @return the race result
        */
        // const std::vector<data::SRacePlayer *> getRaceResult();

        /**
        * Retrieve the world of the race
        * @return the world of the race
        */
        CWorld* getWorld();
    };
  }
}