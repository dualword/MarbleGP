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
        
        std::map<int, std::string> m_mHints; /**< The tutorial texts */

        irr::gui::IGUITab        *m_pRoot;    /**< The root GUI element for the tutorial hints */
        irr::gui::IGUIStaticText *m_pHint;    /**< The text GUI element for the tutorial hints */

        threads::COutputQueue *m_pQueue;

        void pauseGame();

      public:
        CTutorialHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti &a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment *a_pGui, std::vector<gameclasses::SPlayer *> *a_vRanking, threads::COutputQueue *a_pQueue);
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
    };
  }
}
