// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/CControllerGame.h>
#include <messages/CSerializer64.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gui/CMenuBackground.h>
#include <platform/CPlatform.h>
#include <menu/IMenuHandler.h>
#include <menu/CMenuFactory.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <algorithm>
#include <random>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSetupGame
    * @author Christian Keimel
    * This menu is used for game setup (players, class...)
    */
    class CMenuSetupGame : public IMenuHandler {
      private:
        int m_iMaxIndex;  /**< The maximum index of the player UI */

        data::SGameSettings m_cSettings;    /**< The game settings data struct */

        /**
        * A vector of tuples with the UI elements for the players:
        * [0] = Number Label
        * [1] = Name Label
        * [2] = Remove Button
        * [3] = Add Button
        */
        std::vector<std::tuple<irr::gui::IGUIStaticText *, irr::gui::IGUIStaticText *, gui::CMenuButton *, gui::CMenuButton *>> m_vPlayerUI;

        std::vector<std::tuple<irr::gui::IGUIStaticText *, irr::gui::IGUIStaticText *, gui::CMenuButton *, gui::CMenuButton *>>::iterator m_itAdd;

        std::vector<std::string> m_vSelectedPlayers;  /**< The players that are selected for the next game */

        std::vector<data::SPlayerData> m_vProfiles;  /**< A vector with all player data */

        irr::gui::IGUITab *m_pSelectPlayer; /**< The "select player" dialog */

        std::vector<gui::CMenuButton *> m_vSelectPlayer;  /**< The player buttons in the "select player" dialog */

        irr::s32 m_iAssigned;   /**< Was a joystick assigned in the last frame? Store the joystick index here. */

        std::vector<std::string> m_vAssignJoystick;   /**< Vector of the players which need to assign a joystick */

        gui::CMenuButton *m_pOk;  /**< The OK button */

        irr::gui::IGUITab        *m_pSelectCtrl;    /**< The root element of the "select controller" dialog */
        irr::gui::IGUIStaticText *m_pSelectName;    /**< The name element of the "select controller" dialog */

        std::vector<irr::u8> m_vUsedJoysticks;

        std::vector<irr::gui::IGUIElement *> m_vSetupUI;   /**< All elements relevant for game setup, hidden if Network Client is selected */

        /**
        * Update the list of selected players
        */
        void updateSelectedPlayers() {
          std::vector<std::string>::iterator it2 = m_vSelectedPlayers.begin();

          bool b = true;
          int l_iNum = 0;

          for (std::vector<std::tuple<irr::gui::IGUIStaticText*, irr::gui::IGUIStaticText*, gui::CMenuButton*, gui::CMenuButton*>>::iterator it = m_vPlayerUI.begin(); it != m_vPlayerUI.end(); it++) {
            std::get<0>(*it)->setVisible(it2 != m_vSelectedPlayers.end());
            std::get<1>(*it)->setVisible(it2 != m_vSelectedPlayers.end());
            std::get<2>(*it)->setVisible(it2 != m_vSelectedPlayers.end());
            std::get<3>(*it)->setVisible(it2 == m_vSelectedPlayers.end() && b && l_iNum < m_vProfiles.size());

            if (it2 != m_vSelectedPlayers.end())  {
              std::get<1>(*it)->setText(helpers::s2ws(*it2).c_str());
              it2++;
            }
            else b = false;

            l_iNum++;
          }

          if (m_pSelectCtrl != nullptr && m_pSelectName != nullptr && m_vAssignJoystick.size() > 0) {
            m_pSelectCtrl->setVisible(true);

            std::wstring s = L"Player " + helpers::s2ws(*m_vAssignJoystick.begin()) + L": Select your gamepad by clicking a button.";
            m_pSelectName->setText(s.c_str());
          }

          if (m_pOk != nullptr)
            m_pOk->setVisible(m_vSelectedPlayers.size() > 0);
        }

        void playerSelectCancel() {
          if (m_pSelectPlayer != nullptr) {
            m_pSelectPlayer->setVisible(false);
          }
        }

        void checkAiElements() {
          gui::CSelector        *l_pRaceclass = reinterpret_cast<gui::CSelector        *>(findElementByNameAndType("raceclass"  , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId       , m_pGui->getRootGUIElement()));
          gui::CSelector        *l_pGridSize  = reinterpret_cast<gui::CSelector        *>(findElementByNameAndType("gridsize"   , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId       , m_pGui->getRootGUIElement()));
          gui::CDustbinCheckbox *l_pFillGrid  = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("fillgrid_ai", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement()));

          if (l_pGridSize != nullptr) {
            int l_iSize = std::wcstol(l_pGridSize->getSelectedItem().c_str(), nullptr, 10);
            bool l_bEnabled = l_iSize > m_vSelectedPlayers.size();
            printf("==> %i, %i (%s)\n", l_iSize, (int)m_vSelectedPlayers.size(), l_bEnabled ? "true" : "false");

            if (l_pRaceclass != nullptr)
              l_pRaceclass->setEnabled(l_bEnabled && m_cSettings.m_bFillGridAI);

            if (l_pRaceclass != nullptr)
              l_pRaceclass->setEnabled(l_bEnabled && m_cSettings.m_bFillGridAI);

            l_pGridSize->setEnabled(m_cSettings.m_bFillGridAI);
          }
        }

      public:
        CMenuSetupGame(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_iMaxIndex    (-1), 
          m_itAdd        (m_vPlayerUI.end()),
          m_pSelectPlayer(nullptr),
          m_pOk          (nullptr),
          m_pSelectCtrl  (nullptr),
          m_pSelectName  (nullptr),
          m_iAssigned    (-1)
        {
          m_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));

          std::string l_sSelected = m_pState->getGlobal()->getSetting("selectedplayers");

          if (l_sSelected != "") {
            messages::CSerializer64 l_cSerializer(l_sSelected.c_str());
            while (l_cSerializer.hasMoreMessages()) {
              bool l_bAdd = false;

              std::string l_sName = l_cSerializer.getString();

              for (std::vector<data::SPlayerData>::iterator it = m_vProfiles.begin(); it != m_vProfiles.end(); it++)
                if ((*it).m_sName == l_sName) {
                  l_bAdd = true;


                  controller::CControllerGame l_cCtrl = controller::CControllerGame();
                  l_cCtrl.deserialize((*it).m_sControls);

                  if (l_cCtrl.usesJoystick()) {
                    l_cCtrl.resetJoystick();
                    (*it).m_sControls = l_cCtrl.serialize();
                  }

                  break;
                }

              if (l_bAdd)
                m_vSelectedPlayers.push_back(l_sName);
            }
          }

          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_setupgame.xml", m_pGui->getRootGUIElement(), m_pGui);

          irr::gui::IGUIElement *l_pRoot = m_pGui->getRootGUIElement();

          std::string l_sSettings = m_pState->getGlobal()->getSetting("gamesetup");
          if (l_sSettings != "") {
            m_cSettings.deserialize(l_sSettings);
          }

          m_pSelectPlayer = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("addplayer_dialog", irr::gui::EGUIET_TAB, l_pRoot));

          gui::CDustbinCheckbox *l_pCheckbox = nullptr;
          gui::CSelector        *l_pSelector = nullptr;

          l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("raceclass", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pRoot));
          if (l_pSelector != nullptr) l_pSelector->setSelected(m_cSettings.m_iRaceClass); else printf("Ui element \"raceclass\" not found.");

          l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("starting_positions", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pRoot));
          if (l_pSelector != nullptr) l_pSelector->setSelected(m_cSettings.m_iGridPos); else printf("Ui element \"starting_positions\" not found.");

          l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("gridsize", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pRoot));
          if (l_pSelector != nullptr) {
            checkAiElements();
          }
          else printf("Ui element \"gridsize\" not found.");

          l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("race_finish", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pRoot));
          if (l_pSelector != nullptr) {
            l_pSelector->setSelected(m_cSettings.m_iAutoFinish); 
          }
          
          l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("reverse_grid", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pRoot));
          if (l_pCheckbox != nullptr) l_pCheckbox->setChecked(m_cSettings.m_bReverseGrid); else printf("Ui element \"reverse_grid\" not found.");
          
          l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("randomize_first", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pRoot));
          if (l_pCheckbox != nullptr) l_pCheckbox->setChecked(m_cSettings.m_bRandomFirstRace); else printf("Ui element \"randomize_first\" not found.");
          
          l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("fillgrid_ai", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pRoot));
          if (l_pCheckbox != nullptr) l_pCheckbox->setChecked(m_cSettings.m_bFillGridAI); else printf("Ui element \"fillgrid_ai\" not found.");

          for (int i = 0; i < 8 && m_iMaxIndex == -1; i++) {
            std::string s = "player" + std::to_string(i + 1);
            gui::CMenuBackground *p = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType(s.c_str(), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, l_pRoot));

            if (p != nullptr) {
              irr::gui::IGUIStaticText *l_pNumber = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, p)),
                                       *l_pName   = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_name" , irr::gui::EGUIET_STATIC_TEXT, p));

              gui::CMenuButton *l_pDelete = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p)),
                               *l_pAdd    = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("add_player"   , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p));

              if (l_pNumber != nullptr && l_pName != nullptr && l_pDelete != nullptr && l_pAdd != nullptr) {
                l_pNumber->setText(std::to_wstring(i + 1).c_str());
                m_vPlayerUI.push_back(std::make_tuple(l_pNumber, l_pName, l_pDelete, l_pAdd));
              }
              else {
                printf("Element not found: %s, %s, %s, %s\n", 
                  l_pNumber == nullptr ? "player_label"  : "", 
                  l_pName   == nullptr ? "player_name"   : "", 
                  l_pDelete == nullptr ? "remove_player" : "", 
                  l_pAdd    == nullptr ? "add_player"    : "");

                m_iMaxIndex = i;
              }
            }
            else {
              printf("Menu for player #%i not found.\n", i + 1);
              m_iMaxIndex = i;
            }
          }

          if (m_iMaxIndex == -1)
            m_iMaxIndex = 8;

          for (int i = 0; i < 8; i++) {
            std::string s = "add_pl" + std::to_string(i + 1);
            gui::CMenuButton *p = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType(s.c_str(), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
            if (p != nullptr)
              m_vSelectPlayer.push_back(p);
          }

          m_pOk         = reinterpret_cast<gui::CMenuButton         *>(findElementByNameAndType("ok"               , (irr::gui::EGUI_ELEMENT_TYPE)     gui::g_MenuButtonId    , m_pGui->getRootGUIElement()));
          m_pSelectCtrl = reinterpret_cast<irr::gui::IGUITab        *>(findElementByNameAndType("selectctrl_dialog",                              irr::gui::EGUIET_TAB        , m_pGui->getRootGUIElement()));
          m_pSelectName = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("selectctrl_player",                              irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          updateSelectedPlayers();

          if (m_pSelectCtrl != nullptr && m_pSelectName != nullptr) {
            for (std::vector<std::string>::iterator l_itName = m_vSelectedPlayers.begin(); l_itName != m_vSelectedPlayers.end(); l_itName++) {
              for (std::vector<data::SPlayerData>::iterator l_itPlr = m_vProfiles.begin(); l_itPlr != m_vProfiles.end(); l_itPlr++) {
                if (*l_itName == (*l_itPlr).m_sName) {
                  controller::CControllerGame l_cCtrl;
                  l_cCtrl.deserialize((*l_itPlr).m_sControls);

                  if (l_cCtrl.usesJoystick()) {
                    l_cCtrl.resetJoystick();
                    (*l_itPlr).m_sControls = l_cCtrl.serialize();
                    m_vAssignJoystick.push_back(*l_itName);

                    m_pSelectCtrl->setVisible(true);
                    m_iAssigned = -1;

                    std::wstring s = L"Player " + helpers::s2ws(*m_vAssignJoystick.begin()) + L": Select your gamepad by clicking a button.";
                    m_pSelectName->setText(s.c_str());
                  }
                }
              }
            }
          }

          std::string l_sGameSetup[] = {
            "label_gridpos",
            "starting_positions",
            "label_reverse_grid",
            "reverse_grid",
            "label_randomize_first",
            "randomize_first",
            "label_fillgrid_ai",
            "fillgrid_ai",
            "label_raceclass",
            "raceclass",
            "label_gridsize",
            "gridsize",
            "label_race_finish",
            "race_finish",
            ""
          };

          for (int i = 0; l_sGameSetup[i] != ""; i++) {
            irr::gui::IGUIElement *p = findElementByName(l_sGameSetup[i], m_pGui->getRootGUIElement());
            if (p != nullptr)
              m_vSetupUI.push_back(p);
            else
              printf("%s not found.\n", l_sGameSetup[i].c_str());
          }

          checkAiElements();
          printf("Ready.\n");
        }

        ~CMenuSetupGame() {

        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller != nullptr ? a_cEvent.GUIEvent.Caller->getName() : "[null]";

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                bool l_bFillGridAI = false;
                int  l_iGridSize   = 1;
                irr::gui::IGUIElement *l_pRoot = m_pGui->getRootGUIElement();

                gui::CDustbinCheckbox *l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(findElementByNameAndType("fillgrid_ai", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pRoot));
                if (l_pCheckbox != nullptr && l_pCheckbox->isChecked())
                  l_bFillGridAI = true;

                gui::CSelector *l_pSelector = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("gridsize", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
                if (l_pSelector != nullptr)
                  l_iGridSize = std::wcstol(l_pSelector->getItem(l_pSelector->getSelected()).c_str(), nullptr, 10);

                messages::CSerializer64 l_cSerializer;

                for (std::vector<std::string>::iterator it = m_vSelectedPlayers.begin(); it != m_vSelectedPlayers.end(); it++)
                  l_cSerializer.addString(*it);

                m_pState->getGlobal()->setSetting("selectedplayers", l_cSerializer.getMessageAsString());
                m_pState->getGlobal()->setSetting("gamesetup"      , m_cSettings.serialize());

                // We fill a vector with the grid positions and ..
                std::vector<int> l_vGrid;

                for (int i = 0; i < (m_vSelectedPlayers.size() > l_iGridSize ? m_vSelectedPlayers.size() : l_iGridSize); i++)
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
                m_pState->getGlobal()->setGlobal("free_game_slots", l_cSlots.serialize());

                gui::CSelector *l_pNet = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("network_game", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                m_pManager->pushToMenuStack("menu_fillgrid");

                if (l_pNet != nullptr) {
                  if (l_pNet->getSelected() == 1) {
                    m_pManager->pushToMenuStack("menu_startserver");
                  }
                  else if (l_pNet->getSelected() == 2) {
                    // We don't want to go to the "fill grid" menu when connecting to a server
                    m_pManager->popMenuStack();

                    m_pManager->pushToMenuStack("menu_joinserver"  );
                    m_pManager->pushToMenuStack("menu_searchserver");
                  }
                }

                std::string l_sPlayers = l_cPlayers.serialize();
                std::string l_sNext    = m_pManager->popMenuStack();

                m_pState->getGlobal()->setGlobal("raceplayers", l_sPlayers);

                platform::saveSettings();

                createMenu(l_sNext, m_pDevice, m_pManager, m_pState);

                l_bRet = true;
              }
              else if (l_sSender == "cancel") {
                if (m_pSelectPlayer != nullptr && m_pSelectPlayer->isVisible()) {
                  playerSelectCancel();
                }
                else createMenu("menu_main", m_pDevice, m_pManager, m_pState);

                l_bRet = true;
              }
              else if (l_sSender == "close_addplayer") {
                playerSelectCancel();
                l_bRet = true;
              }
              else if (l_sSender == "add_player") {
                for (std::vector<std::tuple<irr::gui::IGUIStaticText *, irr::gui::IGUIStaticText *, gui::CMenuButton *, gui::CMenuButton *>>::iterator it = m_vPlayerUI.begin(); it != m_vPlayerUI.end(); it++) {
                  if (std::get<3>(*it) == a_cEvent.GUIEvent.Caller) {
                    m_itAdd = it;

                    if (m_pSelectPlayer != nullptr) {
                      m_pSelectPlayer->setVisible(true);

                      std::vector<std::string> l_vNames;

                      for (std::vector<data::SPlayerData >::iterator l_itPlr = m_vProfiles.begin(); l_itPlr != m_vProfiles.end(); l_itPlr++) {
                        l_vNames.push_back((*l_itPlr).m_sName);
                      }

                      for (std::vector<gui::CMenuButton*>::iterator l_itBtn = m_vSelectPlayer.begin(); l_itBtn != m_vSelectPlayer.end(); l_itBtn++) {
                        std::string l_sName = "";

                        for (std::vector<std::string>::iterator l_itName = l_vNames.begin(); l_itName != l_vNames.end(); l_itName++) {
                          if (std::find(m_vSelectedPlayers.begin(), m_vSelectedPlayers.end(), *l_itName) == m_vSelectedPlayers.end()) {
                            l_sName = *l_itName;
                            l_vNames.erase(l_itName);
                            break;
                          }
                        }

                        if (l_sName != "") {
                          (*l_itBtn)->setText(helpers::s2ws(l_sName).c_str());
                          (*l_itBtn)->setVisible(true);
                        }
                        else {
                          (*l_itBtn)->setVisible(false);
                        }
                      }
                    }

                    break;
                  }
                }
                l_bRet = true;
              }
              else if (l_sSender == "CancelAssignJoystick") {
                if (m_vAssignJoystick.size() > 0 && m_pSelectCtrl != nullptr && m_pSelectName != nullptr) {
                  std::string l_sName = (*m_vAssignJoystick.begin());

                  printf("==> %s\n", l_sName.c_str());
                  m_vAssignJoystick.erase(m_vAssignJoystick.begin());

                  for (std::vector<std::string>::iterator l_itSelected = m_vSelectedPlayers.begin(); l_itSelected != m_vSelectedPlayers.end(); l_itSelected++) {
                    if (*l_itSelected == l_sName) {
                      m_vSelectedPlayers.erase(l_itSelected);
                      updateSelectedPlayers();
                      checkAiElements();
                      break;
                    }
                  }

                  if (m_vAssignJoystick.size() > 0) {
                    std::wstring s = L"Player " + helpers::s2ws(*m_vAssignJoystick.begin()) + L": Select your gamepad by clicking a button.";
                    m_pSelectName->setText(s.c_str());
                  }
                  else {
                    m_pSelectCtrl->setVisible(false);
                  }
                }
              } 
              else {
                for (std::vector<gui::CMenuButton *>::iterator it = m_vSelectPlayer.begin(); it != m_vSelectPlayer.end(); it++) {
                  if (*it == a_cEvent.GUIEvent.Caller && a_cEvent.GUIEvent.Caller->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId) {
                    std::string l_sName = helpers::ws2s(reinterpret_cast<gui::CMenuButton *>(a_cEvent.GUIEvent.Caller)->getText());
                    m_vSelectedPlayers.push_back(l_sName);
                    updateSelectedPlayers();
                    checkAiElements();

                    for (std::vector<data::SPlayerData>::iterator l_itPlr = m_vProfiles.begin(); l_itPlr != m_vProfiles.end(); l_itPlr++) {
                      controller::CControllerGame l_cCtrl;
                      l_cCtrl.deserialize((*l_itPlr).m_sControls);

                      if (l_cCtrl.usesJoystick() && !l_cCtrl.isJoystickAssigned() && m_pSelectCtrl != nullptr && m_pSelectName != nullptr) {
                        (*l_itPlr).m_sControls = l_cCtrl.serialize();

                        m_pSelectCtrl->setVisible(true);
                        m_vAssignJoystick.push_back((*l_itPlr).m_sName);
                      }
                    }

                    if (m_pSelectPlayer != nullptr)
                      m_pSelectPlayer->setVisible(false);

                    l_bRet = true;
                  }
                }

                if (!l_bRet) {
                  if (a_cEvent.GUIEvent.Caller->getType() == (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId) {
                    int l_iNum = 0;
                    for (std::vector<std::tuple<irr::gui::IGUIStaticText*, irr::gui::IGUIStaticText*, gui::CMenuButton*, gui::CMenuButton *>>::iterator it = m_vPlayerUI.begin(); it != m_vPlayerUI.end(); it++) {
                      if (a_cEvent.GUIEvent.Caller == std::get<2>(*it)) {
                        if (l_iNum < m_vSelectedPlayers.size()) {
                          std::string l_sName = *(m_vSelectedPlayers.begin() + l_iNum);

                          for (std::vector<data::SPlayerData>::iterator l_itPlr = m_vProfiles.begin(); l_itPlr != m_vProfiles.end(); l_itPlr++) {
                            if ((*l_itPlr).m_sName == l_sName) {
                              controller::CControllerGame l_cCtrl;
                              l_cCtrl.deserialize((*l_itPlr).m_sControls);

                              if (l_cCtrl.usesJoystick()) {
                                printf("Unassign joystick for player %s (%i)\n", l_sName.c_str(), l_cCtrl.getJoystickIndex());

                                for (std::vector<irr::u8>::iterator l_itIndex = m_vUsedJoysticks.begin(); l_itIndex != m_vUsedJoysticks.end(); l_itIndex++) {
                                  if (*l_itIndex == (irr::u8)l_cCtrl.getJoystickIndex()) {
                                    printf("Remove joystick #%i from list of assigned joysticks.\n", l_cCtrl.getJoystickIndex());
                                    m_vUsedJoysticks.erase(l_itIndex);
                                    break;
                                  }
                                }

                                l_cCtrl.resetJoystick();
                                (*l_itPlr).m_sControls = l_cCtrl.serialize();
                              }
                            }
                          }

                          m_vSelectedPlayers.erase(m_vSelectedPlayers.begin() + l_iNum);
                          updateSelectedPlayers();
                          checkAiElements();
                        }
                        break;
                      }
                      l_iNum++;
                    }
                  }
                }

                if (!l_bRet) printf("Button \"%s\" clicked.\n", l_sSender.c_str());
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
              if (a_cEvent.GUIEvent.Caller->getType() == gui::g_DustbinCheckboxId) {
                gui::CDustbinCheckbox *p = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller);

                if (l_sSender == "reverse_grid")
                  m_cSettings.m_bReverseGrid = p->isChecked();
                else if (l_sSender == "randomize_first")
                  m_cSettings.m_bRandomFirstRace = p->isChecked();
                else if (l_sSender == "fillgrid_ai") {
                  m_cSettings.m_bFillGridAI = p->isChecked();

                  checkAiElements();
                  l_bRet = true;
                }
                else printf("Unknown checkbox: \"%s\"\n", l_sSender.c_str());
              }
              else printf("Checkbox is not Dustbin!\n");
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
              if (a_cEvent.GUIEvent.Caller->getType() == gui::g_SelectorId) {
                gui::CSelector *p = reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller);

                if (l_sSender == "gridsize") {
                  m_cSettings.m_iGridSize = p->getSelected();
                  checkAiElements();
                  l_bRet = true;
                }
                else if (l_sSender == "race_finish") {
                  m_cSettings.m_iAutoFinish = p->getSelected();
                  l_bRet = true;
                }
                else if (l_sSender == "raceclass") {
                  m_cSettings.m_iRaceClass = p->getSelected();
                  l_bRet = true;
                }
                else if (l_sSender == "starting_positions") {
                  m_cSettings.m_iGridPos = p->getSelected();
                  l_bRet = true;
                }
                else if (l_sSender == "touchcontrol") {
                  // If a touch controller was selected we need to reset the "menu pad" flag
                  CGlobal::getInstance()->getSettingData().m_bMenuPad = false;
                }
                else if (l_sSender == "network_game") {
                  gui::CSelector *l_pNet = reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller);
                  bool l_bVisible = l_pNet->getSelectedItem() != L"Join Server";

                  for (std::vector<irr::gui::IGUIElement*>::iterator l_itElement = m_vSetupUI.begin(); l_itElement != m_vSetupUI.end(); l_itElement++) {
                    (*l_itElement)->setVisible(l_bVisible);
                  }
                }
                else printf("Unkown selector \"%s\"\n", l_sSender.c_str());
              }
              else printf("Scrollbar is not a selector!\n");
            }
          }
          else if (a_cEvent.EventType == irr::EET_JOYSTICK_INPUT_EVENT) {
            if (m_iAssigned != -1) {
              if (a_cEvent.JoystickEvent.Joystick == m_iAssigned && a_cEvent.JoystickEvent.ButtonStates == 0) {
                m_iAssigned = -1;
              }
            }
            else {
              if (m_pSelectCtrl != nullptr && m_pSelectCtrl->isVisible() && m_vAssignJoystick.size() > 0) {
                if (a_cEvent.JoystickEvent.ButtonStates != 0) {
                  m_iAssigned = a_cEvent.JoystickEvent.Joystick;
                  printf("Player %s has selected joystick #%i.\n", (*m_vAssignJoystick.begin()).c_str(), a_cEvent.JoystickEvent.Joystick);

                  for (std::vector<data::SPlayerData>::iterator l_itPlayer = m_vProfiles.begin(); l_itPlayer != m_vProfiles.end(); l_itPlayer++) {
                    if ((*l_itPlayer).m_sName == *m_vAssignJoystick.begin()) {
                      bool l_bOk = true;

                      for (std::vector<irr::u8>::iterator l_itIndex = m_vUsedJoysticks.begin(); l_itIndex != m_vUsedJoysticks.end(); l_itIndex++) {
                        if (*l_itIndex == a_cEvent.JoystickEvent.Joystick) {
                          m_pSelectName->setText(L"This gamepad has already been assigned. Please select another controller.");
                          l_bOk = false;
                          break;
                        }
                      }

                      if (l_bOk) {
                        controller::CControllerGame l_cCtrl;
                        l_cCtrl.deserialize((*l_itPlayer).m_sControls);
                        l_cCtrl.setJoystickIndices(a_cEvent.JoystickEvent.Joystick);
                        (*l_itPlayer).m_sControls = l_cCtrl.serialize();

                        m_vAssignJoystick.erase(m_vAssignJoystick.begin());
                        m_vUsedJoysticks.push_back(a_cEvent.JoystickEvent.Joystick);

                        if (m_vAssignJoystick.size() > 0) {
                          std::wstring s = L"Player " + helpers::s2ws(*m_vAssignJoystick.begin()) + L": Select your gamepad by clicking a button.";
                          m_pSelectName->setText(s.c_str());
                        }
                        else {
                          m_pSelectCtrl->setVisible(false);
                        }
                      }
                      break;
                    }
                  }
                }
              }
            }
          }

          return l_bRet;
        }
    };

    IMenuHandler* createMenuSetupGame(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSetupGame(a_pDevice, a_pManager, a_pState);
    }
  }
}