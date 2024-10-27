// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <gamecloud/CGameCloud.h>

namespace dustbin {
  namespace gamecloud {
    /**
    * Initialize whatever library / cloud service is used
    * @return true if initialization was successful, false otherwise
    */
    bool initialize() {
      return true;
    }

    /**
    * Shut down whatever library / cloud service is used
    */
    void shutdown() {
    }

    /**
    * Poll the cloud service
    */
    void pollService() {
    }

    /**
    * Add an achievement to the cloud
    * @param a_sId ID of the achievement
    */
    void awardAchievement(const std::string& a_sId) {
    }
  }
}