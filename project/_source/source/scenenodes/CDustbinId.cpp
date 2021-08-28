#pragma once

#include <scenenodes/CDustbinId.h>

namespace dustbin {
  namespace scenenodes {
    
    irr::s32 getNextSceneNodeId() {
      static irr::s32 g_iNextId = 23000;
      return g_iNextId++;
    }
  }
}
