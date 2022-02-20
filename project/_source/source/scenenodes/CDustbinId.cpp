#include <scenenodes/CDustbinId.h>

namespace dustbin {
  namespace scenenodes {
    irr::s32 g_iNextId = 23000;

    irr::s32 getNextSceneNodeId() {
      return g_iNextId++;
    }

    void sceneNodeIdUsed(irr::s32 a_iId) {
      if (a_iId >= g_iNextId)
        g_iNextId = a_iId + 1;
    }
  }
}
