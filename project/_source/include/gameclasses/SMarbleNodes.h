// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace gfx {
    struct SViewPort;
  }

  namespace gameclasses {
    /**
    * This data structure is a collection
    * of all scene nodes that define a marble
    */
    struct SMarbleNodes {
      /**
       * The empty scene node that acts as root node. A rotation might be applied
       * to this node to correct the starting number direction
       */
      irr::scene::ISceneNode* m_pPositional;

      /**
      * This is the actual marble. This node is used to update the marble texture,
      * and rotations are applied to this node
      */
      irr::scene::IMeshSceneNode* m_pRotational;

      /**
      * The viewport that might be assigned to the marble
      */
      gfx::SViewPort *m_pViewport;

      SMarbleNodes() {
        m_pPositional = nullptr;
        m_pRotational = nullptr;
        m_pViewport   = nullptr;
      }
    };
  }
}