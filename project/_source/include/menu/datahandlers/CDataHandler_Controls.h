#pragma once

#include <menu/datahandlers/IMenuDataHandler.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace data {
    struct SChampionship;
  }

  namespace menu {
    /**
    * @class CDataHandler_Controls
    * @author Christian Keimel
    * This data handler verifies that all controllers are assigned
    */
    class CDataHandler_Controls : public IMenuDataHandler {
      private:
        std::vector<data::SPlayerData> *m_vProfiles;        /**< A vector with all player data */
        data::SChampionship            *m_pChampionship;    /**< The championship */

        std::vector<std::string> m_vUnassigned;   /**< List of the players with unassigned gamepads */

        std::vector<irr::u8> m_vAssigned;   /**< List of the already assiged gamepads */

        std::map<irr::u8, irr::u32> m_mBtnStates;     /**< The button states */

        irr::u8  m_iJoystick;     /**< The selected joystick */
        irr::u32 m_iBtnState;     /**< The current button state of the joystick */

        /**
        * Update the "please choose controller" label
        * @param a_bOther if true the text is set to "gampad already in use, please choose other"
        */
        void updateLabel(bool a_bOther);

      public:
        CDataHandler_Controls(std::vector<data::SPlayerData> *a_vProfiles, data::SChampionship *a_pChampionship);
        virtual ~CDataHandler_Controls();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Check whether or not all controllers (gaempads) of the selected players are assigned
        * @return true if all controllers are assigned, false otherwise
        */
        bool allControllersAssigned();
      };
  }
}
