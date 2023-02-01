// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <gui/CControllerUi_Game.h>
#include <helpers/CMenuLoader.h>
#include <gui/CReactiveLabel.h>
#include <menu/IMenuHandler.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuProfileWizard
    * @author Christian Keimel
    * This class handles the menu that acts as a profile
    * for creating / editing a player profile
    */
    class CMenuProfileWizard : public IMenuHandler {
      private:
        enum class enMenuStep {
          Initialize,   /**< Initial step: only used when entering the wizard */
          Name,         /**< First step: define the player's name */
          Abbreviation, /**< Second step: pick an abbreviation for the ranking display during race */
          AiHelp,       /**< Third step: define the AI help for the player */
          Controls,     /**< Fourth step: define the profile's controls */
          Texture       /**< Fifth step: create a custom texture (if wanted) */
        };

        enMenuStep   m_eStep;         /**< The current step of the wizard */
        CGlobal     *m_pGlobal;       /**< Instance of the global object */
        std::string  m_sColorEdit;    /**< The currently edited color (Step 5: Texture) */
        std::string  m_sProfile;      /**< Number of the currently edited profile */

        data::SPlayerData m_cPlayer;  /**< The player we edit */

        gui::CControllerUi_Game   *m_pCtrl;     /**< The controller UI (Step 4/5) */
        irr::scene::ISceneManager *m_pMySmgr;   /**< Own scene manager for rendering the texture of the marble */
        irr::video::ITexture      *m_pMyRtt;    /**< Render target texture for the marble with it's custom texture */
        irr::scene::ISceneNode    *m_pMarble;   /**< The marble for the texture preview */

        irr::gui::IGUIElement *m_pBtnBack;    /**< The back button */
        irr::gui::IGUIElement *m_pBtnCancel;  /**< The cancel button */
        irr::gui::IGUIElement *m_pBtnNext;    /**< The next button */
        irr::gui::IGUIElement *m_pBtnSave;    /**< The save button */
        irr::gui::IGUIImage   *m_pWarning;    /**< The warning image for the name steps */

        /**
        * Update the texture of the preview marble in the texture wizard step
        * @param a_sTexture the texture string of the marble
        */
        void updateMarbleTexture(const std::string &a_sTexture) {
          if (m_pMyRtt != nullptr && m_pMarble != nullptr) {
            m_pMarble->getMaterial(0).setTexture(0, helpers::createTexture(a_sTexture != "" ? a_sTexture : "default://number=1", m_pDrv, m_pFs));
          }
        }

        /**
        * Get the texture string from the texture dialog
        * @return the texture string from the texture dialog
        */
        std::wstring getTextureString() {
          gui::CSelector *l_pMode = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("texture_mode", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          std::wstring l_sTexture = L"default://";

          if (l_pMode != nullptr) {
            if (l_pMode->getSelected() == 1) {
              l_sTexture = L"";

              // All parameters for the generated textures are stored
              // in Edit fields. This map uses the parameter name (key)
              // and maps them to the edit field name (value)
              std::map<std::wstring, std::string> l_mParameters{
                { L"numbercolor" , "texture_fg_nb"       },
                { L"numberback"  , "texture_bg_nb"       },
                { L"ringcolor"   , "texture_nr"          },
                { L"numberborder", "texture_nf"          },
                { L"patterncolor", "texture_fg_pt"       },
                { L"patternback" , "texture_bg_pt"       },
                { L"pattern"     , "texture_pattern"     }
              };

              for (std::map<std::wstring, std::string>::iterator it = l_mParameters.begin(); it != l_mParameters.end(); it++) {
                irr::gui::IGUIEditBox *p = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(it->second, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                if (p != nullptr) {
                  if (l_sTexture == L"")
                    l_sTexture = L"generate://";
                  else
                    l_sTexture = l_sTexture + L"&";

                  l_sTexture += it->first + L"=" + p->getText();
                }
              }
            }
          }

          return l_sTexture;
        }

        /**
        * Initialize the scrollbars of the color dialog
        * @param a_sColor the color to initialize with
        */
        void initializeColorDialog(const std::wstring& a_sColor) {
          if (a_sColor.size() >= 6) {
            std::map<int, std::string> l_mMap = {
              { 0, "scrollbar_red"   },
              { 2, "scrollbar_green" },
              { 4, "scrollbar_blue"  }
            };

            for (std::map<int, std::string>::iterator l_itColor = l_mMap.begin(); l_itColor != l_mMap.end(); l_itColor++) {
              irr::gui::IGUIScrollBar  *l_pColor = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(l_itColor->second, irr::gui::EGUIET_SCROLL_BAR , m_pGui->getRootGUIElement()));
              if (l_pColor != nullptr) {
                char *p = nullptr;
                std::string l_sSub = helpers::ws2s(a_sColor.substr(l_itColor->first, 2));
                l_pColor->setPos(std::strtol(l_sSub.c_str(), &p, 16));
              }
            }

            irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_display", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

            if (l_pTab != nullptr)
              l_pTab->setBackgroundColor(getColorFromColorDialog());
          }
        }

        /**
        * Translate a color to a HTML style string (RRGGBB)
        * @param a_cColor the color to translate
        * @return the translated color as string
        */
        std::string colorToString(const irr::video::SColor &a_cColor) {
          std::string l_sColor = "";
          char s[0xFF]; 

          sprintf(s,"%x", a_cColor.getRed  ()); std::string l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor  = l_sDummy;
          sprintf(s,"%x", a_cColor.getGreen());             l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor += l_sDummy;
          sprintf(s,"%x", a_cColor.getBlue ());             l_sDummy = s; while (l_sDummy.size() < 2) l_sDummy = "0" + l_sDummy; l_sColor += l_sDummy;

          return l_sColor;
        }

        /**
        * Get the selected color from the color dialog
        * @return the selected color from the color dialog
        */
        irr::video::SColor getColorFromColorDialog() {
          std::map<std::string, std::string> l_mColorParts = {
            { "scrollbar_red"  , "value_red"   },
            { "scrollbar_green", "value_green" },
            { "scrollbar_blue" , "value_blue"  }
          };

          irr::video::SColor l_cColor = irr::video::SColor(0xFF, 0, 0, 0);

          for (std::map<std::string, std::string>::iterator l_itColor = l_mColorParts.begin(); l_itColor != l_mColorParts.end(); l_itColor++) {
            irr::gui::IGUIScrollBar  *l_pColor = reinterpret_cast<irr::gui::IGUIScrollBar  *>(findElementByNameAndType(l_itColor->first , irr::gui::EGUIET_SCROLL_BAR , m_pGui->getRootGUIElement()));
            irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType(l_itColor->second, irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));


            if (l_pColor != nullptr && l_pLabel != nullptr) {
              wchar_t s[0xFF];
              swprintf(s, 0xFF, L"%i", l_pColor->getPos());
              l_pLabel->setText(s);

              if (l_itColor->first == "scrollbar_red"  ) l_cColor.setRed  ((irr::u32)l_pColor->getPos());
              if (l_itColor->first == "scrollbar_green") l_cColor.setGreen((irr::u32)l_pColor->getPos());
              if (l_itColor->first == "scrollbar_blue" ) l_cColor.setBlue ((irr::u32)l_pColor->getPos());
            }
          }

          return l_cColor;
        }

        /**
        * Update the generated texture of the preview marble
        */
        void updateGeneratedTexture() {
          gui::CSelector *l_pMode = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("texture_mode", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (l_pMode != nullptr) {
            irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("texture_generated", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

            if (l_pMode->getSelected() == 0) {
              if (l_pTab != nullptr) {
                l_pTab->setVisible(false);
              }
              updateMarbleTexture(m_cPlayer.m_sTexture);
            }
            else {
              if (l_pTab != nullptr) {
                l_pTab->setVisible(true);
              }
            }
          }
        }

        /**
        * Update the info text for the AI help
        */
        void updateAiHelpInfo() {
          irr::gui::IGUIStaticText *l_pInfo  = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("aihelp_info", irr::gui::EGUIET_STATIC_TEXT                  , m_pGui->getRootGUIElement()));
          gui::CSelector           *l_pLevel = reinterpret_cast<gui::CSelector           *>(findElementByNameAndType("ai_help"    , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

          if (l_pLevel != nullptr && l_pInfo != nullptr) {
            switch (l_pLevel->getSelected()) {
              case 0:
                l_pInfo->setText(L"The AI does not help you, you are on your own.");
                break;

              case 1:
                l_pInfo->setText(L"You get some hints by the AI showing you the way it would control the marble at any time in the race.");
                break;

              case 2:
                l_pInfo->setText(L"You get some hints by the AI showing you the way it would control the marble at any time in the race, and it will take over speed control when you approach a jump.");
                break;

              case 3:
                l_pInfo->setText(L"The AI takes over speed control so you can learn a good (probably not the best possible) way to race. You can however override the speed control by using the brake.");
                break;

              case 4:
                l_pInfo->setText(L"Speed is controlled by the AI, and when you approach a jump the AI takes over complete control. But you have the option to override the speed control by using the brake.");
                break;

              case 5:
                l_pInfo->setText(L"The MarbleGP AI controls your marble. MarbleGP is the best option AI can offer, use this mode to watch the AI and learn a good way to race.");
                break;

              case 6:
                l_pInfo->setText(L"Marble2 AI takes control of the marble. Marble2 is a good option to race, you can use this mode to watch racing and possibly learn a reasonable way to race.");
                break;

              case 7:
                l_pInfo->setText(L"Have your marble controlled by Marble3 AI. Note however that this AI level is not really good, so to learn how to race a better AI level would be more sufficient.");
                break;
              }
          }
        }

        /**
        * We are changing the current wizard step, so we need to save the data
        */
        void saveDataOfWizardStep() {
          // Save the data of the old step
          switch (m_eStep) {
            case enMenuStep::Initialize:
              // Nothing to save here, this is the initial step
              break;

            case enMenuStep::Name: {
              // Copy the name field to the m_sName member
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr)
                m_cPlayer.m_sName = helpers::ws2s(l_pEdit->getText());
              break;
            }

            case enMenuStep::Abbreviation: {
              // Copy the short name to the m_sShort member
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr)
                m_cPlayer.m_sShortName = helpers::ws2s(l_pEdit->getText());
              break;
            }

            case enMenuStep::AiHelp: {
              // Save the selected AI help to the member m_eAiHelp
              gui::CSelector *l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("ai_help", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
              if (l_pSelector != nullptr) {
                switch (l_pSelector->getSelected()) {
                  case 0: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Off    ; break;
                  case 1: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Display; break;
                  case 2: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Low    ; break;
                  case 3: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::Medium ; break;
                  case 4: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::High   ; break;
                  case 5: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMgp ; break;
                  case 6: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMb2 ; break;
                  case 7: m_cPlayer.m_eAiHelp = data::SPlayerData::enAiHelp::BotMb3 ; break;
                }
              }
              break;
            }

            case enMenuStep::Controls: {
              // Serialize the controls and save it to m_sControls
              if (m_pCtrl != nullptr) {
                if (m_pCtrl != nullptr) {
                  m_cPlayer.m_sControls = m_pCtrl->serialize();
                }

                m_pCtrl = nullptr;
              }
              break;
            }

            case enMenuStep::Texture: {
              // Serialize the texture and save it to m_sTexture
              break;
            }
          }
        }

        /**
        * Fill the edit fields with the member data
        */
        void restoreDataOfWizardStep() {
          // Fill the wizard edit fields with the available data
          switch (m_eStep) {
            case enMenuStep::Initialize:
              // Nothing to do here, we never reach this state again
              break;

            case enMenuStep::Name: {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr)
                l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sName).c_str());
              break;
            }

            case enMenuStep::Abbreviation: {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
              if (l_pEdit != nullptr)
                l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sShortName).c_str());
              break;
            }

            case enMenuStep::AiHelp: {
              gui::CSelector *l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("ai_help", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

              if (l_pSelector != nullptr) {
                switch (m_cPlayer.m_eAiHelp) {
                  case data::SPlayerData::enAiHelp::Off    : l_pSelector->setSelected(0); break;
                  case data::SPlayerData::enAiHelp::Display: l_pSelector->setSelected(1); break;
                  case data::SPlayerData::enAiHelp::Low    : l_pSelector->setSelected(2); break;
                  case data::SPlayerData::enAiHelp::Medium : l_pSelector->setSelected(3); break;
                  case data::SPlayerData::enAiHelp::High   : l_pSelector->setSelected(4); break;
                  case data::SPlayerData::enAiHelp::BotMgp : l_pSelector->setSelected(5); break;
                  case data::SPlayerData::enAiHelp::BotMb2 : l_pSelector->setSelected(6); break;
                  case data::SPlayerData::enAiHelp::BotMb3 : l_pSelector->setSelected(7); break;
                }
              }

              updateAiHelpInfo();
              break;
            }

            case enMenuStep::Controls: {
              m_pCtrl = reinterpret_cast<gui::CControllerUi_Game *>(findElementByNameAndType("controller_ui", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ControllerUiGameId, m_pGui->getRootGUIElement()));

              if (m_pCtrl != nullptr) {
                m_pCtrl->setText(helpers::s2ws(m_cPlayer.m_sControls).c_str());
                m_pCtrl->setMenuManager(m_pManager);
              }
              break;
            }

            case enMenuStep::Texture: {
              size_t l_iPos = m_cPlayer.m_sTexture.find("://");

              if (l_iPos != std::string::npos) {
                std::string l_sPrefix  = m_cPlayer.m_sTexture.substr(0, l_iPos);

                printf("Prefix: %s\n", l_sPrefix.c_str());
                gui::CSelector    *l_pMode = reinterpret_cast<gui::CSelector    *>(findElementByNameAndType("texture_mode"     , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
                irr::gui::IGUITab *l_pTab  = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("texture_generated", irr::gui::EGUIET_TAB                          , m_pGui->getRootGUIElement()));
                if (l_pMode != nullptr) {
                  if (l_pTab != nullptr)
                    l_pTab->setVisible(l_sPrefix == "generate");

                  if (l_sPrefix == "generate") {
                    l_pMode->setSelected(1);

                    std::string l_sTexture = m_cPlayer.m_sTexture.substr(l_iPos + 3);

                    std::map<std::string, std::string> l_mParamMap{
                      { "numbercolor" , "texture_fg_nb"       },
                      { "numberback"  , "texture_bg_nb"       },
                      { "ringcolor"   , "texture_nr"          },
                      { "numberborder", "texture_nf"          },
                      { "patterncolor", "texture_fg_pt"       },
                      { "patternback" , "texture_bg_pt"       },
                      { "pattern"     , "texture_pattern"     },
                      { "nameback"    , "imported_name_color" },
                      { "namecolor"   , "imported_name_back"  }
                    };

                    std::vector<std::string> l_vParts = helpers::splitString(l_sTexture, '&');

                    for (std::vector<std::string>::iterator l_itPart = l_vParts.begin(); l_itPart != l_vParts.end(); l_itPart++) {
                      std::vector<std::string> l_vParam = helpers::splitString(*l_itPart, '=');

                      printf("%s ==> %s\n", l_vParam[0].c_str(), l_vParam[1].c_str());

                      if (l_vParam.size() == 2 && l_mParamMap.find(l_vParam[0]) != l_mParamMap.end()) {
                        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(l_mParamMap[l_vParam[0]], irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
                        if (l_pEdit != nullptr)
                          l_pEdit->setText(helpers::s2ws(l_vParam[1]).c_str());
                      }
                    }
                  }
                  else {
                    l_pMode->setSelected(0);
                  }
                }
              }
              break;
            }
          }
        }

        /**
        * Toggle the button visibility depending on state and input fields. Also
        * update the background color of the edit fields for (short) name
        */
        void toggleButtonVisibility() {
          if (m_pBtnBack != nullptr)
            m_pBtnBack->setVisible(m_eStep != enMenuStep::Initialize && m_eStep != enMenuStep::Name);

          if (m_pBtnNext != nullptr) {
            bool l_bVisible = m_eStep != enMenuStep::Texture;

            if (m_eStep == enMenuStep::Name) {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

              if (l_pEdit != nullptr) {
                std::wstring s = l_pEdit->getText();
                l_bVisible = s.size() >= 3;

                if (m_pWarning != nullptr)
                  m_pWarning->setVisible(!l_bVisible);
              }
            }
            else if (m_eStep == enMenuStep::Abbreviation) {
              irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("shortname", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

              if (l_pEdit != nullptr) {
                std::wstring s = l_pEdit->getText();
                l_bVisible = s.size() >= 2;

                if (m_pWarning != nullptr)
                  m_pWarning->setVisible(!l_bVisible);
              }
            }

            m_pBtnNext->setVisible(l_bVisible);
          }

          if (m_pBtnCancel != nullptr)
            m_pBtnCancel->setVisible(m_sProfile != "commit_profile");

          if (m_pBtnSave != nullptr)
            m_pBtnSave->setVisible(m_eStep == enMenuStep::Texture);
        }

        /**
        * Change the current wizard step
        * @para a_eStep the new step
        */
        void changeStep(enMenuStep a_eStep) {
          saveDataOfWizardStep();

          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML(
            a_eStep == enMenuStep::Name         ? "data/menu/profilewizard_name.xml"      :
                   a_eStep == enMenuStep::Abbreviation ? "data/menu/profilewizard_shortname.xml" :
                   a_eStep == enMenuStep::AiHelp       ? "data/menu/profilewizard_aihelp.xml"    :
                   a_eStep == enMenuStep::Controls     ? "data/menu/profilewizard_controls.xml"  :
                   a_eStep == enMenuStep::Texture      ? "data/menu/profilewizard_texture.xml"   : "",
            m_pGui->getRootGUIElement(), m_pGui
          );

          if (a_eStep == enMenuStep::Name) {
            irr::gui::IGUIElement *l_pBack = findElementByName("back", m_pGui->getRootGUIElement());

            if (l_pBack != nullptr)
              l_pBack->setVisible(false);

            irr::gui::IGUIElement *l_pEdit = findElementByName("name", m_pGui->getRootGUIElement());
            if (l_pEdit != nullptr)
              m_pGui->setFocus(l_pEdit);
          }
          else if (a_eStep == enMenuStep::Abbreviation) {
            irr::gui::IGUIElement *l_pEdit = findElementByName("shortname", m_pGui->getRootGUIElement());
            if (l_pEdit != nullptr)
              m_pGui->setFocus(l_pEdit);
          }
          else if (a_eStep == enMenuStep::Texture) {
            // In the texture dialog we hide the "next" button ...
            irr::gui::IGUIElement *l_pNext = findElementByName("next", m_pGui->getRootGUIElement());

            if (l_pNext != nullptr)
              l_pNext->setVisible(false);

            // ... and we shot the "save" button
            irr::gui::IGUIElement *l_pSave = findElementByName("save", m_pGui->getRootGUIElement());

            if (l_pSave != nullptr)
              l_pSave->setVisible(true);

            if (m_pMySmgr == nullptr) {
              m_pMySmgr = m_pSmgr->createNewSceneManager(false);
              m_pMySmgr->loadScene("data/scenes/texture_scene.xml");
              irr::scene::ICameraSceneNode *l_pCam = m_pMySmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
              l_pCam->setAspectRatio(1.0f);
            }

            if (m_pMyRtt == nullptr) {
              m_pMyRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");
            }

            irr::gui::IGUIImage *l_pTexture = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("texture_image", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));

            if (l_pTexture != nullptr && m_pMyRtt != nullptr)
              l_pTexture->setImage(m_pMyRtt);

            if (m_pMySmgr != nullptr) {
              m_pMarble = m_pMySmgr->getSceneNodeFromName("marble");
              updateMarbleTexture(m_cPlayer.m_sTexture);
            }
          }

          m_eStep = a_eStep;

          restoreDataOfWizardStep();

          m_pBtnBack   = findElementByName("back"  , m_pGui->getRootGUIElement());
          m_pBtnCancel = findElementByName("cancel", m_pGui->getRootGUIElement());
          m_pBtnNext   = findElementByName("next"  , m_pGui->getRootGUIElement());
          m_pBtnSave   = findElementByName("save"  , m_pGui->getRootGUIElement());

          m_pWarning   = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("warning", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));

          if (m_pWarning != nullptr) {
            m_pWarning->setImage(m_pDrv->getTexture("data/images/edit_warning.png"));
          }

          toggleButtonVisibility();
        }

      public:
        CMenuProfileWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_eStep     (enMenuStep::Initialize),
          m_pGlobal   (CGlobal::getInstance()),
          m_sColorEdit(""),
          m_sProfile  (""),
          m_pCtrl     (nullptr),
          m_pMySmgr   (nullptr),
          m_pMyRtt    (nullptr),
          m_pMarble   (nullptr),
          m_pBtnBack  (nullptr),
          m_pBtnCancel(nullptr),
          m_pBtnNext  (nullptr),
          m_pBtnSave  (nullptr)
        {
          m_sProfile = m_pGlobal->getGlobal("edit_profile");
          m_pGlobal->setGlobal("edit_profile", "");

          changeStep(enMenuStep::Name);

          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          if (m_sProfile == "commit_profile") {
            // ToDo: add profile when wizard is confirmed, hide "cancel" buttons
          }

          m_cPlayer = data::SPlayerData();
          m_cPlayer.m_iPlayerId = 1;

          if (m_sProfile != "") {  
            m_cPlayer.deserialize(m_sProfile);

            irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType("name", irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));
            if (l_pEdit != nullptr) {
              l_pEdit->setText(helpers::s2ws(m_cPlayer.m_sName).c_str());
            }

            toggleButtonVisibility();
          }
        }

        virtual ~CMenuProfileWizard() {
          if (m_pMySmgr != nullptr)
            m_pMySmgr->drop();
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (m_pCtrl != nullptr) {
            m_pCtrl->update(a_cEvent);
          }

          if (!l_bRet) {
            if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
              if (a_cEvent.KeyInput.Key == irr::KEY_RETURN && !a_cEvent.KeyInput.PressedDown) {
                if ((m_pBtnNext != nullptr && m_pBtnNext->isVisible()) || (m_pBtnSave != nullptr && m_pBtnSave->isVisible())) {
                  irr::SEvent l_cEvent;

                  l_cEvent.EventType = irr::EET_GUI_EVENT;
                  l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
                  l_cEvent.GUIEvent.Caller    = (m_pBtnNext != nullptr && m_pBtnNext->isVisible()) ? m_pBtnNext : m_pBtnSave;
                  l_cEvent.GUIEvent.Element   = m_pBtnNext;
 
                  OnEvent(l_cEvent);

                  l_bRet = true;
                }
              }
            }
            else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
              if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

                if (l_sButton == "cancel") {
                  m_pManager->changeMenu(createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState));
                }
                else if (l_sButton == "save") {
                  if (m_sProfile == "commit_profile") {
                    // Save the profile to the settings (it's the first profile in the list)
                    m_cPlayer.m_iPlayerId = 1;

                    messages::CSerializer64 l_cSerializer;

                    l_cSerializer.addS32(c_iProfileHead);
                    l_cSerializer.addString(c_sProfileHead);

                    l_cSerializer.addS32(c_iProfileStart);
                    l_cSerializer.addString(m_cPlayer.serialize());
                    l_cSerializer.addS32(c_iProfileEnd);

                    l_cSerializer.addS32(c_iAllProfileEnd);

                    m_pState->getGlobal()->setSetting("profiles", l_cSerializer.getMessageAsString());
                  }
                  else {
                    CGlobal::getInstance()->setGlobal("edited_profile", m_cPlayer.serialize());                    
                  }
                    
                  createMenu(m_pManager->popMenuStack(), m_pDevice,m_pManager, m_pState);
                }
                else if (l_sButton == "next") {
                  switch (m_eStep) {
                    case enMenuStep::Name        : changeStep(enMenuStep::Abbreviation); break;
                    case enMenuStep::Abbreviation: changeStep(enMenuStep::AiHelp      ); break;
                    case enMenuStep::AiHelp      : changeStep(enMenuStep::Controls    ); break;
                    case enMenuStep::Controls    : changeStep(enMenuStep::Texture     ); break;
                    case enMenuStep::Texture     :
                    case enMenuStep::Initialize  :
                      // This button is not visible here, so let's ignore it
                      break;
                  }
                }
                else if (l_sButton == "back") {
                  switch (m_eStep) {
                    case enMenuStep::Name      :
                    case enMenuStep::Initialize:
                      // This button isn't visible here, so we ignore it
                      break;

                    case enMenuStep::Abbreviation: changeStep(enMenuStep::Name        ); break;
                    case enMenuStep::AiHelp      : changeStep(enMenuStep::Abbreviation); break;
                    case enMenuStep::Controls    : changeStep(enMenuStep::AiHelp      ); break;
                    case enMenuStep::Texture     : changeStep(enMenuStep::Controls    );  break;
                  }
                }
                else {
                  if (m_eStep == enMenuStep::Texture) {
                    if (l_sButton == "btn_color_ok") {
                      irr::gui::IGUITab *l_pColor = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                      if (l_pColor != nullptr) {
                        irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(m_sColorEdit, irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                        if (l_pEdit != nullptr) {
                          l_pEdit->setText(helpers::s2ws(colorToString(getColorFromColorDialog())).c_str());
                        }

                        updateMarbleTexture(helpers::ws2s(getTextureString()));
                      
                        l_pColor->setVisible(false);
                        changeZLayer(0);
                      }
                    }
                    else if (l_sButton == "btn_color_cancel") {
                      irr::gui::IGUITab *l_pColor = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                      if (l_pColor != nullptr) {
                        l_pColor->setVisible(false);
                        changeZLayer(0);
                      }
                    }
                    else if (l_sButton == "pick_color") {
                      gui::CReactiveLabel *p = reinterpret_cast<gui::CReactiveLabel *>(a_cEvent.GUIEvent.Caller);
                      irr::video::SColor   c = p->getBackgroundColor();

                      initializeColorDialog(helpers::s2ws(colorToString(c)));
                    }
                    else {
                      std::map<std::string, std::string> l_mButtonLinks = {
                        { "btn_select_fg_nb"     , "texture_fg_nb"       },
                        { "btn_select_bg_nb"     , "texture_bg_nb"       },
                        { "btn_select_nr"        , "texture_nr"          },
                        { "btn_select_nf"        , "texture_nf"          },
                        { "btn_select_fg_pt"     , "texture_fg_pt"       },
                        { "btn_select_bg_pt"     , "texture_bg_pt"       },
                        { "btn_select_name_color", "imported_name_color" },
                        { "btn_select_name_back" , "imported_name_back"  }
                      };

                      printf("Button: %s\n", l_sButton.c_str());

                      if (l_mButtonLinks.find(l_sButton) != l_mButtonLinks.end()) {
                        irr::gui::IGUITab     *l_pColor = reinterpret_cast<irr::gui::IGUITab     *>(findElementByNameAndType("color_dialog"           , irr::gui::EGUIET_TAB     , m_pGui->getRootGUIElement()));
                        irr::gui::IGUIEditBox *l_pEdit  = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByNameAndType(l_mButtonLinks[l_sButton], irr::gui::EGUIET_EDIT_BOX, m_pGui->getRootGUIElement()));

                        if (l_pColor != nullptr && l_pEdit != nullptr) {
                          m_sColorEdit = l_mButtonLinks[l_sButton];
                          l_pColor->setVisible(true);
                          initializeColorDialog(l_pEdit->getText());
                          changeZLayer(23);
                        }
                        // irr::gui::IGUIEditBox *l_pEdit = reinterpret_cast<irr::gui::IGUIEditBox *>(findElementByName(l_mButtonLinks[l_sButton], m_pGui->getRootGUIElement()));

                        // if (l_pEdit != nullptr) {

                        // }
                      }
                    }
                  }
                }
              }
              else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
                std::string l_sScrollbar = a_cEvent.GUIEvent.Caller->getName();
                if (l_sScrollbar == "ai_help")
                  updateAiHelpInfo();
                else if (l_sScrollbar == "texture_mode") {
                  updateGeneratedTexture();
                }
                else if (m_eStep == enMenuStep::Texture) {
                  if (m_sColorEdit != "") {
                    irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("color_display", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

                    if (l_pTab != nullptr)
                      l_pTab->setBackgroundColor(getColorFromColorDialog());
                  }
                }
              }
              else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_EDITBOX_CHANGED) {
                toggleButtonVisibility();
              }
            }
          }

          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override {
          if (m_eStep == enMenuStep::Texture && m_pMyRtt != nullptr && m_pMySmgr != nullptr) {
            m_pDrv->setRenderTarget(m_pMyRtt, true, true);
            m_pMySmgr->drawAll();
            m_pDrv->setRenderTarget(nullptr, false, false);
          }

          return false;
        }
    };

    IMenuHandler* createMenuProfileWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuProfileWizard(a_pDevice, a_pManager, a_pState);
    }
  }
}