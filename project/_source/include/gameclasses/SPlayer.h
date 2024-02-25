// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <gameclasses/SMarbleNodes.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <controller/IController.h>
#include <data/CDataStructs.h>
#include <CGlobal.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace gameclasses {
    typedef struct SStandings SStandings;   /**< Forward declaration of SStandings */
    typedef struct SPlayer    SPlayer;      /**< Forward declaration of SPlayer */
    /**
    * @class SRaceData
    * @author Christian Keimel
    * This struct stores single race data of a player
    * @see SPlayer::m_cRaceData
    */
    typedef struct SRaceData {
      int  m_iPlayer;       /**< The player ID */
      int  m_iMarble;       /**< The marble ID */
      int  m_iPosition;     /**< The position in the race */
      int  m_iDiffLeader;   /**< The deficit to the leader */
      int  m_iDiffAhead;    /**< The deficit to the marble ahead */
      int  m_iState;        /**< The player's state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished) */
      int  m_iLapNo;        /**< The player's current lap */
      int  m_iGridPos;      /**< The player's position in the starting grid of the race */
      int  m_iFastestLap;   /**< Time of the fastest lap */
      int  m_iFastestLapNo; /**< Number of the fastest lap */
      bool m_bWithdrawn;    /**< Has the player withdrawn from the race? */

      std::vector<std::vector<int>> m_vLapCheckpoints;    /**< Time of the passed checkpoints per lap */

      SPlayer *m_pPlayer;   /**< The player this race data is assigned to */

      std::vector<int> m_vRespawn;    /**< Timestamps of the respawns of this player in the race */
      std::vector<int> m_vStunned;    /**< Timestamps of the stuns of this player in the race */

      /**
      * The default contructor
      */
      SRaceData();

      /**
      * De-serialization constructor
      * @param a_sData the data to de-serialize
      * @param a_pTournament the tournament
      */
      SRaceData(const std::string &a_sData, STournament *a_pTournament);

      /**
      * Store the data in a JSON string
      * @return a JSON string
      */
      std::string toJSON();

      /**
      * Serialize this race data
      */
      std::string serialize();
    }
    SRaceData;

    /**
    * @class SPlayer
    * @author Christian Keimel
    * This is the struct for the players
    */
    typedef struct SPlayer {
      int          m_iPlayer;           /**< The player id */
      int          m_iViewport;         /**< The player's viewport (-1 == has not viewport) */
      std::string  m_sName;             /**< The name of the player */
      std::wstring m_sWName;            /**< The player's name as wide string */
      std::string  m_sTexture;          /**< The texture of the player's marble */
      std::string  m_sController;       /**< The serialized controller configuration */
      std::string  m_sShortName;        /**< The short name of the player */
      std::wstring m_sNumber;           /**< The player's starting number */
      std::wstring m_wsShortName;       /**< The player's short name as wide string */
      bool         m_bShowRanking;      /**< Flag for some controllers to hide ranking and laptimes */

      irr::video::SColor m_cText;   /**< The text color (for the starting number in the ranking display) */
      irr::video::SColor m_cBack;   /**< The background color (for the starting number in the ranking display) */
      irr::video::SColor m_cFrme;   /**< The frame color (for the starting number in the ranking display) */

      data::enPlayerType m_eType;   /**< The type of player (local, AI, network) */

      data::SPlayerData::enAiHelp m_eAiHelp;    /**< The AI help level of this player */

      gameclasses::SMarbleNodes *m_pMarble;       /**< The marble of the player */
      controller::IController   *m_pController;   /**< The controller of this player */

      SRaceData *m_pRaceData;     /**< Data of a race */

      /**
      * Some debugging: dump the lap checkpoints vector to stdout
      */
      void dumpLapCheckpoints();

      /**
      * The default constructor
      */
      SPlayer();

      /**
      * Copy constructor
      * @param a_cOther the player to copy
      */
      SPlayer(const SPlayer &a_cOther);

      /**
      * The main constructor
      * @param a_iPlayer the player id
      * @param a_iViewport the viewport of the player (-1 == no viewport)
      * @param a_sName the name of the player
      * @param a_sTexture the texture string of the player's marble
      * @param a_sController the controller configuration string of the player
      * @param a_pMarble the marble of the player
      */
      SPlayer(
        int a_iPlayer, 
        int a_iViewport,
        const std::string& a_sName, 
        const std::string& a_sTexture, 
        const std::string &a_sController, 
        const std::string &a_sShortName, 
        data::SPlayerData::enAiHelp a_eAiHelp, 
        gameclasses::SMarbleNodes* a_pMarble, 
        data::enPlayerType a_eType);

      /**
      * The destructor
      */
      ~SPlayer();

      void setName(const std::string &a_sName);
      bool isBot();

      /**
      * Is this player in front of another player?
      * @param a_pOther the other player
      * @return true if this player is in front
      */
      bool isInFront(SPlayer *a_pOther);

      /**
      * Lap start callback
      * @param a_iStep the current step
      */
      void onLapStart(int a_iStep);

      /**
      * Checkpoint callback
      * @param a_iStep the step when the checkpoint was passed
      */
      void onCheckpoint(int a_iStep);

      /**
      * A callback for a changed state
      * @param a_iState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
      * @param a_iStep step when the change happened
      */
      void onStateChanged(int a_iState, int a_iStep);

      /**
      * Get the deficit to another player
      * @param a_pOther the other player
      * @param a_iSteps [out] the time deficit
      * @param a_iLaps [out] the lap deficit
      */
      void getDeficitTo(SPlayer *a_pOther, int &a_iSteps, int &a_iLaps);

      /**
      * Set the player's marble node
      * @param a_pMarble to marble node
      */
      void setMarbleNode(SMarbleNodes *a_pMarble);

      /**
      * Store the race data of this player to a JSON string
      * @return the JSON string
      */
      std::string toJSON();
    } SPlayer;

    /**
    * @class SRace
    * @author Christian Keimel
    * A data struct for a race
    */
    typedef struct SRace {
      std::string m_sTrack;   /**< The track of the race (folder name) */
      std::string m_sName ;   /**< The actual track name */
      std::string m_sInfo ;   /**< The information shown in the loading track screen */

      int m_iLaps;      /**< The number of laps of the race */

      std::vector<SPlayer   *> m_vPlayers;    /**< The players of the race */
      std::vector<SRaceData *> m_vRanking;    /**< The ranking of the race */

      std::vector<SStandings> m_vStandings;   /**< The tournament standings after this race */

      STournament *m_pTournament;   /**< The tournament this race is assigned to */

      SRace(const std::string &a_sTrack, const std::string &a_sTrackName, const std::string &a_sInfo, int a_iLaps, STournament *a_pTournament);

      /**
      * De-serialization constructor
      * @param a_sData the data to de-serialize
      * @param a_pTournament the tournament
      */
      SRace(const std::string &a_sData, STournament *a_pTournament);

      /**
      * The destructor
      */
      ~SRace();

      /**
      * Update the ranking of the race
      * @see SRace::m_vRanking
      * @see SRace::onCheckpoint
      * @see SRace::onLapStart
      */
      void updateRanking();

      /**
      * Callback for the checkpoint message
      * @param a_iMarble ID of the marble
      * @param a_iCheckpoint ID of the checkpoint
      * @param a_iStep the step when the checkpoint was passed
      */
      void onCheckpoint(int a_iMarble, int a_iCheckpoint, int a_iStep);

      /**
      * Callback for a lap start message
      * @param a_iMarble the marble
      * @param a_iStep the current step
      */
      void onLapStart(int a_iMarble, int a_iStep);

      /**
      * Callback for a state (normal, stunned, respawn, finished)
      * @param a_iMarble ID of the marble
      * @param a_iNewState the new state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished)
      * @param a_iStep the step of the lap change
      */
      void onStateChange(int a_iMarble, int a_iNewState, int a_iStep);

      /**
      * Finish the race, i.e. finish the current lap for all players
      */
      void finishRace();

      /**
      * Serialize the race to a JSON string
      * @return a JSON string
      */
      std::string toJSON();

      /**
      * Serialize this race
      */
      std::string serialize();

    } SRace;

    /**
    * @class SStandings
    * @author Christian Keimel
    * The data relevant for the tournament standings
    */
    typedef struct SStandings {
      int m_iPlayer;    /**< The player ID */
      int m_iRespawn;   /**< The number of respawns */
      int m_iStunned;   /**< The counter for the player's stuns */
      int m_iNoFinish;  /**< The counter for the races the player didn't finish */
      int m_iScore;     /**< The score of the player */
      int m_iBestPos;   /**< The best position of the player */
      int m_iBestRace;  /**< The race the player has scored his best position */

      int m_aResult[16];    /**< List of the player's race results */
      
      /**
      * The default constructor
      */
      SStandings();

      /**
      * Constructor with the player ID
      * @param a_iPlayer the player ID
      */
      SStandings(int a_iPlayer);

      /**
      * The copy constructor
      * @param a_cOther the data to be copied
      */
      SStandings(const SStandings &a_cOther);

      /**
      * Add a race result to the standings
      * @param a_pRace the race to add
      * @param a_iRace the race number
      */
      void addRaceResult(SRace *a_pRace, int a_iRace);

      /**
      * Compare the standing data to get the standings at a race 
      * @param a_cOther the standings to compare 
      */
      bool isBetterThan(const SStandings &a_cOther) const;

      /**
      * Encode as JSON string
      * @return JSON String
      */
      std::string toJSON();
    }
    SStandings;

    /**
    * @class STournament
    * @author Christian Keimel
    * This data struct holds all data for a multi-race game, i.e. a tournament
    */
    typedef struct STournament {
      data::SGameSettings::enAutoFinish m_eAutoFinish;    /**< The race finish option */
      data::SGameSettings::enRaceClass  m_eRaceClass;     /**< The race class option */

      std::vector<SPlayer    *> m_vPlayers;     /**< The players of the tournament */
      std::vector<SRace      *> m_vRaces;       /**< The races of the tournament */
      std::vector<SStandings  > m_vStandings;   /**< The standings data */

      int  m_iThisRace;     /**< The index of the race (-1 == last race in the vector) */
      bool m_bReverse;      /**< Reverse grid order? */

      /**
      * The standard contructor
      */
      STournament();

      /**
      * The destructor
      */
      ~STournament();

      /**
      * Calculated the standings
      */
      void calculateStandings();

      /**
      * Start the race
      */
      void startRace();

      /**
      * Get the current race
      * @return the current race
      */
      SRace *getRace();

      /**
      * Finish the current race
      */
      void finishCurrentRace();

      /**
      * Save the tournament standings to a JSON file
      */
      void saveToJSON();

      /**
      * Convert the data to a JSON string
      * @return a JSON String
      */
      std::string toJSON();
    }
    STournament;
  }
}