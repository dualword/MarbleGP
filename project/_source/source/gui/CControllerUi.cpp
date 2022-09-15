// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerBase.h>
#include <gui/CDustbinScrollPane.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <gui/CControllerUi.h>
#include <menu/IMenuManager.h>
#include <irrlicht.h>
#include <Defines.h>
#include <algorithm>
#include <CGlobal.h>
#include <string>


namespace dustbin {
  namespace gui {
    irr::core::stringw keyCodeToString(irr::EKEY_CODE e) {
      if (e == irr::KEY_LBUTTON) return irr::core::stringw(L"KEY_LBUTTON");
      if (e == irr::KEY_RBUTTON) return irr::core::stringw(L"KEY_RBUTTON");
      if (e == irr::KEY_CANCEL) return irr::core::stringw(L"KEY_CANCEL");
      if (e == irr::KEY_MBUTTON) return irr::core::stringw(L"KEY_MBUTTON");
      if (e == irr::KEY_XBUTTON1) return irr::core::stringw(L"KEY_XBUTTON1");
      if (e == irr::KEY_XBUTTON2) return irr::core::stringw(L"KEY_XBUTTON2");
      if (e == irr::KEY_BACK) return irr::core::stringw(L"KEY_BACK");
      if (e == irr::KEY_TAB) return irr::core::stringw(L"KEY_TAB");
      if (e == irr::KEY_CLEAR) return irr::core::stringw(L"KEY_CLEAR");
      if (e == irr::KEY_RETURN) return irr::core::stringw(L"KEY_RETURN");
      if (e == irr::KEY_SHIFT) return irr::core::stringw(L"KEY_SHIFT");
      if (e == irr::KEY_CONTROL) return irr::core::stringw(L"KEY_CONTROL");
      if (e == irr::KEY_MENU) return irr::core::stringw(L"KEY_MENU");
      if (e == irr::KEY_PAUSE) return irr::core::stringw(L"KEY_PAUSE");
      if (e == irr::KEY_CAPITAL) return irr::core::stringw(L"KEY_CAPITAL");
      if (e == irr::KEY_KANA) return irr::core::stringw(L"KEY_KANA");
      if (e == irr::KEY_HANGUEL) return irr::core::stringw(L"KEY_HANGUEL");
      if (e == irr::KEY_HANGUL) return irr::core::stringw(L"KEY_HANGUL");
      if (e == irr::KEY_JUNJA) return irr::core::stringw(L"KEY_JUNJA");
      if (e == irr::KEY_FINAL) return irr::core::stringw(L"KEY_FINAL");
      if (e == irr::KEY_HANJA) return irr::core::stringw(L"KEY_HANJA");
      if (e == irr::KEY_KANJI) return irr::core::stringw(L"KEY_KANJI");
      if (e == irr::KEY_ESCAPE) return irr::core::stringw(L"KEY_ESCAPE");
      if (e == irr::KEY_CONVERT) return irr::core::stringw(L"KEY_CONVERT");
      if (e == irr::KEY_NONCONVERT) return irr::core::stringw(L"KEY_NONCONVERT");
      if (e == irr::KEY_ACCEPT) return irr::core::stringw(L"KEY_ACCEPT");
      if (e == irr::KEY_MODECHANGE) return irr::core::stringw(L"KEY_MODECHANGE");
      if (e == irr::KEY_SPACE) return irr::core::stringw(L"KEY_SPACE");
      if (e == irr::KEY_PRIOR) return irr::core::stringw(L"KEY_PRIOR");
      if (e == irr::KEY_NEXT) return irr::core::stringw(L"KEY_NEXT");
      if (e == irr::KEY_END) return irr::core::stringw(L"KEY_END");
      if (e == irr::KEY_HOME) return irr::core::stringw(L"KEY_HOME");
      if (e == irr::KEY_LEFT) return irr::core::stringw(L"KEY_LEFT");
      if (e == irr::KEY_UP) return irr::core::stringw(L"KEY_UP");
      if (e == irr::KEY_RIGHT) return irr::core::stringw(L"KEY_RIGHT");
      if (e == irr::KEY_DOWN) return irr::core::stringw(L"KEY_DOWN");
      if (e == irr::KEY_SELECT) return irr::core::stringw(L"KEY_SELECT");
      if (e == irr::KEY_PRINT) return irr::core::stringw(L"KEY_PRINT");
      if (e == irr::KEY_EXECUT) return irr::core::stringw(L"KEY_EXECUT");
      if (e == irr::KEY_SNAPSHOT) return irr::core::stringw(L"KEY_SNAPSHOT");
      if (e == irr::KEY_INSERT) return irr::core::stringw(L"KEY_INSERT");
      if (e == irr::KEY_DELETE) return irr::core::stringw(L"KEY_DELETE");
      if (e == irr::KEY_HELP) return irr::core::stringw(L"KEY_HELP");
      if (e == irr::KEY_KEY_0) return irr::core::stringw(L"KEY_KEY_0");
      if (e == irr::KEY_KEY_1) return irr::core::stringw(L"KEY_KEY_1");
      if (e == irr::KEY_KEY_2) return irr::core::stringw(L"KEY_KEY_2");
      if (e == irr::KEY_KEY_3) return irr::core::stringw(L"KEY_KEY_3");
      if (e == irr::KEY_KEY_4) return irr::core::stringw(L"KEY_KEY_4");
      if (e == irr::KEY_KEY_5) return irr::core::stringw(L"KEY_KEY_5");
      if (e == irr::KEY_KEY_6) return irr::core::stringw(L"KEY_KEY_6");
      if (e == irr::KEY_KEY_7) return irr::core::stringw(L"KEY_KEY_7");
      if (e == irr::KEY_KEY_8) return irr::core::stringw(L"KEY_KEY_8");
      if (e == irr::KEY_KEY_9) return irr::core::stringw(L"KEY_KEY_9");
      if (e == irr::KEY_KEY_A) return irr::core::stringw(L"KEY_KEY_A");
      if (e == irr::KEY_KEY_B) return irr::core::stringw(L"KEY_KEY_B");
      if (e == irr::KEY_KEY_C) return irr::core::stringw(L"KEY_KEY_C");
      if (e == irr::KEY_KEY_D) return irr::core::stringw(L"KEY_KEY_D");
      if (e == irr::KEY_KEY_E) return irr::core::stringw(L"KEY_KEY_E");
      if (e == irr::KEY_KEY_F) return irr::core::stringw(L"KEY_KEY_F");
      if (e == irr::KEY_KEY_G) return irr::core::stringw(L"KEY_KEY_G");
      if (e == irr::KEY_KEY_H) return irr::core::stringw(L"KEY_KEY_H");
      if (e == irr::KEY_KEY_I) return irr::core::stringw(L"KEY_KEY_I");
      if (e == irr::KEY_KEY_J) return irr::core::stringw(L"KEY_KEY_J");
      if (e == irr::KEY_KEY_K) return irr::core::stringw(L"KEY_KEY_K");
      if (e == irr::KEY_KEY_L) return irr::core::stringw(L"KEY_KEY_L");
      if (e == irr::KEY_KEY_M) return irr::core::stringw(L"KEY_KEY_M");
      if (e == irr::KEY_KEY_N) return irr::core::stringw(L"KEY_KEY_N");
      if (e == irr::KEY_KEY_O) return irr::core::stringw(L"KEY_KEY_O");
      if (e == irr::KEY_KEY_P) return irr::core::stringw(L"KEY_KEY_P");
      if (e == irr::KEY_KEY_Q) return irr::core::stringw(L"KEY_KEY_Q");
      if (e == irr::KEY_KEY_R) return irr::core::stringw(L"KEY_KEY_R");
      if (e == irr::KEY_KEY_S) return irr::core::stringw(L"KEY_KEY_S");
      if (e == irr::KEY_KEY_T) return irr::core::stringw(L"KEY_KEY_T");
      if (e == irr::KEY_KEY_U) return irr::core::stringw(L"KEY_KEY_U");
      if (e == irr::KEY_KEY_V) return irr::core::stringw(L"KEY_KEY_V");
      if (e == irr::KEY_KEY_W) return irr::core::stringw(L"KEY_KEY_W");
      if (e == irr::KEY_KEY_X) return irr::core::stringw(L"KEY_KEY_X");
      if (e == irr::KEY_KEY_Y) return irr::core::stringw(L"KEY_KEY_Y");
      if (e == irr::KEY_KEY_Z) return irr::core::stringw(L"KEY_KEY_Z");
      if (e == irr::KEY_LWIN) return irr::core::stringw(L"KEY_LWIN");
      if (e == irr::KEY_RWIN) return irr::core::stringw(L"KEY_RWIN");
      if (e == irr::KEY_APPS) return irr::core::stringw(L"KEY_APPS");
      if (e == irr::KEY_SLEEP) return irr::core::stringw(L"KEY_SLEEP");
      if (e == irr::KEY_NUMPAD0) return irr::core::stringw(L"KEY_NUMPAD0");
      if (e == irr::KEY_NUMPAD1) return irr::core::stringw(L"KEY_NUMPAD1");
      if (e == irr::KEY_NUMPAD2) return irr::core::stringw(L"KEY_NUMPAD2");
      if (e == irr::KEY_NUMPAD3) return irr::core::stringw(L"KEY_NUMPAD3");
      if (e == irr::KEY_NUMPAD4) return irr::core::stringw(L"KEY_NUMPAD4");
      if (e == irr::KEY_NUMPAD5) return irr::core::stringw(L"KEY_NUMPAD5");
      if (e == irr::KEY_NUMPAD6) return irr::core::stringw(L"KEY_NUMPAD6");
      if (e == irr::KEY_NUMPAD7) return irr::core::stringw(L"KEY_NUMPAD7");
      if (e == irr::KEY_NUMPAD8) return irr::core::stringw(L"KEY_NUMPAD8");
      if (e == irr::KEY_NUMPAD9) return irr::core::stringw(L"KEY_NUMPAD9");
      if (e == irr::KEY_MULTIPLY) return irr::core::stringw(L"KEY_MULTIPLY");
      if (e == irr::KEY_ADD) return irr::core::stringw(L"KEY_ADD");
      if (e == irr::KEY_SEPARATOR) return irr::core::stringw(L"KEY_SEPARATOR");
      if (e == irr::KEY_SUBTRACT) return irr::core::stringw(L"KEY_SUBTRACT");
      if (e == irr::KEY_DECIMAL) return irr::core::stringw(L"KEY_DECIMAL");
      if (e == irr::KEY_DIVIDE) return irr::core::stringw(L"KEY_DIVIDE");
      if (e == irr::KEY_F1) return irr::core::stringw(L"KEY_F1");
      if (e == irr::KEY_F2) return irr::core::stringw(L"KEY_F2");
      if (e == irr::KEY_F3) return irr::core::stringw(L"KEY_F3");
      if (e == irr::KEY_F4) return irr::core::stringw(L"KEY_F4");
      if (e == irr::KEY_F5) return irr::core::stringw(L"KEY_F5");
      if (e == irr::KEY_F6) return irr::core::stringw(L"KEY_F6");
      if (e == irr::KEY_F7) return irr::core::stringw(L"KEY_F7");
      if (e == irr::KEY_F8) return irr::core::stringw(L"KEY_F8");
      if (e == irr::KEY_F9) return irr::core::stringw(L"KEY_F9");
      if (e == irr::KEY_F10) return irr::core::stringw(L"KEY_F10");
      if (e == irr::KEY_F11) return irr::core::stringw(L"KEY_F11");
      if (e == irr::KEY_F12) return irr::core::stringw(L"KEY_F12");
      if (e == irr::KEY_F13) return irr::core::stringw(L"KEY_F13");
      if (e == irr::KEY_F14) return irr::core::stringw(L"KEY_F14");
      if (e == irr::KEY_F15) return irr::core::stringw(L"KEY_F15");
      if (e == irr::KEY_F16) return irr::core::stringw(L"KEY_F16");
      if (e == irr::KEY_F17) return irr::core::stringw(L"KEY_F17");
      if (e == irr::KEY_F18) return irr::core::stringw(L"KEY_F18");
      if (e == irr::KEY_F19) return irr::core::stringw(L"KEY_F19");
      if (e == irr::KEY_F20) return irr::core::stringw(L"KEY_F20");
      if (e == irr::KEY_F21) return irr::core::stringw(L"KEY_F21");
      if (e == irr::KEY_F22) return irr::core::stringw(L"KEY_F22");
      if (e == irr::KEY_F23) return irr::core::stringw(L"KEY_F23");
      if (e == irr::KEY_F24) return irr::core::stringw(L"KEY_F24");
      if (e == irr::KEY_NUMLOCK) return irr::core::stringw(L"KEY_NUMLOCK");
      if (e == irr::KEY_SCROLL) return irr::core::stringw(L"KEY_SCROLL");
      if (e == irr::KEY_LSHIFT) return irr::core::stringw(L"KEY_LSHIFT");
      if (e == irr::KEY_RSHIFT) return irr::core::stringw(L"KEY_RSHIFT");
      if (e == irr::KEY_LCONTROL) return irr::core::stringw(L"KEY_LCONTROL");
      if (e == irr::KEY_RCONTROL) return irr::core::stringw(L"KEY_RCONTROL");
      if (e == irr::KEY_LMENU) return irr::core::stringw(L"KEY_LMENU");
      if (e == irr::KEY_RMENU) return irr::core::stringw(L"KEY_RMENU");
      if (e == irr::KEY_PLUS) return irr::core::stringw(L"KEY_PLUS");
      if (e == irr::KEY_COMMA) return irr::core::stringw(L"KEY_COMMA");
      if (e == irr::KEY_MINUS) return irr::core::stringw(L"KEY_MINUS");
      if (e == irr::KEY_PERIOD) return irr::core::stringw(L"KEY_PERIOD");
      if (e == irr::KEY_ATTN) return irr::core::stringw(L"KEY_ATTN");
      if (e == irr::KEY_CRSEL) return irr::core::stringw(L"KEY_CRSEL");
      if (e == irr::KEY_EXSEL) return irr::core::stringw(L"KEY_EXSEL");
      if (e == irr::KEY_EREOF) return irr::core::stringw(L"KEY_EREOF");
      if (e == irr::KEY_PLAY) return irr::core::stringw(L"KEY_PLAY");
      if (e == irr::KEY_ZOOM) return irr::core::stringw(L"KEY_ZOOM");
      if (e == irr::KEY_PA1) return irr::core::stringw(L"KEY_PA1");
      if (e == irr::KEY_OEM_CLEAR) return irr::core::stringw(L"KEY_OEM_CLEAR");
      return irr::core::stringw(L"UNKNOWN");
    }

    CControllerUi::CControllerUi(irr::gui::IGUIElement* a_pParent) :
      CMenuBackground(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId),
      m_bMouseDown   (false),
      m_bSelected    (false),
      m_pParent      (a_pParent),
      m_sConfigData  (""),
      m_sHeadline    ("Controller Setup"),
      m_sReturn      (L""),
      m_pFont        (nullptr),
      m_pDrv         (CGlobal::getInstance()->getVideoDriver()),
      m_pGlobal      (CGlobal::getInstance()),
      m_pMenuManager (nullptr),
      m_pScrollPane  (nullptr),
      m_pCursor      (CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl())
    {
      m_pFont   = m_pGlobal->getFont(enFont::Regular, m_pDrv->getScreenSize());
      m_itHover = m_vItems.end();
      m_itClick = m_vItems.end();
      m_itSelct = m_vItems.end();
    }

    CControllerUi::~CControllerUi() {
      if (m_pScrollPane != nullptr) {
        delete m_pScrollPane;
        m_pScrollPane = nullptr;
      }
    }

    /**
    * This method builds the UI for the controller
    * @param a_pParent the parent element.
    */
    void CControllerUi::buildUi(irr::gui::IGUIElement* a_pParent) {
      m_cItemRect = AbsoluteClippingRect;

      m_cItemRect.UpperLeftCorner  += irr::core::vector2di(m_pGlobal->getRasterSize() / 2);
      m_cItemRect.LowerRightCorner -= irr::core::vector2di(m_pGlobal->getRasterSize() / 2);

      if (m_pScrollPane != nullptr)
        delete m_pScrollPane;

      m_pScrollPane = new CDustbinScrollPane(m_cItemRect);

      irr::core::dimension2du l_cFont = m_pFont->getDimension(L"ThisShouldBeEnoughForShowingAllControls");
      irr::core::dimension2du l_cItem;

      for (std::vector<controller::CControllerBase::SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        irr::core::dimension2du l_cDim = m_pFont->getDimension(helpers::s2ws((*it).m_sName).c_str());

        l_cItem.Width  = std::max(l_cItem.Width , l_cDim.Width );
        l_cItem.Height = std::max(l_cItem.Height, l_cDim.Height);
      }

      l_cItem.Width  = 5 * l_cItem.Width  / 4;
      l_cItem.Height = 5 * l_cItem.Height / 4;

      l_cFont.Width  = 5 * l_cFont.Width  / 4;
      l_cFont.Height = 5 * l_cFont.Height / 4;

      irr::core::position2di l_cPosL = irr::core::position2di(l_cFont.Height / 2, l_cFont.Height / 2);
      irr::core::position2di l_cPosI = l_cPosL + irr::core::position2di(l_cItem.Width + l_cFont.Height / 4, 0);

      for (std::vector<controller::CControllerBase::SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
        irr::core::recti r1 = irr::core::recti(l_cPosL, l_cItem);
        irr::core::recti r2 = irr::core::recti(l_cPosI, l_cFont);

        std::wstring l_sCtrl = L"";

        if ((*it).m_eType == controller::CControllerBase::enInputType::Key) {
          l_sCtrl = keyCodeToString((*it).m_eKey).c_str();
        }
        else if ((*it).m_eType == controller::CControllerBase::enInputType::JoyButton) {
          std::string s = (*it).m_sJoystick + " Button " + std::to_string((*it).m_iButton);
          l_sCtrl = helpers::s2ws(s).c_str();
        }
        else if ((*it).m_eType == controller::CControllerBase::enInputType::JoyPov) {
          std::string l_sPov = "";

          if ((*it).m_iPov == 0)
            l_sPov = "Up";
          else if ((*it).m_iPov == 9000)
            l_sPov = "Right";
          else if ((*it).m_iPov == 18000)
            l_sPov = "Down";
          else if ((*it).m_iPov == 27000) 
            l_sPov = "Left";

          std::string s = (*it).m_sJoystick + " POV " + l_sPov;
          l_sCtrl = helpers::s2ws(s).c_str();
        }
        else if ((*it).m_eType == controller::CControllerBase::enInputType::JoyAxis) {
          std::string s = (*it).m_sJoystick + " Axis " + std::to_string((*it).m_iAxis) + ((*it).m_iDirection > 0 ? " +" : " -");
          l_sCtrl = helpers::s2ws(s).c_str();
        }

        m_vItems.push_back(SCtrlUi(&(*it), helpers::s2ws((*it).m_sName), (std::wstring(L" ") + l_sCtrl).c_str(), r1, r2));

        m_pScrollPane->addRectangle(r1);
        m_pScrollPane->addRectangle(r2);

        l_cPosL += irr::core::position2di(0, 4 * l_cFont.Height / 3);
        l_cPosI += irr::core::position2di(0, 4 * l_cFont.Height / 3);
      }

      m_itHover = m_vItems.end();
      m_itClick = m_vItems.end();
      m_itSelct = m_vItems.end();
    }

    bool CControllerUi::isEditing() {
      return false;
    }

    void CControllerUi::setMenuManager(menu::IMenuManager* a_pMenuManager) {
      m_pMenuManager = a_pMenuManager;
    }

    bool CControllerUi::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
          if (a_cEvent.UserEvent.UserData2 != 0) {
            if (!m_bSelected) {
              m_bSelected = true;

              if (m_itHover == m_vItems.end())
                m_itHover = m_vItems.begin();

              m_itSelct = m_vItems.end();

              if (Parent != nullptr) {
                irr::SEvent l_cEvent{};
                l_cEvent.EventType = irr::EET_USER_EVENT;
                l_cEvent.UserEvent.UserData1 = c_iEventHideCursor;
                l_cEvent.UserEvent.UserData2 = 1;
                Parent->OnEvent(l_cEvent);

                irr::core::position2di l_cScroll = m_pScrollPane != nullptr ? m_pScrollPane->getScrollPosition() : irr::core::position2di();
                irr::core::recti l_cRect = (*m_itHover).m_cRectLabel + AbsoluteClippingRect.UpperLeftCorner + l_cScroll;

                l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
                l_cEvent.MouseInput.Event        = irr::EMIE_MOUSE_MOVED;
                l_cEvent.MouseInput.X            = l_cRect.getCenter().X;
                l_cEvent.MouseInput.Y            = l_cRect.getCenter().Y;
                l_cEvent.MouseInput.ButtonStates = 0;
                l_cEvent.MouseInput.Shift        = false;
                l_cEvent.MouseInput.Control      = false;
                l_cEvent.MouseInput.Wheel        = 0.0f;
                Parent->OnEvent(l_cEvent);

                if (m_pCursor != nullptr)
                  m_pCursor->setPosition(l_cRect.getCenter());
              }

              l_bRet = true;
            }
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMoveMouse) {
          if (m_bSelected) {
            if (a_cEvent.UserEvent.UserData2 == 1) {
              if (m_itSelct == m_vItems.end()) {
                if (m_itHover + 1 != m_vItems.end()) {
                  m_itHover++;

                  irr::SEvent l_cEvent{};

                  irr::core::position2di l_cScroll = m_pScrollPane != nullptr ? m_pScrollPane->getScrollPosition() : irr::core::position2di();
                  irr::core::recti l_cRect = (*m_itHover).m_cRectLabel + AbsoluteClippingRect.UpperLeftCorner + l_cScroll;

                  l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
                  l_cEvent.MouseInput.Event        = irr::EMIE_MOUSE_MOVED;
                  l_cEvent.MouseInput.X            = l_cRect.getCenter().X;
                  l_cEvent.MouseInput.Y            = l_cRect.getCenter().Y;
                  l_cEvent.MouseInput.ButtonStates = 0;
                  l_cEvent.MouseInput.Shift        = false;
                  l_cEvent.MouseInput.Control      = false;
                  l_cEvent.MouseInput.Wheel        = 0.0f;
                  Parent->OnEvent(l_cEvent);

                  if (m_pCursor != nullptr) {
                    m_pCursor->setPosition(l_cRect.getCenter());
                  }
                }
              }
            }
            else if (a_cEvent.UserEvent.UserData2 == 0) {
              if (m_itSelct == m_vItems.end()) {
                if (m_itHover != m_vItems.begin()) {
                  m_itHover--;

                  irr::SEvent l_cEvent{};

                  irr::core::position2di l_cScroll = m_pScrollPane != nullptr ? m_pScrollPane->getScrollPosition() : irr::core::position2di();
                  irr::core::recti l_cRect = (*m_itHover).m_cRectLabel + AbsoluteClippingRect.UpperLeftCorner + l_cScroll;

                  l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
                  l_cEvent.MouseInput.Event        = irr::EMIE_MOUSE_MOVED;
                  l_cEvent.MouseInput.X            = l_cRect.getCenter().X;
                  l_cEvent.MouseInput.Y            = l_cRect.getCenter().Y;
                  l_cEvent.MouseInput.ButtonStates = 0;
                  l_cEvent.MouseInput.Shift        = false;
                  l_cEvent.MouseInput.Control      = false;
                  l_cEvent.MouseInput.Wheel        = 0.0f;
                  Parent->OnEvent(l_cEvent);

                  if (m_pCursor != nullptr) {
                    m_pCursor->setPosition(l_cRect.getCenter());
                  }
                }
              }
            }

            l_bRet = true;
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventOkClicked) {
          if (m_bSelected) {
            m_bSelected = false;
            if (Parent != nullptr) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventHideCursor;
              l_cEvent.UserEvent.UserData2 = 0;
              Parent->OnEvent(l_cEvent);
            }

            l_bRet = true;
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventCancelClicked) {
          // Consume cancel event if we are selected via menu controller
          if (m_bSelected)
            l_bRet = true;
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          if (m_pCursor == nullptr) {
            m_cMousePos.X = a_cEvent.MouseInput.X;
            m_cMousePos.Y = a_cEvent.MouseInput.Y;

            l_bRet = true;
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bMouseDown = true;
          m_itClick = m_itHover;

          l_bRet = true;
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseDown = false;
          m_itSelct    = m_itClick;
          m_itClick    = m_vItems.end();

          m_vJoyStates.clear();

          for (unsigned i = 0; i < m_pGlobal->getJoystickInfo().size(); i++) {
            SJoystickState l_cState;
            l_cState.m_iIndex       = i;
            l_cState.m_sName        = m_pGlobal->getJoystickInfo()[i].Name.c_str();
            l_cState.m_iAxes        = m_pGlobal->getJoystickInfo()[i].Axes;
            l_cState.m_iButtons     = m_pGlobal->getJoystickInfo()[i].Buttons;
            l_cState.m_bHasPov      = m_pGlobal->getJoystickInfo()[i].PovHat == irr::SJoystickInfo::POV_HAT_PRESENT;
            l_cState.m_bInitialized = false;

            for (int i = 0; i < l_cState.m_iAxes; i++) {
              l_cState.m_vAxes.push_back(0.0);
            }

            m_vJoyStates.push_back(l_cState);
          }

          l_bRet = true;
        }
      }
      if (!l_bRet)
        Parent->OnEvent(a_cEvent);

      return l_bRet;
    }

    // This method is necessary because UI elements don't receive Joystick events
    bool CControllerUi::update(const irr::SEvent& a_cEvent) {
      CControllerBase::update(a_cEvent);

      if (m_pScrollPane != nullptr && m_pScrollPane->OnEvent(a_cEvent))
        return true;
      
      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT)
        if (OnJoystickEvent(a_cEvent))
          return true;
      

      if (m_itSelct != m_vItems.end()) {
        if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
          if (!a_cEvent.KeyInput.PressedDown) {
            (*m_itSelct).m_pInput->m_eType = controller::CControllerBase::enInputType::Key;
            (*m_itSelct).m_pInput->m_eKey  = a_cEvent.KeyInput.Key;
            (*m_itSelct).m_sValue = keyCodeToString(a_cEvent.KeyInput.Key).c_str();

            m_itSelct = m_vItems.end();
            return true;
          }
        }
      }

      return false;
    }

    /**
    * Get the configuration of the controller as serialized string
    * @return the configuration of the controller
    */
    std::string CControllerUi::getControllerString() {
      return "";
    }

    irr::gui::EGUI_ELEMENT_TYPE CControllerUi::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId;
    }

    /**
    * The "setText" method is abused to pass the configuration serialized string to the UI
    * @param a_pText the new serialized string
    */
    void CControllerUi::setText(const wchar_t* a_pText) {
      irr::gui::IGUIElement::setText(a_pText);

      std::wstring ws = a_pText;
      std::string s = helpers::ws2s(ws);

      if (s != m_sConfigData) {

        m_sConfigData = s;

        deserialize(m_sConfigData);
        buildUi(m_pParent);
      }
    }

    bool CControllerUi::OnJoystickEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
        for (std::vector<SJoystickState>::iterator it = m_vJoyStates.begin(); it != m_vJoyStates.end(); it++) {
          if (!(*it).m_bInitialized && (*it).m_iIndex == a_cEvent.JoystickEvent.Joystick) {
            for (int i = 0; i < (*it).m_iAxes; i++) {
              (*it).m_vAxes[i] = ((float)a_cEvent.JoystickEvent.Axis[i]) / 32500.0f;
              if ((*it).m_vAxes[i] > 1.0f) (*it).m_vAxes[i] = 1.0f;
              if ((*it).m_vAxes[i] < -1.0f) (*it).m_vAxes[i] = -1.0f;
            }
            (*it).m_bInitialized = true;
          }
        }

        if (m_itSelct != m_vItems.end()) {
          for (std::vector<SJoystickState>::iterator it = m_vJoyStates.begin(); it != m_vJoyStates.end(); it++) {
            if ((*it).m_bInitialized && (*it).m_iIndex == a_cEvent.JoystickEvent.Joystick) {
              for (int i = 0; i < (*it).m_iButtons; i++) {
                if (a_cEvent.JoystickEvent.IsButtonPressed(i)) {
                  (*m_itSelct).m_pInput->m_eType     = controller::CControllerBase::enInputType::JoyButton;
                  (*m_itSelct).m_pInput->m_iButton   = i;
                  (*m_itSelct).m_pInput->m_iJoystick = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelct).m_pInput->m_sJoystick = (*it).m_sName;

                  std::string s = (*it).m_sName + " Button " + std::to_string(i);
                  (*m_itSelct).m_sValue = helpers::s2ws(s).c_str();

                  m_itSelct = m_vItems.end();
                  return true;
                }
              }

              if ((*it).m_bHasPov) {
                irr::u16 l_iPov = a_cEvent.JoystickEvent.POV;

                if (l_iPov == 0 || l_iPov == 9000 || l_iPov == 18000 || l_iPov == 27000) {
                  (*m_itSelct).m_pInput->m_eType     = controller::CControllerBase::enInputType::JoyPov;
                  (*m_itSelct).m_pInput->m_iJoystick = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelct).m_pInput->m_sJoystick = (*it).m_sName;
                  (*m_itSelct).m_pInput->m_iPov      = l_iPov;

                  std::string l_sPov = "";

                  if (l_iPov == 0)
                    l_sPov = "Up";
                  else if (l_iPov == 9000)
                    l_sPov = "Right";
                  else if (l_iPov == 18000)
                    l_sPov = "Down";
                  else if (l_iPov == 27000)
                    l_sPov = "Left";

                  std::string s = (*it).m_sName + " POV " + l_sPov;
                  (*m_itSelct).m_sValue = helpers::s2ws(s).c_str();
                  m_itSelct = m_vItems.end();

                  return true;
                }
              }

              for (int i = 0; i < (*it).m_iAxes; i++) {
                float f = (((irr::f32)a_cEvent.JoystickEvent.Axis[i]) / 32500.0f) - (*it).m_vAxes[i];
                
                if (abs(f) > 0.5f) {
                  (*m_itSelct).m_pInput->m_eType      = controller::CControllerBase::enInputType::JoyAxis;
                  (*m_itSelct).m_pInput->m_iJoystick  = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelct).m_pInput->m_sJoystick  = (*it).m_sName;
                  (*m_itSelct).m_pInput->m_iAxis      = i;
                  (*m_itSelct).m_pInput->m_iDirection = f > 0.0f ? 1 : -1;

                  std::string s = (*it).m_sName + " Axis " + std::to_string(i) + ((*m_itSelct).m_pInput->m_iDirection > 0 ? " +" : " -");
                  (*m_itSelct).m_sValue = helpers::s2ws(s).c_str();
                  m_itSelct = m_vItems.end();

                  return true;
                }
              }
            }
          }
        }
      }
      return false;
    }

    /**
    * Change the font for the configuration dialog
    */
    void CControllerUi::setFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
    }

    void CControllerUi::updateConfigData() {
      m_sConfigData = serialize();
      irr::gui::IGUIElement::setText(helpers::s2ws(m_sConfigData).c_str());
    }

    void CControllerUi::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      gui::CMenuBackground::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addString("headline", m_sHeadline.c_str());
    }

    void CControllerUi::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      gui::CMenuBackground::deserializeAttributes(a_pIn, a_pOptions);

      m_sHeadline = a_pIn->getAttributeAsString("headline").c_str();
    }

    void CControllerUi::draw() {
      CMenuBackground::draw();

      if (m_pCursor != nullptr)
        m_cMousePos = irr::core::position2di(m_pCursor->getPosition());

      m_pDrv->draw2DRectangleOutline(m_cItemRect, irr::video::SColor(0xFF, 0, 0, 0));

      irr::core::position2di l_cScroll = m_pScrollPane != nullptr ? m_pScrollPane->getScrollPosition() : irr::core::position2di();

      std::vector<SCtrlUi>::iterator l_itHover = m_vItems.end();

      for (std::vector<SCtrlUi>::iterator it = m_vItems.begin(); it != m_vItems.end(); it++) {
        irr::core::recti l_cClip1 = (*it).m_cRectLabel + AbsoluteClippingRect.UpperLeftCorner + l_cScroll;
        irr::core::recti l_cClip2 = (*it).m_cRectItem  + AbsoluteClippingRect.UpperLeftCorner + l_cScroll;

        bool l_bHover = l_cClip1.isPointInside(m_cMousePos) || l_cClip2.isPointInside(m_cMousePos);

        if (l_bHover) {
          if (m_itHover != it)
            m_itClick = m_vItems.end();

          l_itHover = it;
        }

        m_pDrv->draw2DRectangle(m_itSelct == it ? irr::video::SColor(224, 192, 192, 255) : m_itHover == it ? m_itClick == it ? irr::video::SColor(224, 255, 255, 192) : irr::video::SColor(224, 255, 192, 192) : irr::video::SColor(224, 192, 192, 192), l_cClip1, &m_cItemRect);
        m_pDrv->draw2DRectangle(m_itSelct == it ? irr::video::SColor(224, 192, 192, 255) : m_itHover == it ? m_itClick == it ? irr::video::SColor(224, 255, 255, 192) : irr::video::SColor(224, 255, 192, 192) : irr::video::SColor(224, 192, 192, 192), l_cClip2, &m_cItemRect);

        m_pFont->draw((*it).m_sName .c_str(), l_cClip1, irr::video::SColor(0xFF, 0, 0, 0), true , true, &m_cItemRect);
        m_pFont->draw((*it).m_sValue.c_str(), l_cClip2, irr::video::SColor(0xFF, 0, 0, 0), false, true, &m_cItemRect);
      }

      m_itHover = l_itHover;
    }
  } // namespace controller 
} // namespace dustbin