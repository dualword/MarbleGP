// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <controller/CControllerBase.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CTextureHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CDataHelpers.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <gui/CReactiveLabel.h>
#include <gui/CGuiImageList.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <gui/CClipImage.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <Defines.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace menu {

    /**
    * @class SPlayerProfileUI
    * @author Christian Keimel
    * This data structure holds references to all UI
    * elements for editing the player profiles
    */
    struct SPlayerProfileUI {
      gui::CMenuBackground *m_pRoot;  /**< The root element for the player profile dialogs */

      irr::gui::IGUITab *m_pDataRoot; /**< The tab that acts as root element for all edits */

      irr::gui::IGUIEditBox *m_pName;   /**< The player name edit box */
      irr::gui::IGUIEditBox *m_pShort;  /**< The player name abbreviation edit box */

      irr::gui::IGUIStaticText *m_pControl;     /**< The label for type of controls */
      irr::gui::IGUIStaticText *m_pTextureType; /**< The label for type of texture */
      irr::gui::IGUIStaticText *m_pLblAiHelp;   /**< The label for the AI help level */

      gui::CMenuButton *m_pDelete;      /**< The "remove profile" button */
      gui::CMenuButton *m_pEdit;        /**< The "edit controls" button */
      gui::CMenuButton *m_pTutorial;    /**< The "play tutorial" button */
      gui::CMenuButton *m_pAddProfile;  /**< The "add profile" button */

      data::SPlayerData m_cData;   /**< The Player Data structure linked to this profile UI */


      SPlayerProfileUI() :
        m_pRoot       (nullptr),
        m_pDataRoot   (nullptr),
        m_pName       (nullptr),
        m_pShort      (nullptr),
        m_pControl    (nullptr),
        m_pTextureType(nullptr),
        m_pLblAiHelp  (nullptr),
        m_pDelete     (nullptr),
        m_pEdit       (nullptr),
        m_pTutorial   (nullptr),
        m_pAddProfile (nullptr)
      {
      }

      bool isValid() {
        return m_pAddProfile  != nullptr &&
               m_pEdit        != nullptr &&
               m_pDataRoot    != nullptr &&
               m_pDelete      != nullptr &&
               m_pName        != nullptr &&
               m_pRoot        != nullptr &&
               m_pShort       != nullptr;
      }

      void fillUI() {
        if (isValid()) {
          m_pName ->setText(helpers::s2ws(m_cData.m_sName     ).c_str());
          m_pShort->setText(helpers::s2ws(m_cData.m_sShortName).c_str());

          m_pDataRoot->setVisible(true);
          m_pAddProfile->setVisible(false);

          std::string l_sCtrl = m_cData.m_sControls;

          if (l_sCtrl == "DustbinTouchControl") {
            if (m_pControl != nullptr)
              m_pControl->setText(L"Controls: Touch");
          }
          else if (l_sCtrl == "DustbinGyroscope") {
            if (m_pControl != nullptr)
              m_pControl->setText(L"Controls: Gyroscope");
          }
          else {
            controller::CControllerGame l_cCtrl;
            l_cCtrl.deserialize(l_sCtrl);
            switch ((*l_cCtrl.getInputs().begin()).m_eType) {
              case controller::CControllerBase::enInputType::JoyAxis:
              case controller::CControllerBase::enInputType::JoyButton:
              case controller::CControllerBase::enInputType::JoyPov:
                if (m_pControl != nullptr) m_pControl->setText(L"Controls: Gamepad");
                break;

              case controller::CControllerBase::enInputType::Key:
                if (m_pControl != nullptr) m_pControl->setText(L"Controls: Keyboard");
                break;
            }
          }

          size_t l_iPos = m_cData.m_sTexture.find("://");

          if (l_iPos != std::string::npos && m_pTextureType != nullptr) {
            std::string l_sPrefix  = m_cData.m_sTexture.substr(0, l_iPos );
            if (l_sPrefix == "file" || l_sPrefix == "imported")
              m_pTextureType->setText(L"Imported");
            else if (l_sPrefix == "generate") {
              m_pTextureType->setText(L"Generated");
            }
            else {
              m_pTextureType->setText(L"Default");
            }
          }

          updateAiHelp();
        }
      }

      void updateAiHelp() {
        if (m_pLblAiHelp != nullptr)
          switch (m_cData.m_eAiHelp) {
            case data::SPlayerData::enAiHelp::Off    : m_pLblAiHelp->setText(L"Off"              ); break;
            case data::SPlayerData::enAiHelp::Display: m_pLblAiHelp->setText(L"Display"          ); break;
            case data::SPlayerData::enAiHelp::Low    : m_pLblAiHelp->setText(L"Low"              ); break;
            case data::SPlayerData::enAiHelp::Medium : m_pLblAiHelp->setText(L"Medium"           ); break;
            case data::SPlayerData::enAiHelp::High   : m_pLblAiHelp->setText(L"High"             ); break;
            case data::SPlayerData::enAiHelp::BotMgp : m_pLblAiHelp->setText(L"AI Bot (MarbleGP)"); break;
            case data::SPlayerData::enAiHelp::BotMb2 : m_pLblAiHelp->setText(L"AI Bot (Marble2)" ); break;
            case data::SPlayerData::enAiHelp::BotMb3 : m_pLblAiHelp->setText(L"AI Bot (Marble3)" ); break;
        }
      }
    };


    /**
    * @class CMenuProfiles
    * @author Christian Keimel
    * The handler for the profiles menu
    */
    class CMenuProfiles : public IMenuHandler {
      private:
        SPlayerProfileUI  m_aProfiles[8];   /**< The UI array for modifying the profiles */

        irr::gui::IGUITab *m_pConfirmDialog;  /**< The error message dialog */
        irr::gui::IGUITab *m_pButtonTab;      /**< The tab with the "edit texture colors" buttons (Android) */

        std::map<std::string, irr::gui::IGUIElement *> m_mCtrlTypes;    /**< The type of controller */

        irr::scene::ISceneManager* m_pMySmgr;   /**< Alternative scene manager to draw the texture scene */
        irr::video::ITexture* m_pMyRtt;         /**< Render target for the texture scene */
        irr::scene::ISceneNode* m_pMarble;      /**< The marble scene node in the texture scene */

        int m_iMaxIndex;    /**< The highest found index of the profile edit tabs */

        std::map<std::string, std::tuple<std::string, irr::gui::IGUITab*>> m_mButtonLinks;    /**< This map links the button names (key) with the edit boxes (value) */
        std::map<std::string, irr::gui::IGUIEditBox* > m_mGeneratedEd;    /**< Link of buttons to their edit fields */
        std::map<irr::gui::IGUIScrollBar*, irr::gui::IGUIStaticText* > m_mColorLink;      /**< Link of the color scrollbars to their edits */

        std::vector<irr::video::ITexture*> m_vPatterns;   /**< List of the available texture patterns */
        std::vector<gui::CReactiveLabel*> m_vColorPick;  /**< List of the predefined colors to pick */

        std::map<irr::u8, irr::SEvent> m_mJoyStates;    /**< Joystick states to make sure events are only passed when the joystick state changes */

        int m_iTutorialPlr;     /**< The player who wants to start a tutorial. -1 means no tutorial is to be started */
        bool m_bJoySelect;      /**< Are we currently waiting for the player to select a gamepad? */

        /**
        * The general OK button has been clicked. The profiles are serialized and stored
        * as string in the "profiles" setting, the last step loads the main menu
        * @see messages::CSerializer64
        */
        void buttonOkClicked() {
          bool l_bClose = true;

          if (m_pManager->peekMenuStack() != "menu_main") {
            std::wstring l_sErrors = L"";

            l_bClose = false;

            for (int i = 0; i < m_iMaxIndex; i++)
              if (m_aProfiles[i].m_cData.m_sName != "") {
                l_bClose = true;
                break;
              }

            if (!l_bClose && m_pConfirmDialog != nullptr) {
              irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("confirmDialog_text", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
              if (p != nullptr)
                l_sErrors += L"You must add at least one profile before you can start a game.";

              m_pConfirmDialog->setVisible(true);
            }
          }

          if (l_bClose) {
            saveProfiles();
            platform::saveSettings();
            createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
          }
        }

        /**
        * The general cancel button was clicked, nothing is saved
        * and the main menu is loaded
        */
        void buttonCancelClicked() {
          createMenu("menu_main", m_pDevice, m_pManager, m_pState);
        }

        void saveProfiles() {
          std::vector<data::SPlayerData> l_vPlayers;

          for (int i = 0; i <= m_iMaxIndex; i++) {
            m_aProfiles[i].m_cData.m_iPlayerId = i + 1;
            if (m_aProfiles[i].m_cData.m_iPlayerId > 0 && m_aProfiles[i].m_cData.m_iPlayerId <= m_iMaxIndex + 1) {
              if (m_aProfiles[i].m_pDataRoot->isVisible()) {
                l_vPlayers.push_back(m_aProfiles[i].m_cData);
              }
            }
          }

          helpers::saveProfiles(l_vPlayers);
        }

        public:
          CMenuProfiles(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) : 
            IMenuHandler(a_pDevice, a_pManager, a_pState), 
            m_pConfirmDialog (nullptr),
            m_pButtonTab     (nullptr),
            m_pMySmgr        (nullptr),
            m_pMyRtt         (nullptr),
            m_pMarble        (nullptr),
            m_iMaxIndex      (-1),
            m_iTutorialPlr   (-1),
            m_bJoySelect     (false)
          {
            m_pState->getGlobal()->clearGui();

            helpers::loadMenuFromXML("data/menu/menu_profiles.xml", m_pGui->getRootGUIElement(), m_pGui);
            m_pSmgr->loadScene("data/scenes/skybox.xml");
            m_pSmgr->addCameraSceneNode();

            m_pMySmgr = m_pSmgr->createNewSceneManager(false);
            m_pMySmgr->loadScene("data/scenes/texture_scene.xml");
            irr::scene::ICameraSceneNode *l_pCam = m_pMySmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
            l_pCam->setAspectRatio(1.0f);

            m_pMarble = m_pMySmgr->getSceneNodeFromName("marble");

            m_pMyRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");

            irr::gui::IGUIElement *l_pRoot = m_pGui->getRootGUIElement();

            m_pConfirmDialog  = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("confirmDialog"    , irr::gui::EGUIET_TAB                                  , l_pRoot));
            m_pButtonTab      = reinterpret_cast<irr::gui::IGUITab            *>(findElementByNameAndType("ButtonTab"        , irr::gui::EGUIET_TAB                                  , l_pRoot));

            std::string l_sCtrls[] = {
              "controller_ui",
              "controller_touch_steerleft",
              "controller_touch_steerright",
              "controller_touch_steer",
              "controller_gyroscope",
              ""
            };

            for (int i = 0; l_sCtrls[i] != ""; i++) {
              irr::gui::IGUIElement *p = findElementByName(l_sCtrls[i], l_pRoot);
              if (p != nullptr) {
                m_mCtrlTypes[l_sCtrls[i]] = p;
              }
            }

            std::vector<std::string> l_vKeys = {
              "texture_fg_nb",
              "texture_bg_nb",
              "texture_nr",
              "texture_nf",
              "texture_fg_pt",
              "texture_bg_pt",
              "imported_name_color",
              "imported_name_back"
            };

            for (std::vector<std::string>::iterator it = l_vKeys.begin(); it != l_vKeys.end(); it++) {
              m_mGeneratedEd[*it] = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(*it, irr::gui::EGUIET_EDIT_BOX, l_pRoot));
            }

            l_vKeys = { "_red", "_green", "_blue" };

            for (std::vector<std::string>::iterator it = l_vKeys.begin(); it != l_vKeys.end(); it++) {
              irr::gui::IGUIScrollBar  *l_pKey   = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(std::string("scrollbar") + *it, irr::gui::EGUIET_SCROLL_BAR , l_pRoot));
              irr::gui::IGUIStaticText *l_pValue = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType(std::string("value"    ) + *it, irr::gui::EGUIET_STATIC_TEXT, l_pRoot));

              m_mColorLink[l_pKey] = l_pValue;
            }

            // Link the buttons (key) to the edit fields (value)
            m_mButtonLinks = {
              { "btn_select_fg_nb"     , std::make_tuple("texture_fg_nb"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_bg_nb"     , std::make_tuple("texture_bg_nb"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_nr"        , std::make_tuple("texture_nr"         , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_nf"        , std::make_tuple("texture_nf"         , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_fg_pt"     , std::make_tuple("texture_fg_pt"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_bg_pt"     , std::make_tuple("texture_bg_pt"      , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_name_color", std::make_tuple("imported_name_color", reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) },
              { "btn_select_name_back" , std::make_tuple("imported_name_back" , reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, l_pRoot))) }
            };

            for (int i = 0; i < 8; i++) {
              std::string s = "root_player" + std::to_string(i + 1);

              m_aProfiles[i].m_pRoot = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType(s, (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, l_pRoot));

              if (m_aProfiles[i].m_pRoot != nullptr) {
                irr::gui::IGUIElement *l_pRoot = m_aProfiles[i].m_pRoot;

                m_aProfiles[i].m_pDataRoot    = reinterpret_cast<irr::gui::IGUITab        *>(findElementByNameAndType("tab"              , irr::gui::EGUIET_TAB                            , l_pRoot));
                m_aProfiles[i].m_pName        = reinterpret_cast<irr::gui::IGUIEditBox    *>(findElementByNameAndType("edit_name"        , irr::gui::EGUIET_EDIT_BOX                       , l_pRoot));
                m_aProfiles[i].m_pShort       = reinterpret_cast<irr::gui::IGUIEditBox    *>(findElementByNameAndType("edit_abbreviation", irr::gui::EGUIET_EDIT_BOX                       , l_pRoot));
                m_aProfiles[i].m_pControl     = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_controls"   , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pTextureType = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_texture"    , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pLblAiHelp   = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_aihelp"     , irr::gui::EGUIET_STATIC_TEXT                    , l_pRoot));
                m_aProfiles[i].m_pDelete      = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_delete"       , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pEdit        = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_edit_profile" , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pTutorial    = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_play_tutorial", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                m_aProfiles[i].m_pAddProfile  = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("btn_add"          , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));

                if (m_aProfiles[i].isValid()) {
                  m_aProfiles[i].m_pDataRoot  ->setVisible(false);
                  m_aProfiles[i].m_pAddProfile->setVisible(i == 0);
                  m_iMaxIndex = i;
                  printf("==> %i\n", i);
                }
                else printf("**> %i\n", i);
              }
            }

            std::vector<data::SPlayerData> l_vPlayers = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));

            int l_iIndex = 0;

            for (auto& l_cPlayer : l_vPlayers) {
              l_cPlayer.m_iPlayerId = l_iIndex + 1;

              m_aProfiles[l_iIndex].m_cData = l_cPlayer;
              m_aProfiles[l_iIndex].fillUI();

              l_iIndex++;

              if (l_iIndex > m_iMaxIndex)
                break;
              else if (m_aProfiles[l_iIndex].isValid()) {
                m_aProfiles[l_iIndex].m_pAddProfile->setVisible(true);
              }
            }
          }

          virtual ~CMenuProfiles() {
            if (m_pMySmgr) m_pMySmgr->drop();
          }

          virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
            // A guard to make sure joystick events are only handled once per change
            if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
              if (m_bJoySelect && m_iTutorialPlr != -1) {
                if (a_cEvent.JoystickEvent.ButtonStates != 0) {
                  controller::CControllerGame l_cCtrl;
                  l_cCtrl.deserialize(m_aProfiles[m_iTutorialPlr].m_cData.m_sControls);
                  l_cCtrl.setJoystickIndices(a_cEvent.JoystickEvent.Joystick);
                  m_aProfiles[m_iTutorialPlr].m_cData.m_sControls = l_cCtrl.serialize();
                  m_bJoySelect = false;
                }
              }
              else {
                irr::u8 l_iIndex = a_cEvent.JoystickEvent.Joystick;
                bool    l_bSkip  = false;

                if (m_mJoyStates.find(l_iIndex) != m_mJoyStates.end()) {
                  l_bSkip = m_mJoyStates[l_iIndex].JoystickEvent.ButtonStates == a_cEvent.JoystickEvent.ButtonStates &&
                            m_mJoyStates[l_iIndex].JoystickEvent.POV          == a_cEvent.JoystickEvent.POV;

                  if (l_bSkip) {
                    for (int i = 0; i < a_cEvent.JoystickEvent.NUMBER_OF_AXES && l_bSkip; i++) {
                      int l_iOld = m_mJoyStates[l_iIndex].JoystickEvent.Axis[i];
                      int l_iNew = a_cEvent              .JoystickEvent.Axis[i];

                      if (abs(l_iOld) < 16000) l_iOld = 0;
                      if (abs(l_iNew) < 16000) l_iNew = 0;

                      l_bSkip &= l_iOld > 0 == l_iNew > 0 && l_iOld < 0 == l_iNew < 0;
                    }
                  }
                }

                m_mJoyStates[l_iIndex] = irr::SEvent(a_cEvent);

                if (l_bSkip)
                  return true;
              }
            }

            bool l_bRet = false;

            if (!l_bRet) {
              if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
                std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

                if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED) {
                  for (int i = 0; i <= m_iMaxIndex; i++) {
                    if (m_aProfiles[i].m_pName == a_cEvent.GUIEvent.Caller) {
                      bool l_bUpdateShort = m_aProfiles[i].m_cData.m_sName.substr(0, 5) == m_aProfiles[i].m_cData.m_sShortName || m_aProfiles[i].m_cData.m_sShortName == "";

                      m_aProfiles[i].m_cData.m_sName = helpers::ws2s(m_aProfiles[i].m_pName->getText()).c_str();

                      if (l_bUpdateShort) {
                        m_aProfiles[i].m_cData.m_sShortName = m_aProfiles[i].m_cData.m_sName.substr(0, 5);
                        m_aProfiles[i].m_pShort->setText(helpers::s2ws(m_aProfiles[i].m_cData.m_sShortName).c_str());
                      }

                      break;
                    }
                    else if (m_aProfiles[i].m_pShort == a_cEvent.GUIEvent.Caller) {
                      m_aProfiles[i].m_cData.m_sShortName = helpers::ws2s(m_aProfiles[i].m_pShort->getText()).c_str();
                    }
                  }
                }
                else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                  if (l_sSender == "ok") {
                    buttonOkClicked();
                    l_bRet = true;
                  }
                  else if (l_sSender == "cancel") {
                    buttonCancelClicked();
                    l_bRet = true;
                  }
                  else if (l_sSender == "btn_add") {
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].isValid() && m_aProfiles[i].m_pAddProfile == a_cEvent.GUIEvent.Caller) {
                        CGlobal::getInstance()->setGlobal("edit_profileno", "-1");
                        CGlobal::getInstance()->setGlobal("edit_profile", "");
                        m_pManager->pushToMenuStack("menu_profiles");
                        createMenu("menu_profilewizard", m_pDevice, m_pManager, m_pState);

                        break;
                      }
                    }
                  }
                  else if (l_sSender == "btn_delete") {
                    int l_iDeleted = -1;
                    bool l_bFirstEmpty = true;

                    // Search for the delete button that was clicked
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      // if found ..
                      if (m_aProfiles[i].m_pDelete == a_cEvent.GUIEvent.Caller && m_aProfiles[i].isValid()) {
                        l_iDeleted = i;
                        // .. we iterate all following profiles and copy
                        // the data of the next profile to the iterated
                        for (int j = i; j < m_iMaxIndex; j++) {
                          if (m_aProfiles[j].isValid() && m_aProfiles[j + 1].isValid()) {
                            m_aProfiles[j].m_cData.copyFrom(m_aProfiles[j + 1].m_cData);

                            // If the profile is used (m_iPlayerId set) the we fill the UI ...
                            if (m_aProfiles[j].m_cData.m_iPlayerId > 0)
                              m_aProfiles[j].fillUI();
                            else {
                              // .. Otherwise we hide the data ..
                              m_aProfiles[j].m_pDataRoot->setVisible(false);
                              if (m_aProfiles[j].m_cData.m_iPlayerId <= 0) {
                                // .. and if it's the first empty profile we show the add button
                                if (l_bFirstEmpty) {
                                  l_bFirstEmpty = false;
                                  m_aProfiles[j].m_pAddProfile->setVisible(true);
                                }
                                else m_aProfiles[j].m_pAddProfile->setVisible(false);
                              }
                            }
                          }
                        }

                        // One of m_iMaxIndex profiles has been deleted, we need 
                        // to set the last add button to visible if all profiles
                        // were used before the deletion
                        if (l_bFirstEmpty) {
                          if (m_aProfiles[m_iMaxIndex].isValid()) {
                            m_aProfiles[m_iMaxIndex].m_pAddProfile->setVisible(true);
                            m_aProfiles[m_iMaxIndex].m_pDataRoot->setVisible(false);
                            m_aProfiles[m_iMaxIndex].m_cData.m_iPlayerId = -1;
                            m_aProfiles[m_iMaxIndex].m_cData.m_sName     = "";
                            m_aProfiles[m_iMaxIndex].m_cData.m_sControls = "";
                            m_aProfiles[m_iMaxIndex].m_cData.m_sTexture  = "";
                          }
                        }
                        else m_aProfiles[m_iMaxIndex].m_pAddProfile->setVisible(false);
                      }
                    }

                    for (int i = 0; i <= m_iMaxIndex; i++)
                      if (m_aProfiles[i].m_cData.m_iPlayerId > 0)
                        m_aProfiles[i].m_cData.m_iPlayerId = i + 1;
                  }
                  else if (l_sSender == "btn_edit_profile") {
                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].isValid() && m_aProfiles[i].m_pEdit == a_cEvent.GUIEvent.Caller) {
                        saveProfiles();
                        CGlobal::getInstance()->setGlobal("edit_profileno", std::to_string(i));
                        CGlobal::getInstance()->setGlobal("edit_profile", m_aProfiles[i].m_cData.serialize());
                        m_pManager->pushToMenuStack("menu_profiles");
                        createMenu("menu_profilewizard", m_pDevice, m_pManager, m_pState);
                        break;
                      }
                    }
                  }
                  else if (l_sSender == "btn_play_tutorial") {
                    saveProfiles();

                    for (int i = 0; i <= m_iMaxIndex; i++) {
                      if (m_aProfiles[i].m_pTutorial == a_cEvent.GUIEvent.Caller) {
                        m_iTutorialPlr = i;

                        controller::CControllerGame l_cCtrl;
                        l_cCtrl.deserialize(m_aProfiles[i].m_cData.m_sControls);
                        m_bJoySelect = l_cCtrl.usesJoystick();

                        if (m_bJoySelect) {
                          irr::gui::IGUIElement *l_pSelect = findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());

                          if (l_pSelect != nullptr) {
                            irr::gui::IGUIStaticText *l_pPlr = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("selectctrl_player", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

                            if (l_pPlr != nullptr) {
                              std::wstring s = L"Player \"" + helpers::s2ws(m_aProfiles[m_iTutorialPlr].m_cData.m_sName) + L"\": Please select your gamepad by clicking a button.";
                              l_pPlr->setText(s.c_str());
                            }

                            l_pSelect->setVisible(true);
                          }
                        }

                        break;
                      }
                    }
                  }
                  else if (l_sSender == "CancelAssignJoystick") {
                    m_iTutorialPlr = -1;
                    m_bJoySelect   = false;

                    irr::gui::IGUIElement *l_pSelect = findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
                    if (l_pSelect != nullptr)
                      l_pSelect->setVisible(false);
                  }
                }
              }
            }

            return l_bRet;
          }

          /**
          * This method is called every frame after "scenemanager::drawall" is called
          */
          virtual bool run() override {
            m_pDrv->setRenderTarget(m_pMyRtt, true, true);
            m_pMySmgr->drawAll();
            m_pDrv->setRenderTarget(nullptr, false, false);

            if (m_iTutorialPlr != -1 && !m_bJoySelect) {
              data::SGameData l_cData;

              l_cData.m_eType       = data::SGameData::enType::Local;
              l_cData.m_iClass      = 0;
              l_cData.m_iLaps       = 1;
              l_cData.m_sTrack      = "tutorial";
              l_cData.m_bIsTutorial = true;

              CGlobal::getInstance()->setGlobal("gamedata", l_cData.serialize());

              data::SGameSettings l_cSettings;

              m_aProfiles[m_iTutorialPlr].m_cData.m_iViewPort = 1;
              m_aProfiles[m_iTutorialPlr].m_cData.m_iGridPos  = 1;
              m_aProfiles[m_iTutorialPlr].m_cData.m_iPlayerId = 1;

              data::SRacePlayers l_cPlayers;
              l_cPlayers.m_vPlayers.push_back(m_aProfiles[m_iTutorialPlr].m_cData);

              CGlobal::getInstance()->setGlobal("raceplayers", l_cPlayers.serialize());

              m_pState->setState(state::enState::Game);
              m_pManager->pushToMenuStack("menu_profiles");

              m_iTutorialPlr = -1;
            }

            return false;
          }
    };

    IMenuHandler* createMenuProfiles(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuProfiles(a_pDevice, a_pManager, a_pState);
    }
  }
}