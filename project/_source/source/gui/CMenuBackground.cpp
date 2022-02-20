// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CMenuBackground.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CMenuBackground::CMenuBackground(irr::gui::IGUIElement* a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType) :
      irr::gui::IGUIElement(a_eType, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_cOverrideColor(irr::video::SColor(0xc0, 0xef, 0xef, 0xff)),
      m_bOverrideColor(false),
      m_iZLayer(0)
    {
    }

    CMenuBackground::~CMenuBackground() {
    }

    int CMenuBackground::getZLayer() {
      return m_iZLayer;
    }

    void CMenuBackground::draw() {
      if (IsVisible) {
        renderBackground(getAbsoluteClippingRect(), m_bOverrideColor ? m_cOverrideColor : irr::video::SColor(0x80, 0xef, 0xef, 0xff));

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
      a_pOut->addInt("ZLayer", m_iZLayer);
    }

    void CMenuBackground::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      m_bOverrideColor = a_pIn->getAttributeAsBool("UseOverrideColor");
      m_cOverrideColor = a_pIn->getAttributeAsColor("OverrideColor");
      m_iZLayer        = a_pIn->getAttributeAsInt("ZLayer");
    }
  }
}
