// (w) 2021 by Dustbin::Games / Christian Keimel

#include <gui/CMenuBackground.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CMenuBackground::CMenuBackground(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_cOverrideColor(irr::video::SColor(0xc0, 0xef, 0xef, 0xff)),
      m_bOverrideColor(false),
      m_pTexture(nullptr)
    {
    }

    CMenuBackground::~CMenuBackground() {
    }

    void CMenuBackground::draw() {
      if (IsVisible) {
        if (m_pTexture == nullptr) {
          std::string l_sColor = "c0efefff";

          if (m_bOverrideColor) {
            char s[0xFF];
            sprintf_s(s, "%02x%02x%02x%02x", m_cOverrideColor.getAlpha(), m_cOverrideColor.getRed(), m_cOverrideColor.getGreen(), m_cOverrideColor.getBlue());
            printf("==> %s\n", s);
            l_sColor = s;
          }


          std::string l_sUri = "button://" + l_sColor + "_" + std::to_string(getAbsoluteClippingRect().getWidth()) + "x" + std::to_string(getAbsoluteClippingRect().getHeight());
          m_pTexture = CGlobal::getInstance()->createTexture(l_sUri);
        }

        m_pDrv->draw2DImage(m_pTexture, getAbsoluteClippingRect(), irr::core::recti(irr::core::position2di(0, 0), m_pTexture->getSize()), nullptr, nullptr, true);

        IGUIElement::draw();
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CMenuBackground::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId;
    }

    void CMenuBackground::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);
      a_pOut->addBool("UseOverrideColor", m_bOverrideColor);
      a_pOut->addColor("OverrideColor", m_cOverrideColor);
    }

    void CMenuBackground::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      m_bOverrideColor = a_pIn->getAttributeAsBool("UseOverrideColor");
      m_cOverrideColor = a_pIn->getAttributeAsColor("OverrideColor");
    }
  }
}
