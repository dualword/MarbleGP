// (w) 2021 by Dustbin::Games / Christian Keimel
#include <controller/CControllerMenu.h>
#include <_generated/lua/lua_tables.h>
#include <messages/CMessageHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {

    CControllerMenu::CControllerMenu() : 
      m_pGui(CGlobal::getInstance()->getGuiEnvironment()), m_pCursor(CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl()), 
      m_pSelected(nullptr),
      m_bButtonDown(true),
      m_pHovered(nullptr),
      m_bMoved(false),
      m_iZLayer(-1)
    {
      SCtrlInput l_cInput;

      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_UP   ; l_cInput.m_sName = "Up"    ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_DOWN ; l_cInput.m_sName = "Down"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_LEFT ; l_cInput.m_sName = "Left"  ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_RIGHT; l_cInput.m_sName = "Right" ; m_vControls.push_back(l_cInput);
      l_cInput.m_eType = enInputType::Key; l_cInput.m_eKey = irr::KEY_SPACE; l_cInput.m_sName = "Enter" ; m_vControls.push_back(l_cInput);

      setZLayer(0);

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
          case irr::gui::EGUIET_CHECK_BOX:
          case irr::gui::EGUIET_COMBO_BOX:
          case irr::gui::EGUIET_SCROLL_BAR:
          case dustbin::gui::g_DustbinCheckboxId:
          case dustbin::gui::g_SelectorId:
          case dustbin::gui::g_MenuButtonId:
            printf("Ui element #%i found: \"%s\" (%i)\n", (int)m_vElements.size(), a_pParent->getName(), a_pParent->getID());
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
    void CControllerMenu::update(const irr::SEvent& a_cEvent) {
      CControllerBase::update(a_cEvent);

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
              reinterpret_cast<gui::CSelector*>(m_pSelected)->setSelected(reinterpret_cast<gui::CSelector*>(m_pSelected)->getSelected() - 1);
              m_bMoved = true;
            }
          }
          else if (m_vControls[3].m_fValue > 0.5f) {
            if (!m_bMoved) {
              reinterpret_cast<gui::CSelector*>(m_pSelected)->setSelected(reinterpret_cast<gui::CSelector*>(m_pSelected)->getSelected() + 1);
              m_bMoved = true;
            }
          }
          else if (m_vControls[0].m_fValue < 0.5f && m_vControls[1].m_fValue < 0.5f) {
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
                if (m_pSelected != nullptr)
                  reinterpret_cast<gui::CSelector*>(m_pSelected)->setItemSelected(false);

                m_pSelected = m_pHovered == m_pSelected ? nullptr : m_pHovered;

                if (m_pSelected != nullptr)
                  reinterpret_cast<gui::CSelector*>(m_pSelected)->setItemSelected(true);

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
              l_cEvent.MouseInput.X            = m_pCursor->getPosition().X;
              l_cEvent.MouseInput.Y            = m_pCursor->getPosition().Y;

              m_pHovered->OnEvent(l_cEvent);
            }
          }
        }
      }
      else {
        if (m_vControls[4].m_fValue > 0.5f) {
          m_bButtonDown = true;

          if (m_pHovered != nullptr && m_pHovered->getType() != gui::g_DustbinCheckboxId) {
            irr::SEvent l_cEvent;

            l_cEvent.EventType               = irr::EET_MOUSE_INPUT_EVENT;
            l_cEvent.MouseInput.Event        = irr::EMIE_LMOUSE_PRESSED_DOWN;
            l_cEvent.MouseInput.Control      = false;
            l_cEvent.MouseInput.ButtonStates = 1;
            l_cEvent.MouseInput.Shift        = false;
            l_cEvent.MouseInput.Wheel        = 0;
            l_cEvent.MouseInput.X            = m_pCursor->getPosition().X;
            l_cEvent.MouseInput.Y            = m_pCursor->getPosition().Y;

            m_pHovered->OnEvent(l_cEvent);
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

      irr::core::recti l_cRect = m_pHovered->getAbsoluteClippingRect();

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

      irr::core::recti l_cHoverRect = m_pHovered->getAbsoluteClippingRect();
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
        m_pCursor->setPosition(l_pNew->getAbsoluteClippingRect().getCenter());
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
      if (m_iZLayer >= 0 && m_pHovered != nullptr)
        m_mFocused[m_iZLayer] = m_pHovered;

      m_iZLayer = a_iZLayer;
      m_vElements.clear();
      fillItemList(m_pGui->getRootGUIElement(), m_iZLayer);

      if (m_mFocused.find(m_iZLayer) != m_mFocused.end()) {
        m_pHovered = m_mFocused[m_iZLayer];
        m_pCursor->setPosition(m_pHovered->getAbsoluteClippingRect().getCenter());
      }
      else {
        if (m_vElements.size() > 0)
          m_pCursor->setPosition((*m_vElements.begin())->getAbsoluteClippingRect().getCenter());

        moveMouse(enDirection::Up);
      }
    }
  } // namespace controller
} // namespace dustbin