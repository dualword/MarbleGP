// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CGameHUD.h>
#include <map>

namespace dustbin {
  namespace gui {
    /**
    * @class CTutorialHUD
    * @author Christian Keimel
    * This is a special HUD for the tutorial level
    */
    class CTutorialHUD : public CGameHUD {
      private:
        int m_iCurrent;   /**< The Current tutorial Step */
        
        std::map<int, irr::gui::IGUIElement *> m_pTips; /**< The root elements for the tutorial tip text */

      public:
        CTutorialHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti &a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment *a_pGui, std::vector<gameclasses::SPlayer *> *a_vRanking);
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
