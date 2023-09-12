// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <algorithm>
#include <random>

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
              data::SRacePlayers l_cPlayers;

              // Tuple: "Name", "Short Name", "Texture Index", "Preferred Class One", "Preferred class Two"
              std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> l_vAiPlayers;

              helpers::loadAiProfiles(l_vAiPlayers);

              std::random_device l_cRd { };
              std::default_random_engine l_cRe { l_cRd() };
              std::shuffle(l_vAiPlayers.begin(), l_vAiPlayers.end(), l_cRe);

              int l_iGridPos = 0;

              for (auto l_itAi : l_vAiPlayers) {
                int l_iAiClass = l_iGridPos % 3;

                std::string l_sAiClass = l_iAiClass == 0 ? "marblegp" : l_iAiClass == 1 ? "marble2" : "marble3";

                data::SPlayerData l_cData;
                l_cData.m_eType      = data::enPlayerType::Ai;
                l_cData.m_iGridPos   = l_iGridPos;
                l_cData.m_iPlayerId  = l_iGridPos + 1;
                l_cData.m_sName      = std::get<0>(l_itAi) + "|" + l_sAiClass;
                l_cData.m_eAiHelp    = data::SPlayerData::enAiHelp::Off;
                l_cData.m_sShortName = std::get<1>(l_itAi);
                l_cData.m_fDeviation = std::get<5>(l_itAi) / 100.0f;
                l_cData.m_sControls  = "class=" + l_sAiClass;
                l_cData.m_sTexture   = helpers::createDefaultTextureString(std::get<2>(l_itAi), l_iAiClass) + "&number=" + std::to_string(l_cData.m_iGridPos + 1);

                l_cPlayers.m_vPlayers.push_back(l_cData);

                l_iGridPos++;
              }


              m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

              platform::consumeBackEvent(true);
              createMenu("menu_selecttrack", m_pDevice, m_pManager, m_pState);

              // m_pState->getGlobal()->setGlobal("championship", "-Kb;-Lb;f;-Nb;b;-Mb;q;-Ob;b;-Pb;-Qb;q;-Rb;-8%3b-9%3bb%3b-%2b%3bBrainsaw%3b-%3d%3bq%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bz%3b-bb%3ba%3b-cb%3ba%3b-db%3bb%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bc%3b-%2b%3bCommodus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3b-%3d%3b-ab%3bb%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bd%3b-%2b%3bCaracalla%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bl%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3be%3b-%2b%3bTitus%2520Domitianus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3b-%3d%3b-ab%3ba%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bf%3b-%2b%3bTiberius%2520Claudius%257cmarblegp%3b-%3d%3bq%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bu%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bg%3b-%2b%3bSeptimius%2520Severus%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3b-%3d%3b-ab%3bc%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bh%3b-%2b%3bFlavius%2520Josephus%257cmarblegp%3b-%3d%3bq%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bq%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bi%3b-%2b%3bVespasianus%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bg%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bj%3b-%2b%3bQuintus%2520Scipio%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bi%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bk%3b-%2b%3bGaius%2520Octavius%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bj%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bl%3b-%2b%3bPorcius%2520Cato%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3be%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bm%3b-%2b%3bNero%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bh%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bn%3b-%2b%3bCassius%2520Dio%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bk%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bo%3b-%2b%3bCaligula%257cmarble2%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bf%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bp%3b-%2b%3bPertinax%257cmarble3%3b-%3d%3bq%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3b-%3d%3b-ab%3bd%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Rb;-8%3b-9%3bq%3b-%2b%3bMarcus%2520Aurelius%257cmarblegp%3b-%3d%3bq%3ba%3ba%3ba%3bb%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3ba%3b-%3d%3b-ab%3bn%3b-bb%3ba%3b-cb%3ba%3b-db%3ba%3b-eb%3ba%3b-fb%3bb%3b-gb;-Sb;-Tb;-Ub;b;-Vb;-qb%3b-tb%3boval%3b-sb%3bb%3b-rb%3bq%3b-ub%3b-vb%3b-4b%253b-5b%253bqCc%253b-6b%253bb%253b-7b%253ba%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bkn%253bkw%253bEE%253bEL%253b9T%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253btCc%253b-6b%253bc%253b-7b%253bhc%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253boo%253bVw%253b1F%253b7M%253beW%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253brCc%253b-6b%253bd%253b-7b%253b0c%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bCn%253bHw%253bSF%253bGN%253bXW%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bvCc%253b-6b%253be%253b-7b%253bxd%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b6o%253bqx%253bHG%253b%253dN%253buX%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bACc%253b-6b%253bf%253b-7b%253b6e%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b1q%253bZy%253beI%253bIP%253b3Y%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bFCc%253b-6b%253bg%253b-7b%253bXf%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bwr%253b2z%253bfJ%253byQ%253bUZ%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bDCc%253b-6b%253bh%253b-7b%253bXg%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bnr%253bFz%253b9I%253bZQ%253bU0%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253buCc%253b-6b%253bi%253b-7b%253bnh%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bFp%253b7y%253b0I%253bmR%253bk1%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bxCc%253b-6b%253bj%253b-7b%253b0h%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b1p%253bqz%253bRJ%253bUR%253bX1%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bCCc%253b-6b%253bk%253b-7b%253bhi%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bIr%253bhA%253beK%253bgS%253be2%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bECc%253b-6b%253bl%253b-7b%253bSi%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b6r%253b7A%253bXK%253bWS%253bP2%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bzCc%253b-6b%253bm%253b-7b%253bUk%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bVq%253bZA%253bqL%253bPT%253bR4%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bwCc%253b-6b%253bn%253b-7b%253bpl%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bJq%253bEA%253bNL%253bPU%253bm5%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bsCc%253b-6b%253bo%253b-7b%253bTl%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bfp%253bPz%253b2K%253bmU%253bQ5%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253byCc%253b-6b%253bp%253b-7b%253b1l%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253b2q%253bNA%253bdM%253bdV%253bY5%253b-cc%253b-mc%253b-nc%3b-vb%3b-4b%253b-5b%253bBCc%253b-6b%253bq%253b-7b%253bGm%253b-8b%253ba%253b-9b%253ba%253b-%252bb%253ba%253b-%253db%253b-b%253ba%253b-ac%253b-bc%253bfr%253b3z%253bjL%253bIU%253bD6%253b-cc%253b-mc%253b-nc%3b-wb%3b-xb%3bqCc%3bb%3brCc%3bh%3bsCc%3bg%3btCc%3bf%3buCc%3bj%3bvCc%3bq%3bwCc%3bp%3bxCc%3bm%3byCc%3bc%3bzCc%3bl%3bACc%3bd%3bBCc%3be%3bCCc%3bi%3bDCc%3bk%3bECc%3bo%3bFCc%3bn%3b-yb%3b-zb;-Wb");
              // m_pState->getGlobal()->setGlobal("raceplayers", "Y;Z;x%3bPlayerProfile%3by%3ba%3bz%3bBrainsaw%3bB%3bDustbinController%253bcontrol%253bf%25253bl%25253bForward%25253bh%25253bb%25253bn%25253bM%25253br%25253ba%25253bt%25253ba%25253bx%25253bc%25253bD%25253b-b%25253bF%25253ba%253bcontrol%253bf%25253bl%25253bBackward%25253bh%25253bb%25253bn%25253bO%25253br%25253ba%25253bt%25253ba%25253bx%25253bc%25253bD%25253bb%25253bF%25253bqze%253bcontrol%253bf%25253bl%25253bLeft%25253bh%25253bb%25253bn%25253bL%25253br%25253ba%25253bt%25253ba%25253bx%25253ba%25253bD%25253b-b%25253bF%25253b4Lg%253bcontrol%253bf%25253bl%25253bRight%25253bh%25253bb%25253bn%25253bN%25253br%25253ba%25253bt%25253ba%25253bx%25253ba%25253bD%25253bb%25253bF%25253bOmc%253bcontrol%253bf%25253bl%25253bBrake%25253bh%25253bc%25253bn%25253bG%25253br%25253ba%25253bt%25253ba%25253bx%25253ba%25253bD%25253bb%25253bF%25253bmZm%253bcontrol%253bf%25253bl%25253bRearview%25253bh%25253bc%25253bn%25253bj%25253br%25253ba%25253bt%25253be%25253bx%25253ba%25253bD%25253bb%25253bF%25253bmZm%253bcontrol%253bf%25253bl%25253bRespawn%25253bh%25253bc%25253bn%25253bn%25253br%25253ba%25253bt%25253bf%25253bx%25253ba%25253bD%25253bb%25253bF%25253bmZm%253bcontrol%253bf%25253bl%25253bPause%25253bh%25253bc%25253bn%25253bt%25253br%25253ba%25253bt%25253bh%25253bx%25253ba%25253bD%25253bb%25253bF%25253bmZm%253bcontrol%253bf%25253bl%25253bCancel%25252520Race%25253bh%25253bc%25253bn%25253bB%25253br%25253ba%25253bt%25253bg%25253bx%25253ba%25253bD%25253bb%25253bF%25253bmZm%3bA%3bgenerate%253a%252f%252fnumberback%253dffffff%2526numberborder%253dffffff%2526numbercolor%253dff0000%2526pattern%253dtexture_rgbg%252epng%2526patternback%253dff0000%2526patterncolor%253dffffff%2526ringcolor%253dff7f7f%2526number%253d1%3bC%3ba%3bD%3ba%3bE%3bb%3bG%3bBrain%3bH%3ba%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bCommodus%257cmarble3%3bB%3bclass%253dmarble3%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d9932cc%2526numberborder%253d9932cc%2526ringcolor%253dda70d6%2526patterncolor%253d000000%2526patternback%253d9932cc%2526pattern%253dtexture_rookie%252epng%2526number%253d9%3bC%3ba%3bD%3bi%3bE%3b-b%3bG%3bComod%3bH%3bai%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bCaracalla%257cmarblegp%3bB%3bclass%253dmarblegp%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253dfd5320%2526numberborder%253dfd5320%2526ringcolor%253dba3b15%2526patterncolor%253d000000%2526patternback%253dfd5320%2526pattern%253dtexture_marbles2%252epng%2526number%253d11%3bC%3ba%3bD%3bk%3bE%3b-b%3bG%3bCarac%3bH%3b-QA%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bTitus%2520Domitianus%257cmarble3%3bB%3bclass%253dmarble3%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d3aec1e%2526numberborder%253d3aec1e%2526ringcolor%253d28ae13%2526patterncolor%253d000000%2526patternback%253d3aec1e%2526pattern%253dtexture_rookie%252epng%2526number%253d12%3bC%3ba%3bD%3bl%3bE%3b-b%3bG%3bDmiti%3bH%3b-yj%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bTiberius%2520Claudius%257cmarblegp%3bB%3bclass%253dmarblegp%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d4b64f9%2526numberborder%253d4b64f9%2526ringcolor%253d3548b7%2526patterncolor%253d000000%2526patternback%253d4b64f9%2526pattern%253dtexture_marbles2%252epng%2526number%253d4%3bC%3ba%3bD%3bd%3bE%3b-b%3bG%3bClaud%3bH%3bTz%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bSeptimius%2520Severus%257cmarble3%3bB%3bclass%253dmarble3%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d20b2aa%2526numberborder%253d20b2aa%2526ringcolor%253d7fffd4%2526patterncolor%253d000000%2526patternback%253d20b2aa%2526pattern%253dtexture_rookie%252epng%2526number%253d3%3bC%3ba%3bD%3bc%3bE%3b-b%3bG%3bSever%3bH%3b-aq%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bFlavius%2520Josephus%257cmarblegp%3bB%3bclass%253dmarblegp%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d00ffff%2526numberborder%253d00ffff%2526ringcolor%253d00bcbc%2526patterncolor%253d000000%2526patternback%253d00ffff%2526pattern%253dtexture_marbles2%252epng%2526number%253d2%3bC%3ba%3bD%3bb%3bE%3b-b%3bG%3bFlJos%3bH%3bd%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bVespasianus%257cmarble2%3bB%3bclass%253dmarble2%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253dffc0cb%2526numberborder%253dffc0cb%2526ringcolor%253dbc8d95%2526patterncolor%253d000000%2526patternback%253dffc0cb%2526pattern%253dtexture_marbles3%252epng%2526number%253d13%3bC%3ba%3bD%3bm%3bE%3b-b%3bG%3bVspia%3bH%3b-Se%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bQuintus%2520Scipio%257cmarble2%3bB%3bclass%253dmarble2%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253dffffff%2526numberborder%253dffffff%2526ringcolor%253ddddddd%2526patterncolor%253d000000%2526patternback%253dffffff%2526pattern%253dtexture_marbles3%252epng%2526number%253d5%3bC%3ba%3bD%3be%3bE%3b-b%3bG%3bQints%3bH%3b-Cl%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bGaius%2520Octavius%257cmarble2%3bB%3bclass%253dmarble2%3bA%3bgenerate%253a%252f%252fnumbercolor%253dffffff%2526numberback%253d000000%2526numberborder%253d000000%2526ringcolor%253dbbbbbb%2526patterncolor%253dffffff%2526patternback%253d000000%2526pattern%253dtexture_marbles3%252epng%2526number%253d14%3bC%3ba%3bD%3bn%3bE%3b-b%3bG%3bGaius%3bH%3blr%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bPorcius%2520Cato%257cmarble3%3bB%3bclass%253dmarble3%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253dffdab9%2526numberborder%253dffdab9%2526ringcolor%253db9906b%2526patterncolor%253d000000%2526patternback%253dffdab9%2526pattern%253dtexture_rookie%252epng%2526number%253d10%3bC%3ba%3bD%3bj%3bE%3b-b%3bG%3bCato%3bH%3blF%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bNero%257cmarble2%3bB%3bclass%253dmarble2%3bA%3bgenerate%253a%252f%252fnumbercolor%253dffffff%2526numberback%253d00008b%2526numberborder%253d00008b%2526ringcolor%253d0000ff%2526patterncolor%253dffffff%2526patternback%253d00008b%2526pattern%253dtexture_marbles3%252epng%2526number%253d8%3bC%3ba%3bD%3bh%3bE%3b-b%3bG%3bNero%3bH%3b-ov%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bCassius%2520Dio%257cmarblegp%3bB%3bclass%253dmarblegp%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253dc0c000%2526numberborder%253dc0c000%2526ringcolor%253d8d8d00%2526patterncolor%253d000000%2526patternback%253dc0c000%2526pattern%253dtexture_marbles2%252epng%2526number%253d16%3bC%3ba%3bD%3bp%3bE%3b-b%3bG%3bDio%2520%2520%3bH%3b-CK%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bCaligula%257cmarble2%3bB%3bclass%253dmarble2%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253ddaa520%2526numberborder%253ddaa520%2526ringcolor%253db8860b%2526patterncolor%253d000000%2526patternback%253ddaa520%2526pattern%253dtexture_marbles3%252epng%2526number%253d15%3bC%3ba%3bD%3bo%3bE%3b-b%3bG%3bCalgl%3bH%3b6e%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bPertinax%257cmarble3%3bB%3bclass%253dmarble3%3bA%3bgenerate%253a%252f%252fnumbercolor%253dffffff%2526numberback%253d6a0dad%2526numberborder%253d6a0dad%2526ringcolor%253dc1bbcc%2526patterncolor%253dffffff%2526patternback%253d6a0dad%2526pattern%253dtexture_rookie%252epng%2526number%253d7%3bC%3ba%3bD%3bg%3bE%3b-b%3bG%3bPertx%3bH%3bhn%3bI%3bb%3bF;0;Z;x%3bPlayerProfile%3by%3bb%3bz%3bMarcus%2520Aurelius%257cmarblegp%3bB%3bclass%253dmarblegp%3bA%3bgenerate%253a%252f%252fnumbercolor%253d000000%2526numberback%253d87cefa%2526numberborder%253d87cefa%2526ringcolor%253d8ebfdd%2526patterncolor%253d000000%2526patternback%253d87cefa%2526pattern%253dtexture_marbles2%252epng%2526number%253d6%3bC%3ba%3bD%3bf%3bE%3b-b%3bG%3bAurel%3bH%3b-Dy%3bI%3bb%3bF;0;1");
              // createMenu("menu_raceresult", m_pDevice, m_pManager, m_pState);
              // createMenu("menu_standings", m_pDevice, m_pManager, m_pState);

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