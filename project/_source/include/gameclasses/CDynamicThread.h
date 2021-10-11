// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <_generated/messages/IDynamicThread.h>
#include <gameclasses/ITriggerHandler.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/SPlayer.h>
#include <threads/IThread.h>
#include <lua.hpp>
#include <vector>
#include <chrono>

class CLuaScript_gamelogic;
class CLuaScript_physics;

namespace dustbin {
  namespace gameclasses {
    /**
    * Foreward declaration of the CWorld object which
    * contains all physics objects and is defined in
    * the .cpp file
    */
    class CWorld;
    class CObjectMarble;

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

        enGameState m_eGameState;

        CWorld* m_pWorld;
        CObjectMarble* m_aMarbles[16];

        int m_iWorldStep;

        bool m_bPaused;

        irr::f32 m_fGridAngle;

        std::chrono::high_resolution_clock::time_point m_cNextStep;

        CLuaScript_gamelogic* m_pGameLogic;
        CLuaScript_physics  * m_pPhysicsScript;

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
        virtual void onMarblecontrol(irr::s32 a_ObjectId, irr::s8 a_CtrlX, irr::s8 a_CtrlY, bool a_Brake, bool a_RearView, bool a_Respawn);

        /**
         * This function receives messages of type "CancelRace"
         */
        virtual void onCancelrace();

        /**
         * This function receives messages of type "TogglePause"
         */
        virtual void onTogglepause();

        virtual void execute();

      public:
        CDynamicThread(scenenodes::CWorldNode *a_pWorld, const std::vector<gameclasses::SPlayer*> &a_vPlayers, int a_iLaps, const std::string &a_sLuaTrackScript);

        virtual ~CDynamicThread();

        /**
         * Callback to handle a trigger
         * @param a_iTrigger trigger id
         * @param a_iMarble Id of the marble
         * @param a_vPosition position of the marble that triggered
         */
        virtual void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition);

        /**
        * Callback to start respawn of a marble
        * @param a_iMarble Id of the marble to respawn
        */
        virtual void handleRespawn(int a_iMarble);

        /**
        * Callback for sending a "Checkpoint" message
        * @param a_iMarble Id of the marble
        * @param a_iCheckpoint Checkpoint id
        */
        virtual void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint);

        /**
        * Callback for sending a "LapStart" message
        * @param a_iMarbleId Id of the marble
        * @param a_iLapNo Number of the started lap
        */
        virtual void handleLapStart(int a_iMarbleId, int a_iLapNo);

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
        * Retrieve the world of the race
        * @return the world of the race
        */
        CWorld* getWorld();
    };
  }
}