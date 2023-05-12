// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <sound/ISoundInterface.h>
#include <gui/CDustbinCheckbox.h>
#include <sound/CSoundEnums.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {

    CDustbinCheckbox::CDustbinCheckbox(irr::gui::IGUIElement* a_pParent) : 
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent, -1, irr::core::recti()), 
      m_pDrv               (CGlobal::getInstance()->getVideoDriver()),
      m_bChecked           (false),
      m_bMouseL            (false),
      m_bHover             (false),
      m_bInside            (false),
      m_iBorder            (CGlobal::getInstance()->getRasterSize() / 10),
      m_cBackgroundColor   (irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF)),
      m_cHoverColor        (irr::video::SColor(0xff, 0x33, 0x67, 0xb8)),
      m_cClickColor        (irr::video::SColor(0xff, 0xec, 0xf1, 0x63)),
      m_cDisabledColor     (irr::video::SColor(0xFF, 0xA0, 0xA0, 0xA0))
    {
    }

    CDustbinCheckbox::~CDustbinCheckbox() {
    }

    void CDustbinCheckbox::draw() {
      if (IsVisible) {
        int l_iWidth  = getAbsoluteClippingRect().getWidth (),
            l_iHeight = getAbsoluteClippingRect().getHeight(),
            l_iDrawX  = 85 * l_iWidth  / 100,
            l_iDrawY  = 85 * l_iHeight / 100,
            l_iCheckX = 75 * l_iWidth  / 100,
            l_iCheckY = 75 * l_iWidth  / 100,
            l_iBorder = l_iDrawX  > l_iDrawY  ? l_iHeight - l_iDrawY  : l_iWidth - l_iDrawX,
            l_iCheckB = l_iCheckX > l_iCheckY ? l_iHeight - l_iCheckY : l_iWidth - l_iCheckX,
            l_iPosX   = getAbsoluteClippingRect().UpperLeftCorner.X,
            l_iPosY   = getAbsoluteClippingRect().UpperLeftCorner.Y;

        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), irr::core::recti(l_iPosX, l_iPosY, l_iPosX + l_iWidth, l_iPosY + l_iHeight));

        m_pDrv->draw2DRectangle(IsEnabled ? (getAbsoluteClippingRect().isPointInside(m_cMousePos) && m_bInside) ? m_bMouseL ? m_cClickColor : m_cHoverColor : m_cBackgroundColor : m_cDisabledColor, irr::core::recti(l_iPosX + l_iBorder, l_iPosY + l_iBorder, l_iPosX + l_iDrawX, l_iPosY + l_iDrawY ));

        if (m_bChecked)
          m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), irr::core::recti(l_iPosX + l_iCheckB, l_iPosY + l_iCheckB, l_iPosX + l_iCheckX, l_iPosY + l_iCheckY));

        if (getAbsoluteClippingRect().isPointInside(m_cMousePos)) {
          if (!m_bHover)
            CGlobal::getInstance()->getSoundInterface()->play2d(en2dSounds::ButtonHover, CGlobal::getInstance()->getSettingData().m_fSfxMenu, 0.0f);

          m_bHover = true;
        }
        else m_bHover = false;
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CDustbinCheckbox::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_DustbinCheckboxId;
    }

    bool CDustbinCheckbox::OnEvent(const irr::SEvent& a_cEvent) {
      if (!isEnabled())
        return false;

      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseL = false;
          if (getAbsoluteClippingRect().isPointInside(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y))) {
            m_bChecked = !m_bChecked;

            if (Parent != nullptr) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_CHECKBOX_CHANGED;
              l_cEvent.GUIEvent.Caller    = this;
              l_cEvent.GUIEvent.Element   = this;
              Parent->OnEvent(l_cEvent);
            }

            CGlobal::getInstance()->getSoundInterface()->play2d(en2dSounds::ButtonPress, CGlobal::getInstance()->getSettingData().m_fSfxMenu, 0.0f);
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN)
          m_bMouseL = true;
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          m_cMousePos.X = a_cEvent.MouseInput.X;
          m_cMousePos.Y = a_cEvent.MouseInput.Y;
        }
      }
      else if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.Caller == this) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED)
          m_bInside = true;
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT)
          m_bInside = false;
      }

      if (!l_bRet)
        IGUIElement::OnEvent(a_cEvent);

      return l_bRet;
    }

    void CDustbinCheckbox::setChecked(bool a_bChecked) {
      m_bChecked = a_bChecked;
    }

    bool CDustbinCheckbox::isChecked() {
      return m_bChecked;
    }

    void CDustbinCheckbox::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);
      a_pOut->addBool("Checked"         , m_bChecked);
      a_pOut->addColor("BackgroundColor", m_cBackgroundColor);
      a_pOut->addColor("DisabledColor"  , m_cDisabledColor);
      a_pOut->addColor("HoverColor"     , m_cHoverColor);
      a_pOut->addColor("ClickColor"     , m_cClickColor);
    }

    void CDustbinCheckbox::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      m_bChecked         = a_pIn->getAttributeAsBool("Checked");
      m_cBackgroundColor = a_pIn->getAttributeAsColor("BackgroundColor");
      m_cDisabledColor   = a_pIn->getAttributeAsColor("DisabledColor");
      m_cHoverColor      = a_pIn->getAttributeAsColor("HoverColor");
      m_cClickColor      = a_pIn->getAttributeAsColor("ClickColor");
    }

  } // namespace gui
}   // namespace dustbin