// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaScript_physics.h>

namespace dustbin {
  namespace lua {
    /**
    * Tell the script about the world it's managing
    * @param The physics world
    */
    void CLuaScript_physics::setWorld(gameclasses::CWorld* a_world, gameclasses::CObjectMarble * a_marbles[16]) {
      m_LuaSgt_physics->setworld(a_world, a_marbles);
    }
  }
}