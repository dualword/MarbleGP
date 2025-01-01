// (w) 2024 by Dustbin::Games / Christian Keimel
#include <platform/CTrackEdPlatform.h>
#include <gui/CGuiScrollContainer.h>
#include <commands/CCmdDecoration.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    CCmdDecoration::CCmdDecoration(irr::scene::ISceneManager *a_pSmgr, irr::gui::IGUIEnvironment *a_pGui) :
      ICommand   ("CmdDecoration"),
      m_bFinished(false),
      m_pSmgr    (a_pSmgr),
      m_pGui     (a_pGui),
      m_pNode    (nullptr),
      m_pScene   (nullptr),
      m_pTabMain (nullptr),
      m_pTabNode (nullptr),
      m_pMaterial(nullptr)
    {
      m_pNode = getDecorationRoot(m_pSmgr->getRootSceneNode());

      if (m_pNode == nullptr) {
        m_pNode = m_pSmgr->addEmptySceneNode(m_pSmgr->getRootSceneNode());
        m_pNode->setName("__DecorationRootNode");
      }

      onUiSetupModified();
    }

    CCmdDecoration::~CCmdDecoration() {
      if (m_pDialog != nullptr) {
        m_pDialog->setVisible(false);
        m_pGui->addToDeletionQueue(m_pDialog);
      }
    }

    /**
    * Find the decoration scene node
    * @param a_pNode the node to check (all children will be checked as well)
    * @return the decoration scene node
    */
    irr::scene::ISceneNode *CCmdDecoration::getDecorationRoot(irr::scene::ISceneNode *a_pNode) {
      return helpers::getDecorationRootNode(m_pSmgr->getRootSceneNode());
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdDecoration::isFinished() {
      return m_bFinished;
    }

    /**
    * Get the type name of a scene node
    * @param a_pNode the scene node
    * @return the type name
    */
    std::string CCmdDecoration::getTypeName(irr::scene::ISceneNode *a_pNode) {
      irr::u32 l_iIndex = 0;
      while (true) {
        irr::scene::ISceneNodeFactory* l_pFactory = m_pSmgr->getSceneNodeFactory(l_iIndex++);
        if (l_pFactory != nullptr) {
          const irr::c8* l_sType = l_pFactory->getCreateableSceneNodeTypeName(a_pNode->getType());
          if (l_sType != nullptr) {
            return "(" + std::string(l_sType) + ")";
          }
        }
        else break;
      }

      return "";
    }

    /**
    * Update the list of decoration objects
    * @param a_pNode the node to add
    * @param a_pParent the parent tree view node
    */
    void CCmdDecoration::updateDecorationList(irr::scene::ISceneNode *a_pNode, irr::gui::IGUITreeViewNode *a_pParent) {
      std::string l_sName = a_pNode->getName();
      if (l_sName == "") {
        l_sName = getTypeName(a_pNode);
      }
      
      irr::gui::IGUITreeViewNode *l_pNew = a_pParent->addChildBack(a_pNode == m_pNode ? L"Root" : helpers::s2ws(l_sName).c_str());

      m_mNodeMap[l_pNew] = a_pNode;

      for (auto l_pChild: a_pNode->getChildren()) {
        updateDecorationList(l_pChild, l_pNew);
      }
    }

    /**
    * Callback method for changes in the UI like modifying
    * font size or resizing window
    */
    void CCmdDecoration::onUiSetupModified() {
      if (m_pDialog != nullptr) {
        m_pDialog->setVisible(false);
        m_pGui->addToDeletionQueue(m_pDialog);
        m_pDialog = nullptr;
      }

      irr::core::dimension2du l_cSize   = m_pGui->getSkin()->getFont()->getDimension(L"ThisTextShouldReallyBeLongEnoughInMyOpinionButIMayBeWrongAsWell");
      irr::core::dimension2du l_cScreen = m_pSmgr->getVideoDriver()->getScreenSize();

      irr::core::recti l_cDialog = irr::core::recti(
        irr::core::vector2di(l_cScreen.Width / 2 - l_cSize.Width / 2, l_cScreen.Height / 2 - 20 * l_cSize.Height / 2),
        irr::core::dimension2du(l_cSize.Width, 30 * l_cSize.Height)
      );

      irr::u32 l_iBtnHeight = 5 * l_cSize.Height / 4;

      m_pDialog = m_pGui->addWindow(l_cDialog, false, L"Edit Decoration");

      irr::s32 x = (irr::s32)(std::atoi(tracked::IGlobal::getInstance()->getSetting("decoration_x").c_str()));
      irr::s32 y = (irr::s32)(std::atoi(tracked::IGlobal::getInstance()->getSetting("decoration_y").c_str()));

      m_pDialog->setRelativePosition(irr::core::position2di(x, y));

      irr::core::recti l_cTabRect = irr::core::recti(
        0,
        5 * m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH) / 4,
        l_cDialog.getWidth(),
        l_cDialog.getHeight() - 5 * m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH) / 4
      );

      m_pTabMain = m_pGui->addTabControl(l_cTabRect, m_pDialog);

      m_aTabs[0] = m_pGui->addTab(m_pTabMain->getRelativePosition(), m_pTabMain);
      m_aTabs[0]->setText(L"Scene Tree");
      m_pTabMain->addTab(m_aTabs[0]);

      m_aTabs[1] = m_pGui->addTab(l_cTabRect, m_pTabMain);
      m_aTabs[1]->setText(L"Node Properties");
      m_pTabMain->addTab(m_aTabs[1]);

      l_cTabRect = irr::core::recti(
        0,
        0,
        l_cTabRect.getWidth(),
        l_cTabRect.getHeight() - 5 * m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH) / 4
      );

      m_pTabNode = m_pGui->addTabControl(l_cTabRect, m_aTabs[1]);
      m_pTabNode->setTabVerticalAlignment(irr::gui::EGUIA_LOWERRIGHT);

      irr::core::recti l_cParamRect = irr::core::recti(
        0,
        3 * l_iBtnHeight / 2,
        l_cTabRect.getWidth() - 10,
        l_cTabRect.getHeight() - 5 - 2 * m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH)
      );

      m_aNodeTabs[0] = m_pGui->addTab(m_pTabNode->getRelativePosition(), m_pTabNode);
      m_aNodeTabs[0]->setText(L"Properties");
      m_pTabNode->addTab(m_aNodeTabs[0]);

      m_aParameters[0] = new gui::CGuiScrollContainer(m_pGui, m_aNodeTabs[0], l_cParamRect);

      m_aNodeTabs[1] = m_pGui->addTab(m_pTabNode->getRelativePosition(), m_pTabNode);
      m_aNodeTabs[1]->setText(L"Materials");
      m_pTabNode->addTab(m_aNodeTabs[1]);

      m_aParameters[1] = new gui::CGuiScrollContainer(m_pGui, m_aNodeTabs[1], l_cParamRect);

      m_aNodeTabs[2] = m_pGui->addTab(m_pTabNode->getRelativePosition(), m_pTabNode);
      m_aNodeTabs[2]->setText(L"Animators");
      m_pTabNode->addTab(m_aNodeTabs[2]);

      m_aParameters[2] = new gui::CGuiScrollContainer(m_pGui, m_aNodeTabs[2], l_cParamRect);

      m_pScene = m_pGui->addTreeView(m_aTabs[0]->getRelativePosition(), m_aTabs[0]);

      m_mNodeMap.clear();
      updateDecorationList(m_pNode, m_pScene->getRoot());
    }

    /**
    * Update the attributes with the selected node
    * @param a_pAttr the attributes to use
    * @param a_iIndex the tab index (0 == properties, 1 == materials, 2 == animators)
    * @param a_pParent the parent UI element to add the attributes to
    */
    void CCmdDecoration::updateAttributeList(irr::io::IAttributes *a_pAttr, int a_iIndex, irr::gui::IGUIElement *a_pParent) {
      for (auto l_tAttribute: m_vAttributes[a_iIndex]) {
        std::get<1>(l_tAttribute)->setVisible(false);
        std::get<2>(l_tAttribute)->setVisible(false);

        m_pGui->addToDeletionQueue(std::get<1>(l_tAttribute));
        m_pGui->addToDeletionQueue(std::get<2>(l_tAttribute));
      }

      m_vAttributes[a_iIndex].clear();

      if (a_pAttr != nullptr) {
        irr::core::dimension2du l_cSize;

        for (irr::u32 i = 0; i < a_pAttr->getAttributeCount(); i++) {
          std::string l_sName = a_pAttr->getAttributeName(i);

          irr::core::dimension2du l_cAttributeSize = m_pGui->getSkin()->getFont()->getDimension(helpers::s2ws(l_sName).c_str());

          if (l_cAttributeSize.Width  > l_cSize.Width ) l_cSize.Width  = l_cAttributeSize.Width;
          if (l_cAttributeSize.Height > l_cSize.Height) l_cSize.Height = l_cAttributeSize.Height;
        }

        irr::core::position2di  l_cPos = irr::core::position2di(l_cSize.Height / 2, 0);

        for (irr::u32 i = 0; i < a_pAttr->getAttributeCount(); i++) {
          std::wstring l_sName = helpers::s2ws(a_pAttr->getAttributeName(i));

          irr::gui::IGUIStaticText *p = m_pGui->addStaticText(l_sName.c_str(), irr::core::recti(l_cPos, l_cSize), false, false, a_pParent);
          p->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);

          irr::gui::IGUIElement *l_pEdit = nullptr;

          irr::core::position2di  l_cEditPos  = l_cPos + irr::core::position2di(l_cPos.X + l_cSize.Width, 0);
          irr::core::dimension2du l_cEditSize = irr::core::dimension2du(a_pParent->getRelativePosition().getWidth() - l_cSize.Width - 2 * l_cSize.Height, l_cSize.Height);
          irr::core::recti        l_cEditRect = irr::core::recti(l_cEditPos, l_cEditSize);

          switch (a_pAttr->getAttributeType(i)) {
	          case irr::io::EAT_INT: { 
              l_pEdit = m_pGui->addSpinBox(std::to_wstring(a_pAttr->getAttributeAsInt(i)).c_str(), l_cEditRect, true, a_pParent);
              break; 
            }

	          case irr::io::EAT_FLOAT: { 
              l_pEdit = m_pGui->addEditBox(std::to_wstring(a_pAttr->getAttributeAsFloat(i)).c_str(), l_cEditRect, true, a_pParent);
              break; 
            }

	          case irr::io::EAT_STRING: { 
              l_pEdit = m_pGui->addEditBox(helpers::s2ws(a_pAttr->getAttributeAsString(i).c_str()).c_str(), l_cEditRect, true, a_pParent);
              break; 
            }

	          case irr::io::EAT_BOOL: { 
              l_pEdit = m_pGui->addCheckBox(a_pAttr->getAttributeAsBool(i), irr::core::recti(l_cEditPos, irr::core::dimension2du(l_cEditSize.Height, l_cEditSize.Height)), a_pParent);
              break; 
            }
	          case irr::io::EAT_ENUM: { 
              irr::gui::IGUIComboBox *l_pCombo = m_pGui->addComboBox(l_cEditRect, a_pParent);

              irr::scene::quake3::tStringList l_cAttributes;

              a_pAttr->getAttributeEnumerationLiteralsOfEnumeration(i, l_cAttributes);
              std::string l_sSelected = a_pAttr->getAttributeAsEnumeration(i);

              for (irr::u32 i = 0; i < l_cAttributes.size(); i++) {
                l_pCombo->addItem(helpers::s2ws(l_cAttributes[i].c_str()).c_str(), i);
              }

              l_pEdit = l_pCombo;
              break; 
            }

	          case irr::io::EAT_COLOR: { break; }
	          case irr::io::EAT_COLORF: { break; }
	          case irr::io::EAT_VECTOR3D: { 
              irr::core::vector3df l_cValue = a_pAttr->getAttributeAsVector3d(i);
              std::wstring l_sText = std::to_wstring(l_cValue.X) + L", " + std::to_wstring(l_cValue.Y) + L", " + std::to_wstring(l_cValue.Z);
              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
	          case irr::io::EAT_POSITION2D: { break; }
	          case irr::io::EAT_VECTOR2D: { break; }
	          case irr::io::EAT_RECT: { break; }
	          case irr::io::EAT_MATRIX: { break; }
	          case irr::io::EAT_QUATERNION: { break; }
	          case irr::io::EAT_BBOX: { break; }
	          case irr::io::EAT_PLANE: { break; }
	          case irr::io::EAT_TRIANGLE3D: { break; }
	          case irr::io::EAT_LINE2D: { break; }
	          case irr::io::EAT_LINE3D: { break; }
	          case irr::io::EAT_STRINGWARRAY: { break; }
	          case irr::io::EAT_FLOATARRAY: { break; }
	          case irr::io::EAT_INTARRAY: { break; }
	          case irr::io::EAT_BINARY: { break; }
	          case irr::io::EAT_TEXTURE: { 
              std::wstring l_sText = helpers::s2ws(a_pAttr->getAttributeAsString(i).c_str());

              wchar_t l_sDelim[] = { L'\\', L'/', L'\0' };

              for (int i = 0; l_sDelim[i] != L'\0'; i++) {
                if (l_sText.find_last_of(l_sDelim[i]) != std::string::npos) {
                  l_sText = l_sText.substr(l_sText.find_last_of(l_sDelim[i]) + 1);
                  break;
                }
              }

              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
	          case irr::io::EAT_USER_POINTER: { break; }
            case irr::io::EAT_DIMENSION2D: { break; }
          }

          l_cPos.Y += 5 * l_cSize.Height / 4;

          if (l_pEdit != nullptr) {
            m_vAttributes[a_iIndex].push_back(std::make_tuple(a_pAttr->getAttributeName(i), p, l_pEdit, a_pAttr->getAttributeType(i)));
            a_pParent->addChild(l_pEdit);
          }
        }
      }
    }

    /**
    * Commit a text-input value to the attributes
    * @param a_sName name of the attribute
    * @param a_pEdit the edit field to get the value from
    * @param a_pAttr the attributes to modify
    */
    void CCmdDecoration::commitTextInputValue(const std::string &a_sName, irr::gui::IGUIEditBox *a_pEdit, irr::io::IAttributes *a_pAttr) {
      if (a_pAttr->existsAttribute(a_sName.c_str())) {
        switch (a_pAttr->getAttributeType(a_sName.c_str())) {
	        case irr::io::EAT_INT: { 
            a_pAttr->setAttribute(a_sName.c_str(), (irr::s32)std::wcstol(a_pEdit->getText(), nullptr, 10));
            break; 
          }

	        case irr::io::EAT_FLOAT: { 
            a_pAttr->setAttribute(a_sName.c_str(), (irr::f32)std::wcstof(a_pEdit->getText(), nullptr));
            break; 
          }

	        case irr::io::EAT_STRING: {
            std::wstring l_sNew = a_pEdit->getText();
            a_pAttr->setAttribute(a_sName.c_str(), l_sNew.c_str());
            break; 
          }

	        case irr::io::EAT_COLOR: { break; }
	        case irr::io::EAT_COLORF: { break; }
	        case irr::io::EAT_VECTOR3D: { 
            std::string l_sValue = helpers::ws2s(a_pEdit->getText());

            std::vector<std::string> l_vValues = helpers::splitString(l_sValue, ',');
            while (l_vValues.size() < 3)
              l_vValues.push_back("0.0");

            a_pAttr->setAttribute(a_sName.c_str(), irr::core::vector3df((irr::f32)std::atof(l_vValues[0].c_str()), (irr::f32)std::atof(l_vValues[1].c_str()), (irr::f32)std::atof(l_vValues[2].c_str())));
            break; 
          }
	        case irr::io::EAT_POSITION2D: { break; }
	        case irr::io::EAT_VECTOR2D: { break; }
	        case irr::io::EAT_RECT: { break; }
	        case irr::io::EAT_MATRIX: { break; }
	        case irr::io::EAT_QUATERNION: { break; }
	        case irr::io::EAT_BBOX: { break; }
	        case irr::io::EAT_PLANE: { break; }
	        case irr::io::EAT_TRIANGLE3D: { break; }
	        case irr::io::EAT_LINE2D: { break; }
	        case irr::io::EAT_LINE3D: { break; }
	        case irr::io::EAT_STRINGWARRAY: { break; }
	        case irr::io::EAT_FLOATARRAY: { break; }
	        case irr::io::EAT_INTARRAY: { break; }
	        case irr::io::EAT_BINARY: { break; }
	        case irr::io::EAT_TEXTURE: { 
            break; 
          }
	        case irr::io::EAT_USER_POINTER: { break; }
          case irr::io::EAT_DIMENSION2D: { break; }
        }
      }
    }


    /**
    * Commit a text-input value to the attributes
    * @param a_sName name of the attribute
    * @param a_pEdit the edit field to get the value from
    * @param a_pAttr the attributes to modify
    */
    void CCmdDecoration::revertTextInputValue(const std::string &a_sName, irr::gui::IGUIEditBox *a_pEdit, irr::io::IAttributes *a_pAttr) {
      if (a_pAttr->existsAttribute(a_sName.c_str())) {
        switch (a_pAttr->getAttributeType(a_sName.c_str())) {
	        case irr::io::EAT_INT: { 
            a_pEdit->setText(std::to_wstring(a_pAttr->getAttributeAsInt(a_sName.c_str())).c_str());
            break; 
          }

	        case irr::io::EAT_FLOAT: { 
            a_pEdit->setText(std::to_wstring(a_pAttr->getAttributeAsFloat(a_sName.c_str())).c_str());
            break; 
          }

	        case irr::io::EAT_STRING: {
            a_pEdit->setText(helpers::s2ws(a_pAttr->getAttributeAsString(a_sName.c_str()).c_str()).c_str());
            break; 
          }

	        case irr::io::EAT_COLOR: { break; }
	        case irr::io::EAT_COLORF: { break; }
	        case irr::io::EAT_VECTOR3D: {
            irr::core::vector3df v = a_pAttr->getAttributeAsVector3d(a_sName.c_str());
            a_pEdit->setText((std::to_wstring(v.X) + L", " + std::to_wstring(v.Y) + L", " + std::to_wstring(v.Z)).c_str());
            break;
          }
	        case irr::io::EAT_POSITION2D: { break; }
	        case irr::io::EAT_VECTOR2D: { break; }
	        case irr::io::EAT_RECT: { break; }
	        case irr::io::EAT_MATRIX: { break; }
	        case irr::io::EAT_QUATERNION: { break; }
	        case irr::io::EAT_BBOX: { break; }
	        case irr::io::EAT_PLANE: { break; }
	        case irr::io::EAT_TRIANGLE3D: { break; }
	        case irr::io::EAT_LINE2D: { break; }
	        case irr::io::EAT_LINE3D: { break; }
	        case irr::io::EAT_STRINGWARRAY: { break; }
	        case irr::io::EAT_FLOATARRAY: { break; }
	        case irr::io::EAT_INTARRAY: { break; }
	        case irr::io::EAT_BINARY: { break; }
	        case irr::io::EAT_TEXTURE: { 
            break; 
          }
	        case irr::io::EAT_USER_POINTER: { break; }
          case irr::io::EAT_DIMENSION2D: { break; }
        }
      }
    }

    /**
    * Update a scene node in the tree
    * @param a_pNode the node to update
    */
    void CCmdDecoration::updateNodeInTree(irr::scene::ISceneNode *a_pNode) {
      for (auto l_tTree: m_mNodeMap) {
        if (l_tTree.second == a_pNode) {
          l_tTree.first->setText(helpers::s2ws(a_pNode->getName()).c_str());
        }
      }
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdDecoration::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {

        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_CLOSED) {
          if (a_cEvent.GUIEvent.Caller == m_pDialog) {
            irr::s32 x = m_pDialog->getAbsolutePosition().UpperLeftCorner.X;
            irr::s32 y = m_pDialog->getAbsolutePosition().UpperLeftCorner.Y;

            tracked::IGlobal::getInstance()->setSetting("decoration_x", std::to_string(x));
            tracked::IGlobal::getInstance()->setSetting("decoration_y", std::to_string(y));

            m_pDialog   = nullptr;
            m_bFinished = true;

            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_FOCUS_LOST) {

        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          for (int i = 0; i < 3; i++) {
            for (auto l_tAttribute: m_vAttributes[i]) {
              if (std::get<2>(l_tAttribute) == a_cEvent.GUIEvent.Caller) {
                printf("Checked.\n");
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_TREEVIEW_NODE_SELECT) {
          if (a_cEvent.GUIEvent.Caller == m_pScene) {
            irr::gui::IGUITreeViewNode *l_pSelected = m_pScene->getSelected();
            if (l_pSelected != nullptr) {
              if (m_mNodeMap.find(l_pSelected) != m_mNodeMap.end()) {
                m_pSelected = m_mNodeMap[l_pSelected];

                irr::io::IAttributes *l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();

                if (l_pAttr != nullptr) {
                  m_pSelected->serializeAttributes(l_pAttr);

                  updateAttributeList(l_pAttr, 0, m_aParameters[0]);

                  l_pAttr->drop();
                }

                l_pAttr = m_pSmgr->getVideoDriver()->createAttributesFromMaterial(m_pSelected->getMaterial(0));

                if (l_pAttr != nullptr) {
                  updateAttributeList(l_pAttr, 1, m_aParameters[1]);
                  l_pAttr->drop();
                }
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_TREEVIEW_NODE_DESELECT) {
          if (a_cEvent.GUIEvent.Caller == m_pScene) {
            irr::gui::IGUITreeViewNode *l_pUnSelected = m_pScene->getSelected();
            if (l_pUnSelected != nullptr) {
              irr::scene::ISceneNode *l_pNode = reinterpret_cast<irr::scene::ISceneNode *>(l_pUnSelected->getData2());
              if (l_pNode == m_pSelected) {
                m_pSelected = nullptr;
                updateAttributeList(nullptr, 0, nullptr);
                updateAttributeList(nullptr, 1, nullptr);
                updateAttributeList(nullptr, 2, nullptr);
              }
            }
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (m_pGui->getFocus()->getType() == irr::gui::EGUIET_EDIT_BOX && m_pSelected != nullptr) {
          if (a_cEvent.KeyInput.PressedDown && a_cEvent.KeyInput.Key == irr::KEY_RETURN) {
            for (int i = 0; i < 3; i++) {
              for (auto l_tAttribute : m_vAttributes[i]) {
                if (m_pGui->getFocus() == std::get<2>(l_tAttribute)) {
                  irr::io::IAttributes *l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();

                  if (l_pAttr != nullptr) {
                    m_pSelected->serializeAttributes(l_pAttr);
                    commitTextInputValue(std::get<0>(l_tAttribute), reinterpret_cast<irr::gui::IGUIEditBox *>(m_pGui->getFocus()), l_pAttr);
                    m_pSelected->deserializeAttributes(l_pAttr);
                    updateNodeInTree(m_pSelected);
                    l_pAttr->drop();
                  }
                }
              }
            }
          }
          else if (a_cEvent.KeyInput.PressedDown && a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) {
            for (int i = 0; i < 3; i++) {
              for (auto l_tAttribute : m_vAttributes[i]) {
                if (m_pGui->getFocus() == std::get<2>(l_tAttribute)) {
                  irr::io::IAttributes *l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();

                  if (l_pAttr != nullptr) {
                    m_pSelected->serializeAttributes(l_pAttr);
                    revertTextInputValue(std::get<0>(l_tAttribute), reinterpret_cast<irr::gui::IGUIEditBox *>(m_pGui->getFocus()), l_pAttr);
                    l_pAttr->drop();
                  }
                }
              }
            }
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iSceneWillBeCleared) {
          m_bFinished = true;
        }
      }

      return l_bRet;
    }
  }
}