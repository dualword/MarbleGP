// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <menu/datahandlers/CDataHandler_SelectPlayers.h>
#include <menu/datahandlers/CDataHandler_EditProfile.h>
#include <menu/datahandlers/CDataHandler_Customize.h>
#include <menu/datahandlers/CDataHandler_Controls.h>
#include <controller/CControllerGame.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <messages/CSerializer64.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CDataHelpers.h>
#include <gameclasses/SPlayer.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <gui/CReactiveLabel.h>
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
        /**
        * Steps of the "New Game" wizard
        */
        enum class enWizardStep {
          Unknown,
          Profiles,
          Controllers,
          GameType,
          GameLevel,
          Custom,
          EditProfile
        };

        /**
        * The selected game type
        */
        enum class enGameType {
          Unknown,
          FreeRacing,
          Cup,
          NetworkFree,
          NetworkCup,
          NetworkClient
        };

        enWizardStep      m_eStep;      /**< The active wizard step */
        enGameType        m_eGameType;  /**< The selected game type */

        data::SRacePlayers  m_cRacePlayers;     /**< The available race players */
        data::SChampionship m_cChampionship;    /**< The championship that might be started */
        data::SGameSettings m_cSettings;        /**< The game settings */

        std::vector<irr::gui::IGUIElement *> m_vSteps;      /**< The root GUI element for each available step */
        std::vector<data::SPlayerData      > m_vProfiles;   /**< A vector with all player data */

        std::vector<std::string> m_vSelectedPlayers;

        irr::video::ITexture *m_pTextureRtt;    /**< The render target texture for the texture preview */

        irr::scene::ISceneManager *m_pPreviewSmgr;  /**< The scene manager for the texture preview */

        /**
        * Save the championship to the global data
        * @param a_bNetClient true if the setup is for a game as network client
        */
        void saveChampionship(bool a_bNetClient) {
          m_pState->getGlobal()->setGlobal("championship", m_cChampionship.serialize());
          
          helpers::saveProfiles(m_vProfiles);

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

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

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

                controller::CControllerGame p = controller::CControllerGame();
                p.deserialize(l_cPlayer.m_sControls);

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
            gui::CReactiveLabel* l_pName = reinterpret_cast<gui::CReactiveLabel*>(helpers::findElementByNameAndType("PlayerName", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ReactiveLabelId, a_pRoot));
            if (l_pName != nullptr)
              l_pName->setText(helpers::s2ws(a_pPlayer->m_sName).c_str());

            if (a_bSelected) {
              gui::CDustbinCheckbox *l_pCheck = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("PlayerSelect", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, a_pRoot));
              if (l_pCheck != nullptr)
                l_pCheck->setChecked(true);
            }
          }

          irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("PlayerTab", irr::gui::EGUIET_TAB, a_pRoot));
          if (l_pTab != nullptr)
            l_pTab->setVisible(a_pPlayer != nullptr);

          irr::gui::IGUIElement *l_pAdd = helpers::findElementByNameAndType("PlayerAdd", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, a_pRoot);
          if (l_pAdd != nullptr)
            l_pAdd->setVisible(a_bAddNew);
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
              m_vSelectedPlayers = reinterpret_cast<CDataHandler_SelectPlayers *>(m_pDataHandler)->getSelectedPlayers();
              m_cChampionship.m_vPlayers.clear();

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

          gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElement("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId));
          if (l_pBtn != nullptr)
            l_pBtn->setImage(m_eStep == enWizardStep::GameType ? "data/images/btn_ok.png" : "data/images/arrow_right.png");

          l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElement("cancel", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId));
          if (l_pBtn != nullptr)
            l_pBtn->setImage(m_eStep == enWizardStep::Profiles ? "data/images/btn_cancel.png" : "data/images/arrow_left.png");

          switch (m_eStep) {
            case enWizardStep::Profiles: {
              // First we reset the player IDs
              for (std::vector<data::SPlayerData>::iterator it2 = m_vProfiles.begin(); it2 != m_vProfiles.end(); it2++) {
                (*it2).m_iPlayerId = -1;
              }

              std::string l_sSelected = m_pState->getGlobal()->getSetting("selectedplayers");

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

              std::vector<std::string> l_vToRemove;
              for (auto l_sSelected : m_vSelectedPlayers) {
                bool l_bRemove = true;
                for (auto l_cProfile : m_vProfiles) {
                  if (l_sSelected == messages::urlEncode(l_cProfile.m_sName)) {
                    l_bRemove = false;
                    break;
                  }
                }
                if (l_bRemove)
                  l_vToRemove.push_back(l_sSelected);
              }

              for (auto l_sRemove : l_vToRemove) {
                for (std::vector<std::string>::iterator l_itSelected = m_vSelectedPlayers.begin(); l_itSelected != m_vSelectedPlayers.end(); l_itSelected++) {
                  if (*l_itSelected == l_sRemove) {
                    m_vSelectedPlayers.erase(l_itSelected);
                    break;
                  }
                }
              }

              std::string l_sNewSelected;
              for (std::vector<std::string>::iterator l_itSeleted = m_vSelectedPlayers.begin(); l_itSeleted != m_vSelectedPlayers.end(); l_itSeleted++) {
                if (l_itSeleted != m_vSelectedPlayers.begin())
                  l_sNewSelected += ";";
                l_sNewSelected += *l_itSeleted;
              }

              m_pDataHandler = new CDataHandler_SelectPlayers(&m_cRacePlayers, &m_cChampionship, m_vProfiles, l_sNewSelected);
              printf("==> %i\n", (int)m_vSelectedPlayers.size());
              gui::CMenuButton *l_pOk = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
              if (l_pOk != nullptr)
                l_pOk->setVisible(m_vSelectedPlayers.size() > 0 || CGlobal::getInstance()->getSettingData().m_bNoPlayerRace);

              break;
            }

            case enWizardStep::Controllers: {
              CDataHandler_Controls *l_pHandler = new CDataHandler_Controls(&m_vProfiles, &m_cChampionship);
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
          IMenuHandler  (a_pDevice, a_pManager, a_pState),
          m_eGameType   (enGameType   ::Unknown),
          m_eStep       (enWizardStep ::Unknown),
          m_pTextureRtt (nullptr),
          m_pPreviewSmgr(nullptr)
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

          m_pTextureRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "texture_rtt");
        }

        virtual ~CMenuNewGameWizard() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
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

                    case enWizardStep::EditProfile: {
                      printf("Edit Profile ok.\n");
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
                else if (l_sSender == "EditProfileOk" || l_sSender == "ProfileData_Tutorial") {
                  if (m_eStep == enWizardStep::EditProfile) {
                    CDataHandler_EditProfile *l_pHandler = reinterpret_cast<CDataHandler_EditProfile *>(m_pDataHandler);

                    if (l_pHandler->getProfileIndex() == -1) {
                      m_vProfiles.push_back(data::SPlayerData(l_pHandler->getEditedProfile()));
                    }
                    else {
                      m_vProfiles[l_pHandler->getProfileIndex()] = data::SPlayerData(l_pHandler->getEditedProfile());
                    }

                    helpers::saveProfiles(m_vProfiles);
                    platform::saveSettings();

                    if (l_sSender == "ProfileData_Tutorial") {
                      data::SPlayerData l_cPlayer = reinterpret_cast<CDataHandler_EditProfile *>(m_pDataHandler)->getEditedProfile();

                      std::vector<data::SPlayerData> l_vProfiles = {
                        l_cPlayer
                      };

                      data::SGameSettings l_cSettings;

                      l_cPlayer.m_eType     = data::enPlayerType::Local;
                      l_cPlayer.m_iViewPort = 1;
                      l_cPlayer.m_iGridPos  = 1;
                      l_cPlayer.m_iPlayerId = 1;

                      data::SGameData l_cData;

                      l_cData.m_iLaps       = 1;
                      l_cData.m_sTrack      = "tutorial";
                      l_cData.m_bIsTutorial = true;

                      CGlobal::getInstance()->setGlobal("gamedata", l_cData.serialize());

                      data::SRacePlayers l_cPlayers;
                      l_cPlayers.m_vPlayers.push_back(l_cPlayer);

                      CGlobal::getInstance()->setSetting("selectedplayers", messages::urlEncode(l_cPlayer.m_sName));
                      platform:: saveSettings();

                      m_pManager->pushToMenuStack("menu_newgamewizard");
                      CGlobal::getInstance()->setGlobal("raceplayers", l_cPlayers.serialize());

                      m_pState->setState(state::enState::Game);
                    }
                    else setWizardStep(enWizardStep::Profiles);
                  }
                }
                else if (l_sSender == "EditProfileCancel") {
                  if (m_eStep == enWizardStep::EditProfile)
                    setWizardStep(enWizardStep::Profiles);
                }
                else if (l_sSender == "PlayerAdd") {
                  setWizardStep(enWizardStep::EditProfile);
                  m_pDataHandler = new CDataHandler_EditProfile(-1, data::SPlayerData(), m_pTextureRtt);
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
                  setWizardStep(enWizardStep::EditProfile);
                  m_pDataHandler = new CDataHandler_EditProfile(l_iIndex, m_vProfiles[l_iIndex], m_pTextureRtt);
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
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble3;
                      m_cSettings.m_bRandomFirstRace = false;
                      m_cSettings.m_bReverseGrid     = false;
                      break;

                    case 2:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 2;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble2;
                      m_cSettings.m_bRandomFirstRace = false;
                      m_cSettings.m_bReverseGrid     = false;
                      break;

                    case 3:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::AllClasses;
                      m_cSettings.m_bRandomFirstRace = true;
                      m_cSettings.m_bReverseGrid     = true;
                      break;

                    case 4:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::Marble2_GP;
                      m_cSettings.m_bRandomFirstRace = true;
                      m_cSettings.m_bReverseGrid     = true;
                      break;

                    case 5:
                      m_cSettings.m_bFillGridAI      = true;
                      m_cSettings.m_iGridSize        = 4;
                      m_cSettings.m_eRaceClass       = data::SGameSettings::enRaceClass::MarbleGP;
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
                else if (l_sSender == "CancelAssignJoystick") {
                  setWizardStep(enWizardStep::Profiles);
                }
                else if (l_sSender == "BtnConfirmYes") {
                  CDataHandler_EditProfile *p = reinterpret_cast<CDataHandler_EditProfile *>(m_pDataHandler);
                  if (p != nullptr) {
                    int l_iNum = p->getProfileIndex();
                    if (l_iNum >= 0 && l_iNum < m_vProfiles.size()) {
                      auto l_itProfile = m_vProfiles.begin();
                      while (l_iNum > 0 && l_itProfile != m_vProfiles.end()) {
                        l_itProfile++;
                        l_iNum--;
                      }

                      if (l_itProfile != m_vProfiles.end()) {
                        printf("Delete Profile \"%s\"\n", (*l_itProfile).m_sName.c_str());
                        m_vProfiles.erase(l_itProfile);
                        setWizardStep(enWizardStep::Profiles);
                        helpers::saveProfiles(m_vProfiles);
                        platform::saveSettings();
                      }
                      else printf("Profile with index %i not found.\n", p->getProfileIndex());
                    }
                  }
                  
                  irr::gui::IGUIElement *l_pEditor = findElement("GameWizardEditProfile");
                  if (l_pEditor != nullptr)
                    l_pEditor->setVisible(false);
                }
                else if (l_sSender == "PlayerName") {
                  if (a_cEvent.GUIEvent.Caller->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ReactiveLabelId) {
                    printf("Show player details of %s\n", helpers::ws2s(a_cEvent.GUIEvent.Caller->getText()).c_str());
                    irr::gui::IGUIElement *p = helpers::findElementByName("DialogPlayerDetails", m_pGui->getRootGUIElement());
                    if (p != nullptr) {
                      p->setVisible(true);

                      for (auto l_cProfile : m_vProfiles) {
                        if (l_cProfile.m_sName == helpers::ws2s(a_cEvent.GUIEvent.Caller->getText())) {
                          irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerDetail_Name", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                          if (l_pLabel != nullptr)
                            l_pLabel->setText(helpers::s2ws(l_cProfile.m_sName).c_str());

                          l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerDetail_Short", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                          if (l_pLabel != nullptr)
                            l_pLabel->setText(helpers::s2ws(l_cProfile.m_sShortName).c_str());

                          l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerDetail_AiHelp", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                          if (l_pLabel != nullptr)
                            l_pLabel->setText(helpers::s2ws(helpers::getAiHelpString(l_cProfile.m_eAiHelp)).c_str());

                          l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerDetail_Ctrl", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                          if (l_pLabel != nullptr)
                            l_pLabel->setText(helpers::getControllerType(l_cProfile.m_sControls).c_str());

                          irr::gui::IGUIImage *l_pImg = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("PlayerDetail_TextureRtt", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
                          if (l_pImg != nullptr) {
                            m_pPreviewSmgr = m_pSmgr->createNewSceneManager();
                            m_pPreviewSmgr->loadScene("data/scenes/texture_scene.xml");
                            irr::scene::ICameraSceneNode *l_pCam = m_pPreviewSmgr->addCameraSceneNode(nullptr, irr::core::vector3df(-2.0f, 2.0f, -5.0f), irr::core::vector3df(0.0f));
                            l_pCam->setAspectRatio(1.0f);
                            l_pImg->setImage(m_pTextureRtt);

                            irr::scene::ISceneNode *l_pMarbleNode = m_pPreviewSmgr->getSceneNodeFromName("marble");
                            if (l_pMarbleNode != nullptr) {
                              l_pMarbleNode->getMaterial(0).setTexture(0, helpers::createTexture(l_cProfile.m_sTexture != "" ? l_cProfile.m_sTexture : "default://number=1", m_pDrv, m_pFs));
                            }
                          }
                        }
                      }
                    }
                  }
                }
                else if (l_sSender == "BtnClosePlayerDetails") {
                  irr::gui::IGUIElement *p = helpers::findElementByName("DialogPlayerDetails", m_pGui->getRootGUIElement());
                  if (p != nullptr)
                    p->setVisible(false);

                  if (m_pPreviewSmgr != nullptr) {
                    m_pPreviewSmgr->drop();
                    m_pPreviewSmgr = nullptr;
                  }
                }
                else printf("Button \"%s\" clicked.\n", l_sSender.c_str());
              }
              else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
                if (m_eStep == enWizardStep::Profiles) {
                  irr::gui::IGUIElement *p = findElement("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId);
                  if (p != nullptr)
                    p->setVisible(CGlobal::getInstance()->getSettingData().m_bNoPlayerRace || reinterpret_cast<CDataHandler_SelectPlayers *>(m_pDataHandler)->getNumberOfSelectedPlayers() > 0);
                }
              }
            }
          }

          return l_bRet;
        }

        virtual bool run() override {
          IMenuHandler::run();

          if (m_eStep == enWizardStep::Controllers) {
            if (m_pDataHandler != nullptr) {
              CDataHandler_Controls *l_pHandler = reinterpret_cast<CDataHandler_Controls *>(m_pDataHandler);

              if (l_pHandler->allControllersAssigned())
                setWizardStep(enWizardStep::GameType);
            }
          }

          if (m_pPreviewSmgr != nullptr && m_pTextureRtt != nullptr) {
            m_pDrv->setRenderTarget(m_pTextureRtt, true, true);
            m_pPreviewSmgr->drawAll();
            m_pDrv->setRenderTarget(nullptr, false, false);
          }
          return false;
        }
      };

    IMenuHandler *createMenuNewGameWizard(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNewGameWizard(a_pDevice, a_pManager, a_pState);
    }
  }
}