// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gameclasses/IGameLogic.h>

namespace dustbin {
  namespace gameclasses {
    /**
    * @class CGameLogicDefault
    * @author Christian Keimel
    * This is the default game logic for standard races
    */
    class CGameLogicDefault : public IGameLogic {
      private:
        struct SPlayer {
          int m_iId,        /**< Marble ID */
              m_iCpCount,   /**< Number of passed checkpoints*/
              m_iStunned,   /**< Stunned counter */
              m_iRespawn,   /**< Respawn counter */
              m_iLapNo;     /**< The current lap */

          SPlayer() : m_iId(-1), m_iCpCount(0), m_iStunned(0), m_iRespawn(0), m_iLapNo(0) {
          }
        };

        SPlayer m_aPlayers[16];   /**< Array with player data */
        int     m_iPlayerCount,   /**< Number of players */
                m_iStepNo,        /**< The current simulation step */
                m_iLapCount;      /**< The number of laps */

      public:
        CGameLogicDefault();
        virtual ~CGameLogicDefault();

        /**
        * Set the number of laps
        * @param a_iLaps the number of laps
        */
        virtual void setNumberOfLaps(int a_iLaps);

        /**
        * Called once per step
        * @param a_iStep the simulation's step number
        */
        virtual void onStep(int a_iStep);

        /**
        * Add a marble
        * @param a_iMarble the ID of the marble
        */
        virtual void addMarble(int a_iMarble);

        /**
        * This method is called when a marble starts a new lap.
        * @param a_iMarble ID of the marble
        * @return "true" if the marble has finished the race, "false" otherwise
        */
        virtual bool onLapStart(int a_iMarble);

        /**
        * Check wether or not all marbles have finished the race
        * @return true if all marbles have finished the race, false otherwise
        */
        virtual bool raceFinished();

        /**
        * Callback for checkpoint passes of the marbles
        * @param a_iMarble the ID of the marble
        * @param a_iCheckpoint the checkpoint ID
        */
        virtual void onCheckpoint(int a_iMarble, int a_iCheckpoint);

        /**
        * A method called when a marble is stunned
        * @param a_iMarble the marble ID
        */
        virtual void onMarbleStunned(int a_iMarble);

        /**
        * Respawn callback for a marble's respawn
        * @param a_iMarble the marble ID
        */
        virtual void onRespawn(int a_iMarble);
    };
  }
}

