// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <vector>
#include <string>

namespace dustbin {
  namespace helpers {
    void saveProfiles(std::vector<data::SPlayerData> &a_cProfiles);

    /**
    * Load the AI players from ai_profiles.xml
    * @param a_vAiPlayers the vector that will be filled (0 == Name, 1 == Abbreviation, 2 == Texture, 3 == Preferred Class 1, 4 == Preferred Class 2, 5 == Deviation)
    */
    bool loadAiProfiles(std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> &a_vAiPlayers);
  }
}
