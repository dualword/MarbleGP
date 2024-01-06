// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
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
    /**
    * @class SRaceData
    * @author Christian Keimel
    * This struct stores single race data of a player
    * @see SPlayer::m_cRaceData
    */
    typedef struct SRaceData {
      int m_iPlayer;      /**< The player ID */
      int m_iMarble;      /**< The marble ID */
      int m_iPosition;    /**< The position in the race */
      int m_iDiffLeader;  /**< The deficit to the leader */
      int m_iDiffAhead;   /**< The deficit to the marble ahead */

      std::vector<std::vector<int>> m_vLapCheckpoints;    /**< Time of the passed checkpoints per lap */

      std::vector<int> m_vRespawn;    /**< Timestamps of the respawns of this player in the race */
      std::vector<int> m_vStunned;    /**< Timestamps of the stuns of this player in the race */

      /**
      * The default contructor
      */
      SRaceData();

      /**
      * The de-serialize constructor
      * @param a_sData serialized data
      */
      SRaceData(const std::string &a_sData);

      /**
      * The copy constructor
      * @param a_cRace the race to copy
      */
      SRaceData(const SRaceData &a_cRace);

      /**
      * Serialize the data struct
      * @return the serialized data
      */
      std::string serialize();

      /**
      * Store the data in a JSON string
      * @return a JSON string
      */
      std::string toJSON();
    }
    SRaceData;

    /**
    * @class SPlayer
    * @author Christian Keimel
    * This is the struct for the players
    */
    typedef struct SPlayer {
      int          m_iPlayer;           /**< The player id */
      std::string  m_sName;             /**< The name of the player */
      std::wstring m_sWName;            /**< The player's name as wide string */
      std::string  m_sTexture;          /**< The texture of the player's marble */
      std::string  m_sController;       /**< The serialized controller configuration */
      std::string  m_sShortName;        /**< The short name of the player */
      std::wstring m_sNumber;           /**< The player's starting number */
      std::wstring m_wsShortName;       /**< The player's short name as wide string */
      bool         m_bWithdrawn;        /**< Has the player withdrawn from the race? */
      bool         m_bShowRanking;      /**< Flag for some controllers to hide ranking and laptimes */
      int          m_iState;            /**< The player's state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished) */
      int          m_iLapNo;            /**< The player's current lap */

      irr::video::SColor m_cText;   /**< The text color (for the starting number in the ranking display) */
      irr::video::SColor m_cBack;   /**< The background color (for the starting number in the ranking display) */
      irr::video::SColor m_cFrme;   /**< The frame color (for the starting number in the ranking display) */

      data::enPlayerType m_eType;   /**< The type of player (local, AI, network) */

      data::SPlayerData::enAiHelp m_eAiHelp;    /**< The AI help level of this player */

      gameclasses::SMarbleNodes *m_pMarble;       /**< The marble of the player */
      controller::IController   *m_pController;   /**< The controller of this player */

      SRaceData m_cRaceData;    /**< Data of a race */

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
      * @param a_cPlayer the player to copy
      */
      SPlayer(const SPlayer &a_cPlayer);

      /**
      * The constructor using serialized data
      * @param a_sData serialized data
      */
      SPlayer(const std::string &a_sData);

      /**
      * The main constructor
      * @param a_iPlayer the player id
      * @param a_sName the name of the player
      * @param a_sTexture the texture string of the player's marble
      * @param a_sController the controller configuration string of the player
      * @param a_pMarble the marble of the player
      */
      SPlayer(
        int a_iPlayer, 
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
      */
      void onLapStart();

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
      * Serialize the race data of this player to a string
      * @return the serialized string
      */
      std::string serialize();

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

      int m_iLaps;      /**< The number of laps of the race */

      std::vector<SPlayer *> m_vPlayers;    /**< The players of the race */
      std::vector<SPlayer *> m_vRanking;    /**< The ranking of the race */

      STournament *m_pTournament;   /**< The tournament this race is assigned to */

      SRace(const std::string &a_sTrack, int a_iLaps, STournament *a_pTournament);

      /**
      * Constructor with serialized data
      * @para a_sData serialized data
      */
      SRace(const std::string &a_sData);

      /**
      * Copy constructor
      * @param a_cRace the race to copy
      */
      SRace(const SRace &a_cRace, STournament *a_pTournament);

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
      */
      void onLapStart(int a_iMarble);

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
      * Serialize the race
      * @return serialized data of the race
      */
      std::string serialize();

      /**
      * Serialize the race to a JSON string
      * @return a JSON string
      */
      std::string toJSON();

    } SRace;

    /**
    * @class SStandingData
    * @author Christian Keimel
    * The data relevant for the tournament standings
    */
    typedef struct SStandings {
      int m_iPlayer;    /**< The player ID */
      int m_iRespawn;   /**< The number of respawns */
      int m_iStunned;   /**< The counter for the player's stuns */
      int m_iNoFinish;  /**< The counter for the races the player didn't finish */
      
      std::vector<int> m_vResults;   /**< This vector holds a vector with the finishing position of each race */

      /**
      * The default constructor
      */
      SStandings();

      /**
      * The copy constructor
      * @param a_cOther the data to be copied
      */
      SStandings(const SStandings &a_cOther);

      /**
      * Compare the standing data to get the standings at a race 
      * @param a_cOther the standings to compare 
      * @param a_iRace the race up to which the standings are to be calculated (-1 == all races)
      * @param a_iPlayers the number of players (important for the score table calculation)
      */
      bool isBetterThan(const SStandings &a_cOther, int a_iRace, int a_iPlayers);
    }
    SStandings;

    /**
    * @class STournament
    * @author Christian Keimel
    * This data struct holds all data for a multi-race game, i.e. a tournament
    */
    typedef struct STournament {
      data::SGameSettings::enAutoFinish m_eAutoFinish;    /**< The race finish option */
      data::SGameSettings::enGridPos    m_eGridPos;       /**< The starting grid order option */
      data::SGameSettings::enRaceClass  m_eRaceClass;     /**< The race class option */

      std::vector<SPlayer    *> m_vPlayers;     /**< The players of the tournament */
      std::vector<SRace      *> m_vRaces;       /**< The races of the tournament */
      std::vector<SStandings  > m_vStandings;   /**< The standings data */

      int m_iThisRace;    /**< The index of the race (-1 == last race in the vector) */

      /**
      * The standard contructor
      */
      STournament();

      /**
      * The copy constructor
      * @param a_cOther the data to be copied
      */
      STournament(const STournament &a_cOther);

      /**
      * The de-serialization constructor
      * @param a_sData the data to de-serialize
      */
      STournament(const std::string &a_sData);

      /**
      * The destructor
      */
      ~STournament();

      /**
      * Calculated the standings
      */
      void calculateStandings();

      /**
      * Serialize the tournament
      * @return the serialized data
      */
      std::string serialize();

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