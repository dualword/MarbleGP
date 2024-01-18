// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <tuple>
#include <map>

namespace dustbin {
  namespace json {
    class CIrrJSON;
  }

  namespace data {
    /**
    * Enumeration for the player type
    */
    enum class enPlayerType {
      Local,    /**< A local player (with his own viewport) */
      Ai,       /**< An AI player (has no viewport) */
      Network   /**< A player connected by network */
    };

    /**
    * @class SGameGFX
    * @author Christian Keimel
    * Gfx Settings for the game. Instances of this struct
    * are stored in an array to separate settings for each
    * splitscreen possibility
    */
    typedef struct SGameGFX {
      bool m_bHighlight;   /**< Highlight the leader and the marble ahead*/
      bool m_bShowControls; /**< Show the marble controls */
      bool m_bShowRanking;  /**< Show the race ranking */
      bool m_bShowLapTimes; /**< Show the laptimes of the race? */

      SGameGFX();

      void copyFrom(const SGameGFX &a_cOther);
    } SGameGFX;

    /**
    * @class SSettings
    * @author Christian Keimel
    * This data structure holds all settings data, e.g. resolution and sound volume
    */
    typedef struct SSettings {
      int m_iResolutionW;   /**< Selected window width */
      int m_iResolutionH;   /**< Selected window height */
      int m_iShadows;       /**< Shadow details */
      int m_iAmbient;       /**< Ambient light */
      int m_iWizardDfc;     /**< Game Wizard: Difficulty */
      int m_iWizardGmt;     /**< Game Wizard: Game Type */

      float m_fSfxMaster;   /**< Sound master volume */
      float m_fSoundTrack;  /**< Sound track volume */
      float m_fSfxMenu;     /**< Menu sound effect volume */
      float m_fSfxGame;     /**< Game sound effect volume */

      bool m_bFullscreen;   /**< Fullscreen or not? */
      bool m_bGfxChange;    /**< Gfx settings have changed */
      bool m_bMenuPad;      /**< Was the menu controlled by Joypad? */
      bool m_bDebugAIPath;  /**< Show the AI path debug image (lower left) */
      bool m_bDebugAIDice;  /**< Show the AI dice debug image (lower right) */
      bool m_bVirtualKeys;  /**< Use virtual keyboard (always true for Android) */
      bool m_bNoPlayerRace; /**< Use the game wizard (true) or show all options */

      std::string m_sController;  /**< The serialized menu controller configuration */
      std::string m_sMenuCtrl;    /**< The menu controller setting (off, keyboard, gamepad) */

      SGameGFX m_aGameGFX[8];   /**< Separate settings for splitscreen up to 8 players */

      SSettings();

      void loadSettings(const std::map<std::string, std::string> &a_mData);   /**< Load the settings from a string-string map */
      void saveSettings(      std::map<std::string, std::string> &a_mData);   /**< Save the settings to a string-string map */

      void copyFrom(const SSettings &a_cOther);
    } SSettings;

    /**
    * @class SPlayerData
    * @author Christian Keimel
    * This data structure holds the players' data
    */
    typedef struct SPlayerData {
      /**
      * Enumeration for the AI help level of the player
      */
      enum class enAiHelp {
        Off,      /**< No AI help at all */
        Display,  /**< AI shows best path, steering and speed hints */
        Low,      /**< Display + adjust speed before jumps */
        Medium,   /**< Low + adjust steering to the correct value + brake + try to keep marbles on the road */
        High,     /**< Medium + full throttle control */
        BotMgp,   /**< Complete AI control (MarbleGP) */
        BotMb2,   /**< Complete AI control (Marble2) */
        BotMb3    /**< Complete AI control (Marble3) */
      };

      enPlayerType m_eType;         /**< The type of player */
      enAiHelp     m_eAiHelp;       /**< The level of AI help */
      int          m_iPlayerId;     /**< The ID of the player */
      int          m_iGridPos;      /**< Position in the grid (1..16) */
      int          m_iViewPort;     /**< The viewport of this player (if any) */
      float        m_fDeviation;    /**< The deviation in power for this marble (AI only) */

      std::string m_sName;      /**< The player's name */
      std::string m_sTexture;   /**< The player's texture generation string */
      std::string m_sControls;  /**< The player's controls encoded as a string */
      std::string m_sShortName; /**< Abbreviation of the name for ranking display */

      std::wstring m_wsShortName; /**< Abbreviation of the player's name as long string */

      SPlayerData();

      void copyFrom(const SPlayerData &a_cOther);
      
      void reset();

      std::string serialize() const;
      bool deserialize(const std::string &a_sSerialized);

      std::string toString();

      /**
      * Is this player a bot?
      * @return "true" if this is a bot, "false" otherwise
      */
      bool isBot();

      /**
      * Convert this profile to a JSON string
      * @return a JSON string with the data of this profile
      */
      std::string to_json();

      static std::vector<SPlayerData> createPlayerVector(const std::string a_sSerialized);
    } SPlayerData;

    /**
    * @class SGameSettings
    * @author Christian Keimel
    * A class for the general game settings
    */
    typedef struct SGameSettings {
      enum class enRaceClass {
        Marble3    = 0,     /**< The AI players of the race are Marble3 */
        Marble3_2  = 1,     /**< The field is filled with a mix of Marble3 and Marble2 AI players */
        Marble2    = 2,     /**< Every AI player of the race is Marble2 */
        Marble2_GP = 3,     /**< Some Marble2 and some MarbleGP AI players are added if wanted */
        MarbleGP   = 4,     /**< All AI opponents are MarbleGP, so this is the hardest class */
        AllClasses = 5      /**< A mixture of all classes is added if the field is filled */
      };

      enum class enAutoFinish {
        AllPlayers   = 0,       /**< The race is ended once all players have reached the finish line */
        SecondToLast = 1,       /**< Race ends once all players except for the last one have finished */
        FirstPlayer  = 2,       /**< The race ends once the first player has crossed the finish line */
        AllAndAi     = 3        /**< All marbles, human and AI alike, must have finished for the race to end */
      };

      enRaceClass  m_eRaceClass;   /**< The race class (0 == Marble3, 1 == Marble3 + 2, 2 == Marble2, 3 == Marble2 + GP, 4 == MarbleGP, 5 == All Classes) */
      enAutoFinish m_eAutoFinish;  /**< The auto finish mode (0 == All Players finished, 1 == Second to Last player finished, 2 == First Player Finished, 3 == All Players and AI finished) */

      int m_iGridSize;    /**< The grid size, filled with AI players */

      bool m_bReverseGrid;      /**< Shall the grid be reversed? */
      bool m_bRandomFirstRace;  /**< Shall the grid of the first race be randomized? */
      bool m_bFillGridAI;       /**< Should the grid be filled with AI players? */

      SGameSettings();

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);
    } SGameSettings;

    /**
    * @class SRacePlayers
    * @author Christian Keimel
    * This data struct holds the players of a race / championship 
    */
    typedef struct SRacePlayers {
      std::vector<SPlayerData> m_vPlayers;  /**< The players of the race / championship */

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);

      std::string toString();

      /**
      * Get a player by ID
      * @param a_iId player ID
      * @return pointer to the player data structure with the ID, nullptr if the ID was not found
      */
      SPlayerData *getPlayer(int a_iId);
    } SRacePlayers;

    /**
    * @class SRacePlayer
    * @author Christian Keimel
    * This data structure holds all data to keep up with
    * a player during a race in the dynamics thread
    */
    typedef struct SRacePlayer {
      int m_iId;          /**< Marble ID */
      int m_iStunned;     /**< Stunned counter */
      int m_iRespawn;     /**< Respawn counter */
      int m_iDeficitL;    /**< Deficit to the leader */
      int m_iDeficitA;    /**< Deficit to the player ahead */
      int m_iPos;         /**< The position of the player in the race */
      int m_iFastest;     /**< Fastest lap of the player */
      int m_iWithdrawn;   /**< The step when the player has withdrawn from the race */
      int m_iLapNo;       /**< The current lap of the player */
      int m_iLapCp;       /**< The current checkpoint in the lap */
      int m_iLastCp;      /**< Time of the last passed checkpoint */
      bool m_bWithdrawn;  /**< Has the player withdrawn from the race? */
      bool m_bFinished;   /**< Has the player finished the race? */

      std::vector<std::vector<int>> m_vLapCheckpoints;    /**< A vector that contains all checkpoint times for the laps */

      SRacePlayer();
      SRacePlayer(const SRacePlayer &a_cOther);
      SRacePlayer(const std::string &a_sData);

      /**
      * Get the race time of the player
      * @return the race time of the player
      */
      int getRaceTime() const;

      /**
      * Get the current lap of the player
      * @return the current lap of the player
      */
      int getLapNo();

      /**
      * Auto finish the player, i.e. add checkpoint times until the current lap is finished
      * @param a_pOther the player ahead of the current, used for the calculation
      */
      void finishPlayer(SRacePlayer *a_pOther);

      /**
      * Serialize the data structure to the internal data format
      * using the 64-char serializer
      * @return the serialized string
      */
      std::string serialize();

      /**
      * Convert the data structure to a human-readable string
      * @return a human-readable string
      */
      std::string to_string();

      /**
      * Convert the data structure to a XML string
      * @return a XML string
      */
      std::string to_xml() const;

      /**
      * Convert the data structure to a JSON string
      * @return a XML string
      */
      std::string to_json() const;
    } SRacePlayer;

    /**
    * A little struct for storing the available free game slots in the globals
    */
    typedef struct SFreeGameSlots {
      std::vector<int> m_vSlots;

      SFreeGameSlots();
      SFreeGameSlots(const SFreeGameSlots &a_cOther);
      SFreeGameSlots(const std::string &a_sData);

      std::string serialize();
    } SFreeGameSlots;


    /**
    * @class SMarblePosition
    * @author Christian Keimel
    * This struct helps the AI to keep track of the other marbles
    */
    typedef struct SMarblePosition {
      int                  m_iMarbleId;   /**< The ID of the marble */
      irr::core::vector3df m_cPosition;   /**< The marble's position */
      irr::core::vector3df m_cVelocity;   /**< The marble's velocity */
      irr::core::vector3df m_cDirection;  /**< Camera direction */
      irr::core::vector3df m_cCamera;     /**< Camera position */
      irr::core::vector3df m_cCameraUp;   /**< Up-vector of the camera */
      bool                 m_bContact;    /**< Does the marble touch the ground? */

      SMarblePosition();
    }
    SMarblePosition;


    /**
    * @class SMarbleGpCup
    * @author Christian Keimel
    * This struct holds information about MarbleGP Cups
    */
    typedef struct SMarbleGpCup {
      std::string m_sName;          /**< The name of the cup */
      std::string m_sDescription;   /**< The description of the cup */

      bool m_bUserDefined;    /**< Is this cup user defined? */

      int m_iRaceCount;   /**< The original number of races (During a cup races will be removed from m_vRaces) */

      std::vector<std::tuple<std::string, int>> m_vRaces;   /**< The cup's races (0 == track, 1 == laps) */

      SMarbleGpCup();
      SMarbleGpCup(const SMarbleGpCup &a_cOther);
      SMarbleGpCup(json::CIrrJSON *a_pJson);
      SMarbleGpCup(const std::string &a_cSerialized);

      /**
      * Load the cup definition from a JSON. The json must be located at the start of the cup object
      * @param a_pJson the JSON object to parse
      */
      void loadFromJson(json::CIrrJSON *a_pJson);

      /**
      * Serialize the cup for storage in the global data map
      * @return serialized string with the cup data
      */
      std::string serialize();
    }
    SMarbleGpCup;

    /**
    * @class SMarbleAiData
    * @author Christian Keimel
    * This struct holds all data for the AI
    */
    typedef struct SMarbleAiData {
      /**
      * Enum for all the AI mode flags. Evade and Off-Track are always possible
      */
      enum class enAiMode {
        Default = 0,    /**< Default mode is zero, always possible */
        Cruise = 1,
        TimeAttack = 2
      };

      int m_iMarbleClass;   /**< The marble class (0 == MarbleGP, 1 == Marble2, 2 == Marble3) */
      int m_iModeMap;       /**< Bit field of the available modes */
 
      float m_fSpeedFactor1;    /**< The first factor for calculating the wanted velocity */
      float m_fSpeedFactor2;    /**< The second factor for calculating the wanted velocity */
      float m_fSpeedThreshold;  /**< Threshold for using the second calculated point  */
      float m_fThrottleAdd;     /**< How fast does this AI player push the throttle to full? */

      int m_iOvertake;    /**< The skill value for overtake mode (0..100) */
      int m_iAvoid;       /**< The skill value for avoid other marble mode (0..100) */
      int m_iJumpMode;    /**< The skill value for switching to jump mode (0..100) */
      int m_iJumpDir;     /**< The skill value for the jump direction (0..100) */
      int m_iJumpVel;     /**< The skill value for the jump velocity (0..100) */
      int m_iBestJumpVel; /**< The skill value for the best jump velocity (0..100) */
      int m_iPathSelect;  /**< The skill value for selecting the correct path (by calling the LUA script) at turnoffs (0..100) */
      int m_iRoadBlock;   /**< The skill value for detecting road blocks (0..100) */
      
      SMarbleAiData();

      /**
      * Constructor with the AI help class. Values will be
      * filled with the class defaults
      */
      SMarbleAiData(SPlayerData::enAiHelp a_eHelp);

      /**
      * Constructor with serialized data
      * @param a_sData data to deserialize
      */
      SMarbleAiData(const std::string &a_sData);

      void setDefaults();

      std::string serialize();
    }
    SMarbleAiData;

#ifdef _ANDROID
    const std::string c_sDefaultControls = "";
#else
    const std::string c_sDefaultControls = "";
#endif
  }
}


