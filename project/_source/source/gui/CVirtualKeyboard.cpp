// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CVirtualKeyboard.h>
#include <CGlobal.h>
#include <Defines.h>
#include <tuple>
#include <cwctype>

namespace dustbin {
  namespace gui {
    const int g_KeaboardId = MAKE_IRR_ID('d', 'k', 'e', 'y');

    CVirtualKeyboard::CVirtualKeyboard(bool a_bCtrlActive) : 
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_KeaboardId, CGlobal::getInstance()->getGuiEnvironment(), CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())),
      m_pDevice    (CGlobal::getInstance()->getIrrlichtDevice()),
      m_pFont      (nullptr),
      m_pDrv       (nullptr),
      m_cBackground(irr::video::SColor(192, 255, 255, 192)),
      m_bMouseDown (false),
      m_bInside    (false),
      m_bMoved     (false),
      m_bCtrlActive(a_bCtrlActive),
      m_bMoving    (false),
      m_bShift     (false),
      m_iMouseDown (0),
      m_pTimer     (nullptr),
      m_pTarget    (nullptr),
      m_eMode      (enMode::VirtualKeys)
    {
      m_pDrv = CGlobal::getInstance()->getVideoDriver();

      wchar_t s[] = L" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-.";

      for (int i = 0; s[i] != L'\0'; i++) {
        m_vKeys.push_back(s[i]);
      }

      m_itKey = m_vKeys.begin();

      m_pTimer = CGlobal::getInstance()->getIrrlichtDevice()->getTimer();

      std::vector<std::vector<std::tuple<irr::EKEY_CODE, std::wstring, bool>>> l_eLayout;

      l_eLayout = {
        { std::make_tuple(irr::KEY_KEY_1, L"1", false), std::make_tuple(irr::KEY_KEY_2, L"2"    , false), std::make_tuple(irr::KEY_KEY_3 , L"3", false), std::make_tuple(irr::KEY_KEY_4, L"4", false), std::make_tuple(irr::KEY_KEY_5, L"5", false), std::make_tuple(irr::KEY_KEY_6, L"6", false), std::make_tuple(irr::KEY_KEY_7, L"7", false), std::make_tuple(irr::KEY_KEY_8, L"8", false), std::make_tuple(irr::KEY_KEY_9, L"9", false), std::make_tuple(irr::KEY_KEY_0, L"0", false), std::make_tuple(irr::KEY_BACK , L"◄", false) },
        { std::make_tuple(irr::KEY_KEY_Q, L"q", true ), std::make_tuple(irr::KEY_KEY_W, L"w"    , true ), std::make_tuple(irr::KEY_KEY_E , L"e", true ), std::make_tuple(irr::KEY_KEY_R, L"r", true ), std::make_tuple(irr::KEY_KEY_T, L"t", true ), std::make_tuple(irr::KEY_KEY_Y, L"y", true ), std::make_tuple(irr::KEY_KEY_U, L"u", true ), std::make_tuple(irr::KEY_KEY_I, L"i", true ), std::make_tuple(irr::KEY_KEY_O, L"o", true ), std::make_tuple(irr::KEY_KEY_P, L"p", true ) },
        { std::make_tuple(irr::KEY_KEY_A, L"a", true ), std::make_tuple(irr::KEY_KEY_S, L"s"    , true ), std::make_tuple(irr::KEY_KEY_D , L"d", true ), std::make_tuple(irr::KEY_KEY_F, L"f", true ), std::make_tuple(irr::KEY_KEY_G, L"g", true ), std::make_tuple(irr::KEY_KEY_G, L"g", true ), std::make_tuple(irr::KEY_KEY_H, L"h", true ), std::make_tuple(irr::KEY_KEY_J, L"j", true ), std::make_tuple(irr::KEY_KEY_K, L"k", true ), std::make_tuple(irr::KEY_KEY_L, L"l", true)},
        { std::make_tuple(irr::KEY_KEY_Z, L"z", true ), std::make_tuple(irr::KEY_KEY_X, L"x"    , true ), std::make_tuple(irr::KEY_KEY_C , L"c", true ), std::make_tuple(irr::KEY_KEY_V, L"v", true ), std::make_tuple(irr::KEY_KEY_B, L"b", true ), std::make_tuple(irr::KEY_KEY_N, L"n", true ), std::make_tuple(irr::KEY_KEY_M, L"m", true ), std::make_tuple(irr::KEY_PLUS , L"+", false), std::make_tuple(irr::KEY_MINUS, L"-", false), std::make_tuple(irr::KEY_DECIMAL, L".", false) },
        { std::make_tuple(irr::KEY_SHIFT, L"▲", false), std::make_tuple(irr::KEY_SPACE, L"Space", false), std::make_tuple(irr::KEY_RETURN, L"▼", false) }
      };

      irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(enFont::Huge, m_pDrv->getScreenSize());

      irr::core::dimension2du l_cDim = l_pFont->getDimension(L"X");

      if (l_cDim.Width > l_cDim.Height) l_cDim.Height = l_cDim.Width; else l_cDim.Width = l_cDim.Height;

      l_cDim.Width  = 3 * l_cDim.Width  / 2;
      l_cDim.Height = 3 * l_cDim.Height / 2;

      irr::core::position2di l_cPos = irr::core::position2di(0, 0);

      irr::s32 l_iMax = -1;

      for (auto &l_eRow : l_eLayout) {
        l_cPos.X = l_eRow.size() == 10 ? l_cDim.Width / 2 : l_eRow.size() == 9 ? l_cDim.Width : 0;
        for (auto &l_eKey : l_eRow) {
          irr::core::recti l_cRect = irr::core::recti(l_cPos, l_cDim);

          if (std::get<0>(l_eKey) == irr::KEY_SHIFT)
            l_cRect.LowerRightCorner.X += l_cDim.Width;
          else if (std::get<0>(l_eKey) == irr::KEY_SPACE)
            l_cRect.LowerRightCorner.X += 6 * l_cDim.Width;
          else if (std::get<0>(l_eKey) == irr::KEY_RETURN)
            l_cRect.LowerRightCorner.X += l_cDim.Width;

          m_vVirtualKeys.push_back(SVirtualKey(l_cRect, std::get<0>(l_eKey), std::get<1>(l_eKey), std::get<2>(l_eKey)));
          l_cPos.X += l_cRect.getWidth();
        }

        if (l_cPos.X > l_iMax)
          l_iMax = l_cPos.X;

        l_cPos.Y += l_cDim.Height;
      }

      m_cVkOffset = irr::core::position2di(m_pDrv->getScreenSize().Width / 2 - l_iMax / 2, m_pDrv->getScreenSize().Height - l_cPos.Y);
      m_cVkSize   = irr::core::dimension2du(l_iMax, l_cPos.Y);

      initUI();
      // AbsoluteClippingRect = irr::core::recti(m_cVkOffset, irr::core::dimension2du(l_iMax, l_cPos.Y));
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

      irr::SEvent l_cEvent{};

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

        irr::SEvent l_cEvent{};

        l_cEvent.EventType = irr::EET_GUI_EVENT;
        l_cEvent.GUIEvent.Element   = m_pTarget;
        l_cEvent.GUIEvent.Caller    = m_pTarget;
        l_cEvent.GUIEvent.EventType = irr::gui::EGET_EDITBOX_CHANGED;

        m_pTarget->OnEvent(l_cEvent);
      }

      initUI();
    }

    void CVirtualKeyboard::hideKeyboard() {
      setVisible(false);

      irr::SEvent l_cEvent{};

      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventHideCursor;
      l_cEvent.UserEvent.UserData2 = 0;

      m_pDevice->postEventFromUser(l_cEvent);
    }

    /** Inherited from irr::gui::IGUIElement **/
    bool CVirtualKeyboard::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (m_eMode == enMode::VirtualKeys) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            m_cMouse.X = a_cEvent.MouseInput.X;
            m_cMouse.Y = a_cEvent.MouseInput.Y;

            for (auto& l_eKey : m_vVirtualKeys) {
              l_eKey.m_bHover = (l_eKey.m_cRect + m_cVkOffset).isPointInside(m_cMouse);
              l_bRet |= l_eKey.m_bHover;
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_cMouse.X = a_cEvent.MouseInput.X;
            m_cMouse.Y = a_cEvent.MouseInput.Y;

            for (auto& l_eKey : m_vVirtualKeys) {
              if ((l_eKey.m_cRect + m_cVkOffset).isPointInside(m_cMouse)) {
                if (l_eKey.m_eKey == irr::KEY_RETURN) {
                  setVisible(false);
                }
                else if (l_eKey.m_eKey == irr::KEY_BACK) {
                  std::wstring l_sText = m_pTarget->getText();
                  m_pTarget->setText(l_sText.substr(0, l_sText.size() - 1).c_str());
                }
                else if (l_eKey.m_eKey == irr::KEY_SHIFT) {
                  m_bShift = !m_bShift;
                }
                else if (l_eKey.m_eKey == irr::KEY_SPACE) {
                  std::wstring l_sText = m_pTarget->getText();
                  m_pTarget->setText((l_sText + L" ").c_str());

                }
                else {
                  std::wstring l_sText = m_pTarget->getText();
                  std::wstring l_sChar = l_eKey.m_sChar;

                  if (m_bShift && l_eKey.m_bHasUpper) {
                    l_sChar = std::towupper(l_sChar[0]);
                  }

                  irr::SEvent l_cEvent{};
                  
                  l_cEvent.EventType            = irr::EET_KEY_INPUT_EVENT;
                  l_cEvent.KeyInput.Key         = l_eKey.m_eKey;
                  l_cEvent.KeyInput.Char        = l_sChar[0];
                  l_cEvent.KeyInput.Control     = false;
                  l_cEvent.KeyInput.Shift       = m_bShift;
                  l_cEvent.KeyInput.PressedDown = true;
                  
                  m_pTarget->OnEvent(l_cEvent);

                  l_cEvent.KeyInput.PressedDown = false;
                  m_pTarget->OnEvent(l_cEvent);

                  m_pTarget->setText((l_sText + l_sChar).c_str());
                  m_bShift = false;
                }

                l_bRet = true;
              }
            }
          }
        }
      }
      else {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            m_cMouse.X = a_cEvent.MouseInput.X;
            m_cMouse.Y = a_cEvent.MouseInput.Y;

            if (m_bMouseDown) {
              m_cOffset.Y -= m_cMDown.Y - m_cMouse.Y;
              m_cOffset.X += m_cMDown.X - m_cMouse.X;

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

              if (m_cOffset.X > m_cChar.getWidth()) {
                m_cBackground = irr::video::SColor(192, 255, 128, 128);
              }
              else if (m_cOffset.X < -m_cChar.getWidth()) {
                m_cBackground = irr::video::SColor(192, 128, 255, 128);
              }
              else {
                m_cBackground = irr::video::SColor(192, 255, 255, 192);
              }

              m_bMoved = true;
            }

            m_cMDown = m_cMouse;
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_cMouse = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            m_cMDown = m_cMouse;
            m_bMouseDown = true;
            m_bMoved     = false;
            m_cOffset    = irr::core::position2di();
            m_cTotalMove = irr::core::position2di();

            m_bMoving = m_cInner.isPointInside(m_cMouse);

            m_iMouseDown = m_pTimer->getTime();
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            if (m_pTimer->getTime() - m_iMouseDown < 125) {
              hideKeyboard();
            }
            else if (m_bMoving) {
              if (m_cOffset.X > m_cChar.getWidth()) {
                delChar();
              }
              else if (m_cOffset.X < -m_cChar.getWidth()) {
                addChar();
              }
              else if (m_cOffset.Y < -m_cChar.getHeight() / 2) {
                m_itKey++;
                if (m_itKey == m_vKeys.end())
                  m_itKey = m_vKeys.begin();
              }
              else if (m_cOffset.Y > m_cChar.getHeight() / 2) {
                if (m_itKey == m_vKeys.begin())
                  m_itKey = m_vKeys.end();
                m_itKey--;
              }
            }

            m_bMouseDown = false;
            m_iMouseDown = 0;
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
                hideKeyboard();
              }

              l_bRet = true;
            }
          }
        }
      }

      return l_bRet;
    }

    void CVirtualKeyboard::draw() {
      if (IsVisible) {
        if (m_eMode == enMode::VirtualKeys) {
          for (auto l_cKey : m_vVirtualKeys) {
            irr::core::recti l_cRect = l_cKey.m_cRect + m_cVkOffset;

            m_pDrv->draw2DRectangle(l_cKey.m_bHover ? irr::video::SColor(208, 208, 128, 32) : irr::video::SColor(208, 208, 208, 208), l_cRect);
            m_pDrv->draw2DRectangleOutline(l_cRect, irr::video::SColor(0xFF, 0, 0, 0));

            std::wstring l_sKey = l_cKey.m_sChar;

            if (m_bShift && l_cKey.m_bHasUpper) {
              l_sKey = std::towupper(l_sKey[0]);
            }

            m_pFont->draw(l_sKey.c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), true, true);
          }
        }
        else {
          m_pDrv->draw2DRectangleOutline(m_cChar, irr::video::SColor(0xFF, 0, 0, 0));
          m_pDrv->draw2DRectangleOutline(m_cUp  , irr::video::SColor(0xFF, 0, 0, 0));
          m_pDrv->draw2DRectangleOutline(m_cDown, irr::video::SColor(0xFF, 0, 0, 0));

          m_pDrv->draw2DRectangle(m_cBackground, m_cUp  );
          m_pDrv->draw2DRectangle(m_cBackground, m_cDown);

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

            if (it == m_vKeys.begin()) {
              it = m_vKeys.end();
            }

            it--;

            s[0] = *it;
            m_pFont->draw(s, r, irr::video::SColor(0xFF, 0, 0, 0), true, true, &AbsoluteClippingRect);
          }
        }
      }
    }

    void CVirtualKeyboard::setTarget(irr::gui::IGUIEditBox* a_pTarget) {
      m_pTarget = a_pTarget;

      if (m_pTarget != nullptr) {
        initUI();

        if (m_pDevice->getCursorControl() != nullptr) {
          m_pDevice->getCursorControl()->setPosition(AbsoluteClippingRect.getCenter());
        }
        else {
          irr::SEvent l_cEvent{};

          l_cEvent.EventType               = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event        = irr::EMIE_MOUSE_MOVED;
          l_cEvent.MouseInput.ButtonStates = 0;
          l_cEvent.MouseInput.Wheel        = 0.0f;
          l_cEvent.MouseInput.Control      = false;
          l_cEvent.MouseInput.Shift        = false;
          l_cEvent.MouseInput.X            = AbsoluteClippingRect.getCenter().X;
          l_cEvent.MouseInput.Y            = AbsoluteClippingRect.getCenter().Y;

          Parent->OnEvent(l_cEvent);
        }
      }
    }

    void CVirtualKeyboard::initUI() {
      setVisible(true);

      if (m_eMode == enMode::Scrolling) {
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

        irr::gui::ICursorControl *l_pCursor = m_pDevice->getCursorControl();
        if (l_pCursor != nullptr && m_bCtrlActive) {
          l_pCursor->setPosition(AbsoluteClippingRect.getCenter());
        }
        else {
          irr::SEvent l_cEvent{};
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
          l_cEvent.MouseInput.X = AbsoluteClippingRect.getCenter().X;
          l_cEvent.MouseInput.Y = AbsoluteClippingRect.getCenter().Y;
          m_pDevice->postEventFromUser(l_cEvent);
        }

        /*irr::SEvent l_cEvent{};

        l_cEvent.EventType = irr::EET_USER_EVENT;
        l_cEvent.UserEvent.UserData1 = c_iEventHideCursor;
        l_cEvent.UserEvent.UserData2 = 1;

        m_pDevice->postEventFromUser(l_cEvent);*/
      }
      else {
        m_pFont = CGlobal::getInstance()->getFont(enFont::Big, m_pDrv->getScreenSize());
        AbsoluteClippingRect = irr::core::recti(m_cVkOffset, m_cVkSize);
      }
    }

    CVirtualKeyboard::SVirtualKey::SVirtualKey(const irr::core::recti& a_cRect, irr::EKEY_CODE a_eKey, const std::wstring& a_sChar, bool a_bHasUpper) : m_cRect(a_cRect), m_eKey(a_eKey), m_sChar(a_sChar), m_bHover(false), m_bHasUpper(a_bHasUpper) {
    }
  }
}