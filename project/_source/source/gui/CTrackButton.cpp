#include <platform/CPlatform.h>
#include <gui/CTrackButton.h>
#include <CGlobal.h>
#include <vector>

namespace dustbin {
  namespace gui {
    CTrackButton::CTrackButton(irr::gui::IGUIElement* a_pParent) :
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_TrackButtonId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent, -1, irr::core::recti()),
      m_pFont(CGlobal::getInstance()->getFont(enFont::Regular, CGlobal::getInstance()->getVideoDriver()->getScreenSize())),
      m_pGui(CGlobal::getInstance()->getGuiEnvironment()),
      m_cOverlay(irr::video::SColor(128, 208, 208, 208)),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_cClicked(irr::video::SColor(128, 96, 96, 208)),
      m_iTotalHeight(0),
      m_pImage(nullptr),
      m_bHovered(false),
      m_iLineHeight(0),
      m_iSpaceWidth(5),
      m_sMyText(L""),
      m_sTrack(""),
      m_sImage("")
    {
      m_iSpaceWidth = m_pFont->getDimension(L" ").Width;
    }

    CTrackButton::~CTrackButton() {
    }

    void CTrackButton::draw() {
      if (IsVisible) {
        if (m_pImage != nullptr) {
          m_pDrv->draw2DImage(m_pImage, getAbsoluteClippingRect(), irr::core::recti(0, 0, m_pImage->getSize().Width, m_pImage->getSize().Height), nullptr, nullptr, true);
        }

        if (m_bHovered) {
          m_pDrv->draw2DRectangle(m_bLDown ? m_cClicked : m_cOverlay, getAbsoluteClippingRect());

          if (Text != "") {
            irr::core::recti l_cRect = getAbsoluteClippingRect();

            if (m_iTotalHeight == 0 || m_sMyText != Text.c_str()) {
              calculateLines(getText());
              m_sMyText = Text.c_str();
            }

            irr::core::position2di l_cPos = l_cRect.getCenter() - (m_iTotalHeight / 2);
            l_cPos.X = l_cRect.UpperLeftCorner.X;

            irr::core::recti l_cText = irr::core::recti(l_cPos, irr::core::dimension2du(l_cRect.getWidth(), m_iLineHeight));

            for (std::vector<std::wstring>::iterator it = m_vLines.begin(); it != m_vLines.end(); it++) {
              m_pFont->draw((*it).c_str(), l_cText, irr::video::SColor(255, 0, 0, 0), true, true);
              l_cText.UpperLeftCorner .Y += m_iLineHeight;
              l_cText.LowerRightCorner.Y += m_iLineHeight;
            }
          }
        }
      }
    }

    void CTrackButton::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      IGUIElement::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addString("Texture", m_sImage.c_str());
      a_pOut->addColor("Overlay", m_cOverlay);
      a_pOut->addColor("Clicked", m_cClicked);
      a_pOut->addString("Track", m_sTrack.c_str());
    }
    
    void CTrackButton::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      std::string l_sImage = a_pIn->getAttributeAsString("Texture").c_str();

      if (l_sImage != m_sImage) {
        m_sImage = l_sImage;
        m_pImage = CGlobal::getInstance()->createTexture(l_sImage);
      }

      m_cOverlay = a_pIn->getAttributeAsColor ("Overlay");
      m_cClicked = a_pIn->getAttributeAsColor ("Clicked");
      m_sTrack   = a_pIn->getAttributeAsString("Track"  ).c_str();
    }

    irr::gui::EGUI_ELEMENT_TYPE CTrackButton::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_TrackButtonId;
    }

    void CTrackButton::setOverrideFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
      m_iSpaceWidth = m_pFont->getDimension(L" ").Width;
      calculateLines(getText());
    }

    void CTrackButton::calculateLines(const std::wstring& a_sText) {
      std::wstring l_sText = a_sText;

      std::vector<std::wstring> l_vWords;

      for (size_t i = 0; i < l_sText.size(); i++) {
        if (l_sText.substr(i, 1) == L" " || l_sText.substr(i, 1) == L"\t") {
          std::wstring s = l_sText.substr(0, i);
          l_sText = l_sText.substr(i + 1);
          l_vWords.push_back(s);
        }
      }

      l_vWords.push_back(l_sText);

      m_vLines.clear();

      std::vector<std::wstring> l_vLine;

      int l_iWidth = 90 * getAbsoluteClippingRect().getWidth() / 100;

      for (std::vector<std::wstring>::iterator it = l_vWords.begin(); it != l_vWords.end(); it++) {
        int l_iRight = 0;
        for (std::vector<std::wstring>::iterator it = l_vLine.begin(); it != l_vLine.end(); it++) {
          l_iRight += m_pFont->getDimension((*it).c_str()).Width + m_iSpaceWidth;
        }

        if (l_iRight + m_pFont->getDimension((*it).c_str()).Width >= (unsigned)l_iWidth) {
          std::wstring l_sLine = L"";
          for (std::vector<std::wstring>::iterator it = l_vLine.begin(); it != l_vLine.end(); it++) {
            if (it != l_vLine.begin())
              l_sLine += L" ";

            l_sLine += *it;
          }
          m_vLines.push_back(l_sLine);
          l_vLine.clear();
        }
        l_vLine.push_back(*it);
      }

      if (l_vLine.size() > 0) {
        std::wstring l_sLine = L"";
        for (std::vector<std::wstring>::iterator it = l_vLine.begin(); it != l_vLine.end(); it++) {
          if (it != l_vLine.begin())
            l_sLine += L" ";
          l_sLine += *it;
        }
        m_vLines.push_back(l_sLine);
      }

      m_iTotalHeight = (int)(m_vLines.size() * m_pFont->getDimension(L"Test").Height);
      m_iLineHeight  =                         m_pFont->getDimension(L"Test").Height;
    }

    bool CTrackButton::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.Caller == this) {
          if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
            m_bHovered = true;
            l_bRet = true;
          }
          else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
            m_bHovered = false;
            l_bRet = true;
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bLDown = true;
          l_bRet = true;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          if (m_bLDown && m_bHovered) {
            irr::SEvent l_cEvent;
            l_cEvent.EventType = irr::EET_GUI_EVENT;
            l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
            l_cEvent.GUIEvent.Caller = this;
            l_cEvent.GUIEvent.Element = this;
            CGlobal::getInstance()->getActiveState()->OnEvent(l_cEvent);
          }

          m_bLDown = false;
          l_bRet = true;
        }
      }

      return l_bRet;
    }

  } // namespace gui
}   // namespace dustbin
