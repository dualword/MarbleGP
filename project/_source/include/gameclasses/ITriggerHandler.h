// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gameclasses {
    /**
    * @class ITriggerHandler
    * @author Christian Keimel
    * This interface needs to be implemented by a class
    * handling triggers from the game
    * @see CDynamicThread
    */
    class ITriggerHandler {
      public:
        /**
        * Callback to handle a trigger
        * @param a_iTrigger trigger id
        * @param a_iMarble Id of the marble 
        * @param a_vPosition position of the marble that triggered
        * @param a_bBroadcast shall we send the trigger to the output queue?
        */
        virtual void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition, bool a_bBroadcast) = 0;

        /**
        * Callback to start respawn of a marble
        * @param a_iMarble Id of the marble to respawn
        */
        virtual void handleRespawn(int a_iMarble) = 0;

        /**
        * Callback for sending a "Checkpoint" message
        * @param a_iMarble Id of the marble
        * @param a_iCheckpoint Checkpoint id
        * @param a_iStepNo The simulation step when the checkpoint was triggered
        */
        virtual void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint, int a_iStepNo) = 0;

        /**
        * Callback for sending a "LapStart" message
        * @param a_iMarbleId Id of the marble
        * @param a_iLapNo Number of the started lap
        */
        virtual void handleLapStart(int a_iMarbleId, int a_iLapNo) = 0;

        /**
        * Callback for "Marble Touch" Triggers
        * @param a_iMarbleId the ID of the marble
        * @param a_iTouchId the ID of the touched trigger
        */
        virtual void handleMarbleTouch(int a_iMarbleId, int a_iTouchId) = 0;
    };
  }
}