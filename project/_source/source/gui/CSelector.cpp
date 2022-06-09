// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <sound/ISoundInterface.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace gui {

    CSelector::CSelector(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_SelectorId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent, -1, irr::core::recti()),
      m_pDrv               (CGlobal::getInstance()->getVideoDriver()),
      m_bCenterH           (false),
      m_bLeftBtn           (false),
      m_bCenterV           (true),
      m_bHoverL            (false),
      m_bHoverR            (false),
      m_bHoverT            (false),
      m_bInside            (false),
      m_cBackgroundColor   (irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF)),
      m_cHoverColor        (irr::video::SColor(0xff, 0x33, 0x67, 0xb8)),
      m_cClickColor        (irr::video::SColor(0xff, 0xec, 0xf1, 0x63)),
      m_cDisabledColor     (irr::video::SColor(0xFF, 0xA0, 0xA0, 0xA0)),
      m_pFont              (CGlobal::getInstance()->getGuiEnvironment()->getSkin()->getFont()),
      m_iSelected          (-1)
    {
    }

    CSelector::~CSelector() {
    }

    void CSelector::draw() {
      if (IsVisible) {
        bool l_bHover = m_bHoverL || m_bHoverR || m_bHoverT;

        m_bHoverL = m_cBoxL.isPointInside(m_cMousePos) && m_bInside;
        m_bHoverR = m_cBoxR.isPointInside(m_cMousePos) && m_bInside;
        m_bHoverT = m_cText.isPointInside(m_cMousePos) && m_bInside;

        if (!l_bHover && (m_bHoverL || m_bHoverR || m_bHoverT) && isEnabled())
          CGlobal::getInstance()->getSoundInterface()->play2d(L"data/sounds/button_hover.ogg", 1.0f, 0.0f);

        irr::core::recti l_cRect = getAbsoluteClippingRect();

        int l_iRectSize = l_cRect.getWidth() > l_cRect.getHeight() ? l_cRect.getHeight() : l_cRect.getWidth(),
            l_iTextOffs = l_iRectSize / 4;

        m_cBoxL = irr::core::recti(l_cRect.UpperLeftCorner, irr::core::dimension2du(l_iRectSize, l_iRectSize));
        m_cBoxR = irr::core::recti(l_cRect.LowerRightCorner.X - l_iRectSize, l_cRect.UpperLeftCorner.Y, l_cRect.LowerRightCorner.X, l_cRect.LowerRightCorner.Y);

        m_cText = irr::core::recti(l_cRect.UpperLeftCorner.X + l_iRectSize + l_iTextOffs, l_cRect.UpperLeftCorner.Y, l_cRect.LowerRightCorner.X - l_iRectSize - l_iTextOffs, l_cRect.LowerRightCorner.Y);

        int l_iOffset = 10 * l_iRectSize / 100;

        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), l_cRect);

        l_cRect.UpperLeftCorner .X += l_iOffset;
        l_cRect.UpperLeftCorner .Y += l_iOffset;
        l_cRect.LowerRightCorner.X -= l_iOffset;
        l_cRect.LowerRightCorner.Y -= l_iOffset;

        m_pDrv->draw2DRectangle(m_bHoverT ? (m_bSelected || m_bLeftBtn) ? m_cClickColor : m_cHoverColor : IsEnabled ? m_cBackgroundColor : m_cDisabledColor, l_cRect);

        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), m_cBoxL);

        m_cBoxL.UpperLeftCorner.X += l_iOffset;
        m_cBoxL.UpperLeftCorner.Y += l_iOffset;
        m_cBoxL.LowerRightCorner.X -= l_iOffset;
        m_cBoxL.LowerRightCorner.Y -= l_iOffset;

        m_pDrv->draw2DRectangle(m_bHoverL ? (m_bSelected || m_bLeftBtn) ? m_cClickColor : m_cHoverColor : IsEnabled ? m_cBackgroundColor : m_cDisabledColor, m_cBoxL);

        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0, 0, 0), m_cBoxR);

        m_cBoxR.UpperLeftCorner.X += l_iOffset;
        m_cBoxR.UpperLeftCorner.Y += l_iOffset;
        m_cBoxR.LowerRightCorner.X -= l_iOffset;
        m_cBoxR.LowerRightCorner.Y -= l_iOffset;

        m_pDrv->draw2DRectangle(m_bHoverR ? (m_bSelected || m_bLeftBtn) ? m_cClickColor : m_cHoverColor : IsEnabled ? m_cBackgroundColor : m_cDisabledColor, m_cBoxR);

        int l_iArrowOffset = l_iRectSize / 10;

        irr::core::recti l_cArrow[] = {
          irr::core::recti(m_cBoxL.UpperLeftCorner + irr::core::vector2di(l_iArrowOffset, l_iArrowOffset), m_cBoxL.LowerRightCorner - irr::core::vector2di(l_iArrowOffset, l_iArrowOffset)),
          irr::core::recti(m_cBoxR.UpperLeftCorner + irr::core::vector2di(l_iArrowOffset, l_iArrowOffset), m_cBoxR.LowerRightCorner - irr::core::vector2di(l_iArrowOffset, l_iArrowOffset))
        };


        for (int i = 0; i < 2; i++) {
          irr::core::vector2di l_cCenter = l_cArrow[i].getCenter();

          int x  = l_cCenter.X,
              d  = 15 * l_cArrow[i].getHeight() / 100,
              t1 = l_cCenter.Y - d,
              t2 = l_cCenter.Y + d;

          for (int y = l_cArrow[i].UpperLeftCorner.Y; y < l_cArrow[i].LowerRightCorner.Y; y++) {
            if (y < l_cCenter.Y) {
              if (i == 0) x--; else x++;
            }
            else if (y > l_cCenter.Y) {
              if (i == 0) x++; else x--;
            }

            m_pDrv->draw2DLine(irr::core::vector2di(x, y), (y >= t1 && y < t2) ? irr::core::vector2di(i == 0 ? l_cArrow[i].LowerRightCorner.X : l_cArrow[i].UpperLeftCorner.X, y) : irr::core::vector2di(l_cCenter.X, y), irr::video::SColor(0xFF, 0, 0, 0));
          }
        }

        if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
          m_pFont->draw(m_vItems[m_iSelected].c_str(), m_cText, irr::video::SColor(0xFF, 0, 0, 0), m_bCenterH, m_bCenterV);
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CSelector::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_SelectorId;
    }

    bool CSelector::OnEvent(const irr::SEvent& a_cEvent) {
      if (!IsEnabled)
        return false;

      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bLeftBtn = false;
          if (m_cBoxL.isPointInside(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y))) {
            if (m_iSelected > 0) {
              m_iSelected--;

              if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
                setText(m_vItems[m_iSelected].c_str());

              distributeEvent();
            }
          }
          else if (m_cBoxR.isPointInside(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y))) {
            if (m_iSelected < m_vItems.size() - 1) {
              m_iSelected++;

              if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
                setText(m_vItems[m_iSelected].c_str());

              distributeEvent();
            }
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_bLeftBtn = true;
          if (m_bHoverL || m_bHoverR || m_bHoverT)
            CGlobal::getInstance()->getSoundInterface()->play2d(L"data/sounds/button_press.ogg", 1.0f, 0.0f);
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          m_cMousePos.X = a_cEvent.MouseInput.X;
          m_cMousePos.Y = a_cEvent.MouseInput.Y;
        }
      }
      else if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.Caller == this) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          m_bInside = false;
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          m_bInside = true;
        }
      }
      else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
          if (a_cEvent.UserEvent.UserData2 == 0) {
            m_bSelected = !m_bSelected;
            m_bLeftBtn  =  m_bSelected;
            l_bRet = true;
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMoveMouse) {
          if (m_bSelected) {
            if (a_cEvent.UserEvent.UserData2 == 2) {
              if (m_iSelected < m_vItems.size() - 1)
                m_iSelected++;

              if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
                setText(m_vItems[m_iSelected].c_str());

              distributeEvent();
              l_bRet = true;
            }
            else if (a_cEvent.UserEvent.UserData2 == 3) {
              if (m_iSelected > 0)
                m_iSelected--;

              if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
                setText(m_vItems[m_iSelected].c_str());

              distributeEvent();
              l_bRet = true;
            }
          }
        }
      }

      if (!l_bRet)
        IGUIElement::OnEvent(a_cEvent);

      return l_bRet;
    }

    /**
    * Pass a "scrollbar changed" event on to the parent
    */
    void CSelector::distributeEvent() {
      if (Parent != nullptr) {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_GUI_EVENT;
        l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
        l_cEvent.GUIEvent.Caller = this;
        Parent->OnEvent(l_cEvent);
      }
    }

    void CSelector::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::gui::IGUIElement::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("CenterH", m_bCenterH);
      a_pOut->addBool("CenterV", m_bCenterV);
      a_pOut->addInt("Selected", m_iSelected);

      a_pOut->addColor("BackgroundColor", m_cBackgroundColor);
      a_pOut->addColor("DisabledColor"  , m_cDisabledColor);
      a_pOut->addColor("HoverColor"     , m_cHoverColor);
      a_pOut->addColor("ClickColor"     , m_cClickColor);
    }

    void CSelector::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::gui::IGUIElement::deserializeAttributes(a_pIn, a_pOptions);

      m_bCenterH = a_pIn->getAttributeAsBool("CenterH");
      m_bCenterV = a_pIn->getAttributeAsBool("CenterV");
      m_iSelected = a_pIn->getAttributeAsInt("Selected");

      if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
        setText(m_vItems[m_iSelected].c_str());

      m_cBackgroundColor = a_pIn->getAttributeAsColor("BackgroundColor");
      m_cDisabledColor   = a_pIn->getAttributeAsColor("DisabledColor");
      m_cHoverColor      = a_pIn->getAttributeAsColor("HoverColor");
      m_cClickColor      = a_pIn->getAttributeAsColor("ClickColor");
    }

    /**
    * Clear the items of the selector
    */
    void CSelector::clear() {
      m_vItems.clear();
    }

    /**
    * Add an item to the selector
    * @param a_sItem the new item
    */
    void CSelector::addItem(const std::wstring& a_sItem) {
      m_vItems.push_back(a_sItem);

      if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
        setText(m_vItems[m_iSelected].c_str());
    }

    /**
    * Get an item by it's index
    * @param a_iIndex the index of the requested item
    * @return the item, empty string if the index is out of bounds
    */
    std::wstring CSelector::getItem(int a_iIndex) {
      return a_iIndex >= 0 && a_iIndex < m_vItems.size() ? m_vItems[a_iIndex] : L"";
    }

    /**
    * Get the index of the selected item
    * @return the index of the selected item
    */
    int CSelector::getSelected() {
      return m_iSelected;
    }

    /**
    * Get the number of items in the selector
    * @return the number of items in the selector
    */
    unsigned CSelector::getItemCount() {
      return (unsigned)m_vItems.size();
    }

    /**
    * Set the selected item
    * @param a_iIndex the new index
    */
    void CSelector::setSelected(int a_iIndex) {
      if ((a_iIndex >= 0 && a_iIndex < m_vItems.size()) || m_vItems.size() == 0) {
        m_iSelected = a_iIndex;

        if (m_iSelected >= 0 && m_iSelected < m_vItems.size())
          setText(m_vItems[m_iSelected].c_str());
      }
    }
  } // namespace gui
}   // namespace dustbin
