// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CButtonRenderer.h>
#include <gameclasses/SPlayer.h>
#include <algorithm>

namespace dustbin {
  namespace gameclasses {
    /**
    * The default constructor
    */
    SPlayer::SPlayer() :
      m_iPlayer        (0),
      m_iId            (-1),
      m_iPosition      (99),
      m_iLastPosUpdate (0),
      m_iDiffLeader    (0),
      m_iDiffAhead     (0),
      m_sName          (""),
      m_sTexture       (""),
      m_sController    (""),
      m_sShortName     (""),
      m_sNumber        (L""),
      m_wsShortName    (L""),
      m_bWithdrawn     (false),
      m_bShowRanking   (true),
      m_iState         (0),
      m_iLapNo         (0),
      m_iLapCp         (0),
      m_iLastCp        (0),
      m_eType          (data::enPlayerType::Local),
      m_eAiHelp        (data::SPlayerData::enAiHelp::Off),
      m_pMarble        (nullptr),
      m_pController    (nullptr)
    {
      m_cText = irr::video::SColor(0xFF,    0,    0,    0);
      m_cBack = irr::video::SColor(0x80, 0xFF, 0xFF, 0xFF);
      m_cFrme = irr::video::SColor(0x80,    0,    0,    0);
    }

    /**
    * The destructor
    */
    SPlayer::~SPlayer() {
      if (m_pMarble != nullptr)
        delete m_pMarble;

      if (m_pController != nullptr)
        delete m_pController;
    }

    /**
    * The main constructor
    * @param a_iPlayer the player id
    * @param a_sName the name of the player
    * @param a_sTexture the texture string of the player's marble
    * @param a_sController the controller configuration string of the player
    * @param a_pMarble the marble of the player
    */
    SPlayer::SPlayer(int a_iPlayer, const std::string& a_sName, const std::string& a_sTexture, const std::string &a_sController, const std::string &a_sShortName, data::SPlayerData::enAiHelp a_eAiHelp, gameclasses::SMarbleNodes* a_pMarble, data::enPlayerType a_eType) :
      m_iPlayer       (a_iPlayer),
      m_iId            (-1),
      m_iPosition     (99),
      m_iLastPosUpdate(0),
      m_iDiffLeader   (0),
      m_iDiffAhead    (0),
      m_sName         (a_sName),
      m_sWName        (helpers::s2ws(a_sName)),
      m_sTexture      (a_sTexture),
      m_sController   (a_sController),
      m_sShortName    (a_sShortName),
      m_wsShortName   (helpers::s2ws(a_sShortName)),
      m_wsName        (helpers::s2ws(a_sName)),
      m_bWithdrawn    (false),
      m_bShowRanking  (true),
      m_iState        (0),
      m_iLapNo        (0),
      m_iLapCp        (0),
      m_iLastCp       (0),
      m_eType         (a_eType),
      m_eAiHelp       (a_eAiHelp),
      m_pMarble       (a_pMarble),
      m_pController   (nullptr)
    {

      if (m_pMarble != nullptr && m_pMarble->m_pPositional != nullptr)
        m_iId = m_pMarble->m_pPositional->getID();
      else
        m_iId = -1;

      if (m_pMarble != nullptr) {
        m_pMarble->m_pRotational->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(m_sTexture));
      }
      else printf("Empty texture string.\n");

      std::string l_sType = "";

      std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, m_sTexture);
      m_sNumber = helpers::s2ws(helpers::findTextureParameter(l_mParams, "number"));

      if (l_sType == "default") {
        l_mParams = helpers::parseParameters(l_sType, helpers::createDefaultTextureString(helpers::ws2s(m_sNumber), 0));
      }

      if (helpers::findTextureParameter(l_mParams, "numbercolor") != "") helpers::fillColorFromString(m_cText, helpers::findTextureParameter(l_mParams, "numbercolor"));
      if (helpers::findTextureParameter(l_mParams, "numberback" ) != "") helpers::fillColorFromString(m_cBack, helpers::findTextureParameter(l_mParams, "numberback" ));
      if (helpers::findTextureParameter(l_mParams, "ringcolor"  ) != "") helpers::fillColorFromString(m_cFrme, helpers::findTextureParameter(l_mParams, "ringcolor"  ));

      setName(m_sName);
    }

    void SPlayer::setName(const std::string& a_sName) {
      m_sName  =                       a_sName;
      m_sWName = helpers::s2ws(a_sName);
    }

    bool SPlayer::isBot() {
      return m_eAiHelp == data::SPlayerData::enAiHelp::BotMgp || m_eAiHelp == data::SPlayerData::enAiHelp::BotMb2 || m_eAiHelp == data::SPlayerData::enAiHelp::BotMb3;
    }

    /**
    * Is this player in front of another player?
    * @param a_pOther the other player
    * @return true if this player is in front
    */
    bool SPlayer::isInFront(SPlayer* a_pOther) {
      // At race start: the starting grid number (aka marble id) defines the positions
      if (m_vLapCheckpoints.size() == 0 && a_pOther->m_vLapCheckpoints.size() == 0)
        return m_iId < a_pOther->m_iId;
      
      // If the number of laps differs: more laps == in front
      if (m_vLapCheckpoints.size() != a_pOther->m_vLapCheckpoints.size())
        return m_vLapCheckpoints.size() > a_pOther->m_vLapCheckpoints.size();

      // Same number of laps: more checkpoints == in front
      if (m_vLapCheckpoints.back().size() != a_pOther->m_vLapCheckpoints.back().size())
        return m_vLapCheckpoints.back().size() > a_pOther->m_vLapCheckpoints.back().size();

      // Last option: the checkpoint was passed earlier
      return m_vLapCheckpoints.back().back() < a_pOther->m_vLapCheckpoints.back().back();
    }

    /**
    * Lap start callback
    */
    void SPlayer::onLapStart() {
      m_vLapCheckpoints.push_back(std::vector<int>());
    }

    /**
    * Checkpoint callback
    * @param a_iStep the step when the checkpoint was passed
    */
    void SPlayer::onCheckpoint(int a_iStep) {
      if (m_vLapCheckpoints.size() > 0) {
        m_vLapCheckpoints.back().push_back(a_iStep);
      }
    }

    void SPlayer::getDeficitTo(SPlayer* a_pOther, int& a_iSteps, int& a_iLaps) {
      a_iSteps = 0;
      a_iLaps  = 0;

      if (m_vLapCheckpoints.size() != 0 && a_pOther->m_vLapCheckpoints.size() != 0) {
        // Same number of laps
        if (m_vLapCheckpoints.size() == a_pOther->m_vLapCheckpoints.size()) {
          size_t l_iIndex = m_vLapCheckpoints.back().size() < a_pOther->m_vLapCheckpoints.back().size() ? m_vLapCheckpoints.back().size() - 1 : a_pOther->m_vLapCheckpoints.back().size() - 1;
          a_iSteps = m_vLapCheckpoints.back()[l_iIndex] - a_pOther->m_vLapCheckpoints.back()[l_iIndex];
        }
        else {
          size_t l_iLapIdx = m_vLapCheckpoints.size() < a_pOther->m_vLapCheckpoints.size() ? m_vLapCheckpoints.size() - 1 : a_pOther->m_vLapCheckpoints.size() - 1;
          size_t l_iChkIdx = m_vLapCheckpoints[l_iLapIdx].size() < a_pOther->m_vLapCheckpoints[l_iLapIdx].size() ? m_vLapCheckpoints[l_iLapIdx].size() - 1 : a_pOther->m_vLapCheckpoints[l_iLapIdx].size() - 1;

          a_iSteps = m_vLapCheckpoints[l_iLapIdx][l_iChkIdx] - a_pOther->m_vLapCheckpoints[l_iLapIdx][l_iChkIdx];

          a_iLaps = (int)(a_pOther->m_vLapCheckpoints.size() - m_vLapCheckpoints.size());

          // printf("==> %i, %i\n", (int)(m_vLapCheckpoints[l_iLapIdx].size() - a_pOther->m_vLapCheckpoints[l_iLapIdx].size()), a_iSteps);

          if (m_vLapCheckpoints[l_iLapIdx].size() >= a_pOther->m_vLapCheckpoints[l_iLapIdx].size())
            a_iLaps--;
          else if (a_iSteps < 0)
            a_iLaps--;
          else {
            if (m_vLapCheckpoints[l_iLapIdx][l_iChkIdx] < a_pOther->m_vLapCheckpoints[l_iLapIdx][l_iChkIdx])
              a_iLaps--;
            else {
              if (a_pOther->m_vLapCheckpoints.size() >= l_iLapIdx + 1 && a_pOther->m_vLapCheckpoints[l_iLapIdx + 1].size() > 0) {
                if (
                  a_pOther->m_vLapCheckpoints.back().size() == 0 || (
                  m_vLapCheckpoints[l_iLapIdx].size() >= a_pOther->m_vLapCheckpoints.back().size() &&
                  m_vLapCheckpoints[l_iLapIdx][a_pOther->m_vLapCheckpoints.back().size() - 1] < a_pOther->m_vLapCheckpoints.back()[a_pOther->m_vLapCheckpoints.back().size() - 1]
                ))
                  a_iLaps--;
              }
            }
          }
        }
      }
      if (a_iSteps < 0)
        a_iSteps = 0;
    }

    /**
    * A callback for a changed state
    * @param a_iState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
    * @param a_iStep step when the change happened
    */
    void SPlayer::onStateChanged(int a_iState, int a_iStep) {
      m_iState = a_iState;

      if (m_iState == 2 || m_iState == 3) {
        if (m_vLapCheckpoints.size() > 0 && m_vLapCheckpoints.back().size() > 0) {
          m_vLapCheckpoints.back().back() = a_iStep;
        }
      }
    }

    SRace::SRace(const std::string& a_sTrack, int a_iLaps) : m_sTrack(a_sTrack), m_iLaps(a_iLaps) {
    }

    /**
    * Update the ranking of the race
    * @see SRace::m_vRanking
    * @see SRace::onCheckpoint
    * @see SRace::onLapStart
    */
    void SRace::updateRanking() {
      std::sort(m_vRanking.begin(), m_vRanking.end(), [](SPlayer* p1, SPlayer* p2) {
        return p1->isInFront(p2);
      });

      int l_iPos  = 1;
      
      SPlayer *l_pLead = nullptr;


      for (std::vector<SPlayer*>::iterator l_itPlr = m_vRanking.begin(); l_itPlr != m_vRanking.end(); l_itPlr++) {
        (*l_itPlr)->m_iPosition = l_iPos++;

        if (l_itPlr == m_vRanking.begin()) {
          l_pLead = *l_itPlr;
          l_pLead->m_iDiffAhead  = 0;
          l_pLead->m_iDiffLeader = 0;
        }
        else {
          int l_iLaps = 0;
          int l_iStep = 0;

          (*l_itPlr)->getDeficitTo(l_pLead, l_iStep, l_iLaps);
          if (l_iLaps == 0)
            (*l_itPlr)->m_iDiffLeader = l_iStep;
          else
            (*l_itPlr)->m_iDiffLeader = -l_iLaps;

          (*l_itPlr)->getDeficitTo(*(l_itPlr - 1), l_iStep, l_iLaps);
          if (l_iLaps == 0)
            (*l_itPlr)->m_iDiffAhead = l_iStep;
          else
            (*l_itPlr)->m_iDiffAhead = -l_iLaps;
        }
      }

      /*printf("\n**** Update Ranking ****\n\n");

      for (auto l_pPlr : m_vRanking) {
        printf("%i: %s:\n", l_pPlr->m_iPosition, l_pPlr->m_sName.c_str());

        int i = 1;
        for (auto l_vCp : l_pPlr->m_vLapCheckpoints) {
          printf("\t\t%i: ", i++);
          for (auto l_iSt : l_vCp) {
            printf("%5i, ", l_iSt);
          }
          printf("\n");
        }
      }

      printf("\n**** Update Ranking ****\n\n");*/
    }

    /**
    * Callback for the checkpoint message
    * @param a_iMarble ID of the marble
    * @param a_iCheckpoint ID of the checkpoint
    * @param a_iStep the step when the checkpoint was passed
    */
    void SRace::onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep) {
      for (auto l_pPlayer : m_vPlayers) {
        if (l_pPlayer->m_iId == a_iMarble) {
          l_pPlayer->onCheckpoint(a_iStep);
          updateRanking();
        }
      }
    }

    /**
    * Callback for a lap start message
    * @param a_iMarble the marble
    */
    void SRace::onLapStart(int a_iMarble) {
      for (auto l_pPlayer : m_vPlayers) {
        if (l_pPlayer->m_iId == a_iMarble && l_pPlayer->m_vLapCheckpoints.size() < m_iLaps) {
          l_pPlayer->onLapStart();
        }
      }
    }

    /**
    * Callback for a state (normal, stunned, respawn, finished)
    * @param a_iMarble ID of the marble
    * @param a_iNewState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
    * @param a_iStep the step of the lap change
    */
    void SRace::onStateChange(int a_iMarble, int a_iNewState, int a_iStep) {
      for (auto l_pPlayer : m_vPlayers) {
        if (l_pPlayer->m_iId == a_iMarble) {
          l_pPlayer->onStateChanged(a_iNewState, a_iStep);
          updateRanking();
        }
      }
    }
  }
}