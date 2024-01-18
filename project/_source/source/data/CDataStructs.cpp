// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <messages/CMessageHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <json/CIrrJSON.h>
#include <Defines.h>
#include <CGlobal.h>
#include <algorithm>
#include <ostream>
#include <random>
#include <vector>
#include <string>

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
    const irr::s32 c_iPlayerDeviation    = 33;  /**< Marker for the player's power deviation (AI Marbles only) */

    // Game Settings
    const irr::s32 c_iGameSettings    = 42;   /**< Marker for the game settings */
    const irr::s32 c_iRaceClass       = 43;   /**< Marker for the race class */
    const irr::s32 c_iGridSize        = 44;   /**< Marker for the grid size */
    const irr::s32 c_iAutoFinish      = 45;   /**< Marker for the auto finish type */
    const irr::s32 c_iReverseGrid     = 46;   /**< Marker for the reverse grid flag */
    const irr::s32 c_iFirstRaceRandom = 47;   /**< Marker for the randomize first race flag */
    const irr::s32 c_iFillGrid        = 48;   /**< Marker for the fill grid with AI flag */

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
    const irr::s32 c_iChampionshipReverseGrid  = -103;    /**< Marker for the the championship class */
    const irr::s32 c_iChampionshipPlayersStart = -104;    /**< Marker for the the start of the championship players */
    const irr::s32 c_iChampionshipPlayersCount = -105;    /**< Marker for the the number of championship players */
    const irr::s32 c_iChampionshipPlayersData  = -106;    /**< Marker for the the start of a championship player */
    const irr::s32 c_iChampionshipPlayersEnd   = -107;    /**< Marker for the the end of the players of the championship */
    const irr::s32 c_iChampionshipRacesStart   = -108;    /**< Marker for the the start of the championship races */
    const irr::s32 c_iChampionshipRacesCount   = -109;    /**< Marker for the the number of championship races */
    const irr::s32 c_iChampionshipRacesData    = -110;    /**< Marker for the start of a championship race structure */
    const irr::s32 c_iChampionshipRacesFooter  = -111;    /**< Marker for the end of the championship data structure */

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
    const irr::s32 c_iGameTrack = -151;   /**< The track of the upcoming race */
    const irr::s32 c_iGameInfo  = -152;   /**< Additional information about the race */
    const irr::s32 c_iGameLaps  = -153;   /**< The laps of the race */
    const irr::s32 c_iTutorial  = -154;   /**< Is this race a tutorial race? */
    const irr::s32 c_iGrid      = -155;   /**< A starting grid position */

    // Marble AI Class
    const irr::s32 c_iMarbleData  = -170;   /**< Header for the Marble AI data */
    const irr::s32 c_iMarbleClass = -171;   /**< Marker for the marble class */
    const irr::s32 c_iMarbleModes = -172;   /**< Marker for the AI mode flags */

    // MarbleGP Cup
    const irr::s32 c_iMarbleGpCupHead   = -200;   /**< Header for MarbleGP Cup data */
    const irr::s32 c_iMarbleGpCupRace   = -201;   /**< Header for a cup race */
    const irr::s32 c_iMarbleGpCupFooter = -202;   /**< End of the MarbleGP Cup data */

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
      m_iWizardDfc   (1),
      m_iWizardGmt   (0),
      m_fSfxMaster   (1.0f),
      m_fSoundTrack  (1.0f),
      m_fSfxMenu     (1.0f),
      m_fSfxGame     (1.0f),
      m_bFullscreen  (false),
      m_bGfxChange   (false),
      m_bMenuPad     (false),
      m_bDebugAIPath (false),
      m_bDebugAIDice (false),
#ifdef _ANDROID
      m_bVirtualKeys (true),
      m_sController("DustbinTouchControl"),
#else
      m_bVirtualKeys (false),
      m_sController  ("DustbinController;control;f%3bl%3bForward%3bh%3ba%3bn%3bM%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bBackward%3bh%3ba%3bn%3bO%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bLeft%3bh%3ba%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRight%3bh%3ba%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bBrake%3bh%3ba%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRearview%3bh%3ba%3bn%3bj%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRespawn%3bh%3ba%3bn%3bn%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bPause%3bh%3ba%3bn%3bt%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bCancel%2520Race%3bh%3ba%3bn%3bB%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb"),
#endif
      m_bNoPlayerRace(false),
      m_sMenuCtrl    ("Off")
    {
    }

    void SSettings::loadSettings(const std::map<std::string, std::string>& a_mData) {
      if (a_mData.find("resolution_w") != a_mData.end()) m_iResolutionW  = std::atoi(a_mData.at("resolution_w").c_str());
      if (a_mData.find("resolution_h") != a_mData.end()) m_iResolutionH  = std::atoi(a_mData.at("resolution_h").c_str());
      if (a_mData.find("shadows"     ) != a_mData.end()) m_iShadows      = std::atoi(a_mData.at("shadows"     ).c_str());
      if (a_mData.find("ambient"     ) != a_mData.end()) m_iAmbient      = std::atoi(a_mData.at("ambient"     ).c_str());
      if (a_mData.find("wizardfdc"   ) != a_mData.end()) m_iWizardDfc    = std::atoi(a_mData.at("wizardfdc"   ).c_str());
      if (a_mData.find("wizardgmt"   ) != a_mData.end()) m_iWizardGmt    = std::atoi(a_mData.at("wizardgmt"   ).c_str());

      if (a_mData.find("fullscreen"  ) != a_mData.end()) m_bFullscreen   = a_mData.at("fullscreen"  ) == "true";
      if (a_mData.find("debugaipath" ) != a_mData.end()) m_bDebugAIPath  = a_mData.at("debugaipath" ) == "true";
      if (a_mData.find("debugaidice" ) != a_mData.end()) m_bDebugAIDice  = a_mData.at("debugaidice" ) == "true";
      if (a_mData.find("noplayerrace") != a_mData.end()) m_bNoPlayerRace = a_mData.at("noplayerrace") == "true";

      if (a_mData.find("sfx_master") != a_mData.end()) m_fSfxMaster  = (float)std::atof(a_mData.at("sfx_master").c_str());
      if (a_mData.find("soundtrack") != a_mData.end()) m_fSoundTrack = (float)std::atof(a_mData.at("soundtrack").c_str());
      if (a_mData.find("sfx_menu"  ) != a_mData.end()) m_fSfxMenu    = (float)std::atof(a_mData.at("sfx_menu"  ).c_str());
      if (a_mData.find("sfx_game"  ) != a_mData.end()) m_fSfxGame    = (float)std::atof(a_mData.at("sfx_game"  ).c_str());

      if (a_mData.find("menu_control") != a_mData.end()) m_sController = a_mData.at("menu_control");
      if (a_mData.find("menuctrl"    ) != a_mData.end()) m_sMenuCtrl   = a_mData.at("menuctrl"    );

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
      a_mData["wizardfdc"   ] = std::to_string(m_iWizardDfc  );
      a_mData["wizardgmt"   ] = std::to_string(m_iWizardGmt  );

      a_mData["fullscreen"  ] = m_bFullscreen  ? "true" : "false";
      a_mData["debugaipath" ] = m_bDebugAIPath ? "true" : "false";
      a_mData["debugaidice" ] = m_bDebugAIDice ? "true" : "false";
      a_mData["noplayerrace"] = m_bNoPlayerRace  ? "true" : "false";

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
      a_mData["menuctrl"    ] = m_sMenuCtrl  ;

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
      m_sController   = a_cOther.m_sController;
      m_bVirtualKeys  = a_cOther.m_bVirtualKeys;
      m_bDebugAIPath  = a_cOther.m_bDebugAIPath;
      m_bDebugAIDice  = a_cOther.m_bDebugAIDice;
      m_sMenuCtrl     = a_cOther.m_sMenuCtrl;
      m_bNoPlayerRace   = a_cOther.m_bNoPlayerRace;

      for (int i = 0; i < 8; i++)
        m_aGameGFX[i].copyFrom(a_cOther.m_aGameGFX[i]);
    }

    SPlayerData::SPlayerData() :
      m_eType        (enPlayerType::Local),
      m_eAiHelp      (enAiHelp::Medium),
      m_iPlayerId    (-1),
      m_iGridPos     (0 ),
      m_iViewPort    (-1),
      m_fDeviation   (0.0f),
      m_sName        (""),
      m_sTexture     (""),
      m_sControls    (""),
      m_sShortName   (""),
      m_wsShortName  (L"")
    {
      m_sControls = c_sDefaultControls;
    }

    void SPlayerData::reset() {
      m_iPlayerId = -1;
      m_sName     = "";
      m_sTexture  = "";
      m_sControls = "";
    }

    void SPlayerData::copyFrom(const SPlayerData& a_cOther) {
      m_eType         = a_cOther.m_eType;
      m_sName         = a_cOther.m_sName;
      m_sTexture      = a_cOther.m_sTexture;
      m_sControls     = a_cOther.m_sControls;
      m_sShortName    = a_cOther.m_sShortName;
      m_wsShortName   = a_cOther.m_wsShortName;
      m_iPlayerId     = a_cOther.m_iPlayerId;
      m_eAiHelp       = a_cOther.m_eAiHelp;
      m_iGridPos      = a_cOther.m_iGridPos;
      m_iViewPort     = a_cOther.m_iViewPort;
      m_fDeviation    = a_cOther.m_fDeviation;
    }

    std::string SPlayerData::serialize() const {
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

        l_cSerializer.addS32(c_iPlayerDeviation);
        l_cSerializer.addF32(m_fDeviation);

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
            case c_iPlayerType        : m_eType         = (enPlayerType)l_cSerializer.getS32   ()     ; break;
            case c_iPlayerName        : m_sName         =               l_cSerializer.getString()     ; break;
            case c_iPlayerControls    : m_sControls     =               l_cSerializer.getString()     ; break;
            case c_iPlayerTexture     : m_sTexture      =               l_cSerializer.getString()     ; break;
            case c_iPlayerAiHelp      : m_eAiHelp       = (enAiHelp    )l_cSerializer.getS32   ()     ; break;
            case c_iPlayerGridPos     : m_iGridPos      =               l_cSerializer.getS32   ()     ; break;
            case c_iPlayerViewPort    : m_iViewPort     =               l_cSerializer.getS32   ()     ; break;
            case c_iPlayerShortName   : m_sShortName    =               l_cSerializer.getString()     ; m_wsShortName = helpers::s2ws(m_sShortName); break;
            case c_iPlayerDeviation   : m_fDeviation    =               l_cSerializer.getF32   ()     ; break;
            case c_iPlayerDataEnd     : return true;
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

      // s += "  Texture: \"" + m_sTexture + "\"\n";
      // s += "  Controls: \"" + m_sControls + "\"\n";
      s += "  Grid Pos: " + std::to_string(m_iGridPos) + "\n"; 
      s += "  Viewport: " + std::to_string(m_iViewPort) + "\n";
      s += "  Deviation: " + std::to_string(m_fDeviation) + "\n";

      s += "  Ai Help: ";

      std::vector<std::string> l_vAiOptions = helpers::getAiHelpOptions();

      if ((int)m_eAiHelp >= 0 && (int)m_eAiHelp < l_vAiOptions.size())
        s += "\"" + l_vAiOptions[(int)m_eAiHelp] + "\"";

      return s;
    }

    /**
    * Is this player a bot?
    * @return "true" if this is a bot, "false" otherwise
    */
    bool SPlayerData::isBot() {
      return m_eAiHelp == enAiHelp::BotMgp || m_eAiHelp == enAiHelp::BotMb2 || m_eAiHelp == enAiHelp::BotMb3;
    }

    /**
    * Convert this profile to a JSON string
    * @return a JSON string with the data of this profile
    */
    std::string SPlayerData::to_json() {
      std::string l_sReturn = "{";

      l_sReturn += "\"name\": \"" + m_sName + "\", ";
      l_sReturn += "\"short\": \"" + m_sShortName + "\", ";
      l_sReturn += "\"ai_help\": " + std::to_string((int)m_eAiHelp) + ",";

      l_sReturn += "\"texture\": \"" + m_sTexture + "\", \"controller\": ";

      controller::CControllerGame l_cCtrl;

      if (m_sControls == "DustbinTouchControl") {
        l_sReturn += "\"TouchControl\"";
      }
      else if (m_sControls == "DustbinGyroscope") {
        l_sReturn += "\"Gyroscope\"";
      }
      else {
        l_cCtrl.deserialize(m_sControls);
        switch ((*l_cCtrl.getInputs().begin()).m_eType) {
          case controller::CControllerBase::enInputType::JoyAxis:
          case controller::CControllerBase::enInputType::JoyButton:
          case controller::CControllerBase::enInputType::JoyPov:
            l_sReturn += "\"Gamepad\"";
            break;

          case controller::CControllerBase::enInputType::Key:
            l_sReturn += "\"Keyboard\"";
            break;
        }
      }

      return l_sReturn + ",\"ctrl_data\": \"" + messages::urlEncode(m_sControls) + "\" }";
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
      m_eRaceClass      (enRaceClass::Marble2),
      m_eAutoFinish     (enAutoFinish::AllPlayers),
      m_iGridSize       (1),
      m_bReverseGrid    (false),
      m_bRandomFirstRace(true),
      m_bFillGridAI     (true)
    {

    }

    std::string SGameSettings::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iGameSettings   );
      l_cSerializer.addS32(c_iRaceClass      ); l_cSerializer.addS32((irr::s32)m_eRaceClass );
      l_cSerializer.addS32(c_iGridSize       ); l_cSerializer.addS32(m_iGridSize  );
      l_cSerializer.addS32(c_iAutoFinish     ); l_cSerializer.addS32((irr::s32)m_eAutoFinish);
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
            case c_iRaceClass      : m_eRaceClass       = (enRaceClass )l_cSerializer.getS32()     ; break;
            case c_iGridSize       : m_iGridSize        =               l_cSerializer.getS32()     ; break;
            case c_iAutoFinish     : m_eAutoFinish      = (enAutoFinish)l_cSerializer.getS32()     ; break;
            case c_iReverseGrid    : m_bReverseGrid     =               l_cSerializer.getS32() != 0; break;
            case c_iFirstRaceRandom: m_bRandomFirstRace =               l_cSerializer.getS32() != 0; break;
            case c_iFillGrid       : m_bFillGridAI      =               l_cSerializer.getS32() != 0; break;
          }
        }
      }
      else if (a_sSerialized != "") {
        printf("Invalid game settings data.\n");
        return false;
      }
      return true;
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
      m_iLapNo    (0),
      m_iLapCp    (0),
      m_iLastCp   (0),
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
      m_iLapNo    (a_cOther.m_iLapNo),
      m_iLapCp    (a_cOther.m_iLapCp),
      m_iLastCp   (a_cOther.m_iLastCp),
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
      m_iLapNo    (0),
      m_iLapCp    (0),
      m_iLastCp   (0),
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

    int SRacePlayer::getRaceTime() const {
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

    /**
    * Auto finish the player, i.e. add checkpoint times until the current lap is finished
    * @param a_pOther the player ahead of the current, used for the calculation
    */
    void SRacePlayer::finishPlayer(SRacePlayer* a_pOther) {
      printf("Finishing player %i (%i)\n", m_iId, m_iPos);
      if (m_vLapCheckpoints.size() == 0) {
        m_iWithdrawn = 0;
      }
      else if (a_pOther->m_vLapCheckpoints.size() >= m_vLapCheckpoints.size()) {
        if (m_iDeficitA <= 0)
          m_iDeficitA = 90 + (90 * std::rand() / RAND_MAX);

        printf("  Deficit: %.2f\n", (double)m_iDeficitA / 120.0);
        size_t l_iIndex = m_vLapCheckpoints.size() - 1;

        for (size_t i = 0; i < a_pOther->m_vLapCheckpoints[l_iIndex].size(); i++) {
          if (i >= m_vLapCheckpoints.back().size()) {
            m_vLapCheckpoints.back().push_back(a_pOther->m_vLapCheckpoints[l_iIndex][i] + m_iDeficitA);
            printf("    Checkpoint time %i: %.2f (%.2f)\n", (int)i, (double)(a_pOther->m_vLapCheckpoints[l_iIndex][i] + m_iDeficitA) / 120.0f, (double)(a_pOther->m_vLapCheckpoints[l_iIndex][i]) / 120.0f);
          }
        }
      }
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
      std::string s = "<raceplayer>";

      s += "<id>"       + std::to_string(m_iId      ) + "</id>";
      s += "<pos>"      + std::to_string(m_iPos     ) + "</pos>";
      s += "<deficit>"  + std::to_string(m_iDeficitL) + "</deficit>";
      s += "<fastest>"  + std::to_string(m_iFastest ) + "</fastest>";
      s += "<stunned>"  + std::to_string(m_iStunned ) + "</stunned>";
      s += "<respawn>"  + std::to_string(m_iRespawn ) + "</respawn>";
      s += "<racetime total=\"" + std::to_string(getRaceTime()) + "\">";

      for (std::vector<std::vector<int>>::const_iterator l_itLap = m_vLapCheckpoints.begin(); l_itLap != m_vLapCheckpoints.end(); l_itLap++) {
        s += "<lap>";
        
        for (std::vector<int>::const_iterator l_itCp = (*l_itLap).begin(); l_itCp != (*l_itLap).end(); l_itCp++) {
          s += "<checkpoint>" + std::to_string(*l_itCp) + "</checkpoint>";
        }

        s += "</lap>";
      }

      s += "</racetime>";

      s += "</raceplayer>";

      return s;
    }

    std::string SRacePlayer::to_json() const {
      std::string s = "{";

      s += "\"id\":"       + std::to_string(m_iId      ) + ",";
      s += "\"pos\":"      + std::to_string(m_iPos     ) + ",";
      s += "\"deficit\":"  + std::to_string(m_iDeficitL) + ",";
      s += "\"fastest\":"  + std::to_string(m_iFastest ) + ",";
      s += "\"stunned\":"  + std::to_string(m_iStunned ) + ",";
      s += "\"respawn\":"  + std::to_string(m_iRespawn ) + ",";
      s += "\"racetime\":" + std::to_string(getRaceTime()) + ",";
      s += "\"laps\": [";

      for (std::vector<std::vector<int>>::const_iterator l_itLap = m_vLapCheckpoints.begin(); l_itLap != m_vLapCheckpoints.end(); l_itLap++) {
        if (l_itLap != m_vLapCheckpoints.begin())
          s += ",";

        s += "[";
        
        for (std::vector<int>::const_iterator l_itCp = (*l_itLap).begin(); l_itCp != (*l_itLap).end(); l_itCp++) {
          if (l_itCp != (*l_itLap).begin())
            s += ",";

          s += std::to_string(*l_itCp);
        }

        s += "]";
      }

      s += "]";

      s += "}";

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
        case data::SPlayerData::enAiHelp::Medium : m_iMarbleClass = 0; break;   // Help Medium : AI is MarbleGP class
        case data::SPlayerData::enAiHelp::High   : m_iMarbleClass = 1; break;   // Help High   : AI is Marble2  class
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
          m_iBestJumpVel    = 98;
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
          m_iJumpDir        = 90;
          m_iJumpVel        = 95;
          m_iBestJumpVel    = 60;
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
          m_iBestJumpVel    = 40;
          m_iJumpDir        = 80;
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

    SMarbleGpCup::SMarbleGpCup() : m_sName(""), m_sDescription(""), m_bUserDefined(false), m_iRaceCount(-1) {
    }

    SMarbleGpCup::SMarbleGpCup(const SMarbleGpCup& a_cOther) : m_sName(a_cOther.m_sName), m_sDescription(a_cOther.m_sDescription), m_bUserDefined(false), m_iRaceCount(a_cOther.m_iRaceCount) {
      for (std::vector<std::tuple<std::string, int>>::const_iterator l_itRace = a_cOther.m_vRaces.begin(); l_itRace != a_cOther.m_vRaces.end(); l_itRace++) {
        m_vRaces.push_back(std::make_tuple(std::get<0>(*l_itRace), std::get<1>(*l_itRace)));
      }
    }

    SMarbleGpCup::SMarbleGpCup(json::CIrrJSON *a_pJson) : m_sName(""), m_sDescription(""), m_bUserDefined(false), m_iRaceCount(-1) {
      loadFromJson(a_pJson);
      m_iRaceCount = (int)m_vRaces.size();
    }

    SMarbleGpCup::SMarbleGpCup(const std::string& a_cSerialized) : m_sName(""), m_sDescription(""), m_bUserDefined(false), m_iRaceCount(-1) {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_cSerialized.c_str());

      while (l_cSerializer.hasMoreMessages()) {
        irr::s32 l_iData = l_cSerializer.getS32();

        if (l_iData == c_iMarbleGpCupHead) {
          m_sName        = l_cSerializer.getString();
          m_sDescription = l_cSerializer.getString();
          m_iRaceCount   = l_cSerializer.getS32();
        }
        else if (l_iData == c_iMarbleGpCupRace) {
          std::string l_sTrack =      l_cSerializer.getString();
          int         l_iLaps  = (int)l_cSerializer.getS32   ();
          m_vRaces.push_back(std::make_tuple(l_sTrack, l_iLaps));
        }
        else if (l_iData == c_iMarbleGpCupFooter) {
          break;
        }
      }
    }

    /**
    * Serialize the cup for storage in the global data map
    * @return serialized string with the cup data
    */
    std::string SMarbleGpCup::serialize() {
      messages::CSerializer64 l_cSerializer;

      l_cSerializer.addS32(c_iMarbleGpCupHead);
      l_cSerializer.addString(m_sName);
      l_cSerializer.addString(m_sDescription);
      l_cSerializer.addS32(m_iRaceCount);

      for (auto l_cRace : m_vRaces) {
        l_cSerializer.addS32(c_iMarbleGpCupRace);
        l_cSerializer.addString(std::get<0>(l_cRace));
        l_cSerializer.addS32(std::get<1>(l_cRace));
      }

      l_cSerializer.addS32(c_iMarbleGpCupFooter);

      return l_cSerializer.getMessageAsString();
    }

    /**
    * Load the cup definition from a JSON. The json must be located at the start of the cup object
    * @param a_pJson the JSON object to parse
    */
    void SMarbleGpCup::loadFromJson(json::CIrrJSON* a_pJson) {
      if (a_pJson->getType() == json::CIrrJSON::enToken::ObjectStart) {
        int l_iState = 0;
        std::string l_sKey = "";
        std::string l_sTrack = "";
        int l_iLaps = 0;

        while (a_pJson->read()) {
          json::CIrrJSON::enToken l_eToken = a_pJson->getType();

          switch (l_iState) {
            case 0:
              if (l_eToken == json::CIrrJSON::enToken::ValueString) {
                l_sKey = a_pJson->asString();
                l_iState = 1;
              }
              break;

            case 1:
              if (l_eToken == json::CIrrJSON::enToken::Colon) {
                if (l_sKey == "name")
                  l_iState = 2;
                else if (l_sKey == "description")
                  l_iState = 3;
                else if (l_sKey == "races")
                  l_iState = 4;
              }
              break;

            case 2:
              if (l_eToken == json::CIrrJSON::enToken::ValueString) {
                m_sName = a_pJson->asString();
                l_iState = 0;
              }
              break;

            case 3:
              if (l_eToken == json::CIrrJSON::enToken::ValueString) {
                m_sDescription = a_pJson->asString();
                l_iState = 0;
              }
              break;

            case 4:
              if (l_eToken == json::CIrrJSON::enToken::ArrayStart)
                l_iState = 5;
              else if (l_eToken == json::CIrrJSON::enToken::ObjectEnd)
                return;
              break;

            case 5:
              if (l_eToken == json::CIrrJSON::enToken::ObjectStart) 
                l_iState = 6;
              else if (l_eToken == json::CIrrJSON::enToken::ArrayEnd)
                l_iState = 4;
              break;

            case 6:
              if (l_eToken == json::CIrrJSON::enToken::ValueString) {
                if (a_pJson->asString() == "track")
                  l_iState = 7;
                else if (a_pJson->asString() == "laps")
                  l_iState = 8;
                else
                  printf("Unexpected key \"%s\"\n", a_pJson->asString().c_str());
              }
              else if (l_eToken == json::CIrrJSON::enToken::ObjectEnd) {
                if (l_sTrack != "" && l_iLaps > 0)
                  m_vRaces.push_back(std::make_tuple(l_sTrack, l_iLaps));

                l_sTrack = "";
                l_iLaps  = -1;
                l_iState = 5;
              }
              break;

            case 7:
              if (l_eToken == json::CIrrJSON::enToken::ValueString) {
                l_sTrack = a_pJson->asString();
                l_iState = 6;
              }
              break;

            case 8:
              if (l_eToken == json::CIrrJSON::enToken::ValueInt) {
                l_iLaps = a_pJson->asInt();
                l_iState = 6;
              }
              break;
          }
        }
      }
      else {
        printf("Invalid cup definition!\n");
      }
    }
  }
}