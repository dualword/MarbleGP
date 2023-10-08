#pragma once

#include <menu/datahandlers/IMenuDataHandler.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace data {
    struct SRacePlayers;
    struct SChampionship;
  }

  namespace menu {
    /**
    * @class CDataHandler_Customize
    * @author Christian Keimel
    * The data handler for the "Customize Game" dialog
    */
    class CDataHandler_Customize : public IMenuDataHandler {
      private:
        data::SGameSettings m_cSettings;    /**< The game settings */

      public:
        CDataHandler_Customize(data::SGameSettings a_cSettings);
        virtual ~CDataHandler_Customize();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) override;

        const data::SGameSettings &getSettings();
    };
  }
}