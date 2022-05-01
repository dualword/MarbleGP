// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CMenuLoader.h>
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
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_main.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pManager->clearMenuStack();

          m_pState->setZLayer(0);
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT && a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
            std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

            if (l_sButton == "settings") {
              createMenu("menu_settings", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "profiles") {
              createMenu("menu_profiles", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "free_racing") {
              std::vector<data::SPlayerData> l_vProfiles = data::SPlayerData::createPlayerVector(m_pState->getGlobal()->getSetting("profiles"));
              m_pManager->pushToMenuStack("menu_selecttrack");

              if (l_vProfiles.size() == 0) {
                m_pManager->pushToMenuStack("menu_setupgame");
                m_pManager->pushToMenuStack("menu_profiles");
                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
              }
              else createMenu("menu_setupgame", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "view_track") {
              data::SRacePlayers l_cPlayers;

              for (int i = 0; i < (m_pState->getGlobal()->getSetting("show_ai_data") == "1" ? 1 : 16); i++) {
                data::SPlayerData l_cData;
                l_cData.m_eType     = data::enPlayerType::Ai;
                l_cData.m_iGridPos  = i;
                l_cData.m_iPlayerId = i + 1;
                l_cData.m_sName     = "AI Demo Player #" + std::to_string(i + 1);
                l_cData.m_sControls = "ai_player";
                l_cData.m_eAiHelp   = data::SPlayerData::enAiHelp::Off;
                l_cData.m_sTexture  = "default://number=" + std::to_string(i + 1);

                l_cPlayers.m_vPlayers.push_back(l_cData);
              }

              m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "credits") {
              m_pState->getGlobal()->setGlobal("championship", "-Kb;-Lb;c;-Nb;b;-Mb;q;-Ob;a;-Pb;-Qb;q;-Rb;-8%3b-9%3bb%3b-%2b%3bBrainsaw%3b-%3d%3bq%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bz%3b-bb%3bb%3b-cb%3ba%3b-db%3bb%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bc%3b-%2b%3bAI%2520Demo%2520Player%2520%25231%3b-%3d%3bq%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bl%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bd%3b-%2b%3bAI%2520Demo%2520Player%2520%25236%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3be%3b-bb%3bc%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3be%3b-%2b%3bAI%2520Demo%2520Player%2520%25238%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3b-%3d%3b-ab%3bd%3b-bb%3bc%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bf%3b-%2b%3bAI%2520Demo%2520Player%2520%252316%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3b-%3d%3b-ab%3bb%3b-bb%3bd%3b-cb%3bd%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bg%3b-%2b%3bAI%2520Demo%2520Player%2520%252311%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bh%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bh%3b-%2b%3bAI%2520Demo%2520Player%2520%25237%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bf%3b-bb%3bb%3b-cb%3bb%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bi%3b-%2b%3bAI%2520Demo%2520Player%2520%252315%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bg%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bj%3b-%2b%3bAI%2520Demo%2520Player%2520%252314%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bk%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bk%3b-%2b%3bAI%2520Demo%2520Player%2520%25234%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3b-%3d%3b-ab%3ba%3b-bb%3be%3b-cb%3bd%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bl%3b-%2b%3bAI%2520Demo%2520Player%2520%25233%3b-%3d%3bq%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bq%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bm%3b-%2b%3bAI%2520Demo%2520Player%2520%25235%3b-%3d%3bq%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bn%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bn%3b-%2b%3bAI%2520Demo%2520Player%2520%252312%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bi%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bo%3b-%2b%3bAI%2520Demo%2520Player%2520%25232%3b-%3d%3bq%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bu%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bp%3b-%2b%3bAI%2520Demo%2520Player%2520%252313%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3b-%3d%3b-ab%3bc%3b-bb%3bc%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Rb;-8%3b-9%3bq%3b-%2b%3bAI%2520Demo%2520Player%2520%25239%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bj%3b-bb%3bb%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb;-Sb;-Tb;-Ub;b;-Vb;-gb%3b-jb%3bthe_gates%3b-ib%3bh%3b-hb%3bq%3b-kb%3b-lb%3b-4b%253b-5b%253bzCc%253b-6b%253bb%253b-7b%253ba%253b-8b%253b6nb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253brCc%253b-6b%253bc%253b-7b%253b8qb%253b-8b%253bzBb%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bsCc%253b-6b%253bd%253b-7b%253btvb%253b-8b%253bXCb%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253buCc%253b-6b%253be%253b-7b%253b%253dxb%253b-8b%253bwBb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bqCc%253b-6b%253bf%253b-7b%253beMb%253b-8b%253blCb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bDCc%253b-6b%253bg%253b-7b%253bWNb%253b-8b%253bjDb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253byCc%253b-6b%253bh%253b-7b%253bpOb%253b-8b%253bECb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bBCc%253b-6b%253bi%253b-7b%253bCRb%253b-8b%253b4Cb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bACc%253b-6b%253bj%253b-7b%253bKSb%253b-8b%253bbDb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bECc%253b-6b%253bk%253b-7b%253b1Vb%253b-8b%253b6Cb%253b-9b%253ba%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bwCc%253b-6b%253bl%253b-7b%253bVXb%253b-8b%253bGCb%253b-9b%253bb%253b-%252bb%253bb%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bvCc%253b-6b%253bm%253b-7b%253b8Yb%253b-8b%253bhCb%253b-9b%253ba%253b-%252bb%253bc%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bxCc%253b-6b%253bn%253b-7b%253bN1b%253b-8b%253bYBb%253b-9b%253ba%253b-%252bb%253bc%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bCCc%253b-6b%253bo%253b-7b%253b-b%253b-8b%253b9Ab%253b-9b%253ba%253b-%252bb%253bc%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253bFCc%253b-6b%253bp%253b-7b%253b-b%253b-8b%253beCb%253b-9b%253bd%253b-%252bb%253bd%253b-%253db%253b-b%253ba%253b-ac%3b-lb%3b-4b%253b-5b%253btCc%253b-6b%253bq%253b-7b%253b-b%253b-8b%253b%252bBb%253b-9b%253bd%253b-%252bb%253be%253b-%253db%253b-b%253ba%253b-ac%3b-mb%3b-nb%3bqCc%3bc%3brCc%3bo%3bsCc%3bl%3btCc%3bk%3buCc%3bm%3bvCc%3bd%3bwCc%3bh%3bxCc%3be%3byCc%3bq%3bzCc%3bb%3bACc%3bg%3bBCc%3bn%3bCCc%3bp%3bDCc%3bj%3bECc%3bi%3bFCc%3bf%3b-ob%3b-pb;-Wb");
              createMenu("menu_standings", m_pDevice, m_pManager, m_pState);
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