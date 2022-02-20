// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/STriggerAction.h>
#include <vector>

namespace dustbin {
  namespace gameclasses {
    class CWorld; /**< Forward declaration of the world */

    /**
    * @class CMarbleCounter
    * @author Christian Keimel
    * This class counts marbles that are in a specific section marked
    * by two triggers and executes the necessary actions
    */
    class CMarbleCounter {
      private:
        int m_iTriggerPlus;   /**< The trigger ID that increases the marble count */
        int m_iTriggerMinus;  /**< The trigger ID that decreases the marble count */
        int m_iMarbleCount;   /**< The marble counter */

        std::vector<scenenodes::STriggerAction> m_vActions;   /**< The actions that are possibly executed */

        CWorld *m_pWorld;

        bool m_aMarblesIn[16];

        void checkAction(int a_iMarbleCountNew);
        void executeAction(const scenenodes::STriggerAction &a_cAction);

      public:
        CMarbleCounter(int a_iTriggerPlus, int a_iTriggerMinus, const std::vector<scenenodes::STriggerAction> &a_vActions);
        CMarbleCounter(const CMarbleCounter &a_cOther, CWorld *a_pWorld);
        CMarbleCounter(const CMarbleCounter &a_cOther);

        virtual ~CMarbleCounter();

        void handleTrigger(int a_iTriggerId, int a_iMarbleId);
        void setWorld(CWorld *a_pWorld);
        void marbleRespawn(int a_iMarbleId);
    };
  }
}

