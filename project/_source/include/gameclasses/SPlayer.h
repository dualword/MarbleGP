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
    * @class SPlayer
    * @author Christian Keimel
    * This is the struct for the players
    */
    typedef struct SPlayer {
      int          m_iPlayer;           /**< The player id */
      int          m_iId;               /**< The Marble id */
      int          m_iPosition;         /**< Position in the race */
      int          m_iLastPosUpdate;    /**< Step of the last positional update */
      int          m_iDiffLeader;       /**< Deficit to the leader */
      int          m_iDiffAhead;        /**< Deficit to the marble ahead */
      std::string  m_sName;             /**< The name of the player */
      std::wstring m_sWName;            /**< The player's name as wide string */
      std::string  m_sTexture;          /**< The texture of the player's marble */
      std::string  m_sController;       /**< The serialized controller configuration */
      std::string  m_sShortName;        /**< The short name of the player */
      std::wstring m_sNumber;           /**< The player's starting number */
      std::wstring m_wsShortName;       /**< The player's short name as wide string */
      std::wstring m_wsName;            /**< The player's name as wide string without the AI identifier */
      bool         m_bWithdrawn;        /**< Has the player withdrawn from the race? */
      bool         m_bShowRanking;      /**< Flag for some controllers to hide ranking and laptimes */
      int          m_iState;            /**< The player's state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished) */
      int          m_iLapNo;            /**< The player's current lap */
      int          m_iLapCp;            /**< The checkpoint number of the player's current lap */
      int          m_iLastCp;           /**< Time when the player passed the last checkpoint */

      irr::video::SColor m_cText;   /**< The text color (for the starting number in the ranking display) */
      irr::video::SColor m_cBack;   /**< The background color (for the starting number in the ranking display) */
      irr::video::SColor m_cFrme;   /**< The frame color (for the starting number in the ranking display) */

      data::enPlayerType m_eType;   /**< The type of player (local, AI, network) */

      data::SPlayerData::enAiHelp m_eAiHelp;    /**< The AI help level of this player */

      gameclasses::SMarbleNodes *m_pMarble;       /**< The marble of the player */
      controller::IController   *m_pController;   /**< The controller of this player */

      std::vector<std::vector<int>> m_vLapCheckpoints;    /**< Time of the passed checkpoints per lap */

      /**
      * The default constructor
      */
      SPlayer();

      /**
      * The destructor
      */
      ~SPlayer();

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

      SRace(const std::string &a_sTrack, int a_iLaps);

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

    } SRace;
  }
}