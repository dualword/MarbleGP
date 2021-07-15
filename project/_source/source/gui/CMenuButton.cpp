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
    }

    CMenuButton::~CMenuButton() {
    }

    void CMenuButton::draw() {
      irr::core::recti l_cRect = getAbsoluteClippingRect();

      renderBackground(getAbsoluteClippingRect(), m_bHovered ? m_bLDown ? irr::video::SColor(0xff, 0xec, 0xf1, 0x63) : irr::video::SColor(0xff, 0x33, 0x67, 0xb8) : irr::video::SColor(0xff, 0xb8, 0xc8, 0xff));

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

            irr::SEvent l_cEvent;
            l_cEvent.EventType          = irr::EET_GUI_EVENT;
            l_cEvent.GUIEvent.EventType = irr::gui::EGET_ELEMENT_HOVERED;
            l_cEvent.GUIEvent.Caller    = this;
            CGlobal::getInstance()->OnEvent(l_cEvent);
          }
          else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
            m_bHovered = false;
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        bool l_bWasDown = m_bLDown;
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bLDown = true;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bLDown = false;
        }
        
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