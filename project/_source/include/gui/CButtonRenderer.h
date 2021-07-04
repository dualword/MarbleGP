// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

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
      private:
        CGlobal                 * m_pGlobal;
        irr::video::IVideoDriver* m_pDrv;
      public:
        CButtonRenderer();
        virtual ~CButtonRenderer();

        /**
        * Render the button-style background
        * @param a_cRect the rectangle to render to
        * @param a_cColor the background color
        */
        void renderBackground(const irr::core::recti& a_cRect, const irr::video::SColor &a_cColor);
    };
  }
}
