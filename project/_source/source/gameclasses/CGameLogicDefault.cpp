// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gameclasses/CGameLogicDefault.h>
#include <algorithm>

namespace dustbin {
  namespace gameclasses {
    CGameLogicDefault::CGameLogicDefault() : m_iPlayerCount(0), m_iStepNo(0), m_iLapCount(0), m_iCpPerLap(-1), m_iLapNo(0), m_bRaceFinished(false) {
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
        m_vPositions.push_back(&m_aPlayers[m_iPlayerCount]);
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

        if (m_iLapNo < m_aPlayers[l_iId].m_iLapNo)
          m_iLapNo = m_aPlayers[l_iId].m_iLapNo;

        // The first marble that completes a lap
        // shows us how many checkpoints a lap has
        if (m_iCpPerLap == -1 && m_aPlayers[l_iId].m_iLapNo > 1) {
          m_iCpPerLap = m_aPlayers[l_iId].m_iCpCount;
        }

        if (m_aPlayers[l_iId].m_iLapNo > m_iLapCount || m_bRaceFinished) {
          m_bRaceFinished = true;
          return true;
        }
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
    void CGameLogicDefault::onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        while (m_vCpTimes.size() <= m_aPlayers[l_iId].m_iCpCount) {
          m_vCpTimes.push_back(-1);
        }

        if (m_vCpTimes[m_aPlayers[l_iId].m_iCpCount] == -1) {
          m_vCpTimes[m_aPlayers[l_iId].m_iCpCount] = a_iStep;
          m_aPlayers[l_iId].m_iDeficit = 0;
        }
        else {
          m_aPlayers[l_iId].m_iDeficit = a_iStep - m_vCpTimes[m_aPlayers[l_iId].m_iCpCount];
        }

        m_aPlayers[l_iId].m_iCpCount++;

        std::sort(m_vPositions.begin(), m_vPositions.end(), [](data::SRacePlayer* p1, data::SRacePlayer* p2) {
          if (p1->m_iCpCount != p2->m_iCpCount)
            return p1->m_iCpCount > p2->m_iCpCount;
          else
            return p1->m_iLastCp < p2->m_iLastCp;
        });
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

    /**
    * Get the data of all the players in a race
    * @param a_iCount [out] the number of players
    * @return the array of the players
    */
    const data::SRacePlayer* CGameLogicDefault::getPlayersOfRace(int& a_iCount) {
      a_iCount = m_iPlayerCount;
      return m_aPlayers;
    }

    /**
    * Get the current race positions
    * @return the current race positions
    */
    const std::vector<data::SRacePlayer *> CGameLogicDefault::getRacePositions() {
      return m_vPositions;
    }
 }
}