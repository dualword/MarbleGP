// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  class CGlobal;

  namespace gui {
    /**
    * @class CButtonRenderer
    * @author Christian Keimel
    * This class renders the button-syle background, used for buttons
    * and the menu background
    */
    class CButtonRenderer {
      protected:
        CGlobal                  *m_pGlobal;
        irr::video::IVideoDriver *m_pDrv;
        irr::video::SColor        m_cBorder;    /**< The border color */

        int m_iBorder;  /**< The border size */
        int m_iRaster;  /**< The raster size */
        
      public:
        CButtonRenderer();
        virtual ~CButtonRenderer();

        /**
        * Render the button-style background
        * @param a_cRect the rectangle to render to
        * @param a_cColor the background color
        * @param a_cBorder the border color
        */
        void renderBackground(const irr::core::recti& a_cRect, const irr::video::SColor &a_cColor, irr::video::SColor *a_pBorder = nullptr);
    };
  }
}
