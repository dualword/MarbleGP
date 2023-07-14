/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)

#include <_generated/luamesh/CLuaScript_meshbuilder.h>

namespace dustbin {
  namespace luamesh {
    /**
    * Get the mesh created by the script
    */
    irr::scene::SMesh* CLuaScript_meshbuilder::getmesh() {
      return m_LuaSgt_luamesh->getmesh();
    }
  }
}