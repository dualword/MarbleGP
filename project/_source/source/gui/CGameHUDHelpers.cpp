// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGameHUDHelpers.h>

namespace dustbin {
  namespace gui {
    SHighLight::SHighLight() : m_iMarbleId(-1), m_iPosition(0), m_bVisible(false), m_bFinished(false), m_bViewport(false), m_pArrow(nullptr), m_pPosition(nullptr) {
    }

    SLapTime::SLapTime() : m_iStart(0), m_iEnd(0), m_iLapNo(0), m_iLapTime(-1) {
    }

    SPlayerRacetime::SPlayerRacetime() : m_iFastest(-1), m_iLastSplit(-1) {
    }

    STextElement::STextElement(const irr::core::recti &a_cRect, const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont, const irr::video::SColor &a_cBackground, const irr::video::SColor &a_cTextColor, const irr::core::recti &a_cClip, irr::video::IVideoDriver *a_pDrv) :
      m_cThisRect  (a_cRect),
      m_cClip      (a_cClip),
      m_sText      (a_sText),
      m_pFont      (a_pFont),
      m_cBackground(a_cBackground),
      m_cTextColor (a_cTextColor),
      m_pDrv       (a_pDrv),
      m_eAlignV    (irr::gui::EGUIA_UPPERLEFT),
      m_eAlignH    (irr::gui::EGUIA_UPPERLEFT),
      m_bVisible   (true)
    {
    }

    STextElement::STextElement() :
      m_sText(L""),
      m_pFont(nullptr),
      m_pDrv (nullptr)
    {
    }

    void STextElement::render() {
      if (m_bVisible) {
        m_pDrv->draw2DRectangle(m_cBackground, m_cThisRect, &m_cClip);

        irr::core::recti        l_cTextRect = m_cThisRect;
        irr::core::dimension2du l_cTextDim  = m_pFont->getDimension(m_sText.c_str());

        if (m_eAlignH == irr::gui::EGUIA_LOWERRIGHT) {
          l_cTextRect.UpperLeftCorner.X = m_cThisRect.LowerRightCorner.X - l_cTextDim.Width;
        }

        if (m_eAlignV == irr::gui::EGUIA_LOWERRIGHT) {
          l_cTextRect.UpperLeftCorner.Y = m_cThisRect.LowerRightCorner.Y - l_cTextDim.Height;
        }

        m_pFont->draw(m_sText.c_str(), l_cTextRect, m_cTextColor, m_eAlignH == irr::gui::EGUIA_CENTER, m_eAlignV == irr::gui::EGUIA_CENTER, &m_cClip);
      }
    }

    void STextElement::setPosition(const irr::core::position2di& a_cPos) {
      irr::core::dimension2di l_cSize = m_cThisRect.getSize();
      m_cThisRect.UpperLeftCorner  = a_cPos;
      m_cThisRect.LowerRightCorner = a_cPos + l_cSize;
    }
  }
}