// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CMessageHelpers.h>
#include <messages/CSerializer64.h>
#include <data/CDataStructs.h>
#include <Defines.h>

namespace dustbin {
  namespace data {
    // The message IDs for (de)serialization of the
    // player data structures
    const irr::s32 c_iPlayerDataHead = 23;  /**< Marker for the player data to start */
    const irr::s32 c_iPlayerType     = 24;  /**< Marker for the type of player */
    const irr::s32 c_iPlayerName     = 25;  /**< Marker for the player name */
    const irr::s32 c_iPlayerTexture  = 26;  /**< Marker for the player texture definition string */
    const irr::s32 c_iPlayerControls = 27;  /**< Marker for the player controller setup string */
    const irr::s32 c_iPlayerAiHelp   = 28;  /**< Marker for the level of AI help for the player */
    const irr::s32 c_iPlayerGridPos  = 29;  /**< Marker for the position in the grid */
    const irr::s32 c_iPlayerDataEnd  = 30;  /**< Marker for the player data to end */

    const irr::s32 c_iGameSettings    = 32;   /**< Marker for the game settings */
    const irr::s32 c_iRaceClass       = 33;   /**< Marker for the race class */
    const irr::s32 c_iGridPos         = 34;   /**< Marker for the Grid position scheme */
    const irr::s32 c_iGridSize        = 35;   /**< Marker for the grid size */
    const irr::s32 c_iAiClass         = 36;   /**< Marker for the AI class */
    const irr::s32 c_iReverseGrid     = 37;   /**< Marker for the reverse grid flag */
    const irr::s32 c_iFirstRaceRandom = 38;   /**< Marker for the randomize first race flag */
    const irr::s32 c_iFillGrid        = 39;   /**< Marker for the fill grid with AI flag */

    const irr::s32 c_iRacePlayersHead  = 50;   /**< Marker for the beginning of the race player list */
    const irr::s32 c_iRacePlayerStart  = 51;   /**< Marker for a new dataset of a player */
    const irr::s32 c_iRacePlayerEnd    = 52;   /**< Marker for the end of a dataset */
    const irr::s32 c_iRacePlayerFooter = 53;   /**< Marker for the end of the list of players */

    const char c_sPlayerDataHead[] = "PlayerProfile";

    SGameGFX::SGameGFX() : m_bRearview(true), m_bRanking(true), m_bRaceTime(true), m_bLapTimes(false) {
    }

    void SGameGFX::copyFrom(const SGameGFX& a_cOther) {
      m_bRearview = a_cOther.m_bRearview;
      m_bRanking  = a_cOther.m_bRanking;
      m_bRaceTime = a_cOther.m_bRaceTime;
      m_bLapTimes = a_cOther.m_bLapTimes;
    }

    SSettings::SSettings() : 
      m_iResolutionW (1980), 
      m_iResolutionH (1080), 
      m_iShadows     (2), 
      m_iAmbient     (2),
      m_iTouchType   (1),
      m_fSfxMaster   (1.0f),
      m_fSoundTrack  (1.0f),
      m_fSfxMenu     (1.0f),
      m_fSfxGame     (1.0f),
      m_bFullscreen  (false),
      m_bUseMenuCtrl (false),
      m_bGfxChange   (false),
      m_bTouchControl(true),
      m_sController  ("")
    {
    }

    void SSettings::loadSettings(const std::map<std::string, std::string>& a_mData) {
      if (a_mData.find("resolution_w") != a_mData.end()) m_iResolutionW = std::atoi(a_mData.at("resolution_w").c_str());
      if (a_mData.find("resolution_h") != a_mData.end()) m_iResolutionH = std::atoi(a_mData.at("resolution_h").c_str());
      if (a_mData.find("shadows"     ) != a_mData.end()) m_iShadows     = std::atoi(a_mData.at("shadows"     ).c_str());
      if (a_mData.find("ambient"     ) != a_mData.end()) m_iAmbient     = std::atoi(a_mData.at("ambient"     ).c_str());
      if (a_mData.find("touchtype"   ) != a_mData.end()) m_iTouchType   = std::atoi(a_mData.at("touchtype"   ).c_str());

      if (a_mData.find("fullscreen"  ) != a_mData.end()) m_bFullscreen   = a_mData.at("fullscreen"  ) == "true";
      if (a_mData.find("usemenuctrl" ) != a_mData.end()) m_bUseMenuCtrl  = a_mData.at("usemenuctrl" ) == "true";
      if (a_mData.find("touchcontrol") != a_mData.end()) m_bTouchControl = a_mData.at("touchcontrol") == "true";

      if (a_mData.find("sfx_master") != a_mData.end()) m_fSfxMaster  = (float)std::atof(a_mData.at("sfx_master").c_str());
      if (a_mData.find("soundtrack") != a_mData.end()) m_fSoundTrack = (float)std::atof(a_mData.at("soundtrack").c_str());
      if (a_mData.find("sfx_menu"  ) != a_mData.end()) m_fSfxMenu    = (float)std::atof(a_mData.at("sfx_menu"  ).c_str());
      if (a_mData.find("sfx_game"  ) != a_mData.end()) m_fSfxGame    = (float)std::atof(a_mData.at("sfx_game"  ).c_str());

      if (a_mData.find("menu_control") != a_mData.end()) m_sController = a_mData.at("menu_control");

      for (int i = 0; i < 8; i++) {
        if (a_mData.find(std::string("rearview_") + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bRearview = a_mData.at(std::string("rearview_") + std::to_string(i)) == "true";
        if (a_mData.find(std::string("ranking_" ) + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bRanking  = a_mData.at(std::string("ranking_" ) + std::to_string(i)) == "true";
        if (a_mData.find(std::string("racetime_") + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bRaceTime = a_mData.at(std::string("racetime_") + std::to_string(i)) == "true";
        if (a_mData.find(std::string("laptimes_") + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bLapTimes = a_mData.at(std::string("laptimes_") + std::to_string(i)) == "true";
      }
    }

    void SSettings::saveSettings(std::map<std::string, std::string>& a_mData) {
      a_mData["resolution_w"] = std::to_string(m_iResolutionW);
      a_mData["resolution_h"] = std::to_string(m_iResolutionH);
      a_mData["shadows"     ] = std::to_string(m_iShadows    );
      a_mData["ambient"     ] = std::to_string(m_iAmbient    );
      a_mData["touchtype"   ] = std::to_string(m_iTouchType  );

      a_mData["fullscreen"  ] = m_bFullscreen   ? "true" : "false";
      a_mData["usemenuctrl" ] = m_bUseMenuCtrl  ? "true" : "false";
      a_mData["touchcontrol"] = m_bTouchControl ? "true" : "false";

      a_mData["sfx_master"] = std::to_string(m_fSfxMaster );
      a_mData["soundtrack"] = std::to_string(m_fSoundTrack);
      a_mData["sfx_menu"  ] = std::to_string(m_fSfxMenu   );
      a_mData["sfx_game"  ] = std::to_string(m_fSfxGame   );

      a_mData["menu_control"] = m_sController;

      for (int i = 0; i < 8; i++) {
        a_mData[std::string("rearview_") + std::to_string(i)] = m_aGameGFX[i].m_bRearview ? "true" : "false";
        a_mData[std::string("ranking_" ) + std::to_string(i)] = m_aGameGFX[i].m_bRanking  ? "true" : "false";
        a_mData[std::string("racetime_") + std::to_string(i)] = m_aGameGFX[i].m_bRaceTime ? "true" : "false";
        a_mData[std::string("laptimes_") + std::to_string(i)] = m_aGameGFX[i].m_bLapTimes ? "true" : "false";
      }
    }

    void SSettings::copyFrom(const SSettings& a_cOther) {
      m_bGfxChange = m_iResolutionH != a_cOther.m_iResolutionH || m_iResolutionW != a_cOther.m_iResolutionW || m_bFullscreen != a_cOther.m_bFullscreen;

      m_iResolutionW  = a_cOther.m_iResolutionW;
      m_iResolutionH  = a_cOther.m_iResolutionH;
      m_iShadows      = a_cOther.m_iShadows;
      m_iAmbient      = a_cOther.m_iAmbient;
      m_iTouchType    = a_cOther.m_iTouchType;
      m_bFullscreen   = a_cOther.m_bFullscreen;
      m_fSfxMaster    = a_cOther.m_fSfxMaster;
      m_fSoundTrack   = a_cOther.m_fSoundTrack;
      m_fSfxMenu      = a_cOther.m_fSfxMenu;
      m_fSfxGame      = a_cOther.m_fSfxGame;
      m_bUseMenuCtrl  = a_cOther.m_bUseMenuCtrl;
      m_bTouchControl = a_cOther.m_bTouchControl;
      m_sController   = a_cOther.m_sController;

      for (int i = 0; i < 8; i++)
        m_aGameGFX[i].copyFrom(a_cOther.m_aGameGFX[i]);
    }

    SPlayerData::SPlayerData() :
      m_eType    (enPlayerType::Local),
      m_eAiHelp  (enAiHelp::Off),
      m_iPlayerId(-1),
      m_iGridPos (0 ),
      m_sName    (""),
      m_sTexture (""),
      m_sControls("")
    {
      // Default controls for new player
      m_sControls = "DustbinController;control;Key;Forward;Controller%20%28GAME%20FOR%20WINDOWS%29;M;a;a;a;-0md;b;control;Key;Backward;Controller%20%28GAME%20FOR%20WINDOWS%29;O;a;a;c;-0md;b;control;Key;Left;Controller%20%28GAME%20FOR%20WINDOWS%29;L;a;a;a;-0md;-b;control;Key;Right;Controller%20%28GAME%20FOR%20WINDOWS%29;N;a;a;a;-0md;b;control;Key;Brake;Controller%20%28GAME%20FOR%20WINDOWS%29;G;a;a;a;-0md;b;control;Key;Rearview;Controller%20%28GAME%20FOR%20WINDOWS%29;j;a;e;a;-0md;b;control;Key;Respawn;Controller%20%28GAME%20FOR%20WINDOWS%29;n;a;f;a;-0md;b";
    }

    void SPlayerData::reset() {
      m_iPlayerId = -1;
      m_sName     = "";
      m_sTexture  = "";
      m_sControls = "";
    }

    void SPlayerData::copyFrom(const SPlayerData& a_cOther) {
      m_eType     = a_cOther.m_eType;
      m_sName     = a_cOther.m_sName;
      m_sTexture  = a_cOther.m_sTexture;
      m_sControls = a_cOther.m_sControls;
      m_iPlayerId = a_cOther.m_iPlayerId;
      m_eAiHelp   = a_cOther.m_eAiHelp;
      m_iGridPos  = a_cOther.m_iGridPos;
    }

    std::string SPlayerData::serialize() {
      if (m_iPlayerId > 0 && (m_iPlayerId <= 8 || (m_eType != enPlayerType::Local && m_iPlayerId <= 16))) {
        messages::CSerializer64 l_cSerializer;

        l_cSerializer.addS32(c_iPlayerDataHead);
        l_cSerializer.addString(c_sPlayerDataHead);

        l_cSerializer.addS32(c_iPlayerType);
        l_cSerializer.addS32((irr::s32)m_eType);

        l_cSerializer.addS32(c_iPlayerName);
        l_cSerializer.addString(m_sName);

        l_cSerializer.addS32(c_iPlayerControls);
        l_cSerializer.addString(m_sControls);

        l_cSerializer.addS32(c_iPlayerTexture);
        l_cSerializer.addString(m_sTexture);

        l_cSerializer.addS32(c_iPlayerAiHelp);
        l_cSerializer.addS32((irr::s32)m_eAiHelp);

        l_cSerializer.addS32(c_iPlayerGridPos);
        l_cSerializer.addS32(m_iGridPos);

        l_cSerializer.addS32(c_iPlayerDataEnd);

        return l_cSerializer.getMessageAsString();
      }
      else return "";
    }

    bool SPlayerData::deserialize(const std::string &a_sSerialized) {
      m_iPlayerId = -1;

      if (a_sSerialized != "") {
        messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sSerialized.c_str());

        if (l_cSerializer.getS32() != c_iPlayerDataHead) {
          printf("Error reading header of player data.\n");
          return false;
        }

        if (l_cSerializer.getString() != c_sPlayerDataHead) {
          printf("Error reading player data identifier.\n");
          return false;
        }

        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iToken = l_cSerializer.getS32();

          switch (l_iToken) {
            case c_iPlayerType    : m_eType     = (enPlayerType)l_cSerializer.getS32   (); break;
            case c_iPlayerName    : m_sName     =               l_cSerializer.getString(); break;
            case c_iPlayerControls: m_sControls =               l_cSerializer.getString(); break;
            case c_iPlayerTexture : m_sTexture  =               l_cSerializer.getString(); break;
            case c_iPlayerAiHelp  : m_eAiHelp   = (enAiHelp    )l_cSerializer.getS32   (); break;
            case c_iPlayerGridPos : m_iGridPos  =               l_cSerializer.getS32   (); break;
            case c_iPlayerDataEnd : return true;
            default:
              printf("Unknow token %i\n", l_iToken);
              return false;
          }
        }
        return true;
      }

      return true;
    }
    
    std::string SPlayerData::toString() {
      std::string s = "Player Data\n  \"" + m_sName + "\"\n";

      s += "  Type: ";

      switch (m_eType) {
        case enPlayerType::Local: s += "\"Local\"\n"; break;
        case enPlayerType::Ai   : s += "\"Ai\"\n"   ; break;
      }

      s += "  Texture: \"" + m_sTexture + "\"\n";
      s += "  Controls: \"" + m_sControls + "\"\n";
      s += "  Grid Pos: " + std::to_string(m_iGridPos) + "\n";

      s += "  Ai Help: ";

      switch (m_eAiHelp) {
        case enAiHelp::Off    : s += "\"Off\"\n"    ; break;
        case enAiHelp::Display: s += "\"Display\"\n"; break;
        case enAiHelp::Low    : s += "\"Low\"\n"    ; break;
        case enAiHelp::Medium : s += "\"Medium\"\n" ; break;
        case enAiHelp::High   : s += "\"High\"\n"   ; break;
        case enAiHelp::Bot    : s += "\"Ai Bot\"\n" ; break;
      }

      return s;
    }

    std::vector<SPlayerData> SPlayerData::createPlayerVector(const std::string a_sSerialized) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sSerialized.c_str());
      std::vector<SPlayerData> l_vRet;

      if (l_cSerializer.getS32() == c_iProfileHead) {
        if (l_cSerializer.getString() == c_sProfileHead) {
          while (l_cSerializer.hasMoreMessages()) {
            irr::s32 l_iToken = l_cSerializer.getS32();

            if (l_iToken == c_iProfileStart) {
              SPlayerData l_cPlayer;
              l_cPlayer.deserialize(l_cSerializer.getString());
              if (l_cSerializer.getS32() != c_iProfileEnd)
                printf("Invalid profile end token.\n");
              else
                l_vRet.push_back(l_cPlayer);
            }
            else printf("Invalid profile start token.\n");
          }
        }
        else printf("Invalid player profiles marker.\n");
      }
      else printf("Invalid player profiles header id.\n");

      return l_vRet;
    }

    SGameSettings::SGameSettings() :
      m_iRaceClass      (0),
      m_iGridPos        (1),
      m_iGridSize       (16),
      m_iAiClass        (0),
      m_bReverseGrid    (false),
      m_bRandomFirstRace(true),
      m_bFillGridAI     (true)
    {

    }

    std::string SGameSettings::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iGameSettings   );
      l_cSerializer.addS32(c_iRaceClass      ); l_cSerializer.addS32(m_iRaceClass);
      l_cSerializer.addS32(c_iGridPos        ); l_cSerializer.addS32(m_iGridPos  );
      l_cSerializer.addS32(c_iGridSize       ); l_cSerializer.addS32(m_iGridSize );
      l_cSerializer.addS32(c_iAiClass        ); l_cSerializer.addS32(m_iAiClass  );
      l_cSerializer.addS32(c_iReverseGrid    ); l_cSerializer.addS32(m_bReverseGrid     ? 1 : 0);
      l_cSerializer.addS32(c_iFirstRaceRandom); l_cSerializer.addS32(m_bRandomFirstRace ? 1 : 0);
      l_cSerializer.addS32(c_iFillGrid       ); l_cSerializer.addS32(m_bFillGridAI      ? 1 : 0);

      return l_cSerializer.getMessageAsString();
    }

    bool SGameSettings::deserialize(const std::string& a_sSerialized) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sSerialized.c_str());

      if (l_cSerializer.getS32() == c_iGameSettings) {
        while (l_cSerializer.hasMoreMessages()) {
          switch (l_cSerializer.getS32()) {
            case c_iRaceClass      : m_iRaceClass       = l_cSerializer.getS32()     ; break;
            case c_iGridPos        : m_iGridPos         = l_cSerializer.getS32()     ; break;
            case c_iGridSize       : m_iGridSize        = l_cSerializer.getS32()     ; break;
            case c_iAiClass        : m_iAiClass         = l_cSerializer.getS32()     ; break;
            case c_iReverseGrid    : m_bReverseGrid     = l_cSerializer.getS32() != 0; break;
            case c_iFirstRaceRandom: m_bRandomFirstRace = l_cSerializer.getS32() != 0; break;
            case c_iFillGrid       : m_bFillGridAI      = l_cSerializer.getS32() != 0; break;
          }
        }
      }
      else {
        printf("Invalid game settings data.\n");
        return false;
      }
      return true;
    }

    std::string SRacePlayers::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iRacePlayersHead);

      for (std::vector<SPlayerData>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        l_cSerializer.addS32(c_iRacePlayerStart);
        l_cSerializer.addString((*it).serialize());
        l_cSerializer.addS32(c_iRacePlayerEnd);
      }

      l_cSerializer.addS32(c_iRacePlayerFooter);

      return l_cSerializer.getMessageAsString();
    }

    bool SRacePlayers::deserialize(const std::string& a_sSerialized) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sSerialized.c_str());

      if (l_cSerializer.getS32() == c_iRacePlayersHead) {
        int l_iPlayerId = 1;

        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iToken = l_cSerializer.getS32();

          if (l_iToken == c_iRacePlayerStart) {
            SPlayerData l_cPlayer;
            if (!l_cPlayer.deserialize(l_cSerializer.getString())) {
              printf("Unable to deserialize race player, aborting.\n");
              return false;
            }

            if (l_cSerializer.getS32() != c_iRacePlayerEnd) {
              printf("Invalid flag for end of race player dataset.\n");
              return false;
            }

            l_cPlayer.m_iPlayerId = l_iPlayerId++;
            m_vPlayers.push_back(l_cPlayer);
          }
          else if (l_iToken == c_iRacePlayerFooter) {
            return false;
          }
          else {
            printf("Invalid token %i found in race player dataset.\n", l_iToken);
            return false;
          }
        }
      }
      else {
        printf("Invalid head for the race player dataset.\n");
        return false;
      }

      return true;
    }

    std::string SRacePlayers::toString() {
      std::string s = "Race Player Setup (" + std::to_string(m_vPlayers.size()) + " Players)\n";

      for (std::vector<SPlayerData>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        s += (*it).toString() + "\n";
      }

      return s;
    }
  }
}