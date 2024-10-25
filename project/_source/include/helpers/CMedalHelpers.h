// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  namespace data {
    typedef struct SPlayerData SPlayerData;   /**< Forward declaration of the player data struct*/
  }

  namespace gameclasses {
    typedef struct STournament STournament;   /**< Forward declaration of the STournament struct */
    typedef struct SPlayer     SPlayer;       /**< Forward declaration of the SPlayer struct */
  }

  namespace helpers {
    /**
    * Check whether or not a player has achieved a new achievement
    * @param a_pPlayer the player to check
    * @param a_pTournament the current tournament
    * @return a vector with the achievement identifiers
    */
    std::vector<std::string> checkForNewAchievements(data::SPlayerData* a_pPlayer, gameclasses::STournament* a_pTournament);

    /**
    * Get all available achievements
    * @return vector with the data (0 == id, 1 == name, 2 == description, 3 == image)
    */
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> getAchievements();

    /**
    * Get an achievement by it's ID
    * @param a_sId the ID of the achievement
    * @return vector with the data (0 == id, 1 == name, 2 == description, 3 == image)
    */
    std::tuple<std::string, std::string, std::string, std::string> getAchievementById(const std::string &a_sId);
  }
}
