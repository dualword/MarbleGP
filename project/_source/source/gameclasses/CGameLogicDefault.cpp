// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gameclasses/CGameLogicDefault.h>

namespace dustbin {
  namespace gameclasses {
    CGameLogicDefault::CGameLogicDefault() : m_iPlayerCount(0), m_iStepNo(0), m_iLapCount(0) {
    }

    CGameLogicDefault::~CGameLogicDefault() {
    }

    /**
    * Set the number of laps
    * @param a_iLaps the number of laps
    */
    void CGameLogicDefault::setNumberOfLaps(int a_iLaps) {
      m_iLapCount = a_iLaps;
    }

    /**
    * Called once per step
    * @param a_iStep the simulation's step number
    */
    void CGameLogicDefault::onStep(int a_iStep) {
      m_iStepNo = a_iStep;
    }

    /**
    * Add a marble
    * @param a_iMarble the ID of the marble
    */
    void CGameLogicDefault::addMarble(int a_iMarble) {
      if (m_iPlayerCount < 16) {
        m_aPlayers[m_iPlayerCount].m_iId = a_iMarble;
        m_iPlayerCount++;
      }
    }

    /**
    * This method is called when a marble starts a new lap.
    * @param a_iMarble ID of the marble
    * @return "true" if the marble has finished the race, "false" otherwise
    */
    bool CGameLogicDefault::onLapStart(int a_iMarble) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_iLapNo++;

        if (m_aPlayers[l_iId].m_iLapNo > m_iLapCount)
          return true;
      }

      return false;
    }

    /**
    * Check wether or not all marbles have finished the race
    * @return true if all marbles have finished the race, false otherwise
    */
    bool CGameLogicDefault::raceFinished() {
      for (int i = 0; i < m_iPlayerCount; i++)
        if (m_aPlayers[i].m_iLapNo < m_iLapCount)
          return false;

      return true;
    }

    /**
    * Callback for checkpoint passes of the marbles
    * @param a_iMarble the ID of the marble
    * @param a_iCheckpoint the checkpoint ID
    */
    void CGameLogicDefault::onCheckpoint(int a_iMarble, int a_iCheckpoint) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_iCpCount++;
      }
    }

    /**
    * A method called when a marble is stunned
    * @param a_iMarble the marble ID
    */
    void CGameLogicDefault::onMarbleStunned(int a_iMarble) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_iStunned++;
      }
    }

    /**
    * Respawn callback for a marble's respawn
    * @param a_iMarble the marble ID
    */
    void CGameLogicDefault::onRespawn(int a_iMarble) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_iRespawn++;
      }
    }
  }
}