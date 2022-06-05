// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CVirtualKeyboard.h>
#include <gui/CMenuBackground.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiImageList.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <Defines.h>
#include <algorithm>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu(int a_iZLayer) :
      m_bButtonDown   (false),
      m_bActive       (false),
      m_bEvent        (false),
      m_bCancelDown   (false),
      m_bOkDown       (false),
      m_bAllowOkCancel(true),
      m_bVisible      (true),
      m_pGui          (CGlobal::getInstance()->getGuiEnvironment()),
      m_pDrv          (CGlobal::getInstance()->getVideoDriver()),
      m_pCursor       (CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()),
      m_pDevice       (CGlobal::getInstance()->getIrrlichtDevice()),
      m_pTimer        (CGlobal::getInstance()->getIrrlichtDevice()->getTimer()),
      m_iZLayer       (a_iZLayer),
      m_pSelected     (nullptr),
      m_sEditChars    (L""),
      m_iEditTime     (-1),
      m_bFirstCall    (true)
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

      m_sEditChars = L" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:";

      m_pArrows[0] = m_pDrv->getTexture("data/images/arrow_up.png"  );
      m_pArrows[1] = m_pDrv->getTexture("data/images/arrow_down.png");
    }

    CControllerMenu::~CControllerMenu() {
    }

    /**
    * Fill the "m_vElements" vector
    * @param a_pParent the item to add, the children will be iterated and added as well
    */
    void CControllerMenu::fillItemList(irr::gui::IGUIElement* a_pParent, int a_iZLayer) {
      irr::gui::EGUI_ELEMENT_TYPE l_eType = a_pParent->getType();

      int l_iZLayer = getElementZLayer(a_pParent);

      if ((l_iZLayer & a_iZLayer) == l_iZLayer) {
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
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_ReactiveLabelId:
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_DustbinCheckboxId:
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_SelectorId:
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_MenuButtonId:
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_ClipImageId:
          case (irr::gui::EGUI_ELEMENT_TYPE)dustbin::gui::g_ImageListId:
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

      for (int i = 0; i < 4; i++)
        m_aNextEvent[i] = 0;
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    bool CControllerMenu::update(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;
      irr::gui::IGUIElement *l_pHovered = m_pGui->getRootGUIElement()->getElementFromPoint(m_cMousePos);

      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventSettingsChanged) {
          m_bActive = CGlobal::getInstance()->getSettingData().m_bUseMenuCtrl;
          deserialize(CGlobal::getInstance()->getSettingData().m_sController);
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventChangeZLayer) {
          setZLayer((int)a_cEvent.UserEvent.UserData2);
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMoveMouse) {
          switch (a_cEvent.UserEvent.UserData2) {
            case 0: moveMouse(enDirection::Up   ); l_bRet = true; break;
            case 1: moveMouse(enDirection::Down ); l_bRet = true; break;
            case 2: moveMouse(enDirection::Left ); l_bRet = true; break;
            case 3: moveMouse(enDirection::Right); l_bRet = true; break;
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
          if (a_cEvent.UserEvent.UserData2 == 0) {
            irr::SEvent l_cEvent{};

            l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

            l_cEvent.MouseInput.Event        = irr::EMIE_LMOUSE_LEFT_UP;
            l_cEvent.MouseInput.Control      = false;
            l_cEvent.MouseInput.Shift        = false;
            l_cEvent.MouseInput.ButtonStates = 0;
            l_cEvent.MouseInput.Wheel        = 0.0f;
            l_cEvent.MouseInput.X            = m_cMousePos.X;
            l_cEvent.MouseInput.Y            = m_cMousePos.Y;

            l_pHovered->OnEvent(l_cEvent);
          }
          else {
            irr::SEvent l_cEvent{};

            l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;

            l_cEvent.MouseInput.Event        = irr::EMIE_LMOUSE_PRESSED_DOWN;
            l_cEvent.MouseInput.Control      = false;
            l_cEvent.MouseInput.Shift        = false;
            l_cEvent.MouseInput.ButtonStates = irr::EMBSM_LEFT;
            l_cEvent.MouseInput.Wheel        = 0.0f;
            l_cEvent.MouseInput.X            = m_cMousePos.X;
            l_cEvent.MouseInput.Y            = m_cMousePos.Y;

            l_pHovered->OnEvent(l_cEvent);
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT && a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
        m_cMousePos.X = a_cEvent.MouseInput.X;
        m_cMousePos.Y = a_cEvent.MouseInput.Y;
      }

      if (!m_bActive)
        return false;

      if (a_cEvent.EventType != irr::EET_GUI_EVENT && !m_bEvent) {
        m_bEvent = true;

        CControllerBase::update(a_cEvent);
        
        for (int i = 0; i < 4; i++) {
          if (m_vControls[i].m_fValue > 0.5f) {
            if (m_pTimer->getTime() > m_aNextEvent[i]) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMoveMouse;
              l_cEvent.UserEvent.UserData2 = i;

              if (l_pHovered != nullptr) 
                l_pHovered->OnEvent(l_cEvent);

              if (m_aNextEvent[i] == 0)
                m_aNextEvent[i] = m_pTimer->getTime() + 500;
              else
                m_aNextEvent[i] = m_pTimer->getTime() + 50;
            }
          }
          else m_aNextEvent[i] = 0;
        }
        
        if (m_bButtonDown) {
          if (m_vControls[4].m_fValue < 0.5f) {
            m_bButtonDown = false;
            if (l_pHovered != nullptr) {
              irr::SEvent l_cEvent{};

              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMouseClicked;
              l_cEvent.UserEvent.UserData2 = 0;

              l_pHovered->OnEvent(l_cEvent);

              m_pSelected = l_pHovered;
            }
          }
        }
        else {
          if (m_vControls[4].m_fValue > 0.5f) {
            m_bButtonDown = true;

            irr::gui::IGUIElement *l_pHovered = m_pGui->getRootGUIElement()->getElementFromPoint(m_cMousePos);
            if (l_pHovered != nullptr) {
              irr::SEvent l_cEvent{};

              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMouseClicked;
              l_cEvent.UserEvent.UserData2 = 1;

              l_pHovered->OnEvent(l_cEvent);
            }
          }
        }

        if (m_vControls[5].m_fValue > 0.5f) {
          if (m_bAllowOkCancel)
            m_bOkDown = true;
        }
        else {
          if (m_bOkDown) {
            m_bOkDown = false;

            if (m_bAllowOkCancel) {
              irr::gui::IGUIElement *p = m_pGui->getRootGUIElement()->getElementFromId(20000);

              if (p != nullptr) {
                irr::SEvent l_cEvent{};

                l_cEvent.EventType          = irr::EET_GUI_EVENT;
                l_cEvent.GUIEvent.Caller    = p;
                l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

                CGlobal::getInstance()->OnEvent(l_cEvent);
              }
            }
          }
        }

        if (m_vControls[6].m_fValue > 0.5f) {
          if (m_bAllowOkCancel)
            m_bCancelDown = true;
        }
        else {
          if (m_bCancelDown) {
            m_bCancelDown = false;

            if (m_bAllowOkCancel) {
              irr::gui::IGUIElement *p = m_pGui->getRootGUIElement()->getElementFromId(20001);

              if (p != nullptr) {
                irr::SEvent l_cEvent{};

                l_cEvent.EventType          = irr::EET_GUI_EVENT;
                l_cEvent.GUIEvent.Caller    = p;
                l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

                CGlobal::getInstance()->OnEvent(l_cEvent);
              }
            }
          }
        }
        m_bEvent = false;
      }

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          m_pSelected = nullptr;
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
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

    void CControllerMenu::moveMouse(CControllerMenu::enDirection a_eDirection) {
      irr::core::position2di l_cMouse = m_cMousePos;

      switch (a_eDirection) {
        case enDirection::Up: {
          l_cMouse.Y -= CGlobal::getInstance()->getRasterSize();
          break;
        }

        case enDirection::Down: {
          l_cMouse.Y += CGlobal::getInstance()->getRasterSize();
          break;
        }

        case enDirection::Left: {
          l_cMouse.X -= CGlobal::getInstance()->getRasterSize();
          break;
        }

        case enDirection::Right: {
          l_cMouse.X += CGlobal::getInstance()->getRasterSize();
          break;
        }
      }

      if (m_pCursor != nullptr)
        m_pCursor->setPosition(l_cMouse);
      else {
        irr::SEvent l_cEvent{};
        l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
        l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
        l_cEvent.MouseInput.X = l_cMouse.X;
        l_cEvent.MouseInput.Y = l_cMouse.Y;
        m_pDevice->postEventFromUser(l_cEvent);
      }
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

      for (std::vector<irr::gui::IGUIElement*>::iterator it = m_vElements.begin(); it != m_vElements.end(); it++) {
        irr::core::position2di l_cPos = (*it)->getAbsoluteClippingRect().getCenter();

        if (m_bFirstCall) {
          m_bFirstCall = false;
          m_cMousePos = l_cPos;

          irr::SEvent l_cEvent{};
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
          l_cEvent.MouseInput.X = m_cMousePos.X;
          l_cEvent.MouseInput.Y = m_cMousePos.Y;
          m_pDevice->postEventFromUser(l_cEvent);
        }
      }
      printf("Ready.\n");
    }

    /**
    * Reset all necessary members as a new menu was loaded
    */
    void CControllerMenu::reset() {
      m_pSelected = nullptr;
    }

    /**
    * Optionally deactivate the "ok" and "cancel" options
    * @param a_bAllow is it active or not?
    */
    void CControllerMenu::allowOkCancel(bool a_bAllow) {
      m_bAllowOkCancel = a_bAllow;
    }

    /**
    * Set the cursor visibility (only relevant for Android)
    * @param a_bVisible the new visibility flag
    */
    void CControllerMenu::setVisible(bool a_bVisible) {
      m_bVisible = a_bVisible;
    }

    void CControllerMenu::draw() {
      if (m_pCursor == nullptr && m_bVisible) {
        m_pGui->getVideoDriver()->draw2DRectangle(irr::video::SColor(0xFF,    0,    0,    0), irr::core::recti(m_cMousePos - irr::core::position2di(16, 16), irr::core::dimension2du(32, 32)));
        m_pGui->getVideoDriver()->draw2DRectangle(irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), irr::core::recti(m_cMousePos - irr::core::position2di(15, 15), irr::core::dimension2du(30, 30)));
      }
    }
  } // namespace controller
} // namespace dustbin