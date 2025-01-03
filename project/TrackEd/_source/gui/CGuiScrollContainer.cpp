// (w) 2025 by Dustbin::Games / Christian Keimel
#include <gui/CGuiScrollContainer.h>

namespace dustbin {
  namespace gui {
    CGuiScrollContainer::CGuiScrollContainer(irr::gui::IGUIEnvironment *a_pGui, irr::gui::IGUIElement *a_pParent, const irr::core::recti &a_cRect) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)(irr::gui::EGET_COUNT + 1234), a_pGui, a_pParent, -1, a_cRect),
      m_pScrollBar(nullptr),
      m_pDrv      (a_pGui->getVideoDriver()),
      m_pGui      (a_pGui),
      m_iScrollPos(0)
    {
      m_pScrollBar = a_pGui->addScrollBar(
        false,
        irr::core::recti(
          a_cRect.getWidth () - a_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH),
          0,
          a_cRect.getWidth (),
          a_cRect.getHeight()
        ),
        this
      );

      m_pScrollBar->setVisible(false);
    }

    CGuiScrollContainer::~CGuiScrollContainer() {
    }

    void CGuiScrollContainer::updateScrollbar() {
      irr::s32 l_iMaxY = 0;

      for (auto l_pChild: getChildren()) {
        if (l_pChild->getRelativePosition().LowerRightCorner.Y > l_iMaxY) {
          l_iMaxY = l_pChild->getRelativePosition().LowerRightCorner.Y;
        }
      }

      if (l_iMaxY > RelativeRect.getHeight()) {
        m_pScrollBar->setVisible(true);
        m_pScrollBar->setMax(l_iMaxY - RelativeRect.getHeight());

        irr::s32 l_iMaxX = m_pScrollBar->getRelativePosition().UpperLeftCorner.X - m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH) / 4;

        for (auto l_pChild : getChildren()) {
          if (l_pChild != m_pScrollBar) {
            if (l_pChild->getRelativePosition().LowerRightCorner.X > l_iMaxX) {
              irr::core::recti r = l_pChild->getRelativePosition();
              r.LowerRightCorner.X = l_iMaxX;
              l_pChild->setRelativePosition(r);
            }
          }
        }
      }
      else m_pScrollBar->setVisible(false);
    }

    void CGuiScrollContainer::addChild(IGUIElement *a_pChild) {
      IGUIElement::addChild(a_pChild);
      updateScrollbar();
    }

    void CGuiScrollContainer::removeChild(IGUIElement *a_pChild) {
      IGUIElement::removeChild(a_pChild);
      m_pScrollBar->setPos(0);
      m_iScrollPos = 0;
      updateScrollbar();
    }

    void CGuiScrollContainer::draw() {
      IGUIElement::draw();
    }

    bool CGuiScrollContainer::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED && a_cEvent.GUIEvent.Caller == m_pScrollBar) {
          irr::s32 l_iMove = m_pScrollBar->getPos() - m_iScrollPos;

          for (auto l_pChild : getChildren())
            if (l_pChild != m_pScrollBar)
              l_pChild->setRelativePosition(l_pChild->getRelativePosition().UpperLeftCorner - irr::core::position2di(0, l_iMove));

          m_iScrollPos = m_pScrollBar->getPos();

          l_bRet = true;
        }
      }

      if (!l_bRet)
        IGUIElement::OnEvent(a_cEvent);

      return l_bRet;
    }
  }
}