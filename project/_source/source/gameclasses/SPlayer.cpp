// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CSerializer64.h>
#include <gui/CButtonRenderer.h>
#include <gameclasses/SPlayer.h>
#include <platform/CPlatform.h>
#include <algorithm>

namespace dustbin {
  namespace gameclasses {
    // #define _DEBUG_DUMP_RANKING

    // Some defines for serilization
    const irr::s32 c_iPlayerStart       = -100;   /**< Start of the player data structure */
    const irr::s32 c_iPlayerId          = -101;   /**< ID of the player */
    const irr::s32 c_iPlayerMarble      = -102;   /**< The marble ID assigned to the player for this race */
    const irr::s32 c_iPlayerTimesStart  = -103;   /**< Start of the checkpoint times of the player */
    const irr::s32 c_iPlayerLapStart    = -104;   /**< Start of a new lap in the race time vector */
    const irr::s32 c_iPlayerTimesEnd    = -105;   /**< End of the checkpoint times of the player */
    const irr::s32 c_iPlayerRespawnStart = -106;   /**< Start of the list of a player's respawn */
    const irr::s32 c_iPlayerStunStart   = -107;   /**< Start of the list of a player's stuns */
    const irr::s32 c_iPlayerEnd         = -108;   /**< End of the player data structure */

    const irr::s32 c_iRaceStart         = -200;   /**< Start of the race data structure */
    const irr::s32 c_iRaceTrack         = -201;   /**< The race track */
    const irr::s32 c_iRaceLaps          = -202;   /**< The number of laps */
    const irr::s32 c_iRacePlayerStart   = -203;   /**< Start marker for the player list */
    const irr::s32 c_iRacePlayerEnd     = -204;   /**< End marker for the player list */
    const irr::s32 c_iRaceEnd           = -205;   /**< End of the race data structure */


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
    * Copy constructor
    * @param a_cPlayer the player to copy
    */
    SPlayer::SPlayer(const SPlayer& a_cPlayer) :
      m_iPlayer        (a_cPlayer.m_iPlayer),
      m_iId            (a_cPlayer.m_iId),
      m_iPosition      (a_cPlayer.m_iPosition),
      m_iLastPosUpdate (a_cPlayer.m_iLastPosUpdate),
      m_iDiffLeader    (a_cPlayer.m_iDiffLeader),
      m_iDiffAhead     (a_cPlayer.m_iDiffAhead),
      m_sName          (a_cPlayer.m_sName),
      m_sTexture       (a_cPlayer.m_sTexture),
      m_sController    (a_cPlayer.m_sController),
      m_sShortName     (a_cPlayer.m_sShortName),
      m_sNumber        (a_cPlayer.m_sNumber),
      m_wsShortName    (a_cPlayer.m_wsShortName),
      m_bWithdrawn     (a_cPlayer.m_bWithdrawn),
      m_bShowRanking   (a_cPlayer.m_bShowRanking),
      m_iState         (a_cPlayer.m_iState),
      m_iLapNo         (a_cPlayer.m_iLapNo),
      m_iLapCp         (a_cPlayer.m_iLapCp),
      m_iLastCp        (a_cPlayer.m_iLastCp),
      m_eType          (a_cPlayer.m_eType),
      m_eAiHelp        (a_cPlayer.m_eAiHelp),
      m_pMarble        (a_cPlayer.m_pMarble),
      m_pController    (a_cPlayer.m_pController)
    {

    }

    /**
    * The constructor using serialized data
    * @param a_sData serialized data
    */
    SPlayer::SPlayer(const std::string &a_sData) :
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

      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      if (l_cSerializer.getS32() == c_iPlayerStart) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iKey = l_cSerializer.getS32();

          switch (l_iKey) {
            case c_iPlayerId    : m_iPlayer = l_cSerializer.getS32(); break;
            case c_iPlayerMarble: m_iId     = l_cSerializer.getS32(); break;

            case c_iPlayerTimesStart: {
              irr::s32 l_iLapCount = l_cSerializer.getS32();

              printf("%i laps found for player.\n", l_iLapCount);

              for (irr::s32 i = 0; i < l_iLapCount; i++) {
                m_vLapCheckpoints.push_back(std::vector<int>());

                if (l_cSerializer.getS32() == c_iPlayerLapStart) {
                  irr::s32 l_iCpCount = l_cSerializer.getS32();

                  printf("  %i checkpoints found in lap %i\n", l_iCpCount, i);
                  for (irr::s32 j = 0; j < l_iCpCount; j++) {
                    m_vLapCheckpoints.back().push_back((int)l_cSerializer.getS32());
                  }
                }
                else {
                  printf("Invalid key .. \"Player Lap Start\" expected.\n");
                  break;
                }
              }

              if (l_cSerializer.getS32() != c_iPlayerTimesEnd) {
                printf("Invalid key .. \"Player Times End\" expected.\n");
              }

              break;
            }

            case c_iPlayerRespawnStart: {
              irr::s32 l_iCount = l_cSerializer.getS32();
              printf("%i Respawns found.\n", l_iCount);
              for (irr::s32 i = 0; i < l_iCount; i++)
                m_vRespawn.push_back((int)l_cSerializer.getS32());
              break;
            }

            case c_iPlayerStunStart: {
              irr::s32 l_iCount = l_cSerializer.getS32();
              printf("%i Respawns found.\n", l_iCount);
              for (irr::s32 i = 0; i < l_iCount; i++)
                m_vStunned.push_back((int)l_cSerializer.getS32());
              break;
            }

            case c_iPlayerEnd:
              printf("End marker found, exiting.\n");
              return;
          }
        }
      }
      else printf("Invalid header in \"SPlayer\" serialized data.\n");
    }

    /**
    * Serialize the race data of this player to a string
    * @return the serialized string
    */
    std::string SPlayer::serializeRaceData() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iPlayerStart);
      l_cSerializer.addS32(c_iPlayerId);
      l_cSerializer.addS32(m_iPlayer);
      l_cSerializer.addS32(c_iPlayerMarble);
      l_cSerializer.addS32(m_iId);

      l_cSerializer.addS32(c_iPlayerTimesStart);
      l_cSerializer.addS32((irr::s32)m_vLapCheckpoints.size());

      for (auto l_vLap : m_vLapCheckpoints) {
        l_cSerializer.addS32(c_iPlayerLapStart);
        l_cSerializer.addS32((irr::s32)l_vLap.size());

        for (auto l_iTime : l_vLap) {
          l_cSerializer.addS32((irr::s32)l_iTime);
        }
      }

      l_cSerializer.addS32(c_iPlayerTimesEnd);
      l_cSerializer.addS32(c_iPlayerRespawnStart);
      l_cSerializer.addS32((irr::s32)m_vRespawn.size());

      for (auto l_iRespawn: m_vRespawn)
        l_cSerializer.addS32((irr::s32)l_iRespawn);

      l_cSerializer.addS32(c_iPlayerStunStart);
      l_cSerializer.addS32((irr::s32)m_vStunned.size());

      for (auto l_iStunned: m_vStunned)
        l_cSerializer.addS32((irr::s32)l_iStunned);

      l_cSerializer.addS32(c_iPlayerEnd);

      return l_cSerializer.getMessageAsString();
    }

    /**
    * Store the race data of this player to a JSON string
    * @return the JSON string
    */
    std::string SPlayer::raceDataToJSON() {
      std::string s = "{ ";

      s += "\"playerid\": " + std::to_string(m_iPlayer) + ", ";
      s += "\"marbleid\": " + std::to_string(m_iId    ) + ", ";
      s += "\"laps\": [";
      
      for (std::vector<std::vector<int>>::iterator l_itLaps = m_vLapCheckpoints.begin(); l_itLaps != m_vLapCheckpoints.end(); l_itLaps++) {
        if (l_itLaps != m_vLapCheckpoints.begin())
          s += ", ";

        s += " [";

        for (std::vector<int>::iterator l_itCp = (*l_itLaps).begin(); l_itCp != (*l_itLaps).end(); l_itCp++) {
          if (l_itCp != (*l_itLaps).begin())
            s += ", ";

          s += std::to_string(*l_itCp);
        }

        s += "] ";
      }

      s += "], ";
      s += "\"respawn\": [";

      for (std::vector<int>::iterator l_itRsp = m_vRespawn.begin(); l_itRsp != m_vRespawn.end(); l_itRsp++) {
        if (l_itRsp != m_vRespawn.begin())
          s += ",";

        s += std::to_string(*l_itRsp);
      }

      s += "], ";
      s += "\"stunned\": [";

      for (std::vector<int>::iterator l_itStun = m_vStunned.begin(); l_itStun != m_vStunned.end(); l_itStun++) {
        if (l_itStun != m_vStunned.begin())
          s += ", ";

        s += std::to_string(*l_itStun);
      }

      s += "]";
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
    * Some debugging: dump the lap checkpoints vector to stdout
    */
    void SPlayer::dumpLapCheckpoints() {
      printf("\nCheckpoints \"%s\"\n\n", m_sName.c_str());
      int i = 0;
      for (auto l_vLap : m_vLapCheckpoints) {
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
      if (m_vLapCheckpoints.size() > 0) {
        m_vLapCheckpoints.back().push_back(a_iStep);
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

      int l_iLapIdx = (int)m_vLapCheckpoints.size();
    
#ifdef _DEBUG_DUMP_RANKING
      printf("Lap Index: %i\n", l_iLapIdx);
#endif

      if (l_iLapIdx > 0) {
        int l_iCkpIdx = (int)m_vLapCheckpoints.back().size();

#ifdef _DEBUG_DUMP_RANKING
        printf("Checkpoint Index: %i\n", l_iCkpIdx);
#endif

        if (l_iCkpIdx > 0 && a_pOther->m_vLapCheckpoints.size() >= l_iLapIdx && a_pOther->m_vLapCheckpoints[l_iLapIdx - 1].size() >= l_iCkpIdx) {
          a_iSteps = m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] - a_pOther->m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1];

#ifdef _DEBUG_DUMP_RANKING
          printf("Steps: %i\n", a_iSteps);
#endif
        }

        a_iLaps = (int)a_pOther->m_vLapCheckpoints.size() - l_iLapIdx;

#ifdef _DEBUG_DUMP_RANKING
        printf("Laps : %i\n", a_iLaps);
#endif

        if (a_iLaps > 0) {
          int l_iCpkIdx2 = (int)a_pOther->m_vLapCheckpoints.back().size();

          if (a_pOther->m_vLapCheckpoints.size() > l_iLapIdx) {
            if (m_vLapCheckpoints[l_iLapIdx - 1].size() > a_pOther->m_vLapCheckpoints.back().size()) {
#ifdef _DEBUG_DUMP_RANKING
              printf("\t\t1\n");
#endif
              a_iLaps--;
            }
            else if (
              m_vLapCheckpoints[l_iLapIdx - 1].size() == a_pOther->m_vLapCheckpoints.back().size() && 
              m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] < a_pOther->m_vLapCheckpoints.back()[l_iCkpIdx - 1]
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
    * A callback for a changed state
    * @param a_iState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
    * @param a_iStep step when the change happened
    */
    void SPlayer::onStateChanged(int a_iState, int a_iStep) {
      m_iState = a_iState;

      if (m_iState == 3) {
        if (m_vLapCheckpoints.size() > 0 && m_vLapCheckpoints.back().size() > 0) {
          m_vLapCheckpoints.back().back() = a_iStep;
          m_vRespawn.push_back(a_iStep);
        }
      }
      else if (a_iState == 1) {
        m_vStunned.push_back(a_iStep);
      }
    }

    SRace::SRace(const std::string& a_sTrack, int a_iLaps) : m_sTrack(a_sTrack), m_iLaps(a_iLaps), m_bOwnsPlayers(false) {
    }

    /**
    * The destructor
    */
    SRace::~SRace() {
      std::wstring l_sFileName = platform::portableGetDataPath() + L"/race.json";

      std::string l_sJson = toJSON();

      irr::io::path l_sFilePath = irr::core::stringc(l_sFileName.c_str());

      irr::io::IWriteFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndWriteFile(l_sFilePath);
      l_pFile->write(l_sJson.c_str(), l_sJson.size());
      l_pFile->drop();

      if (m_bOwnsPlayers) {
        for (auto l_pPlayer : m_vPlayers) {
          delete l_pPlayer;
        }

        m_vPlayers.clear();
        m_vRanking.clear();
      }
    }

    /**
    * Copy constructor
    * @param a_cRace the race to copy
    */
    SRace::SRace(const SRace& a_cRace) : m_sTrack(a_cRace.m_sTrack), m_iLaps(a_cRace.m_iLaps), m_bOwnsPlayers(true) {
      for (auto l_pPlayer : a_cRace.m_vPlayers) {
        SPlayer *p = new SPlayer(*l_pPlayer);
        m_vPlayers.push_back(p);
        m_vRanking.push_back(p);
      }

      updateRanking();
    }

    /**
    * Constructor with serialized data
    * @para a_sData serialized data
    */
    SRace::SRace(const std::string& a_sData) : m_sTrack(""), m_iLaps(0), m_bOwnsPlayers(true) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      if (l_cSerializer.getS32() == c_iRaceStart) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iToken = l_cSerializer.getS32();

          switch (l_iToken) {
            case c_iRaceStart: m_sTrack = l_cSerializer.getString(); break;
            case c_iRaceLaps : m_iLaps  = l_cSerializer.getS32   (); break;

            case c_iRacePlayerStart: {
              irr::s32 l_iCount = l_cSerializer.getS32();

              for (int i = 0; i < l_iCount; i++) {
                SPlayer *l_pPlayer = new SPlayer(l_cSerializer.getString());
                m_vPlayers.push_back(l_pPlayer);
                m_vRanking.push_back(l_pPlayer);
              }

              if (l_cSerializer.getS32() != c_iRacePlayerEnd)
                printf("SRace: unexpected token (Race Player End expected.");

              break;
            }
            case c_iRaceEnd:
              updateRanking();
              break;
          }
        }
      }
      else printf("SRace: Unexpected token (Race Start expected)\n");
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

        s += (*l_itPlr)->raceDataToJSON();
      }


      s += " ]";

      return s + "}";
    }

    /**
    * Serialize the race
    * @return serialized data of the race
    */
    std::string SRace::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iRaceStart);
      l_cSerializer.addS32(c_iRaceTrack);
      l_cSerializer.addString(m_sTrack);
      l_cSerializer.addS32(c_iRaceLaps);
      l_cSerializer.addS32(m_iLaps);
      l_cSerializer.addS32(c_iRacePlayerStart);
      l_cSerializer.addS32((irr::s32)m_vPlayers.size());

      for (auto l_pPlayer : m_vPlayers) {
        l_cSerializer.addString(l_pPlayer->serializeRaceData());
      }

      l_cSerializer.addS32(c_iRacePlayerEnd);
      l_cSerializer.addS32(c_iRaceEnd);

      return l_cSerializer.getMessageAsString();
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
        if (l_pPlayer->m_iId == a_iMarble) {
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

    /**
    * Finish the race, i.e. finish the current lap for all players
    */
    void SRace::finishRace() {
      printf("SRace::finishRace\n");
      updateRanking();

      SPlayer *l_pLeader = *m_vRanking.begin();
      l_pLeader->dumpLapCheckpoints();

      if (l_pLeader->m_vLapCheckpoints.size() > 0) {
        for (std::vector<SPlayer*>::iterator l_itPlr = m_vRanking.begin() + 1; l_itPlr != m_vRanking.end(); l_itPlr++) {
          if ((*l_itPlr)->m_iState != 4) {
            int l_iSteps = 0;
            int l_iLaps  = 0;

            (*l_itPlr)->getDeficitTo(l_pLeader, l_iSteps, l_iLaps);

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
          (*l_itPlr)->dumpLapCheckpoints();
        }
      }
    }
  }
}