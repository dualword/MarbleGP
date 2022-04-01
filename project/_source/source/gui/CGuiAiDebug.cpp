// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGuiAiDebug.h>

namespace dustbin {
  namespace gui {
    CGuiAiDebug::CGuiAiDebug(irr::gui::IGUIEnvironment* a_pGui) : 
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)MAKE_IRR_ID('a', 'i', 'd', 'b'), a_pGui, a_pGui->getRootGUIElement(), -1, irr::core::recti(irr::core::position2di(0, 0), a_pGui->getVideoDriver()->getScreenSize() / 4)),
      m_pGui(a_pGui),
      m_pDrv(a_pGui->getVideoDriver())
    {

    }

    CGuiAiDebug::~CGuiAiDebug() {
    }

    void CGuiAiDebug::draw() {
      m_pDrv->draw2DRectangle(irr::video::SColor(96, 224, 224, 224), AbsoluteClippingRect);

      irr::f32 l_iSize = (irr::f32)(AbsoluteClippingRect.getHeight()) / 2.0f;

      irr::core::vector2di l_cCenter = irr::core::vector2di(AbsoluteClippingRect.getWidth() / 2, AbsoluteClippingRect.getHeight());

      for (std::vector<irr::core::line2df>::iterator it = m_vLines.begin(); it != m_vLines.end(); it++) {
        irr::core::vector2di l_cStart = irr::core::vector2di(l_cCenter.X + (irr::s32)(((irr::f32)(*it).start.X) * l_iSize), l_cCenter.Y - (irr::s32)(((irr::f32)(*it).start.Y) * l_iSize));
        irr::core::vector2di l_cEnd   = irr::core::vector2di(l_cCenter.X + (irr::s32)(((irr::f32)(*it).end  .X) * l_iSize), l_cCenter.Y - (irr::s32)(((irr::f32)(*it).end  .Y) * l_iSize));

        m_pDrv->draw2DLine(l_cStart, l_cEnd, irr::video::SColor(0xFF, 0, 0, 0));
      }
    }

    void CGuiAiDebug::clearLines() {
      m_vLines.clear();
    }

    void CGuiAiDebug::addLine(const irr::core::line2df& a_cLine) {
      m_vLines.push_back(a_cLine);
    }
  }
}