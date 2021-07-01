// (w) 2021 by Dustbin::Games / Christian Keimel

#include <gui/CMenuBackground.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CMenuBackground::CMenuBackground(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_pTexture(nullptr)
    {
    }

    CMenuBackground::~CMenuBackground() {
    }

    void CMenuBackground::draw() {
      if (m_pTexture == nullptr) {
        std::string l_sUri = "button://c0efefff_" + std::to_string(getAbsoluteClippingRect().getWidth()) + "x" + std::to_string(getAbsoluteClippingRect().getHeight());
        m_pTexture = CGlobal::getInstance()->createTexture(l_sUri);
      }

      m_pDrv->draw2DImage(m_pTexture, getAbsoluteClippingRect(), irr::core::recti(irr::core::position2di(0, 0), m_pTexture->getSize()), nullptr, nullptr, true);
    }

    irr::gui::EGUI_ELEMENT_TYPE CMenuBackground::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId;
    }
  }
}
