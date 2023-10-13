#include <menu/datahandlers/CDataHandler_EditProfile.h>
#include <helpers/CTextureHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiImageList.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <Defines.h>
#include <random>

namespace dustbin {
  namespace menu {
    CDataHandler_EditProfile::CDataHandler_EditProfile(int a_iProfileIndex, const data::SPlayerData& a_cEditProfile) :
      IMenuDataHandler(),
      m_cEditProfile  (a_cEditProfile),
      m_iProfileIndex (a_iProfileIndex),
      m_bConfigCtrl   (false),
      m_pPreviewSmgr  (nullptr),
      m_pMarbleNode   (nullptr),
      m_pTextureRtt   (nullptr),
      m_sEditColor    ("")
    {
      std::vector<std::string> l_vColors = helpers::readLinesOfFile("data/colors.txt");

      for (std::vector<std::string>::iterator l_itColor = l_vColors.begin(); l_itColor != l_vColors.end(); l_itColor++) {
        std::vector<std::string> l_vDefault = helpers::splitString(*l_itColor, ',');
        while (l_vDefault.size() < 4)
          l_vDefault.push_back("");
        m_vDefaultColors.push_back(std::make_tuple(l_vDefault[0], l_vDefault[1], l_vDefault[2], l_vDefault[3]));
      }

      m_vDefaultPatterns = helpers::getTexturePatterns();

      std::vector<std::string> l_vNames = helpers::readLinesOfFile("data/names.txt");

      for (auto l_sName : l_vNames) {
        std::vector<std::string> l_vSplit = helpers::splitString(l_sName, ',');

        std::string l_sFirstName = l_vSplit.size() > 0 ? l_vSplit[0] : "";
        std::string l_sSurName   = l_vSplit.size() > 0 ? l_vSplit[1] : "";

        m_vDefaultNames.push_back(std::make_tuple(l_sFirstName, l_sSurName));
      }

      if (m_cEditProfile.m_sName == "")
        generateDefaultName();

      if (m_cEditProfile.m_sTexture == "")
        m_cEditProfile.m_sTexture = createRandomTexture();

      
      if (a_iProfileIndex == -1) {
        // New Profile
        m_vSteps.push_back(enEditProfileStep::Name    );
        m_vSteps.push_back(enEditProfileStep::Ctrls   );
        m_vSteps.push_back(enEditProfileStep::AiHelp  );
        m_vSteps.push_back(enEditProfileStep::Texture );
        m_vSteps.push_back(enEditProfileStep::Overview);
      }
      else {
        // Edit existing profile
        m_vSteps.push_back(enEditProfileStep::Data    );
        m_vSteps.push_back(enEditProfileStep::Name    );
        m_vSteps.push_back(enEditProfileStep::Ctrls   );
        m_vSteps.push_back(enEditProfileStep::AiHelp  );
        m_vSteps.push_back(enEditProfileStep::Texture );
      }

      m_itStep = m_vSteps.begin();
      setEditProfileStep(enDirection::NoChange);

      updateAiHelp((int)m_cEditProfile.m_eAiHelp);

      setElementVisibility("EditProfileNew" , a_iProfileIndex == -1);
      setElementVisibility("EditProfileEdit", a_iProfileIndex != -1);

      m_pCtrl = reinterpret_cast<gui::CControllerUi_Game *>(findElement("EditProfile_ControlUi", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId));

      m_pPreviewSmgr = CGlobal::getInstance()->getIrrlichtDevice()->getSceneManager()->createNewSceneManager();

      if (m_pPreviewSmgr != nullptr) {
        m_pPreviewSmgr->loadScene("data/scenes/texture_scene.xml");
        irr::scene::ICameraSceneNode *l_pCam = m_pPreviewSmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
        l_pCam->setAspectRatio(1.0f);
        m_pTextureRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");

        if (m_pTextureRtt != nullptr) {
          irr::gui::IGUIImage *l_pTexture = reinterpret_cast<irr::gui::IGUIImage *>(findElement("EditProfile_TextureRtt", irr::gui::EGUIET_IMAGE));
          if (l_pTexture != nullptr) {
            l_pTexture->setImage(m_pTextureRtt);
          }

          m_pMarbleNode = m_pPreviewSmgr->getSceneNodeFromName("marble");
        }
      }

      updatePatterns();
      printf("Ready.\n");
    }

    CDataHandler_EditProfile::~CDataHandler_EditProfile() {
      if (m_pPreviewSmgr != nullptr)
        m_pPreviewSmgr->drop();
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_EditProfile::handleIrrlichtEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (m_pCtrl != nullptr) {
        switch (m_pCtrl->getMode()) {
          case gui::CControllerUi_Game::enMode::Test:
          case gui::CControllerUi_Game::enMode::Wizard:
            if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT)
              l_bRet = m_pCtrl->OnEvent(a_cEvent);

            if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT)
              l_bRet = m_pCtrl->OnJoystickEvent(a_cEvent);

            break;

          default:
            break;
        }
      }

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (l_sCaller == "EditProfile_GenerateName") {
            generateDefaultName();
            l_bRet = true;
          }
          else if (l_sCaller == "EditProfileOk") {
            l_bRet = setEditProfileStep(enDirection::Next);
          }
          else if (l_sCaller == "EditProfileCancel") {
            l_bRet = setEditProfileStep(enDirection::Previous);
          }
          else if (l_sCaller == "EditProfile_TestCtrl") {
            if (m_pCtrl != nullptr) {
              if (m_pCtrl->getMode() == gui::CControllerUi::enMode::Display)
                m_pCtrl->startTest();
              else
                m_pCtrl->setMode(gui::CControllerUi::enMode::Display);
            }

            l_bRet = true;
          }
          else if (l_sCaller == "EditProfile_ConfigCtrl") {
            if (m_pCtrl != nullptr) {
              if (m_pCtrl->getMode() == gui::CControllerUi::enMode::Display) {
                m_pCtrl->startWizard();
                m_bConfigCtrl = true;
              }
              else m_pCtrl->setMode(gui::CControllerUi::enMode::Display);
            }

            l_bRet = true;
          }
          else if (l_sCaller == "EditProfile_EditParams") {
            l_bRet = setElementVisibility("EditProfile_TextureParams", true);
          }
          else if (l_sCaller == "EditProfile_EditParamsClose") {
            l_bRet = setElementVisibility("EditProfile_TextureParams", false);
          }
          else if (l_sCaller == "EditProfile_RandomTexture") {
            m_cEditProfile.m_sTexture = createRandomTexture();
            updateMarbleTexture(m_cEditProfile.m_sTexture);
            l_bRet = true;
          }
          else if (l_sCaller == "EditProfile_BtnPattern") {
            l_bRet = setElementVisibility("pattern_dialog", true);
          }
          else if (l_sCaller == "btn_pattern_close") {
            l_bRet = setElementVisibility("pattern_dialog", false);
          }
          else if (l_sCaller == "btn_color_cancel") {
            l_bRet = setElementVisibility("color_dialog", false);
          }
          else if (l_sCaller == "btn_color_ok") {
            irr::gui::IGUITab *l_pColor = reinterpret_cast<irr::gui::IGUITab *>(findElement("color_display", irr::gui::EGUIET_TAB));
            if (l_pColor != nullptr) {
              irr::video::SColor l_cColor = l_pColor->getBackgroundColor();

              irr::u32 l_iR = std::max(0, std::min(255, (int)l_cColor.getRed  ()));
              irr::u32 l_iG = std::max(0, std::min(255, (int)l_cColor.getGreen()));
              irr::u32 l_iB = std::max(0, std::min(255, (int)l_cColor.getBlue ()));

              char s[255];
              memset(s, 0, 255);

              sprintf(s, "%2X%2X%2X", l_iR, l_iG, l_iB);
              for (int i = 0; i < 255 && s[i] != '\0'; i++)
                if (s[i] == ' ') s[i] = '0';

              modifyTextureParameter(m_sEditColor, s);
              setElementVisibility("color_dialog", false);
              l_bRet = true;
            }
          }
          else if (l_sCaller == "pick_color") {
            gui::CReactiveLabel *p = reinterpret_cast<gui::CReactiveLabel *>(a_cEvent.GUIEvent.Caller);
            if (p != nullptr) {
              initializeColorDialog(helpers::ws2s(p->getText()));
              l_bRet = true;
            }
          }
          else {
            std::map<std::string, std::string> l_mBtnColor = {
              { "EditProfile_BtnNoFore" , "numbercolor"  },
              { "EditProfile_BtnNoBack" , "numberback"   },
              { "EditProfile_BtnNoRing" , "ringcolor"    },
              { "EditProfile_BtnNoFrame", "numberborder" },
              { "EditProfile_PattrnBack", "patternback"  },
              { "EditProfile_PattrnFore", "patterncolor" }
            };

            if (l_mBtnColor.find(l_sCaller) != l_mBtnColor.end()) {
              std::string l_sType;
              std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, m_cEditProfile.m_sTexture);

              if (l_mParams.find(l_mBtnColor[l_sCaller]) != l_mParams.end()) {
                m_sEditColor = l_mBtnColor[l_sCaller];
                initializeColorDialog(l_mParams[l_mBtnColor[l_sCaller]]);
                l_bRet = setElementVisibility("color_dialog", true);
              }
            }
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
          if (l_sCaller == "EditProfile_AiHelp") {
            updateAiHelp(reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected());
          }
          else if (l_sCaller == "EditProfile_ControlType") {
            int l_iCtrl = reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected();
            updateCtrlUi(l_iCtrl);

            switch (l_iCtrl) {
              // Keyboard
              case 0:
                m_cEditProfile.m_sControls = helpers::getDefaultGameCtrl_Keyboard();
                if (m_pCtrl != nullptr)
                  m_pCtrl->setController(m_cEditProfile.m_sControls);
                break;

              // Gamepad
              case 1:
                m_cEditProfile.m_sControls = helpers::getDefaultGameCtrl_Gamepad();
                if (m_pCtrl != nullptr)
                  m_pCtrl->setController(m_cEditProfile.m_sControls);
                break;

              // Touch
              case 2:
                m_cEditProfile.m_sControls = "DustbinTouchControl";
                break;

              // Gyro
              case 3:
                m_cEditProfile.m_sControls = "DustbinGyroscope";
                break;
            }
          }
          else if (l_sCaller == "scrollbar_red" || l_sCaller == "scrollbar_green" || l_sCaller == "scrollbar_blue") {
            updateColorDialog();
          }
          else printf("Scrollbar Changed: \"%s\"\n", l_sCaller.c_str());
        }
      }
      else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventImageSelected && a_cEvent.UserEvent.UserData2 == c_iEventImageSelected) {
          gui::CGuiImageList *l_pList = reinterpret_cast<gui::CGuiImageList *>(findElement("PatternList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId));
          if (l_pList != nullptr) {
            modifyTextureParameter("pattern", l_pList->getSelectedData());

            irr::gui::IGUIElement *l_pRoot = findElement("pattern_dialog");
            if (l_pRoot != nullptr)
              l_pRoot->setVisible(false);
          }
        }
      }

      return l_bRet;
    }

    /**
    * Get the index of the edited profile (-1 == new profile)
    * @return the index of the edited profile
    */
    int CDataHandler_EditProfile::getProfileIndex() {
      return m_iProfileIndex;
    }

    /**
    * Get the edited profile
    * @return the edited profile
    */
    const data::SPlayerData& CDataHandler_EditProfile::getEditedProfile() {
      return m_cEditProfile;
    }

    /**
    * Generate a random name and fill the edit field
    */
    void CDataHandler_EditProfile::generateDefaultName() {
      if (m_vDefaultNames.size() > 0) {
        std::string l_sFirstName = std::get<0>(m_vDefaultNames[std::rand() % m_vDefaultNames.size()]);
        std::string l_sSurName   = std::get<1>(m_vDefaultNames[std::rand() % m_vDefaultNames.size()]);

        std::string l_sName = l_sFirstName + " " + l_sSurName;

        m_cEditProfile.m_sName      = l_sName;
        m_cEditProfile.m_sShortName = l_sFirstName.substr(0, 2) + l_sSurName.substr(0, 3);

        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElement("EditProfile_EditName", irr::gui::EGUIET_EDIT_BOX));

        if (l_pEdit != nullptr) {
          l_pEdit->setText(helpers::s2ws(m_cEditProfile.m_sName).c_str());
        }

        l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElement("EditProfile_EditShort", irr::gui::EGUIET_EDIT_BOX));

        if (l_pEdit != nullptr) {
          l_pEdit->setText(helpers::s2ws(m_cEditProfile.m_sShortName).c_str());
        }
      }
      else printf("No default names loaded.\n");
    }

    /**
    * Set to another step in the dialog
    * @param a_eStep the new step
    * @return true if the new active UI element was found
    */
    bool CDataHandler_EditProfile::setEditProfileStep(enDirection a_eDirection) {
      irr::gui::IGUIElement *l_pRoot = nullptr;

      std::string l_sItems[] = {
        "EditProfile_Data"   ,
        "EditProfile_Name"   ,
        "EditProfile_AiHelp" ,
        "EditProfile_Control",
        "EditProfile_Texture",
        "EditProfile_Overview",
        ""
      };

      for (int i = 0; l_sItems[i] != ""; i++) {
        irr::gui::IGUIElement *p = findElement(l_sItems[i]);
        if (p != nullptr)
          p->setVisible(false);
      }

      switch (a_eDirection) {
        case enDirection::Previous: if ( m_itStep      == m_vSteps.begin()) return false; else m_itStep--; break;
        case enDirection::Next    : if ((m_itStep + 1) == m_vSteps.end  ()) return false; else m_itStep++; break;
        case enDirection::NoChange: break;
      }

      switch (*m_itStep) {
        case enEditProfileStep::Data    : l_pRoot = findElement("EditProfile_Data"    ); break;
        case enEditProfileStep::Name    : l_pRoot = findElement("EditProfile_Name"    ); break;
        case enEditProfileStep::AiHelp  : l_pRoot = findElement("EditProfile_AiHelp"  ); break;
        case enEditProfileStep::Ctrls   : l_pRoot = findElement("EditProfile_Control" ); break;
        case enEditProfileStep::Texture : l_pRoot = findElement("EditProfile_Texture" ); break;
        case enEditProfileStep::Overview: l_pRoot = findElement("EditProfile_Overview"); break;
      }

      if (l_pRoot != nullptr)
        l_pRoot->setVisible(true);

      switch (*m_itStep) {
        case dustbin::menu::enEditProfileStep::Unknown:
          break;

        case dustbin::menu::enEditProfileStep::Data:
          break;

        case dustbin::menu::enEditProfileStep::Name:
          if (m_cEditProfile.m_sName != "") {
            irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(findElement("EditProfile_EditName", irr::gui::EGUIET_EDIT_BOX));
            if (p != nullptr)
              p->setText(helpers::s2ws(m_cEditProfile.m_sName).c_str());
          }

          if (m_cEditProfile.m_sShortName != "") {
            irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(findElement("EditProfile_EditShort", irr::gui::EGUIET_EDIT_BOX));
            if (p != nullptr)
              p->setText(helpers::s2ws(m_cEditProfile.m_sShortName).c_str());
          }
          break;
        case dustbin::menu::enEditProfileStep::AiHelp: {
          gui::CSelector *m_pAiHelp = reinterpret_cast<gui::CSelector *>(findElement("EditProfile_AiHelp", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId));
          if (m_pAiHelp != nullptr)
            m_pAiHelp->setSelected((int)m_cEditProfile.m_eAiHelp);
          break;
        }
        case dustbin::menu::enEditProfileStep::Ctrls: {
          gui::CSelector *p = reinterpret_cast<gui::CSelector *>(findElement("EditProfile_ControlType", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId));

          if (m_cEditProfile.m_sControls == "DustbinTouchControl") {
            updateCtrlUi(2);
            if (p != nullptr)
              p->setSelected(2);
          }
          else if (m_cEditProfile.m_sControls == "DustbinGyroscope") {
            updateCtrlUi(3);
            if (p != nullptr)
              p->setSelected(3);
          }
          else if (m_pCtrl != nullptr) {
            m_pCtrl->setController(m_cEditProfile.m_sControls);
            if (m_pCtrl->getControlType() == gui::CControllerUi::enControl::Joystick) {
              updateCtrlUi(1);
              if (p != nullptr)
                p->setSelected(1);
            }
            else {
              updateCtrlUi(0);
              if (p != nullptr)
                p->setSelected(0);
            }
          }
          break;
        }

        case dustbin::menu::enEditProfileStep::Texture:
          updateMarbleTexture(m_cEditProfile.m_sTexture);
          break;

        case dustbin::menu::enEditProfileStep::Overview:
          break;

        default:
          break;
      }

      gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(findElement("EditProfileOk", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId));

      if (l_pOk != nullptr)
        l_pOk->setImage(std::string((m_itStep + 1) == m_vSteps.end()  ? "data/images/btn_ok.png" : "data/images/arrow_right.png"));

      gui::CMenuButton *l_pCancel = reinterpret_cast<gui::CMenuButton *>(findElement("EditProfileCancel", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId));

      if (l_pCancel != nullptr)
        l_pCancel->setImage(std::string(m_itStep == m_vSteps.begin() ? "data/images/btn_cancel.png" : "data/images/arrow_left.png"));

      return l_pRoot != nullptr;
    }

    /**
    * Update the hint for the AI help dialog and the AI help in m_cEditProfile
    * @param a_iAiHelp the new AI help level
    */
    void CDataHandler_EditProfile::updateAiHelp(int a_iAiHelp) {
      if (a_iAiHelp >= (int)data::SPlayerData::enAiHelp::Off && a_iAiHelp <= (int)data::SPlayerData::enAiHelp::BotMb3) {
        m_cEditProfile.m_eAiHelp = (data::SPlayerData::enAiHelp)a_iAiHelp;

        for (int i = 0; i < 8; i++) {
          irr::gui::IGUIElement *l_pHint = findElement("EditProfile_AiHelpInfo" + std::to_string(i));
          if (l_pHint != nullptr)
            l_pHint->setVisible(i == a_iAiHelp);
        }
      }
    }

    /**
    * Check for controller if we are in the correct state
    */
    void CDataHandler_EditProfile::runDataHandler() {
      if (*m_itStep == enEditProfileStep::Ctrls && m_pCtrl != nullptr) {
        if (m_bConfigCtrl && m_pCtrl->getMode() != gui::CControllerUi_Game::enMode::Wizard) {
          printf("Update Profile Controls.\n");
          m_cEditProfile.m_sControls = m_pCtrl->serialize();
          m_bConfigCtrl = false;
        }
      }
      else if (*m_itStep == enEditProfileStep::Texture) {
        if (m_pTextureRtt != nullptr && m_pPreviewSmgr != nullptr) {
          m_pDrv->setRenderTarget(m_pTextureRtt, true, true);
          m_pPreviewSmgr->drawAll();
          m_pDrv->setRenderTarget(nullptr, false, false);
        }
      }
    }


    /**
    * Update the texture of the preview marble in the texture wizard step
    * @param a_sTexture the texture string of the marble
    */
    void CDataHandler_EditProfile::updateMarbleTexture(const std::string &a_sTexture) {
      if (m_pPreviewSmgr != nullptr && m_pMarbleNode != nullptr) {
        m_pMarbleNode->getMaterial(0).setTexture(0, helpers::createTexture(a_sTexture != "" ? a_sTexture : "default://number=1", m_pDrv, m_pFs));
      }
    }

    /**
    * Update the controller UI
    * @param a_iCtrl the controller index (0 == keyboard, 1 == gamepad, 2 == touch, 3 == gyroscope)
    */
    void CDataHandler_EditProfile::updateCtrlUi(int a_iCtrl) {
      gui::CSelector *l_pCtrl = reinterpret_cast<gui::CSelector *>(findElement("EditProfile_ControlType", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId));

      if (l_pCtrl != nullptr) {
        irr::gui::IGUIElement* l_pItems[] = {
          findElement("EditProfile_JoyKeyTab"     , (irr::gui::EGUI_ELEMENT_TYPE)     gui::g_ControllerUiGameId),
          findElement("EditProfile_ControlUiTouch",                              irr::gui::EGUIET_IMAGE        ),
          findElement("EditProfile_ControlUiGyro" ,                              irr::gui::EGUIET_IMAGE        )
        };

        int l_iCtrl = l_pCtrl->getSelected();

        switch (l_iCtrl) {
          // Keyboard
          case 0: {
            if (l_pItems[0] != nullptr) l_pItems[0]->setVisible(true);
            if (l_pItems[1] != nullptr) l_pItems[1]->setVisible(false);
            if (l_pItems[2] != nullptr) l_pItems[2]->setVisible(false);
            break;
          }

          // Gamepad
          case 1: {
            if (l_pItems[0] != nullptr) l_pItems[0]->setVisible(true);
            if (l_pItems[1] != nullptr) l_pItems[1]->setVisible(false);
            if (l_pItems[2] != nullptr) l_pItems[2]->setVisible(false);
            break;
          }

          // Touch
          case 2: {
            if (l_pItems[0] != nullptr) l_pItems[0]->setVisible(false);
            if (l_pItems[1] != nullptr) l_pItems[1]->setVisible(true);
            if (l_pItems[2] != nullptr) l_pItems[2]->setVisible(false);
            break;
          }

          // Gyro
          case 3: {
            if (l_pItems[0] != nullptr) l_pItems[0]->setVisible(false);
            if (l_pItems[1] != nullptr) l_pItems[1]->setVisible(false);
            if (l_pItems[2] != nullptr) l_pItems[2]->setVisible(true);
            break;
          }
        }
      }
    }

    /**
    * Generate a random texture
    * @return a string with random texture parameters
    */
    std::string CDataHandler_EditProfile::createRandomTexture() {
      std::string l_sRet = "";

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(m_vDefaultColors.begin(), m_vDefaultColors.end(), l_cRe);
      }

      std::tuple<std::string, std::string, std::string, std::string> l_tColor = *m_vDefaultColors.begin();

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(m_vDefaultPatterns.begin(), m_vDefaultPatterns.end(), l_cRe);
      }

      std::string l_sPattern = *m_vDefaultPatterns.begin();

      std::vector<int> l_vIndex = { 0, 1, 2 };

      std::vector<std::vector<std::string>> l_vElements;

      if (std::get<3>(l_tColor) == "") {
        l_vElements.push_back({ "numbercolor", "ringcolor" });
        l_vElements.push_back({ "patterncolor" });
        l_vElements.push_back({ "numberback", "patternback", "numberborder" });
      }
      else {
        l_vIndex.push_back(3);

        l_vElements.push_back({ "numbercolor" });
        l_vElements.push_back({ "ringcolor" });
        l_vElements.push_back({ "patterncolor" });
        l_vElements.push_back({ "numberback", "patternback", "numberborder" });
      }

      {
        std::random_device l_cRd { };
        std::default_random_engine l_cRe { l_cRd() };
        std::shuffle(l_vIndex.begin(), l_vIndex.end(), l_cRe);
      }

      for (std::vector<int>::iterator l_itIndex = l_vIndex.begin(); l_itIndex != l_vIndex.end(); l_itIndex++) {
        for (std::vector<std::string>::iterator l_itPart = l_vElements[*l_itIndex].begin(); l_itPart != l_vElements[*l_itIndex].end(); l_itPart++) {
          if (l_sRet == "")
            l_sRet = "generate://";
          else
            l_sRet += "&";

          l_sRet += *l_itPart + "=";

          switch (*l_itIndex) {
          case 0: l_sRet += std::get<0>(l_tColor); break;
          case 1: l_sRet += std::get<1>(l_tColor); break;
          case 2: l_sRet += std::get<2>(l_tColor); break;
          case 3: l_sRet += std::get<3>(l_tColor); break;
          }
        }
      }

      return l_sRet + "&pattern=" + l_sPattern;
    }

    /**
    * Set the visibility of a GUI element
    * @param a_sName name of the GUI element
    * @param a_bVisible new visibility flag
    * @return true if the element was found
    */
    bool CDataHandler_EditProfile::setElementVisibility(const std::string& a_sName, bool a_bVisible) {
      irr::gui::IGUIElement *p = findElement(a_sName);
      if (p != nullptr)
        p->setVisible(a_bVisible);

      return p != nullptr;
    }

    /**
    * Initialize the color dialog
    * @param a_cColor the initial color
    */
    void CDataHandler_EditProfile::initializeColorDialog(const std::string& a_sColor) {
      std::vector<std::tuple<std::string, std::string, int>> l_mData = {
        std::make_tuple("scrollbar_red"  , "value_red"  , 0),
        std::make_tuple("scrollbar_green", "value_green", 2),
        std::make_tuple("scrollbar_blue" , "value_blue" , 4)
      };

      irr::video::SColor l_cColor;

      for (auto l_cData : l_mData) {
        irr::gui::IGUIScrollBar  *p = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElement(std::get<0>(l_cData), irr::gui::EGUIET_SCROLL_BAR ));
        irr::gui::IGUIStaticText *t = reinterpret_cast<irr::gui::IGUIStaticText *>(findElement(std::get<1>(l_cData), irr::gui::EGUIET_STATIC_TEXT));

        std::string l_sSub = a_sColor.substr(std::get<2>(l_cData), 2);
        char s[3] = {
          l_sSub.c_str()[0],
          l_sSub.c_str()[1],
          '\0'
        };

        char *l_pEnd;

        int l_iValue = (int)strtoul(s, &l_pEnd, 16);

        if (p != nullptr)
          p->setPos(l_iValue);

        if (t != nullptr)
          t->setText(std::to_wstring(l_iValue).c_str());

        switch (std::get<2>(l_cData)) {
          case 0: l_cColor.setRed  (l_iValue); break;
          case 2: l_cColor.setGreen(l_iValue); break;
          case 4: l_cColor.setBlue (l_iValue); break;
        }
      }

      irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElement("color_display", irr::gui::EGUIET_TAB));
      if (l_pTab != nullptr)
        l_pTab->setBackgroundColor(l_cColor);
    }

    /**
    * Update the color dialog
    */
    void CDataHandler_EditProfile::updateColorDialog() {
      irr::video::SColor l_cColor;

      std::map<std::string, irr::u8> l_mValues = {
        { "value_red"  , 0 },
        { "value_green", 0 },
        { "value_blue" , 0 }
      };

      std::vector<std::tuple<std::string, std::string>> l_vColors = {
        std::make_tuple("scrollbar_red"  , "value_red"  ),
        std::make_tuple("scrollbar_green", "value_green"),
        std::make_tuple("scrollbar_blue" , "value_blue" )
      };

      for (auto l_cData : l_vColors) {
        irr::gui::IGUIScrollBar  *p = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElement(std::get<0>(l_cData), irr::gui::EGUIET_SCROLL_BAR ));
        irr::gui::IGUIStaticText *t = reinterpret_cast<irr::gui::IGUIStaticText *>(findElement(std::get<1>(l_cData), irr::gui::EGUIET_STATIC_TEXT));

        if (p != nullptr && t != nullptr) {
          std::wstring s = std::to_wstring(p->getPos());
          t->setText(s.c_str());
          
          if (l_mValues.find(std::get<1>(l_cData)) != l_mValues.end())
            l_mValues[std::get<1>(l_cData)] = (irr::u8)p->getPos();
        }

        if (std::get<1>(l_cData) == "value_red")
          l_cColor.setRed(p->getPos());
        else if (std::get<1>(l_cData) == "value_green")
          l_cColor.setGreen(p->getPos());
        else if (std::get<1>(l_cData) == "value_blue")
          l_cColor.setBlue(p->getPos());
      }

      irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElement("color_display", irr::gui::EGUIET_TAB));
      if (l_pTab != nullptr)
        l_pTab->setBackgroundColor(l_cColor);
    }

    /**
    * Modify a texture parameter
    * @param a_sKey the parameter key
    * @param a_sValue the new value
    */
    void CDataHandler_EditProfile::modifyTextureParameter(const std::string& a_sKey, const std::string& a_sValue) {
      std::string l_sTextureType;
      std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sTextureType, m_cEditProfile.m_sTexture);
      if (l_mParams.find(a_sKey) != l_mParams.end()) {
        if (a_sKey == "numberback") {
          if (l_mParams.find("numberborder") != l_mParams.end() && l_mParams["numberborder"] == l_mParams["numberback"])
            l_mParams["numberborder"] = a_sValue;
        }

        l_mParams[a_sKey] = a_sValue;
        
        m_cEditProfile.m_sTexture = "";

        for (auto l_cParam : l_mParams) {
          if (m_cEditProfile.m_sTexture != "")
            m_cEditProfile.m_sTexture += "&";

          m_cEditProfile.m_sTexture += l_cParam.first + "=" + l_cParam.second;
        }
        m_cEditProfile.m_sTexture = "generate://" + m_cEditProfile.m_sTexture;
        updateMarbleTexture(m_cEditProfile.m_sTexture);
      }
    }

    /**
    * Update the pattern images starting with "m_iPatternPage"
    * @see m_iPatternPage
    */
    void CDataHandler_EditProfile::updatePatterns() {
      gui::CGuiImageList *l_pPatternList = reinterpret_cast<gui::CGuiImageList *>(findElement("PatternList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId));

      if (l_pPatternList != nullptr) {
        irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/patterns/patterns.xml");

        if (l_pXml) {
          std::vector<gui::CGuiImageList::SListImage> l_vPatterns;

          while (l_pXml->read()) {
            if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
              std::string l_sNode = l_pXml->getNodeName();

              if (l_sNode == "pattern") {
                std::string l_sPattern = l_pXml->getAttributeValueSafe("file");

                if (l_sPattern != "") {
                  std::string l_sPath = "data/patterns/" + l_sPattern;

                  if (m_pFs->existFile(l_sPath.c_str())) {
                    l_vPatterns.push_back(gui::CGuiImageList::SListImage(l_sPath, l_sPattern, l_sPattern, -1));
                  }
                }
              }
            }
          }

          l_pPatternList->setImageList(l_vPatterns);

          irr::gui::IGUIEditBox *l_pPattern = reinterpret_cast<irr::gui::IGUIEditBox *>(findElement("texture_pattern", irr::gui::EGUIET_EDIT_BOX));

          if (l_pPattern != nullptr)
            l_pPatternList->setSelected(helpers::ws2s(l_pPattern->getText()), false);

          l_pXml->drop();
        }
      }
    }
  }
}
