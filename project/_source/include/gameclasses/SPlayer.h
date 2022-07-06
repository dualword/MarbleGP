// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <gameclasses/SMarbleNodes.h>
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
    struct SPlayer {
      int          m_iPlayer;           /**< The player id */
      int          m_iId;               /**< The Marble id */
      int          m_iPosition;         /**< Position in the race */
      int          m_iLastPosUpdate;    /**< Step of the last positional update */
      int          m_iDiffLeader;       /**< Deficit to the leader */
      int          m_iDiffAhead;        /**< Deficit to the marble ahead */
      std::string  m_sName;             /**< The name of the player */
      std::string  m_sTexture;          /**< The texture of the player's marble */
      std::string  m_sController;       /**< The serialized controller configuration */
      bool         m_bWithdrawn;        /**< Has the player withdrawn from the race? */

      data::enPlayerType m_eType;

      data::SPlayerData::enAiHelp m_eAiHelp;

      gameclasses::SMarbleNodes *m_pMarble;  /**< The marble of the player */

      controller::IController* m_pController;

      /**
      * The default constructor
      */
      SPlayer() :
        m_iPlayer        (0),
        m_iId            (-1),
        m_iPosition      (0),
        m_iLastPosUpdate (0),
        m_iDiffLeader    (0),
        m_iDiffAhead     (0),
        m_sName          (""),
        m_sTexture       (""),
        m_sController    (""),
        m_bWithdrawn     (false),
        m_eType          (data::enPlayerType::Local),
        m_eAiHelp        (data::SPlayerData::enAiHelp::Off),
        m_pMarble        (nullptr),
        m_pController    (nullptr)
      {
      }

      /**
      * The destructor
      */
      ~SPlayer() {
        if (m_pMarble != nullptr)
          delete m_pMarble;

        if (m_pController != nullptr)
          delete m_pController;
      }

      /**
      * The main constructor
      * @param a_iPlayer the player id
      * @param a_sName the name of the player
      * @param a_sTexture the texture string of the player's marble
      * @param a_sController the controller configuration string of the player
      * @param a_pMarble the marble of the player
      */
      SPlayer(int a_iPlayer, const std::string& a_sName, const std::string& a_sTexture, const std::string &a_sController, data::SPlayerData::enAiHelp a_eAiHelp, gameclasses::SMarbleNodes* a_pMarble, data::enPlayerType a_eType) :
        m_iPlayer       (a_iPlayer),
        m_iPosition     (0),
        m_iLastPosUpdate(0),
        m_iDiffLeader   (0),
        m_iDiffAhead    (0),
        m_sName         (a_sName),
        m_sTexture      (a_sTexture),
        m_sController   (a_sController),
        m_bWithdrawn    (false),
        m_eType         (a_eType),
        m_eAiHelp       (a_eAiHelp),
        m_pMarble       (a_pMarble),
        m_pController   (nullptr)
      {

        if (m_pMarble != nullptr && m_pMarble->m_pPositional != nullptr)
          m_iId = m_pMarble->m_pPositional->getID();
        else
          m_iId = -1;

        if (m_sTexture != "" && m_pMarble != nullptr) {
          m_pMarble->m_pRotational->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(m_sTexture));
        }
      }
    };
  }
}