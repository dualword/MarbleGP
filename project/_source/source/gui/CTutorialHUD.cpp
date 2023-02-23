// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CTutorialHUD.h>

namespace dustbin {
  namespace gui {
    CTutorialHUD::CTutorialHUD(gameclasses::SPlayer* a_pPlayer, const irr::core::recti& a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment* a_pGui, std::vector<gameclasses::SPlayer*>* a_vRanking) :
      CGameHUD(a_pPlayer, a_cRect, a_iLapCnt, a_pGui, a_vRanking), m_iCurrent(0)
    {
    }

    CTutorialHUD::~CTutorialHUD() {

    }

    /**
    * Callback if a trigger was triggered. This method is only
    * implemented in the tutorial HUD
    * @param a_iObjectId the triggering object
    * @param a_iTriggerId the ID of the trigger
    * @see CGameHUD::triggerCallback
    */
    void CTutorialHUD::triggerCallback(int a_iObjectId, int a_iTriggerId) {
      if (a_iObjectId == m_iMarble) {
        if (a_iTriggerId == m_iCurrent + 1 || (a_iTriggerId == 1 && m_iCurrent == 7)) {
          m_iCurrent = a_iTriggerId;

          printf("Got Trigger %i\n", a_iTriggerId);
        }
      }
    }

    /**
    * This function receives messages of type "PauseChanged"
    * @param a_Paused The current paused state
    */
    void CTutorialHUD::onPausechanged(bool a_Paused) {

    }
  }
}