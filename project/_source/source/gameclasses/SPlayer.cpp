// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CSerializer64.h>
#include <gui/CButtonRenderer.h>
#include <gameclasses/SPlayer.h>
#include <platform/CPlatform.h>
#include <algorithm>

namespace dustbin {
  namespace gameclasses {
    // #define _DEBUG_DUMP_RANKING

    /**
    * The default contructor
    */
    SRaceData::SRaceData() : m_iPlayer(0), m_iMarble(0), m_iPosition(0), m_iDiffLeader(0), m_iDiffAhead(0), m_iState(0), m_iLapNo(0), m_pPlayer(nullptr), m_bWithdrawn(false) {

    }

    /**
    * Store the data in a JSON string
    * @return a JSON string
    */
    std::string SRaceData::toJSON() {
      std::string s = "{";

      s += "\"playerid\": "  + std::to_string(m_iPlayer) + ", ";
      s += "\"marbleid\": "  + std::to_string(m_iMarble) + ", ";
      s += "\"state\": "     + std::to_string(m_iState ) + ",";
      s += "\"withdrawn\": " + std::string(m_bWithdrawn ? "true" : "false") + ",";
      s += "\"laps\": [";

      for (std::vector<std::vector<int>>::iterator l_itLap = m_vLapCheckpoints.begin(); l_itLap != m_vLapCheckpoints.end(); l_itLap++) {
        if (l_itLap != m_vLapCheckpoints.begin())
          s += ", ";

        s += "[ ";
        for (std::vector<int>::iterator l_itCp = (*l_itLap).begin(); l_itCp != (*l_itLap).end(); l_itCp++) {
          if (l_itCp != (*l_itLap).begin())
            s += ", ";

          s += std::to_string(*l_itCp);
        }

        s += " ]";
      }

      s += "],";
      s += "\"respawn\": [";

      for (std::vector<int>::iterator l_itRespawn = m_vRespawn.begin(); l_itRespawn != m_vRespawn.end(); l_itRespawn++) {
        if (l_itRespawn != m_vRespawn.begin())
          s += ", ";

        s += std::to_string(*l_itRespawn);
      }

      s += "],";
      s += "\"stunned\": [";

      for (std::vector<int>::iterator l_itStun = m_vStunned.begin(); l_itStun != m_vStunned.end(); l_itStun++) {
        if (l_itStun != m_vStunned.begin())
          s += ", ";

        s += std::to_string(*l_itStun);
      }

      s += "]";
      return s + "}";
    }


    /**
    * The default constructor
    */
    SPlayer::SPlayer() :
      m_iPlayer        (0),
      m_sName          (""),
      m_sTexture       (""),
      m_sController    (""),
      m_sShortName     (""),
      m_sNumber        (L""),
      m_wsShortName    (L""),
      m_bShowRanking   (true),
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
    * Store the race data of this player to a JSON string
    * @return the JSON string
    */
    std::string SPlayer::toJSON() {
      std::string s = "{ ";

      s += "\"playerid\": "     + std::to_string(m_iPlayer) + ",";
      s += "\"name\": \""       + m_sName + "\",";
      s += "\"shortname\": \""  + m_sShortName + "\",";
      s += "\"texture\": \""    + m_sTexture + "\",";
      s += "\"controller\": \"" + m_sController + "\"";

      return s + " }";
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
      m_sName         (a_sName),
      m_sWName        (helpers::s2ws(a_sName)),
      m_sTexture      (a_sTexture),
      m_sController   (a_sController),
      m_sShortName    (a_sShortName),
      m_wsShortName   (helpers::s2ws(a_sShortName)),
      m_bShowRanking  (true),
      m_eType         (a_eType),
      m_eAiHelp       (a_eAiHelp),
      m_pMarble       (a_pMarble),
      m_pController   (nullptr)
    {
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
      if (m_pRaceData->m_vLapCheckpoints.size() == 0 && a_pOther->m_pRaceData->m_vLapCheckpoints.size() == 0)
        return m_pRaceData->m_iMarble < a_pOther->m_pRaceData->m_iMarble;
      
      // If the number of laps differs: more laps == in front
      if (m_pRaceData->m_vLapCheckpoints.size() != a_pOther->m_pRaceData->m_vLapCheckpoints.size())
        return m_pRaceData->m_vLapCheckpoints.size() > a_pOther->m_pRaceData->m_vLapCheckpoints.size();

      // Same number of laps: more checkpoints == in front
      if (m_pRaceData->m_vLapCheckpoints.back().size() != a_pOther->m_pRaceData->m_vLapCheckpoints.back().size())
        return m_pRaceData->m_vLapCheckpoints.back().size() > a_pOther->m_pRaceData->m_vLapCheckpoints.back().size();

      // Last option: the checkpoint was passed earlier
      return m_pRaceData->m_vLapCheckpoints.back().back() < a_pOther->m_pRaceData->m_vLapCheckpoints.back().back();
    }

    /**
    * Lap start callback
    */
    void SPlayer::onLapStart() {
      m_pRaceData->m_vLapCheckpoints.push_back(std::vector<int>());
    }
    /**
    * Some debugging: dump the lap checkpoints vector to stdout
    */
    void SPlayer::dumpLapCheckpoints() {
      printf("\nCheckpoints \"%s\"\n\n", m_sName.c_str());
      int i = 0;
      for (auto l_vLap : m_pRaceData->m_vLapCheckpoints) {
        printf("Lap %i (%i): ", i++, (int)l_vLap.size());

        for (auto l_iCp : l_vLap) {
          printf("%-5i", l_iCp);
          if (l_iCp != l_vLap.back())
            printf(", ");
        }

        printf("\n");
      }

      printf("\n");
    }

    /**
    * Checkpoint callback
    * @param a_iStep the step when the checkpoint was passed
    */
    void SPlayer::onCheckpoint(int a_iStep) {
      if (m_pRaceData->m_vLapCheckpoints.size() > 0) {
        m_pRaceData->m_vLapCheckpoints.back().push_back(a_iStep);
      }
    }

    void SPlayer::getDeficitTo(SPlayer* a_pOther, int& a_iSteps, int& a_iLaps) {
#ifdef _DEBUG_DUMP_RANKING
      printf("\n**** GetDeficit (%s -- %s) ****\n\n", m_sName.c_str(), a_pOther->m_sName.c_str());

      dumpLapCheckpoints();
      a_pOther->dumpLapCheckpoints();
#endif

      a_iSteps = 0;
      a_iLaps  = 0;

      int l_iLapIdx = (int)m_pRaceData->m_vLapCheckpoints.size();
    
#ifdef _DEBUG_DUMP_RANKING
      printf("Lap Index: %i\n", l_iLapIdx);
#endif

      if (l_iLapIdx > 0) {
        int l_iCkpIdx = (int)m_pRaceData->m_vLapCheckpoints.back().size();

#ifdef _DEBUG_DUMP_RANKING
        printf("Checkpoint Index: %i\n", l_iCkpIdx);
#endif

        if (l_iCkpIdx > 0 && a_pOther->m_pRaceData->m_vLapCheckpoints.size() >= l_iLapIdx && a_pOther->m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1].size() >= l_iCkpIdx) {
          a_iSteps = m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] - a_pOther->m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1];

#ifdef _DEBUG_DUMP_RANKING
          printf("Steps: %i\n", a_iSteps);
#endif
        }

        a_iLaps = (int)a_pOther->m_pRaceData->m_vLapCheckpoints.size() - l_iLapIdx;

#ifdef _DEBUG_DUMP_RANKING
        printf("Laps : %i\n", a_iLaps);
#endif

        if (a_iLaps > 0) {
          int l_iCpkIdx2 = (int)a_pOther->m_pRaceData->m_vLapCheckpoints.back().size();

          if (a_pOther->m_pRaceData->m_vLapCheckpoints.size() > l_iLapIdx) {
            if (m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1].size() > a_pOther->m_pRaceData->m_vLapCheckpoints.back().size()) {
#ifdef _DEBUG_DUMP_RANKING
              printf("\t\t1\n");
#endif
              a_iLaps--;
            }
            else if (
              m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1].size() == a_pOther->m_pRaceData->m_vLapCheckpoints.back().size() && 
              m_pRaceData->m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] < a_pOther->m_pRaceData->m_vLapCheckpoints.back()[l_iCkpIdx - 1]
            ) 
            {
#ifdef _DEBUG_DUMP_RANKING
              printf("\t\t2\n");
#endif
              a_iLaps--;
            }

#ifdef _DEBUG_DUMP_RANKING
            printf("Laps : %i\n\n", a_iLaps);
            printf("First size: %i\n", (int)m_vLapCheckpoints.size());
            printf("Other size: %i\n", (int)a_pOther->m_vLapCheckpoints.size());
            printf("LapIndex  : %i\n", l_iLapIdx);
            printf("Last Check: %i -- %i\n", (int)m_vLapCheckpoints[l_iLapIdx - 1].size(), (int)a_pOther->m_vLapCheckpoints.back().size());

            if (l_iCkpIdx > 0 && m_vLapCheckpoints[l_iLapIdx - 1].size() >= l_iCkpIdx && a_pOther->m_vLapCheckpoints.back().size() >= l_iCkpIdx)
              printf("Last Time : %i -- %i\n", m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1], a_pOther->m_vLapCheckpoints.back()[l_iCkpIdx - 1]);
#endif
          }
        }
      }

#ifdef _DEBUG_DUMP_RANKING
      printf("\n\n**** End get deficit ****\n");
#endif
      if (a_iSteps < 0)
        a_iSteps = 0;
    }

    /**
    * Set the player's marble node
    * @param a_pMarble to marble node
    */
    void SPlayer::setMarbleNode(SMarbleNodes* a_pMarble) {
      m_pMarble = a_pMarble;

      if (m_pMarble != nullptr) {
        m_pRaceData->m_iMarble = a_pMarble->m_pPositional->getID();
        m_pMarble->m_pRotational->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(m_sTexture));
      }
      else printf("Empty texture string.\n");
    }

    /**
    * A callback for a changed state
    * @param a_iState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
    * @param a_iStep step when the change happened
    */
    void SPlayer::onStateChanged(int a_iState, int a_iStep) {
      m_pRaceData->m_iState = a_iState;

      if (m_pRaceData->m_iState == 3) {
        if (m_pRaceData->m_vLapCheckpoints.size() > 0 && m_pRaceData->m_vLapCheckpoints.back().size() > 0) {
          m_pRaceData->m_vLapCheckpoints.back().back() = a_iStep;
          m_pRaceData->m_vRespawn.push_back(a_iStep);
        }
      }
      else if (a_iState == 1) {
        m_pRaceData->m_vStunned.push_back(a_iStep);
      }
    }

    SRace::SRace(const std::string &a_sTrack, int a_iLaps, STournament *a_pTournament) : m_sTrack(a_sTrack), m_iLaps(a_iLaps), m_pTournament(a_pTournament) {
    }

    /**
    * The destructor
    */
    SRace::~SRace() {
      m_vPlayers.clear();

      while (m_vRanking.size() > 0) {
        SRaceData *p = *m_vRanking.begin();
        m_vRanking.erase(m_vRanking.begin());
        delete p;
      }
    }

    /**
    * Serialize the race to a JSON string
    * @return a JSON string
    */
    std::string SRace::toJSON() {
      std::string s = "{";

      s += "\"track\": \"" + m_sTrack + "\", ";
      s += "\"laps\": " + std::to_string(m_iLaps) + ", ";
      s += "\"players\": [ ";

      for (std::vector<SPlayer*>::iterator l_itPlr = m_vPlayers.begin(); l_itPlr != m_vPlayers.end(); l_itPlr++) {
        if (l_itPlr != m_vPlayers.begin())
          s += ", ";

        s += (*l_itPlr)->m_pRaceData->toJSON();
      }


      s += " ], \"result\": [";

      for (std::vector<SRaceData *>::iterator l_itPlr = m_vRanking.begin(); l_itPlr != m_vRanking.end(); l_itPlr++) {
        if (l_itPlr != m_vRanking.begin())
          s += ",";
        
        s += std::to_string((*l_itPlr)->m_iPlayer);
      }

      s += "], \"standings\": [";

      for (std::vector<SStandings>::iterator l_itStand = m_vStandings.begin(); l_itStand != m_vStandings.end(); l_itStand++) {
        if (l_itStand != m_vStandings.begin())
          s += ",";

        s += (*l_itStand).toJSON();
      }

      return s + "] }";
    }

    /**
    * Update the ranking of the race
    * @see SRace::m_vRanking
    * @see SRace::onCheckpoint
    * @see SRace::onLapStart
    */
    void SRace::updateRanking() {
      std::sort(m_vRanking.begin(), m_vRanking.end(), [](SRaceData *r1, SRaceData *r2) {
        return r1->m_pPlayer->isInFront(r2->m_pPlayer);
      });

      int l_iPos  = 1;
      
      SPlayer *l_pLead = nullptr;


      for (std::vector<SRaceData *>::iterator l_itPlr = m_vRanking.begin(); l_itPlr != m_vRanking.end(); l_itPlr++) {
        (*l_itPlr)->m_iPosition = l_iPos++;

        if (l_itPlr == m_vRanking.begin()) {
          l_pLead = (*l_itPlr)->m_pPlayer;
          l_pLead->m_pRaceData->m_iDiffAhead  = 0;
          l_pLead->m_pRaceData->m_iDiffLeader = 0;
        }
        else {
          int l_iLaps = 0;
          int l_iStep = 0;

          (*l_itPlr)->m_pPlayer->getDeficitTo(l_pLead, l_iStep, l_iLaps);
          if (l_iLaps == 0)
            (*l_itPlr)->m_iDiffLeader = l_iStep;
          else
            (*l_itPlr)->m_iDiffLeader = -l_iLaps;

          (*l_itPlr)->m_pPlayer->getDeficitTo((*(l_itPlr - 1))->m_pPlayer, l_iStep, l_iLaps);
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
        if (l_pPlayer->m_pRaceData->m_iMarble == a_iMarble) {
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
        if (l_pPlayer->m_pRaceData->m_iMarble == a_iMarble) {
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
        if (l_pPlayer->m_pRaceData->m_iMarble == a_iMarble) {
          l_pPlayer->onStateChanged(a_iNewState, a_iStep);
          updateRanking();
        }
      }
    }

    /**
    * Finish the race, i.e. finish the current lap for all players
    */
    void SRace::finishRace() {
      printf("SRace::finishRace\n");
      updateRanking();

      SRaceData *l_pLeader = *m_vRanking.begin();
      l_pLeader->m_pPlayer->dumpLapCheckpoints();

      if (l_pLeader->m_vLapCheckpoints.size() > 0) {
        for (std::vector<SRaceData *>::iterator l_itPlr = m_vRanking.begin() + 1; l_itPlr != m_vRanking.end(); l_itPlr++) {
          if ((*l_itPlr)->m_pPlayer->m_pRaceData->m_iState != 4) {
            int l_iSteps = 0;
            int l_iLaps  = 0;

            (*l_itPlr)->m_pPlayer->getDeficitTo(l_pLeader->m_pPlayer, l_iSteps, l_iLaps);

            std::vector<std::vector<int>>::iterator l_itLeader = l_pLeader->m_vLapCheckpoints.end();

            do {
              l_itLeader--;
            }
            while ((*l_itLeader).size() <= 1 && l_itLeader != l_pLeader->m_vLapCheckpoints.begin());

            while ((*l_itPlr)->m_vLapCheckpoints.back().size() < (*l_itLeader).size()) {
              int l_iLeader = (*l_itLeader)[(*l_itPlr)->m_vLapCheckpoints.back().size()];
              int l_iPlayer = (*l_itPlr)->m_vLapCheckpoints.back().back();

              (*l_itPlr)->m_vLapCheckpoints.back().push_back(l_iLeader + l_iSteps);
            }

            (*l_itPlr)->m_vLapCheckpoints.push_back(std::vector<int>());
            (*l_itPlr)->m_vLapCheckpoints.back().push_back(l_pLeader->m_vLapCheckpoints.back().back() + l_iSteps);
          }
          (*l_itPlr)->m_pPlayer->dumpLapCheckpoints();
        }


      }
    }



    /**
    * The default constructor
    */
    SStandings::SStandings() : m_iPlayer(0), m_iRespawn(0), m_iStunned(0), m_iNoFinish(0) {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = 0;
    }

    /**
    * Constructor with the player ID
    * @param a_iPlayer the player ID
    */
    SStandings::SStandings(int a_iPlayer) : m_iPlayer(a_iPlayer), m_iRespawn(0), m_iStunned(0), m_iNoFinish(0), m_iScore(0), m_iBestPos(-1), m_iBestRace(-1) {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = 0;
    }

    /**
    * The copy constructor
    * @param a_cOther the data to be copied
    */
    SStandings::SStandings(const SStandings& a_cOther) : 
      m_iPlayer  (a_cOther.m_iPlayer  ),
      m_iRespawn (a_cOther.m_iRespawn ),
      m_iStunned (a_cOther.m_iStunned ),
      m_iNoFinish(a_cOther.m_iNoFinish),
      m_iScore   (a_cOther.m_iScore   ),
      m_iBestPos (a_cOther.m_iBestPos ),
      m_iBestRace(a_cOther.m_iBestRace)
    {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = a_cOther.m_aResult[i];
    }

    /**
    * Add a race result to the standings
    * @param a_pRace the race to add
    * @param a_iRace the race number
    */
    void SStandings::addRaceResult(SRace* a_pRace, int a_iRace) {
      for (auto l_pRaceData : a_pRace->m_vRanking) {
        if (l_pRaceData->m_pPlayer->m_iPlayer == m_iPlayer) {
          int l_iScoreTable[16][16] = {
            /*  1 player  */ {  0 },
            /*  2 players */ {  2,  0 },
            /*  3 players */ {  3,  1,  0 },
            /*  4 players */ {  4,  2,  1,  0 },
            /*  5 players */ {  5,  3,  2,  1,  0 },
            /*  6 players */ {  7,  4,  3,  2,  1,  0 },
            /*  7 players */ {  9,  6,  4,  3,  2,  1, 0 },
            /*  8 players */ { 10,  7,  5,  4,  3,  2, 1, 0 },
            /*  9 players */ { 10,  7,  6,  5,  4,  3, 2, 1, 0 },
            /* 10 players */ { 10,  8,  7,  6,  5,  4, 3, 2, 1, 0 },
            /* 11 players */ { 11,  9,  8,  7,  6,  5, 4, 3, 2, 1, 0 },
            /* 12 players */ { 13, 11, 10,  8,  7,  6, 5, 4, 3, 2, 1, 0 },
            /* 13 players */ { 15, 12, 10,  9,  8,  7, 6, 5, 4, 3, 2, 1, 0 },
            /* 14 players */ { 16, 13, 11, 10,  9,  8, 7, 6, 5, 4, 3, 2, 1, 0 },
            /* 15 players */ { 20, 16, 13, 11, 10,  9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
            /* 16 players */ { 25, 20, 16, 13, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 }
          };

          m_iScore   += l_iScoreTable[(int)a_pRace->m_vPlayers.size() - 1][l_pRaceData->m_iPosition - 1];
          m_iRespawn += (int)l_pRaceData->m_vRespawn.size();
          m_iStunned += (int)l_pRaceData->m_vStunned.size();

          if (l_pRaceData->m_pPlayer->m_pRaceData->m_bWithdrawn)
            m_iNoFinish++;

          if (m_iBestPos == -1 || l_pRaceData->m_iPosition < m_iBestPos) {
            m_iBestPos  = l_pRaceData->m_iPosition;
            m_iBestRace = a_iRace;
          }

          m_aResult[l_pRaceData->m_iPosition - 1]++;

          break;
        }
      }
    }

    /**
    * Compare the standing data to get the standings at a race 
    * @param a_cOther the standings to compare 
    */
    bool SStandings::isBetterThan(const SStandings& a_cOther) const {
      // The sorting criteria are:
      // 1. higher score
      // 2. less withdrawals
      // 3. less respawns
      // 4. less stuns
      // 5. better single race result
      // 6. best race result scored earlier
      // 7. (should never happen except before the first race): Player ID

      if (m_iScore    != a_cOther.m_iScore   ) return m_iScore    > a_cOther.m_iScore;
      if (m_iNoFinish != a_cOther.m_iNoFinish) return m_iNoFinish < a_cOther.m_iNoFinish;
      if (m_iRespawn  != a_cOther.m_iRespawn ) return m_iRespawn  < a_cOther.m_iRespawn;
      if (m_iStunned  != a_cOther.m_iStunned ) return m_iStunned  < a_cOther.m_iStunned;
      if (m_iBestPos  != a_cOther.m_iBestPos ) return m_iBestPos  < a_cOther.m_iBestPos;
      if (m_iBestRace != a_cOther.m_iBestRace) return m_iBestRace < a_cOther.m_iBestRace;

      return m_iPlayer < a_cOther.m_iPlayer;
    }

    /**
    * Encode as JSON string
    * @return JSON String
    */
    std::string SStandings::toJSON() {
      std::string s = "{";

      s += "\"playerid\":"  + std::to_string(m_iPlayer  ) + ",";
      s += "\"score\":"     + std::to_string(m_iScore   ) + ",";
      s += "\"respawn\":"   + std::to_string(m_iRespawn ) + ",";
      s += "\"stunned\":"   + std::to_string(m_iStunned ) + ",";
      s += "\"widhdrawn\":" + std::to_string(m_iNoFinish) + ",";
      s += "\"bestpos\":"   + std::to_string(m_iBestPos ) + ",";
      s += "\"bestrace\":"  + std::to_string(m_iBestRace) + ",";
      s += "\"results\": [";

      for (int i = 0; i < 16; i++) {
        if (i != 0)
          s += ",";
        
        s += std::to_string(m_aResult[i]);
      }

      return s + "] }";
    }


    /**
    * The standard contructor
    */
    STournament::STournament() : 
      m_eAutoFinish(data::SGameSettings::enAutoFinish::AllAndAi),
      m_eRaceClass (data::SGameSettings::enRaceClass ::AllClasses),
      m_iThisRace  (-1),
      m_bReverse   (false)
    {
    }

    /**
    * The destructor
    */
    STournament::~STournament() {
      while (m_vRaces.size() > 0) {
        SRace *p = *m_vRaces.begin();
        m_vRaces.erase(m_vRaces.begin());
        delete p;
      }

      while (m_vPlayers.size() > 0) {
        SPlayer *p = *m_vPlayers.begin();
        m_vPlayers.erase(m_vPlayers.begin());
        delete p;
      }
    }

    /**
    * Calculated the standings
    */
    void STournament::calculateStandings() {
      std::sort(m_vStandings.begin(), m_vStandings.end(), [](const SStandings& s1, const SStandings& s2) {
        return s1.isBetterThan(s2);
      });
    }

    /**
    * Start the race
    */
    void STournament::startRace() {
      SRace *l_pThisRace = nullptr;
      SRace *l_pLastRace = nullptr;

      if (m_iThisRace == -1) {
        if (m_vRaces.size() > 0) {
          l_pThisRace = m_vRaces.back();
        }

        if (m_vRaces.size() > 1) {
          l_pLastRace = *(m_vRaces.end() - 2);
        }
      }
      else if (m_iThisRace < m_vRaces.size()) {
        l_pThisRace = m_vRaces[m_iThisRace];

        if (m_iThisRace > 0) {
          l_pLastRace = m_vRaces[m_iThisRace - 1];
        }
      }

      if (l_pThisRace != nullptr) {
        int l_iPosition = 1;

        for (auto l_pPlayer : m_vPlayers) {
          SRaceData *l_pRaceData = new SRaceData();
          l_pRaceData->m_iPosition = l_iPosition;
          l_pRaceData->m_iPlayer   = l_pPlayer->m_iPlayer;
          l_pRaceData->m_pPlayer   = l_pPlayer;
          l_pPlayer->m_pRaceData   = l_pRaceData;

          l_pThisRace->m_vPlayers.push_back(l_pPlayer);
          l_pThisRace->m_vRanking.push_back(l_pRaceData);

          l_iPosition++;
        }

        std::sort(l_pThisRace->m_vRanking.begin(), l_pThisRace->m_vRanking.end(), [&](SRaceData *p1, SRaceData *p2) {
          if (l_pLastRace != nullptr) {
            for (auto l_pRaceData : l_pLastRace->m_vRanking) { 
              if (l_pRaceData->m_iPlayer == p1->m_iPlayer) return !m_bReverse;
              if (l_pRaceData->m_iPlayer == p2->m_iPlayer) return  m_bReverse;
            }
          }

          return std::wcstol(p1->m_pPlayer->m_sNumber.c_str(), nullptr, 10) < std::wcstol(p2->m_pPlayer->m_sNumber.c_str(), nullptr, 10);
        });
      }
    }

    /**
    * Get the current race
    * @return the current race
    */
    SRace* STournament::getRace() {
      if (m_iThisRace == -1 && m_vRaces.size() > 0)
        return m_vRaces.back();
      else if (m_iThisRace < m_vRaces.size())
        return m_vRaces[m_iThisRace];

      return nullptr;
    }

    /**
    * Finish the current race
    */
    void STournament::finishCurrentRace() {
      if (m_vStandings.size() == 0) {
        for (auto l_pPlayer : m_vPlayers) {
          m_vStandings.push_back(SStandings(l_pPlayer->m_iPlayer));
        }
      }

      SRace *l_pRace = nullptr;
      int l_iRaceIdx = 0;

      if (m_iThisRace == -1 && m_vRaces.size() > 0) {
        l_pRace = m_vRaces.back();
        l_iRaceIdx = (int)m_vRaces.size();
      }
      else if (m_iThisRace < m_vRaces.size()) {
        l_pRace = m_vRaces[m_iThisRace];
        l_iRaceIdx = m_iThisRace + 1;
      }

      if (l_pRace != nullptr) {
        l_pRace->finishRace();

        for (auto &l_cStanding : m_vStandings) {
          l_cStanding.addRaceResult(l_pRace, l_iRaceIdx);
        }

        calculateStandings();

        for (auto &l_cStanding : m_vStandings) {
          l_pRace->m_vStandings.push_back(SStandings(l_cStanding));
        }
      }
    }

    /**
    * Save the tournament standings to a JSON file
    */
    void STournament::saveToJSON() {
      std::wstring l_sFileName = platform::portableGetDataPath() + L"/tournament.json";

      std::string l_sJson = toJSON();

      irr::io::path l_sFilePath = irr::core::stringc(l_sFileName.c_str());

      irr::io::IWriteFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndWriteFile(l_sFilePath);

      if (l_pFile != nullptr) {
        l_pFile->write(l_sJson.c_str(), l_sJson.size());
        l_pFile->drop();
      }
    }


    /**
    * Convert the data to a JSON string
    * @return a JSON String
    */
    std::string STournament::toJSON() {
      std::string s = "{";

      s += "\"autofinish\": " + std::to_string((int)m_eAutoFinish   ) + ",";
      s += "\"raceclass\":"   + std::to_string((int)m_eRaceClass    ) + ",";
      s += "\"reversegrid\":" + std::string(m_bReverse ? "true" : "false") + ",";

      s += "\"players\": [";

      for (std::vector<SPlayer *>::iterator l_itPlr = m_vPlayers.begin(); l_itPlr != m_vPlayers.end(); l_itPlr++) {
        if (l_itPlr != m_vPlayers.begin())
          s += ",";

        s += (*l_itPlr)->toJSON();
      }

      s += "], ";

      s += "\"races\": [";

      for (std::vector<SRace *>::iterator l_itRace = m_vRaces.begin(); l_itRace != m_vRaces.end(); l_itRace++) {
        if (l_itRace != m_vRaces.begin())
          s += ",";

        s += (*l_itRace)->toJSON();
      }

      s += "], \"standings\": [";

      for (std::vector<SStandings>::iterator l_itStand = m_vStandings.begin(); l_itStand != m_vStandings.end(); l_itStand++) {
        if (l_itStand != m_vStandings.begin())
          s += ",";

        s += (*l_itStand).toJSON();
      }

      return s + "] }";
    }
  }
}