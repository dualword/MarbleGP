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
      Ai        /**< An AI player (has no viewport) */
    };

    /**
    * @class SGameGFX
    * @author Christian Keimel
    * Gfx Settings for the game. Instances of this struct
    * are stored in an array to separate settings for each
    * splitscreen possibility
    */
    struct SGameGFX {
      bool m_bRearview,   /**< Show rearview camera? */
           m_bRanking,    /**< Show ranking? */
           m_bRaceTime,   /**< Show racetime? */
           m_bLapTimes;   /**< Show laptimes? */

      SGameGFX();

      void copyFrom(const SGameGFX &a_cOther);
    };

    /**
    * @class SSettings
    * @author Christian Keimel
    * This data structure holds all settings data, e.g. resolution and sound volume
    */
    struct SSettings {
      int m_iResolutionW,   /**< Selected window width */
          m_iResolutionH,   /**< Selected window height */
          m_iShadows,       /**< Shadow details */
          m_iAmbient,       /**< Ambient light */
          m_iTouchType;     /**< Touch controller left or right (Android only) */

      float m_fSfxMaster,   /**< Sound master volume */
            m_fSoundTrack,  /**< Sound track volume */
            m_fSfxMenu,     /**< Menu sound effect volume */
            m_fSfxGame;     /**< Game sound effect volume */

      bool m_bFullscreen,   /**< Fullscreen or not? */
           m_bUseMenuCtrl,  /**< Use the controller for the menu */
           m_bGfxChange,    /**< Gfx settings have changed */
           m_bTouchControl; /**< Flag for touch control (Android only) */

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
      int          m_iPlayerId, /**< The ID of the player */
                   m_iGridPos;  /**< Position in the grid (1..16) */

      std::string m_sName,      /**< The player's name */
                  m_sTexture,   /**< The player's texture generation string */
                  m_sControls;  /**< The player's controls encoded as a string */

      

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
      int m_iRaceClass,   /**< The race class (0 == Marbles3, 1 = Marbles2, 2 = MarbleGP) */
          m_iGridPos,     /**< The grid positions of the next race (0 == Fixed, 1 == Last Race, 2 == Championship Standings, 3 == Random) */
          m_iGridSize,    /**< The grid size, filled with AI players */
          m_iAiClass;     /**< The class of the AI players (0 == Marbles3, 1 == Marbles2, 2 == MarbleGP, 3 == Manual Configure, 4 == All Classes) */

      bool m_bReverseGrid,      /**< Shall the grid be reversed? */
           m_bRandomFirstRace,  /**< Shall the grid of the first race be randomized? */
           m_bFillGridAI;       /**< Should the grid be filled with AI players? */

      SGameSettings();

      std::string serialize();
      bool deserialize(const std::string &a_sSerialized);
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
      int m_iId,        /**< Marble ID */
        m_iCpCount,   /**< Number of passed checkpoints*/
        m_iStunned,   /**< Stunned counter */
        m_iRespawn,   /**< Respawn counter */
        m_iLapNo,     /**< The current lap */
        m_iDeficit,   /**< Deficit to the leader */
        m_iLastCp;    /**< The time of the last checkpoint */

      SRacePlayer() : m_iId(-1), m_iCpCount(0), m_iStunned(0), m_iRespawn(0), m_iLapNo(0), m_iDeficit(0), m_iLastCp(0) {
      }
    };
  }
}

