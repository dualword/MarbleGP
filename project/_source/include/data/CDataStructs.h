// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
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
    struct SGameGFX {
      bool m_bHightlight;   /**< Highlight the leader and the marble ahead*/
      bool m_bShowControls; /**< Show the marble controls */
      bool m_bShowRanking;  /**< Show the race ranking */

      SGameGFX();

      void copyFrom(const SGameGFX &a_cOther);
    };

    /**
    * @class SSettings
    * @author Christian Keimel
    * This data structure holds all settings data, e.g. resolution and sound volume
    */
    struct SSettings {
      int m_iResolutionW;   /**< Selected window width */
      int m_iResolutionH;   /**< Selected window height */
      int m_iShadows;       /**< Shadow details */
      int m_iAmbient;       /**< Ambient light */
      int m_iTouchType;     /**< Touch controller left or right (Android only) */

      float m_fSfxMaster;   /**< Sound master volume */
      float m_fSoundTrack;  /**< Sound track volume */
      float m_fSfxMenu;     /**< Menu sound effect volume */
      float m_fSfxGame;     /**< Game sound effect volume */

      bool m_bFullscreen;   /**< Fullscreen or not? */
      bool m_bUseMenuCtrl;  /**< Use the controller for the menu */
      bool m_bGfxChange;    /**< Gfx settings have changed */
      bool m_bTouchControl; /**< Flag for touch control (Android only) */

      std::string m_sController;  /**< The serialized menu controller configuration */

      SGameGFX m_aGameGFX[8];   /**< Separate settings for splitscreen up to 8 players */

      SSettings();

      void loadSettings(const std::map<std::string, std::string> &a_mData);   /**< Load the settings from a string-string map */
      void saveSettings(      std::map<std::string, std::string> &a_mData);   /**< Save the settings to a string-string map */

      void copyFrom(const SSettings &a_cOther);
    };

    /**
    * @class SPlayerData
    * @author Christian Keimel
    * This data structure holds the players' data
    */
    struct SPlayerData {
      /**
      * Enumeration for the AI help level of the player
      */
      enum class enAiHelp {
        Off,      /**< No AI help at all */
        Display,  /**< AI shows best path, steering and speed hints */
        Low,      /**< Display + adjust speed before jumps */
        Medium,   /**< Low + adjust steering to the correct value */
        High,     /**< Medium + try to make sure marble does not fall off the track */
        Bot       /**< Complete AI control */
      };

      enPlayerType m_eType;     /**< The type of player */
      enAiHelp     m_eAiHelp;   /**< The level of AI help */
      int          m_iPlayerId; /**< The ID of the player */
      int          m_iGridPos;  /**< Position in the grid (1..16) */
      int          m_iViewPort; /**< The viewport of this player (if any) */

      std::string m_sName;      /**< The player's name */
      std::string m_sTexture;   /**< The player's texture generation string */
      std::string m_sControls;  /**< The player's controls encoded as a string */

      

      SPlayerData();

      void copyFrom(const SPlayerData &a_cOther);
      
      void reset();

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);

      std::string toString();

      static std::vector<SPlayerData> createPlayerVector(const std::string a_sSerialized);
    };

    /**
    * @class SGameSettings
    * @author Christian Keimel
    * A class for the general game settings
    */
    struct SGameSettings {
      int m_iRaceClass;   /**< The race class (0 == Marbles3, 1 = Marbles2, 2 = MarbleGP) */
      int m_iGridPos;     /**< The grid positions of the next race (0 == Fixed, 1 == Last Race, 2 == Championship Standings, 3 == Random) */
      int m_iGridSize;    /**< The grid size, filled with AI players */
      int m_iAutoFinish;  /**< The auto finish mode (0 == All Players finished, 1 == Second to Last player finished, 2 == First Player Finished, 3 == All Players and AI finished) */

      bool m_bReverseGrid;      /**< Shall the grid be reversed? */
      bool m_bRandomFirstRace;  /**< Shall the grid of the first race be randomized? */
      bool m_bFillGridAI;       /**< Should the grid be filled with AI players? */

      SGameSettings();

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);
    };

    /**
    * @class SGameData
    * @author Christian Keimel
    * This struct holds the data for the upcoming race
    */
    struct SGameData {
      /**
      * The type of race for this instance of the application
      */
      enum class enType {
        Local,      /**< The race is local. Might be a network race running on the server */
        Network,    /**< The instance is a network client */
        ViewTrack,  /**< The race is a view track race */
        Replay      /**< The race is a race replay */
      };

      enType      m_eType;    /**< The type of race */
      std::string m_sTrack;   /**< The track */
      int         m_iLaps;    /**< The number of laps */
      int         m_iClass;   /**< The race class */

      SGameData();
      SGameData(enType a_eType, const std::string &a_sTrack, int a_iLaps, int a_iClass);
      SGameData(const std::string &a_sData);

      std::string serialize();
    };

    /**
    * @class SRacePlayers
    * @author Christian Keimel
    * This data struct holds the players of a race / championship 
    */
    struct SRacePlayers {
      std::vector<SPlayerData> m_vPlayers;  /**< The players of the race / championship */

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);

      std::string toString();
    };

    /**
    * @class SRacePlayer
    * @author Christian Keimel
    * This data structure holds all data to keep up with
    * a player during a race in the dynamics thread
    */
    struct SRacePlayer {
      int m_iId;          /**< Marble ID */
      int m_iCpCount;     /**< Number of passed checkpoints*/
      int m_iStunned;     /**< Stunned counter */
      int m_iRespawn;     /**< Respawn counter */
      int m_iLapNo;       /**< The current lap */
      int m_iDeficitL;    /**< Deficit to the leader */
      int m_iDeficitA;    /**< Deficit to the player ahead */
      int m_iLastCp;      /**< The time of the last checkpoint */
      int m_iPos;         /**< The position of the player in the race */
      int m_iFastest;     /**< Fastest lap of the player */
      int m_iLapStart;    /**< Start of the current lap */
      int m_iWithdrawn;   /**< The step when the player has withdrawn from the race */
      bool m_bWithdrawn;  /**< Has the player withdrawn from the race? */
      bool m_bFinished;   /**< Has the player finished the race? */

      std::vector<int> m_vCpTimes;

      SRacePlayer();
      SRacePlayer(const SRacePlayer &a_cOther);
      SRacePlayer(const std::string &a_sData);

      std::string serialize();

      std::string to_string();
    };

    /**
    * @class SChampionshipPlayer
    * @author Christian Keimel
    * This data struct stores the data of a player during a championship
    */
    struct SChampionshipPlayer {
      int         m_iPlayerId;      /**< The player ID (0..16) */
      std::string m_sName;          /**< Name */
      int         m_aResult[16];    /**< The race positions */
      int         m_iPoints;        /**< Points of the player */
      int         m_iRespawn;       /**< The total number of respawns */
      int         m_iStunned;       /**< The total number of stuns */
      int         m_iFastestLaps;   /**< Race in which this player made the fastest lap */
      int         m_iDidNotFinish;  /**< Number of race a player hasn't finished */

      SChampionshipPlayer(int a_iPlayerId, const std::string &a_sName);
      SChampionshipPlayer(const std::string &a_sData);
      SChampionshipPlayer(const SChampionshipPlayer &a_cOther);

      std::string serialize();

      std::string to_string();
    };

    /**
    * @class SChampionshipRace
    * @author Christian Keimel
    * Used to store the races of a championship
    */
    struct SChampionshipRace {
      std::string m_sTrack;       /**< The track of the race */
      int         m_iPlayers;     /**< The number of players of the race*/
      int         m_iLaps;        /**< The number of laps of the race */
      SRacePlayer m_aResult[16];  /**< The race result */

      std::map<int, int> m_mAssignment;   /**< The assignment of marbles (key == marble id) to players (value == player id) in the race */

      SChampionshipRace(const std::string &a_sTrack, int a_iPlayers, int a_iLaps);
      SChampionshipRace(const std::string &a_sData);
      SChampionshipRace(const SChampionshipRace &a_cOther);

      std::string serialize();

      std::string to_string();
    };

    /**
    * @class SChampionship
    * @author Christian Keimel
    * This data structure stores
    */
    struct SChampionship {
      std::vector<SChampionshipPlayer> m_vPlayers;  /**< The players of the championship */
      std::vector<SChampionshipRace  > m_vRaces;    /**< The races of the championship */

      int m_iClass;     /**< Class of the championship (0 == Marbles3, 1 == Marbles2, 2 == MarbleGP) */
      int m_iGridSize;  /**< Grid size (0..16) */
      int m_iGridOrder; /**< The grid order (0 == Last Race, 1 == Standings, 2 == Random) */

      bool m_bReverseGrid;  /**< Reverse the grid? */

      SChampionship(int a_iClass, int a_iGridSize, int a_iGridOrder, bool a_bReverseGrid);
      SChampionship(const std::string &a_sData);
      SChampionship(const SChampionship &a_sOther);

      std::string serialize();

      std::vector<SChampionshipPlayer> getStandings();

      void addRace(const SChampionshipRace &a_cRace);

      SChampionshipRace *getLastRace();

      std::string to_string();
    };

    /**
    * A little struct for storing the available free game slots in the globals
    */
    struct SFreeGameSlots {
      std::vector<int> m_vSlots;

      SFreeGameSlots();
      SFreeGameSlots(const SFreeGameSlots &a_cOther);
      SFreeGameSlots(const std::string &a_sData);

      std::string serialize();
    };
  }
}

