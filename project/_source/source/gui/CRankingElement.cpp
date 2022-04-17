// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <gui/CRankingElement.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CRankingElement::CRankingElement(int a_iPosition, const irr::core::recti& a_cRect, const irr::video::SColor &a_cBackground, irr::gui::IGUIFont* a_pFont, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui) :
      IGUIElement(irr::gui::EGUIET_COUNT, a_pGui, a_pParent != nullptr ? a_pParent : a_pGui->getRootGUIElement(), -1, a_cRect),
      m_cBackground(a_cBackground),
      m_cOriginal  (a_cBackground),
      m_cTextColor (irr::video::SColor(0xFF, 0, 0, 0)),
      m_iDeficit   (0),
      m_sName      (L""),
      m_sPosition  (std::to_wstring(a_iPosition) + L": "),
      m_pFont      (a_pFont)
    {
      irr::core::dimension2du l_cDimPos = m_pFont->getDimension(L"66: ");
      l_cDimPos.Width  = 3 * l_cDimPos.Width / 2;
      l_cDimPos.Height = a_cRect.getHeight();

      m_cPosition = irr::core::recti(AbsoluteClippingRect.UpperLeftCorner, AbsoluteClippingRect.UpperLeftCorner + irr::core::vector2di(l_cDimPos.Width, l_cDimPos.Height));

      irr::core::dimension2du l_cDimDeficit = m_pFont->getDimension(L"+666.66");
      l_cDimDeficit.Width  = 3 * l_cDimDeficit.Width / 2;
      l_cDimDeficit.Height = a_cRect.getHeight();

      m_cDeficit = irr::core::recti(AbsoluteClippingRect.LowerRightCorner.X - l_cDimDeficit.Width, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);

      m_iDeficit = 0;

      m_cName = irr::core::recti(m_cPosition.LowerRightCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, m_cDeficit.UpperLeftCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);

      m_iRaster = a_cRect.getHeight() /  4;
      m_iBorder = a_cRect.getHeight() / 16;

      if (m_iRaster < 8) m_iRaster = 8;
      if (m_iBorder < 1) m_iBorder = 1;
    }

    CRankingElement::~CRankingElement() {

    }

    /**
    * Update the data
    * @param a_sName name of the player
    * @param a_iDeficit deficit to the leader
    */
    void CRankingElement::setData(const std::wstring& a_sName, int a_iDeficit) {
      m_sName    = helpers::fitString(a_sName, m_pFont, irr::core::dimension2du(m_cName.getWidth(), m_cName.getHeight()));
      m_iDeficit = a_iDeficit;
    }

    void CRankingElement::draw() {
      if (IsVisible) {
        renderBackground(AbsoluteClippingRect, m_cBackground);

        m_pFont->draw(m_sPosition.c_str(), m_cPosition, m_cTextColor, true, true, &AbsoluteClippingRect);


        wchar_t s[0xFF];

        if (m_iDeficit >= 0) {
          swprintf(s, 0xFF, L"+%.2f", ((irr::f32)m_iDeficit) / 120.0f);
        }
        else {
          swprintf(s, 0xFF, L"+%i lap%s", abs(m_iDeficit), m_iDeficit != -1 ? L"s" : L"");
        }

        if (m_iDeficit != 0)
          m_pFont->draw(s, m_cDeficit, m_cTextColor, true, true, &AbsoluteClippingRect);

        m_pFont->draw(m_sName.c_str(), m_cName, m_cTextColor, false, true, &AbsoluteClippingRect);
      }
    }


    /**
    * Set the alpha value
    * @param a_fAlpha the alpha value
    */
    void CRankingElement::setAlpha(irr::f32 a_fAlpha) {
      m_cBackground = m_cOriginal;
      irr::u32 l_iAlpha = (irr::u32)(((irr::f32)m_cOriginal.getAlpha()) * a_fAlpha);
      m_cBackground.setAlpha(l_iAlpha);
      m_cBorder    .setAlpha((irr::u32)(255.0f * a_fAlpha));
      m_cTextColor .setAlpha((irr::u32)(255.0f * a_fAlpha));
    }
  }
}