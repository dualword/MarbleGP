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

      irr::core::recti l_cRects[] = {
        irr::core::recti(l_cUpperLeft                                                                                          , irr::core::dimension2di(l_iRaster                         , l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(l_iRaster                     , 0                              ), irr::core::dimension2di(a_cRect.getWidth() - 2 * l_iRaster, l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(a_cRect.getWidth() - l_iRaster, 0                              ), irr::core::dimension2di(l_iRaster                         , l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(                             0, l_iRaster                      ), irr::core::dimension2di(l_iRaster                         , a_cRect.getHeight() - 2 * l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(l_iRaster                     , l_iRaster                      ), irr::core::dimension2di(a_cRect.getWidth() - 2 * l_iRaster, a_cRect.getHeight() - 2 * l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(a_cRect.getWidth() - l_iRaster, l_iRaster                      ), irr::core::dimension2di(l_iRaster                         , a_cRect.getHeight() - 2 * l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(                             0, a_cRect.getHeight() - l_iRaster), irr::core::dimension2di(l_iRaster                         , l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(l_iRaster                     , a_cRect.getHeight() - l_iRaster), irr::core::dimension2di(a_cRect.getWidth() - 2 * l_iRaster, l_iRaster)),
        irr::core::recti(l_cUpperLeft + irr::core::position2di(a_cRect.getWidth() - l_iRaster, a_cRect.getHeight() - l_iRaster), irr::core::dimension2di(l_iRaster                         , l_iRaster))
      };

      for (int i = 0; i < 9; i++) {
        if (i != 0 && i != 2 && i != 6 && i != 8) {
          bool l_bBorder = false;

          irr::core::recti l_cBorder = l_cRects[i];

          switch (i) {
            case 1:
              l_cRects[i].UpperLeftCorner .Y += l_iBorder;
              l_cBorder  .LowerRightCorner.Y  = l_cRects[i].UpperLeftCorner.Y;
              l_bBorder = true;
              break;

            case 3:
              l_cRects[i].UpperLeftCorner .X += l_iBorder;
              l_cBorder  .LowerRightCorner.X  = l_cRects[i].UpperLeftCorner.X;
              l_bBorder = true;
              break;

            case 5:
              l_cRects[i].LowerRightCorner.X -= l_iBorder;
              l_cBorder  .UpperLeftCorner .X  = l_cRects[i].LowerRightCorner.X;
              l_bBorder = true;
              break;

            case 7:
              l_cRects[i].LowerRightCorner.Y -= l_iBorder;
              l_cBorder  .UpperLeftCorner .Y  = l_cRects[i].LowerRightCorner.Y;
              l_bBorder = true;
              break;
          }

          if (l_bBorder)
            m_pDrv->draw2DRectangle(m_cBorder, l_cBorder);

          m_pDrv->draw2DRectangle(a_cColor, l_cRects[i]);
        }
        else {
          for (int y = l_cRects[i].UpperLeftCorner.Y; y < l_cRects[i].LowerRightCorner.Y; y++) {
            int x1 = l_cRects[i].UpperLeftCorner .X;
            int x2 = l_cRects[i].LowerRightCorner.X;
            int x3 = x2;

            int l_iLine = (i == 0 || i == 2) ? y - l_cRects[i].UpperLeftCorner.Y : l_cRects[i].LowerRightCorner.Y - y;

            int l_iOffset = l_iRaster - (int)(sqrt(l_iRaster * l_iRaster - (l_iRaster - l_iLine) * (l_iRaster - l_iLine)));
            int l_iInner  = l_iRaster - (int)(sqrt(l_iRadius * l_iRadius - (l_iRaster - l_iLine) * (l_iRaster - l_iLine)));

            switch (i) {
              // Upper left corner
              case 0:
              // Lower left corner
              case 6: {
                if (l_iLine > l_iBorder) {
                  x3 = x1 + l_iOffset - 2;
                  x1 = x1 + l_iInner  - 1;

                  m_pDrv->draw2DLine(irr::core::vector2di(x1, y), irr::core::vector2di(x3, y), m_cBorder);
                  m_pDrv->draw2DLine(irr::core::vector2di(x1, y), irr::core::vector2di(x2, y), a_cColor);
                }
                else {
                  x1 = x1 + l_iOffset;
                  m_pDrv->draw2DLine(irr::core::vector2di(x1, y), irr::core::vector2di(x2, y), m_cBorder);
                }
                break;
              }

              // Upper right corner
              case 2:
              // Lower right corner
              case 8: {
                if (l_iLine > l_iBorder) {
                  x3 = x2 - l_iOffset;
                  x2 -= l_iInner;

                  m_pDrv->draw2DLine(irr::core::vector2di(x2, y), irr::core::vector2di(x3, y), m_cBorder);
                  m_pDrv->draw2DLine(irr::core::vector2di(x1, y), irr::core::vector2di(x2, y), a_cColor);
                }
                else {
                  x2 -= l_iOffset;
                  m_pDrv->draw2DLine(irr::core::vector2di(x1, y), irr::core::vector2di(x2, y), m_cBorder);
                }
                break;
              }
            }
          }
        }
      }
    }
  }
}