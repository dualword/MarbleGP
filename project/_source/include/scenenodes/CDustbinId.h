// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    irr::s32 getNextSceneNodeId();
    void sceneNodeIdUsed(irr::s32 a_iId);
  }
}