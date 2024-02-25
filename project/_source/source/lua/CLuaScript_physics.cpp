// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaScript_physics.h>

namespace dustbin {
  namespace lua {
    /**
    * Tell the script about the world it's managing
    * @param The physics world
    */
    void CLuaScript_physics::setWorld(gameclasses::CWorld * a_world, gameclasses::CDynamicThread * a_dynamics) {
      m_LuaSgt_physics->setworld(a_world, a_dynamics);
    }

    /**
    * Add a marble to the LUA physics script
    * @param The marble to add
    * @param ID of the marble (10000 .. 100016)
    */
    void CLuaScript_physics::addmarble(gameclasses::CObjectMarble* a_marble, int a_id) {
      m_LuaSgt_physics->addmarble(a_marble, a_id);
    }
  }
}