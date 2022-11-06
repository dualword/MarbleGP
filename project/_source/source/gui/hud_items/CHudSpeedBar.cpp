// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudSpeedBar.h>

namespace dustbin {
  namespace gui {
    CHudSpeedBar::CHudSpeedBar(irr::video::IVideoDriver* a_pDrv, irr::gui::IGUIFont *a_pFont, const irr::core::recti& a_cViewport) : m_pDrv(a_pDrv), m_pFont(a_pFont) {
      m_cText = a_pFont->getDimension(L"666");
      m_cText.Width  = 5 * m_cText.Width  / 4;
      m_cText.Height = 5 * m_cText.Height / 4;

      m_cSize.Width  = a_cViewport.getWidth() / 4;
      m_cSize.Height = m_cText.Height;

      m_cBar.Width  = m_cSize.Width - m_cText.Width;
      m_cBar.Height = m_cSize.Height;

      m_cOffset.X = m_cSize.Width - m_cBar.Width;

      int l_iSpeedOffset = m_cBar.Height / 8;
      if (l_iSpeedOffset < 2)
        l_iSpeedOffset = 2;

      m_cOffset.Y += l_iSpeedOffset;
      m_cBar.Width -= l_iSpeedOffset;
      m_cBar.Height -= 2 * l_iSpeedOffset;

      m_cOffset.Y = (m_cSize.Height - m_cBar.Height) / 2;

    }

    CHudSpeedBar::~CHudSpeedBar() {
    }

    /**
    * Render the speed bar
    * @param a_fVelocity the velocity of the marble
    * @param a_cPos the position to render to
    * @return the height of the rendered speed bar
    */
    irr::s32 CHudSpeedBar::render(irr::f32 a_fVelocity, const irr::core::position2di& a_cPos, const irr::core::recti &a_cClip) {
      irr::core::recti l_cTotal = irr::core::recti(a_cPos - irr::core::vector2di(m_cSize.Width, 0) / 2, m_cSize);

      m_pDrv->draw2DRectangle(irr::video::SColor(160, 192, 192, 192), l_cTotal, &a_cClip);
      wchar_t s[0xFF];
      swprintf(s, 0xFF, L"%.0f", a_fVelocity);
      m_pFont->draw(s, irr::core::recti(l_cTotal.UpperLeftCorner, m_cText), irr::video::SColor(0xFF, 0, 0, 0), true, true, &a_cClip);

      int l_iVel = a_fVelocity > 150.0f ? 150 : (int)a_fVelocity;

      irr::f32 l_fRd = a_fVelocity < 75.0f ? 0.0f : a_fVelocity > 125.0f ? 1.0f :        (a_fVelocity - 75.0f) / 50.0f;
      irr::f32 l_fGr = a_fVelocity < 75.0f ? 1.0f : a_fVelocity > 125.0f ? 0.0f : 1.0f - (a_fVelocity - 75.0f) / 50.0f;

      l_fGr = l_fGr * l_fGr;

      m_pDrv->draw2DRectangle(irr::video::SColor(128, (irr::u32)(255.0f * l_fRd), (irr::u32)(255.0f * l_fGr), 0), irr::core::recti(l_cTotal.UpperLeftCorner + m_cOffset, irr::core::dimension2di(l_iVel * m_cBar.Width / 150, m_cBar.Height)), &a_cClip);

      return l_cTotal.getHeight();
    }
  }
}