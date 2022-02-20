// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <messages/CMessageHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <gui/CReactiveLabel.h>
#include <gui/CTrackButton.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu(int a_iZLayer) :
      m_bButtonDown(false),
      m_bMoved     (false),
      m_bActive    (false),
      m_bCancelDown(false),
      m_bOkDown    (false),
      m_pGui       (CGlobal::getInstance()->getGuiEnvironment()),
      m_pCursor    (CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()),
      m_pDevice    (CGlobal::getInstance()->getIrrlichtDevice()),
      m_pTimer     (CGlobal::getInstance()->getIrrlichtDevice()->getTimer()),
      m_iZLayer    (a_iZLayer),
      m_pHovered   (nullptr),
      m_pSelected  (nullptr)
    {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP    ; l_cInput.m_sName = "Up"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN  ; l_cInput.m_sName = "Down"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT  ; l_cInput.m_sName = "Left"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT ; l_cInput.m_sName = "Right" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE ; l_cInput.m_sName = "Enter" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RETURN; l_cInput.m_sName = "Ok"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_ESCAPE; l_cInput.m_sName = "Cancel"; m_vControls.push_back(l_cInput);

      setZLayer(m_iZLayer > 0 ? m_iZLayer : 0);
    }

    CControllerMenu::~CControllerMenu() {
    }

    /**
    * Fill the "m_vElements" vector
    * @param a_pParent the item to add, the children will be iterated and added as well
    */
    void CControllerMenu::fillItemList(irr::gui::IGUIElement* a_pParent, int a_iZLayer) {
      irr::gui::EGUI_ELEMENT_TYPE l_eType = a_pParent->getType();

      if (getElementZLayer(a_pParent) == a_iZLayer) {
        switch (l_eType) {
          case irr::gui::EGUIET_EDIT_BOX:
            if (reinterpret_cast<irr::gui::IGUIEditBox*>(a_pParent)->isEnabled()) {
              printf("Ui element #%i found on Z-Layer %i: \"%s\" (%i)\n", (int)m_vElements.size(), a_iZLayer, a_pParent->getName(), a_pParent->getID());
              m_vElements.push_back(a_pParent);
            }
            break;

          case irr::gui::EGUIET_CHECK_BOX:
          case irr::gui::EGUIET_COMBO_BOX:
          case irr::gui::EGUIET_SCROLL_BAR:
          case dustbin::gui::g_ReactiveLabelId:
          case dustbin::gui::g_DustbinCheckboxId:
          case dustbin::gui::g_SelectorId:
          case dustbin::gui::g_MenuButtonId:
          case dustbin::gui::g_ClipImageId:
          case dustbin::gui::g_TrackButtonId:
            printf("Ui element #%i found on Z-Layer %i: \"%s\" (%i)\n", (int)m_vElements.size(), a_iZLayer, a_pParent->getName(), a_pParent->getID());
            m_vElements.push_back(a_pParent);
            break;

          default:
            break;
        }
      }

      for (irr::core::list<irr::gui::IGUIElement*>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++) {
        fillItemList(*it, a_iZLayer);
      }
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    bool CControllerMenu::update(const irr::SEvent& a_cEvent) {
      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventSettingsChanged) {
          m_bActive = CGlobal::getInstance()->getSettingData().m_bUseMenuCtrl;
          deserialize(CGlobal::getInstance()->getSettingData().m_sController);
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventChangeZLayer) {
          setZLayer((int)a_cEvent.UserEvent.UserData2);
        }
      }

      if (!m_bActive)
        return false;

      bool l_bRet = CControllerBase::update(a_cEvent);

      if (m_pSelected == nullptr) {
        if (m_vControls[0].m_fValue > 0.5f) {
          moveMouse(enDirection::Up);
        }
        else if (m_vControls[1].m_fValue > 0.5f) {
          moveMouse(enDirection::Down);
        }
        else if (m_vControls[2].m_fValue > 0.5f) {
          moveMouse(enDirection::Left);
        }
        else if (m_vControls[3].m_fValue > 0.5f) {
          moveMouse(enDirection::Right);
        }
        else m_bMoved = false;
      }
      else {
        if (m_pSelected->getType() == gui::g_SelectorId) {
          if (m_vControls[2].m_fValue > 0.5f) {
            if (!m_bMoved) {
              m_bMoved = true;

              reinterpret_cast<gui::CSelector*>(m_pSelected)->setSelected(reinterpret_cast<gui::CSelector*>(m_pSelected)->getSelected() - 1);

              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
              l_cEvent.GUIEvent.Element   = m_pSelected;
              l_cEvent.GUIEvent.Caller    = m_pSelected;
              m_pDevice->postEventFromUser(l_cEvent);

            }
          }
          else if (m_vControls[3].m_fValue > 0.5f) {
            if (!m_bMoved) {
              m_bMoved = true;

              reinterpret_cast<gui::CSelector*>(m_pSelected)->setSelected(reinterpret_cast<gui::CSelector*>(m_pSelected)->getSelected() + 1);

              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
              l_cEvent.GUIEvent.Element   = m_pSelected;
              l_cEvent.GUIEvent.Caller    = m_pSelected;
              m_pSelected->OnEvent(l_cEvent);
            }
          }
          else if (m_vControls[0].m_fValue < 0.5f && m_vControls[1].m_fValue < 0.5f) {
            m_bMoved = false;
          }
        }
        else if (m_pSelected->getType() == irr::gui::EGUIET_SCROLL_BAR) {
          irr::gui::IGUIScrollBar *p = reinterpret_cast<irr::gui::IGUIScrollBar *>(m_pSelected);
          if (m_vControls[2].m_fValue > 0.5f) {
            if (!m_bMoved) {
              m_bMoved = true;

              p->setPos(p->getPos() - p->getSmallStep());

              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
              l_cEvent.GUIEvent.Element   = m_pSelected;
              l_cEvent.GUIEvent.Caller    = m_pSelected;
              m_pSelected->getParent()->OnEvent(l_cEvent);
            }
          }
          else if (m_vControls[3].m_fValue > 0.5f) {
            if (!m_bMoved) {
              m_bMoved = true;

              p->setPos(p->getPos() + p->getSmallStep());

              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_SCROLL_BAR_CHANGED;
              l_cEvent.GUIEvent.Element   = m_pSelected;
              l_cEvent.GUIEvent.Caller    = m_pSelected;
              m_pSelected->getParent()->OnEvent(l_cEvent);
            }
          }
          else if (m_vControls[2].m_fValue < 0.5f && m_vControls[3].m_fValue < 0.5f) {
            m_bMoved = false;
          }
        }
      }

      if (m_bButtonDown) {
        if (m_vControls[4].m_fValue < 0.5f) {
          m_bButtonDown = false;
          if (m_pHovered != nullptr) {
            bool l_bPost = true;

            switch (m_pHovered->getType()) {
              case gui::g_DustbinCheckboxId:
                break;

              case gui::g_SelectorId:
              case irr::gui::EGUIET_SCROLL_BAR:
                if (m_pSelected != nullptr) {
                  if (m_pSelected->getType() == gui::g_SelectorId) reinterpret_cast<gui::CSelector*>(m_pSelected)->setItemSelected(false);
                }

                m_pSelected = m_pHovered == m_pSelected ? nullptr : m_pHovered;

                if (m_pSelected != nullptr) {
                  if (m_pSelected->getType() == gui::g_SelectorId) reinterpret_cast<gui::CSelector*>(m_pSelected)->setItemSelected(true);
                  if (m_pSelected->getType() == irr::gui::EGUIET_SCROLL_BAR) l_bPost = false;
                }

                break;
            }

            if (l_bPost) {
              irr::SEvent l_cEvent;

              l_cEvent.EventType               = irr::EET_MOUSE_INPUT_EVENT;
              l_cEvent.MouseInput.Event        = irr::EMIE_LMOUSE_LEFT_UP;
              l_cEvent.MouseInput.Control      = false;
              l_cEvent.MouseInput.ButtonStates = 1;
              l_cEvent.MouseInput.Shift        = false;
              l_cEvent.MouseInput.Wheel        = 0;
              l_cEvent.MouseInput.X            = m_pCursor != nullptr ? m_pCursor->getPosition().X : m_pHovered != nullptr ? m_pHovered->getAbsoluteClippingRect().getCenter().X : 0;
              l_cEvent.MouseInput.Y            = m_pCursor != nullptr ? m_pCursor->getPosition().Y : m_pHovered != nullptr ? m_pHovered->getAbsoluteClippingRect().getCenter().Y : 0;

              m_pDevice->postEventFromUser(l_cEvent);
            }
          }
        }
      }
      else {
        if (m_vControls[4].m_fValue > 0.5f) {
          m_bButtonDown = true;

          if (m_pHovered != nullptr && m_pHovered->getType() != gui::g_DustbinCheckboxId && m_pHovered->getType() != irr::gui::EGUIET_SCROLL_BAR) {
            irr::SEvent l_cEvent;

            l_cEvent.EventType               = irr::EET_MOUSE_INPUT_EVENT;
            l_cEvent.MouseInput.Event        = irr::EMIE_LMOUSE_PRESSED_DOWN;
            l_cEvent.MouseInput.Control      = false;
            l_cEvent.MouseInput.ButtonStates = 1;
            l_cEvent.MouseInput.Shift        = false;
            l_cEvent.MouseInput.Wheel        = 0;
            l_cEvent.MouseInput.X            = m_pCursor != nullptr ? m_pCursor->getPosition().X : m_pHovered != nullptr ? m_pHovered->getAbsoluteClippingRect().getCenter().X : 0;
            l_cEvent.MouseInput.Y            = m_pCursor != nullptr ? m_pCursor->getPosition().Y : m_pHovered != nullptr ? m_pHovered->getAbsoluteClippingRect().getCenter().Y : 0;

            m_pDevice->postEventFromUser(l_cEvent);
          }
        }
      }

      if (m_vControls[5].m_fValue > 0.5f) {
        m_bOkDown = true;
      }
      else {
        if (m_bOkDown) {
          m_bOkDown = false;
          irr::gui::IGUIElement *p = m_pGui->getRootGUIElement()->getElementFromId(20000);

          if (p != nullptr) {
            irr::SEvent l_cEvent;

            l_cEvent.EventType          = irr::EET_GUI_EVENT;
            l_cEvent.GUIEvent.Caller    = p;
            l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

            CGlobal::getInstance()->OnEvent(l_cEvent);
          }
        }
      }

      if (m_vControls[6].m_fValue > 0.5f) {
        m_bCancelDown = true;
      }
      else {
        if (m_bCancelDown) {
          m_bCancelDown = false;

          irr::gui::IGUIElement *p = m_pGui->getRootGUIElement()->getElementFromId(20001);

          if (p != nullptr) {
            irr::SEvent l_cEvent;

            l_cEvent.EventType          = irr::EET_GUI_EVENT;
            l_cEvent.GUIEvent.Caller    = p;
            l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

            CGlobal::getInstance()->OnEvent(l_cEvent);
          }
        }
      }

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          m_pHovered  = a_cEvent.GUIEvent.Caller;
          m_pSelected = nullptr;
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          m_pHovered  = nullptr;
          m_pSelected = nullptr;
        }
      }

      return l_bRet;
    }

    bool CControllerMenu::isVisible(irr::gui::IGUIElement* a_pItem) {
      if (!a_pItem->isVisible())
        return false;

      if (a_pItem == m_pGui->getRootGUIElement())
        return true;
      else
        return isVisible(a_pItem->getParent());
    }

    irr::gui::IGUIElement* CControllerMenu::findElement(int a_iStep, enDirection a_eDirection) {
      if (m_pHovered == nullptr)
        if (m_vElements.size() > 0)
          return *m_vElements.begin();
        else
          return nullptr;

      irr::core::recti l_cRect;
      
      if (m_pHovered != nullptr)
        l_cRect = m_pHovered->getAbsoluteClippingRect();
      else {
        if (m_pCursor != nullptr)
          l_cRect = irr::core::recti(m_pCursor->getPosition() - irr::core::position2di(25, 25), m_pCursor->getPosition() + irr::core::position2di(25, 25));
        else
          l_cRect = irr::core::recti(0, 0, 50, 50);
      }
      

      if (a_iStep == 0) {
        switch (a_eDirection) {
          case enDirection::Up   : l_cRect = irr::core::recti(l_cRect.UpperLeftCorner .X       , l_cRect.UpperLeftCorner .Y - 5000, l_cRect.LowerRightCorner.X       , l_cRect.UpperLeftCorner .Y       ); break;
          case enDirection::Down : l_cRect = irr::core::recti(l_cRect.UpperLeftCorner .X       , l_cRect.LowerRightCorner.Y       , l_cRect.LowerRightCorner.X       , l_cRect.LowerRightCorner.Y + 5000); break;
          case enDirection::Left : l_cRect = irr::core::recti(l_cRect.UpperLeftCorner .X - 5000, l_cRect.UpperLeftCorner .Y       , l_cRect.UpperLeftCorner .X       , l_cRect.LowerRightCorner.Y       ); break;
          case enDirection::Right: l_cRect = irr::core::recti(l_cRect.LowerRightCorner.X       , l_cRect.UpperLeftCorner .Y       , l_cRect.LowerRightCorner.X + 5000, l_cRect.LowerRightCorner.Y       ); break;
        }
      }
      else {
        irr::core::dimension2du l_cScreen = CGlobal::getInstance()->getVideoDriver()->getScreenSize();

        switch (a_eDirection) {
          case enDirection::Up   : l_cRect = irr::core::recti(                         0,                          0, l_cScreen.Width            , l_cRect.UpperLeftCorner.Y); break;
          case enDirection::Down : l_cRect = irr::core::recti(                         0, l_cRect.LowerRightCorner.Y, l_cScreen.Width            , l_cScreen.Height         ); break;
          case enDirection::Left : l_cRect = irr::core::recti(                         0,                          0, l_cRect  .UpperLeftCorner.X, l_cScreen.Height         ); break;
          case enDirection::Right: l_cRect = irr::core::recti(l_cRect.LowerRightCorner.X,                          0, l_cScreen.Width            , l_cScreen.Height         ); break;
        }
      }

      irr::core::recti l_cHoverRect;
      
      if (m_pHovered != nullptr)
        l_cHoverRect = m_pHovered->getAbsoluteClippingRect();
      else {
        if (m_pCursor != nullptr)
          l_cHoverRect = irr::core::recti(m_pCursor->getPosition() - irr::core::position2di(25, 25), m_pCursor->getPosition() + irr::core::position2di(25, 25));
        else
          l_cHoverRect = irr::core::recti(0, 0, 50, 50);
      }

      irr::gui::IGUIElement* l_pNew = nullptr;

      int i = 0;
      irr::core::vector2di l_cDist = irr::core::vector2di(-1, -1);

      for (std::vector<irr::gui::IGUIElement*>::iterator it = m_vElements.begin(); it != m_vElements.end(); it++) {
        if (*it != m_pHovered) {
          if (isVisible(*it)) {
            if (l_cRect.isRectCollided((*it)->getAbsoluteClippingRect())) {
              irr::core::recti l_cOtherRect = (*it)->getAbsoluteClippingRect();

              irr::core::vector2di l_cNewDist = irr::core::vector2di(-1, -1);

              switch (a_eDirection) {
                case enDirection::Up   : l_cNewDist.X = l_cHoverRect.getCenter().Y - l_cOtherRect.getCenter().Y; l_cNewDist.Y = abs(l_cHoverRect.getCenter().X - l_cOtherRect.getCenter().X); break;
                case enDirection::Down : l_cNewDist.X = l_cOtherRect.getCenter().Y - l_cHoverRect.getCenter().Y; l_cNewDist.Y = abs(l_cOtherRect.getCenter().X - l_cHoverRect.getCenter().X); break;
                case enDirection::Left : l_cNewDist.X = l_cHoverRect.getCenter().X - l_cOtherRect.getCenter().X; l_cNewDist.Y = abs(l_cHoverRect.getCenter().Y - l_cOtherRect.getCenter().Y); break;
                case enDirection::Right: l_cNewDist.X = l_cOtherRect.getCenter().X - l_cHoverRect.getCenter().X; l_cNewDist.Y = abs(l_cOtherRect.getCenter().Y - l_cHoverRect.getCenter().Y); break;
              }

              if (l_pNew == nullptr || l_cNewDist.X < l_cDist.X || (l_cNewDist.X == l_cDist.X && l_cNewDist.Y < l_cDist.Y)) {
                l_pNew  = *it;
                l_cDist = l_cNewDist;
              }
            }
          }
        }
        i++;
      }

      m_bMoved = true;

      if (l_pNew != nullptr) {
        if (m_pCursor != nullptr) {
          m_pCursor->setPosition(l_pNew->getAbsoluteClippingRect().getCenter());
        }
        else {
          irr::SEvent l_cEvent;

          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
          l_cEvent.MouseInput.X = l_pNew->getAbsoluteClippingRect().getCenter().X;
          l_cEvent.MouseInput.Y = l_pNew->getAbsoluteClippingRect().getCenter().Y;
          l_cEvent.MouseInput.ButtonStates = 0;

          m_pDevice->postEventFromUser(l_cEvent);
        }
      }

      return l_pNew;
    }

    void CControllerMenu::moveMouse(CControllerMenu::enDirection a_eDirection) {
      if (m_bMoved)
        return;

      irr::gui::IGUIElement *l_pNew = findElement(0, a_eDirection);

      if (l_pNew == nullptr)
        l_pNew = findElement(1, a_eDirection);

      if (l_pNew != nullptr)
        m_pHovered = l_pNew;
    }

    /**
    * Get the Z-Layer of an item. Iterates through all ancestors until either a "MenuBackground" element or the root element
    * is found. If a "MenuBackground" is found it's "Z-Layer" property is returned, for the root element "0" is returned
    * @param a_pItem the item to get the Z-Layer
    * @return the Z-Layer of the item
    */
    int CControllerMenu::getElementZLayer(irr::gui::IGUIElement* a_pItem) {
      if (a_pItem->getType() == gui::g_MenuBackgroundId)
        return reinterpret_cast<gui::CMenuBackground*>(a_pItem)->getZLayer();
      else if (a_pItem == m_pGui->getRootGUIElement())
        return 0;
      else
        return getElementZLayer(a_pItem->getParent());
    }

    /**
    * Change the Z-Layer the controller controls
    * @param a_iZLayer the new Z-Layer
    */
    void CControllerMenu::setZLayer(int a_iZLayer) {
      m_iZLayer = a_iZLayer;
      m_vElements.clear();
      fillItemList(m_pGui->getRootGUIElement(), m_iZLayer);

      if (m_vElements.size() > 0) {
        irr::gui::IGUIElement *l_pHovered = *m_vElements.begin();

        for (std::vector<irr::gui::IGUIElement*>::iterator it = m_vElements.begin(); it != m_vElements.end(); it++) {
          if ((*it)->isTabStop())
            l_pHovered = *it;
        }

        m_pHovered = l_pHovered;

        if (m_pCursor != nullptr) {
          m_pCursor->setPosition(l_pHovered->getAbsoluteClippingRect().getCenter());
        }
        else {
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.X = l_pHovered->getAbsoluteClippingRect().getCenter().X;
          l_cEvent.MouseInput.Y = l_pHovered->getAbsoluteClippingRect().getCenter().Y;
          l_cEvent.MouseInput.ButtonStates = 0;
          m_pDevice->postEventFromUser(l_cEvent);
        }
      }
    }

    /**
    * Reset all necessary members as a new menu was loaded
    */
    void CControllerMenu::reset() {
      m_pHovered  = nullptr;
      m_pSelected = nullptr;
    }
  } // namespace controller
} // namespace dustbin