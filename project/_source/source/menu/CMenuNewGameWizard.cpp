// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <menu/datahandlers/CDataHandler_SelectPlayers.h>
#include <menu/datahandlers/CDataHandler_Customize.h>
#include <menu/datahandlers/CDataHandler_Controls.h>
#include <controller/CControllerGame.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <messages/CSerializer64.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <menu/IMenuHandler.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <algorithm>
#include <random>
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
          Controllers,
          GameType,
          GameLevel,
          Custom,
          EditProfile_Name,
          EditProfile_Short,
          EditProfile_AiHelp,
          EditProfile_Ctrl,
          EditProfile_Texture
        };

        /**
        * The selected game cup.
        */
        enum class enGameType {
          Unknown,
          FreeRacing,
          Cup,
          NetworkFree,
          NetworkCup,
          NetworkClient
        };

        enGameType m_eGameType;

        data::SRacePlayers  m_cRacePlayers;     /**< The available race players */
        data::SChampionship m_cChampionship;    /**< The championship that might be started */
        data::SGameSettings m_cSettings;        /**< The game settings */

        std::vector<irr::gui::IGUIElement *> m_vSteps;      /**< The root GUI element for each available step */
        std::vector<data::SPlayerData      > m_vProfiles;   /**< A vector with all player data */

        std::vector<std::string> m_vSelectedPlayers;

        enWizardStep m_eStep;   /**< The active wizard step */

        /**
        * Save the championship to the global data
        * @param a_bNetClient true if the setup is for a game as network client
        */
        void saveChampionship(bool a_bNetClient) {
          m_pState->getGlobal()->setGlobal("championship", m_cChampionship.serialize());

          int l_iGridSize = 1;

          if (m_cSettings.m_bFillGridAI && !a_bNetClient) {
            if (m_cSettings.m_bFillGridAI) {
              if (m_cSettings.m_iGridSize == 0)
                l_iGridSize = 2;
              else if (m_cSettings.m_iGridSize == 1)
                l_iGridSize = 4;
              else if (m_cSettings.m_iGridSize == 2)
                l_iGridSize = 8;
              else if (m_cSettings.m_iGridSize == 3)
                l_iGridSize = 12;
              else if (m_cSettings.m_iGridSize == 4)
                l_iGridSize = 16;
            }
          }

          // We fill a vector with the grid positions and ..
          std::vector<int> l_vGrid;

          for (int i = 0; i < (m_cChampionship.m_vPlayers.size() > l_iGridSize ? m_cChampionship.m_vPlayers.size() : l_iGridSize); i++)
            l_vGrid.push_back(i);

          // .. if necessary shuffle the vector
          if (m_cSettings.m_bRandomFirstRace) {
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };

            std::shuffle(l_vGrid.begin(), l_vGrid.end(), l_cRe);
          }
          data::SRacePlayers l_cPlayers;

          int l_iNum = 1;

          // Now we iterate all selected players ..
          for (std::vector<std::string>::iterator it = m_vSelectedPlayers.begin(); it != m_vSelectedPlayers.end(); it++) {
            // .. search for the matching profile ..
            for (std::vector<data::SPlayerData>::iterator it2 = m_vProfiles.begin(); it2 != m_vProfiles.end(); it2++) {
              if (*it == (*it2).m_sName) {
                // .. and create a copy of the player element (will be modified for each game)
                data::SPlayerData l_cPlayer;
                l_cPlayer.copyFrom(*it2);

                // The grid position for the player is the first element of the grid vector
                l_cPlayer.m_iGridPos = *l_vGrid.begin();
                // Remove the first element that was just assigned
                l_vGrid.erase(l_vGrid.begin());

                l_cPlayer.m_iViewPort = l_iNum;
                l_cPlayer.m_iPlayerId = l_iNum++;
                l_cPlayers.m_vPlayers.push_back(l_cPlayer);

                break;
              }
            }
          }

          data::SFreeGameSlots l_cSlots = data::SFreeGameSlots();

          l_cSlots.m_vSlots = l_vGrid;
          m_pState->getGlobal()->setGlobal("free_game_slots", l_cSlots  .serialize());
          m_pState->getGlobal()->setGlobal("raceplayers"    , l_cPlayers.serialize());

          m_pState->getGlobal()->setSetting("gamesetup", m_cSettings.serialize());
          platform::saveSettings();
        }

        /**
        * Start the game
        */
        void startGame() {
          switch (m_eGameType) {
          case enGameType::FreeRacing:
            m_pManager->pushToMenuStack("menu_selecttrack");
            m_pManager->pushToMenuStack("menu_fillgrid");
            break;

          case enGameType::Cup:
            m_pManager->pushToMenuStack("menu_selectcup");
            m_pManager->pushToMenuStack("menu_fillgrid");
            break;

          case enGameType::NetworkFree:
            m_pManager->pushToMenuStack("menu_selecttrack");
            m_pManager->pushToMenuStack("menu_fillgrid");
            m_pManager->pushToMenuStack("menu_startserver");
            break;

          case enGameType::NetworkCup:
            m_pManager->pushToMenuStack("menu_selectcup");
            m_pManager->pushToMenuStack("menu_fillgrid");
            m_pManager->pushToMenuStack("menu_startserver");
            break;
          }

          saveChampionship(false);
          createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
        }

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
            case enWizardStep::EditProfile_Name: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::EditProfile_Short: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::EditProfile_AiHelp: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::EditProfile_Ctrl: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::EditProfile_Texture: {
              irr::gui::IGUIElement *l_pRoot = helpers::findElementByNameAndType("GameWizardEditProfile", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());
              if (l_pRoot != nullptr) l_pRoot->setVisible(false);
              break;
            }

            case enWizardStep::Profiles: {
              std::vector<std::string> m_vSelectedPlayers = reinterpret_cast<CDataHandler_SelectPlayers *>(m_pDataHandler)->getSelectedPlayers();

              int l_iPlayerId = 1;
              for (auto l_sName : m_vSelectedPlayers) {
                for (auto &l_cPlayer: m_vProfiles) {
                  if (l_cPlayer.m_sName == l_sName) {
                    printf("Player \"%s\" selected.\n", l_sName.c_str());
                    l_cPlayer.m_iPlayerId = l_iPlayerId++;
                    m_cChampionship.m_vPlayers.push_back(data::SChampionshipPlayer(l_cPlayer.m_iPlayerId, l_cPlayer.m_sName));
                    break;
                  }
                }
              }
              break;
            }

            case enWizardStep::Controllers: {
              irr::gui::IGUITab *l_pRoot = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));
              if (l_pRoot != nullptr) {
                l_pRoot->setVisible(false);
              }
              break;
            }

            case enWizardStep::Custom: {
              break;
            }

            case enWizardStep::GameType: {
              break;
            }
              
            case enWizardStep::GameLevel: {
              break;
            }
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
              m_pDataHandler = new CDataHandler_SelectPlayers(&m_cRacePlayers, &m_cChampionship, m_vProfiles, l_sSelected);

              if (m_vSteps.size() > 0)
                m_vSteps[0]->setVisible(true);

              std::vector<irr::gui::IGUIElement *> l_vRoot;

              m_vSelectedPlayers = helpers::splitString(l_sSelected, ';');

              for (int i = 0; i < c_iMaxProfiles; i++) {
                irr::gui::IGUIElement *p = helpers::findElementByNameAndType("TabPlayer" + std::to_string(i + 1), irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement());

                if (p != nullptr) {
                  // The names are store URL encoded in the list so we need to encode the string to search for
                  bool l_bChecked = false;
                    
                  if (i < m_vProfiles.size()) {
                    std::string l_sName = messages::urlEncode(m_vProfiles[i].m_sName);
                    l_bChecked = std::find(m_vSelectedPlayers.begin(), m_vSelectedPlayers.end(), l_sName) != m_vSelectedPlayers.end();
                  }

                  fillPlayer(i < m_vProfiles.size() ? &m_vProfiles[i] : nullptr, p, l_bChecked, i == (int)m_vProfiles.size());
                }
              }

              gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
              if (l_pOk != nullptr)
                l_pOk->setVisible(true);

              break;
            }

            case enWizardStep::Controllers: {
              CDataHandler_Controls *l_pHandler = new CDataHandler_Controls(m_vProfiles, &m_cChampionship);
              m_pDataHandler = l_pHandler;

              if (l_pHandler->allControllersAssigned()) {
                printf("No Joystick Assignment necessary.");
                setWizardStep(enWizardStep::GameType);
                return;
              }
              
              irr::gui::IGUITab *l_pRoot = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("selectctrl_dialog", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));
              if (l_pRoot != nullptr) {
                l_pRoot->setVisible(true);
              }

              break;
            }

            case enWizardStep::GameType: {
              if (m_vSteps.size() > 1)
                m_vSteps[1]->setVisible(true);

              gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
              if (l_pOk != nullptr)
                l_pOk->setVisible(false);

              break;
            }

            case enWizardStep::GameLevel: {
              if (m_vSteps.size() > 2)
                m_vSteps[2]->setVisible(true);

              gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
              if (l_pOk != nullptr)
                l_pOk->setVisible(false);

              break;
            }

            case enWizardStep::Custom: {
              if (m_vSteps.size() > 3)
                m_vSteps[3]->setVisible(true);

              gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
              if (l_pOk != nullptr)
                l_pOk->setVisible(true);

              m_pDataHandler = new CDataHandler_Customize(m_cSettings);
              break;
            }

            case enWizardStep::EditProfile_Texture: {
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
          m_eGameType (enGameType  ::Unknown),
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

          m_cSettings.deserialize(m_pState->getGlobal()->getSetting("gamesetup"));

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
                        setWizardStep(enWizardStep::Controllers);
                      }
                      break;
                    }

                    case enWizardStep::Custom: {
                      m_cSettings = reinterpret_cast<CDataHandler_Customize *>(m_pDataHandler)->getSettings();
                      startGame();
                      break;
                    }

                    default:
                      break;
                  }

                  l_bRet = true;
                }
                else if (l_sSender == "cancel") {
                  switch (m_eStep) {
                    case enWizardStep::Profiles:
                      createMenu("menu_main", m_pDevice, m_pManager, m_pState);
                      break;

                    case enWizardStep::GameType:
                      setWizardStep(enWizardStep::Profiles);
                      break;

                    case enWizardStep::GameLevel:
                      setWizardStep(enWizardStep::GameType);
                      break;

                    case enWizardStep::Custom:
                      setWizardStep(enWizardStep::GameLevel);
                      break;

                  }
                  
                  l_bRet = true;
                }
                else if (l_sSender == "PlayerAdd") {
                  setWizardStep(enWizardStep::EditProfile_Name);
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
                else if (l_sSender.substr(0, std::string("GameType").length()) == "GameType") {
                  int l_iType = std::atoi(l_sSender.substr(std::string("GameType").length()).c_str());

                  switch (l_iType) {
                    case 0:
                      m_eGameType = enGameType::FreeRacing;
                      printf("GameType: Free Racing\n");
                      break;

                    case 1:   // MarbleGP Cup
                      m_eGameType = enGameType::Cup;
                      printf("GameType: MarbleGP Cup\n");
                      break;

                    case 2:   // Network | Free Racing
                      m_eGameType = enGameType::NetworkFree;
                      printf("GameType: Network | Free Racing\n");
                      break;

                    case 3:   // Network | MarbleGP Cup
                      m_eGameType = enGameType::NetworkCup;
                      printf("GameType: Network | MarbleGP Cup\n");
                      break;

                    case 4:   // Network | Join Server
                      saveChampionship(true);
                      m_pManager->pushToMenuStack("menu_joinserver"  );
                      createMenu("menu_searchserver", m_pDevice, m_pManager, m_pState);
                      m_eGameType = enGameType::NetworkClient;
                      break;
                  }

                  if (l_iType >= 0 && l_iType < 4) {
                    setWizardStep(enWizardStep::GameLevel);
                  }
                }
                else if (l_sSender.substr(0, std::string("AiLevel").size()) == "AiLevel") {
                  int l_iAiLevel = std::atoi(l_sSender.substr(std::string("AiLevel").length()).c_str());
                  printf("Ai Level: %i\n", l_iAiLevel);
                  
                  switch (l_iAiLevel) {
                    case 0:
                      m_cSettings.m_bFillGridAI = false;
                      break;

                    case 1:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 1;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble3_2;
                      m_cSettings.m_eGridPos         = data::SGameSettings::enGridPos  ::LastRace;
                      m_cSettings.m_bRandomFirstRace = false;
                      m_cSettings.m_bReverseGrid     = false;
                      break;

                    case 2:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 2;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble2;
                      m_cSettings.m_eGridPos         = data::SGameSettings::enGridPos  ::LastRace;
                      m_cSettings.m_bRandomFirstRace = false;
                      m_cSettings.m_bReverseGrid     = false;
                      break;

                    case 3:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::AllClasses;
                      m_cSettings.m_eGridPos         = data::SGameSettings::enGridPos  ::LastRace;
                      m_cSettings.m_bRandomFirstRace = true;
                      m_cSettings.m_bReverseGrid     = true;
                      break;

                    case 4:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble2_GP;
                      m_cSettings.m_eGridPos         = data::SGameSettings::enGridPos  ::LastRace;
                      m_cSettings.m_bRandomFirstRace = true;
                      m_cSettings.m_bReverseGrid     = true;
                      break;

                    case 5:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::MarbleGP;
                      m_cSettings.m_eGridPos         = data::SGameSettings::enGridPos  ::LastRace;
                      m_cSettings.m_bRandomFirstRace = true;
                      m_cSettings.m_bReverseGrid     = true;
                      break;

                    case 6:
                      printf("Custom!\n");
                      break;
                  }

                  if (l_iAiLevel >= 0 && l_iAiLevel < 6) {
                    startGame();
                  }
                  else if (l_iAiLevel == 6) setWizardStep(enWizardStep::Custom);
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
                setWizardStep(enWizardStep::GameType);
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