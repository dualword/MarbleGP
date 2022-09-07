// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gameclasses/CGameLogicDefault.h>
#include <algorithm>

namespace dustbin {
  namespace gameclasses {
    CGameLogicDefault::CGameLogicDefault() : m_iPlayerCount(0), m_iStepNo(0), m_iLapCount(0), m_iLapNo(0), m_bRaceFinished(false) {
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
    * @param a_iPlayerId ID of the player
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
        if (m_aPlayers[l_iId].m_vLapCheckpoints.size() >= m_iLapCount || m_bRaceFinished) {
          m_aPlayers[l_iId].m_bFinished = true;
          m_bRaceFinished = true;
          return true;
        }
        else {
          if (m_aPlayers[l_iId].m_vLapCheckpoints.size() > 0) {
            int l_iLapTime = m_aPlayers[l_iId].m_vLapCheckpoints.back().back() - *m_aPlayers[l_iId].m_vLapCheckpoints.back().begin();
            if (m_aPlayers[l_iId].m_iFastest == 0 || l_iLapTime < m_aPlayers[l_iId].m_iFastest)
              m_aPlayers[l_iId].m_iFastest = l_iLapTime;
          }
          m_aPlayers[l_iId].m_vLapCheckpoints.push_back(std::vector<int>());

          if (m_iLapNo < m_aPlayers[l_iId].m_vLapCheckpoints.size())
            m_iLapNo = (int)m_aPlayers[l_iId].m_vLapCheckpoints.size();
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
        if (m_aPlayers[i].getLapNo() < m_iLapCount)
          return false;

      return true;
    }

    /**
    * Withdraw a player from the race
    * @param a_iMarble the marble ID
    * @param a_iStep the current simulation step
    */
    data::SRacePlayer *CGameLogicDefault::withdrawPlayer(int a_iMarble, int a_iStep) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_bWithdrawn = true;
        m_aPlayers[l_iId].m_iWithdrawn = a_iStep;

        return updatePositions(l_iId);
      }

      return nullptr;
    }

    int CGameLogicDefault::calculateLead(data::SRacePlayer* a_pAhead, data::SRacePlayer* a_pThis) {
      int l_iRet = a_pThis->m_vLapCheckpoints.back().back() - a_pAhead->m_vLapCheckpoints[a_pThis->m_vLapCheckpoints.size() - 1][a_pThis->m_vLapCheckpoints.back().size() - 1];
      return l_iRet > 0 ? l_iRet : 0;
    }

    /**
    * Update the positions in the race
    * @param a_iId the ID of the current player (0..15)
    * @see m_vPositions
    * @see CGameLogicDefault::withdrawPlayer
    * @see CGameLogicDefault::onCheckpoint
    */
    data::SRacePlayer *CGameLogicDefault::updatePositions(int a_iId) {
      std::sort(m_vPositions.begin(), m_vPositions.end(), [](data::SRacePlayer* p1, data::SRacePlayer* p2) {
        if (p1->m_bWithdrawn == true && p2->m_bWithdrawn == true) {
          return p1->m_iWithdrawn > p2->m_iWithdrawn;
        }
        else if (p1->m_bWithdrawn == true && p2-> m_bWithdrawn == false) {
          return false;
        }
        else if (p1->m_bWithdrawn == false && p2->m_bWithdrawn == true) {
          return true;
        }
        else {
          if (p1->m_vLapCheckpoints.size() == 0 && p2->m_vLapCheckpoints.size() == 0)
            return p1->m_iId < p2->m_iId;

          // The number of laps of the players differ
          // ==> the player with more passed laps is in front
          if (p1->m_vLapCheckpoints.size() != p2->m_vLapCheckpoints.size())
            return p1->m_vLapCheckpoints.size() > p2->m_vLapCheckpoints.size();
          // The number of checkpoints the players have passed differs
          // ==> the player with more passed checkpoints is in front
          else if (p1->m_vLapCheckpoints.back().size() != p2->m_vLapCheckpoints.back().size())
            return p1->m_vLapCheckpoints.back().size() > p2->m_vLapCheckpoints.back().size();
          else
            return p1->m_vLapCheckpoints.back().back() < p2->m_vLapCheckpoints.back().back(); // The player who has passed the checkpoint earlier is in front
        }
      });

      data::SRacePlayer *l_pLeader = *m_vPositions.begin();

      int l_iPos = 1;
      for (std::vector<data::SRacePlayer*>::iterator l_itPlayer = m_vPositions.begin(); l_itPlayer != m_vPositions.end(); l_itPlayer++) {
        (*l_itPlayer)->m_iPos = l_iPos++;

        if (l_itPlayer == m_vPositions.begin() || (*l_itPlayer)->m_vLapCheckpoints.size() == 0) {
          (*l_itPlayer)->m_iDeficitL = 0;
          (*l_itPlayer)->m_iDeficitA = 0;
        }
        else {
          data::SRacePlayer* l_pAhead[] = {
            l_pLeader,
            *(l_itPlayer - 1)
          };

          // Update the deficit times
          for (int i = 0; i < 2; i++) {
            if (l_pAhead[i]->m_vLapCheckpoints.size() == (*l_itPlayer)->m_vLapCheckpoints.size()) {
              int l_iDiff = calculateLead(l_pAhead[i], *l_itPlayer);
              // In the same lap
              if (i == 0) {
                (*l_itPlayer)->m_iDeficitL = l_iDiff;
              }
              else {
                (*l_itPlayer)->m_iDeficitA = l_iDiff;
              }
            }
            else {
              // Laps differ
              int l_iLapDiff = (int)(l_pAhead[i]->m_vLapCheckpoints.size() - (*l_itPlayer)->m_vLapCheckpoints.size());

              // The player ahead has not passed more checkpoints in his current lap than the current player,
              // i.e. on lap less of difference than calculated above
              if (l_pAhead[i]->m_vLapCheckpoints.back().size() < (*l_itPlayer)->m_vLapCheckpoints.back().size()) {
                l_iLapDiff--;
                // If lap diff == 0 then the player ahead has not yet lapped the current player
                if (l_iLapDiff == 0) {
                  int l_iDiff = calculateLead(l_pAhead[i], *l_itPlayer);;
                  if (i == 0)
                    (*l_itPlayer)->m_iDeficitL = l_iDiff;
                  else
                    (*l_itPlayer)->m_iDeficitA = l_iDiff;
                }
                else {
                  if (i == 0)
                    (*l_itPlayer)->m_iDeficitL = -l_iLapDiff;
                  else
                    (*l_itPlayer)->m_iDeficitA = -l_iLapDiff;
                }
              }
              // If the player and the marble ahead have in their respective laps passed the same amount
              // of checkpoints we need to check who has passed earlier, and if the player ahead passed
              // the last checkpoint later we decrement the lap diff by one and check then
              else if ((*l_itPlayer)->m_vLapCheckpoints.back().size() == l_pAhead[i]->m_vLapCheckpoints.back().size()) {
                if ((*l_itPlayer)->m_vLapCheckpoints.back().back() < l_pAhead[i]->m_vLapCheckpoints.back().back()) {
                  l_iLapDiff--;
                  // If lap diff == 0 then the player ahead has not yet lapped the current player
                  if (l_iLapDiff == 0) {
                    int l_iDiff = calculateLead(l_pAhead[i], *l_itPlayer);;
                    if (i == 0)
                      (*l_itPlayer)->m_iDeficitL = l_iDiff;
                    else
                      (*l_itPlayer)->m_iDeficitA = l_iDiff;
                  }
                  else {
                    if (i == 0)
                      (*l_itPlayer)->m_iDeficitL = -l_iLapDiff;
                    else
                      (*l_itPlayer)->m_iDeficitA = -l_iLapDiff;
                  }
                }
              }
              else {
                if (i == 0)
                  (*l_itPlayer)->m_iDeficitL = -l_iLapDiff;
                else
                  (*l_itPlayer)->m_iDeficitA = -l_iLapDiff;
              }
            }
          }
        }
      }

      return &m_aPlayers[a_iId];
    }

    /**
    * Callback for checkpoint passes of the marbles
    * @param a_iMarble the ID of the marble
    * @param a_iCheckpoint the checkpoint ID
    * @param a_iStep the current simulation step 
    */
    const std::vector<data::SRacePlayer *> &CGameLogicDefault::onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep) {
      int l_iId = a_iMarble - 10000;
      if (l_iId >= 0 && l_iId < m_iPlayerCount) {
        m_aPlayers[l_iId].m_vLapCheckpoints.back().push_back(a_iStep);
        updatePositions(l_iId);
      }

      return m_vPositions;
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