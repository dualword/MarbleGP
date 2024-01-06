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
    const irr::s32 c_iRaceDataStart        = -100;   /**< Start of the player data structure */
    const irr::s32 c_iRaceDataPlayer       = -101;   /**< ID of the player */
    const irr::s32 c_iRaceDataMarble       = -102;   /**< The marble ID assigned to the player for this race */
    const irr::s32 c_iRaceDataTimesStart   = -103;   /**< Start of the checkpoint times of the player */
    const irr::s32 c_iRaceDataLapStart     = -104;   /**< Start of a new lap in the race time vector */
    const irr::s32 c_iRaceDataTimesEnd     = -105;   /**< End of the checkpoint times of the player */
    const irr::s32 c_iRaceDataRespawnStart = -106;   /**< Start of the list of a player's respawn */
    const irr::s32 c_iRaceDataStunStart    = -107;   /**< Start of the list of a player's stuns */
    const irr::s32 c_iRaceDataEnd          = -108;   /**< End of the player data structure */

    const irr::s32 c_iRaceStart         = -200;   /**< Start of the race data structure */
    const irr::s32 c_iRaceTrack         = -201;   /**< The race track */
    const irr::s32 c_iRaceLaps          = -202;   /**< The number of laps */
    const irr::s32 c_iRacePlayerStart   = -203;   /**< Start marker for the player list */
    const irr::s32 c_iRacePlayerEnd     = -204;   /**< End marker for the player list */
    const irr::s32 c_iRaceEnd           = -205;   /**< End of the race data structure */

    /**
    * The default contructor
    */
    SRaceData::SRaceData() : m_iPlayer(0), m_iMarble(0), m_iPosition(0), m_iDiffLeader(0), m_iDiffAhead(0) {

    }

    /**
    * The de-serialize constructor
    * @param a_sData serialized data
    */
    SRaceData::SRaceData(const std::string& a_sData) : m_iPlayer(0), m_iMarble(0), m_iPosition(0), m_iDiffLeader(0), m_iDiffAhead(0) {

    }

    /**
    * The copy constructor
    * @param a_cRace the race to copy
    */
    SRaceData::SRaceData(const SRaceData& a_cRace) : 
      m_iPlayer    (a_cRace.m_iPlayer), 
      m_iMarble    (a_cRace.m_iMarble), 
      m_iPosition  (a_cRace.m_iPosition), 
      m_iDiffLeader(a_cRace.m_iDiffLeader), 
      m_iDiffAhead (a_cRace.m_iDiffAhead) 
    {
      for (auto l_vLap : a_cRace.m_vLapCheckpoints) {
        m_vLapCheckpoints.push_back(std::vector<int>());

        for (auto l_iCp : l_vLap) {
          m_vLapCheckpoints.back().push_back(l_iCp);
        }
      }
    }

    /**
    * Serialize the data struct
    * @return the serialized data
    */
    std::string SRaceData::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iRaceDataStart);
      l_cSerializer.addS32(c_iRaceDataPlayer);
      l_cSerializer.addS32((irr::s32)m_iPlayer);
      l_cSerializer.addS32(c_iRaceDataMarble);
      l_cSerializer.addS32((irr::s32)m_iMarble);

      l_cSerializer.addS32(c_iRaceDataTimesStart);
      l_cSerializer.addS32((irr::s32)m_vLapCheckpoints.size());
      
      for (auto l_vLap : m_vLapCheckpoints) {
        l_cSerializer.addS32(c_iRaceDataLapStart);
        l_cSerializer.addS32((irr::s32)l_vLap.size());

        for (auto l_iCp : l_vLap) {
          l_cSerializer.addS32((irr::s32)l_iCp);
        }
      }

      l_cSerializer.addS32(c_iRaceDataTimesEnd);

      l_cSerializer.addS32(c_iRaceDataRespawnStart);
      l_cSerializer.addS32((irr::s32)m_vRespawn.size());

      for (auto l_iRespawn: m_vRespawn)
        l_cSerializer.addS32((irr::s32)l_iRespawn);

      l_cSerializer.addS32(c_iRaceDataStunStart);
      l_cSerializer.addS32((irr::s32)m_vStunned.size());

      for (auto l_iStunned: m_vStunned)
        l_cSerializer.addS32((irr::s32)l_iStunned);

      l_cSerializer.addS32(c_iRaceDataEnd);

      return l_cSerializer.getMessageAsString();
    }

    /**
    * Store the data in a JSON string
    * @return a JSON string
    */
    std::string SRaceData::toJSON() {
      std::string s = "{";

      s += "\"playerid\": " + std::to_string(m_iPlayer) + ", ";
      s += "\"marbleid\": " + std::to_string(m_iMarble) + ", ";
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
      m_bWithdrawn     (false),
      m_bShowRanking   (true),
      m_iState         (0),
      m_iLapNo         (0),
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
      m_eType          (a_cPlayer.m_eType),
      m_eAiHelp        (a_cPlayer.m_eAiHelp),
      m_pMarble        (a_cPlayer.m_pMarble),
      m_pController    (a_cPlayer.m_pController)
    {
      m_cRaceData = SRaceData(a_cPlayer.m_cRaceData);
    }

    /**
    * The constructor using serialized data
    * @param a_sData serialized data
    */
    SPlayer::SPlayer(const std::string &a_sData) :
      m_iPlayer        (0),
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
      m_eType          (data::enPlayerType::Local),
      m_eAiHelp        (data::SPlayerData::enAiHelp::Off),
      m_pMarble        (nullptr),
      m_pController    (nullptr)
    {
      m_cText = irr::video::SColor(0xFF,    0,    0,    0);
      m_cBack = irr::video::SColor(0x80, 0xFF, 0xFF, 0xFF);
      m_cFrme = irr::video::SColor(0x80,    0,    0,    0);

      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());
    }

    /**
    * Serialize the race data of this player to a string
    * @return the serialized string
    */
    std::string SPlayer::serialize() {
      messages::CSerializer64 l_cSerializer;

      return l_cSerializer.getMessageAsString();
    }

    /**
    * Store the race data of this player to a JSON string
    * @return the JSON string
    */
    std::string SPlayer::toJSON() {
      std::string s = "{ ";

      s += "\"playerid\": "   + std::to_string(m_iPlayer) + ",";
      s += "\"name\": "       + m_sName + ",";
      s += "\"texture\": "    + m_sTexture + ",";
      s += "\"controller\": " + m_sController + ",";

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
      m_bWithdrawn    (false),
      m_bShowRanking  (true),
      m_iState        (0),
      m_iLapNo        (0),
      m_eType         (a_eType),
      m_eAiHelp       (a_eAiHelp),
      m_pMarble       (a_pMarble),
      m_pController   (nullptr)
    {
      m_cRaceData.m_iPlayer = m_iPlayer;

      if (m_pMarble != nullptr && m_pMarble->m_pPositional != nullptr)
        m_cRaceData.m_iMarble = m_pMarble->m_pPositional->getID();
      else
        m_cRaceData.m_iMarble = -1;

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
      if (m_cRaceData.m_vLapCheckpoints.size() == 0 && a_pOther->m_cRaceData.m_vLapCheckpoints.size() == 0)
        return m_cRaceData.m_iMarble < a_pOther->m_cRaceData.m_iMarble;
      
      // If the number of laps differs: more laps == in front
      if (m_cRaceData.m_vLapCheckpoints.size() != a_pOther->m_cRaceData.m_vLapCheckpoints.size())
        return m_cRaceData.m_vLapCheckpoints.size() > a_pOther->m_cRaceData.m_vLapCheckpoints.size();

      // Same number of laps: more checkpoints == in front
      if (m_cRaceData.m_vLapCheckpoints.back().size() != a_pOther->m_cRaceData.m_vLapCheckpoints.back().size())
        return m_cRaceData.m_vLapCheckpoints.back().size() > a_pOther->m_cRaceData.m_vLapCheckpoints.back().size();

      // Last option: the checkpoint was passed earlier
      return m_cRaceData.m_vLapCheckpoints.back().back() < a_pOther->m_cRaceData.m_vLapCheckpoints.back().back();
    }

    /**
    * Lap start callback
    */
    void SPlayer::onLapStart() {
      m_cRaceData.m_vLapCheckpoints.push_back(std::vector<int>());
    }
    /**
    * Some debugging: dump the lap checkpoints vector to stdout
    */
    void SPlayer::dumpLapCheckpoints() {
      printf("\nCheckpoints \"%s\"\n\n", m_sName.c_str());
      int i = 0;
      for (auto l_vLap : m_cRaceData.m_vLapCheckpoints) {
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
      if (m_cRaceData.m_vLapCheckpoints.size() > 0) {
        m_cRaceData.m_vLapCheckpoints.back().push_back(a_iStep);
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

      int l_iLapIdx = (int)m_cRaceData.m_vLapCheckpoints.size();
    
#ifdef _DEBUG_DUMP_RANKING
      printf("Lap Index: %i\n", l_iLapIdx);
#endif

      if (l_iLapIdx > 0) {
        int l_iCkpIdx = (int)m_cRaceData.m_vLapCheckpoints.back().size();

#ifdef _DEBUG_DUMP_RANKING
        printf("Checkpoint Index: %i\n", l_iCkpIdx);
#endif

        if (l_iCkpIdx > 0 && a_pOther->m_cRaceData.m_vLapCheckpoints.size() >= l_iLapIdx && a_pOther->m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1].size() >= l_iCkpIdx) {
          a_iSteps = m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] - a_pOther->m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1];

#ifdef _DEBUG_DUMP_RANKING
          printf("Steps: %i\n", a_iSteps);
#endif
        }

        a_iLaps = (int)a_pOther->m_cRaceData.m_vLapCheckpoints.size() - l_iLapIdx;

#ifdef _DEBUG_DUMP_RANKING
        printf("Laps : %i\n", a_iLaps);
#endif

        if (a_iLaps > 0) {
          int l_iCpkIdx2 = (int)a_pOther->m_cRaceData.m_vLapCheckpoints.back().size();

          if (a_pOther->m_cRaceData.m_vLapCheckpoints.size() > l_iLapIdx) {
            if (m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1].size() > a_pOther->m_cRaceData.m_vLapCheckpoints.back().size()) {
#ifdef _DEBUG_DUMP_RANKING
              printf("\t\t1\n");
#endif
              a_iLaps--;
            }
            else if (
              m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1].size() == a_pOther->m_cRaceData.m_vLapCheckpoints.back().size() && 
              m_cRaceData.m_vLapCheckpoints[l_iLapIdx - 1][l_iCkpIdx - 1] < a_pOther->m_cRaceData.m_vLapCheckpoints.back()[l_iCkpIdx - 1]
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
        if (m_cRaceData.m_vLapCheckpoints.size() > 0 && m_cRaceData.m_vLapCheckpoints.back().size() > 0) {
          m_cRaceData.m_vLapCheckpoints.back().back() = a_iStep;
          m_cRaceData.m_vRespawn.push_back(a_iStep);
        }
      }
      else if (a_iState == 1) {
        m_cRaceData.m_vStunned.push_back(a_iStep);
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

        s += (*l_itPlr)->m_cRaceData.toJSON();
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
        l_cSerializer.addString(l_pPlayer->serialize());
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
        (*l_itPlr)->m_cRaceData.m_iPosition = l_iPos++;

        if (l_itPlr == m_vRanking.begin()) {
          l_pLead = *l_itPlr;
          l_pLead->m_cRaceData.m_iDiffAhead  = 0;
          l_pLead->m_cRaceData.m_iDiffLeader = 0;
        }
        else {
          int l_iLaps = 0;
          int l_iStep = 0;

          (*l_itPlr)->getDeficitTo(l_pLead, l_iStep, l_iLaps);
          if (l_iLaps == 0)
            (*l_itPlr)->m_cRaceData.m_iDiffLeader = l_iStep;
          else
            (*l_itPlr)->m_cRaceData.m_iDiffLeader = -l_iLaps;

          (*l_itPlr)->getDeficitTo(*(l_itPlr - 1), l_iStep, l_iLaps);
          if (l_iLaps == 0)
            (*l_itPlr)->m_cRaceData.m_iDiffAhead = l_iStep;
          else
            (*l_itPlr)->m_cRaceData.m_iDiffAhead = -l_iLaps;
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
        if (l_pPlayer->m_cRaceData.m_iMarble == a_iMarble) {
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
        if (l_pPlayer->m_cRaceData.m_iMarble == a_iMarble) {
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
        if (l_pPlayer->m_cRaceData.m_iMarble == a_iMarble) {
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

      if (l_pLeader->m_cRaceData.m_vLapCheckpoints.size() > 0) {
        for (std::vector<SPlayer*>::iterator l_itPlr = m_vRanking.begin() + 1; l_itPlr != m_vRanking.end(); l_itPlr++) {
          if ((*l_itPlr)->m_iState != 4) {
            int l_iSteps = 0;
            int l_iLaps  = 0;

            (*l_itPlr)->getDeficitTo(l_pLeader, l_iSteps, l_iLaps);

            std::vector<std::vector<int>>::iterator l_itLeader = l_pLeader->m_cRaceData.m_vLapCheckpoints.end();

            do {
              l_itLeader--;
            }
            while ((*l_itLeader).size() <= 1 && l_itLeader != l_pLeader->m_cRaceData.m_vLapCheckpoints.begin());

            while ((*l_itPlr)->m_cRaceData.m_vLapCheckpoints.back().size() < (*l_itLeader).size()) {
              int l_iLeader = (*l_itLeader)[(*l_itPlr)->m_cRaceData.m_vLapCheckpoints.back().size()];
              int l_iPlayer = (*l_itPlr)->m_cRaceData.m_vLapCheckpoints.back().back();

              (*l_itPlr)->m_cRaceData.m_vLapCheckpoints.back().push_back(l_iLeader + l_iSteps);
            }

            (*l_itPlr)->m_cRaceData.m_vLapCheckpoints.push_back(std::vector<int>());
            (*l_itPlr)->m_cRaceData.m_vLapCheckpoints.back().push_back(l_pLeader->m_cRaceData.m_vLapCheckpoints.back().back() + l_iSteps);
          }
          (*l_itPlr)->dumpLapCheckpoints();
        }
      }
    }



    /**
    * The default constructor
    */
    SStandings::SStandings() : m_iPlayer(0), m_iRespawn(0), m_iStunned(0), m_iNoFinish(0) {

    }

    /**
    * The copy constructor
    * @param a_cOther the data to be copied
    */
    SStandings::SStandings(const SStandings& a_cOther) : 
      m_iPlayer  (a_cOther.m_iPlayer  ),
      m_iRespawn (a_cOther.m_iRespawn ),
      m_iStunned (a_cOther.m_iStunned ),
      m_iNoFinish(a_cOther.m_iNoFinish)
    {
      for (auto l_iResult: a_cOther.m_vResults)
        m_vResults.push_back(l_iResult);
    }

    /**
    * Compare the standing data to get the standings at a race 
    * @param a_cOther the standings to compare 
    * @param a_iRace the race up to which the standings are to be calculated (-1 == all races)
    * @param a_iPlayers the number of players (important for the score table calculation)
    */
    bool SStandings::isBetterThan(const SStandings& a_cOther, int a_iRace, int a_iPlayers) {
      // The scores for each finishing position, depending on the number of players
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

      int l_iThisScore = 0;     // My score
      int l_iOthrScore = 0;     // Opponent's score

      int l_iThisBest = 66;     // My best race result
      int l_iOthrBest = 66;     // Opponent's best race result

      for (auto l_iResult : m_vResults) { l_iThisScore += l_iScoreTable[a_iPlayers - 1][l_iResult - 1]; if (l_iResult < l_iThisBest) l_iThisBest = l_iResult; }
      for (auto l_iResult : m_vResults) { l_iOthrScore += l_iScoreTable[a_iPlayers - 1][l_iResult - 1]; if (l_iResult < l_iOthrBest) l_iOthrBest = l_iResult; }

      if (l_iThisScore != l_iOthrScore) {
        // First criteria: the score
        return l_iThisScore > l_iOthrScore;
      }
      else if (m_iRespawn != a_cOther.m_iRespawn) {
        // Second criteria: less respawns
        return m_iRespawn < a_cOther.m_iRespawn;
      }
      else if (m_iStunned != a_cOther.m_iStunned) {
        // Third criteria: less stuns
        return m_iStunned < a_cOther.m_iStunned;
      }
      else if (m_iNoFinish != a_cOther.m_iNoFinish) {
        // Fourth criteria: less not finishes races
        return m_iNoFinish < a_cOther.m_iNoFinish;
      }
      else if (l_iThisBest != l_iOthrBest) {
        // Fifth criteria: Best Race Result
        return l_iThisBest < l_iOthrBest;
      }
      else {
        int l_iThisNum = 0;       // Index of my best race result
        int l_iOthrNum = 0;       // Index of opponent's best race result

        for (auto l_iResult : m_vResults) {
          if (l_iResult == l_iThisBest)
            break;
          else
            l_iThisNum++;
        }

        for (auto l_iResult : a_cOther.m_vResults) {
          if (l_iResult == l_iOthrBest)
            break;
          else
            l_iOthrNum++;
        }

        // Sixth criteria: best result scored earlier
        if (l_iThisNum != l_iOthrNum) {
          return l_iThisNum < l_iOthrNum;
        }
      }

      // Last criteria (should never happen except before the first race): Player ID
      return m_iPlayer < a_cOther.m_iPlayer;
    }


    /**
    * The standard contructor
    */
    STournament::STournament() : 
      m_eAutoFinish(data::SGameSettings::enAutoFinish::AllAndAi),
      m_eGridPos   (data::SGameSettings::enGridPos   ::LastRace),
      m_eRaceClass (data::SGameSettings::enRaceClass ::AllClasses)
    {
    }

    /**
    * The copy constructor
    * @param a_cOther the data to be copied
    */
    STournament::STournament(const STournament &a_cOther) :
      m_eAutoFinish(a_cOther.m_eAutoFinish),
      m_eGridPos   (a_cOther.m_eGridPos   ),
      m_eRaceClass (a_cOther.m_eRaceClass )
    {
      for (auto l_cPlayer : a_cOther.m_vPlayers) {
        m_vPlayers.push_back(SPlayer(l_cPlayer));
      }

      for (auto l_cRace : a_cOther.m_vRaces) {
        m_vRaces.push_back(SRace(l_cRace));
      }

      calculateStandings();
    }

    /**
    * The de-serialization constructor
    * @param a_sData the data to de-serialize
    */
    STournament::STournament(const std::string &a_sData) :
      m_eAutoFinish(data::SGameSettings::enAutoFinish::AllAndAi),
      m_eGridPos   (data::SGameSettings::enGridPos   ::LastRace),
      m_eRaceClass (data::SGameSettings::enRaceClass ::AllClasses)
    {
    }

    /**
    * Calculated the standings
    */
    void STournament::calculateStandings() {
    }

    /**
    * Serialize the tournament
    * @return the serialized data
    */
    std::string STournament::serialize() {
      messages::CSerializer64 l_cSerializer;

      return l_cSerializer.getMessageAsString();
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

      s += "\"autofinish\": " + std::to_string((int)m_eAutoFinish) + ",";
      s += "\"gridpos\": "    + std::to_string((int)m_eGridPos   ) + ",";
      s += "\"raceclass\":"   + std::to_string((int)m_eRaceClass ) + ",";

      s += "\"players\": [";

      for (auto l_cPlayer : m_vPlayers)
        s += l_cPlayer.toJSON();

      s += "], ";

      s += "\"races\": [";

      for (auto l_cRace : m_vRaces)
        s += l_cRace.toJSON();

      return s + "] }";
    }
  }
}