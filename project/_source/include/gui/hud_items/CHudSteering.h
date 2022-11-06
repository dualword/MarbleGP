// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CHudSteering
    * @author Christian Keimel
    * This class displays the current controls of the marble (if wanted)
    */
    class CHudSteering {
      private:
        irr::video::IVideoDriver *m_pDrv;     /**< The Irrlicht video driver */
        irr::core::dimension2du   m_cSize;    /**< Size of the element */
        int                       m_iHeight;  /**< The height of the lines */

        /**
        * Draw steering or throttle
        * @param a_cPos the (central) position to draw to
        * @param a_fFactor the steering value (-1 .. 1)
        * @param a_cColor the color to use
        * @param a_cClip the clipping rectangle
        */
        void drawControlItem(const irr::core::vector2di &a_cPos, irr::f32 a_fFactor, const irr::video::SColor &a_cColor, const irr::core::recti &a_cClip);

      public:
        CHudSteering(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        ~CHudSteering();

        /**
        * Render the HUD item
        * @param a_cPos the (center) position to render to
        * @param a_fSteer the current steering input (-1 .. 1)
        * @param a_fThrottle the current throttle input (-1 .. 1)
        * @param a_bBrake is the brake active?
        * @param a_bRespawn is the manual respawn button pressed?
        * @param a_cClip the clipping rectangle
        */
        int render(const irr::core::vector2di &a_cPos, irr::f32 a_fSteer, irr::f32 a_fThrottle, bool a_bBrake, bool a_bRespawn, const irr::core::recti &a_cClip);
    };
  }
}