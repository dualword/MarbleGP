// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuMain
    * @author Christian Keimel
    * The handler for the main menu
    */
    class CMenuMain : public IMenuHandler {
      public:
        CMenuMain(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_main.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pManager->clearMenuStack();

          m_pState->getGlobal()->stopGameServer();
          m_pState->getGlobal()->stopGameClient();

          std::vector<data::SPlayerData> l_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));

          for (std::vector<data::SPlayerData>::iterator l_itPlayer = l_vProfiles.begin(); l_itPlayer != l_vProfiles.end(); l_itPlayer++) {
            printf("\n%s\n", (*l_itPlayer).m_sControls.c_str());
          }

          platform::consumeBackEvent(false);
        }

        virtual ~CMenuMain() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

            if (l_sButton == "settings") {
              platform::consumeBackEvent(true);
              createMenu("menu_settings", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "profiles") {
              platform::consumeBackEvent(true);
              createMenu("menu_profiles", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "free_racing") {
              std::vector<data::SPlayerData> l_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));

              if (l_vProfiles.size() == 0) {
                CGlobal::getInstance()->setGlobal("edit_profileno", "-1");
                CGlobal::getInstance()->setGlobal("edit_profile", "");

                m_pManager->pushToMenuStack("menu_selecttrack");
                m_pManager->pushToMenuStack("menu_setupgame");
                platform::consumeBackEvent(true);
                createMenu("menu_profilewizard", m_pDevice, m_pManager, m_pState);
              }
              else {
                m_pManager->pushToMenuStack("menu_selecttrack");
                platform::consumeBackEvent(true);
                createMenu("menu_setupgame", m_pDevice, m_pManager, m_pState);
              }
            }
            else if (l_sButton == "view_track") {
              data::SRacePlayers l_cPlayers;

              for (int i = 0; i < (m_pState->getGlobal()->getSetting("show_ai_data") == "1" ? 1 : 16); i++) {
                data::SPlayerData l_cData;
                l_cData.m_eType      = data::enPlayerType::Ai;
                l_cData.m_iGridPos   = i;
                l_cData.m_iPlayerId  = i + 1;
                l_cData.m_sName      = "AI Demo Player #" + std::to_string(i + 1);
                l_cData.m_eAiHelp    = data::SPlayerData::enAiHelp::Off;
                l_cData.m_sTexture   = "default://number=" + std::to_string(i + 1);
                l_cData.m_sShortName = "Ai#" + std::to_string(i + 1);

                switch (i % 3) {
                  case 0 : l_cData.m_sControls = "class=marblegp"; l_cData.m_sName = "AI MarbleGP #" + std::to_string(l_cData.m_iGridPos + 1); break;
                  case 2 : l_cData.m_sControls = "class=marble3" ; l_cData.m_sName = "AI Marble3 #"  + std::to_string(l_cData.m_iGridPos + 1); break;
                  case 1 : l_cData.m_sControls = "class=marble2" ; l_cData.m_sName = "AI Marble2 #"  + std::to_string(l_cData.m_iGridPos + 1); break;
                }

                l_cPlayers.m_vPlayers.push_back(l_cData);
              }

              m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

              platform::consumeBackEvent(true);
              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "cup") {
#ifdef _DEBUG
              platform::consumeBackEvent(true);
              createMenu("menu_selectcup", m_pDevice, m_pManager, m_pState);
#else
              m_pState->getGlobal()->setGlobal("message_headline", "To be Implemented");
              m_pState->getGlobal()->setGlobal("message_text"    , "MarbleGP Cup is not yet implemented");
              platform::consumeBackEvent(true);
              createMenu("menu_message", m_pDevice, m_pManager, m_pState);
#endif
            }
            else if (l_sButton == "credits") {
#ifdef _DEBUG
              data::SRacePlayers l_cPlayers;

              for (int i = 0; i < (m_pState->getGlobal()->getSetting("show_ai_data") == "1" ? 1 : 16); i++) {
                data::SPlayerData l_cData;
                l_cData.m_eType      = data::enPlayerType::Ai;
                l_cData.m_iGridPos   = i;
                l_cData.m_iPlayerId  = i + 1;
                l_cData.m_sName      = "AI Demo Player #" + std::to_string(i + 1);
                l_cData.m_eAiHelp    = data::SPlayerData::enAiHelp::Off;
                l_cData.m_sTexture   = "default://number=" + std::to_string(i + 1) + "&class=" + std::to_string(i % 3);
                l_cData.m_sShortName = "Ai#" + std::to_string(i + 1);

                switch (i % 3) {
                  case 0 : l_cData.m_sControls = "class=marblegp"; l_cData.m_sName = "AI MarbleGP #" + std::to_string(l_cData.m_iGridPos + 1); break;
                  case 2 : l_cData.m_sControls = "class=marble3" ; l_cData.m_sName = "AI Marble3 #"  + std::to_string(l_cData.m_iGridPos + 1); break;
                  case 1 : l_cData.m_sControls = "class=marble2" ; l_cData.m_sName = "AI Marble2 #"  + std::to_string(l_cData.m_iGridPos + 1); break;
                }

                l_cPlayers.m_vPlayers.push_back(l_cData);
              }

              m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

              platform::consumeBackEvent(true);
              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);
#else
              platform::consumeBackEvent(true);
              createMenu("menu_credits", m_pDevice, m_pManager, m_pState);
#endif
            }
            else if (l_sButton == "race_replay") {
              m_pState->getGlobal()->setGlobal("message_headline", "To be Implemented");
              m_pState->getGlobal()->setGlobal("message_text"    , "Race Replay is not yet implemented");
              platform::consumeBackEvent(true);
              createMenu("menu_message", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "exit") {
              m_pState->setState(state::enState::Quit);
            }
            else printf("Button clicked (%s, %i, CMenuMain).\n", l_sButton.c_str(), a_cEvent.GUIEvent.Caller->getID());
          }

          return l_bRet;
        }
    };



    IMenuHandler* createMenuMain(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuMain(a_pDevice, a_pManager, a_pState);
    }
  }
}