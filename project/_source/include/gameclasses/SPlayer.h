// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <gameclasses/SMarbleNodes.h>
#include <controller/IController.h>
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
      int m_iPlayer;               /**< The player id */
      std::string m_sName,         /**< The name of the player */
                  m_sTexture,      /**< The texture of the player's marble */
                  m_sController;   /**< The serialized controller configuration */

      gameclasses::SMarbleNodes *m_pMarble;  /**< The marble of the player */

      controller::IController* m_pController;

      /**
      * The default constructor
      */
      SPlayer() {
        m_iPlayer     = 0;
        m_sName       = "";
        m_pMarble     = nullptr;
        m_pController = nullptr;
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
      SPlayer(int a_iPlayer, const std::string& a_sName, const std::string& a_sTexture, const std::string &a_sController, gameclasses::SMarbleNodes* a_pMarble) {
        m_iPlayer     = a_iPlayer;
        m_sName       = a_sName;
        m_sTexture    = a_sTexture;
        m_pMarble     = a_pMarble;
        m_sController = a_sController;
        m_pController = nullptr;

        if (m_sTexture != "" && m_pMarble != nullptr) {
          m_pMarble->m_pRotational->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(m_sTexture));
        }
      }
    };
  }
}