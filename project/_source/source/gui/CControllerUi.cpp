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
    std::wstring keyCodeToString(irr::EKEY_CODE e) {
      if (e == irr::KEY_LBUTTON   ) return L"KEY_LBUTTON";
      if (e == irr::KEY_RBUTTON   ) return L"KEY_RBUTTON";
      if (e == irr::KEY_CANCEL    ) return L"KEY_CANCEL";
      if (e == irr::KEY_MBUTTON   ) return L"KEY_MBUTTON";
      if (e == irr::KEY_XBUTTON1  ) return L"KEY_XBUTTON1";
      if (e == irr::KEY_XBUTTON2  ) return L"KEY_XBUTTON2";
      if (e == irr::KEY_BACK      ) return L"KEY_BACK";
      if (e == irr::KEY_TAB       ) return L"KEY_TAB";
      if (e == irr::KEY_CLEAR     ) return L"KEY_CLEAR";
      if (e == irr::KEY_RETURN    ) return L"KEY_RETURN";
      if (e == irr::KEY_SHIFT     ) return L"KEY_SHIFT";
      if (e == irr::KEY_CONTROL   ) return L"KEY_CONTROL";
      if (e == irr::KEY_MENU      ) return L"KEY_MENU";
      if (e == irr::KEY_PAUSE     ) return L"KEY_PAUSE";
      if (e == irr::KEY_CAPITAL   ) return L"KEY_CAPITAL";
      if (e == irr::KEY_KANA      ) return L"KEY_KANA";
      if (e == irr::KEY_HANGUEL   ) return L"KEY_HANGUEL";
      if (e == irr::KEY_HANGUL    ) return L"KEY_HANGUL";
      if (e == irr::KEY_JUNJA     ) return L"KEY_JUNJA";
      if (e == irr::KEY_FINAL     ) return L"KEY_FINAL";
      if (e == irr::KEY_HANJA     ) return L"KEY_HANJA";
      if (e == irr::KEY_KANJI     ) return L"KEY_KANJI";
      if (e == irr::KEY_ESCAPE    ) return L"KEY_ESCAPE";
      if (e == irr::KEY_CONVERT   ) return L"KEY_CONVERT";
      if (e == irr::KEY_NONCONVERT) return L"KEY_NONCONVERT";
      if (e == irr::KEY_ACCEPT    ) return L"KEY_ACCEPT";
      if (e == irr::KEY_MODECHANGE) return L"KEY_MODECHANGE";
      if (e == irr::KEY_SPACE     ) return L"KEY_SPACE";
      if (e == irr::KEY_PRIOR     ) return L"KEY_PRIOR";
      if (e == irr::KEY_NEXT      ) return L"KEY_NEXT";
      if (e == irr::KEY_END       ) return L"KEY_END";
      if (e == irr::KEY_HOME      ) return L"KEY_HOME";
      if (e == irr::KEY_LEFT      ) return L"KEY_LEFT";
      if (e == irr::KEY_UP        ) return L"KEY_UP";
      if (e == irr::KEY_RIGHT     ) return L"KEY_RIGHT";
      if (e == irr::KEY_DOWN      ) return L"KEY_DOWN";
      if (e == irr::KEY_SELECT    ) return L"KEY_SELECT";
      if (e == irr::KEY_PRINT     ) return L"KEY_PRINT";
      if (e == irr::KEY_EXECUT    ) return L"KEY_EXECUT";
      if (e == irr::KEY_SNAPSHOT  ) return L"KEY_SNAPSHOT";
      if (e == irr::KEY_INSERT    ) return L"KEY_INSERT";
      if (e == irr::KEY_DELETE    ) return L"KEY_DELETE";
      if (e == irr::KEY_HELP      ) return L"KEY_HELP";
      if (e == irr::KEY_KEY_0     ) return L"KEY_KEY_0";
      if (e == irr::KEY_KEY_1     ) return L"KEY_KEY_1";
      if (e == irr::KEY_KEY_2     ) return L"KEY_KEY_2";
      if (e == irr::KEY_KEY_3     ) return L"KEY_KEY_3";
      if (e == irr::KEY_KEY_4     ) return L"KEY_KEY_4";
      if (e == irr::KEY_KEY_5     ) return L"KEY_KEY_5";
      if (e == irr::KEY_KEY_6     ) return L"KEY_KEY_6";
      if (e == irr::KEY_KEY_7     ) return L"KEY_KEY_7";
      if (e == irr::KEY_KEY_8     ) return L"KEY_KEY_8";
      if (e == irr::KEY_KEY_9     ) return L"KEY_KEY_9";
      if (e == irr::KEY_KEY_A     ) return L"KEY_KEY_A";
      if (e == irr::KEY_KEY_B     ) return L"KEY_KEY_B";
      if (e == irr::KEY_KEY_C     ) return L"KEY_KEY_C";
      if (e == irr::KEY_KEY_D     ) return L"KEY_KEY_D";
      if (e == irr::KEY_KEY_E     ) return L"KEY_KEY_E";
      if (e == irr::KEY_KEY_F     ) return L"KEY_KEY_F";
      if (e == irr::KEY_KEY_G     ) return L"KEY_KEY_G";
      if (e == irr::KEY_KEY_H     ) return L"KEY_KEY_H";
      if (e == irr::KEY_KEY_I     ) return L"KEY_KEY_I";
      if (e == irr::KEY_KEY_J     ) return L"KEY_KEY_J";
      if (e == irr::KEY_KEY_K     ) return L"KEY_KEY_K";
      if (e == irr::KEY_KEY_L     ) return L"KEY_KEY_L";
      if (e == irr::KEY_KEY_M     ) return L"KEY_KEY_M";
      if (e == irr::KEY_KEY_N     ) return L"KEY_KEY_N";
      if (e == irr::KEY_KEY_O     ) return L"KEY_KEY_O";
      if (e == irr::KEY_KEY_P     ) return L"KEY_KEY_P";
      if (e == irr::KEY_KEY_Q     ) return L"KEY_KEY_Q";
      if (e == irr::KEY_KEY_R     ) return L"KEY_KEY_R";
      if (e == irr::KEY_KEY_S     ) return L"KEY_KEY_S";
      if (e == irr::KEY_KEY_T     ) return L"KEY_KEY_T";
      if (e == irr::KEY_KEY_U     ) return L"KEY_KEY_U";
      if (e == irr::KEY_KEY_V     ) return L"KEY_KEY_V";
      if (e == irr::KEY_KEY_W     ) return L"KEY_KEY_W";
      if (e == irr::KEY_KEY_X     ) return L"KEY_KEY_X";
      if (e == irr::KEY_KEY_Y     ) return L"KEY_KEY_Y";
      if (e == irr::KEY_KEY_Z     ) return L"KEY_KEY_Z";
      if (e == irr::KEY_LWIN      ) return L"KEY_LWIN";
      if (e == irr::KEY_RWIN      ) return L"KEY_RWIN";
      if (e == irr::KEY_APPS      ) return L"KEY_APPS";
      if (e == irr::KEY_SLEEP     ) return L"KEY_SLEEP";
      if (e == irr::KEY_NUMPAD0   ) return L"KEY_NUMPAD0";
      if (e == irr::KEY_NUMPAD1   ) return L"KEY_NUMPAD1";
      if (e == irr::KEY_NUMPAD2   ) return L"KEY_NUMPAD2";
      if (e == irr::KEY_NUMPAD3   ) return L"KEY_NUMPAD3";
      if (e == irr::KEY_NUMPAD4   ) return L"KEY_NUMPAD4";
      if (e == irr::KEY_NUMPAD5   ) return L"KEY_NUMPAD5";
      if (e == irr::KEY_NUMPAD6   ) return L"KEY_NUMPAD6";
      if (e == irr::KEY_NUMPAD7   ) return L"KEY_NUMPAD7";
      if (e == irr::KEY_NUMPAD8   ) return L"KEY_NUMPAD8";
      if (e == irr::KEY_NUMPAD9   ) return L"KEY_NUMPAD9";
      if (e == irr::KEY_MULTIPLY  ) return L"KEY_MULTIPLY";
      if (e == irr::KEY_ADD       ) return L"KEY_ADD";
      if (e == irr::KEY_SEPARATOR ) return L"KEY_SEPARATOR";
      if (e == irr::KEY_SUBTRACT  ) return L"KEY_SUBTRACT";
      if (e == irr::KEY_DECIMAL   ) return L"KEY_DECIMAL";
      if (e == irr::KEY_DIVIDE    ) return L"KEY_DIVIDE";
      if (e == irr::KEY_F1        ) return L"KEY_F1";
      if (e == irr::KEY_F2        ) return L"KEY_F2";
      if (e == irr::KEY_F3        ) return L"KEY_F3";
      if (e == irr::KEY_F4        ) return L"KEY_F4";
      if (e == irr::KEY_F5        ) return L"KEY_F5";
      if (e == irr::KEY_F6        ) return L"KEY_F6";
      if (e == irr::KEY_F7        ) return L"KEY_F7";
      if (e == irr::KEY_F8        ) return L"KEY_F8";
      if (e == irr::KEY_F9        ) return L"KEY_F9";
      if (e == irr::KEY_F10       ) return L"KEY_F10";
      if (e == irr::KEY_F11       ) return L"KEY_F11";
      if (e == irr::KEY_F12       ) return L"KEY_F12";
      if (e == irr::KEY_F13       ) return L"KEY_F13";
      if (e == irr::KEY_F14       ) return L"KEY_F14";
      if (e == irr::KEY_F15       ) return L"KEY_F15";
      if (e == irr::KEY_F16       ) return L"KEY_F16";
      if (e == irr::KEY_F17       ) return L"KEY_F17";
      if (e == irr::KEY_F18       ) return L"KEY_F18";
      if (e == irr::KEY_F19       ) return L"KEY_F19";
      if (e == irr::KEY_F20       ) return L"KEY_F20";
      if (e == irr::KEY_F21       ) return L"KEY_F21";
      if (e == irr::KEY_F22       ) return L"KEY_F22";
      if (e == irr::KEY_F23       ) return L"KEY_F23";
      if (e == irr::KEY_F24       ) return L"KEY_F24";
      if (e == irr::KEY_NUMLOCK   ) return L"KEY_NUMLOCK";
      if (e == irr::KEY_SCROLL    ) return L"KEY_SCROLL";
      if (e == irr::KEY_LSHIFT    ) return L"KEY_LSHIFT";
      if (e == irr::KEY_RSHIFT    ) return L"KEY_RSHIFT";
      if (e == irr::KEY_LCONTROL  ) return L"KEY_LCONTROL";
      if (e == irr::KEY_RCONTROL  ) return L"KEY_RCONTROL";
      if (e == irr::KEY_LMENU     ) return L"KEY_LMENU";
      if (e == irr::KEY_RMENU     ) return L"KEY_RMENU";
      if (e == irr::KEY_PLUS      ) return L"KEY_PLUS";
      if (e == irr::KEY_COMMA     ) return L"KEY_COMMA";
      if (e == irr::KEY_MINUS     ) return L"KEY_MINUS";
      if (e == irr::KEY_PERIOD    ) return L"KEY_PERIOD";
      if (e == irr::KEY_ATTN      ) return L"KEY_ATTN";
      if (e == irr::KEY_CRSEL     ) return L"KEY_CRSEL";
      if (e == irr::KEY_EXSEL     ) return L"KEY_EXSEL";
      if (e == irr::KEY_EREOF     ) return L"KEY_EREOF";
      if (e == irr::KEY_PLAY      ) return L"KEY_PLAY";
      if (e == irr::KEY_ZOOM      ) return L"KEY_ZOOM";
      if (e == irr::KEY_PA1       ) return L"KEY_PA1";
      if (e == irr::KEY_OEM_CLEAR ) return L"KEY_OEM_CLEAR";
      return L"UNKNOWN";
    }

    CControllerUi::CControllerUi(irr::gui::IGUIElement* a_pParent) :
      CMenuBackground(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiId),
      m_pCursor      (CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()),
      m_pFont        (nullptr),
      m_pMenuMgr     (nullptr),
      m_sSelected    (""),
      m_sConfigData  ("")
    {
      m_pFont = CGlobal::getInstance()->getFont(enFont::Regular, CGlobal::getInstance()->getVideoDriver()->getScreenSize());

      m_iFontHeight = m_pFont->getDimension(L"ABCdefg").Height;

      std::string l_aImages[] = {
        "ctrl_config_joykey",
        "ctrl_config_touch_left",
        "ctrl_config_touch_right",
        "ctrl_config_touch_center",
        "ctrl_config_gyro",
        "ctrl_config_touch_steer"
      };

      for (std::string l_sImage : l_aImages) {
        std::string l_sPath = "data/images/" + l_sImage + ".png";
        irr::video::ITexture *l_pImage = m_pDrv->getTexture(l_sPath.c_str());

        if (l_pImage != nullptr) {
          printf("Loaded \"%s\"\n", l_sImage.c_str());
          m_mImages[l_sImage] = std::make_tuple(l_pImage, irr::core::recti(0, 0, l_pImage->getSize().Width, l_pImage->getSize().Height));
        }
      }
      printf("Constructor: %i, %i - %i, %i\n", AbsoluteClippingRect.UpperLeftCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);
    }

    CControllerUi::~CControllerUi() {
    }

    /**
    * This method builds the UI for the controller
    * @param a_pParent the parent element.
    */
    void CControllerUi::buildUi(irr::gui::IGUIElement* a_pParent) {
      printf("Build UI: %i, %i - %i, %i\n", AbsoluteClippingRect.UpperLeftCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);
    }

    bool CControllerUi::isEditing() {
      return false;
    }

    void CControllerUi::setMenuManager(menu::IMenuManager* a_pMenuManager) {
      printf("Set menu manager: %i, %i - %i, %i\n", AbsoluteClippingRect.UpperLeftCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);
      m_pMenuMgr = a_pMenuManager;

      if (m_mImages.size() > 0) {
        // Calculate the drawing rect
        irr::f32 l_fRatio  = (irr::f32)std::get<0>(m_mImages.begin()->second)->getSize().Width / (irr::f32)std::get<0>(m_mImages.begin()->second)->getSize().Height;
        irr::f32 l_fWidth  = 0.8f * (irr::f32)AbsoluteClippingRect.getWidth();
        irr::f32 l_fHeight = l_fWidth  / l_fRatio;
        irr::f32 l_fFactX  = l_fWidth  / (irr::f32)std::get<0>(m_mImages.begin()->second)->getSize().Width;
        irr::f32 l_fFactY  = l_fHeight / (irr::f32)std::get<0>(m_mImages.begin()->second)->getSize().Height;

        m_cDraw = irr::core::recti(
          10 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100 - (irr::s32)l_fHeight + AbsoluteClippingRect.UpperLeftCorner.Y,
          90 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100                       + AbsoluteClippingRect.UpperLeftCorner.Y
        );

        m_mTextPositions[enControls::Forward ] = irr::core::vector2di((irr::s32)(497.0f * l_fFactX), (irr::s32)( 27.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Backward] = irr::core::vector2di((irr::s32)(497.0f * l_fFactX), (irr::s32)(393.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Right   ] = irr::core::vector2di((irr::s32)(675.0f * l_fFactX), (irr::s32)(211.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Left    ] = irr::core::vector2di((irr::s32)(304.0f * l_fFactX), (irr::s32)(211.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Brake   ] = irr::core::vector2di((irr::s32)( 71.0f * l_fFactX), (irr::s32)( 43.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Rearview] = irr::core::vector2di((irr::s32)( 71.0f * l_fFactX), (irr::s32)(128.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Respawn ] = irr::core::vector2di((irr::s32)( 71.0f * l_fFactX), (irr::s32)(212.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Pause   ] = irr::core::vector2di((irr::s32)( 71.0f * l_fFactX), (irr::s32)(294.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
        m_mTextPositions[enControls::Cancel  ] = irr::core::vector2di((irr::s32)( 71.0f * l_fFactX), (irr::s32)(379.0f * l_fFactY)) + m_cDraw.UpperLeftCorner;
      }
    }

    bool CControllerUi::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

    // This method is necessary because UI elements don't receive Joystick events
    bool CControllerUi::update(const irr::SEvent& a_cEvent) {
      bool l_bRet = CControllerBase::update(a_cEvent);

      return l_bRet;
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
        buildUi(Parent);
      }
    }

    bool CControllerUi::OnJoystickEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

    /**
    * Change the font for the configuration dialog
    */
    void CControllerUi::setFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
    }

    void CControllerUi::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      gui::CMenuBackground::serializeAttributes(a_pOut, a_pOptions);
    }

    void CControllerUi::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      gui::CMenuBackground::deserializeAttributes(a_pIn, a_pOptions);
    }

    void CControllerUi::draw() {
      CMenuBackground::draw();

      if (m_mImages.size() > 0) {
        irr::video::ITexture *l_pImage = nullptr;
        irr::core::recti      l_cSource;

        if (m_mImages.find(m_sSelected) != m_mImages.end()) {
          l_pImage  = std::get<0>(m_mImages[m_sSelected]);
          l_cSource = std::get<1>(m_mImages[m_sSelected]);
        }
        else {
          l_pImage  = std::get<0>(m_mImages.begin()->second);
          l_cSource = std::get<1>(m_mImages.begin()->second);
        }

        m_pDrv->draw2DImage(l_pImage, m_cDraw, l_cSource, &AbsoluteClippingRect, nullptr, true);

        for (CControllerBase::SCtrlInput l_cItem : m_vControls) {
          std::wstring l_sControl = getControlText(&l_cItem);
          irr::core::dimension2du l_cSize = m_pFont->getDimension(l_sControl.c_str());
          irr::core::vector2di l_cPos;
          bool l_bDraw = false;

          l_cSize.Width  = 5 * l_cSize.Width  / 4;
          l_cSize.Height = 5 * l_cSize.Height / 4;

          if (l_cItem.m_sName == "Forward") {
            l_cPos = m_mTextPositions[enControls::Forward];
            l_cPos.X -= l_cSize.Width  / 2;
            l_cPos.Y -= l_cSize.Height;
            l_bDraw = true;
          }
          else if (l_cItem.m_sName == "Backward") {
            l_cPos = m_mTextPositions[enControls::Backward];
            l_cPos.X -= l_cSize.Width  / 2;
            l_bDraw = true;
          }
          else if (l_cItem.m_sName == "Right") {
            l_cPos = m_mTextPositions[enControls::Right];
            l_cPos.Y -= l_cSize.Height / 2;
            l_bDraw = true;
          }
          else if (l_cItem.m_sName == "Left") {
            l_cPos = m_mTextPositions[enControls::Left];
            l_cPos.X -= l_cSize.Width;
            l_cPos.Y -= l_cSize.Height / 2;
            l_bDraw = true;
          }
          else {
            if (l_cItem.m_sName == "Brake") {
              l_cPos = m_mTextPositions[enControls::Brake];
              l_bDraw = true;
            }
            else if (l_cItem.m_sName == "Rearview") {
              l_cPos = m_mTextPositions[enControls::Rearview];
              l_bDraw = true;
            }
            else if (l_cItem.m_sName == "Respawn") {
              l_cPos = m_mTextPositions[enControls::Respawn];
              l_bDraw = true;
            }
            else if (l_cItem.m_sName == "Pause") {
              l_cPos = m_mTextPositions[enControls::Pause];
              l_bDraw = true;
            }
            else if (l_cItem.m_sName == "Cancel Race") {
              l_cPos = m_mTextPositions[enControls::Cancel];
              l_bDraw = true;
            }

            l_cPos.Y -= l_cSize.Height / 2;
          }

          if (l_bDraw) {
            irr::core::recti l_cRect = irr::core::recti(l_cPos, l_cSize);

            m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 192, 192, 192), l_cRect, &AbsoluteClippingRect);
            m_pDrv->draw2DRectangleOutline(l_cRect, irr::video::SColor(0xFF, 0, 0, 0));
            m_pFont->draw(getControlText(&l_cItem).c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), true, true, &AbsoluteClippingRect);
          }
        }
      }
      else m_pDrv->draw2DRectangle(irr::video::SColor(50, 0xFF, 0, 0), m_cDraw);
    }

    /**
    * Get a readable string of the set controls
    * @return a readable string of the set controls
    */
    std::wstring CControllerUi::getControlText(CControllerBase::SCtrlInput* a_pCtrl) {
      switch (a_pCtrl->m_eType) {
        case enInputType::JoyAxis:
          return L"Axis " + std::to_wstring(a_pCtrl->m_iAxis) + (a_pCtrl->m_iDirection > 0 ? L"+" : L"-");
          
        case enInputType::JoyButton:
          return L"Button " + std::to_wstring(a_pCtrl->m_iButton);

        case enInputType::JoyPov:
          return L"POV " + std::to_wstring(a_pCtrl->m_iPov);

        case enInputType::Key:
          return keyCodeToString(a_pCtrl->m_eKey);

        default:
          return L"";
      }
    }
  } // namespace controller 
} // namespace dustbin