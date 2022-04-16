// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <vector>

namespace dustbin {
  namespace gameclasses {
    /**
    * @class IGameLogic
    * @author Christian Keimel
    * This interface is used to provide some logic for the games.
    * I have decided to make this an interface to keep some options
    * for other game modes open
    */
    class IGameLogic {
      public:
        virtual ~IGameLogic() { }

        /**
        * Set the number of laps
        * @param a_iLaps the number of laps
        */
        virtual void setNumberOfLaps(int a_iLaps) = 0;

        /**
        * Called once per step
        * @param a_iStep the simulation's step number
        */
        virtual void onStep(int a_iStep) = 0;

        /**
        * Add a marble
        * @param a_iMarble the ID of the marble
        * @param a_iPlayerId ID of the player
        */
        virtual void addMarble(int a_iMarble) = 0;

        /**
        * This method is called when a marble starts a new lap.
        * @param a_iMarble ID of the marble
        * @return "true" if the marble has finished the race, "false" otherwise
        */
        virtual bool onLapStart(int a_iMarble) = 0;

        /**
        * Check wether or not all marbles have finished the race
        * @return true if all marbles have finished the race, false otherwise
        */
        virtual bool raceFinished() = 0;

        /**
        * Callback for checkpoint passes of the marbles
        * @param a_iMarble the ID of the marble
        * @param a_iCheckpoint the checkpoint ID
        * @param a_iStep the current step
        */
        virtual data::SRacePlayer *onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep) = 0;

        /**
        * A method called when a marble is stunned
        * @param a_iMarble the marble ID
        */
        virtual void onMarbleStunned(int a_iMarble) = 0;

        /**
        * Respawn callback for a marble's respawn
        * @param a_iMarble the marble ID
        */
        virtual void onRespawn(int a_iMarble) = 0;

        /**
        * Get the current race positions
        * @return the current race positions
        */
        virtual const std::vector<data::SRacePlayer *> getRacePositions() = 0;
    };
  }
}
