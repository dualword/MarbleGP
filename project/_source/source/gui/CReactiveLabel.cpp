// (w) 2021 by Dustbin::Games / Christian Keimel

#include <gui/CReactiveLabel.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CReactiveLabel::CReactiveLabel(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_ReactiveLabelId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pFont(CGlobal::getInstance()->getFont(enFont::Regular, CGlobal::getInstance()->getVideoDriver()->getScreenSize())),
      m_cBackground(irr::video::SColor(32, 255, 255, 255)),
      m_pGui(CGlobal::getInstance()->getGuiEnvironment()),
      m_cHovered(irr::video::SColor(128, 255, 255, 128)),
      m_cClicked(irr::video::SColor(128, 255, 128, 128)),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_cBorder(irr::video::SColor(255, 0, 0, 0)),
      m_cText(irr::video::SColor(255, 0, 0, 0)),
      m_bBackground(false),
      m_bHovered(false),
      m_bCenterH(false),
      m_bCenterV(true),
      m_bLDown(false)
    {

    }

    CReactiveLabel::~CReactiveLabel() {
    }

    void CReactiveLabel::draw() {
      if (IsVisible) {
        irr::core::recti l_cRect = getAbsoluteClippingRect();

        if (m_bHovered) {
          if (m_bLDown) {
            m_pDrv->draw2DRectangle(m_cClicked, l_cRect);
          }
          else {
            m_pDrv->draw2DRectangle(m_cHovered, l_cRect);
          }
        }
        else {
          if (m_bBackground)
            m_pDrv->draw2DRectangle(m_cBackground, l_cRect);
        }

        if (m_bBorder) {
          irr::core::vector2di l_cPoints[] = {
            irr::core::vector2di(l_cRect.UpperLeftCorner .X, l_cRect.UpperLeftCorner .Y),
            irr::core::vector2di(l_cRect.LowerRightCorner.X, l_cRect.UpperLeftCorner .Y),
            irr::core::vector2di(l_cRect.LowerRightCorner.X, l_cRect.LowerRightCorner.Y),
            irr::core::vector2di(l_cRect.UpperLeftCorner .X, l_cRect.LowerRightCorner.Y)
          };

          for (int i = 0; i < 4; i++)
            m_pDrv->draw2DLine(l_cPoints[i], l_cPoints[i != 3 ? i + 1 : 0], m_cBorder);
        }

        m_pFont->draw(getText(), l_cRect, m_cText, m_bCenterH, m_bCenterV, &l_cRect);
      }
    }

    void CReactiveLabel::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("CenterH"       , m_bCenterH   );
      a_pOut->addBool("CenterV"       , m_bCenterV   );
      a_pOut->addBool("DrawBackground", m_bBackground);
      a_pOut->addBool("DrawBorder"    , m_bBorder    );

      a_pOut->addColor("TextColor"      , m_cText      );
      a_pOut->addColor("BorderColor"    , m_cBorder    );
      a_pOut->addColor("BackgroundColor", m_cBackground);
      a_pOut->addColor("HoverColor"     , m_cHovered   );
      a_pOut->addColor("ClickColor"     , m_cClicked   );
    }

    void CReactiveLabel::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      m_bCenterH    = a_pIn->getAttributeAsBool("CenterH"       );
      m_bCenterV    = a_pIn->getAttributeAsBool("CenterV"       );
      m_bBackground = a_pIn->getAttributeAsBool("DrawBackground");
      m_bBorder     = a_pIn->getAttributeAsBool("DrawBorder"    );

      m_cText       = a_pIn->getAttributeAsColor("TextColor"      );
      m_cBorder     = a_pIn->getAttributeAsColor("BorderColor"    );
      m_cBackground = a_pIn->getAttributeAsColor("BackgroundColor");
      m_cHovered    = a_pIn->getAttributeAsColor("HoverColor"     );
      m_cClicked    = a_pIn->getAttributeAsColor("ClickColor"     );
    }

    void CReactiveLabel::setOverrideFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
    }

    bool CReactiveLabel::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.isLeftPressed()) {
          m_bLDown = true;
        }
        else if (!a_cEvent.MouseInput.isLeftPressed()) {
          if (m_bLDown) {
            m_bLDown = false;
            irr::core::vector2di l_cPos = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            if (getAbsoluteClippingRect().isPointInside(l_cPos)) {
              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
              l_cEvent.GUIEvent.Caller    = this;
              l_cEvent.GUIEvent.Element   = this;

              CGlobal::getInstance()->getActiveState()->OnEvent(l_cEvent);
            }
          }
        }
      }

      return l_bRet;
    }
  } // namespace gui
}   // namespace dustbin