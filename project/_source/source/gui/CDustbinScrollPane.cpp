// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CDustbinScrollPane.h>
#include <limits>

namespace dustbin {
  namespace gui {
    CDustbinScrollPane::CDustbinScrollPane(const irr::core::recti &a_cRect) : m_bMouseDown(false), m_cRectOuter(a_cRect) {
      m_cRectInner.UpperLeftCorner.X = std::numeric_limits<irr::s32>().max();
      m_cRectInner.UpperLeftCorner.Y = std::numeric_limits<irr::s32>().max();

      m_cRectInner.LowerRightCorner.X = std::numeric_limits<irr::s32>().min();
      m_cRectInner.LowerRightCorner.Y = std::numeric_limits<irr::s32>().min();
    }

    /**
    * Add an item to the scroll pane
    * @param a_cRect the rectangle to add
    */
    void CDustbinScrollPane::addRectangle(const irr::core::recti &a_cRect) {
      m_vItems.push_back(a_cRect);

      if (a_cRect.UpperLeftCorner.X < m_cRectInner.UpperLeftCorner.X) m_cRectInner.UpperLeftCorner.X = a_cRect.UpperLeftCorner.X;
      if (a_cRect.UpperLeftCorner.Y < m_cRectInner.UpperLeftCorner.Y) m_cRectInner.UpperLeftCorner.Y = a_cRect.UpperLeftCorner.Y;

      if (a_cRect.LowerRightCorner.X > m_cRectInner.LowerRightCorner.X) m_cRectInner.LowerRightCorner.X = a_cRect.LowerRightCorner.X;
      if (a_cRect.LowerRightCorner.Y > m_cRectInner.LowerRightCorner.Y) m_cRectInner.LowerRightCorner.Y = a_cRect.LowerRightCorner.Y;

      m_cScrollMax.X = (int)fmax(0, m_cRectInner.getWidth () - m_cRectOuter.getWidth () + m_vItems.back().getHeight());
      m_cScrollMax.Y = (int)fmax(0, m_cRectInner.getHeight() - m_cRectOuter.getHeight() + m_vItems.back().getHeight());
    }

    /**
    * Get the current scroll position
    * @return the current scroll position
    */
    const irr::core::position2di& CDustbinScrollPane::getScrollPosition() {
      return m_cScrollPos;
    }

    /**
    * Try to put an UI element into the center of the pane, or at least make it visilbe
    * @param a_cRect the rectangle to center
    */
    void CDustbinScrollPane::centerElement(const irr::core::recti &a_cRect) {

    }

    /**
    * Handle Irrlicht events. Mouse clicks will only be taken notice of, mouse movements
    * will be consumed if the left mouse button is down
    * @param a_cEvent the event to handle
    */
    bool CDustbinScrollPane::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bMouseDown = true;
          m_cMouseDown.X = a_cEvent.MouseInput.X;
          m_cMouseDown.Y = a_cEvent.MouseInput.Y;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseDown = false;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          if (m_bMouseDown) {
            // Handle mouse movement and consume event

            irr::core::position2di l_cMouse = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            if (l_cMouse != m_cMouseDown) {
              m_cScrollPos.X += l_cMouse.X - m_cMouseDown.X;
              m_cScrollPos.Y += l_cMouse.Y - m_cMouseDown.Y;

              if (m_cScrollPos.X > 0) m_cScrollPos.X = 0; if (m_cScrollPos.X <= -m_cScrollMax.X) m_cScrollPos.X = -m_cScrollMax.X + 1;
              if (m_cScrollPos.Y > 0) m_cScrollPos.Y = 0; if (m_cScrollPos.Y <= -m_cScrollMax.Y) m_cScrollPos.Y = -m_cScrollMax.Y + 1;

              m_cMouseDown = l_cMouse;
              l_bRet = true;
            }
          }
        }
      }

      return l_bRet;
    }
  }
}