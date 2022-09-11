// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaScript_scene.h>

namespace dustbin {
  namespace lua {
    /**
    * A callback for initializing the members. Just calls the initialize method of the singleton
    */
    void CLuaScript_scene::initializesingleton() {
      m_LuaSgt_scene->initialize();
    }
  }
}