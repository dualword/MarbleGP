// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CGameHUD.h>
#include <map>

namespace dustbin {
  namespace threads {
    class COutputQueue;
  }

  namespace gui {
    /**
    * @class CTutorialHUD
    * @author Christian Keimel
    * This is a special HUD for the tutorial level
    */
    class CTutorialHUD : public CGameHUD {
      private:
        int m_iCurrent;   /**< The Current tutorial Step */
        
        std::map<int, std::string             > m_mHints  ;   /**< The tutorial texts */
        std::map<int, irr::scene::ISceneNode *> m_mHintObj;   /**< The objects in the scene indicating a hint */

        irr::gui::IGUITab        *m_pRoot;    /**< The root GUI element for the tutorial hints */
        irr::gui::IGUIStaticText *m_pHint;    /**< The text GUI element for the tutorial hints */
        irr::gui::IGUIStaticText *m_pNext;    /**< The "unpause" text */

        threads::COutputQueue *m_pQueue;

        void pauseGame();

        /**
        * Find a scene node by it's name
        * @param a_sName name of the node
        * @param a_pNode current node to check
        * @return the found scene node, nullptr if no node was found
        */
        irr::scene::ISceneNode *findSceneNode(const std::string &a_sName, irr::scene::ISceneNode *a_pNode);

      protected:
        /**
        * This function receives messages of type "Countdown"
        * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
        */
        virtual void onCountdown(irr::u8 a_Tick) override;

        /**
        * This function receives messages of type "PlayerFinished"
        * @param a_MarbleId ID of the finished marble
        * @param a_RaceTime Racetime of the finished player in simulation steps
        * @param a_Laps The number of laps the player has done
        */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::s32 a_RaceTime, irr::s32 a_Laps) override;

      public:
        CTutorialHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti &a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment *a_pGui, std::vector<gameclasses::SRaceData *> *a_vRanking, threads::COutputQueue *a_pQueue);
        virtual ~CTutorialHUD();


        /**
        * Callback if a trigger was triggered. This method is only
        * implemented in the tutorial HUD
        * @param a_iObjectId the triggering object
        * @param a_iTriggerId the ID of the trigger
        * @see CTutorialHUD
        */
        virtual void triggerCallback(int a_iObjectId, int a_iTriggerId) override;

        /**
        * This function receives messages of type "PauseChanged"
        * @param a_Paused The current paused state
        */
        virtual void onPausechanged(bool a_Paused) override;

        /**
        * This method needs to be implemented for the Android tutorial HUD to make sure the player
        * can continue playing
        * @return true if the event was handled, false otherwise
        */
        virtual bool onWithdrawButton() override;

        /**
        * Should this HUD be in front of the rest? Necessary for the Tutorial HUD
        * @return true if this HUD should be in front of the rest
        */
        virtual bool moveToFront() override;
    };
  }
}
