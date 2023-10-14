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
    * @class CDataHandler_SelectPlayers
    * @author Christian Keimel
    * This class handles the data for the player selection
    * when starting a new race
    */
    class CDataHandler_SelectPlayers : public IMenuDataHandler {
      private:
        data::SRacePlayers  *m_pPlayers;      /**< The list of players for the race */
        data::SChampionship *m_pChampionship; /**< The championship */

        std::string m_sSelected;    /**< List of the selected players */

        std::vector<data::SPlayerData> m_vProfiles;   /**< The list of profiles */

      public:
        CDataHandler_SelectPlayers(data::SRacePlayers *a_pPlayers, data::SChampionship *a_pChampionship, std::vector<data::SPlayerData> &a_vProfiles, const std::string &a_sSelected);
        virtual ~CDataHandler_SelectPlayers();

        /**
        * This is the main method of this class. Irrlicht events
        * get forwarded and may be handled by the handler
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleIrrlichtEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get a vector with the names of the selected players
        * @return a vector with the names of the selected players
        */
        std::vector<std::string> getSelectedPlayers();

        /**
        * Get the number of selected players
        * @return the number of selected players
        */
        int getNumberOfSelectedPlayers();
    };
  }
}