// (w) 2021 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <_generated/lua/lua_tables.h>
#include <messages/CMessageHelpers.h>
#include <gui/CMenuButton.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu() : m_pGui(CGlobal::getInstance()->getGuiEnvironment()), m_pCursor(CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()), m_iRaster(CGlobal::getInstance()->getRasterSize()), m_pHovered(nullptr), m_bEnterReady(true), m_bEventPosted(false) {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP   ; l_cInput.m_sName = "Up"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN ; l_cInput.m_sName = "Down"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT ; l_cInput.m_sName = "Left"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT; l_cInput.m_sName = "Right" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE; l_cInput.m_sName = "Enter" ; m_vControls.push_back(l_cInput);

      fillItemList(m_pGui->getRootGUIElement());

      if (m_vElements.size() > 0)
        m_pCursor->setPosition((*m_vElements.begin())->getAbsoluteClippingRect().getCenter());

      moveMouse(enDirection::Up);

      SSettings l_cSettings = CGlobal::getInstance()->getSettings();
      std::string l_sConfig = messages::urlDecode(l_cSettings.m_misc_menuctrl);

      if (l_sConfig != "") {
        irr::io::IReadFile* l_pFile = CGlobal::getInstance()->getFileSystem()->createMemoryReadFile(l_sConfig.c_str(), (irr::u32)l_sConfig.size(), "__controller_xml");
        if (l_pFile) {
          irr::io::IXMLReaderUTF8* l_pXml = CGlobal::getInstance()->getFileSystem()->createXMLReaderUTF8(l_pFile);
          if (l_pXml) {
            deserialize(l_pXml);
            l_pXml->drop();
          }
          l_pFile->drop();
        }
      }

      m_pTimer = CGlobal::getInstance()->getIrrlichtDevice()->getTimer();
      m_iLastMove = m_pTimer->getTime();
    }

    CControllerMenu::~CControllerMenu() {
    }

    /**
    * Fill the "m_vElements" vector
    * @param a_pParent the item to add, the children will be iterated and added as well
    */
    void CControllerMenu::fillItemList(irr::gui::IGUIElement* a_pParent) {
      irr::gui::EGUI_ELEMENT_TYPE l_eType = a_pParent->getType();

      switch (l_eType) {
        case irr::gui::EGUIET_BUTTON:
        case irr::gui::EGUIET_EDIT_BOX:
        case irr::gui::EGUIET_CHECK_BOX:
        case irr::gui::EGUIET_COMBO_BOX:
        case dustbin::gui::g_MenuButtonId:
          m_vElements.push_back(a_pParent);
          break;

        default:
          break;
      }

      for (irr::core::list<irr::gui::IGUIElement*>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++) {
        fillItemList(*it);
      }
    }

    /**
    * Event handler for this input
    * @param a_cEvent the event to handle
    */
    void CControllerMenu::update(const irr::SEvent& a_cEvent) {
      if (m_bEventPosted) return;

      CControllerBase::update(a_cEvent);

      if (m_vControls[0].m_fValue > 0.5f) {
        moveMouse(enDirection::Up);
      }

      if (m_vControls[1].m_fValue > 0.5f) {
        moveMouse(enDirection::Down);
      }

      if (m_vControls[2].m_fValue > 0.5f) {
        moveMouse(enDirection::Left);
      }

      if (m_vControls[3].m_fValue > 0.5f) {
        moveMouse(enDirection::Right);
      }

      if (m_bEnterReady) {
        if (m_vControls[4].m_fValue > 0.5f) {
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
          l_cEvent.MouseInput.Control = false;
          l_cEvent.MouseInput.Shift = false;
          l_cEvent.MouseInput.Wheel = 0;
          l_cEvent.MouseInput.X = m_pCursor->getPosition().X;
          l_cEvent.MouseInput.Y = m_pCursor->getPosition().Y;
          l_cEvent.MouseInput.ButtonStates = 1;
          m_bEnterReady = false;

          if (m_pHovered != nullptr) {
            m_bEventPosted = true;
            m_pGui->postEventFromUser(l_cEvent);
            m_pHovered->OnEvent(l_cEvent);
            m_bEventPosted = false;
          }
        }
      }
      else {
        if (m_vControls[4].m_fValue < 0.5f) {
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_MOUSE_INPUT_EVENT;
          l_cEvent.MouseInput.Event = irr::EMIE_LMOUSE_PRESSED_DOWN;
          l_cEvent.MouseInput.Control = false;
          l_cEvent.MouseInput.Shift = false;
          l_cEvent.MouseInput.Wheel = 0;
          l_cEvent.MouseInput.X = m_pCursor->getPosition().X;
          l_cEvent.MouseInput.Y = m_pCursor->getPosition().Y;
          l_cEvent.MouseInput.ButtonStates = 0;
          m_bEnterReady = true;

          /*if (m_pHovered != nullptr) {
            m_bEventPosted = true;
            m_pGui->postEventFromUser(l_cEvent);
            m_pHovered->OnEvent(l_cEvent);
            m_bEventPosted = false;
          }*/
        }
      }

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          m_pHovered = a_cEvent.GUIEvent.Caller;
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          m_pHovered = nullptr;
        }
      }
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
      if (m_pTimer != nullptr && m_pTimer->getTime() > m_iLastMove + 50) {
        irr::core::vector2di l_cPos = m_pCursor->getPosition();

        switch (a_eDirection) {
          case enDirection::Up   : l_cPos.Y -= m_iRaster; break;
          case enDirection::Down : l_cPos.Y += m_iRaster; break;
          case enDirection::Left : l_cPos.X -= m_iRaster; break;
          case enDirection::Right: l_cPos.X += m_iRaster; break;
        }

        m_pCursor->setPosition(l_cPos);
        m_iLastMove = m_pTimer->getTime();
      }
    }
  } // namespace controller
} // namespace dustbin