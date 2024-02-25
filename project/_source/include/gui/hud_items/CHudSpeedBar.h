// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CHudSpeedBar
    * @author Christian Keimel
    * This class renders the speed bar shown in the HUD
    */
    class CHudSpeedBar {
      private:
        irr::video::IVideoDriver *m_pDrv;     /**< The Irrlicht video driver */
        irr::core::dimension2du   m_cSize;    /**< The total size */
        irr::core::dimension2du   m_cText;    /**< Size of the text field showing the current velocity */
        irr::core::dimension2du   m_cBar;     /**< The size of the speed bar */
        irr::core::position2di    m_cOffset;  /**< Offset of the speed bar */
        irr::gui::IGUIFont       *m_pFont;    /**< The font to use */

      public:
        CHudSpeedBar(irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIFont *a_pFont, const irr::core::recti &a_cViewport);
        ~CHudSpeedBar();

        /**
        * Render the speed bar
        * @param a_fVelocity the velocity of the marble
        * @param a_cPos the position to render to
        * @param a_cClip the clipping rectangle
        * @return the height of the rendered speed bar
        */
        irr::s32 render(irr::f32 a_fVelocity, const irr::core::position2di &a_cPos, const irr::core::recti &a_cClip);
    };
  }
}
