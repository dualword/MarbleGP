// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <messages/CMessageHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <Defines.h>
#include <CGlobal.h>
#include <algorithm>
#include <ostream>

namespace dustbin {
  namespace data {
    // The message IDs for (de)serialization of the
    // player data structures
    const irr::s32 c_iPlayerDataHead     = 23;  /**< Marker for the player data to start */
    const irr::s32 c_iPlayerType         = 24;  /**< Marker for the type of player */
    const irr::s32 c_iPlayerName         = 25;  /**< Marker for the player name */
    const irr::s32 c_iPlayerTexture      = 26;  /**< Marker for the player texture definition string */
    const irr::s32 c_iPlayerControls     = 27;  /**< Marker for the player controller setup string */
    const irr::s32 c_iPlayerAiHelp       = 28;  /**< Marker for the level of AI help for the player */
    const irr::s32 c_iPlayerGridPos      = 29;  /**< Marker for the position in the grid */
    const irr::s32 c_iPlayerViewPort     = 30;  /**< The player's viewport (if any) */
    const irr::s32 c_iPlayerDataEnd      = 31;  /**< Marker for the player data to end */
    const irr::s32 c_iPlayerShortName    = 32;  /**< Marker for the player's short name (for in-game ranking display) */

    // Game Settings
    const irr::s32 c_iGameSettings    = 42;   /**< Marker for the game settings */
    const irr::s32 c_iRaceClass       = 43;   /**< Marker for the race class */
    const irr::s32 c_iGridPos         = 44;   /**< Marker for the Grid position scheme */
    const irr::s32 c_iGridSize        = 45;   /**< Marker for the grid size */
    const irr::s32 c_iAutoFinish      = 46;   /**< Marker for the auto finish type */
    const irr::s32 c_iReverseGrid     = 47;   /**< Marker for the reverse grid flag */
    const irr::s32 c_iFirstRaceRandom = 48;   /**< Marker for the randomize first race flag */
    const irr::s32 c_iFillGrid        = 49;   /**< Marker for the fill grid with AI flag */

    // Race Player
    const irr::s32 c_iRacePlayersHead   = 50;   /**< Marker for the beginning of the race player list */
    const irr::s32 c_iRacePlayersStart  = 51;   /**< Marker for a new dataset of a player */
    const irr::s32 c_iRacePlayersEnd    = 52;   /**< Marker for the end of a dataset */
    const irr::s32 c_iRacePlayersFooter = 53;   /**< Marker for the end of the list of players */

    // Championship Player
    const irr::s32 c_iChampionshipPlayerHead         = -60;       /**< Marker for the beginning of a championship player */
    const irr::s32 c_iChampionshipPlayerId           = -61;       /**< Marker for the name of a championship player */
    const irr::s32 c_iChampionshipPlayerName         = -62;       /**< Marker for the name of a championship player */
    const irr::s32 c_iChampionshipPlayerResults      = -63;       /**< Marker for the race results of a championship player */
    const irr::s32 c_iChampionshipPlayerPoints       = -64;       /**< Marker for the points of a championship player */
    const irr::s32 c_iChampionshipPlayerRespawns     = -65;       /**< Marker for the number of respawns of a championship player */
    const irr::s32 c_iChampionshipPlayerStunned      = -66;       /**< Marker for the number of stuns of a championship player */
    const irr::s32 c_iChampionshipPlayerFastest      = -67;       /**< Marker for the number of fastest race laps of a championship player */
    const irr::s32 c_iChampionshipPlayerDNF          = -68;       /**< Marker for the number of races the player didn't finish */
    const irr::s32 c_iChampionshipPlayerBestFinish   = -69;       /**< Marker for the first race the player scored his best finish */
    const irr::s32 c_iChampionshipPlayerFooter       = -70;       /**< Marker for the end of the dataset */

    // Championship Race
    const irr::s32 c_iChampionshipRaceHead        = -80;      /**< Marker for the beginning of a championship race */
    const irr::s32 c_iChampionshipRacePlayers     = -81;      /**< Marker for number of players of the race */
    const irr::s32 c_iChampionshipRaceLaps        = -82;      /**< Marker for number of laps of the race */
    const irr::s32 c_iChampionshipRaceTrack       = -83;      /**< Marker for number of laps of the race */
    const irr::s32 c_iChampionshipRaceResult      = -84;      /**< Marker for the beginning of result of the race */
    const irr::s32 c_iChampionshipRacePlayer      = -85;      /**< Marker for a player of the result */
    const irr::s32 c_iChampionshipRaceResultEnd   = -86;      /**< Marker for the end of result of the race */
    const irr::s32 c_iChampionshipRaceAssignBegin = -87;      /**< Marker for the beginning of the marble assignment */
    const irr::s32 c_iChampionshipRaceAssignEnd   = -88;      /**< Marker for the end of the marble assignment */
    const irr::s32 c_iChampionshipRaceFooter      = -89;      /**< Marker for the end of a championship race */

    // Championship
    const irr::s32 c_iChampionshipHeader       = -100;    /**< Marker for the start of a championship data structure */
    const irr::s32 c_iChampionshipClass        = -101;    /**< Marker for the the championship class */
    const irr::s32 c_iChampionshipGridSize     = -102;    /**< Marker for the the championship class */
    const irr::s32 c_iChampionshipGridOrder    = -103;    /**< Marker for the the championship class */
    const irr::s32 c_iChampionshipReverseGrid  = -104;    /**< Marker for the the championship class */
    const irr::s32 c_iChampionshipPlayersStart = -105;    /**< Marker for the the start of the championship players */
    const irr::s32 c_iChampionshipPlayersCount = -106;    /**< Marker for the the number of championship players */
    const irr::s32 c_iChampionshipPlayersData  = -107;    /**< Marker for the the start of a championship player */
    const irr::s32 c_iChampionshipPlayersEnd   = -108;    /**< Marker for the the end of the players of the championship */
    const irr::s32 c_iChampionshipRacesStart   = -109;    /**< Marker for the the start of the championship races */
    const irr::s32 c_iChampionshipRacesCount   = -110;    /**< Marker for the the number of championship races */
    const irr::s32 c_iChampionshipRacesData    = -111;    /**< Marker for the start of a championship race structure */
    const irr::s32 c_iChampionshipRacesFooter  = -112;    /**< Marker for the end of the championship data structure */

    // Race Player (limited data is encoded)
    const irr::s32 c_iRacePlayerHeader       = -120;   /**< Marker for the start of a race player data structure */
    const irr::s32 c_iRacePlayerId           = -121;   /**< Marker for the race player's id (aka marble id) */
    const irr::s32 c_iRacePlayerPos          = -122;   /**< Marker for the finishing position of the player */
    const irr::s32 c_iRacePlayerDeficit      = -123;   /**< Marker for the deficit of the player on the leader */
    const irr::s32 c_iRacePlayerFastest      = -124;   /**< Marker for the fastest race lap of the player */
    const irr::s32 c_iRacePlayerStunned      = -125;   /**< Marker for the number of stuns a player has experienced during the race */
    const irr::s32 c_iRacePlayerRespawn      = -126;   /**< Marker for the number of respawns of the player during the race */
    const irr::s32 c_iRacePlayerWithdraw     = -127;   /**< Marker for the "player withdrawn from race" data */
    const irr::s32 c_iRacePlayerLapTimeStart = -128;   /**< Marker for the start of the lap times */
    const irr::s32 c_iRacePlayerCpTimesStart = -129;   /**< Marker for the start of the checkpoints of a lap */
    const irr::s32 c_iRacePlayerCpTimesEnd   = -130;   /**< Marker for the end of the checkpoint times of a lap */
    const irr::s32 c_iRacePlayerLapTimeEnd   = -140;   /**< Marker for the end of the lap times */
    const irr::s32 c_iRacePlayerFooter       = -141;   /**< Marker for the end of the race player data structure */

    // Game data
    const irr::s32 c_iGameHead  = -150;   /**< Header for the game data */
    const irr::s32 c_iGameType  = -151;   /**< The type of the game */
    const irr::s32 c_iGameTrack = -152;   /**< The track of the upcoming race */
    const irr::s32 c_iGameLaps  = -153;   /**< The laps of the race */
    const irr::s32 c_iGameClass = -154;   /**< The class of the race */

    // Marble AI Class
    const irr::s32 c_iMarbleData  = -170;   /**< Header for the Marble AI data */
    const irr::s32 c_iMarbleClass = -171;   /**< Marker for the marble class */
    const irr::s32 c_iMarbleModes = -172;   /**< Marker for the AI mode flags */

    // Free Game Slots
    const irr::s32 c_iFreeGameSlots = -180; /**< Marker for free game slots */

    const char c_sPlayerDataHead[] = "PlayerProfile";

    SGameGFX::SGameGFX() : m_bHighlight(true), m_bShowControls(true), m_bShowRanking(true), m_bShowLapTimes(true) {
    }

    void SGameGFX::copyFrom(const SGameGFX& a_cOther) {
      m_bHighlight   = a_cOther.m_bHighlight;
      m_bShowControls = a_cOther.m_bShowControls;
      m_bShowRanking  = a_cOther.m_bShowRanking;
      m_bShowLapTimes = a_cOther.m_bShowLapTimes;
    }

    SSettings::SSettings() : 
      m_iResolutionW (1920), 
      m_iResolutionH (1080), 
      m_iShadows     (2), 
      m_iAmbient     (2),
      m_fSfxMaster   (1.0f),
      m_fSoundTrack  (1.0f),
      m_fSfxMenu     (1.0f),
      m_fSfxGame     (1.0f),
      m_bFullscreen  (false),
      m_bGfxChange   (false),
      m_bMenuPad     (false),
      m_bDebugAI     (false),
#ifdef _ANDROID
      m_bUseMenuCtrl (true),
      m_bVirtualKeys (true),
      m_sController("DustbinController;control;JoyPov;Up;Gamepad;M;a;a;a;a;b;control;JoyPov;Down;Gamepad;O;a;a;a;qze;b;control;JoyPov;Left;Gamepad;L;a;a;a;4Lg;b;control;JoyPov;Right;Gamepad;N;a;a;a;Omc;b;control;JoyButton;Enter;Gamepad;G;a;a;a;a;b;control;JoyButton;Ok;Gamepad;n;a;m;a;a;b;control;JoyButton;Cancel;Gamepad;B;a;n;a;a;b")
#else
      m_bUseMenuCtrl (false),
      m_bVirtualKeys (false),
      m_sController  ("")
#endif
    {
    }

    void SSettings::loadSettings(const std::map<std::string, std::string>& a_mData) {
      if (a_mData.find("resolution_w") != a_mData.end()) m_iResolutionW  = std::atoi(a_mData.at("resolution_w").c_str());
      if (a_mData.find("resolution_h") != a_mData.end()) m_iResolutionH  = std::atoi(a_mData.at("resolution_h").c_str());
      if (a_mData.find("shadows"     ) != a_mData.end()) m_iShadows      = std::atoi(a_mData.at("shadows"     ).c_str());
      if (a_mData.find("ambient"     ) != a_mData.end()) m_iAmbient      = std::atoi(a_mData.at("ambient"     ).c_str());

      if (a_mData.find("fullscreen"  ) != a_mData.end()) m_bFullscreen   = a_mData.at("fullscreen" ) == "true";
      if (a_mData.find("usemenuctrl" ) != a_mData.end()) m_bUseMenuCtrl  = a_mData.at("usemenuctrl") == "true";
      if (a_mData.find("debugai"     ) != a_mData.end()) m_bDebugAI      = a_mData.at("debugai"    ) == "true";

      if (a_mData.find("sfx_master") != a_mData.end()) m_fSfxMaster  = (float)std::atof(a_mData.at("sfx_master").c_str());
      if (a_mData.find("soundtrack") != a_mData.end()) m_fSoundTrack = (float)std::atof(a_mData.at("soundtrack").c_str());
      if (a_mData.find("sfx_menu"  ) != a_mData.end()) m_fSfxMenu    = (float)std::atof(a_mData.at("sfx_menu"  ).c_str());
      if (a_mData.find("sfx_game"  ) != a_mData.end()) m_fSfxGame    = (float)std::atof(a_mData.at("sfx_game"  ).c_str());

      if (a_mData.find("menu_control") != a_mData.end()) m_sController = a_mData.at("menu_control");

      for (int i = 0; i < 8; i++) {
        if (a_mData.find(std::string("highlight_") + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bHighlight   = a_mData.at(std::string("highlight_") + std::to_string(i)) == "true";
        if (a_mData.find(std::string("showctrls_") + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bShowControls = a_mData.at(std::string("showctrls_") + std::to_string(i)) == "true";
        if (a_mData.find(std::string("showrank_" ) + std::to_string(i)) != a_mData.end()) m_aGameGFX[i].m_bShowRanking  = a_mData.at(std::string("showrank_" ) + std::to_string(i)) == "true";
      }

#ifdef _ANDROID
      m_bVirtualKeys = true;
#else
      if (a_mData.find("virtualkeys") != a_mData.end()) m_bVirtualKeys = a_mData.at("virtualkeys") == "true";
#endif
    }

    void SSettings::saveSettings(std::map<std::string, std::string>& a_mData) {
      a_mData["resolution_w"] = std::to_string(m_iResolutionW);
      a_mData["resolution_h"] = std::to_string(m_iResolutionH);
      a_mData["shadows"     ] = std::to_string(m_iShadows    );
      a_mData["ambient"     ] = std::to_string(m_iAmbient    );

      a_mData["fullscreen"  ] = m_bFullscreen   ? "true" : "false";
      a_mData["usemenuctrl" ] = m_bUseMenuCtrl  ? "true" : "false";
      a_mData["debugai"     ] = m_bDebugAI      ? "true" : "false";

#ifdef _ANDROID
      a_mData["virtualkeys"] = "true";
#else
      a_mData["virtualkeys"] = m_bVirtualKeys ? "true" : "false";
#endif

      a_mData["sfx_master"] = std::to_string(m_fSfxMaster );
      a_mData["soundtrack"] = std::to_string(m_fSoundTrack);
      a_mData["sfx_menu"  ] = std::to_string(m_fSfxMenu   );
      a_mData["sfx_game"  ] = std::to_string(m_fSfxGame   );

      a_mData["menu_control"] = m_sController;

      for (int i = 0; i < 8; i++) {
        a_mData[std::string("highlight_") + std::to_string(i)] = m_aGameGFX[i].m_bHighlight   ? "true" : "false";
        a_mData[std::string("showctrls_") + std::to_string(i)] = m_aGameGFX[i].m_bShowControls ? "true" : "false";
        a_mData[std::string("showrank_" ) + std::to_string(i)] = m_aGameGFX[i].m_bShowRanking  ? "true" : "false";
      }
    }

    void SSettings::copyFrom(const SSettings& a_cOther) {
      m_bGfxChange = m_iResolutionH != a_cOther.m_iResolutionH || m_iResolutionW != a_cOther.m_iResolutionW || m_bFullscreen != a_cOther.m_bFullscreen;

      m_iResolutionW  = a_cOther.m_iResolutionW;
      m_iResolutionH  = a_cOther.m_iResolutionH;
      m_iShadows      = a_cOther.m_iShadows;
      m_iAmbient      = a_cOther.m_iAmbient;
      m_bFullscreen   = a_cOther.m_bFullscreen;
      m_fSfxMaster    = a_cOther.m_fSfxMaster;
      m_fSoundTrack   = a_cOther.m_fSoundTrack;
      m_fSfxMenu      = a_cOther.m_fSfxMenu;
      m_fSfxGame      = a_cOther.m_fSfxGame;
      m_bUseMenuCtrl  = a_cOther.m_bUseMenuCtrl;
      m_sController   = a_cOther.m_sController;
      m_bVirtualKeys  = a_cOther.m_bVirtualKeys;
      m_bDebugAI      = a_cOther.m_bDebugAI;

      for (int i = 0; i < 8; i++)
        m_aGameGFX[i].copyFrom(a_cOther.m_aGameGFX[i]);
    }

    SPlayerData::SPlayerData() :
      m_eType     (enPlayerType::Local),
      m_eAiHelp   (enAiHelp::Off),
      m_iPlayerId (-1),
      m_iGridPos  (0 ),
      m_iViewPort (-1),
      m_sName     (""),
      m_sTexture  (""),
      m_sControls (""),
      m_sShortName("")
    {
#ifdef _ANDROID
      m_sControls = "DustbinController;control;JoyButton;Forward;Gamepad;M;a;j;a;-0md;b;control;JoyButton;Backward;Gamepad;O;a;i;c;-0md;b;control;JoyAxis;Left;Gamepad;L;a;a;a;-0md;-b;control;JoyAxis;Right;Gamepad;N;a;a;a;-0md;b;control;JoyButton;Brake;Gamepad;G;a;a;a;-0md;b;control;JoyButton;Rearview;Gamepad;j;a;g;a;-0md;b;control;JoyButton;Respawn;Gamepad;n;a;h;a;-0md;b;control;JoyButton;Pause;Gamepad;t;a;m;a;-SN;b;control;JoyButton;Cancel%20Race;Gamepad;B;a;n;a;-SN;b";
#else
      // Default controls for new player
      m_sControls = "DustbinController;control;Key;Forward;Controller%20%28GAME%20FOR%20WINDOWS%29;M;a;a;a;-0md;b;control;Key;Backward;Controller%20%28GAME%20FOR%20WINDOWS%29;O;a;a;c;-0md;b;control;Key;Left;Controller%20%28GAME%20FOR%20WINDOWS%29;L;a;a;a;-0md;-b;control;Key;Right;Controller%20%28GAME%20FOR%20WINDOWS%29;N;a;a;a;-0md;b;control;Key;Brake;Controller%20%28GAME%20FOR%20WINDOWS%29;G;a;a;a;-0md;b;control;Key;Rearview;Controller%20%28GAME%20FOR%20WINDOWS%29;j;a;e;a;-0md;b;control;Key;Respawn;Controller%20%28GAME%20FOR%20WINDOWS%29;n;a;f;a;-0md;b";
#endif
    }

    void SPlayerData::reset() {
      m_iPlayerId = -1;
      m_sName     = "";
      m_sTexture  = "";
      m_sControls = "";
    }

    void SPlayerData::copyFrom(const SPlayerData& a_cOther) {
      m_eType      = a_cOther.m_eType;
      m_sName      = a_cOther.m_sName;
      m_sTexture   = a_cOther.m_sTexture;
      m_sControls  = a_cOther.m_sControls;
      m_sShortName = a_cOther.m_sShortName;
      m_iPlayerId  = a_cOther.m_iPlayerId;
      m_eAiHelp    = a_cOther.m_eAiHelp;
      m_iGridPos   = a_cOther.m_iGridPos;
      m_iViewPort  = a_cOther.m_iViewPort;
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

        l_cSerializer.addS32(c_iPlayerViewPort);
        l_cSerializer.addS32(m_iViewPort);

        l_cSerializer.addS32(c_iPlayerShortName);
        l_cSerializer.addString(m_sShortName);

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
            case c_iPlayerType     : m_eType      = (enPlayerType)l_cSerializer.getS32   (); break;
            case c_iPlayerName     : m_sName      =               l_cSerializer.getString(); break;
            case c_iPlayerControls : m_sControls  =               l_cSerializer.getString(); break;
            case c_iPlayerTexture  : m_sTexture   =               l_cSerializer.getString(); break;
            case c_iPlayerAiHelp   : m_eAiHelp    = (enAiHelp    )l_cSerializer.getS32   (); break;
            case c_iPlayerGridPos  : m_iGridPos   =               l_cSerializer.getS32   (); break;
            case c_iPlayerViewPort : m_iViewPort  =               l_cSerializer.getS32   (); break;
            case c_iPlayerShortName: m_sShortName =              l_cSerializer.getString(); break;
            case c_iPlayerDataEnd  : return true;
            default:
              printf("Unknown token %i\n", l_iToken);
              return false;
          }
        }
        return true;
      }

      return true;
    }
    
    std::string SPlayerData::toString() {
      std::string s = "Player Data\n  \"" + m_sName + "\"\n";

      s += "  PlayerId: " + std::to_string(m_iPlayerId) + "\n";
      s += "  Type: ";

      switch (m_eType) {
        case enPlayerType::Local  : s += "\"Local\"\n"  ; break;
        case enPlayerType::Ai     : s += "\"Ai\"\n"     ; break;
        case enPlayerType::Network: s += "\"Network\"\n"; break;
      }

      s += "  Texture: \"" + m_sTexture + "\"\n";
      s += "  Controls: \"" + m_sControls + "\"\n";
      s += "  Grid Pos: " + std::to_string(m_iGridPos) + "\n"; 
      s += "  Viewport: " + std::to_string(m_iViewPort) + "\n";

      s += "  Ai Help: ";

      switch (m_eAiHelp) {
        case enAiHelp::Off    : s += "\"Off\"\n"    ; break;
        case enAiHelp::Display: s += "\"Display\"\n"; break;
        case enAiHelp::Low    : s += "\"Low\"\n"    ; break;
        case enAiHelp::Medium : s += "\"Medium\"\n" ; break;
        case enAiHelp::High   : s += "\"High\"\n"   ; break;
        case enAiHelp::BotMgp : s += "\"Ai Bot (MarbleGP)\"\n" ; break;
        case enAiHelp::BotMb2 : s += "\"Ai Bot (Marble2)\"\n" ; break;
        case enAiHelp::BotMb3 : s += "\"Ai Bot (Marble3)\"\n" ; break;
      }

      return s;
    }

    /**
    * Is this player a bot?
    * @return "true" if this is a bot, "false" otherwise
    */
    bool SPlayerData::isBot() {
      return m_eAiHelp == enAiHelp::BotMgp || m_eAiHelp == enAiHelp::BotMb2 || m_eAiHelp == enAiHelp::BotMb3;
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
      m_iGridSize       (2),
      m_iAutoFinish     (0),
      m_bReverseGrid    (false),
      m_bRandomFirstRace(true),
      m_bFillGridAI     (true)
    {

    }

    std::string SGameSettings::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iGameSettings   );
      l_cSerializer.addS32(c_iRaceClass      ); l_cSerializer.addS32(m_iRaceClass );
      l_cSerializer.addS32(c_iGridPos        ); l_cSerializer.addS32(m_iGridPos   );
      l_cSerializer.addS32(c_iGridSize       ); l_cSerializer.addS32(m_iGridSize  );
      l_cSerializer.addS32(c_iAutoFinish     ); l_cSerializer.addS32(m_iAutoFinish);
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
            case c_iAutoFinish     : m_iAutoFinish      = l_cSerializer.getS32()     ; break;
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

    SGameData::SGameData() : m_eType(enType::Local), m_sTrack(""), m_iLaps(1), m_iClass(0) {
    }

    SGameData::SGameData(enType a_eType, const std::string& a_sTrack, int a_iLaps, int a_iClass) : m_eType(a_eType), m_sTrack(a_sTrack), m_iLaps(a_iLaps), m_iClass(a_iClass) {
    }

    SGameData::SGameData(const std::string& a_sData) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      int l_iHead = l_cSerializer.getS32();

      if (l_iHead == c_iGameHead) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iData = l_cSerializer.getS32();

          switch (l_iData) {
            case c_iGameType:
              m_eType = (enType)l_cSerializer.getS32();
              break;

            case c_iGameTrack:
              m_sTrack = l_cSerializer.getString();
              break;

            case c_iGameLaps:
              m_iLaps = l_cSerializer.getS32();
              break;

            case c_iGameClass:
              m_iClass = l_cSerializer.getS32();
              break;

            default:
              printf("Unkown data in game struct: #%i\n", l_iData);
              break;
          }
        }
      }
      else printf("Invalid game data header #%i\n", l_iHead);
    }

    std::string SGameData::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iGameHead);

      // The type of the game
      l_cSerializer.addS32(c_iGameType);
      l_cSerializer.addS32((irr::s32)m_eType);

      // The track
      l_cSerializer.addS32(c_iGameTrack);
      l_cSerializer.addString(m_sTrack);

      // The number of laps
      l_cSerializer.addS32(c_iGameLaps);
      l_cSerializer.addS32(m_iLaps);

      // The race class
      l_cSerializer.addS32(c_iGameClass);
      l_cSerializer.addS32(m_iClass);

      return l_cSerializer.getMessageAsString();
    }


    std::string SRacePlayers::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iRacePlayersHead);
      
      std::sort(m_vPlayers.begin(), m_vPlayers.end(), [](const SPlayerData &p1, const SPlayerData &p2) {
        return p1.m_iPlayerId < p2.m_iPlayerId;
      });

      for (std::vector<SPlayerData>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        l_cSerializer.addS32(c_iRacePlayersStart);
        l_cSerializer.addString((*it).serialize());
        l_cSerializer.addS32(c_iRacePlayersEnd);
      }

      l_cSerializer.addS32(c_iRacePlayersFooter);

      return l_cSerializer.getMessageAsString();
    }

    bool SRacePlayers::deserialize(const std::string& a_sSerialized) {
      m_vPlayers.clear();

      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sSerialized.c_str());

      if (l_cSerializer.getS32() == c_iRacePlayersHead) {
        int l_iPlayerId = 1;

        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iToken = l_cSerializer.getS32();

          if (l_iToken == c_iRacePlayersStart) {
            SPlayerData l_cPlayer;
            if (!l_cPlayer.deserialize(l_cSerializer.getString())) {
              printf("Unable to deserialize race player, aborting.\n");
              return false;
            }

            if (l_cSerializer.getS32() != c_iRacePlayersEnd) {
              printf("Invalid flag for end of race player dataset.\n");
              return false;
            }

            l_cPlayer.m_iPlayerId = l_iPlayerId++;
            m_vPlayers.push_back(l_cPlayer);
          }
          else if (l_iToken == c_iRacePlayersFooter) {
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

    SChampionshipPlayer::SChampionshipPlayer(int a_iPlayerId, const std::string &a_sName) : 
      m_iPlayerId    (a_iPlayerId), 
      m_sName        (a_sName), 
      m_iPoints      (0), 
      m_iRespawn     (0), 
      m_iStunned     (0), 
      m_iFastestLaps (0), 
      m_iDidNotFinish(0),
      m_iBestFinish  (0)
    {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = 0;
    }

    SChampionshipPlayer::SChampionshipPlayer(const SChampionshipPlayer &a_cOther) : 
      m_iPlayerId    (a_cOther.m_iPlayerId),
      m_sName        (a_cOther.m_sName),
      m_iPoints      (a_cOther.m_iPoints),
      m_iRespawn     (a_cOther.m_iRespawn),
      m_iStunned     (a_cOther.m_iStunned),
      m_iFastestLaps (a_cOther.m_iFastestLaps),
      m_iDidNotFinish(a_cOther.m_iDidNotFinish),
      m_iBestFinish  (a_cOther.m_iBestFinish)
    {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = a_cOther.m_aResult[i];
    }

    SChampionshipPlayer::SChampionshipPlayer(const std::string& a_sData) : m_iPlayerId(0), m_sName(""), m_iPoints(0), m_iRespawn(0), m_iStunned(0), m_iFastestLaps(0), m_iDidNotFinish(0) {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = 0;

      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      if (l_cSerializer.getS32() == c_iChampionshipPlayerHead) {
        while (l_cSerializer.hasMoreMessages()) {
          int l_iType = l_cSerializer.getS32();

          switch (l_iType) {
            case c_iChampionshipPlayerId:
              m_iPlayerId = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerName:
              m_sName = l_cSerializer.getString();
              break;

            case c_iChampionshipPlayerResults: {
              int l_iValue = -1;
              int l_iCount = l_cSerializer.getS32();

              for (int i = 0; i < l_iCount; i++) {
                l_iValue = l_cSerializer.getS32();
                if (l_iValue == c_iChampionshipPlayerResults)
                  break;
                m_aResult[i] = l_iValue;
              }

              if (l_iValue != c_iChampionshipPlayerResults) {
                l_iValue = l_cSerializer.getS32();
              }

              if (l_iValue != c_iChampionshipPlayerResults) {
                printf("Invalid result marker %i\n", l_iValue);
              }
              break;
            }

            case c_iChampionshipPlayerPoints:
              m_iPoints = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerRespawns:
              m_iRespawn = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerStunned:
              m_iStunned = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerFastest:
              m_iFastestLaps = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerDNF:
              m_iDidNotFinish = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerBestFinish:
              m_iBestFinish = l_cSerializer.getS32();
              break;

            case c_iChampionshipPlayerFooter:
              break;

            default:
              printf("Unknown data marker %i\n", l_iType);
              break;
          }
        }
      }
      else printf("Invalid header for championship player.\n");
    }

    std::string SChampionshipPlayer::serialize() {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64();

      l_cSerializer.addS32(c_iChampionshipPlayerHead);  // Header

      // Player ID
      l_cSerializer.addS32(c_iChampionshipPlayerId);
      l_cSerializer.addS32(m_iPlayerId);

      // Name
      l_cSerializer.addS32(c_iChampionshipPlayerName);
      l_cSerializer.addString(m_sName.c_str());

      // Results
      l_cSerializer.addS32(c_iChampionshipPlayerResults);
      l_cSerializer.addS32(16);
      for (int i = 0; i < 16; i++) {
        l_cSerializer.addS32(m_aResult[i]);
      }
      l_cSerializer.addS32(c_iChampionshipPlayerResults);

      // Points
      l_cSerializer.addS32(c_iChampionshipPlayerPoints);
      l_cSerializer.addS32(m_iPoints);

      // Respawns
      l_cSerializer.addS32(c_iChampionshipPlayerRespawns);
      l_cSerializer.addS32(m_iRespawn);

      // Stuns
      l_cSerializer.addS32(c_iChampionshipPlayerStunned);
      l_cSerializer.addS32(m_iStunned);

      // Fastest laps
      l_cSerializer.addS32(c_iChampionshipPlayerFastest);
      l_cSerializer.addS32(m_iFastestLaps);

      // DNFs
      l_cSerializer.addS32(c_iChampionshipPlayerDNF);
      l_cSerializer.addS32(m_iDidNotFinish);

      // First Best Finish Race
      l_cSerializer.addS32(c_iChampionshipPlayerBestFinish);
      l_cSerializer.addS32(m_iBestFinish);

      // Footer
      l_cSerializer.addS32(c_iChampionshipPlayerFooter);

      return l_cSerializer.getMessageAsString();
    }

    std::string SChampionshipPlayer::to_xml() const {
      std::string s = "    <player>\n";

      s += "      <playerid>" + std::to_string(m_iPlayerId) + "</playerid>\n";
      s += "      <name>"              +                m_sName          + "</name>\n";
      s += "      <points>"            + std::to_string(m_iPoints      ) + "</points>\n";
      s += "      <respawn>"           + std::to_string(m_iRespawn     ) + "</respawn>\n";
      s += "      <stunned>"           + std::to_string(m_iStunned     ) + "</stunned>\n";
      s += "      <fastest_laps>"      + std::to_string(m_iFastestLaps ) + "</fastest_laps>\n";
      s += "      <did_not_finish>"    + std::to_string(m_iDidNotFinish) + "</did_not_finish>\n";
      s += "      <first_best_finish>" + std::to_string(m_iBestFinish  ) + "</first_best_finish>\n";

      s += "      <race_results>\n";

      for (int i = 0; i < 16; i++)
        s += "        <position pos=\"" + std::to_string(i + 1) + "\">" + std::to_string(m_aResult[i]) + "</position>\n";

      s += "      </race_results>\n";

      s += "    </player>\n";

      return s;
    }

    std::string SChampionshipPlayer::to_string() {
      std::string s = "SChampionshipPlayer: " +
        std::string("player id=") + std::to_string(m_iPlayerId) + ", " +
        std::string("name="     ) +                m_sName      + "\nresults = [ ";

      for (int i = 0; i < 16; i++) {
        if (i != 0) s += ", ";
        s += std::to_string(m_aResult[i]);
      }

      s += "]\n"+
        std::string("points=" ) + std::to_string(m_iPoints      ) + ", " +
        std::string("respawn=") + std::to_string(m_iRespawn     ) + ", " +
        std::string("stunned=") + std::to_string(m_iStunned     ) + ", " +
        std::string("fastest=") + std::to_string(m_iFastestLaps ) + ", " +
        std::string("DNF="    ) + std::to_string(m_iDidNotFinish) + "\n";

      return s;
    }

    SChampionshipRace::SChampionshipRace(const std::string& a_sTrack, int a_iPlayers, int a_iLaps) : m_sTrack(a_sTrack), m_iPlayers(a_iPlayers), m_iLaps(a_iLaps) {
    }

    SChampionshipRace::SChampionshipRace(const SChampionshipRace &a_cOther) : 
      m_sTrack  (a_cOther.m_sTrack), 
      m_iPlayers(a_cOther.m_iPlayers), 
      m_iLaps   (a_cOther.m_iLaps) 
    {
      for (int i = 0; i < 16; i++)
        m_aResult[i] = a_cOther.m_aResult[i];

      for (std::map<int, int>::const_iterator it = a_cOther.m_mAssignment.begin(); it != a_cOther.m_mAssignment.end(); it++)
        m_mAssignment[it->first] = it->second;
    }

    SChampionshipRace::~SChampionshipRace() {
      printf("Delete championship race\n");
    }

    SChampionshipRace::SChampionshipRace(const std::string& a_sData) : m_sTrack(""), m_iLaps(0) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      int l_iHead = l_cSerializer.getS32();
      if (l_iHead == c_iChampionshipRaceHead) {
        while (l_cSerializer.hasMoreMessages()) {
          int l_iField = l_cSerializer.getS32();

          switch (l_iField) {
            case c_iChampionshipRacePlayers:
              m_iPlayers = l_cSerializer.getS32();
              break;

            case c_iChampionshipRaceLaps:
              m_iLaps = l_cSerializer.getS32();
              break;

            case c_iChampionshipRaceTrack:
              m_sTrack = l_cSerializer.getString();
              break;

            case c_iChampionshipRaceResult: {
              int l_iCount = 0;
              do {
                int l_iValue = l_cSerializer.getS32();

                if (l_iValue == c_iChampionshipRacePlayer) {
                  m_aResult[l_iCount] = SRacePlayer(l_cSerializer.getString());
                }
                else if (l_iValue == c_iChampionshipRaceResultEnd) {
                  break;
                }
                else {
                  printf("Unknown marker %i in championship race result.", l_iValue);
                }
                l_iCount++;
              }
              while (true);
              break;
            }

            case c_iChampionshipRaceAssignBegin:
              while (true) {
                int l_iValue = l_cSerializer.getS32();
                if (l_iValue == c_iChampionshipRaceAssignEnd) {
                  break;
                }
                else {
                  m_mAssignment[l_iValue] = l_cSerializer.getS32();
                }
              }
              break;

            case c_iChampionshipRaceFooter:
              break;

            default:
              printf("Unkown field marker %i in championship race.\n", l_iField);
          }
        }
      }
      else printf("Unknown championship race header %i\n", l_iHead);
    }

    std::string SChampionshipRace::serialize() {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64();

      // Header
      l_cSerializer.addS32(c_iChampionshipRaceHead);

      // Track
      l_cSerializer.addS32(c_iChampionshipRaceTrack);
      l_cSerializer.addString(m_sTrack);

      // Laps
      l_cSerializer.addS32(c_iChampionshipRaceLaps);
      l_cSerializer.addS32(m_iLaps);

      // Number of players
      l_cSerializer.addS32(c_iChampionshipRacePlayers);
      l_cSerializer.addS32(m_iPlayers);

      // Players
      l_cSerializer.addS32(c_iChampionshipRaceResult);
      for (int i = 0; i < m_iPlayers; i++) {
        l_cSerializer.addS32(c_iChampionshipRacePlayer);
        l_cSerializer.addString(m_aResult[i].serialize());
      }
      l_cSerializer.addS32(c_iChampionshipRaceResultEnd);

      // Marble Assignment
      l_cSerializer.addS32(c_iChampionshipRaceAssignBegin);
      for (std::map<int, int>::iterator it = m_mAssignment.begin(); it != m_mAssignment.end(); it++) {
        l_cSerializer.addS32(it->first);
        l_cSerializer.addS32(it->second);
      }
      l_cSerializer.addS32(c_iChampionshipRaceAssignEnd);

      // Footer
      l_cSerializer.addS32(c_iChampionshipRaceFooter);

      return l_cSerializer.getMessageAsString();
    }

    std::string SChampionshipRace::to_string() {
      std::string s = "SChampionshipRace: " + 
        std::string("track="  ) +               m_sTrack     + ", " +
        std::string("laps="   ) + std::to_string(m_iLaps   ) + ", " +
        std::string("players=") + std::to_string(m_iPlayers) + "\n\nResult:\n";

      for (int i = 0; i < m_iPlayers; i++) {
        s += std::to_string(i) + ": " + m_aResult[i].to_string() + "\n";
      }

      s += "\n\nMarble Assignment:\n";

      for (std::map<int, int>::const_iterator it = m_mAssignment.begin(); it != m_mAssignment.end(); it++) {
        if (it != m_mAssignment.begin()) s += ", ";
        s += std::to_string(it->first) + "->" + std::to_string(it->second);
      }
      s += "\n\n";

      return s;
    }

    std::string SChampionshipRace::to_xml() const {
      std::string s = "    <race>\n";

      s += "      <track>"       +                m_sTrack    + "</track>\n";
      s += "      <playercount>" + std::to_string(m_iPlayers) + "</playercount>\n";
      s += "      <laps>"        + std::to_string(m_iLaps   ) + "</laps>\n";

      s += "      <result>\n";

      for (int i = 0; i < m_iPlayers; i++) {
        s += m_aResult[i].to_xml();
      }

      s += "      </result>\n";

      s += "      <marble_assignment>\n";

      for (std::map<int, int>::const_iterator it = m_mAssignment.begin(); it != m_mAssignment.end(); it++)
        s += "          <assignment marble=\"" + std::to_string(it->first) + "\" player=\"" + std::to_string(it->second) + "\" />\n";

      s += "        </marble_assignment>\n";

      s += "      </race>\n";

      return s;
    }

    SChampionship::SChampionship() : m_iClass(0), m_iGridSize(0), m_iGridOrder(0), m_bReverseGrid(false) {
    }

    SChampionship::SChampionship(int a_iClass, int a_iGridSize, int a_iGridOrder, bool a_bReverseGrid) : m_iClass(a_iClass), m_iGridSize(a_iGridSize), m_iGridOrder(a_iGridOrder), m_bReverseGrid(a_bReverseGrid) {
    }

    SChampionship::SChampionship(const SChampionship &a_cOther) : m_iClass(a_cOther.m_iClass) {
      for (std::vector<SChampionshipPlayer>::const_iterator it = a_cOther.m_vPlayers.begin(); it != a_cOther.m_vPlayers.end(); it++)
        m_vPlayers.push_back(SChampionshipPlayer(*it));

      for (std::vector<SChampionshipRace>::const_iterator it = a_cOther.m_vRaces.begin(); it != a_cOther.m_vRaces.end(); it++)
        m_vRaces.push_back(SChampionshipRace(*it));
    }

    SChampionship::SChampionship(const std::string& a_sData) : m_iClass(-1) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      int l_iHead = l_cSerializer.getS32();
      if (l_iHead == c_iChampionshipHeader) {
        while (l_cSerializer.hasMoreMessages()) {
          int l_iType = l_cSerializer.getS32();

          switch (l_iType) {
            case c_iChampionshipClass:
              m_iClass = l_cSerializer.getS32();
              break;

            case c_iChampionshipGridOrder:
              m_iGridOrder = l_cSerializer.getS32();
              break;

            case c_iChampionshipGridSize:
              m_iGridSize = l_cSerializer.getS32();
              break;

            case c_iChampionshipReverseGrid:
              m_bReverseGrid = l_cSerializer.getS32() != 0;
              break;

            case c_iChampionshipPlayersStart: {
              int l_iCountHead = l_cSerializer.getS32();
              if (l_iCountHead == c_iChampionshipPlayersCount) {
                int l_iNum = l_cSerializer.getS32();
                for (int i = 0; i < l_iNum; i++) {
                  int l_iData = l_cSerializer.getS32();
                  if (l_iData == c_iChampionshipPlayersData) {
                    m_vPlayers.push_back(SChampionshipPlayer(l_cSerializer.getString()));
                  }
                  else printf("Invalid data header for championshipplayers: %i (%i of %i)\n", l_iData, i, l_iNum);
                }
              }
              else printf("Inavalid count header for championship players: %i\n", l_iCountHead);
              break;
            }

            case c_iChampionshipRacesStart: {
              int l_iRaceCount = l_cSerializer.getS32();
              if (l_iRaceCount == c_iChampionshipRacesCount) {
                int l_iNum = l_cSerializer.getS32();
                for (int i = 0; i < l_iNum; i++) {
                  int l_iData = l_cSerializer.getS32();
                  if (l_iData == c_iChampionshipRacesData) {
                    m_vRaces.push_back(SChampionshipRace(l_cSerializer.getString()));
                  }
                  else printf("Invalid race data header: %i (%i of %i)\n", l_iData, i, l_iNum);
                }
              }
              else printf("Invalid header for race count: %i\n", l_iRaceCount);
              break;
            }
          }
        }
      }
      else printf("Invalid header for championship: %i\n", l_iHead);
    }

    std::string SChampionship::serialize() {
      messages::CSerializer64 l_cSerializer;

      // Championship header
      l_cSerializer.addS32(c_iChampionshipHeader);

      // Championship class
      l_cSerializer.addS32(c_iChampionshipClass);
      l_cSerializer.addS32(m_iClass);

      // Grid order
      l_cSerializer.addS32(c_iChampionshipGridOrder);
      l_cSerializer.addS32(m_iGridOrder);

      // Grid size
      l_cSerializer.addS32(c_iChampionshipGridSize);
      l_cSerializer.addS32(m_iGridSize);

      // Reverse grid
      l_cSerializer.addS32(c_iChampionshipReverseGrid);
      l_cSerializer.addS32(m_bReverseGrid ? 1 : 0);

      // The players
      l_cSerializer.addS32(c_iChampionshipPlayersStart);
      l_cSerializer.addS32(c_iChampionshipPlayersCount);
      l_cSerializer.addS32((irr::s32)m_vPlayers.size());

      for (std::vector<SChampionshipPlayer>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        l_cSerializer.addS32(c_iChampionshipPlayersData);
        l_cSerializer.addString((*it).serialize());
      }

      l_cSerializer.addS32(c_iChampionshipPlayersEnd);

      // The races
      l_cSerializer.addS32(c_iChampionshipRacesStart);
      l_cSerializer.addS32(c_iChampionshipRacesCount);
      l_cSerializer.addS32((irr::s32)m_vRaces.size());

      for (std::vector<SChampionshipRace>::iterator it = m_vRaces.begin(); it != m_vRaces.end(); it++) {
        l_cSerializer.addS32(c_iChampionshipRacesData);
        l_cSerializer.addString((*it).serialize());
      }

      l_cSerializer.addS32(c_iChampionshipRacesFooter);

      return l_cSerializer.getMessageAsString();
    }

    SChampionshipRace* SChampionship::getLastRace() {
      if (m_vRaces.size() > 0)
        return &m_vRaces.back();
      else
        return nullptr;
    }

    /**
    * Save the result of the championship to an XML file
    * @param a_sPath the path to save the file to
    */
    void SChampionship::saveToXML(const std::string& a_sPath) {
      std::string l_sFile = "<?xml version=\"1.0\"?>\n";
      l_sFile += "<marblegp_championship>\n";

      l_sFile += "  <race_settings>\n";
      l_sFile += "    <class>"     + std::to_string(m_iClass    ) + "</class>\n";
      l_sFile += "    <gridsize>"  + std::to_string(m_iGridSize ) + "</gridsize>\n";
      l_sFile += "    <gridorder>" + std::to_string(m_iGridOrder) + "</gridorder>\n";
      l_sFile += std::string("    <reversegrid>") + (m_bReverseGrid ? "true" : "false") + "</reversegrid>\n";
      l_sFile += "  </race_settings>\n";

      l_sFile += "  <players>\n";

      std::vector<SChampionshipPlayer> l_vResult = getStandings();

      for (std::vector<SChampionshipPlayer>::const_iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++)
        l_sFile += (*it).to_xml();

      l_sFile += "  </players>\n";

      l_sFile += "  <races>\n";

      for (std::vector<SChampionshipRace>::const_iterator it = m_vRaces.begin(); it != m_vRaces.end(); it++)
        l_sFile += (*it).to_xml();

      l_sFile += "  </races>\n";

      l_sFile += "</marblegp_championship>\n";

      const char *s = a_sPath.c_str();
      irr::io::IWriteFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndWriteFile(s);

      if (l_pFile != nullptr) {
        l_pFile->write(l_sFile.c_str(), l_sFile.size());
        l_pFile->drop();
      }
    }

    void SChampionship::addRace(const SChampionshipRace& a_cRace) {
      m_vRaces.push_back(SChampionshipRace(a_cRace));

      // The scores for each finishing position, depending on the number of players
      int l_iScore[16][16] = {
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

      int l_iFastestTime = -1;
      
      std::vector<SChampionshipPlayer>::iterator l_itFastest = m_vPlayers.end();

      for (int i = 0; i < a_cRace.m_iPlayers; i++) {
        int l_iMarble = a_cRace.m_aResult[i].m_iId;
        if (a_cRace.m_mAssignment.find(l_iMarble) != a_cRace.m_mAssignment.end()) {
          int l_iId = a_cRace.m_mAssignment.find(l_iMarble)->second;

          for (std::vector<SChampionshipPlayer>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
            if ((*it).m_iPlayerId == l_iId) {
              int l_iDiff = l_iScore[a_cRace.m_iPlayers - 1][i];

              (*it).m_iPoints  += l_iDiff;
              (*it).m_iRespawn += a_cRace.m_aResult[i].m_iRespawn;
              (*it).m_iStunned += a_cRace.m_aResult[i].m_iStunned;

              // Was this the player's best finish?
              bool l_bBest = true;

              for (int j = 0; j <= i; j++) {
                if ((*it).m_aResult[j] > 0)
                  l_bBest = false;
              }

              if (l_bBest)
                (*it).m_iBestFinish = (int)m_vRaces.size();

              (*it).m_aResult[i]++;

              if (l_iFastestTime == -1 || a_cRace.m_aResult[i].m_iFastest < l_iFastestTime) {
                l_itFastest = it;
                l_iFastestTime = a_cRace.m_aResult[i].m_iFastest;
              }

              break;
            }
          }
        }
      }

      if (l_itFastest != m_vPlayers.end()) {
        (*l_itFastest).m_iFastestLaps++;
      }

      std::string l_sPath = helpers::ws2s(platform::portableGetDataPath()) + "championship_result.xml";
      saveToXML(l_sPath);
    }

    std::string SChampionship::to_string() {
      std::string s = "SChampionship: " + 
        std::string("class=") + std::to_string(m_iClass) + ", " +
        std::string("grid=" ) + std::to_string(m_iGridSize );

      if (m_bReverseGrid)
        s += " [reverted grid]";

      s += "\n\nPlayers:\n";

      for (std::vector<SChampionshipPlayer>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        s += (*it).to_string() + "\n";
      }

      s += "\nRaces:\n";

      for (std::vector<SChampionshipRace>::iterator it = m_vRaces.begin(); it != m_vRaces.end(); it++) {
        s += (*it).to_string() + "\n";
      }

      return s;
    }

    std::vector<SChampionshipPlayer> SChampionship::getStandings() {
      std::vector <SChampionshipPlayer> l_vRet;

      for (std::vector<SChampionshipPlayer>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
        l_vRet.push_back(SChampionshipPlayer(*it));
      }

      std::sort(l_vRet.begin(), l_vRet.end(), [](SChampionshipPlayer p1, SChampionshipPlayer p2) {
        if (p1.m_iPoints != p2.m_iPoints) {
          // The player with more points leads
          return p1.m_iPoints > p2.m_iPoints;
        }
        else {
          // If two players have equal points the
          // player with less respawns leads
          if (p1.m_iRespawn != p2.m_iRespawn) {
            return p1.m_iRespawn < p2.m_iRespawn;
          }

          // If this does not help the number of stuns
          // is taken into account
          if (p1.m_iStunned != p2.m_iStunned) {
            return p1.m_iStunned < p2.m_iStunned;
          }

          // Better race results define the ranking
          // if none of the above helps
          for (int i = 0; i < 16; i++) {
            if (p1.m_aResult[i] != p2.m_aResult[i]) {
              return p1.m_aResult[i] > p2.m_aResult[i];
            }
          }

          // Now we check for did not finishs, less
          // of those is better
          if (p1.m_iDidNotFinish != p2.m_iDidNotFinish) {
            return p1.m_iDidNotFinish < p2.m_iDidNotFinish;
          }

          // If we get here we use the first race the
          // player has scored his best finish - this
          // should do the job
          if (p1.m_iBestFinish != p2.m_iBestFinish) {
            return p1.m_iBestFinish < p2.m_iBestFinish;
          }

          // Nothing to distinguish yet? Then we take
          // the number of fastest laps in a race
          if (p1.m_iFastestLaps != p2.m_iFastestLaps) {
            return p1.m_iFastestLaps < p2.m_iFastestLaps;
          }

          // If nothing helps we take the player id
          return p1.m_iPlayerId < p2.m_iPlayerId;
        }
      });

      return l_vRet;
    }

    SRacePlayer::SRacePlayer() : 
      m_iId       (-1),
      m_iStunned  (0), 
      m_iRespawn  (0), 
      m_iDeficitL (0), 
      m_iDeficitA (0), 
      m_iPos      (99),     // Use very high initial position
      m_iFastest  (0), 
      m_iWithdrawn(-1), 
      m_bWithdrawn(false),
      m_bFinished (false)
    {
    }

    SRacePlayer::SRacePlayer(const SRacePlayer& a_cOther) : 
      m_iId       (a_cOther.m_iId), 
      m_iStunned  (a_cOther.m_iStunned), 
      m_iRespawn  (a_cOther.m_iRespawn), 
      m_iDeficitL (a_cOther.m_iDeficitL), 
      m_iDeficitA (a_cOther.m_iDeficitA), 
      m_iPos      (a_cOther.m_iPos), 
      m_iFastest  (a_cOther.m_iFastest), 
      m_iWithdrawn(a_cOther.m_iWithdrawn),
      m_bWithdrawn(a_cOther.m_bWithdrawn),
      m_bFinished (a_cOther.m_bFinished)
    {
      for (std::vector<std::vector<int>>::const_iterator it = a_cOther.m_vLapCheckpoints.begin(); it != a_cOther.m_vLapCheckpoints.end(); it++) {
        m_vLapCheckpoints.push_back(std::vector<int>());

        for (std::vector<int>::const_iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
          m_vLapCheckpoints.back().push_back(*it2);
        }
      }
    }

    SRacePlayer::SRacePlayer(const std::string &a_sData) : 
      m_iId       (-1), 
      m_iStunned  (0), 
      m_iRespawn  (0), 
      m_iDeficitL (0), 
      m_iDeficitA (0), 
      m_iPos      (0), 
      m_iFastest  (0), 
      m_iWithdrawn(-1), 
      m_bWithdrawn(false),
      m_bFinished (false)
    {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      int l_iHead = l_cSerializer.getS32();
      if (l_iHead == c_iRacePlayerHeader) {
        while (l_cSerializer.hasMoreMessages()) {
          int l_iType = l_cSerializer.getS32();

          switch (l_iType) {
            case c_iRacePlayerId:
              m_iId = l_cSerializer.getS32();
              break;

            case c_iRacePlayerPos:
              m_iPos = l_cSerializer.getS32();
              break;

            case c_iRacePlayerDeficit:
              m_iDeficitL = l_cSerializer.getS32();
              break;

            case c_iRacePlayerFastest:
              m_iFastest = l_cSerializer.getS32();
              break;

            case c_iRacePlayerStunned:
              m_iStunned = l_cSerializer.getS32();
              break;

            case c_iRacePlayerRespawn:
              m_iRespawn = l_cSerializer.getS32();
              break;

            case c_iRacePlayerWithdraw:
              m_iWithdrawn = l_cSerializer.getS32();
              m_bWithdrawn = l_cSerializer.getU8() != 0;
              break;

            case c_iRacePlayerLapTimeStart: {
              int l_iState = 0;
              while (l_cSerializer.hasMoreMessages()) {
                int l_iToken = l_cSerializer.getS32();

                if (l_iToken == c_iRacePlayerCpTimesStart) {
                  l_iState = 1;
                  m_vLapCheckpoints.push_back(std::vector<int>());
                }
                else if (l_iToken == c_iRacePlayerCpTimesEnd)
                  l_iState = 0;
                else if (l_iToken == c_iRacePlayerLapTimeEnd) {
                  break;
                }
                else {
                  if (m_vLapCheckpoints.size() > 0)
                    m_vLapCheckpoints.back().push_back(l_iToken);
                  else
                    printf("Error while decoding player's lap times.\n");
                }
                
                
              }
              break;
            }

            case c_iRacePlayerFooter:
              break;

            default:
              printf("Unknown data marker in race player structure: %i\n", l_iType);
          }
        }
      }
      else printf("Invalid header for the race player (%i)\n", l_iHead);
    }

    int SRacePlayer::getRaceTime() {
      if (m_vLapCheckpoints.size() > 0) {
        if (m_vLapCheckpoints.back().size() > 0) {
          return (int)m_vLapCheckpoints.back().back();
        }
        else return 0;
      }
      else return 0;
    }

    int SRacePlayer::getLapNo() {
      return (int)m_vLapCheckpoints.size();
    }

    std::string SRacePlayer::serialize() {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64();

      // Start of the structure
      l_cSerializer.addS32(c_iRacePlayerHeader);

      // ID of the player in the race (aka marble id)
      l_cSerializer.addS32(c_iRacePlayerId);
      l_cSerializer.addS32(m_iId);

      // Finishing position of the player
      l_cSerializer.addS32(c_iRacePlayerPos);
      l_cSerializer.addS32(m_iPos);

      // Deficit of the player on the leader
      l_cSerializer.addS32(c_iRacePlayerDeficit);
      l_cSerializer.addS32(m_iDeficitL);

      // Fastest lap of the player
      l_cSerializer.addS32(c_iRacePlayerFastest);
      l_cSerializer.addS32(m_iFastest);

      // Number of stuns of the player
      l_cSerializer.addS32(c_iRacePlayerStunned);
      l_cSerializer.addS32(m_iStunned);

      // Number of respawns of the player
      l_cSerializer.addS32(c_iRacePlayerRespawn);
      l_cSerializer.addS32(m_iRespawn);

      // Withdrawn from race data
      l_cSerializer.addS32(c_iRacePlayerWithdraw);
      l_cSerializer.addS32(m_iWithdrawn);
      l_cSerializer.addU8(m_bWithdrawn ? 1 : 0);

      // Lap and checkpoint times
      l_cSerializer.addS32(c_iRacePlayerLapTimeStart);

      for (std::vector<std::vector<int>>::const_iterator l_itLap = m_vLapCheckpoints.begin(); l_itLap != m_vLapCheckpoints.end(); l_itLap++) {
        l_cSerializer.addS32(c_iRacePlayerCpTimesStart);

        for (std::vector<int>::const_iterator l_itCp = (*l_itLap).begin(); l_itCp != (*l_itLap).end(); l_itCp++) {
          l_cSerializer.addS32(*l_itCp);
        }

        l_cSerializer.addS32(c_iRacePlayerCpTimesEnd);
      }

      l_cSerializer.addS32(c_iRacePlayerLapTimeEnd);

      // End of the data structure
      l_cSerializer.addS32(c_iRacePlayerFooter);

      return l_cSerializer.getMessageAsString();
    }

    std::string SRacePlayer::to_string() {
      return "SRacePlayer: " +
        std::string("id="      ) + std::to_string(m_iId      ) + ", " +
        std::string("pos="     ) + std::to_string(m_iPos     ) + ", " +
        std::string("deficit=" ) + std::to_string(m_iDeficitL) + ", " +
        std::string("fastest=" ) + std::to_string(m_iFastest ) + ", " +
        std::string("stunned=" ) + std::to_string(m_iStunned ) + ", " +
        std::string("respawn=" ) + std::to_string(m_iRespawn );
    }

    std::string SRacePlayer::to_xml() const {
      std::string s = "      <raceplayer>\n";

      s += "        <id>"       + std::to_string(m_iId      ) + "</id>\n";
      s += "        <pos>"      + std::to_string(m_iPos     ) + "</pos>\n";
      s += "        <deficit>"  + std::to_string(m_iDeficitL) + "</deficit>\n";
      s += "        <fastest>"  + std::to_string(m_iFastest ) + "</fastest>\n";
      s += "        <stunned>"  + std::to_string(m_iStunned ) + "</stunned>\n";
      s += "        <respawn>"  + std::to_string(m_iRespawn ) + "</respawn>\n";
      s += "        <racetime>\n";

      for (std::vector<std::vector<int>>::const_iterator l_itLap = m_vLapCheckpoints.begin(); l_itLap != m_vLapCheckpoints.end(); l_itLap++) {
        s += "          <lap>\n";
        
        for (std::vector<int>::const_iterator l_itCp = (*l_itLap).begin(); l_itCp != (*l_itLap).end(); l_itCp++) {
          s += "            <checkpoint>" + std::to_string(*l_itCp) + "</checkpoint>\n";
        }

        s += "          </lap>\n";
      }

      s += "        </racetime>\n";

      s += "      </raceplayer>\n";

      return s;
    }

    SFreeGameSlots::SFreeGameSlots() {

    }

    SFreeGameSlots::SFreeGameSlots(const SFreeGameSlots& a_cOther) {
      for (std::vector<int>::const_iterator it = a_cOther.m_vSlots.begin(); it != a_cOther.m_vSlots.end(); it++)
        m_vSlots.push_back(*it);
    }

    SFreeGameSlots::SFreeGameSlots(const std::string& a_sData) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      irr::s32 l_iHead = l_cSerializer.getS32();

      if (l_iHead == c_iFreeGameSlots) {
        while (l_cSerializer.hasMoreMessages())
          m_vSlots.push_back(l_cSerializer.getS32());
      }
      else printf("Invalid header for free game slots data structure: %i\n", l_iHead);
    }

    std::string SFreeGameSlots::serialize() {
      messages::CSerializer64 l_cSerializer;
      
      l_cSerializer.addS32(c_iFreeGameSlots);

      for (std::vector<int>::iterator it = m_vSlots.begin(); it != m_vSlots.end(); it++)
        l_cSerializer.addS32(*it);

      return l_cSerializer.getMessageAsString();
    }

    SMarblePosition::SMarblePosition() : m_iMarbleId(-1), m_bContact(false) {
    }

    SMarbleAiData::SMarbleAiData() : m_iMarbleClass(2), m_iModeMap(0) {
      setDefaults();
    }

    SMarbleAiData::SMarbleAiData(SPlayerData::enAiHelp a_eHelp) : m_iMarbleClass(2), m_iModeMap(0) {
      switch (a_eHelp) {
        case data::SPlayerData::enAiHelp::Off    : m_iMarbleClass = 0; break;   // Help off    : AI is MarbleGP class (never called)
        case data::SPlayerData::enAiHelp::Display: m_iMarbleClass = 0; break;   // Help Display: AI is MarbleGP class
        case data::SPlayerData::enAiHelp::Low    : m_iMarbleClass = 0; break;   // Help Low    : AI is MarbleGP class
        case data::SPlayerData::enAiHelp::Medium : m_iMarbleClass = 1; break;   // Help Medium : AI is Marble2 class
        case data::SPlayerData::enAiHelp::High   : m_iMarbleClass = 0; break;   // Help High   : AI is Marble3  class
        case data::SPlayerData::enAiHelp::BotMgp : m_iMarbleClass = 3; break;   // Help Bot    : AI is MarbleGP class
        case data::SPlayerData::enAiHelp::BotMb2 : m_iMarbleClass = 4; break;   // Help Bot    : AI is Marble2 class
        case data::SPlayerData::enAiHelp::BotMb3 : m_iMarbleClass = 5; break;   // Help Bot    : AI is Marble3 class
      }

      setDefaults();
    }

    SMarbleAiData::SMarbleAiData(const std::string& a_sData) : m_iMarbleClass(0), m_iModeMap(0) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sData.c_str());

      irr::s32 l_iHead = l_cSerializer.getS32();

      if (l_iHead == c_iMarbleData) {
        while (l_cSerializer.hasMoreMessages()) {
          irr::s32 l_iMarker = l_cSerializer.getS32();

          switch (l_iMarker) {
            case c_iMarbleClass:
              m_iMarbleClass = l_cSerializer.getS32();
              setDefaults();
              break;

            case c_iMarbleModes:
              m_iModeMap = l_cSerializer.getS32();
              break;
          }
        }
      }
    }

    void SMarbleAiData::setDefaults() {
      switch (m_iMarbleClass) {
        case 0:
        case 3:
          // All modes available for MarbleGP
          m_iModeMap = (int)enAiMode::TimeAttack | (int)enAiMode::Cruise | (int)enAiMode::Default;

          m_fSpeedFactor1   = 80.0;
          m_fSpeedFactor2   = 75.0;
          m_fSpeedThreshold = 1.8f;
          m_iOvertake       = 85;
          m_iAvoid          = 10;
          m_iJumpMode       = 100;
          m_iJumpDir        = 100;
          m_iJumpVel        = 100;
          m_iPathSelect     = 97;
          m_iRoadBlock      = 95;
          m_fThrottleAdd    = 1.0f;
          break;

        case 1:
        case 4:
          // Only "Cruise" and "Default" modes for Marble2
          m_iModeMap = (int)enAiMode::Cruise | (int)enAiMode::Default;

          m_fSpeedFactor1   = 60.0;
          m_fSpeedFactor2   = 55.0;
          m_fSpeedThreshold = 1.3f;
          m_iOvertake       = 45;
          m_iAvoid          = 35;
          m_iJumpMode       = 97;
          m_iJumpDir        = 95;
          m_iJumpVel        = 95;
          m_iPathSelect     = 80;
          m_iRoadBlock      = 85;
          m_fThrottleAdd    = 0.02f;
          break;

        case 2:
        case 5:
          // Only "Default" mode for Marble3
          m_iModeMap = (int)enAiMode::Default;

          m_fSpeedFactor1   = 55.0;
          m_fSpeedFactor2   = 50.0;
          m_fSpeedThreshold = 1.25f;
          m_iOvertake       = 15;
          m_iAvoid          = 65;
          m_iJumpMode       = 95;
          m_iJumpDir        = 90;
          m_iJumpVel        = 93;
          m_iPathSelect     = 50;
          m_iRoadBlock      = 65;
          m_fThrottleAdd    = 0.01f;
          break;
      }
    }

    std::string SMarbleAiData::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iMarbleData);
      l_cSerializer.addS32(c_iMarbleClass);
      l_cSerializer.addS32(m_iMarbleClass);
      l_cSerializer.addS32(c_iMarbleModes);
      l_cSerializer.addS32(m_iModeMap);

      return l_cSerializer.getMessageAsString();
    }
  }
}