// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CButtonRenderer.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CButtonRenderer::CButtonRenderer() : m_pGlobal(CGlobal::getInstance()), m_pDrv(m_pGlobal->getInstance()->getVideoDriver()), m_iBorder(m_pGlobal->getRasterSize() / 3), m_iRaster(m_pGlobal->getRasterSize()) {
      if (m_iBorder < 2)
        m_iBorder = 2;

      m_cBorder = irr::video::SColor(0xFF, 0, 0, 0);
    }

    CButtonRenderer::~CButtonRenderer() {
    }

    /**
    * Render the button-style background
    * @param a_cRect the rectangle to render to
    * @param a_cColor the background color
    */
    void CButtonRenderer::renderBackground(const irr::core::recti& a_cRect, const irr::video::SColor& a_cColor) {
      int l_iHeight = a_cRect.getHeight();
      int l_iWidth  = a_cRect.getWidth();
      int l_iBorder = m_iBorder;
      int l_iRaster = m_iRaster;
      int l_iRadius = l_iRaster - l_iBorder;

      if (l_iHeight <= 2 * m_iRaster) {
        l_iRaster = l_iHeight / 3;
        l_iBorder = l_iRaster / 3;

        if (l_iBorder < 2)
          l_iBorder = 2;

        l_iRadius = l_iRaster - l_iBorder;
      }

      // Draw the upper border
      for (int l_iLine = 0; l_iLine < l_iRaster + 1; l_iLine++) {
        int l_iOffset = l_iRaster - (int)(sqrt(l_iRaster * l_iRaster - (l_iRaster - l_iLine) * (l_iRaster - l_iLine))),
            l_iInner  = l_iRaster - (int)(sqrt(l_iRadius * l_iRadius - (l_iRaster - l_iLine) * (l_iRaster - l_iLine)));

        if (l_iLine < l_iBorder) {
          m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, m_cBorder);
        }
        else {
          irr::core::vector2di l_cLeft       = irr::core::vector2di(           l_iOffset, l_iLine) + a_cRect.UpperLeftCorner,
                               l_cInnerLeft  = irr::core::vector2di(           l_iInner , l_iLine) + a_cRect.UpperLeftCorner,
                               l_cRight      = irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner,
                               l_cInnerRight = irr::core::vector2di(l_iWidth - l_iInner , l_iLine) + a_cRect.UpperLeftCorner;

          m_pDrv->draw2DLine(l_cLeft      , l_cInnerLeft , m_cBorder);
          m_pDrv->draw2DLine(l_cInnerRight, l_cRight     , m_cBorder);
          m_pDrv->draw2DLine(l_cInnerLeft , l_cInnerRight, a_cColor);
        }
      }

#ifndef _OPENGL_ES
      // Draw the central part
      irr::core::recti l_cRect = irr::core::recti(a_cRect.UpperLeftCorner + irr::core::vector2di(0, l_iRaster), a_cRect.UpperLeftCorner + irr::core::vector2di(l_iBorder, a_cRect.getHeight() - l_iRaster));
      m_pDrv->draw2DRectangle(m_cBorder, l_cRect);
      l_cRect = irr::core::recti(a_cRect.UpperLeftCorner + irr::core::vector2di(a_cRect.getWidth() - l_iBorder, l_iRaster), a_cRect.LowerRightCorner - irr::core::vector2di(0, l_iRaster));
      m_pDrv->draw2DRectangle(m_cBorder, l_cRect);
      m_pDrv->draw2DRectangle(a_cColor, irr::core::recti(a_cRect.UpperLeftCorner + irr::core::position2di(l_iBorder, l_iRaster + 1), a_cRect.LowerRightCorner - irr::core::position2di(l_iBorder, l_iRaster + 1)));
#else
      // Draw the central part
      irr::core::recti l_cRect = irr::core::recti(a_cRect.UpperLeftCorner + irr::core::vector2di(0, l_iRaster - 1), a_cRect.UpperLeftCorner + irr::core::vector2di(l_iBorder, a_cRect.getHeight() - l_iRaster + 1));
      m_pDrv->draw2DRectangle(m_cBorder, l_cRect);
      l_cRect = irr::core::recti(a_cRect.UpperLeftCorner + irr::core::vector2di(a_cRect.getWidth() - l_iBorder, l_iRaster - 1), a_cRect.LowerRightCorner - irr::core::vector2di(0, l_iRaster - 1));
      m_pDrv->draw2DRectangle(m_cBorder, l_cRect);
      m_pDrv->draw2DRectangle(a_cColor, irr::core::recti(a_cRect.UpperLeftCorner + irr::core::position2di(l_iBorder, l_iRaster), a_cRect.LowerRightCorner - irr::core::position2di(l_iBorder, l_iRaster + 1)));
#endif

      // Draw the lower border
      for (int l_iLine = l_iHeight - l_iRaster - 1; l_iLine < l_iHeight; l_iLine++) {
        int l_iOffset = l_iRaster - (int)(sqrt(l_iRaster * l_iRaster - (l_iLine - l_iHeight + l_iRaster) * (l_iLine - l_iHeight + l_iRaster))),
            l_iInner  = l_iRaster - (int)(sqrt(l_iRadius * l_iRadius - (l_iLine - l_iHeight + l_iRaster) * (l_iLine - l_iHeight + l_iRaster)));

        if (l_iLine >= l_iHeight - l_iBorder) {
          m_pDrv->draw2DLine(irr::core::vector2di(l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner, m_cBorder);
        }
        else {
          irr::core::vector2di l_cLeft       = irr::core::vector2di(           l_iOffset, l_iLine) + a_cRect.UpperLeftCorner,
                               l_cInnerLeft  = irr::core::vector2di(           l_iInner , l_iLine) + a_cRect.UpperLeftCorner,
                               l_cRight      = irr::core::vector2di(l_iWidth - l_iOffset, l_iLine) + a_cRect.UpperLeftCorner,
                               l_cInnerRight = irr::core::vector2di(l_iWidth - l_iInner , l_iLine) + a_cRect.UpperLeftCorner;

          m_pDrv->draw2DLine(l_cLeft, l_cInnerLeft, m_cBorder);
          m_pDrv->draw2DLine(l_cInnerRight, l_cRight, m_cBorder);
          m_pDrv->draw2DLine(l_cInnerLeft, l_cInnerRight, a_cColor);
        }
      }
    }
  }
}