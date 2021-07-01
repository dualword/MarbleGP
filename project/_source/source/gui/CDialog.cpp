// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gui/CMenuButton.h>
#include <gui/CDialog.h>

#include <LuaBridge/LuaBridge.h>

namespace dustbin {
  namespace gui {
    std::vector<std::string> splitString(const std::string a_sInput, const char a_sDelimiter) {
      std::vector<std::string> l_vRet;

      std::string l_sInput = a_sInput;

      while (l_sInput.size() > 0) {
        size_t l_iPos = l_sInput.find_first_of(a_sDelimiter);

        if (l_iPos != std::string::npos) {
          l_vRet.push_back(l_sInput.substr(0, l_iPos));
          l_sInput = l_sInput.substr(l_iPos + 1);
        }
        else {
          l_vRet.push_back(l_sInput);
          l_sInput = "";
        }
      }

      return l_vRet;
    }

    CDialog::SDialogElement::SDialogElement(std::string a_sType, std::string a_sFontSize, dustbin::enLayout a_ePosition, const irr::core::recti& a_cRect) {
      m_sType     = a_sType;
      m_sFontSize = a_sFontSize;
      
      m_cPosition = std::make_tuple(a_ePosition, a_cRect);
    }

    void CDialog::SDialogElement::parse(irr::io::IXMLReaderUTF8* a_pXml, enParseState& a_eState) {
      // Before we continue to read the XML file we need to get the attributes of the "element" node
      if (a_eState == enParseState::element && std::string(a_pXml->getNodeName()) == "element") {
        // First discover the type of the element
        m_sType = a_pXml->getAttributeValueSafe("type");

        // Next get the fond size (if not defined we use "regular")
        m_sFontSize = a_pXml->getAttributeValueSafe("font");
        m_sToolTip  = a_pXml->getAttributeValueSafe("tooltip");

        if (m_sFontSize == "")
          m_sFontSize = "regular";

        // Then we read the rectangle definition..
        std::string l_sRect = a_pXml->getAttributeValueSafe("rect");

        //.. and parse it if set
        if (l_sRect != "") {
          irr::core::recti l_cRect;
          std::string l_sAnchor = "center";

          if (l_sRect.find(":") != std::string::npos) {
            l_sAnchor = l_sRect.substr(0, l_sRect.find(":"));
            l_sRect = l_sRect.substr(l_sRect.find(":") + 1);
          }

          int l_iIndex = 0;

          while (l_sRect.size() > 0) {
            std::size_t l_iPos = l_sRect.find(",");
            std::string s = "";

            if (l_iPos != std::string::npos) {
              s = l_sRect.substr(0, l_iPos);
              l_sRect = l_sRect.substr(l_iPos + 1);
            }
            else {
              s = l_sRect;
              l_sRect = "";
            }

            if (s != "") {
              switch (l_iIndex) {
              case 0: l_cRect.UpperLeftCorner.X = std::atoi(s.c_str()); l_iIndex++; break;
              case 1: l_cRect.UpperLeftCorner.Y = std::atoi(s.c_str()); l_iIndex++; break;
              case 2: l_cRect.LowerRightCorner.X = std::atoi(s.c_str()); l_iIndex++; break;
              case 3: l_cRect.LowerRightCorner.Y = std::atoi(s.c_str()); l_iIndex++; break;
              }
            }
          }

          enLayout l_eLayout = enLayout::Center;

          if (l_sAnchor == "upperleft")
            l_eLayout = enLayout::UpperLeft;
          else if (l_sAnchor == "uppermiddle")
            l_eLayout = enLayout::UpperMiddle;
          else if (l_sAnchor == "upperright")
            l_eLayout = enLayout::UpperRight;
          else if (l_sAnchor == "left")
            l_eLayout = enLayout::Left;
          else if (l_sAnchor == "center")
            l_eLayout = enLayout::Center;
          else if (l_sAnchor == "right")
            l_eLayout = enLayout::Right;
          else if (l_sAnchor == "lowerleft")
            l_eLayout = enLayout::LowerLeft;
          else if (l_sAnchor == "lowermiddle")
            l_eLayout = enLayout::LowerMiddle;
          else if (l_sAnchor == "lowerright")
            l_eLayout = enLayout::LowerRight;

          m_cPosition = std::make_tuple(l_eLayout, l_cRect);
        }
      }

      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        switch (a_eState) {
        case enParseState::dialog:
          if (l_sName == "element" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            a_eState = enParseState::element;
            m_vChildren.push_back(new SDialogElement(a_pXml, a_eState));
            a_eState = enParseState::dialog;
          }
          else if (l_sName == "dialog" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            a_eState = enParseState::root;
            return;
          }
          break;

        case enParseState::element:
          // Attributes found
          if (l_sName == "attributes" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            a_eState = enParseState::attributes;
          }
          // Custom attributes found
          else if (l_sName == "custom" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            a_eState = enParseState::custom;
          }
          // Children found
          else if (l_sName == "children" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            a_eState = enParseState::children;
          }
          // End of the element found
          else if (l_sName == "element" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            return;
          }
          break;

        case enParseState::custom:
        case enParseState::attributes:
          if (l_sName == "attribute" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            std::string l_sKey = a_pXml->getAttributeValueSafe("key");
            if (l_sKey != "") {
              if (a_eState == enParseState::attributes)
                m_mAttributes[l_sKey] = a_pXml->getAttributeValueSafe("value");
              else if (a_eState == enParseState::custom)
                m_mCustom[l_sKey] = a_pXml->getAttributeValueSafe("value");
            }
          }
          else if (l_sName == "attributes" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END && a_eState == enParseState::attributes) {
            a_eState = enParseState::element;
          }
          else if (l_sName == "custom" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END && a_eState == enParseState::custom) {
            a_eState = enParseState::element;
          }
          break;

        case enParseState::children:
          if (l_sName == "element" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            a_eState = enParseState::element;
            m_vChildren.push_back(new SDialogElement(a_pXml, a_eState));
            a_eState = enParseState::children;
          }
          else if (l_sName == "children" && a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            a_eState = enParseState::element;
          }
          break;
        }
      }
    }

    CDialog::SDialogElement::SDialogElement(irr::io::IXMLReaderUTF8* a_pXml, enParseState &a_eState) {
      parse(a_pXml, a_eState);
    }

    CDialog::SDialogElement::~SDialogElement() {
      for (std::vector<SDialogElement*>::iterator it = m_vChildren.begin(); it != m_vChildren.end(); it++) {
        delete* it;
      }
      m_vChildren.clear();
    }

    irr::gui::IGUIElement* CDialog::SDialogElement::createGuiElement(CGlobal* a_pGlobal, irr::gui::IGUIElement* a_pParent) {
      irr::gui::IGUIElement* l_pRet = nullptr;

      if (m_sType != "") {
        l_pRet = a_pGlobal->getGuiEnvironment()->addGUIElement(m_sType.c_str(), a_pParent != nullptr ? a_pParent : a_pGlobal->getGuiEnvironment()->getRootGUIElement());

        if (l_pRet != nullptr) {
          irr::io::IAttributes* l_pAttr = a_pGlobal->getFileSystem()->createEmptyAttributes(a_pGlobal->getVideoDriver());

          l_pRet->serializeAttributes(l_pAttr);

          for (std::map<std::string, std::string>::iterator it = m_mAttributes.begin(); it != m_mAttributes.end(); it++) {
            switch (l_pAttr->getAttributeType(it->first.c_str())) {
              case irr::io::EAT_COLOR: {
                std::vector<std::string> v = splitString(it->second, ',');

                irr::u32 a = v.size() > 0 ? std::atoi(v[0].c_str()) : 255, 
                         r = v.size() > 1 ? std::atoi(v[1].c_str()) : 255,
                         g = v.size() > 2 ? std::atoi(v[2].c_str()) : 255,
                         b = v.size() > 3 ? std::atoi(v[3].c_str()) : 255;

                irr::video::SColor l_cColor = irr::video::SColor(a, r, g, b);
                l_pAttr->setAttribute(it->first.c_str(), l_cColor);
                break;
              }

              case irr::io::EAT_BOOL:
                l_pAttr->setAttribute(it->first.c_str(), it->second == "true" ? true : false);
                break;

              default:
                l_pAttr->setAttribute(it->first.c_str(), it->second.c_str());
                break;
            }
          }

          if (l_pRet->getType() == irr::gui::EGUIET_COMBO_BOX) {
            for (unsigned i = 0; i < l_pAttr->getAttributeCount(); i++)
              printf("Attribute %s: %s\n", l_pAttr->getAttributeName(i), l_pAttr->getAttributeAsString(i).c_str());
          }

          irr::core::recti l_cRect = a_pGlobal->getRect(std::get<1>(m_cPosition), std::get<0>(m_cPosition), a_pParent);

          l_pRet->deserializeAttributes(l_pAttr);
          l_pRet->setRelativePosition(l_cRect);

          l_pAttr->clear();
          l_pAttr->drop();

          l_pRet->setToolTipText(std::wstring(m_sToolTip.begin(), m_sToolTip.end()).c_str());

          if (l_pRet->getType() == irr::gui::EGUIET_IMAGE) {
            if (m_mCustom.find("src") != m_mCustom.end()) {
              reinterpret_cast<irr::gui::IGUIImage*>(l_pRet)->setImage(CGlobal::getInstance()->createTexture(m_mCustom["src"]));
            }
          }

          if (l_pRet->getType() == irr::gui::EGUIET_COMBO_BOX) {
            if (m_mCustom.find("options") != m_mCustom.end()) {
              std::vector<std::string> l_vOptions = splitString(m_mCustom["options"], ';');
              for (std::vector<std::string>::iterator it = l_vOptions.begin(); it != l_vOptions.end(); it++) {
                reinterpret_cast<irr::gui::IGUIComboBox*>(l_pRet)->addItem(std::wstring((*it).begin(), (*it).end()).c_str());
              }
            }

            if (m_mCustom.find("selected") != m_mCustom.end()) {
              reinterpret_cast<irr::gui::IGUIComboBox*>(l_pRet)->setSelected(std::atoi(m_mCustom["selected"].c_str()));
            }
          }

          if (l_pRet->getType() == irr::gui::EGUIET_SPIN_BOX) {
            if (m_mCustom.find("textAlignment") != m_mCustom.end() || m_mCustom.find("vertialAlignment") != m_mCustom.end()) {
              irr::gui::EGUI_ALIGNMENT l_eAlign = irr::gui::EGUIA_UPPERLEFT,
                                       l_eVert  = irr::gui::EGUIA_CENTER;

              if (m_mCustom.find("textAlignment") != m_mCustom.end()) {
                if (m_mCustom["textAlignment"] == "right")
                  l_eAlign = irr::gui::EGUIA_LOWERRIGHT;
                else if (m_mCustom["textAlignment"] == "center")
                  l_eAlign = irr::gui::EGUIA_CENTER;
              }

              if (m_mCustom.find("verticalAlignment") != m_mCustom.end()) {
                if (m_mCustom["vertialAlignment"] == "top")
                  l_eVert = irr::gui::EGUIA_UPPERLEFT;
                else if (m_mCustom["verticalAlignment"] == "bottom")
                  l_eVert = irr::gui::EGUIA_LOWERRIGHT;
              }

              if (reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox() != nullptr)
                reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox()->setTextAlignment(l_eAlign, l_eVert);
            }
          }

          switch (l_pRet->getType()) {
            case irr::gui::EGUIET_BUTTON:
            case irr::gui::EGUIET_STATIC_TEXT:
            case irr::gui::EGUIET_EDIT_BOX: 
            case irr::gui::EGUIET_SPIN_BOX:
            case gui::g_MenuButtonId: {
              enFont l_eFont = enFont::Regular;

              if (m_sFontSize == "tiny")
                l_eFont = enFont::Tiny;
              else if (m_sFontSize == "small")
                l_eFont = enFont::Small;
              else if (m_sFontSize == "big")
                l_eFont = enFont::Big;
              else if (m_sFontSize == "huge")
                l_eFont = enFont::Huge;
                
              irr::gui::IGUIFont* l_pFont = a_pGlobal->getFont(l_eFont, a_pGlobal->getVideoDriver()->getScreenSize());
              if (l_pRet->getType() == irr::gui::EGUIET_BUTTON) {
                reinterpret_cast<irr::gui::IGUIButton*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_STATIC_TEXT) {
                reinterpret_cast<irr::gui::IGUIStaticText*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_EDIT_BOX) {
                reinterpret_cast<irr::gui::IGUIEditBox*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_SPIN_BOX && reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox() != nullptr)
                reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox()->setOverrideFont(l_pFont);
              else if (l_pRet->getType() == gui::g_MenuButtonId) {
                reinterpret_cast<gui::CMenuButton*>(l_pRet)->setOverrideFont(l_pFont);
              }
              break;
            }

            default:
              // No override font available for most of the elements
              break;
          }
        }
      }

      for (std::vector<SDialogElement*>::iterator it = m_vChildren.begin(); it != m_vChildren.end(); it++)
        (*it)->createGuiElement(a_pGlobal, l_pRet);
      
      return l_pRet;
    }


    CDialog::CDialog(lua_State* a_pState) : m_pGlobal(CGlobal::getInstance()), m_pGui(nullptr), m_pFs(nullptr) {
      m_pGui = m_pGlobal->getGuiEnvironment();
      m_pFs = m_pGlobal->getFileSystem();
      m_pDrv = m_pGlobal->getVideoDriver();

      m_pRoot = nullptr;

      if (a_pState != nullptr) {
        luabridge::getGlobalNamespace(a_pState)
          .beginClass<CDialog>("LuaDialog")
            .addFunction("loaddialog"     , &CDialog::loadDialog)
            .addFunction("createui"       , &CDialog::createUi)
            .addFunction("addlayoutraster", &CDialog::addLayoutRaster)
          .endClass();

        std::error_code l_cError;
        luabridge::push(a_pState, this, l_cError);
        lua_setglobal(a_pState, "dialog");
      }
    }

    void CDialog::loadDialog(const std::string& a_sFileName)  {
      if (m_pFs->existFile(a_sFileName.c_str())) {
        irr::io::IXMLReaderUTF8* l_pXml = m_pFs->createXMLReaderUTF8(a_sFileName.c_str());

        if (l_pXml) {
          irr::gui::IGUIElement* l_pParent = nullptr;
          enParseState l_eState = enParseState::root;

          while (l_pXml->read()) {
            std::string l_sName = l_pXml->getNodeName();

            switch (l_eState) {
              case enParseState::root:
                if (l_sName == "dialog" && l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                  l_eState = enParseState::dialog; 
                  if (m_pRoot == nullptr)
                    m_pRoot = new SDialogElement(l_pXml, l_eState);
                  else
                    m_pRoot->parse(l_pXml, l_eState);
                }
                break;
            }
          }
        }
        else {
          printf("Could not open file \"%s\"\n", a_sFileName.c_str());
        }
      }
    }

    CDialog::~CDialog() {
      if (m_pRoot != nullptr)
        delete m_pRoot;
    }

    void CDialog::createUi() {
      if (m_pRoot != nullptr)
        m_pRoot->createGuiElement(CGlobal::getInstance(), nullptr);
    }

    void CDialog::clear() {
      m_pGui->clear();
    }

    void CDialog::addLayoutRaster() {
      for (int y = -20; y <= 20; y++) {
        for (int x = -30; x <= 30; x++) {
          std::wstring s = L"";

          if (x == 0) {
            s = std::to_wstring(y);
          }
          else if (y == 0) {
            s = std::to_wstring(x);
          }
          else if (x == 0 && y == 0) {
            s = L"X";
          }

          for (int i = 0; i < 9; i++) {
            bool l_bAdd = false;
            irr::core::recti l_cRect;

            if (i == 0) {
              l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Center);
              l_bAdd = true;
            }
            else if (i == 1) {
              if (x >= 0 && x < 5 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperLeft);
                l_bAdd = true;
              }
            }
            else if (i == 2) {
              if (x >= -20 && x <= 20 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperMiddle);
                l_bAdd = true;
              }
            }
            else if (i == 3) {
              if (x > -5 && x <= 0 && y >= 0 && y < 5) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::UpperRight);
                l_bAdd = true;
              }
            }
            else if (i == 4) {
              if (x >= 0 && x < 5 && y >= -10 && y <= 10) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Left);
                l_bAdd = true;
              }
            }
            else if (i == 5) {
              if (x > -5 && x <= 0 && y >= -10 && y <= 10) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::Right);
                l_bAdd = true;
              }
            }
            else if (i == 6) {
              if (x >= 0 && x < 5 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerLeft);
                l_bAdd = true;
              }
            }
            else if (i == 7) {
              if (x >= -10 && x <= 10 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerMiddle);
                l_bAdd = true;
              }
            }
            else if (i == 8) {
              if (x > -5 && x <= 0 && y > -5 && y <= 0) {
                l_cRect = m_pGlobal->getRect(x, y, x + 1, y + 1, enLayout::LowerRight);
                l_bAdd = true;
              }
            }

            if (l_bAdd) {
              irr::gui::IGUIStaticText* p = m_pGui->addStaticText(s.c_str(), l_cRect, true, true, nullptr, -1, true);
              p->setOverrideFont(m_pGlobal->getFont(enFont::Tiny, m_pDrv->getScreenSize()));
              p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            }
          }
        }
      }
    }
  }
}