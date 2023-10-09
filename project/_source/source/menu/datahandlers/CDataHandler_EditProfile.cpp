#include <menu/datahandlers/CDataHandler_EditProfile.h>
#include <helpers/CTextureHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <random>

namespace dustbin {
  namespace menu {
    CDataHandler_EditProfile::CDataHandler_EditProfile(int a_iProfileIndex, const data::SPlayerData& a_cEditProfile) :
      IMenuDataHandler(),
      m_cEditProfile  (a_cEditProfile),
      m_pGui          (CGlobal::getInstance()->getGuiEnvironment()),
      m_pFs           (CGlobal::getInstance()->getFileSystem()),
      m_pDrv          (CGlobal::getInstance()->getVideoDriver()),
      m_eStep         (enEditProfileStep::Unknown),
      m_iProfileIndex (a_iProfileIndex),
      m_bConfigCtrl   (false),
      m_pPreviewSmgr  (nullptr),
      m_pMarbleNode   (nullptr),
      m_pTextureRtt   (nullptr)
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

      setEditProfileStep(enEditProfileStep::Name);
      updateAiHelp((int)m_cEditProfile.m_eAiHelp);

      irr::gui::IGUIStaticText *l_pHeadline = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("EditProfileNew", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
      if (l_pHeadline != nullptr)
        l_pHeadline->setVisible(a_iProfileIndex == -1);

      l_pHeadline = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("EditProfileEdit", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
      if (l_pHeadline != nullptr)
        l_pHeadline->setVisible(a_iProfileIndex != -1);

      m_pCtrl = reinterpret_cast<gui::CControllerUi_Game *>(helpers::findElementByNameAndType("EditProfile_ControlUi", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId, m_pGui->getRootGUIElement()));

      m_pPreviewSmgr = CGlobal::getInstance()->getIrrlichtDevice()->getSceneManager()->createNewSceneManager();

      if (m_pPreviewSmgr != nullptr) {
        m_pPreviewSmgr->loadScene("data/scenes/texture_scene.xml");
        irr::scene::ICameraSceneNode *l_pCam = m_pPreviewSmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
        l_pCam->setAspectRatio(1.0f);
        m_pTextureRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");

        if (m_pTextureRtt != nullptr) {
          irr::gui::IGUIImage *l_pTexture = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("EditProfile_TextureRtt", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
          if (l_pTexture != nullptr) {
            l_pTexture->setImage(m_pTextureRtt);
          }

          m_pMarbleNode = m_pPreviewSmgr->getSceneNodeFromName("marble");
        }
      }

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
            switch (m_eStep) {
              case enEditProfileStep::Name   : l_bRet = setEditProfileStep(enEditProfileStep::AiHelp  ); break;
              case enEditProfileStep::AiHelp : l_bRet = setEditProfileStep(enEditProfileStep::Ctrls   ); break;
              case enEditProfileStep::Ctrls  : l_bRet = setEditProfileStep(enEditProfileStep::Texture ); break;
              case enEditProfileStep::Texture: l_bRet = setEditProfileStep(enEditProfileStep::Overview); break;
            }
          }
          else if (l_sCaller == "EditProfileCancel") {
            switch (m_eStep) {
              case enEditProfileStep::Overview: l_bRet = setEditProfileStep(enEditProfileStep::Texture); break;
              case enEditProfileStep::Texture : l_bRet = setEditProfileStep(enEditProfileStep::Ctrls  ); break;
              case enEditProfileStep::Ctrls   : l_bRet = setEditProfileStep(enEditProfileStep::AiHelp ); break;
              case enEditProfileStep::AiHelp  : l_bRet = setEditProfileStep(enEditProfileStep::Name   ); break;
            }
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
            gui::CMenuBackground *l_pWindow = reinterpret_cast<gui::CMenuBackground *>(helpers::findElementByNameAndType("EditProfile_TextureParams", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (l_pWindow != nullptr)
              l_pWindow->setVisible(!l_pWindow->isVisible());

            l_bRet = true;
          }
          else if (l_sCaller == "EditProfile_RandomTexture") {
            m_cEditProfile.m_sTexture = createRandomTexture();
            updateMarbleTexture(m_cEditProfile.m_sTexture);
            l_bRet = true;
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
          else printf("Scrollbar Changed: \"%s\"\n", l_sCaller.c_str());
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

        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(helpers::findElementByNameAndType("EditProfile_EditName", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

        if (l_pEdit != nullptr) {
          l_pEdit->setText(helpers::s2ws(m_cEditProfile.m_sName).c_str());
        }

        l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(helpers::findElementByNameAndType("EditProfile_EditShort", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

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
    bool CDataHandler_EditProfile::setEditProfileStep(enEditProfileStep a_eStep) {
      irr::gui::IGUIElement *l_pRoot = nullptr;

      std::string l_sItems[] = {
        "EditProfile_Name"   ,
        "EditProfile_AiHelp" ,
        "EditProfile_Control",
        "EditProfile_Texture",
        "EditProfile_Overview",
        ""
      };

      for (int i = 0; l_sItems[i] != ""; i++) {
        irr::gui::IGUIElement *p = helpers::findElementByName(l_sItems[i], m_pGui->getRootGUIElement());
        if (p != nullptr)
          p->setVisible(false);
      }

      m_eStep = a_eStep;

      switch (m_eStep) {
        case enEditProfileStep::Name    : l_pRoot = helpers::findElementByName("EditProfile_Name"    , m_pGui->getRootGUIElement()); break;
        case enEditProfileStep::AiHelp  : l_pRoot = helpers::findElementByName("EditProfile_AiHelp"  , m_pGui->getRootGUIElement()); break;
        case enEditProfileStep::Ctrls   : l_pRoot = helpers::findElementByName("EditProfile_Control" , m_pGui->getRootGUIElement()); break;
        case enEditProfileStep::Texture : l_pRoot = helpers::findElementByName("EditProfile_Texture" , m_pGui->getRootGUIElement()); break;
        case enEditProfileStep::Overview: l_pRoot = helpers::findElementByName("EditProfile_Overview", m_pGui->getRootGUIElement()); break;
      }

      if (l_pRoot != nullptr)
        l_pRoot->setVisible(true);

      switch (m_eStep) {
        case dustbin::menu::enEditProfileStep::Unknown:
          break;
        case dustbin::menu::enEditProfileStep::Name:
          if (m_cEditProfile.m_sName != "") {
            irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(helpers::findElementByNameAndType("EditProfile_EditName", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(m_cEditProfile.m_sName).c_str());
          }

          if (m_cEditProfile.m_sShortName != "") {
            irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(helpers::findElementByNameAndType("EditProfile_EditShort", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(m_cEditProfile.m_sShortName).c_str());
          }
          break;
        case dustbin::menu::enEditProfileStep::AiHelp: {
          gui::CSelector *m_pAiHelp = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("EditProfile_AiHelp", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (m_pAiHelp != nullptr)
            m_pAiHelp->setSelected((int)m_cEditProfile.m_eAiHelp);
          break;
        }
        case dustbin::menu::enEditProfileStep::Ctrls: {
          gui::CSelector *p = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("EditProfile_ControlType", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

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

      gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("EditProfileOk", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

      if (l_pOk != nullptr)
        l_pOk->setImage(std::string(m_eStep == enEditProfileStep::Overview ? "data/images/btn_ok.png" : "data/images/arrow_right.png"));

      gui::CMenuButton *l_pCancel = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("EditProfileCancel", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

      if (l_pCancel != nullptr)
        l_pCancel->setImage(std::string(m_eStep == enEditProfileStep::Name ? "data/images/btn_cancel.png" : "data/images/arrow_left.png"));

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
          irr::gui::IGUIElement *l_pHint = helpers::findElementByName("EditProfile_AiHelpInfo" + std::to_string(i), m_pGui->getRootGUIElement());
          if (l_pHint != nullptr)
            l_pHint->setVisible(i == a_iAiHelp);
        }
      }
    }

    /**
    * Check for controller if we are in the correct state
    */
    void CDataHandler_EditProfile::runDataHandler() {
      if (m_eStep == enEditProfileStep::Ctrls && m_pCtrl != nullptr) {
        if (m_bConfigCtrl && m_pCtrl->getMode() != gui::CControllerUi_Game::enMode::Wizard) {
          printf("Update Profile Controls.\n");
          m_cEditProfile.m_sControls = m_pCtrl->serialize();
          m_bConfigCtrl = false;
        }
      }
      else if (m_eStep == enEditProfileStep::Texture) {
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
      gui::CSelector *l_pCtrl = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("EditProfile_ControlType", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

      if (l_pCtrl != nullptr) {
        irr::gui::IGUIElement* l_pItems[] = {
          helpers::findElementByNameAndType("EditProfile_JoyKeyTab"     , (irr::gui::EGUI_ELEMENT_TYPE)     gui::g_ControllerUiGameId, m_pGui->getRootGUIElement()),
          helpers::findElementByNameAndType("EditProfile_ControlUiTouch",                              irr::gui::EGUIET_IMAGE        , m_pGui->getRootGUIElement()),
          helpers::findElementByNameAndType("EditProfile_ControlUiGyro" ,                              irr::gui::EGUIET_IMAGE        , m_pGui->getRootGUIElement())
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
  }
}
