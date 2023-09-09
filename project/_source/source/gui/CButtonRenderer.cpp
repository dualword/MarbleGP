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
    void CButtonRenderer::renderBackground(const irr::core::recti& a_cRect, const irr::video::SColor& a_cColor, irr::video::SColor *a_pBorder) {
      int l_iHeight = a_cRect.getHeight();
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

      irr::core::position2di l_cUpperLeft = a_cRect.UpperLeftCorner;
      irr::core::position2di l_cLowerRght = a_cRect.LowerRightCorner;

      for (int y = 0; y < l_iHeight; y++) {
        int l_iLeft1  = a_cRect.UpperLeftCorner .X;
        int l_iLeft2  = a_cRect.UpperLeftCorner .X + l_iBorder;
        int l_iRight1 = a_cRect.LowerRightCorner.X;
        int l_iRight2 = a_cRect.LowerRightCorner.X - l_iBorder;

        if (y < l_iRaster) {
          double l_fX = sqrt((double)l_iRaster * (double)l_iRaster - (double)(l_iRaster - y) * (double)(l_iRaster - y));

          l_iLeft1  += l_iRaster - (int)l_fX;
          l_iRight1 -= l_iRaster - (int)l_fX;

          if (y >= l_iBorder) {
            int y1 = y - l_iBorder;

            l_fX = sqrt((double)l_iRadius * (double)l_iRadius - (double)(l_iRadius - y1) * (double)(l_iRadius - y1));

            l_iLeft2  += l_iRadius - (int)l_fX;
            l_iRight2 -= l_iRadius - (int)l_fX;
          }
        }
        else if (y > l_iHeight - l_iRaster) {
          double l_fY = y - l_iHeight + l_iRaster;
          double l_fX = sqrt((double)l_iRaster * (double)l_iRaster - l_fY * l_fY);

          l_iLeft1  += l_iRaster - (int)l_fX;
          l_iRight1 -= l_iRaster - (int)l_fX;

          if (y <= l_iHeight - l_iBorder) {
            int y1 = l_iHeight - l_iBorder - y;

            l_fX = sqrt((double)l_iRadius * (double)l_iRadius - (double)(l_iRadius - y1) * (double)(l_iRadius - y1));

            l_iLeft2  += l_iRadius - (int)l_fX;
            l_iRight2 -= l_iRadius - (int)l_fX;
          }
        }

        int l_iY = a_cRect.UpperLeftCorner.Y + y;

        if (y < l_iBorder || y >= l_iHeight - l_iBorder) {
          m_pDrv->draw2DLine(irr::core::vector2di(l_iLeft1, l_iY), irr::core::vector2di(l_iRight1, l_iY), a_pBorder != nullptr ? *a_pBorder : m_cBorder);
        }
        else {
          m_pDrv->draw2DLine(irr::core::vector2di(l_iLeft1 , l_iY), irr::core::vector2di(l_iLeft2 , l_iY), a_pBorder != nullptr ? *a_pBorder : m_cBorder);
          m_pDrv->draw2DLine(irr::core::vector2di(l_iLeft2 , l_iY), irr::core::vector2di(l_iRight1, l_iY), a_cColor);
          m_pDrv->draw2DLine(irr::core::vector2di(l_iRight1, l_iY), irr::core::vector2di(l_iRight2, l_iY), a_pBorder != nullptr ? *a_pBorder : m_cBorder);
        }
      }
    }
  }
}