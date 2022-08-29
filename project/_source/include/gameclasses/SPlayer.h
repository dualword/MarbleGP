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
      std::string  m_sTexture;          /**< The texture of the player's marble */
      std::string  m_sController;       /**< The serialized controller configuration */
      std::string  m_sShortName;        /**< The short name of the player */
      std::wstring m_sNumber;           /**< The player's starting number */
      bool         m_bWithdrawn;        /**< Has the player withdrawn from the race? */
      int          m_iState;            /**< The player's state (0 == normal, 1 == stunned, 2 == Respawn 1, 3 == Respawn 2, 4 == Finished) */

      irr::video::SColor m_cText;   /**< The text color (for the starting number in the ranking display) */
      irr::video::SColor m_cBack;   /**< The background color (for the starting number in the ranking display) */
      irr::video::SColor m_cFrme;   /**< The frame color (for the starting number in the ranking display) */

      data::enPlayerType m_eType;

      data::SPlayerData::enAiHelp m_eAiHelp;

      gameclasses::SMarbleNodes *m_pMarble;  /**< The marble of the player */

      controller::IController* m_pController;

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
      SPlayer(int a_iPlayer, const std::string& a_sName, const std::string& a_sTexture, const std::string &a_sController, const std::string &a_sShortName, data::SPlayerData::enAiHelp a_eAiHelp, gameclasses::SMarbleNodes* a_pMarble, data::enPlayerType a_eType);
    } SPlayer;
  }
}