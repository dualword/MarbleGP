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
              CGlobal::getInstance()->setGlobal("profiles_old", CGlobal::getInstance()->getSetting("profiles"));
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
              /*data::SRacePlayers l_cPlayers;

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
              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);*/

              m_pState->getGlobal()->setGlobal("championship", "-Kb;-Lb;f;-Nb;d;-Mb;q;-Ob;b;-Pb;-Qb;q;-Rb;-8%3b-9%3bb%3b-%2b%3bLatericus%2520Solutus%3b-%3d%3bq%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bz%3b-bb%3ba%3b-cb%3ba%3b-db%3bb%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bc%3b-%2b%3bFlavius%2520Josephus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3b-%3d%3b-ab%3bc%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bd%3b-%2b%3bCornelius%2520Maluginensis%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bl%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3be%3b-%2b%3bRanius%2520Festus%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bk%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bf%3b-%2b%3bCassius%2520Dio%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bn%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bg%3b-%2b%3bUlpius%2520Traianus%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bg%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bh%3b-%2b%3bTitus%2520Vibius%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3b-%3d%3b-ab%3ba%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bi%3b-%2b%3bClaudius%2520Caecus%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bi%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bj%3b-%2b%3bPomponius%2520Silvanus%257cmarblegp%3b-%3d%3bq%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bu%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bk%3b-%2b%3bLucius%2520Lentulus%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bf%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bl%3b-%2b%3bPlinius%2520Secundus%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bh%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bm%3b-%2b%3bQuintus%2520Amatianus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3b-%3d%3b-ab%3bd%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bn%3b-%2b%3bIunius%2520Caepio%257cmarblegp%3b-%3d%3bq%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bq%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bo%3b-%2b%3bMarcus%2520Nerva%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bj%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bp%3b-%2b%3bBarbatus%2520Scipio%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3b-%3d%3b-ab%3bb%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bq%3b-%2b%3bAemilius%2520Lepidus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3be%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Sb;-Tb;-Ub;b;-Vb;-qb%3b-tb%3boval%3b-sb%3bb%3b-rb%3bq%3b-ub%3b-vb%3b-4b%253b-5b%253bCCc%253b-6b%253bb%253b-7b%253ba%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bXq%253bXx%253bTF%253bwM%253bnU%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bsCc%253b-6b%253bc%253b-7b%253bbc%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bro%253bKw%253bwF%253bbN%253boW%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253btCc%253b-6b%253bd%253b-7b%253b1c%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253boo%253bSw%253bGG%253b3N%253bcX%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bqCc%253b-6b%253be%253b-7b%253bpd%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bxn%253bnx%253b5G%253brO%253bCX%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bwCc%253b-6b%253bf%253b-7b%253bQd%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bZp%253bky%253bpH%253bPO%253b3X%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253byCc%253b-6b%253bg%253b-7b%253bae%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253baq%253bvy%253bIH%253b5O%253bnY%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253brCc%253b-6b%253bh%253b-7b%253bIf%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bQn%253bDx%253bTH%253bVP%253bVZ%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bACc%253b-6b%253bi%253b-7b%253b4g%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bfr%253bSz%253bnJ%253beR%253bf1%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bECc%253b-6b%253bj%253b-7b%253bth%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b4r%253bFz%253bkJ%253bSR%253bG1%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bBCc%253b-6b%253bk%253b-7b%253bWh%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b2q%253bxz%253b7J%253b%252bR%253b91%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bDCc%253b-6b%253bl%253b-7b%253bki%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bpr%253boA%253btK%253bwS%253bx2%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bzCc%253b-6b%253bm%253b-7b%253bHk%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bSq%253bfA%253bUK%253bOT%253bU4%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bvCc%253b-6b%253bn%253b-7b%253bBl%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bMp%253b2z%253bvL%253bzU%253bO5%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bxCc%253b-6b%253bo%253b-7b%253b5l%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253buq%253bEA%253b0L%253b%253dU%253bg6%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253buCc%253b-6b%253bp%253b-7b%253bqm%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bPp%253bqA%253bLL%253bZU%253bD6%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bFCc%253b-6b%253bq%253b-7b%253bvm%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bns%253byB%253bzM%253bxV%253bI6%253b-cc%253b-mc%253b-nc%3b-wb%3b-xb%3bqCc%3bf%3brCc%3bo%3bsCc%3bj%3btCc%3bn%3buCc%3bp%3bvCc%3bm%3bwCc%3bd%3bxCc%3bc%3byCc%3be%3bzCc%3bq%3bACc%3bi%3bBCc%3bg%3bCCc%3bb%3bDCc%3bk%3bECc%3bl%3bFCc%3bh%3b-yb%3b-zb;-Wb");
              createMenu("menu_standings", m_pDevice, m_pManager, m_pState);
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