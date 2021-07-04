// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gui/CButtonRenderer.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CButtonRenderer::CButtonRenderer() : m_pGlobal(CGlobal::getInstance()), m_pDrv(m_pGlobal->getInstance()->getVideoDriver()) {
    }

    CButtonRenderer::~CButtonRenderer() {
    }

    /**
    * Render the button-style background
    * @param a_cRect the rectangle to render to
    * @param a_cColor the background color
    */
    void CButtonRenderer::renderBackground(const irr::core::recti& a_cRect, const irr::video::SColor& a_cColor) {
      int l_iRaster = m_pGlobal->getRasterSize(),
          l_iBorder = l_iRaster / 3,
          l_iHeight = a_cRect.getHeight(),
          l_iWidth = a_cRect.getWidth();

      if (l_iBorder < 2)
        l_iBorder = 2;


      int l_iRadius = l_iRaster - l_iBorder;

      // Draw the upper border
      for (int l_iLine = 0; l_iLine < l_iRaster; l_iLine++) {
        int l_iOffset = l_iRaster - (int)(sqrt(l_iRaster * l_iRaster - (l_iRaster - l_iLine) * (l_iRaster - l_iLine)));
        m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::video::SColor(0xFF, 0, 0, 0));
        l_iOffset = l_iRaster - (int)(sqrt(l_iRadius * l_iRadius - (l_iRaster - l_iLine) * (l_iRaster - l_iLine)));
        m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, a_cColor);
      }

      // Draw the central part
      m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), irr::core::recti(a_cRect.UpperLeftCorner + irr::core::position2di(        0, l_iRaster), a_cRect.LowerRightCorner - irr::core::position2di(        0, l_iRaster)));
      m_pDrv->draw2DRectangle(a_cColor                         , irr::core::recti(a_cRect.UpperLeftCorner + irr::core::position2di(l_iBorder, l_iRaster), a_cRect.LowerRightCorner - irr::core::position2di(l_iBorder, l_iRaster)));

      // Draw the lower border
      for (int l_iLine = l_iHeight - l_iRaster; l_iLine < l_iHeight; l_iLine++) {
        int l_iOffset = l_iRaster - (int)(sqrt(l_iRaster * l_iRaster - (l_iLine - l_iHeight + l_iRaster) * (l_iLine - l_iHeight + l_iRaster)));
        m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::video::SColor(0xFF, 0, 0, 0));
        l_iOffset = l_iRaster - (int)(sqrt(l_iRadius * l_iRadius - (l_iLine - l_iHeight + l_iRaster) * (l_iLine - l_iHeight + l_iRaster)));
        m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, a_cColor);
      }
    }
  }
}