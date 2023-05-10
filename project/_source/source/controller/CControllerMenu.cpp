// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <gui/IGuiMoveOptionElement.h>
#include <messages/CMessageHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <gui/CControllerUi_Menu.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CVirtualKeyboard.h>
#include <gui/CMenuBackground.h>
#include <gui/CGuiTrackSelect.h>
#include <gui/CReactiveLabel.h>
#include <gui/CControllerUi.h>
#include <gui/CGuiImageList.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <Defines.h>
#include <algorithm>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu() :
      m_bButtonDown   (false),
      m_bActive       (false),
      m_bEvent        (false),
      m_bCancelDown   (false),
      m_bOkDown       (false),
      m_bVisible      (true),
      m_bDebug        (false),
      m_iRaster       (CGlobal::getInstance()->getRasterSize()),
      m_pGui          (CGlobal::getInstance()->getGuiEnvironment()),
      m_pDrv          (CGlobal::getInstance()->getVideoDriver()),
      m_pCursor       (CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()),
      m_pDevice       (CGlobal::getInstance()->getIrrlichtDevice()),
      m_pTimer        (CGlobal::getInstance()->getIrrlichtDevice()->getTimer()),
      m_pSelected     (nullptr),
      m_sEditChars    (L""),
      m_iGamepad      (-1),
      m_bGamepad      (false),
      m_pCursorTexture(nullptr)
    {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP    ; l_cInput.m_sName = "Up"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN  ; l_cInput.m_sName = "Down"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT  ; l_cInput.m_sName = "Left"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT ; l_cInput.m_sName = "Right" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE ; l_cInput.m_sName = "Enter" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RETURN; l_cInput.m_sName = "Ok"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_ESCAPE; l_cInput.m_sName = "Cancel"; m_vControls.push_back(l_cInput);

      m_sEditChars = L" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:";

      m_pArrows[0] = m_pDrv->getTexture("data/images/arrow_up.png"  );
      m_pArrows[1] = m_pDrv->getTexture("data/images/arrow_down.png");

#ifdef _ANDROID
      m_pCursorTexture = m_pDrv->getTexture("data/images/cursor.png");
#endif

      m_cScreen = m_pDrv->getScreenSize();

      if (m_pCursor != nullptr)
        m_cMousePos = m_pCursor->getPosition();

      m_bDebug = CGlobal::getInstance()->getGlobal("debug_menucontroller") == "true";
    }

    CControllerMenu::~CControllerMenu() {
    }

    /**
    * Is this element really visible?
    * @param p the element to check
    * @return true if the element and all ancestors are visible
    */
    bool CControllerMenu::isElementVisible(irr::gui::IGUIElement* p) {
      if (p != m_pGui->getRootGUIElement()) {
        if (!p->isVisible())
          return false;

        return isElementVisible(p->getParent());
      }
      return true;
    }

    /**
    * Fill the vector with all supported editable UI elements
    * @param a_vElements the vector filled with the options
    * @param a_pThis the current UI element
    */
    void CControllerMenu::fillElementVector(std::vector<irr::gui::IGUIElement*>& a_vElements, irr::gui::IGUIElement* a_pThis) {
      // output is limited to:
      // - button
      // - checkbox
      // - edit field
      // - combobox
      // - scrollbar
      // - spinbox

      switch (a_pThis->getType()) {
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId:
        case irr::gui::EGUIET_BUTTON:
        case irr::gui::EGUIET_CHECK_BOX:
        case irr::gui::EGUIET_COMBO_BOX:
        case irr::gui::EGUIET_EDIT_BOX:
        case irr::gui::EGUIET_SCROLL_BAR:
        case irr::gui::EGUIET_SPIN_BOX:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ReactiveLabelId:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId:
        case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_TrackSelectId: {
          irr::gui::IGUIElement *l_pTest = m_pGui->getRootGUIElement()->getElementFromPoint(a_pThis->getAbsoluteClippingRect().getCenter());

          if (l_pTest == a_pThis)
            a_vElements.push_back(a_pThis);
          break;
        }

        default:
          break;
      }

      for (irr::core::list<irr::gui::IGUIElement *>::ConstIterator it = a_pThis->getChildren().begin(); it != a_pThis->getChildren().end(); it++)
        fillElementVector(a_vElements, *it);
    }

    /**
    * Find the best option for movement
    * @param a_eDirection
    * @return the GUI element that is the best option for the movement
    */
    irr::gui::IGUIElement *CControllerMenu::findBestOption(enDirection a_eDirection) {
      std::vector<irr::gui::IGUIElement *> l_vElements = findMoveOptions(a_eDirection);

      irr::gui::IGUIElement  *l_pRet   = nullptr;
      irr::core::position2di  l_cMouse = m_cMousePos;

      if (m_pCursor != nullptr)
        l_cMouse = m_pCursor->getPosition();

      irr::core::position2di l_cReference;

      irr::gui::IGUIElement *l_pHover = m_pGui->getRootGUIElement()->getElementFromPoint(l_cMouse);

      for (std::vector<irr::gui::IGUIElement*>::iterator it = l_vElements.begin(); it != l_vElements.end(); it++) {
        if (a_eDirection == enDirection::Up) {
          irr::core::position2di l_cPos = irr::core::position2di((*it)->getAbsoluteClippingRect().getCenter().X, (*it)->getAbsoluteClippingRect().UpperLeftCorner.Y);
          int l_iDiff = abs(l_cPos.Y - l_cMouse.Y);          

          if (l_cPos.Y < l_cMouse.Y && (l_pRet == nullptr || l_iDiff < abs(l_cReference.Y - l_cMouse.Y)) && *it != l_pHover && l_iDiff != 0) {
            l_pRet = *it;
            l_cReference = l_cPos;
          }
        }
        else if (a_eDirection == enDirection::Down) {
          irr::core::position2di l_cPos = irr::core::position2di((*it)->getAbsoluteClippingRect().getCenter().X, (*it)->getAbsoluteClippingRect().LowerRightCorner.Y);
          int l_iDiff = abs(l_cPos.Y - l_cMouse.Y);

          if (l_cPos.Y > l_cMouse.Y && (l_pRet == nullptr || l_iDiff < abs(l_cReference.Y - l_cMouse.Y)) && *it != l_pHover && l_iDiff != 0) {
            l_pRet = *it;
            l_cReference = l_cPos;
          }
        }
        else if (a_eDirection == enDirection::Left) {
          irr::core::position2di l_cPos = irr::core::position2di((*it)->getAbsoluteClippingRect().UpperLeftCorner.X, (*it)->getAbsoluteClippingRect().getCenter().Y);
          int l_iDiff = abs(l_cPos.X - l_cMouse.X);

          if (l_cPos.X < l_cMouse.X && (l_pRet == nullptr || l_iDiff < abs(l_cReference.X - l_cMouse.X)) && *it != l_pHover && l_iDiff != 0) {
            l_pRet = *it;
            l_cReference = l_cPos;
          }
        }
        else if (a_eDirection == enDirection::Right) {
          irr::core::position2di l_cPos = irr::core::position2di((*it)->getAbsoluteClippingRect().LowerRightCorner.X, (*it)->getAbsoluteClippingRect().getCenter().Y);
          int l_iDiff = abs(l_cPos.X - l_cMouse.X);

          if (l_cPos.X > l_cMouse.X && (l_pRet == nullptr || l_iDiff < abs(l_cReference.X - l_cMouse.X)) && *it != l_pHover && l_iDiff != 0) {
            l_pRet = *it;
            l_cReference = l_cPos;
          }
        }
      }

      // No good option found? Then we check if we can find a not so good yet valid option.
      if (l_pRet == nullptr) {
        std::vector<irr::gui::IGUIElement *> l_vNotSoGood = findMoveOptions(enDirection::Any);

        for (std::vector<irr::gui::IGUIElement*>::iterator it = l_vNotSoGood.begin(); it != l_vNotSoGood.end(); it++) {
          if (l_pRet == nullptr) {
            if ((a_eDirection == enDirection::Right && (*it)->getAbsoluteClippingRect().getCenter().X > l_cMouse.X) ||
                (a_eDirection == enDirection::Left  && (*it)->getAbsoluteClippingRect().getCenter().X < l_cMouse.X) ||
                (a_eDirection == enDirection::Down  && (*it)->getAbsoluteClippingRect().getCenter().Y > l_cMouse.Y) ||
                (a_eDirection == enDirection::Up    && (*it)->getAbsoluteClippingRect().getCenter().Y < l_cMouse.Y))
            {
              l_pRet = *it;
            }
          }
          else {
            int i1 = l_pRet->getAbsoluteClippingRect().getCenter().getDistanceFrom(l_cMouse);
            int i2 = (*it) ->getAbsoluteClippingRect().getCenter().getDistanceFrom(l_cMouse);

            if (i2 < i1) {
              if (a_eDirection == enDirection::Left) {
                if ((*it)->getAbsoluteClippingRect().getCenter().X < l_pRet->getAbsoluteClippingRect().getCenter().X)
                  l_pRet = *it;
              }
              else if (a_eDirection == enDirection::Right) {
                if ((*it)->getAbsoluteClippingRect().getCenter().X > l_pRet->getAbsoluteClippingRect().getCenter().X)
                  l_pRet = *it;
              }
              else if (a_eDirection == enDirection::Up) {
                if ((*it)->getAbsoluteClippingRect().getCenter().Y < l_pRet->getAbsoluteClippingRect().getCenter().Y)
                  l_pRet = *it;
              }
              else if (a_eDirection == enDirection::Down) {
                if ((*it)->getAbsoluteClippingRect().getCenter().Y > l_pRet->getAbsoluteClippingRect().getCenter().Y)
                  l_pRet = *it;
              }
            }
          }
        }
      }

      return l_pRet;
    }

    /**
    * Find the possible next UI elements when moving the mouse pointer
    * @param a_eDirection the direction to move to
    * @return a vector filled with the options
    */
    std::vector<irr::gui::IGUIElement *> CControllerMenu::findMoveOptions(enDirection a_eDirection) {
      std::vector<irr::gui::IGUIElement *> l_vElements;
      fillElementVector(l_vElements, m_pGui->getRootGUIElement());

      // First step: Search all elements in the same column or row depending on the direction
      irr::core::position2di l_cPos = m_cMousePos;

      if (m_pCursor != nullptr)
        l_cPos = m_pCursor->getPosition();

      irr::core::recti l_cRect = irr::core::recti(
        (a_eDirection == enDirection::Left || a_eDirection == enDirection::Right) ? 0               : l_cPos.X - 5,
        (a_eDirection == enDirection::Left || a_eDirection == enDirection::Right) ? l_cPos.Y - 5    : 0,
        (a_eDirection == enDirection::Left || a_eDirection == enDirection::Right) ? m_cScreen.Width : l_cPos.X + 5,
        (a_eDirection == enDirection::Left || a_eDirection == enDirection::Right) ? l_cPos.Y + 5    : m_cScreen.Height
      );

      // Second step: find the biggest possible element
      irr::gui::IGUIElement *l_pReference = nullptr;

      for (std::vector<irr::gui::IGUIElement*>::iterator it = l_vElements.begin(); it != l_vElements.end(); it++) {
        if ((*it)->getAbsoluteClippingRect().isRectCollided(l_cRect)) {
          if (a_eDirection == enDirection::Up || a_eDirection == enDirection::Down) {
            if (l_pReference == nullptr || (*it)->getAbsoluteClippingRect().getWidth() > l_pReference->getAbsoluteClippingRect().getWidth())
              l_pReference = *it;
          }
          else {
            if (l_pReference == nullptr || (*it)->getAbsoluteClippingRect().getHeight() > l_pReference->getAbsoluteClippingRect().getHeight())
              l_pReference = *it;
          }
        }
      }

      if (l_pReference != nullptr) {
        if (a_eDirection == enDirection::Up || a_eDirection == enDirection::Down) {
          l_cRect.UpperLeftCorner .X = l_pReference->getAbsoluteClippingRect().UpperLeftCorner .X;
          l_cRect.LowerRightCorner.X = l_pReference->getAbsoluteClippingRect().LowerRightCorner.X;
        }
        else {
          l_cRect.UpperLeftCorner .Y = l_pReference->getAbsoluteClippingRect().UpperLeftCorner .Y;
          l_cRect.LowerRightCorner.Y = l_pReference->getAbsoluteClippingRect().LowerRightCorner.Y;
        }
      }

      std::vector<irr::gui::IGUIElement *> a_vOptions;

      // Third step: Search for the element with the biggest outbox that meets the criteria
      for (std::vector<irr::gui::IGUIElement*>::iterator it = l_vElements.begin(); it != l_vElements.end(); it++) {
        if ((*it)->getAbsoluteClippingRect().isRectCollided(l_cRect)) {
          bool l_bAdd = false;

          if (a_eDirection == enDirection::Up) {
            l_bAdd = (*it)->getAbsoluteClippingRect().LowerRightCorner.Y < l_cPos.Y;
          }
          else if (a_eDirection == enDirection::Down) {
            l_bAdd = (*it)->getAbsoluteClippingRect().UpperLeftCorner.Y > l_cPos.Y;
          }
          else if (a_eDirection == enDirection::Left) {
            l_bAdd = (*it)->getAbsoluteClippingRect().LowerRightCorner.X < l_cPos.X;
          }
          else if (a_eDirection == enDirection::Right) {
            l_bAdd = (*it)->getAbsoluteClippingRect().UpperLeftCorner.X > l_cPos.X;
          }

          if (l_bAdd)
            a_vOptions.push_back(*it);
        }
        else if (a_eDirection == enDirection::Any) {
          a_vOptions.push_back(*it);
        }
      }

      return a_vOptions;
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    bool CControllerMenu::update(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;
      irr::gui::IGUIElement *l_pHovered = m_pGui->getRootGUIElement()->getElementFromPoint(m_cMousePos);

      if (m_bGamepad && m_iGamepad == -1) {
        if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
          bool l_bEvent = false;

          if (m_mOldAxisPos.find(a_cEvent.JoystickEvent.Joystick) == m_mOldAxisPos.end()) {
            m_mOldAxisPos[a_cEvent.JoystickEvent.Joystick] = SOldAxisPos();

            for (int i = 0; i < 18; i++)
              m_mOldAxisPos[a_cEvent.JoystickEvent.Joystick].m_iAxis[i] = a_cEvent.JoystickEvent.Axis[i];
          }

          if (a_cEvent.JoystickEvent.ButtonStates != 0)
            l_bEvent = true;
          else if (a_cEvent.JoystickEvent.POV != 65535)
            l_bEvent = true;
          else
            for (int i = 0; i < 18 && !l_bEvent; i++)
              l_bEvent = a_cEvent.JoystickEvent.Axis[i] != m_mOldAxisPos[a_cEvent.JoystickEvent.Joystick].m_iAxis[i];

          if (l_bEvent) {
            printf("Gamepad %i selected for menu control.\n", a_cEvent.JoystickEvent.Joystick);
            m_iGamepad = (irr::s32)a_cEvent.JoystickEvent.Joystick;

            for (std::vector<SCtrlInput>::iterator l_itCtrl = m_vControls.begin(); l_itCtrl != m_vControls.end(); l_itCtrl++) {
              (*l_itCtrl).m_iJoystick = m_iGamepad;
            }
          }
        }
      }

      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventSettingsChanged) {
          m_bActive = CGlobal::getInstance()->getSettingData().m_sMenuCtrl == "Keyboard" || CGlobal::getInstance()->getSettingData().m_sMenuCtrl == "Gamepad";
          deserialize(CGlobal::getInstance()->getSettingData().m_sController);

          for (std::vector<SCtrlInput>::iterator l_itCtrl = m_vControls.begin(); l_itCtrl != m_vControls.end() && !m_bGamepad; l_itCtrl++) {
            m_bGamepad = (*l_itCtrl).m_eType == enInputType::JoyAxis || (*l_itCtrl).m_eType == enInputType::JoyButton || (*l_itCtrl).m_eType == enInputType::JoyPov;
          }

          m_iGamepad = -1;
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMoveMouse) {
          enDirection l_eDirection = enDirection::Any;

          switch (a_cEvent.UserEvent.UserData2) {
          case 0: l_eDirection = enDirection::Up   ; break;
          case 1: l_eDirection = enDirection::Down ; break;
          case 2: l_eDirection = enDirection::Left ; break;
          case 3: l_eDirection = enDirection::Right; break;
          }

          if (l_eDirection != enDirection::Any) {
            moveMouse(l_eDirection);
            l_bRet = true;
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
          if (l_pHovered != nullptr) {
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
              l_bRet = true;

              if (l_pHovered->getType() == irr::gui::EGUIET_EDIT_BOX) {
                l_cEvent.EventType          = irr::EET_GUI_EVENT;
                l_cEvent.GUIEvent.EventType = irr::gui::EGET_ELEMENT_FOCUSED;
                l_cEvent.GUIEvent.Caller    = l_pHovered;
                l_cEvent.GUIEvent.Element   = l_pHovered;

                l_pHovered->OnEvent(l_cEvent);
              }
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
              l_bRet = true;
            }
          }
        }
      }

      if (!m_bActive)
        return false;

      if (a_cEvent.EventType != irr::EET_GUI_EVENT && !m_bEvent) {
        m_bEvent = true;

        l_bRet = CControllerBase::update(a_cEvent);

        for (int i = 0; i < 4; i++) {
          if (m_vControls[i].m_fValue > 0.5f) {
            if (m_pTimer->getTime() > m_aNextEvent[i]) {
              if (m_aNextEvent[i] == 0)
                m_aNextEvent[i] = m_pTimer->getTime() + 500;
              else
                m_aNextEvent[i] = m_pTimer->getTime() + 50;

              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMoveMouse;
              l_cEvent.UserEvent.UserData2 = i;

              if (l_pHovered != nullptr) 
                l_pHovered->OnEvent(l_cEvent);
              else
                m_pGui->getRootGUIElement()->OnEvent(l_cEvent);

              l_bRet = true;
            }
          }
          else m_aNextEvent[i] = 0;
        }

        if (m_bButtonDown) {
          if (m_vControls[4].m_fValue < 0.5f) {
            m_bButtonDown = false;
            if (l_pHovered != nullptr) {
              printf("Mouse Released.\n");
              irr::SEvent l_cEvent{};

              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMouseClicked;
              l_cEvent.UserEvent.UserData2 = 0;

              l_pHovered->OnEvent(l_cEvent);

              m_pSelected = l_pHovered;
              l_bRet = true;
            }
          }
        }
        else {
          if (m_vControls[4].m_fValue > 0.5f) {
            m_bButtonDown = true;

            irr::gui::IGUIElement *l_pHovered = m_pGui->getRootGUIElement()->getElementFromPoint(m_cMousePos);
            if (l_pHovered != nullptr) {
              printf("Mouse Pressed.\n");
              irr::SEvent l_cEvent{};

              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventMouseClicked;
              l_cEvent.UserEvent.UserData2 = 1;

              l_pHovered->OnEvent(l_cEvent);
              l_bRet = true;
            }
          }
        }

        if (m_vControls[5].m_fValue > 0.5f) {
          m_bOkDown = true;
        }
        else {
          if (m_bOkDown) {
            m_bOkDown = false;

            if (l_pHovered != nullptr) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventOkClicked;
              l_cEvent.UserEvent.UserData2 = c_iEventOkClicked;
              l_pHovered->OnEvent(l_cEvent);
            }
          }
        }

        if (m_vControls[6].m_fValue > 0.5f) {
          m_bCancelDown = true;
        }
        else {
          if (m_bCancelDown) {
            m_bCancelDown = false;

            if (l_pHovered != nullptr) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventCancelClicked;
              l_cEvent.UserEvent.UserData2 = c_iEventCancelClicked;
              l_pHovered->OnEvent(l_cEvent);
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
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED && m_pCursor == nullptr) {
          m_cMousePos.X = a_cEvent.MouseInput.X;
          m_cMousePos.Y = a_cEvent.MouseInput.Y;
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

    /**
    * Find the correct iterator for the current column
    */
    void CControllerMenu::findColumnIterator() {
      m_itCol = m_mCols.begin();

      int l_iDist = abs(m_cMousePos.X - m_itCol->first);

      for (std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator it = m_mCols.begin(); it != m_mCols.end(); it++) {
        int l_iNewDist = abs(m_cMousePos.X - it->first);
        if (l_iNewDist < l_iDist) {
          l_iDist = l_iNewDist;
          m_itCol = it;
        }
      }

      if (m_itCol == m_mCols.end())
        m_itCol--;
    }

    /**
    * Find the correct iterator for the current row
    */
    void CControllerMenu::findRowIterator() {
      m_itRow = m_mRows.begin();

      int l_iDist = abs(m_cMousePos.Y - m_itRow->first);

      for (std::map<int, std::vector<irr::gui::IGUIElement*>>::iterator it = m_mRows.begin(); it != m_mRows.end(); it++) {
        int l_iNewDist = abs(m_cMousePos.Y - it->first);
        if (l_iNewDist < l_iDist) {
          l_iDist = l_iNewDist;
          m_itRow = it;
        }
      }

      if (m_itRow == m_mRows.end())
        m_itRow--;
    }

    void CControllerMenu::moveMouse(CControllerMenu::enDirection a_eDirection) {
      irr::gui::IGUIElement  *l_pHover = m_pGui->getRootGUIElement()->getElementFromPoint(m_cMousePos);
      irr::core::position2di  l_cOption;
      bool                    l_bFound = false;

      if (l_pHover != nullptr) {
        if (l_pHover->getType() == gui::g_ImageListId || l_pHover->getType() == gui::g_TrackSelectId) {
          gui::CGuiImageList *l_pImg = static_cast<gui::CGuiImageList *>(l_pHover);
          if (l_pImg->getMoveOption(m_cMousePos, (int)a_eDirection, l_cOption)) {
            m_cMousePos = l_cOption;
            l_bFound = true;
          }
        }
      }

      irr::gui::IGUIElement *p = findBestOption(a_eDirection);

      if (p != nullptr || l_bFound) {
        if (!l_bFound) {
          if (p->getType() == gui::g_ImageListId || p->getType() == gui::g_TrackSelectId) {
            gui::CGuiImageList *l_pImg = static_cast<gui::CGuiImageList *>(p);
            if (l_pImg->getMoveOption(m_cMousePos, (int)a_eDirection, l_cOption))
              m_cMousePos = l_cOption;
            else
              m_cMousePos = p->getAbsoluteClippingRect().getCenter();
          }
          else m_cMousePos = p->getAbsoluteClippingRect().getCenter();
        }

        if (m_pCursor != nullptr)
          m_pCursor->setPosition(m_cMousePos);
        else {
          irr::SEvent l_cEvent{};
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_MOUSE_MOVED;
          l_cEvent.MouseInput.X = m_cMousePos.X;
          l_cEvent.MouseInput.Y = m_cMousePos.Y;
          m_pDevice->postEventFromUser(l_cEvent);
        }
      }
    }

    /**
    * Reset all necessary members as a new menu was loaded
    */
    void CControllerMenu::reset() {
      m_pSelected = nullptr;
    }

    /**
    * Set the cursor visibility (only relevant for Android)
    * @param a_bVisible the new visibility flag
    */
    void CControllerMenu::setVisible(bool a_bVisible) {
      m_bVisible = a_bVisible;
    }

    void CControllerMenu::draw() {
      if (m_pCursor != nullptr)
        m_cMousePos = m_pCursor->getPosition();

      if (m_pCursor == nullptr && m_bVisible && m_pCursorTexture != nullptr) {
        m_pDrv->draw2DImage(m_pCursorTexture, irr::core::recti(m_cMousePos, irr::core::dimension2du(2 * m_iRaster, 2 * m_iRaster)), irr::core::recti(irr::core::position2di(0, 0), m_pCursorTexture->getOriginalSize()), nullptr, nullptr, true);
      }

      if (m_bDebug) {
        std::map<enDirection, irr::video::SColor> l_mDirection = {
          { enDirection::Up   , irr::video::SColor(128,   0, 255,   0 ) },
          { enDirection::Left , irr::video::SColor(128,   0,   0, 255 ) },
          { enDirection::Down , irr::video::SColor(128, 255,   0,   0 ) },
          { enDirection::Right, irr::video::SColor(128, 255, 255,   0 ) }
        };


        for (std::map<enDirection, irr::video::SColor>::iterator mit = l_mDirection.begin(); mit != l_mDirection.end(); mit++) {
          std::vector<irr::gui::IGUIElement *> l_vElements = findMoveOptions(mit->first);

          for (std::vector<irr::gui::IGUIElement*>::iterator it = l_vElements.begin(); it != l_vElements.end(); it++) {
            m_pDrv->draw2DRectangle(mit->second, (*it)->getAbsoluteClippingRect());
          }
        }

        irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(enFont::Big, m_cScreen);

        std::map<enDirection, std::wstring> l_mBest = {
          { enDirection::Up   , L"Up"    },
          { enDirection::Left , L"Left"  },
          { enDirection::Right, L"Right" },
          { enDirection::Down , L"Down"  }
        };

        for (std::map<enDirection, std::wstring>::iterator mit = l_mBest.begin(); mit != l_mBest.end(); mit++) {
          irr::gui::IGUIElement *p = findBestOption(mit->first);

          if (p != nullptr) {
            l_pFont->draw(mit->second.c_str(), p->getAbsoluteClippingRect(), irr::video::SColor(0xFF, 0xFF, 0, 0), true, true);
          }
        }
      }
    }
  } // namespace controller
} // namespace dustbin