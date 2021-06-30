// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gui/CMenuButton.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CMenuButton::CMenuButton(irr::gui::IGUIElement* a_pParent) : 
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_MenuButtonId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent, -1, irr::core::recti()), 
      m_pGui(CGlobal::getInstance()->getGuiEnvironment()), 
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_sImage(""),
      m_bHovered(false)
    {
      m_pBackgrounds[0] = nullptr;
      m_pBackgrounds[1] = nullptr;
      m_pBackgrounds[2] = nullptr;
    }

    CMenuButton::~CMenuButton() {
    }

    void CMenuButton::draw() {
      irr::core::recti l_cRect = getAbsoluteClippingRect();

      if (m_pBackgrounds[0] == nullptr) {
        std::string l_sUri = "button://off_" + std::to_string(l_cRect.getWidth()) + "x" + std::to_string(l_cRect.getHeight());
        m_pBackgrounds[0] = CGlobal::getInstance()->createTexture(l_sUri);
      }

      if (m_pBackgrounds[1] == nullptr) {
        std::string l_sUri = "button://hover_" + std::to_string(l_cRect.getWidth()) + "x" + std::to_string(l_cRect.getHeight());
        m_pBackgrounds[1] = CGlobal::getInstance()->createTexture(l_sUri);
      }

      if (m_pBackgrounds[2] == nullptr) {
        std::string l_sUri = "button://click_" + std::to_string(l_cRect.getWidth()) + "x" + std::to_string(l_cRect.getHeight());
        m_pBackgrounds[2] = CGlobal::getInstance()->createTexture(l_sUri);
      }

      m_pDrv->draw2DImage(m_bHovered ? m_bLDown ? m_pBackgrounds[2] : m_pBackgrounds[1] : m_pBackgrounds[0], l_cRect, irr::core::recti(irr::core::position2di(0, 0), m_pBackgrounds[0]->getSize()), nullptr, nullptr, true);

      if (m_pImage != nullptr)
        m_pDrv->draw2DImage(m_pImage, l_cRect, irr::core::recti(irr::core::position2di(0, 0), m_pImage->getSize()), nullptr, nullptr, true);

      irr::gui::IGUIFont* l_pFont = m_pFont;

      if (l_pFont == nullptr)
        l_pFont = m_pGui->getSkin()->getFont();

      l_pFont->draw(getText(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), true, true);
    }

    void CMenuButton::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);
      a_pOut->addString("ImagePath", m_sImage.c_str());
    }

    void CMenuButton::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      std::string s = m_sImage;
      m_sImage = a_pIn->getAttributeAsString("ImagePath").c_str();

      if (s != m_sImage) {
        if (m_pImage != nullptr)
          m_pImage->drop();

        m_pImage = m_pDrv->getTexture(m_sImage.c_str());
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CMenuButton::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_MenuButtonId;
    }

    void CMenuButton::setOverrideFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
    }

    bool CMenuButton::OnEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.Caller == this) {
          if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
            m_bHovered = true;
          }
          else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
            m_bHovered = false;
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        bool l_bWasDown = m_bLDown;

        m_bLDown = a_cEvent.MouseInput.isLeftPressed();

        if (l_bWasDown && !m_bLDown && m_bHovered) {
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_GUI_EVENT;
          l_cEvent.GUIEvent.Caller    = this;
          l_cEvent.GUIEvent.Element   = this;
          l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
          CGlobal::getInstance()->OnEvent(l_cEvent);
        }
      }

      return false;
    }
  }
}