// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <menu/datahandlers/CDataHandler_SelectPlayers.h>
#include <menu/datahandlers/CDataHandler_Controls.h>
#include <controller/CControllerGame.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <messages/CSerializer64.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <algorithm>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
#ifdef _WINDOWS
    const int c_iMaxProfiles = 8;
#else
    const int c_iMaxProfiles = 4;
#endif


    /**
    * @class CMenuNewGameWizard
    * @author Christian Keimel
    * The wizard shown when starting a game
    */
    class CMenuNewGameWizard : public IMenuHandler {
      private:
        enum class enWizardStep {
          Unknown,
          Profiles,
          EditProfile,
          Controllers,
          GameLevel
        };

        data::SRacePlayers  m_cRacePlayers;     /**< The available race players */
        data::SChampionship m_cChampionship;    /**< The championship that might be started */

        std::vector<irr::gui::IGUIElement *> m_vSteps;      /**< The root GUI element for each available step */
        std::vector<data::SPlayerData      > m_vProfiles;   /**< A vector with all player data */

        data::SGameSettings m_cSettings;    /**< The game settings data struct */

        enWizardStep m_eStep;   /**< The active wizard step */

        /**
        * Fill a player element with the provided data
        * @param a_pPlayer the player data
        * @param a_pRoot the root element of the player UI
        * @param a_bSelected is this player in the list of selected players?
        * @param a_bAddNew if a_pPlayer == nullptr and this parameter is true the "add player" button is shown
        */
        void fillPlayer(data::SPlayerData* a_pPlayer, irr::gui::IGUIElement* a_pRoot, bool a_bSelected, bool a_bAddNew) {
          if (a_pPlayer != nullptr) {
            irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerName", irr::gui::EGUIET_STATIC_TEXT, a_pRoot));
            if (l_pName != nullptr)
              l_pName->setText(helpers::s2ws(a_pPlayer->m_sName).c_str());

            if (a_bSelected) {
              gui::CDustbinCheckbox *l_pCheck = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("PlayerSelect", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, a_pRoot));
              if (l_pCheck != nullptr)
                l_pCheck->setChecked(true);
            }
          }
          else {
            irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("PlayerTab", irr::gui::EGUIET_TAB, a_pRoot));
            if (l_pTab != nullptr)
              l_pTab->setVisible(false);

            if (a_bAddNew) {
              irr::gui::IGUIElement *l_pAdd = helpers::findElementByNameAndType("PlayerAdd", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, a_pRoot);
              if (l_pAdd != nullptr)
                l_pAdd->setVisible(true);
            }
          }
        }

        /**
        * Set the dialog to a new step and initialize the data
        * @param a_eStep the new step
        */
        void setWizardStep(enWizardStep a_eStep) {
          switch (m_eStep) {
            case enWizardStep::EditProfile: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::Profiles: {
              break;
            }

            case enWizardStep::Controllers: {
              irr::gui::IGUITab *l_pRoot = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));
              if (l_pRoot != nullptr) {
                l_pRoot->setVisible(false);
              }
              break;
            }

            case enWizardStep::GameLevel:
              break;
          }

          m_eStep = a_eStep;

          for (auto l_pStep : m_vSteps) {
            l_pStep->setVisible(false);
          }

          if (m_pDataHandler != nullptr) {
            delete m_pDataHandler;
            m_pDataHandler = nullptr;
          }

          switch (m_eStep) {
            case enWizardStep::Profiles: {
              std::string l_sSelected = m_pState->getGlobal()->getSetting("selectedplayers");
              m_pDataHandler = new CDataHandler_SelectPlayers(&m_cRacePlayers, &m_cChampionship, l_sSelected);

              if (m_vSteps.size() > 0)
                m_vSteps[0]->setVisible(true);

              std::vector<irr::gui::IGUIElement *> l_vRoot;

              std::vector<std::string> l_vSelected = helpers::splitString(l_sSelected, ';');

              for (int i = 0; i < c_iMaxProfiles; i++) {
                irr::gui::IGUIElement *p = helpers::findElementByNameAndType("TabPlayer" + std::to_string(i + 1), irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());

                if (p != nullptr) {
                  // The names are store URL encoded in the list so we need to encode the string to search for
                  bool l_bChecked = false;
                    
                  if (i < m_vProfiles.size()) {
                    std::string l_sName = messages::urlEncode(m_vProfiles[i].m_sName);
                    l_bChecked = std::find(l_vSelected.begin(), l_vSelected.end(), l_sName) != l_vSelected.end();
                  }

                  fillPlayer(i < m_vProfiles.size() ? &m_vProfiles[i] : nullptr, p, l_bChecked, i == (int)m_vProfiles.size());
                }
              }

              break;
            }

            case enWizardStep::Controllers: {
              CDataHandler_Controls *l_pHandler = new CDataHandler_Controls(m_vProfiles, &m_cChampionship);
              m_pDataHandler = l_pHandler;

              if (l_pHandler->allControllersAssigned()) {
                printf("No Joystick Assignment necessary.");
                setWizardStep(enWizardStep::GameLevel);
                return;
              }
              
              irr::gui::IGUITab *l_pRoot = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));
              if (l_pRoot != nullptr) {
                l_pRoot->setVisible(true);
              }

              break;
            }

            case enWizardStep::EditProfile: {
              if (m_vSteps.size() > 0)
                m_vSteps[0]->setVisible(true);

              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(true);
              break;
            }

            default:
              // Nothing to do here, this should never happen
              break;
          }
        }

      public:
        CMenuNewGameWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_eStep     (enWizardStep::Unknown)
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_newgamewizard.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          int l_iIndex = 1;

          do {
            irr::gui::IGUIElement *l_pStep = helpers::findElementByNameAndType("gamewizard_step" + std::to_string(l_iIndex), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement());
            if (l_pStep != nullptr)
              m_vSteps.push_back(l_pStep);
            else
              break;

            l_iIndex++;
          }
          while (true);

          printf("%i profile wizard steps found\n", (int)m_vSteps.size());

          m_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));

          m_cSettings   .deserialize(m_pState->getGlobal()->getSetting("gamesetup"));

          printf("%i profiles found.\n", (int)m_vProfiles.size());

          setWizardStep(enWizardStep::Profiles);
        }

        virtual ~CMenuNewGameWizard() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = IMenuHandler::OnEvent(a_cEvent);

          if (!l_bRet) {
            if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
              std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

              if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
                if (l_sSender == "ok") {
                  switch (m_eStep) {
                    case enWizardStep::Profiles: {
                      if (m_pDataHandler != nullptr) {
                        std::vector<std::string> l_vSelected = reinterpret_cast<CDataHandler_SelectPlayers *>(m_pDataHandler)->getSelectedPlayers();

                        int l_iPlayerId = 0;
                        for (auto l_sName : l_vSelected) {
                          for (auto &l_cPlayer: m_vProfiles) {
                            if (l_cPlayer.m_sName == l_sName) {
                              printf("Player \"%s\" selected.\n", l_sName.c_str());
                              l_cPlayer.m_iPlayerId = l_iPlayerId++;
                              m_cChampionship.m_vPlayers.push_back(data::SChampionshipPlayer(l_cPlayer.m_iPlayerId, l_cPlayer.m_sName));
                              break;
                            }
                          }
                        }

                        setWizardStep(enWizardStep::Controllers);
                      }
                    }
                  }

                  l_bRet = true;
                }
                else if (l_sSender == "cancel") {
                  createMenu("menu_main", m_pDevice, m_pManager, m_pState);
                  l_bRet = true;
                }
                else if (l_sSender == "PlayerAdd") {
                  setWizardStep(enWizardStep::EditProfile);
                  l_bRet = true;
                }
                else if (l_sSender == "BtnProfileOK") {
                  setWizardStep(enWizardStep::Profiles);
                  l_bRet = true;
                }
                else if (l_sSender == "BtnProfileCancel") {
                  setWizardStep(enWizardStep::Profiles);
                  l_bRet = true;
                }
                else if (l_sSender == "PlayerEdit") {
                  irr::gui::IGUIElement *l_pParent = a_cEvent.GUIEvent.Caller->getParent();
                  if (l_pParent != nullptr)
                    l_pParent = l_pParent->getParent();

                  std::string l_sName = l_pParent->getName();
                  int l_iIndex = std::atoi(l_sName.substr(l_sName.size() - 1).c_str()) - 1;
                  printf("==> %s [%i]\n", l_pParent->getName(), l_iIndex);
                  l_bRet = true;
                }
                else printf("Button \"%s\" clicked.\n", l_sSender.c_str());
              }
            }
          }

          return l_bRet;
        }

        virtual bool run() override {
          if (m_eStep == enWizardStep::Controllers) {
            if (m_pDataHandler != nullptr) {
              CDataHandler_Controls *l_pHandler = reinterpret_cast<CDataHandler_Controls *>(m_pDataHandler);

              if (l_pHandler->allControllersAssigned())
                setWizardStep(enWizardStep::GameLevel);
            }
          }
          return false;
        }
      };

    IMenuHandler *createMenuNewGameWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNewGameWizard(a_pDevice, a_pManager, a_pState);
    }
  }
}