// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CVirtualKeyboard.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace gui {
    const int g_KeaboardId = MAKE_IRR_ID('d', 'k', 'e', 'y');
    const irr::c8 g_KeyboardName[] = "VirtualKeyboard";

    CVirtualKeyboard::CVirtualKeyboard(bool a_bCtrlActive) : 
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_KeaboardId, CGlobal::getInstance()->getGuiEnvironment(), CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()) ,
      m_pDevice    (CGlobal::getInstance()->getIrrlichtDevice()),
      m_pFont      (nullptr),
      m_pDrv       (nullptr),
      m_pAdd       (nullptr),
      m_pDel       (nullptr),
      m_pEnter     (nullptr),
      m_bMouseDown (false),
      m_bInside    (false),
      m_bMoved     (false),
      m_bCtrlActive(a_bCtrlActive),
      m_bMoving    (false),
      m_iMax       (0),
      m_pTarget    (nullptr)
    {
      m_pDrv = CGlobal::getInstance()->getVideoDriver();

      wchar_t s[] = L" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-";

      for (int i = 0; s[i] != L'\0'; i++) {
        m_vKeys.push_back(s[i]);
      }

      m_itKey = m_vKeys.begin();

      m_pAdd   = m_pDrv->getTexture("data/images/arrow_right.png" );
      m_pDel   = m_pDrv->getTexture("data/images/arrow_left.png"  );
      m_pEnter = m_pDrv->getTexture("data/images/arrow_enter.png" );
    }


    CVirtualKeyboard::~CVirtualKeyboard() {
      if (CGlobal::getInstance() != nullptr)
        CGlobal::getInstance()->virtualKeyboardDestroyed();

      if (Parent != nullptr) {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_USER_EVENT;
        l_cEvent.UserEvent.UserData1 = c_iEventKeyboardDestroyed;
        l_cEvent.UserEvent.UserData2 = 0;

        Parent->OnEvent(l_cEvent);
      }
    }

    void CVirtualKeyboard::addChar() {
      std::wstring s = m_pTarget->getText();
      s += *m_itKey;
      m_pTarget->setText(s.c_str());

      irr::SEvent l_cEvent;

      l_cEvent.EventType = irr::EET_GUI_EVENT;
      l_cEvent.GUIEvent.Element   = m_pTarget;
      l_cEvent.GUIEvent.Caller    = m_pTarget;
      l_cEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_CHANGED;

      m_pTarget->OnEvent(l_cEvent);

      initUI();
    }

    void CVirtualKeyboard::delChar() {
      std::wstring s = m_pTarget->getText();

      if (s.length() > 0) {
        s = s.substr(0, s.length() - 1);
        m_pTarget->setText(s.c_str());

        irr::SEvent l_cEvent;

        l_cEvent.EventType = irr::EET_GUI_EVENT;
        l_cEvent.GUIEvent.Element   = m_pTarget;
        l_cEvent.GUIEvent.Caller    = m_pTarget;
        l_cEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_CHANGED;

        m_pTarget->OnEvent(l_cEvent);
      }

      initUI();
    }

    /** Inherited from irr::gui::IGUIElement **/
    bool CVirtualKeyboard::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          m_cMouse.X = a_cEvent.MouseInput.X;
          m_cMouse.Y = a_cEvent.MouseInput.Y;

          if (m_bMouseDown) {
            m_cOffset.Y -= m_cMDown.Y - m_cMouse.Y;

            m_cTotalMove.Y -= m_cMDown.Y - m_cMouse.Y;
            m_cTotalMove.X += m_cMDown.X - m_cMouse.X;

            if (m_cOffset.Y < -m_cChar.getHeight()) {
              m_cOffset.Y += m_cChar.getHeight();
              m_itKey++;
              if (m_itKey == m_vKeys.end())
                m_itKey = m_vKeys.begin();
            }

            if (m_cOffset.Y > m_cChar.getHeight()) {
              m_cOffset.Y -= m_cChar.getHeight();
              if (m_itKey == m_vKeys.begin())
                m_itKey = m_vKeys.end();
              m_itKey--;
            }

            m_bMoved = true;
          }

          m_cMDown = m_cMouse;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_cMDown = m_cMouse;
          m_bMouseDown = true;
          m_bMoved     = false;
          m_cOffset    = irr::core::position2di();
          m_cTotalMove = irr::core::position2di();

          m_bMoving = m_cInner.isPointInside(m_cMouse);
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseDown = false;

          if (m_bMoving) {
            if (m_cOffset.Y < -m_cChar.getHeight() / 2) {
              m_itKey++;
              if (m_itKey == m_vKeys.end())
                m_itKey = m_vKeys.begin();
            }

            if (m_cOffset.Y > m_cChar.getHeight() / 2) {
              if (m_itKey == m_vKeys.begin())
                m_itKey = m_vKeys.end();
              m_itKey--;
            }
          }
          else {
            if (m_cAdd.isPointInside(m_cMouse))
              addChar();

            if (m_cDel.isPointInside(m_cMouse))
              delChar();
          }

          m_cOffset    = irr::core::position2di();
          m_cTotalMove = irr::core::position2di();
        }
      }
      else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT && a_cEvent.GUIEvent.Caller == this) {
          m_bInside = false;
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED && a_cEvent.GUIEvent.Caller == this) {
          m_bInside = true;
        }
      }
      else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (IsVisible) {
          if (a_cEvent.UserEvent.UserData1 == c_iEventMoveMouse) {
            switch (a_cEvent.UserEvent.UserData2) {
              case 0: if (m_itKey == m_vKeys.begin()) m_itKey = m_vKeys.end(); m_itKey--; break;
              case 1: m_itKey++; if (m_itKey == m_vKeys.end()) m_itKey = m_vKeys.begin(); break;
              case 2: delChar(); break;
              case 3: addChar(); break;
            }

            l_bRet = true;
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
            if (a_cEvent.UserEvent.UserData2 == 0) {
              setVisible(false);

              if (m_pDevice->getCursorControl() != nullptr && m_bCtrlActive)
                m_pDevice->getCursorControl()->setVisible(true);
            }

            l_bRet = true;
          }
        }
      }

      return l_bRet;
    }

    void CVirtualKeyboard::draw() {
      if (IsVisible) {
        m_pDrv->draw2DRectangleOutline(m_cChar, irr::video::SColor(0xFF, 0, 0, 0));
        m_pDrv->draw2DRectangleOutline(m_cUp  , irr::video::SColor(0xFF, 0, 0, 0));
        m_pDrv->draw2DRectangleOutline(m_cDown, irr::video::SColor(0xFF, 0, 0, 0));

        m_pDrv->draw2DRectangle(irr::video::SColor(192, 255, 255, 192), m_cUp  );
        m_pDrv->draw2DRectangle(irr::video::SColor(192, 255, 255, 192), m_cDown);

        irr::core::position2di l_cPos = m_cInner.UpperLeftCorner;

        wchar_t s[2] = { *m_itKey, L'\0' };

        m_pFont->draw(s, m_cChar + irr::core::position2di(0, m_cOffset.Y), irr::video::SColor(0xFF, 0, 0, 0), true, true, &AbsoluteClippingRect);

        std::vector<wchar_t>::iterator it = m_itKey;
        irr::core::recti r = m_cChar + irr::core::position2di(0, m_cOffset.Y);

        for (int i = 0; i < 3; i++) {
          r += irr::core::vector2di(0, m_cChar.getHeight());
          it++;

          if (it == m_vKeys.end())
            it = m_vKeys.begin();

          s[0] = *it;
          m_pFont->draw(s, r, irr::video::SColor(0xFF, 0, 0, 0), true, true, &AbsoluteClippingRect);
        }

        it = m_itKey;
        r = m_cChar + irr::core::position2di(0, m_cOffset.Y);

        for (int i = 0; i < 3; i++) {
          r -= irr::core::vector2di(0, m_cChar.getHeight());
          it--;

          if (it == m_vKeys.begin()) {
            it = m_vKeys.end();
            it--;
          }

          s[0] = *it;
          m_pFont->draw(s, r, irr::video::SColor(0xFF, 0, 0, 0), true, true, &AbsoluteClippingRect);
        }

        if (!m_bCtrlActive) {
          m_pDrv->draw2DRectangle(!m_cAdd.isPointInside(m_cMouse) ? irr::video::SColor(192, 224, 224, 224) : m_bMouseDown ? irr::video::SColor(0xFF, 0xFF, 128, 128) : irr::video::SColor(0xFF, 128, 128, 0xFF), m_cAdd);
          m_pDrv->draw2DRectangleOutline(m_cAdd, irr::video::SColor(0xFF, 0, 0, 0));

          if (m_pAdd != nullptr)
            m_pDrv->draw2DImage(m_pAdd, m_cAdd, irr::core::recti(irr::core::position2di(0, 0), m_pAdd->getOriginalSize()), &AbsoluteClippingRect, nullptr, true);

          m_pDrv->draw2DRectangle(!m_cDel.isPointInside(m_cMouse) ? irr::video::SColor(192, 224, 224, 224) : m_bMouseDown ? irr::video::SColor(0xFF, 0xFF, 128, 128) : irr::video::SColor(0xFF, 128, 128, 0xFF), m_cDel);
          m_pDrv->draw2DRectangleOutline(m_cDel, irr::video::SColor(0xFF, 0, 0, 0));

          if (m_pDel!= nullptr)
            m_pDrv->draw2DImage(m_pDel, m_cDel, irr::core::recti(irr::core::position2di(0, 0), m_pDel->getOriginalSize()), &AbsoluteClippingRect, nullptr, true);
        }
      }
    }

    void CVirtualKeyboard::setTarget(irr::gui::IGUIEditBox* a_pTarget) {
      m_pTarget = a_pTarget;

      if (m_pTarget != nullptr)
        initUI();
    }

    void CVirtualKeyboard::initUI() {
      irr::core::recti        l_cRect = m_pTarget->getAbsoluteClippingRect();
      irr::core::dimension2du l_cText = m_pTarget->getTextDimension();

      m_pFont = m_pTarget->getOverrideFont();

      if (m_pFont == nullptr)
        m_pFont = CGlobal::getInstance()->getGuiEnvironment()->getSkin()->getFont();

      if (l_cText.Width > (irr::u32)l_cRect.getWidth())
        l_cText.Width = l_cRect.getWidth();

      irr::core::dimension2du l_cChar = m_pFont->getDimension(L"X");

      if (l_cChar.Height < (irr::u32)l_cRect.getHeight())
        l_cChar.Height = l_cRect.getHeight();

      l_cChar.Width = l_cChar.Height;

      m_cChar = irr::core::recti(
        l_cRect.UpperLeftCorner .X + l_cText.Width,
        l_cRect.UpperLeftCorner .Y,
        l_cRect.UpperLeftCorner .X + l_cText.Width + l_cChar.Width,
        l_cRect.LowerRightCorner.Y
      );

      m_cUp = irr::core::recti(
        m_cChar.UpperLeftCorner - irr::core::vector2di(0, 2 * l_cChar.Height),
        m_cChar.UpperLeftCorner + irr::core::vector2di(l_cChar.Width, 0)
      );

      m_cDown = irr::core::recti(
        m_cChar.UpperLeftCorner + irr::core::vector2di(0            ,     m_cChar.getHeight()),
        m_cChar.UpperLeftCorner + irr::core::vector2di(l_cChar.Width, 3 * m_cChar.getHeight())
      );

      m_cInner = irr::core::recti(
        m_cUp  .UpperLeftCorner,
        m_cDown.LowerRightCorner
      );

      AbsoluteClippingRect = m_cInner;

      irr::s32 l_iRaster = CGlobal::getInstance()->getRasterSize();

      irr::core::dimension2di l_cDimAdd = irr::core::dimension2di(l_iRaster * 2, l_iRaster * 2);
      irr::core::position2di  l_cPosAdd = irr::core::position2di (m_cInner.LowerRightCorner.X, m_pTarget->getAbsoluteClippingRect().UpperLeftCorner.Y - l_cDimAdd.Height);

      m_cAdd = irr::core::recti(l_cPosAdd, l_cDimAdd);
      AbsoluteClippingRect.LowerRightCorner.X = m_cAdd.LowerRightCorner.X;

      irr::core::position2di l_cPosDel = irr::core::position2di(m_cInner.UpperLeftCorner.X - l_cDimAdd.Width, m_pTarget->getAbsoluteClippingRect().UpperLeftCorner.Y - l_cDimAdd.Height);

      m_cDel = irr::core::recti(l_cPosDel, l_cDimAdd);
      AbsoluteClippingRect.UpperLeftCorner.X = m_cDel.UpperLeftCorner.X;


      irr::gui::ICursorControl *l_pCursor = m_pDevice->getCursorControl();
      if (l_pCursor != nullptr && m_bCtrlActive) {
        l_pCursor->setPosition(AbsoluteClippingRect.getCenter());
        m_pDevice->getCursorControl()->setVisible(false);
      }
      else {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
        l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
        l_cEvent.MouseInput.X = AbsoluteClippingRect.getCenter().X;
        l_cEvent.MouseInput.Y = AbsoluteClippingRect.getCenter().Y;
        m_pDevice->postEventFromUser(l_cEvent);
      }
    }
  }
}