// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <vector>

namespace dustbin {
  namespace gfx {
    struct SViewPort;
  }

  namespace gameclasses {
    struct SPlayer;

    /**
    * This data structure is a collection
    * of all scene nodes that define a marble
    */
    struct SMarbleNodes {
      enum class enMarbleState {
        Rolling,
        Stunned,
        Respawn1,
        Respawn2,
        Finished
      };

      enMarbleState m_eState; /**< The state of the marble */

      int m_iStateChange;   /**< The step of the last state change */
      int m_iRespawnStart;  /**< Start of the respawn state */

      SPlayer *m_pPlayer; /**< The player of the marble */

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
      * Does the camera follow the marble (only false if respawning or finished)?
      */
      bool m_bCamLink;

      /**
      * The viewport that might be assigned to the marble
      */
      gfx::SViewPort *m_pViewport;

      /**
      * The current velocity of the marble
      */
      irr::f32 m_fVeclocity;

      SMarbleNodes() :
        m_eState       (enMarbleState::Rolling),
        m_iStateChange (0),
        m_iRespawnStart(0),
        m_pPlayer      (nullptr),
        m_pPositional  (nullptr),
        m_pRotational  (nullptr),
        m_bCamLink     (true),
        m_pViewport    (nullptr),
        m_fVeclocity   (0.0f)
      {
      }
    };
  }
}