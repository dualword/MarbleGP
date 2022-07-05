// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <gui/CGuiTextField.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CGuiTextField::CGuiTextField(irr::gui::IGUIElement* a_pParent) :
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_TextFieldId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pDrv   (CGlobal::getInstance()->getVideoDriver()),
      m_pScroll(nullptr),
      m_pFont  (nullptr),
      m_bMouse (false),
      m_iScroll(0),
      m_iMax   (0)
    {
      m_pFont = CGlobal::getInstance()->getFont(enFont::Regular, m_pDrv->getScreenSize());
    }

    CGuiTextField::~CGuiTextField() {

    }

    void CGuiTextField::draw() {
      if (IsVisible) {
        m_pDrv->draw2DRectangle(irr::video::SColor(192, 224, 224, 244), AbsoluteClippingRect, &AbsoluteClippingRect);

        irr::u32 l_iPosY = -m_iScroll;

        irr::core::vector2di l_cUpperLeft = irr::core::position2di(m_cInner.UpperLeftCorner.X, m_cInner.UpperLeftCorner.Y + l_iPosY);

        for (std::vector<STextItem>::iterator it = m_vItems.begin(); it != m_vItems.end(); it++) {
          if ((*it).m_eType == enTextItem::Newline) {
            l_cUpperLeft.X = m_cInner.UpperLeftCorner.X;
            l_cUpperLeft.Y += 3 * (*it).m_cSize.Y / 2;
          }
          else {
            std::wstring s = (*it).m_sText;

            irr::core::vector2di l_cLowerRight = l_cUpperLeft + (*it).m_cSize;

            if (l_cLowerRight.X >= m_cInner.LowerRightCorner.X) {
              l_cUpperLeft.X = m_cInner.UpperLeftCorner.X;
              l_cUpperLeft.Y += 3 * (*it).m_cSize.Y / 2;

              l_cLowerRight = l_cUpperLeft + (*it).m_cSize;
            }

            m_pFont->draw(
              s.c_str(), 
              irr::core::recti(l_cUpperLeft, l_cLowerRight),
              irr::video::SColor(0xFF, 0, 0, 0),
              false,
              false,
              &m_cInner
            );

            l_cUpperLeft.X += (*it).m_cSize.X;
          }
        }

        IGUIElement::draw();
      }
    }

    /**
    * Set the text of the element
    * @param a_sText the new text of the element
    */
    void CGuiTextField::setNewText(const std::string& a_sText) {
      m_sFullText = helpers::s2ws(a_sText);

      std::wstring l_sWord = L"";

      m_vItems.clear();

      wchar_t c[] = { L'\0', L'\0' };

      irr::core::vector2di l_cLowerRight = irr::core::vector2di();

      for (size_t i = 0; i < m_sFullText.size(); i++) {
        c[0] = m_sFullText.c_str()[i];

        if (c[0] == L' ') {
          STextItem t = STextItem(enTextItem::Text , l_sWord, m_pFont);

          m_vItems.push_back(t);
          m_vItems.push_back(STextItem(enTextItem::Space, L" "   , m_pFont));
          l_sWord = L"";

          l_cLowerRight.X += t.m_cSize.X;

          if (l_cLowerRight.X >= m_cInner.LowerRightCorner.X) {
            l_cLowerRight.X = m_cInner.UpperLeftCorner.X + t.m_cSize.X;
            l_cLowerRight.Y += 3 * t.m_cSize.Y / 2;
          }
        }
        else if (c[0] == L'\n') {
          STextItem t = STextItem(enTextItem::Text , l_sWord, m_pFont);

          m_vItems.push_back(t);
          m_vItems.push_back(STextItem(enTextItem::Newline, L"X", m_pFont));
          l_sWord = L"";

          l_cLowerRight.X = m_cInner.UpperLeftCorner.X + t.m_cSize.X;
          l_cLowerRight.Y += 3 * t.m_cSize.Y / 2;
        }
        else {
          if (c[0] != L'\r') {
            l_sWord += c[0];
          }
        }
      }

      m_iMax = l_cLowerRight.Y;

      m_iScroll = 0;
    }

    bool CGuiTextField::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bMouse = true;
          m_cMouse = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouse = false;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          if (m_bMouse) {
            m_iScroll -= a_cEvent.MouseInput.Y - m_cMouse.Y;
            m_cMouse.Y = a_cEvent.MouseInput.Y;

            m_iScroll = std::min(m_iScroll, (int)(m_iMax - m_cInner.getHeight() + 3 * m_pFont->getDimension(L"XX").Height));
            m_iScroll = std::max(m_iScroll, 0);
          }
        }
      }

      return l_bRet;
    }

    void CGuiTextField::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      IGUIElement::serializeAttributes(a_pOut, a_pOptions);
    }

    void CGuiTextField::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      if (m_pScroll != nullptr)
        m_pScroll->setVisible(false);

      irr::core::dimension2du l_cDim = m_pFont->getDimension(L"HelloWorld");

      irr::core::recti l_cRect = irr::core::recti(0, AbsoluteClippingRect.getWidth() - l_cDim.Height, AbsoluteClippingRect.getHeight(), AbsoluteClippingRect.getWidth());

      m_pScroll = CGlobal::getInstance()->getGuiEnvironment()->addScrollBar(false, l_cRect, this);

      m_cInner = AbsoluteClippingRect;
      m_cInner.UpperLeftCorner  += irr::core::vector2di(l_cDim.Height / 2);
      m_cInner.LowerRightCorner -= irr::core::vector2di(l_cDim.Height / 2);
      m_cInner.LowerRightCorner.X -= l_cDim.Height;
    }

    CGuiTextField::STextItem::STextItem(enTextItem a_eType, const std::wstring& a_sText, irr::gui::IGUIFont* a_pFont) {
      m_eType = a_eType;
      m_sText = a_sText;

      irr::core::dimension2du d = a_pFont->getDimension(m_sText.c_str());
      m_cSize = irr::core::vector2di(d.Width, d.Height);
    }

    CGuiTextField::STextItem::STextItem(const STextItem& a_cOther) {
      m_eType = a_cOther.m_eType;
      m_sText = a_cOther.m_sText;
      m_cSize = a_cOther.m_cSize;
    }
  }
}