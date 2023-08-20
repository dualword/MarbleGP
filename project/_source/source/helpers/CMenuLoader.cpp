// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CControllerUi_Game.h>
#include <gui/CControllerUi_Menu.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiLogDisplay.h>
#include <gui/CGuiImageList.h>
#include <gui/CControllerUi.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>

#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace helpers {
    irr::gui::IGUIElement *parseElement(irr::io::IXMLReaderUTF8* a_pXml, irr::gui::IGUIElement* a_pParent, irr::gui::IGUIEnvironment *a_pGui);
    void loadMenuFromXML(const std::string& a_sFile, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui);

    std::map<std::string, std::string> parseAttributes(irr::io::IXMLReaderUTF8* a_pXml) {
      std::string l_sNodeName = a_pXml->getNodeName();
      std::map<std::string, std::string> l_mRet;

      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
          if (l_sName == "attribute") {
            std::string l_sKey   = a_pXml->getAttributeValueSafe("key"  ),
                        l_sValue = a_pXml->getAttributeValueSafe("value");

            if (l_sKey != "" && l_sValue != "")
              l_mRet[l_sKey] = l_sValue;
          }
        }
        else if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
          if (l_sNodeName == l_sName) {
            break;
          }
        }
      }

      return l_mRet;
    }

    void deserializeAttributes(irr::gui::IGUIElement* a_pElement, const std::map <std::string, std::string> &a_mAttributes, irr::io::IFileSystem *a_pFs) {
      irr::io::IAttributes *l_pAttributes = a_pFs->createEmptyAttributes();
      if (l_pAttributes && a_pElement != nullptr) {
        a_pElement->setMaxSize(irr::core::dimension2du(a_pElement->getAbsoluteClippingRect().getWidth(), a_pElement->getAbsoluteClippingRect().getHeight()));
        a_pElement->serializeAttributes(l_pAttributes);

        for (std::map<std::string, std::string>::const_iterator it = a_mAttributes.begin(); it != a_mAttributes.end(); it++) {
          switch (l_pAttributes->getAttributeType(it->first.c_str())) {
          case irr::io::EAT_COLOR: {
            std::vector<std::string> v = helpers::splitString(it->second, ',');

            irr::u32 a = v.size() > 0 ? std::atoi(v[0].c_str()) : 255, 
              r = v.size() > 1 ? std::atoi(v[1].c_str()) : 255,
              g = v.size() > 2 ? std::atoi(v[2].c_str()) : 255,
              b = v.size() > 3 ? std::atoi(v[3].c_str()) : 255;

            irr::video::SColor l_cColor = irr::video::SColor(a, r, g, b);
            l_pAttributes->setAttribute(it->first.c_str(), l_cColor);
            break;
          }

          case irr::io::EAT_BOOL:
            l_pAttributes->setAttribute(it->first.c_str(), it->second == "true" ? true : false);
            break;

          default:
            l_pAttributes->setAttribute(it->first.c_str(), it->second.c_str());
            break;
          }
        }

        a_pElement->deserializeAttributes(l_pAttributes);
        l_pAttributes->drop();
      }
    }

    void deserializeCustom(irr::gui::IGUIElement *a_pElement, const std::map<std::string, std::string> &a_mCustom, irr::gui::IGUIEnvironment *a_pGui) {
      irr::io::IAttributes *l_pAttributes = a_pGui->getFileSystem()->createEmptyAttributes();
      if (l_pAttributes) {
        a_pElement->serializeAttributes(l_pAttributes);

        if (a_pElement->getType() == irr::gui::EGUIET_IMAGE) {
          if (a_mCustom.find("src") != a_mCustom.end()) {
            irr::video::ITexture *l_pTexture = CGlobal::getInstance()->createTexture(a_mCustom.at("src"));
            reinterpret_cast<irr::gui::IGUIImage*>(a_pElement)->setImage(l_pTexture);
          }
        }

        if (a_pElement->getType() == gui::g_ClipImageId) {
          if (a_mCustom.find("src") != a_mCustom.end()) {
            reinterpret_cast<gui::CClipImage*>(a_pElement)->setImage(CGlobal::getInstance()->createTexture(a_mCustom.at("src")));
          }
        }

        if (a_pElement->getType() == irr::gui::EGUIET_COMBO_BOX) {
          if (a_mCustom.find("options") != a_mCustom.end()) {
            std::vector<std::string> l_vOptions = helpers::splitString(a_mCustom.at("options"), ';');
            for (std::vector<std::string>::iterator it = l_vOptions.begin(); it != l_vOptions.end(); it++) {
              reinterpret_cast<irr::gui::IGUIComboBox*>(a_pElement)->addItem(helpers::s2ws(*it).c_str());
            }
          }

          if (a_mCustom.find("selected") != a_mCustom.end()) {
            reinterpret_cast<irr::gui::IGUIComboBox*>(a_pElement)->setSelected(std::atoi(a_mCustom.at("selected").c_str()));
          }

          if (a_mCustom.find("textAlignment") != a_mCustom.end() || a_mCustom.find("vertialAlignment") != a_mCustom.end()) {
            irr::gui::EGUI_ALIGNMENT l_eAlign = irr::gui::EGUIA_UPPERLEFT,
              l_eVert = irr::gui::EGUIA_CENTER;

            if (a_mCustom.find("textAlignment") != a_mCustom.end()) {
              if (a_mCustom.at("textAlignment") == "right")
                l_eAlign = irr::gui::EGUIA_LOWERRIGHT;
              else if (a_mCustom.at("textAlignment") == "center")
                l_eAlign = irr::gui::EGUIA_CENTER;
            }

            if (a_mCustom.find("verticalAlignment") != a_mCustom.end()) {
              if (a_mCustom.at("vertialAlignment") == "top")
                l_eVert = irr::gui::EGUIA_UPPERLEFT;
              else if (a_mCustom.at("verticalAlignment") == "bottom")
                l_eVert = irr::gui::EGUIA_LOWERRIGHT;
            }

            reinterpret_cast<irr::gui::IGUIComboBox*>(a_pElement)->setTextAlignment(l_eAlign, l_eVert);
          }
        }

        if (a_pElement->getType() == gui::g_SelectorId) {
          if (a_mCustom.find("options") != a_mCustom.end()) {
            std::vector<std::string> l_vOptions = helpers::splitString(a_mCustom.at("options"), ';');
            for (std::vector<std::string>::iterator it = l_vOptions.begin(); it != l_vOptions.end(); it++) {
              reinterpret_cast<gui::CSelector*>(a_pElement)->addItem(helpers::s2ws(*it).c_str());
            }
          }
        }

        if (a_pElement->getType() == irr::gui::EGUIET_SPIN_BOX) {
          if (a_mCustom.find("textAlignment") != a_mCustom.end() || a_mCustom.find("vertialAlignment") != a_mCustom.end()) {
            irr::gui::EGUI_ALIGNMENT l_eAlign = irr::gui::EGUIA_UPPERLEFT,
              l_eVert  = irr::gui::EGUIA_CENTER;

            if (a_mCustom.find("textAlignment") != a_mCustom.end()) {
              if (a_mCustom.at("textAlignment") == "right")
                l_eAlign = irr::gui::EGUIA_LOWERRIGHT;
              else if (a_mCustom.at("textAlignment") == "center")
                l_eAlign = irr::gui::EGUIA_CENTER;
            }

            if (a_mCustom.find("verticalAlignment") != a_mCustom.end()) {
              if (a_mCustom.at("vertialAlignment") == "top")
                l_eVert = irr::gui::EGUIA_UPPERLEFT;
              else if (a_mCustom.at("verticalAlignment") == "bottom")
                l_eVert = irr::gui::EGUIA_LOWERRIGHT;
            }

            if (reinterpret_cast<irr::gui::IGUISpinBox*>(a_pElement)->getEditBox() != nullptr)
              reinterpret_cast<irr::gui::IGUISpinBox*>(a_pElement)->getEditBox()->setTextAlignment(l_eAlign, l_eVert);
          }
        }

        if (a_pElement->getType() == gui::g_ImageListId) {
          if (a_mCustom.find("rect") != a_mCustom.end()) {
            std::vector<std::string> l_vRect = splitString(a_mCustom.at("rect"), ',');

            if (l_vRect.size() >= 4) {
              irr::core::rectf l_cRect = irr::core::rectf(
                (irr::f32)std::atof(l_vRect[0].c_str()),
                (irr::f32)std::atof(l_vRect[1].c_str()),
                (irr::f32)std::atof(l_vRect[2].c_str()),
                (irr::f32)std::atof(l_vRect[3].c_str())
              );

              reinterpret_cast<gui::CGuiImageList *>(a_pElement)->setImageSourceRect(l_cRect);
            }
          }
        }

        l_pAttributes->drop();
      }    
    }

    irr::gui::IGUIElement* createElement(const std::string &a_sType, 
                                         const std::string &a_sRect, 
                                         const std::string &a_sFont,
                                         const std::string &a_sToolTip,
                                         irr::gui::IGUIElement *a_pParent,
                                         irr::gui::IGUIEnvironment *a_pGui) 
    {
      irr::gui::IGUIElement *l_pRet = a_pGui->addGUIElement(a_sType.c_str(), a_pParent);

      if (l_pRet) {
        if (a_sRect != "") {
          std::string l_sRect = "";
          irr::core::recti l_cRect;
          std::string l_sAnchor = "center";

          if (a_sRect.find(":") != std::string::npos) {
            l_sAnchor = a_sRect.substr(0, a_sRect.find(":"));
            l_sRect   = a_sRect.substr(a_sRect.find(":") + 1);
          }
          else {
            l_sAnchor = a_sRect;
            l_sRect   = "";
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
          else if (l_sAnchor == "relative")
            l_eLayout = enLayout::Relative;
          else if (l_sAnchor == "fillwindow")
            l_eLayout = enLayout::FillWindow;

          irr::core::recti l_cPos = CGlobal::getInstance()->getRect(l_cRect, l_eLayout, a_pParent != a_pGui->getRootGUIElement() ? a_pParent : nullptr);

          l_pRet->setRelativePosition(l_cPos);
        }

        l_pRet->setToolTipText(helpers::s2ws(a_sToolTip).c_str());

        switch (l_pRet->getType()) {
          case irr::gui::EGUIET_BUTTON:
          case irr::gui::EGUIET_STATIC_TEXT:
          case irr::gui::EGUIET_EDIT_BOX: 
          case irr::gui::EGUIET_SPIN_BOX:
          case irr::gui::EGUIET_TABLE:
          case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId: 
          case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ReactiveLabelId: 
          case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId: 
          case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiMenuId:
          case (irr::gui::EGUI_ELEMENT_TYPE)gui::g_GuiLogDisplayId: {
            enFont l_eFont = enFont::Regular;

            if (a_sFont == "tiny")
              l_eFont = enFont::Tiny;
            else if (a_sFont == "small")
              l_eFont = enFont::Small;
            else if (a_sFont == "big")
              l_eFont = enFont::Big;
            else if (a_sFont == "huge")
              l_eFont = enFont::Huge;

            if (l_eFont != enFont::Regular) {
              irr::gui::IGUIFont* l_pFont = CGlobal::getInstance()->getFont(l_eFont, a_pGui->getVideoDriver()->getScreenSize());
              if (l_pRet->getType() == irr::gui::EGUIET_BUTTON) {
                reinterpret_cast<irr::gui::IGUIButton*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_STATIC_TEXT) {
                reinterpret_cast<irr::gui::IGUIStaticText*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_EDIT_BOX) {
                reinterpret_cast<irr::gui::IGUIEditBox*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_SPIN_BOX && reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox() != nullptr) {
                reinterpret_cast<irr::gui::IGUISpinBox*>(l_pRet)->getEditBox()->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == gui::g_MenuButtonId) {
                reinterpret_cast<gui::CMenuButton*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == gui::g_ReactiveLabelId) {
                reinterpret_cast<gui::CReactiveLabel*>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == irr::gui::EGUIET_TABLE) {
                reinterpret_cast<irr::gui::IGUITable *>(l_pRet)->setOverrideFont(l_pFont);
              }
              else if (l_pRet->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_GuiLogDisplayId) {
                reinterpret_cast<gui::CGuiLogDisplay *>(l_pRet)->setFont(l_pFont);
              }
            }
            break;
          }

          default:
            // No override font available for most of the elements
            break;
        }
      }

      return l_pRet;
    }

    /**
    * Parse a <children> node
    * @param a_pXml the XML file to parse
    * @param a_pParent the parent node. If nullptr the parsed children are added to the returned list
    * @param a_pGui the Irrlicht GUI environment
    * @return a vector with the children
    */
    std::vector<irr::gui::IGUIElement*> parseChildren(irr::io::IXMLReaderUTF8* a_pXml, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui) {
      std::vector<irr::gui::IGUIElement*> l_vRet;

      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
          if (l_sName == "element") {
            irr::gui::IGUIElement *p = parseElement(a_pXml, a_pParent != nullptr ? a_pParent : a_pGui->getRootGUIElement(), a_pGui);
            if (p != nullptr) {
              if (a_pParent == nullptr)
                l_vRet.push_back(p);
            }
          }
          else if (l_sName == "include") {
            loadMenuFromXML(a_pXml->getAttributeValueSafe("file"), a_pParent, a_pGui);
          }
        }
        else if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
          if (l_sName == "children")
            break;
        }
      }

      return l_vRet;
    }

    /**
    * Parse an element and create it
    * @param a_pXml the XML file to read from
    * @param a_pParent the parent UI element
    * @param a_pGui the Irrlicht GUI environment
    * @return "true" on success
    */
    irr::gui::IGUIElement *parseElement(irr::io::IXMLReaderUTF8* a_pXml, irr::gui::IGUIElement* a_pParent, irr::gui::IGUIEnvironment *a_pGui) {
      std::string l_sType = a_pXml->getAttributeValueSafe("type"   );
      std::string l_sRect = a_pXml->getAttributeValueSafe("rect"   );
      std::string l_sFont = a_pXml->getAttributeValueSafe("font"   );
      std::string l_sTtip = a_pXml->getAttributeValueSafe("tooltip");

      irr::gui::IGUIElement *l_pElement = createElement(l_sType, l_sRect, l_sFont, l_sTtip, a_pParent, a_pGui);

      if (l_pElement != nullptr) {
        while (a_pXml->read()) {
          std::string l_sName = a_pXml->getNodeName();

          if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            if (l_sName == "attributes") {
              deserializeAttributes(l_pElement, parseAttributes(a_pXml), a_pGui->getFileSystem());
            }
            else if (l_sName == "custom") {
              deserializeCustom(l_pElement, parseAttributes(a_pXml), a_pGui);
            }
            else if (l_sName == "children") {
              parseChildren(a_pXml, l_pElement, a_pGui);
            }
          }
          else if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
            if (l_sName == "element") {
              break;
            }
          }
        }
      }
      else 
        printf("Unknown GUI type \"%s\"\n", l_sType.c_str());

      return l_pElement;
    }

    void parseDialog(irr::io::IXMLReaderUTF8* a_pXml, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment *a_pGui) {
      /*for (int i = 0; i < a_pGui->getRegisteredGUIElementFactoryCount(); i++) {
        irr::gui::IGUIElementFactory *l_pFactory = a_pGui->getGUIElementFactory(i);
        const irr::c8 *l_pName = l_pFactory->getCreateableGUIElementTypeName(irr::gui::EGUIET_TABLE);
        if (l_pName != nullptr) {
          printf("\nAttributes of \"%s\"\n\n", l_pName);

          irr::gui::IGUIElement *p = l_pFactory->addGUIElement(irr::gui::EGUIET_TABLE, a_pParent);
          irr::io::IAttributes *l_pAttr = a_pGui->getFileSystem()->createEmptyAttributes();

          p->serializeAttributes(l_pAttr);

          for (int j = 0; j < l_pAttr->getAttributeCount(); j++) {
            printf("Attribute %s: %s\n", l_pAttr->getAttributeName(j), l_pAttr->getAttributeAsString(j).c_str());
          }

          l_pAttr->drop();

          break;
        }
      }*/

      while (a_pXml->read()) {
        std::string l_sName = a_pXml->getNodeName();

        if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
          if (l_sName == "element") {
            parseElement(a_pXml, a_pParent != nullptr ? a_pParent : a_pGui->getRootGUIElement(), a_pGui);
          }
          else if (l_sName == "include")
            loadMenuFromXML(a_pXml->getAttributeValueSafe("file"), a_pParent, a_pGui);
        }
        else if (a_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
          if (l_sName == "dialog")
            break;
        }
      }
    }

    /**
    * This function loads a menu from an XML file to the GUI environment
    * @param a_sFile the XML file
    * @param a_pGui the GUI environment
    * @return "true" on success
    */
    void loadMenuFromXML(const std::string& a_sFile, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui) {
      printf("Include: \"%s\"\n", a_sFile.c_str());
      std::vector<irr::gui::IGUIElement *> l_vRet;

      irr::io::IXMLReaderUTF8 *l_pXml = a_pGui->getFileSystem()->createXMLReaderUTF8(a_sFile.c_str());

      if (l_pXml) {
        while (l_pXml->read()) {
          std::string l_sName = l_pXml->getNodeName();
          if (l_sName == "dialog" && l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            parseDialog(l_pXml, a_pParent != nullptr ? a_pParent : a_pGui->getRootGUIElement(), a_pGui);
          }
        }

        CGlobal::getInstance()->menuLoaded();
        l_pXml->drop();
      }
   }
  }
}