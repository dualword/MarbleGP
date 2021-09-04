// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace gfx {

    /**
    * @class SViewPort
    * @author Christian Keimel
    * This struct holds all information for the in-game viewports
    * to be rendered
    */
    struct SViewPort {
      irr::core::recti              m_cRect;    /**< The on-screen rectangle to render this viewport to */
      int                           m_iPlayer;  /**< The player id this viewport belongs to */
      irr::scene::ISceneNode*       m_pMarble;  /**< The marble of the player */
      irr::scene::ICameraSceneNode* m_pCamera;  /**< The camera of the viewport */

      /**
      * The constructor
      * @param a_cRect the rectangle to render this viewport to
      * @param a_iPlayer the player id this viewport belongs to
      * @param a_pMarble the marble of the player
      * @param a_pCamera the camera for the viewport
      */
      SViewPort(const irr::core::recti a_cRect, int a_iPlayer, irr::scene::ISceneNode *a_pMarble, irr::scene::ICameraSceneNode *a_pCamera) {
        m_cRect   = a_cRect;
        m_iPlayer = a_iPlayer;
        m_pMarble = a_pMarble;
        m_pCamera = a_pCamera;
      }

      /**
      * Default constructor
      */
      SViewPort() {
        m_iPlayer = 0;
        m_pMarble = nullptr;
        m_pCamera = nullptr;
      }
    };
  }
}