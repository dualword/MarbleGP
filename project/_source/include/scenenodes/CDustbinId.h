// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace scenenodes {
    irr::s32 getNextSceneNodeId();
    void sceneNodeIdUsed(irr::s32 a_iId);
  }
}