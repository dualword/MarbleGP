// (w) 2024 by Dustbin::Games / Christian Keimel
#include <platform/CTrackEdPlatform.h>
#include <gui/CGuiScrollContainer.h>
#include <commands/CCmdDecoration.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CTrackEdHelpers.h>
#include <editor/CPartTranslator.h>
#include <helpers/CStringHelpers.h>
#include <CDefines.h>
#include <IGlobal.h>
#include <algorithm>

namespace dustbin {
  namespace commands {
    CCmdDecoration::CCmdDecoration(irr::scene::ISceneManager *a_pSmgr, irr::gui::IGUIEnvironment *a_pGui) :
      ICommand     ("CmdDecoration"),
      m_bFinished  (false),
      m_pSmgr      (a_pSmgr),
      m_pGui       (a_pGui),
      m_pNode      (nullptr),
      m_pScene     (nullptr),
      m_pTabMain   (nullptr),
      m_pTabNode   (nullptr),
      m_pMaterial  (nullptr),
      m_pAnimator  (nullptr),
      m_pAnimType  (nullptr),
      m_pMeshName  (nullptr),
      m_pAnimAdd   (nullptr),
      m_pAnimDel   (nullptr),
      m_pTranslator(nullptr)
    {
      m_pNode = getDecorationRoot(m_pSmgr->getRootSceneNode());

      if (m_pNode == nullptr) {
        m_pNode = m_pSmgr->addEmptySceneNode(m_pSmgr->getRootSceneNode());
        m_pNode->setName("__DecorationRootNode");
      }

      m_pTranslator = new tracked::CPartTranslator(m_pSmgr);

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

      l_pNew->setExpanded(true);

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

      m_pMeshName = m_pGui->addStaticText(L"-", irr::core::recti(irr::core::position2di(0, 0), irr::core::dimension2du(l_cParamRect.getWidth(), l_cSize.Height)), false, true, m_aNodeTabs[0]);
      m_pMeshName->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
      m_pMeshName->setBackgroundColor(irr::video::SColor(224, 192, 192, 224));
      m_pMeshName->setDrawBackground(true);

      m_aParameters[0] = new gui::CGuiScrollContainer(m_pGui, m_aNodeTabs[0], l_cParamRect);

      m_aNodeTabs[1] = m_pGui->addTab(m_pTabNode->getRelativePosition(), m_pTabNode);
      m_aNodeTabs[1]->setText(L"Materials");
      m_pTabNode->addTab(m_aNodeTabs[1]);

      m_aParameters[1] = new gui::CGuiScrollContainer(m_pGui, m_aNodeTabs[1], l_cParamRect);

      l_cSize = m_pGui->getSkin()->getFont()->getDimension(L"   Select Material:   ");

      irr::core::position2di l_cPos = irr::core::position2di(0, m_pGui->getSkin()->getSize(irr::gui::EGDS_WINDOW_BUTTON_WIDTH) / 2);

      m_pGui->addStaticText(L"Select Material: ", irr::core::recti(l_cPos, l_cSize), false, false, m_aNodeTabs[1])->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
      l_cPos.X += l_cSize.Width;

      irr::s32 l_iSize = m_aParameters[1]->getAbsolutePosition().getWidth() / 4;
      for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 4; x++) {
          int l_iIndex = x + 4 * y;

          irr::core::recti l_cRect = irr::core::recti(l_iSize * x, l_iSize * y, l_iSize * (x + 1), l_iSize * (y + 1));

          m_aTexture[l_iIndex] = m_pGui->addImage(l_cRect, m_aParameters[1]);
          m_aTexture[l_iIndex]->setScaleImage(true);
        }
      }

      m_pMaterial = m_pGui->addComboBox(irr::core::recti(l_cPos, irr::core::dimension2du(l_cParamRect.getWidth() - l_cSize.Width, l_cSize.Height)), m_aNodeTabs[1]);

      m_aNodeTabs[2] = m_pGui->addTab(m_pTabNode->getRelativePosition(), m_pTabNode);
      m_aNodeTabs[2]->setText(L"Animators");
      m_pTabNode->addTab(m_aNodeTabs[2]);

      m_pGui->addStaticText(L"Select Animator: ", irr::core::recti(irr::core::position2di(0, l_cPos.Y), l_cSize), false, false, m_aNodeTabs[2])->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
      m_pAnimator = m_pGui->addComboBox(irr::core::recti(l_cPos, irr::core::dimension2du(l_cParamRect.getWidth() - l_cSize.Width, l_cSize.Height)), m_aNodeTabs[2]);

      l_cPos.Y += 5 * l_cSize.Height / 4;

      m_pGui->addStaticText(L"Animator Type: ", irr::core::recti(irr::core::position2di(0, l_cPos.Y), l_cSize), false, false, m_aNodeTabs[2])->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
      m_pAnimType = m_pGui->addComboBox(irr::core::recti(l_cPos, irr::core::dimension2du(l_cParamRect.getWidth() - l_cSize.Width, l_cSize.Height)), m_aNodeTabs[2]);

      irr::u32 l_iIndex = 0;
      irr::scene::ISceneNodeAnimatorFactory *l_pFactory = nullptr;
      std::vector<std::string> l_vList;

      do {
        l_pFactory = m_pSmgr->getSceneNodeAnimatorFactory(l_iIndex);

        if (l_pFactory != nullptr) {
          for (irr::u32 i = 0; i < l_pFactory->getCreatableSceneNodeAnimatorTypeCount(); i++) {
            l_vList.push_back(l_pFactory->getCreateableSceneNodeAnimatorTypeName(i));
          }
        }

        l_iIndex++;
      } 
      while (l_pFactory != nullptr);

      std::sort(l_vList.begin(), l_vList.end());

      for (auto l_sType: l_vList) {
        m_pAnimType->addItem(helpers::s2ws(l_sType).c_str());
      }

      l_cPos.Y += 5 * l_cSize.Height / 4;

      m_pAnimAdd = m_pGui->addButton(
        irr::core::recti(irr::core::vector2di(0, l_cPos.Y), irr::core::dimension2du(l_cParamRect.getWidth() / 2, l_cSize.Height)),
        m_aNodeTabs[2],
        -1,
        L"Add Animator"
      );

      m_pAnimDel = m_pGui->addButton(
        irr::core::recti(irr::core::vector2di(l_cParamRect.getWidth() / 2, l_cPos.Y), irr::core::dimension2du(l_cParamRect.getWidth() / 2, l_cSize.Height)),
        m_aNodeTabs[2],
        -1,
        L"Remove Animator"
      );

      l_cParamRect.UpperLeftCorner.Y = m_pAnimAdd->getRelativePosition().LowerRightCorner.Y;
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
    void CCmdDecoration::updateAttributeList(irr::io::IAttributes *a_pAttr, int a_iIndex, irr::gui::IGUIElement *a_pParent, const irr::core::position2di &a_cPos) {
      for (auto &l_tAttribute: m_vAttributes[a_iIndex]) {
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

        irr::core::position2di l_cPos = a_cPos + irr::core::position2di(l_cSize.Height / 2, 0);

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
              l_pEdit = m_pGui->addEditBox(std::to_wstring(a_pAttr->getAttributeAsInt(i)).c_str(), l_cEditRect, true, a_pParent);
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

	          case irr::io::EAT_COLOR: { 
              irr::video::SColor l_cColor = a_pAttr->getAttributeAsColor(i);

              wchar_t l_sColor[64];
              swprintf_s<64>(l_sColor, L"%X", l_cColor.color);

              irr::gui::IGUITab *l_pTab = m_pGui->addTab(l_cEditRect, a_pParent);
              l_pTab->setBackgroundColor(l_cColor);
              l_pTab->setDrawBackground(true);
              l_pTab->setText(l_sColor);

              l_pEdit = l_pTab;
              break; 
            }
	          case irr::io::EAT_COLORF: { 
              irr::video::SColorf l_cColor = a_pAttr->getAttributeAsColor(i);
              std::wstring l_sText =
                std::to_wstring(l_cColor.a) + L", " +
                std::to_wstring(l_cColor.r) + L", " +
                std::to_wstring(l_cColor.g) + L", " +
                std::to_wstring(l_cColor.b) + L", ";

              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }

	          case irr::io::EAT_VECTOR3D: { 
              irr::core::vector3df l_cValue = a_pAttr->getAttributeAsVector3d(i);
              std::wstring l_sText = std::to_wstring(l_cValue.X) + L", " + std::to_wstring(l_cValue.Y) + L", " + std::to_wstring(l_cValue.Z);
              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
	          case irr::io::EAT_POSITION2D: { 
              irr::core::position2di l_cValue = a_pAttr->getAttributeAsPosition2d(i);
              std::wstring l_sText = std::to_wstring(l_cValue.X) + L", " + std::to_wstring(l_cValue.Y);
              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
	          case irr::io::EAT_VECTOR2D: { 
              irr::core::vector2df l_cValue = a_pAttr->getAttributeAsVector2d(i);
              std::wstring l_sText = std::to_wstring(l_cValue.X) + L", " + std::to_wstring(l_cValue.Y);
              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
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
            case irr::io::EAT_DIMENSION2D: { 
              irr::core::dimension2du l_cValue = a_pAttr->getAttributeAsDimension2d(i);
              std::wstring l_sText = std::to_wstring(l_cValue.Width) + L", " + std::to_wstring(l_cValue.Height);
              l_pEdit = m_pGui->addEditBox(l_sText.c_str(), l_cEditRect, true, a_pParent);
              break; 
            }
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
            a_pAttr->setAttribute(a_sName.c_str(), m_pSmgr->getVideoDriver()->getTexture(helpers::ws2s(a_pEdit->getText()).c_str()));
            break; 
          }
	        case irr::io::EAT_USER_POINTER: { break; }
          case irr::io::EAT_DIMENSION2D: { break; }
        }
      }
    }

    /**
    * Update the position parameters in the UI
    */
    void CCmdDecoration::updatePositionParameters() {
      for (auto &l_tAttribute: m_vAttributes[0]) {
        if (std::get<0>(l_tAttribute) == "Position") {
          irr::core::vector3df v = m_pSelected->getPosition();
          std::wstring l_sText = std::to_wstring(v.X) + L", " + std::to_wstring(v.Y) + L", " + std::to_wstring(v.Z);
          std::get<2>(l_tAttribute)->setText(l_sText.c_str());
        }
        else if (std::get<0>(l_tAttribute) == "Rotation") {
          irr::core::vector3df v = m_pSelected->getRotation();
          std::wstring l_sText = std::to_wstring(v.X) + L", " + std::to_wstring(v.Y) + L", " + std::to_wstring(v.Z);
          std::get<2>(l_tAttribute)->setText(l_sText.c_str());
        }
      }
    }

    /**
    * Update the dropdown for the animator selection
    */
    void CCmdDecoration::updateAnimatorDropdown() {
      if (m_pAnimator != nullptr) {
        m_pAnimator->clear();

        if (m_pSelected != nullptr) {
          for (irr::u32 i = 0; i < m_pSelected->getAnimators().size(); i++)
            m_pAnimator->addItem(std::to_wstring(i + 1).c_str(), i);
        }

        m_pAnimator->setSelected(m_pAnimator->getItemCount() > 0 ? 0 : -1);
      }
    }

    /**
    * Update the dropdown for the material selection with the data taken from "m_pSelected"
    */
    void CCmdDecoration::updateMaterialDropdown() {
      if (m_pMaterial != nullptr) {
        m_pMaterial->clear();

        if (m_pSelected != nullptr) {
          for (irr::u32 i = 0; i < m_pSelected->getMaterialCount(); i++)
            m_pMaterial->addItem(std::to_wstring(i + 1).c_str(), i);

          m_pMaterial->setSelected(0);
        }
      }

      if (m_pMeshName != nullptr) {
        if (m_pSelected == nullptr)
          m_pMeshName->setText(L"--");
        else {
          std::string l_sText = std::string(m_pSelected->getName()) + " " + getTypeName(m_pSelected);
          m_pMeshName->setText(helpers::s2ws(l_sText).c_str());
        }
      }

      m_pTranslator->setEditNode(m_pSelected);
    }

    /**
    * Update the animator tab
    */
    void CCmdDecoration::updateAnimatorTab() {
      irr::io::IAttributes *l_pAttr = nullptr;

      if (m_pSelected != nullptr) {
        irr::scene::ISceneNodeAnimator *l_pAnimator = getSelectedAnimator();
        if (l_pAnimator != nullptr) {
          l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();
          l_pAnimator->serializeAttributes(l_pAttr);
        }
      }

      irr::s32 l_iOffset = m_pAnimAdd != nullptr ? m_pAnimAdd->getAbsolutePosition().LowerRightCorner.Y + 10 * m_pAnimAdd->getAbsolutePosition().getHeight() / 9 : 0;
      updateAttributeList(l_pAttr, 2, m_aParameters[2], irr::core::position2di(0, 0));
    }

    /**
    * Update the material tab
    */
    void CCmdDecoration::updateMaterialTab() {
      irr::u32 l_iMaterial = m_pMaterial != nullptr ? m_pMaterial->getItemData(m_pMaterial->getSelected()) : 0;

      irr::io::IAttributes *l_pAttr = m_pSmgr->getVideoDriver()->createAttributesFromMaterial(m_pSelected->getMaterial(l_iMaterial));

      if (l_pAttr != nullptr) {
        irr::s32 l_iSize = m_aParameters[1]->getAbsolutePosition().getWidth() / 4;
        updateAttributeList(l_pAttr, 1, m_aParameters[1], irr::core::position2di(0, 9 * l_iSize / 4));
        l_pAttr->drop();
      }

      for (irr::u32 l_iIndex = 0; l_iIndex < 8; l_iIndex++)
        m_aTexture[l_iIndex]->setImage(m_pSelected->getMaterial(l_iMaterial).getTexture(l_iIndex));
    }

    /**
    * Revert a text-input value to the attributes
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
      for (auto &l_tTree: m_mNodeMap) {
        if (l_tTree.second == a_pNode) {
          l_tTree.first->setText(helpers::s2ws(a_pNode->getName()).c_str());
        }
      }
    }

    /**
    * A scene node will be deleted
    * @param a_pNode the node to be deleted
    */
    void CCmdDecoration::deleteNode(irr::scene::ISceneNode *a_pNode) {
      for (auto &l_tTree: m_mNodeMap) {
        if (l_tTree.second == a_pNode) {
          l_tTree.first->getParent()->deleteChild(l_tTree.first);
          m_mNodeMap.erase(l_tTree.first);
          break;
        }
      }
    }

    /**
    * A scene node was added
    * @param a_pNode the new node
    */
    void CCmdDecoration::addNode(irr::scene::ISceneNode *a_pNode) {
      for (auto &l_tTree: m_mNodeMap) {
        if (l_tTree.second == a_pNode->getParent()) {
          updateDecorationList(a_pNode, l_tTree.first);
        }
      }
    }

    /**
    * Create and fill an IAttributes object depending on the selected node tab
    * @return an IAttributes object or nullptr if something wasn't ok
    */
    irr::io::IAttributes *CCmdDecoration::getAttributesForEditing() {
      irr::io::IAttributes *l_pAttr = nullptr;

      if (m_pSelected != nullptr) {
        if (m_pTabNode->getActiveTab() == 0) {
          l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();
          m_pSelected->serializeAttributes(l_pAttr);
        }
        else if (m_pTabNode->getActiveTab() == 1) {
          l_pAttr = m_pSmgr->getVideoDriver()->createAttributesFromMaterial(m_pSelected->getMaterial(m_pMaterial->getItemData(m_pMaterial->getSelected())));
        }
        else if (m_pTabNode->getActiveTab() == 2) {
          irr::scene::ISceneNodeAnimator *l_pAnimator = getSelectedAnimator();
          if (l_pAnimator != nullptr) {
            l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();
            l_pAnimator->serializeAttributes(l_pAttr);
          }
        }
      }

      return l_pAttr;
    }

    /**
    * Let this command draw something (if necessary)
    * @param a_pDrv the video driver
    */
    void CCmdDecoration::draw(irr::video::IVideoDriver *a_pDrv) {
      if (m_pTranslator != nullptr)
        m_pTranslator->draw(a_pDrv, m_pSmgr->getActiveCamera());
    }

    /**
    * Get the selected animator from the selected scene node
    * @return the selected animator
    */
    irr::scene::ISceneNodeAnimator *CCmdDecoration::getSelectedAnimator() {
      if (m_pSelected != nullptr) {
        irr::scene::ISceneNodeAnimatorList l_cList = m_pSelected->getAnimators();

        if (l_cList.size() > 0) {
          irr::u32 l_iIndex = m_pAnimator->getItemData(m_pAnimator->getSelected());
          irr::scene::ISceneNodeAnimatorList::Iterator l_itAnim = l_cList.begin();
          while (l_iIndex > 0 && l_itAnim != l_cList.end()) {
            l_itAnim++;
            l_iIndex--;
          }

          if (l_itAnim != l_cList.end())
            return *l_itAnim;
        }
      }

      return nullptr;
    }

    /**
    * Commit attributes depending on the selected node tab
    * @param a_pAttr the attributes to commit
    */
    void CCmdDecoration::commitAttributes(irr::io::IAttributes *a_pAttr) {
      bool l_bSaved = false;

      if (m_pSelected != nullptr) {
        if (m_pTabNode->getActiveTab() == 0) {
          m_pSelected->deserializeAttributes(a_pAttr);
          l_bSaved = true;
        }
        else if (m_pTabNode->getActiveTab() == 1) {
          m_pSmgr->getVideoDriver()->fillMaterialStructureFromAttributes(m_pSelected->getMaterial(m_pMaterial->getItemData(m_pMaterial->getSelected())), a_pAttr);
          l_bSaved = true;
        }
        else if (m_pTabNode->getActiveTab() == 2) {
          irr::scene::ISceneNodeAnimator *l_pAnimator = getSelectedAnimator();

          if (l_pAnimator != nullptr) {
            l_pAnimator->deserializeAttributes(a_pAttr);
          }
        }
      }

      if (l_bSaved)
        helpers::postUserEvent(c_iUpdateShader, c_iUpdateShader);
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdDecoration::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      for (auto l_pLimiter: m_vLimiters) {
        if (l_pLimiter->consumeEvent(a_cEvent)) {
          return true;
        }
      }

      if (m_pTranslator->OnEvent(a_cEvent)) {
        updatePositionParameters();
        return true;
      }

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_MENU_ITEM_SELECTED) {
          if (a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_CONTEXT_MENU) {
            irr::gui::IGUIContextMenu *l_pMenu = reinterpret_cast<irr::gui::IGUIContextMenu *>(a_cEvent.GUIEvent.Caller);
            std::wstring l_sCaller = l_pMenu->getItemText(l_pMenu->getSelectedItem());

            if (l_sCaller == L"Add Mesh Scene Node") {
              if (m_pSelected == nullptr) {
                m_pSelected = m_pNode;
                updateMaterialDropdown();
                updateAnimatorDropdown();
              }

              irr::io::path l_sDir = m_pGui->getFileSystem()->getWorkingDirectory();

              std::string l_sFile = platform::portableSelectFileOpen(L"All Filed (*.*)\0*.*", helpers::s2ws(tracked::IGlobal::getInstance()->getSetting("meshimport_folder")).c_str(), L"");

              m_pGui->getFileSystem()->changeWorkingDirectoryTo(l_sDir);

              if (l_sFile != "") {
                tracked::IGlobal::getInstance()->setSetting("meshimport_folder", helpers::getFilepath(l_sFile));

                irr::scene::IMesh *l_pMesh = m_pSmgr->getMesh(l_sFile.c_str());
                if (l_pMesh != nullptr) {
                  irr::core::aabbox3df l_cBox = l_pMesh->getBoundingBox();
                  irr::core::vector3df l_cPos = m_pSmgr->getActiveCamera()->getPosition();
                  irr::core::vector3df l_cTgt = m_pSmgr->getActiveCamera()->getTarget();

                  l_cTgt = l_cTgt.normalize();

                  l_cTgt *= 2.0f * l_cBox.getExtent().getLength();

                  irr::scene::ISceneNode *l_pNew = m_pSmgr->addMeshSceneNode(l_pMesh, m_pSelected, -1, l_cTgt);

                  if (l_pNew != nullptr) {
                    l_pNew->setName(helpers::getFilename(l_sFile).c_str());
                    addNode(l_pNew);
                  }
                }
                else helpers::addLogMessage(L"Unable to load file \"" + helpers::s2ws(l_sFile) + L"\".", irr::ELL_ERROR, 2000);
              }

              l_bRet = true;
            }
            else if (l_sCaller == L"Add Empty Scene Node") {
              if (m_pSelected == nullptr) {
                m_pSelected = m_pNode;
                updateMaterialDropdown();
                updateAnimatorDropdown();
              }

              addNode(m_pSmgr->addEmptySceneNode(m_pSelected));
              l_bRet = true;
            }
            else if (l_sCaller == L"Delete Node") {
              if (m_pSelected != nullptr) {
                m_pSelected->setVisible(false);
                deleteNode(m_pSelected);
                m_pSmgr->addToDeletionQueue(m_pSelected);
                helpers::postUserEvent(c_iUpdateShader, c_iUpdateShader);
                m_pSelected = nullptr;
                updateMaterialDropdown();
                updateAnimatorDropdown();
              }
              else helpers::addLogMessage(L"No scene node selected for deletion", irr::ELL_ERROR, 2000);
              l_bRet = true;
            }
            else if (l_sCaller == L"Clone Node") {
              if (m_pSelected != nullptr) {
                irr::scene::ISceneNode *l_pNew = m_pSelected->clone(m_pSelected->getParent(), m_pSmgr);
                addNode(l_pNew);
                helpers::postUserEvent(c_iUpdateShader, c_iUpdateShader);
              }
              else helpers::addLogMessage(L"No scene node to clone selected", irr::ELL_ERROR, 2000);
              l_bRet = true;
            }
          }
        }
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
          for (int i = 0; i < 3; i++) {
            for (auto &l_tAttribute : m_vAttributes[i]) {
              if (std::get<2>(l_tAttribute) == a_cEvent.GUIEvent.Caller) {
                for (auto l_itLimiter = m_vLimiters.begin(); l_itLimiter != m_vLimiters.end(); l_itLimiter++) {
                  if ((*l_itLimiter)->getElement() == a_cEvent.GUIEvent.Caller) {
                    CInputLimiter *p = *l_itLimiter;
                    m_vLimiters.erase(l_itLimiter);
                    delete p;
                    break;
                  }
                }
                printf("Focus lost (%i)\n", (int)m_vLimiters.size());
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_FOCUSED) {
          for (int i = 0; i < 3; i++) {
            for (auto &l_tAttribute : m_vAttributes[i]) {
              if (std::get<2>(l_tAttribute) == a_cEvent.GUIEvent.Caller) {
                bool l_bLimit = true;

                for (auto l_pLimiter: m_vLimiters) {
                  if (l_pLimiter->getElement() == a_cEvent.GUIEvent.Caller) {
                    l_bLimit = false;
                    break;
                  }
                }

                if (l_bLimit)
                  m_vLimiters.push_back(new CInputLimiter(a_cEvent.GUIEvent.Caller, std::get<3>(l_tAttribute)));

                printf("Focus gained (%i)\n", (int)m_vLimiters.size());
                break;
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          for (int i = 0; i < 3; i++) {
            for (auto &l_tAttribute: m_vAttributes[i]) {
              if (std::get<2>(l_tAttribute) == a_cEvent.GUIEvent.Caller) {
                irr::io::IAttributes *l_pAttr = getAttributesForEditing();

                if (l_pAttr != nullptr) {
                  l_pAttr->setAttribute(std::get<0>(l_tAttribute).c_str(), reinterpret_cast<irr::gui::IGUICheckBox *>(a_cEvent.GUIEvent.Caller)->isChecked());
                  commitAttributes(l_pAttr);
                  l_pAttr->drop();
                  l_bRet = true;
                }
                l_bRet = true;
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
                updateMaterialDropdown();
                updateAnimatorDropdown();

                irr::io::IAttributes *l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();

                if (l_pAttr != nullptr) {
                  irr::io::SAttributeReadWriteOptions l_cOptions;
                  l_cOptions.Flags = irr::io::EARWF_FOR_EDITOR | irr::io::EARWF_USE_RELATIVE_PATHS;

                  m_pSelected->serializeAttributes(l_pAttr, &l_cOptions);

                  updateAttributeList(l_pAttr, 0, m_aParameters[0], irr::core::position2di());

                  l_pAttr->drop();
                }

                updateMaterialTab();
                updateAnimatorTab();
              }
            }
            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_TREEVIEW_NODE_DESELECT) {
          if (a_cEvent.GUIEvent.Caller == m_pScene) {
            irr::gui::IGUITreeViewNode *l_pUnSelected = m_pScene->getSelected();
            if (l_pUnSelected != nullptr) {
              irr::scene::ISceneNode *l_pNode = reinterpret_cast<irr::scene::ISceneNode *>(l_pUnSelected->getData2());
              if (l_pNode == m_pSelected) {
                m_pSelected = nullptr;
                updateAttributeList(nullptr, 0, nullptr, irr::core::position2di());
                updateAttributeList(nullptr, 1, nullptr, irr::core::position2di());
                updateAttributeList(nullptr, 2, nullptr, irr::core::position2di());
                updateMaterialDropdown();
                updateAnimatorDropdown();

                l_bRet = true;
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_COMBO_BOX_CHANGED) {
          if (a_cEvent.GUIEvent.Caller == m_pMaterial) {
            updateMaterialTab();
            l_bRet = true;
          }
          else if (a_cEvent.GUIEvent.Caller == m_pAnimator) {
            updateAnimatorTab();
            l_bRet = true;
          }

          if (!l_bRet) {
            for (int i = 0; i < 3; i++) {
              for (auto &l_tAttribute : m_vAttributes[i]) {
                if (m_pGui->getFocus() == std::get<2>(l_tAttribute)) {
                  irr::io::IAttributes *l_pAttr = getAttributesForEditing();

                  if (l_pAttr != nullptr) {
                    irr::gui::IGUIComboBox *l_pBox = reinterpret_cast<irr::gui::IGUIComboBox *>(a_cEvent.GUIEvent.Caller);

                    std::wstring l_sSelected = l_pBox->getItem(l_pBox->getSelected());

                    std::vector<irr::core::stringc> l_vValues;

                    for (irr::u32 i = 0; i < l_pBox->getItemCount(); i++)
                      l_vValues.push_back(helpers::ws2s(l_pBox->getItem(i)).c_str());

                    irr::core::array<const irr::c8 *> l_aValues;

                    for (auto &l_pItem : l_vValues)
                      l_aValues.push_back(l_pItem.c_str());

                    l_aValues.push_back(nullptr);

                    l_pAttr->setAttribute(std::get<0>(l_tAttribute).c_str(), helpers::ws2s(l_sSelected).c_str(), l_aValues.pointer());

                    commitAttributes(l_pAttr);

                    l_pAttr->drop();
                    l_bRet = true;
                  }
                }
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (a_cEvent.GUIEvent.Caller == m_pAnimAdd) {
            if (m_pSelected != nullptr) {
              std::string l_sAnimator = helpers::ws2s(m_pAnimType->getItem(m_pAnimType->getSelected()));

              m_pSmgr->createSceneNodeAnimator(l_sAnimator.c_str(), m_pSelected);

              updateAnimatorDropdown();
              m_pAnimator->setSelected(m_pSelected->getAnimators().size() - 1);
              updateAnimatorTab();

              l_bRet = true;
            }
          }
          else if (a_cEvent.GUIEvent.Caller == m_pAnimDel) {
            if (m_pSelected != nullptr) {
              irr::scene::ISceneNodeAnimator *l_pAnimator = getSelectedAnimator();
              if (l_pAnimator != nullptr) {
                m_pSelected->removeAnimator(l_pAnimator);
                updateAnimatorDropdown();
                updateAnimatorTab();
              }
            }
            l_bRet = true;
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (m_pGui->getFocus() != nullptr && m_pGui->getFocus()->getType() == irr::gui::EGUIET_EDIT_BOX && m_pSelected != nullptr) {
          if (a_cEvent.KeyInput.PressedDown) {
            if (a_cEvent.KeyInput.Key == irr::KEY_RETURN) {
              for (int i = 0; i < 3; i++) {
                for (auto &l_tAttribute : m_vAttributes[i]) {
                  if (m_pGui->getFocus() == std::get<2>(l_tAttribute)) {
                    irr::io::IAttributes *l_pAttr = getAttributesForEditing();

                    if (l_pAttr != nullptr) {
                      commitTextInputValue(std::get<0>(l_tAttribute), reinterpret_cast<irr::gui::IGUIEditBox *>(m_pGui->getFocus()), l_pAttr);
                      commitAttributes(l_pAttr);

                      updateNodeInTree(m_pSelected);
                      l_pAttr->drop();
                      l_bRet = true;
                    }
                  }
                }
              }
            }
            else if (a_cEvent.KeyInput.Key == irr::KEY_ESCAPE) {
              for (int i = 0; i < 3; i++) {
                for (auto &l_tAttribute : m_vAttributes[i]) {
                  if (m_pGui->getFocus() == std::get<2>(l_tAttribute)) {
                    irr::io::IAttributes *l_pAttr = m_pGui->getFileSystem()->createEmptyAttributes();

                    if (l_pAttr != nullptr) {
                      irr::io::SAttributeReadWriteOptions l_cOptions;
                      l_cOptions.Flags = irr::io::EARWF_FOR_EDITOR | irr::io::EARWF_USE_RELATIVE_PATHS;

                      m_pSelected->serializeAttributes(l_pAttr, &l_cOptions);
                      revertTextInputValue(std::get<0>(l_tAttribute), reinterpret_cast<irr::gui::IGUIEditBox *>(m_pGui->getFocus()), l_pAttr);
                      l_pAttr->drop();
                      l_bRet = true;
                    }
                  }
                }
              }
            }
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        irr::gui::IGUIElement *l_pHover = m_pGui->getHovered();

        if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP) {
          if (l_pHover == m_pScene) {
            if (m_pScene->getSelected() != nullptr) {
              irr::gui::IGUIContextMenu *l_pMenu = m_pGui->addContextMenu(irr::core::recti(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y, 5, 5), m_pGui->getRootGUIElement());

              l_pMenu->addItem(L"Add Mesh Scene Node");
              l_pMenu->addItem(L"Add Empty Scene Node");

              if (m_pSelected != m_pNode) {
                l_pMenu->addItem(L"Delete Node");
                l_pMenu->addItem(L"Clone Node");
              }
            }

            l_bRet = true;
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_DOUBLE_CLICK) {
          for (irr::u32 l_iIndex = 0; l_iIndex < 8; l_iIndex++) {
            if (l_pHover == m_aTexture[l_iIndex]) {
              irr::io::path l_sFolder = m_pGui->getFileSystem()->getWorkingDirectory();

              std::wstring l_sImgFolder = helpers::s2ws(tracked::IGlobal::getInstance()->getSetting("texture_folder"));

              std::string l_sFile = platform::portableSelectFileOpen(L"Png Image (*.png)\0*.png\0Jpeg Image (*.jpg)\0*.jpg\0Bitmap Image (*.bmp)\0*.bmp\0", l_sImgFolder.c_str(), L"");

              m_pGui->getFileSystem()->changeWorkingDirectoryTo(l_sFolder);

              if (l_sFile != "") {
                tracked::IGlobal::getInstance()->setSetting("texture_folder", helpers::getFilepath(l_sFile));

                irr::u32 l_iTexture = m_pMaterial->getItemData(m_pMaterial->getSelected());
                if (m_pSelected != nullptr && m_pMaterial != nullptr && l_iTexture < m_pSelected->getMaterialCount()) {
                  m_pSelected->getMaterial(m_pMaterial->getItemData(m_pMaterial->getSelected())).setTexture(l_iIndex, m_pSmgr->getVideoDriver()->getTexture(l_sFile.c_str()));
                  updateMaterialTab();
                  break;
                }
              }

              l_bRet = true;
            }
          }

          if (!l_bRet) {
            for (int i = 0; i < 3; i++) {
              for (auto &l_tAttribute : m_vAttributes[i]) {
                if (l_pHover == std::get<2>(l_tAttribute) && l_pHover->getType() == irr::gui::EGUIET_TAB && std::get<3>(l_tAttribute) == irr::io::EAT_COLOR) {
                  irr::io::IAttributes *l_pAttr = getAttributesForEditing();

                  if (l_pAttr != nullptr) {
                    irr::gui::IGUITab *p = reinterpret_cast<irr::gui::IGUITab *>(l_pHover);

                    irr::video::SColor l_cColor = platform::portableSelectColor(p->getBackgroundColor());
                    l_pAttr->setAttribute(std::get<0>(l_tAttribute).c_str(), l_cColor);
                    commitAttributes(l_pAttr);
                    p->setBackgroundColor(l_cColor);
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





    CCmdDecoration::CInputLimiter::CInputLimiter(irr::gui::IGUIElement *a_pElement, irr::io::E_ATTRIBUTE_TYPE a_eType) : m_pElement(a_pElement) {
      switch (a_eType) {
	      case irr::io::EAT_INT         : m_sLimited = L"+-0123456789" ; break; 
	      case irr::io::EAT_FLOAT       : m_sLimited = L"+-0123456789."; break; 
        case irr::io::EAT_STRING      : break;
	      case irr::io::EAT_BOOL        : break;
	      case irr::io::EAT_COLOR       : m_sLimited = L"+-0123456789," ; break;
	      case irr::io::EAT_COLORF      : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_VECTOR3D    : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_POSITION2D  : m_sLimited = L"+-0123456789," ; break;
	      case irr::io::EAT_VECTOR2D    : m_sLimited = L"+-0123456789.,"; break;
	      case irr::io::EAT_RECT        : break;
	      case irr::io::EAT_MATRIX      : break;
	      case irr::io::EAT_QUATERNION  : break;
	      case irr::io::EAT_BBOX        : break;
	      case irr::io::EAT_PLANE       : break;
	      case irr::io::EAT_TRIANGLE3D  : break;
	      case irr::io::EAT_LINE2D      : break;
	      case irr::io::EAT_LINE3D      : break;
	      case irr::io::EAT_STRINGWARRAY: break;
	      case irr::io::EAT_FLOATARRAY  : break;
	      case irr::io::EAT_INTARRAY    : break;
	      case irr::io::EAT_BINARY      : break;
	      case irr::io::EAT_TEXTURE     : break;
	      case irr::io::EAT_USER_POINTER: break;
        case irr::io::EAT_DIMENSION2D : m_sLimited = L"+-0123456789,"; break;
      }

      m_vValid.push_back(irr::KEY_RETURN);
      m_vValid.push_back(irr::KEY_ESCAPE);
      m_vValid.push_back(irr::KEY_RIGHT);
      m_vValid.push_back(irr::KEY_LEFT);
      m_vValid.push_back(irr::KEY_DOWN);
      m_vValid.push_back(irr::KEY_UP);
      m_vValid.push_back(irr::KEY_END);
      m_vValid.push_back(irr::KEY_BACK);
      m_vValid.push_back(irr::KEY_HOME);
      m_vValid.push_back(irr::KEY_SHIFT);
      m_vValid.push_back(irr::KEY_DELETE);
    }

    CCmdDecoration::CInputLimiter::~CInputLimiter() {
    }

    bool CCmdDecoration::CInputLimiter::consumeEvent(const irr::SEvent &a_cEvent) {
      if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
        // Don't consume keys which are never invalid
        if (std::find(m_vValid.begin(), m_vValid.end(), a_cEvent.KeyInput.Key) != m_vValid.end())
          return false;

        if (a_cEvent.KeyInput.Control && a_cEvent.KeyInput.Key == irr::KEY_KEY_A)
          return false;

        // Only chars not found in the string are valid, i.e. not consumed
        if (m_sLimited.find_first_of(a_cEvent.KeyInput.Char) == std::string::npos)
          return true;
      }

      return false;
    }

    irr::gui::IGUIElement *CCmdDecoration::CInputLimiter::getElement() {
      return m_pElement;
    }
  }
}