#include <menu/datahandlers/CDataHandler_EditProfile.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <random>

namespace dustbin {
  namespace menu {
    CDataHandler_EditProfile::CDataHandler_EditProfile(std::vector<data::SPlayerData>::iterator a_itEditProfile, const data::SPlayerData& a_cEditProfile) :
      IMenuDataHandler(),
      m_itEditProfile(a_itEditProfile),
      m_cEditProfile (a_cEditProfile),
      m_pGui         (CGlobal::getInstance()->getGuiEnvironment()),
      m_eStep        (enEditProfileStep::Unknown),
      m_pAiHelp      (nullptr)
    {
      std::vector<std::string> l_vNames = helpers::readLinesOfFile("data/names.txt");

      for (auto l_sName : l_vNames) {
        std::vector<std::string> l_vSplit = helpers::splitString(l_sName, ',');

        std::string l_sFirstName = l_vSplit.size() > 0 ? l_vSplit[0] : "";
        std::string l_sSurName   = l_vSplit.size() > 0 ? l_vSplit[1] : "";

        m_vDefaultNames.push_back(std::make_tuple(l_sFirstName, l_sSurName));
      }

      if (m_cEditProfile.m_sName == "")
        generateDefaultName();

      setEditProfileStep(enEditProfileStep::Name);
      updateAiHelp((int)m_cEditProfile.m_eAiHelp);
    }

    CDataHandler_EditProfile::~CDataHandler_EditProfile() {
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_EditProfile::handleIrrlichtEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

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
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
          if (l_sCaller == "EditProfile_AiHelp") {
            updateAiHelp(reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected());
          }
        }
      }

      return l_bRet;
    }

    /**
    * Get the edited profile
    * @return the edited profile
    */
    const data::SPlayerData& CDataHandler_EditProfile::getEditedProfile() {
      return m_cEditProfile;
    }

    /**
    * Get the iterator of the profile to see if we are editing or adding a new one
    */
    std::vector<data::SPlayerData>::iterator &CDataHandler_EditProfile::getProfileIterator() {
      return m_itEditProfile;
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

      for (int i = 0; i < 2; i++) {
        

        switch (m_eStep) {
          case enEditProfileStep::Name  : l_pRoot = helpers::findElementByName("EditProfile_Name"  , m_pGui->getRootGUIElement()); break;
          case enEditProfileStep::AiHelp: l_pRoot = helpers::findElementByName("EditProfile_AiHelp", m_pGui->getRootGUIElement()); break;
        }

        if (l_pRoot != nullptr)
          l_pRoot->setVisible(i == 0 ? false : true);

        m_eStep = a_eStep;
      }

      switch (m_eStep) {
        case dustbin::menu::enEditProfileStep::Unknown:
          break;
        case dustbin::menu::enEditProfileStep::Name:
          break;
        case dustbin::menu::enEditProfileStep::AiHelp: {
          gui::CSelector *m_pAiHelp = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("EditProfile_AiHelp", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (m_pAiHelp != nullptr)
            m_pAiHelp->setSelected((int)m_cEditProfile.m_eAiHelp);
          break;
        }
        case dustbin::menu::enEditProfileStep::Ctrls:
          break;
        case dustbin::menu::enEditProfileStep::Texture:
          break;
        case dustbin::menu::enEditProfileStep::Overview:
          break;
        default:
          break;
      }

      return l_pRoot != nullptr;
    }

    /**
    * Update the hint for the AI help dialog and the AI help in m_cEditProfile
    * @param a_iAiHelp the new AI help level
    */
    void CDataHandler_EditProfile::updateAiHelp(int a_iAiHelp) {
      if (a_iAiHelp >= (int)data::SPlayerData::enAiHelp::Off && a_iAiHelp <= (int)data::SPlayerData::enAiHelp::BotMb3) {
        m_cEditProfile.m_eAiHelp = (data::SPlayerData::enAiHelp)a_iAiHelp;

        if (m_pAiHelp != nullptr)
          m_pAiHelp->setVisible(false);

        irr::gui::IGUIElement *l_pHint = helpers::findElementByName("EditProfile_AiHelpInfo" + std::to_string(a_iAiHelp), m_pGui->getRootGUIElement());
        if (l_pHint != nullptr)
          l_pHint->setVisible(true);

        m_pAiHelp = l_pHint;
      }
    }
  }
}
