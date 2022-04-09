// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gameclasses/IGameLogic.h>
#include <data/CDataStructs.h>

namespace dustbin {
  namespace gameclasses {
    /**
    * @class CGameLogicDefault
    * @author Christian Keimel
    * This is the default game logic for standard races
    */
    class CGameLogicDefault : public IGameLogic {
      public:

      private:
        data::SRacePlayer m_aPlayers[16];   /**< Array with player data */
        int               m_iPlayerCount;   /**< Number of players */
        int               m_iStepNo;        /**< The current simulation step */
        int               m_iLapCount;      /**< The number of laps */
        int               m_iCpPerLap;      /**< Checkpoints per lap */
        int               m_iLapNo;         /**< The current lap (defined by the leader) */

        bool m_bRaceFinished;   /**< Will turn true once the first player has finished, thereafter all players will have finished when ending their lap */

        std::vector<data::SRacePlayer *> m_vPositions;  /**< This vector is used to keep track of the positions in a race */
        std::vector<int                > m_vCpTimes;    /**< Time when the leader has passed a checkpoint of the race */

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
        * @param a_iStep the current step
        */
        virtual void onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep);

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

        /**
        * Get the data of all the players in a race
        * @param a_iCount [out] the number of players
        * @return the array of the players
        */
        virtual const data::SRacePlayer *getPlayersOfRace(int &a_iCount);

        /**
        * Get the current race positions
        * @return the current race positions
        */
        virtual const std::vector<data::SRacePlayer *> getRacePositions();
    };
  }
}

