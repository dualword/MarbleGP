// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gameclasses/CGameLogicDefault.h>
#include <string>

namespace dustbin {
  namespace gameclasses {
    /**
    * Factory function for the game logic
    * @param a_sType type of game logic
    * @return an instance of the IGameLogic interface
    */
    IGameLogic* createGameLogic(const std::string& a_sType) {
      return new CGameLogicDefault();
    }
  }
}