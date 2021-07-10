// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <platform/CPlatform.h>
#include <gui/CControllerUi.h>
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
      m_sConfigXml(""),
      m_pParent(a_pParent),
      m_sHeadline("Controller Setup"),
      m_pFont(nullptr),
      m_pCursor(CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()),
      m_pSelectedOne(nullptr),
      m_pSelectedTwo(nullptr),
      m_pHoveredOne(nullptr),
      m_pHoveredTwo(nullptr),
      m_bMouseDown(false)
    {
      CGlobal::getInstance()->getActiveState()->registerJoystickHandler(this);
    }

    CControllerUi::~CControllerUi() {
      CGlobal::getInstance()->getActiveState()->unregisterJoystickHandler(this);
    }

    /**
    * This method builds the UI for the controller
    * @param a_pParent the parent element.
    */
    void CControllerUi::buildUi(irr::gui::IGUIElement* a_pParent) {
      if (m_mElements.size() == 0) {
        CGlobal* l_pGlobal = CGlobal::getInstance();

        if (m_pFont == nullptr)
          m_pFont = l_pGlobal->getGuiEnvironment()->getSkin()->getFont();

        irr::u32 l_iHeight = getAbsoluteClippingRect().getHeight(),
                 l_iWidth  = getAbsoluteClippingRect().getWidth(),
                 l_iCount  = (irr::u32)m_vControls.size() + 1;

        irr::core::dimension2du l_cDim = irr::core::dimension2du(0, 0);

        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
          std::wstring s = platform::s2ws((*it).m_sName);
          irr::core::dimension2du d = m_pFont->getDimension(s.c_str());

          if (d.Width  > l_cDim.Width ) l_cDim.Width  = d.Width;
          if (d.Height > l_cDim.Height) l_cDim.Height = d.Height;
        }

        l_cDim.Height = 3 * l_cDim.Height / 2;
        l_cDim.Width  = 3 * l_cDim.Width  / 2;

        irr::core::position2di l_cPos = irr::core::position2di(0, l_cDim.Height / 8);

        irr::gui::IGUIStaticText* p = l_pGlobal->getGuiEnvironment()->addStaticText(platform::s2ws(m_sHeadline).c_str(), irr::core::recti(l_cPos, irr::core::dimension2du(l_iWidth, l_cDim.Height)), false, true, this);
        p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
        p->setOverrideFont(m_pFont);

        l_cPos.X = l_cDim.Height;

        irr::core::dimension2du l_cDim2 = l_cDim;
        l_cDim2.Width = l_iWidth - 3 * l_cDim.Height - l_cDim.Width;

        irr::core::position2di l_cPos2 = l_cPos;
        l_cPos2.X += l_cDim.Height / 2 + l_cDim.Width;

        int l_iLine = 3 * l_cDim.Height / 2;
        
        for (std::vector<controller::CControllerBase::SCtrlInput>::iterator it = m_vControls.begin(); it != m_vControls.end(); it++) {
          l_cPos .Y += l_iLine;
          l_cPos2.Y += l_iLine;

          irr::gui::IGUIStaticText* p1 = l_pGlobal->getGuiEnvironment()->addStaticText(platform::s2ws((*it).m_sName).c_str(), irr::core::recti(l_cPos, l_cDim), true, true, this);
          p1->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
          p1->setOverrideFont(m_pFont);

          irr::gui::IGUIStaticText* p2 = l_pGlobal->getGuiEnvironment()->addStaticText(L"Controller", irr::core::recti(l_cPos2, l_cDim2), true, true, this);
          p2->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
          p2->setOverrideFont(m_pFont);

          if ((*it).m_eType == controller::CControllerBase::enInputType::Key) {
            p2->setText(keyCodeToString((*it).m_eKey).c_str());
          }
          else if ((*it).m_eType == controller::CControllerBase::enInputType::JoyButton) {
            std::string s = (*it).m_sJoystick + " Button " + std::to_string((*it).m_iButton);
            p2->setText(platform::s2ws(s).c_str());
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
            p2->setText(platform::s2ws(s).c_str());
          }
          else if ((*it).m_eType == controller::CControllerBase::enInputType::JoyAxis) {
            std::string s = (*it).m_sJoystick + "Axis " + std::to_string((*it).m_iAxis) + ((*it).m_iDirection > 0 ? " +" : " -");
            p2->setText(platform::s2ws(s).c_str());
          }

          m_mTextPairs[p1] = p2;
          m_mTextPairs[p2] = p1;

          m_mTextControls[p1] = it;
          m_mTextControls[p2] = it;

          m_mControlText[it] = p2;
        }
      }
    }

    void CControllerUi::resetSelected() {
      if (m_pSelectedOne != nullptr) {
        if (m_pSelectedOne->getType() == irr::gui::EGUIET_STATIC_TEXT)
          reinterpret_cast<irr::gui::IGUIStaticText*>(m_pSelectedOne)->setDrawBackground(false);

        m_pSelectedOne = nullptr;
      }

      if (m_pSelectedTwo != nullptr) {
        if (m_pSelectedTwo->getType() == irr::gui::EGUIET_STATIC_TEXT)
          reinterpret_cast<irr::gui::IGUIStaticText*>(m_pSelectedTwo)->setDrawBackground(false);

        m_pSelectedTwo = nullptr;
      }

      for (std::map<irr::gui::IGUIElement*, irr::gui::IGUIElement*>::iterator it = m_mTextPairs.begin(); it != m_mTextPairs.end(); it++) {
        if (it->first->getAbsoluteClippingRect().isPointInside(m_pCursor->getPosition())) {
          elementEvent(it->first, true);
          break;
        }
      }
      CGlobal::getInstance()->getActiveState()->enableDefault(true);
      updateConfigXml();
    }

    void CControllerUi::elementEvent(irr::gui::IGUIElement* a_pElement, bool a_bEnter) {
      if (m_mTextPairs.find(a_pElement) != m_mTextPairs.end()) {
        irr::gui::IGUIElement* l_pOther = m_mTextPairs[a_pElement];

        if (a_pElement != m_pSelectedOne && a_pElement != m_pSelectedTwo && l_pOther != m_pSelectedOne && l_pOther != m_pSelectedTwo && m_mTextControls.find(a_pElement) != m_mTextControls.end()) {
          if (a_pElement->getType() == irr::gui::EGUIET_STATIC_TEXT && l_pOther->getType() == irr::gui::EGUIET_STATIC_TEXT) {
            irr::gui::IGUIStaticText *p1 = reinterpret_cast<irr::gui::IGUIStaticText*>(a_pElement),
                                     *p2 = reinterpret_cast<irr::gui::IGUIStaticText*>(l_pOther);

            p1->setBackgroundColor(irr::video::SColor(192, 128, 128, 255));
            p1->setDrawBackground(a_bEnter);

            p2->setBackgroundColor(irr::video::SColor(192, 128, 128, 255));
            p2->setDrawBackground(a_bEnter);

            if (a_bEnter) {
              m_pHoveredOne = a_pElement;
              m_pHoveredTwo = l_pOther;

              m_itHovered = m_mTextControls[a_pElement];
            }
            else {
              m_pHoveredOne = nullptr;
              m_pHoveredTwo = nullptr;
            }
          }
        }
      }
    }

    bool CControllerUi::OnEvent(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          elementEvent(a_cEvent.GUIEvent.Caller, true);
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          elementEvent(a_cEvent.GUIEvent.Caller, false);
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.isLeftPressed()) {
          m_bMouseDown = true;
        }
        else if (m_bMouseDown) {
          m_bMouseDown = false;

          if (m_pHoveredOne != nullptr && m_pHoveredTwo != nullptr) {
            resetSelected();

            m_pSelectedOne = m_pHoveredOne;
            m_pSelectedTwo = m_pHoveredTwo;

            m_itSelected = m_itHovered;

            reinterpret_cast<irr::gui::IGUIStaticText*>(m_pSelectedOne)->setBackgroundColor(irr::video::SColor(192, 255, 128, 128));
            reinterpret_cast<irr::gui::IGUIStaticText*>(m_pSelectedTwo)->setBackgroundColor(irr::video::SColor(192, 255, 128, 128));

            m_pHoveredOne = nullptr;
            m_pHoveredTwo = nullptr;

            CGlobal::getInstance()->getActiveState()->enableDefault(false);

            m_vJoyStates.clear();

            for (unsigned i = 0; i < m_aJoysticks.size(); i++) {
              SJoystickState l_cState;
              l_cState.m_iIndex       = i;
              l_cState.m_sName        = m_aJoysticks[i].Name.c_str();
              l_cState.m_iAxes        = m_aJoysticks[i].Axes;
              l_cState.m_iButtons     = m_aJoysticks[i].Buttons;
              l_cState.m_bHasPov      = m_aJoysticks[i].PovHat == irr::SJoystickInfo::POV_HAT_PRESENT;
              l_cState.m_bInitialized = false;

              for (int i = 0; i < l_cState.m_iAxes; i++) {
                l_cState.m_vAxes.push_back(0.0);
              }

              m_vJoyStates.push_back(l_cState);
            }
          }
        }
      }

      if (m_pSelectedOne != nullptr && m_pSelectedTwo != nullptr) {
        if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
          if (!a_cEvent.KeyInput.PressedDown) {
            (*m_itSelected).m_eType = controller::CControllerBase::enInputType::Key;
            (*m_itSelected).m_eKey  = a_cEvent.KeyInput.Key;

            m_mControlText[m_itSelected]->setText(keyCodeToString(a_cEvent.KeyInput.Key).c_str());

            resetSelected();
            return true;
          }
        }
      }

      return false;
    }

    /**
    * Get the configuration of the controller as XML string
    * @return the configuration of the controller
    */
    std::string CControllerUi::getControllerString() {
      return "";
    }

    irr::gui::EGUI_ELEMENT_TYPE CControllerUi::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId;
    }

    /**
    * The "setText" method is abused to pass the configuration XML string to the UI
    * @param a_pText the new XML string
    */
    void CControllerUi::setText(const wchar_t* a_pText) {
      irr::gui::IGUIElement::setText(a_pText);

      std::wstring ws = a_pText;
      std::string s = platform::ws2s(ws);

      if (s != m_sConfigXml) {
        m_sConfigXml = s;

        irr::io::IReadFile* l_pFile = CGlobal::getInstance()->getFileSystem()->createMemoryReadFile(m_sConfigXml.c_str(), (irr::s32)m_sConfigXml.size(), "__controller__xml");
        if (l_pFile) {
          irr::io::IXMLReaderUTF8* l_pXml = CGlobal::getInstance()->getFileSystem()->createXMLReaderUTF8(l_pFile);
          if (l_pXml) {
            deserialize(l_pXml);
            buildUi(m_pParent);
            l_pXml->drop();

            CGlobal::getInstance()->getActiveState()->enableDefault(true);
          }
          l_pFile->drop();
        }
      }
    }

    void CControllerUi::OnJoystickEvent(const irr::SEvent& a_cEvent) {
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

        if (m_pSelectedOne != nullptr && m_pSelectedTwo != nullptr) {
          for (std::vector<SJoystickState>::iterator it = m_vJoyStates.begin(); it != m_vJoyStates.end(); it++) {
            if ((*it).m_bInitialized && (*it).m_iIndex == a_cEvent.JoystickEvent.Joystick) {
              for (int i = 0; i < (*it).m_iButtons; i++) {
                if (a_cEvent.JoystickEvent.IsButtonPressed(i)) {
                  (*m_itSelected).m_eType     = controller::CControllerBase::enInputType::JoyButton;
                  (*m_itSelected).m_iButton   = i;
                  (*m_itSelected).m_iJoystick = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelected).m_sJoystick = (*it).m_sName;

                  std::string s = (*it).m_sName + " Button " + std::to_string(i);
                  m_mControlText[m_itSelected]->setText(platform::s2ws(s).c_str());

                  resetSelected();
                  return;
                }
              }

              if ((*it).m_bHasPov) {
                irr::u16 l_iPov = a_cEvent.JoystickEvent.POV;

                if (l_iPov == 0 || l_iPov == 9000 || l_iPov == 18000 || l_iPov == 27000) {
                  (*m_itSelected).m_eType     = controller::CControllerBase::enInputType::JoyPov;
                  (*m_itSelected).m_iJoystick = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelected).m_sJoystick = (*it).m_sName;
                  (*m_itSelected).m_iPov      = l_iPov;

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
                  m_mControlText[m_itSelected]->setText(platform::s2ws(s).c_str());

                  resetSelected();
                  return;
                }
              }

              for (int i = 0; i < (*it).m_iAxes; i++) {
                float f = (((irr::f32)a_cEvent.JoystickEvent.Axis[i]) / 32500.0f) - (*it).m_vAxes[i];
                
                if (abs(f) > 0.5f) {
                  (*m_itSelected).m_eType      = controller::CControllerBase::enInputType::JoyAxis;
                  (*m_itSelected).m_iJoystick  = a_cEvent.JoystickEvent.Joystick;
                  (*m_itSelected).m_sJoystick  = (*it).m_sName;
                  (*m_itSelected).m_iAxis      = i;
                  (*m_itSelected).m_iDirection = f > 0.0f ? 1 : -1;

                  std::string s = (*it).m_sName + "Axis " + std::to_string(i) + ((*m_itSelected).m_iDirection > 0 ? " +" : " -");
                  m_mControlText[m_itSelected]->setText(platform::s2ws(s).c_str());

                  resetSelected();
                  return;
                }
              }
            }
          }
        }
      }
    }

    /**
    * Change the font for the configuration dialog
    */
    void CControllerUi::setFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
    }

    void CControllerUi::updateConfigXml() {
      char *s = new char[1000000];
      memset(s, 0, 1000000);

      irr::io::IWriteFile* l_pFile = CGlobal::getInstance()->getFileSystem()->createMemoryWriteFile(s, 1000000, "__controller_xml");

      if (l_pFile) {
        irr::io::IXMLWriterUTF8* l_pXml = CGlobal::getInstance()->getFileSystem()->createXMLWriterUTF8(l_pFile);
        if (l_pXml) {
          serialize(l_pXml);
          l_pXml->drop();
        }
        l_pFile->drop();
      }

      m_sConfigXml = std::string(s);
      delete []s;
      irr::gui::IGUIElement::setText(platform::s2ws(m_sConfigXml).c_str());
    }

    void CControllerUi::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      gui::CMenuBackground::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addString("headline", m_sHeadline.c_str());
    }

    void CControllerUi::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      gui::CMenuBackground::deserializeAttributes(a_pIn, a_pOptions);

      m_sHeadline = a_pIn->getAttributeAsString("headline").c_str();
    }
  } // namespace controller 
} // namespace dustbin