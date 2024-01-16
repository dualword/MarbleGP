// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <gui/CMenuBackground.h>
#include <network/CGameClient.h>
#include <helpers/CMenuLoader.h>
#include <gui/CMenuBackground.h>
#include <gameclasses/SPlayer.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuJoinServer
    * @author Christian Keimel
    * In this menu the player selects a server to connect to
    */
    class CMenuJoinServer : public IMenuHandler {
      private:
        threads::CInputQueue *m_pInputQueue;  /**< The input queue to receive messages from the server */
        network::CGameClient *m_pClient;      /**< The game client */

        irr::gui::IGUIStaticText *m_pConnecting;    /**< The "connecting" label */
        gui::CMenuBackground     *m_pMainFrame;     /**< The main frame */

        gameclasses::STournament *m_pTournament;  /**< The tournament */

        std::vector<std::tuple<gui::CMenuBackground *, irr::gui::IGUITab *, irr::gui::IGUIStaticText *, irr::gui::IGUIStaticText *>> m_vPlayers; /**< The root elements and the name text elements for the players */

        void updatePlayerList() {
          for (std::vector<std::tuple<gui::CMenuBackground*, irr::gui::IGUITab*, irr::gui::IGUIStaticText*, irr::gui::IGUIStaticText*>>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
            std::get<1>(*it)->setVisible(false);
          }

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

          std::sort(l_pTournament->m_vPlayers.begin(), l_pTournament->m_vPlayers.end(), [](gameclasses::SPlayer *l_pPlayer1, gameclasses::SPlayer *l_pPlayer2) {
            if (l_pPlayer1->m_sTexture.find("number=") != std::string::npos && l_pPlayer2->m_sTexture.find("number=") != std::string::npos) {
              std::string l_sType = "";

              std::map<std::string, std::string> l_mParams1 = helpers::parseParameters(l_sType, l_pPlayer1->m_sTexture);
              std::map<std::string, std::string> l_mParams2 = helpers::parseParameters(l_sType, l_pPlayer2->m_sTexture);

              int l_iNum1 = std::atoi(l_mParams1["number"].c_str());
              int l_iNum2 = std::atoi(l_mParams2["number"].c_str());

              return l_iNum1 < l_iNum2;
            }

            return l_pPlayer1->m_iPlayer < l_pPlayer2->m_iPlayer;
          });

          int i = 0;
          for (auto l_pPlayer : l_pTournament->m_vPlayers) {
            if (i < m_vPlayers.size()) {
              std::get<1>(m_vPlayers[i])->setVisible(true);
              std::get<2>(m_vPlayers[i])->setText(helpers::s2ws(l_pPlayer->m_sName).c_str());

              printf("Player \"%s\" (%i): \n", l_pPlayer->m_sName.c_str(), l_pPlayer->m_iPlayer);

              if (l_pPlayer->m_sTexture.find("number=") != std::string::npos) {
                std::string l_sType = "";

                std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, l_pPlayer->m_sTexture);

                std::string l_sNumber = helpers::findTextureParameter(l_mParams, "number");
                std::string l_sBack   = helpers::findTextureParameter(l_mParams, "numberback");
                std::string l_sColor  = helpers::findTextureParameter(l_mParams, "numbercolor");

                if (l_sBack   != "" && l_sColor != "" && l_sNumber != "") {
                  std::get<3>(m_vPlayers[i])->setText(helpers::s2ws(l_sNumber).c_str());

                  irr::video::SColor l_cColor;
                  helpers::fillColorFromString(l_cColor, l_sBack);
                  std::get<3>(m_vPlayers[i])->setBackgroundColor(l_cColor);

                  helpers::fillColorFromString(l_cColor, l_sColor);
                  std::get<3>(m_vPlayers[i])->setOverrideColor(l_cColor);

                  std::get<3>(m_vPlayers[i])->setVisible(true);
                }
              }
            }

            i++;
          }
        }

        void hideAiAndNumber(irr::gui::IGUIElement* a_pElement) {
          if (std::string("ai_class") == a_pElement->getName() || std::string("starting_number") == a_pElement->getName()) {
            a_pElement->setVisible(false);
          }

          for (irr::core::list<irr::gui::IGUIElement *>::ConstIterator l_itChild = a_pElement->getChildren().begin(); l_itChild != a_pElement->getChildren().end(); l_itChild++)
            hideAiAndNumber(*l_itChild);
        }

      public:
        CMenuJoinServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pConnecting(nullptr), 
          m_pMainFrame (nullptr),
          m_pTournament(m_pState->getGlobal()->getTournament())
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_joinserver.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pConnecting = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_connecting", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (m_pConnecting)
            m_pConnecting->setVisible(true);

          m_pMainFrame = reinterpret_cast<gui::CMenuBackground *>(helpers::findElementByNameAndType("main_frame", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));

          if (m_pMainFrame != nullptr)
            m_pMainFrame->setVisible(false);

          irr::u32 l_iServerIp = std::atoi(m_pState->getGlobal()->getGlobal("selected_server_ip"  ).c_str());
          irr::u32 l_iPortNo   = std::atoi(m_pState->getGlobal()->getGlobal("selected_server_port").c_str());

          m_pInputQueue = new threads::CInputQueue();
          a_pState->getGlobal()->startGameClient(l_iServerIp, l_iPortNo, m_pInputQueue);
          
          m_pClient = a_pState->getGlobal()->getGameClient();

          for (int i = 0; i < 16; i++) {
            gui::CMenuBackground *l_pRoot = reinterpret_cast<gui::CMenuBackground *>(helpers::findElementByNameAndType(("player" + std::to_string(i + 1)).c_str(), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (l_pRoot != nullptr) {
              irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_name" , irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUIStaticText *l_pNum  = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUITab        *l_pTab  = reinterpret_cast<irr::gui::IGUITab        *>(helpers::findElementByNameAndType("player_tab"  , irr::gui::EGUIET_TAB        , l_pRoot));
              irr::gui::IGUIStaticText *l_pStrt = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("starting_number", irr::gui::EGUIET_STATIC_TEXT, l_pRoot));

              if (l_pName != nullptr && l_pNum != nullptr && l_pTab != nullptr && l_pStrt) {
                l_pNum->setText(std::to_wstring(i + 1).c_str());
                m_vPlayers.push_back(std::make_tuple(l_pRoot, l_pTab, l_pName, l_pStrt));
                l_pTab->setVisible(false);

                gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("add_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);

                l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);
              }
            }
          }

          hideAiAndNumber(m_pGui->getRootGUIElement());
          updatePlayerList();
        }

        virtual ~CMenuJoinServer() {
          if (m_pClient!= nullptr)
            m_pClient->getOutputQueue()->removeListener(m_pInputQueue);

          delete m_pInputQueue;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                m_pClient = nullptr;
                m_pState->getGlobal()->stopGameClient();
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }

          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override { 
          if (m_pInputQueue != nullptr) {
            messages::IMessage *l_pMsg = m_pInputQueue->popMessage();

            if (l_pMsg != nullptr) {
              if (l_pMsg->getMessageId() == messages::enMessageIDs::RegisterPlayer) {
                messages::CRegisterPlayer *p = reinterpret_cast<messages::CRegisterPlayer *>(l_pMsg);

                gameclasses::SPlayer* l_pPlayer = new gameclasses::SPlayer(
                  p->getident(),
                  -1,
                  p->getname(),
                  p->gettexture(),
                  "Network",
                  p->getshortname(),
                  data::SPlayerData::enAiHelp::Off,
                  nullptr,
                  data::enPlayerType::Network
                );

                m_pTournament->m_vPlayers.push_back(l_pPlayer);

                updatePlayerList();
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::UpdatePlayerId) {
                std::string l_sName = "";

                messages::CUpdatePlayerId *p = reinterpret_cast<messages::CUpdatePlayerId *>(l_pMsg);
                for (std::vector<gameclasses::SPlayer *>::iterator it = m_pTournament->m_vPlayers.begin(); it != m_pTournament->m_vPlayers.end(); it++) {
                  if ((*it)->m_iPlayer == p->getoriginal_id()) {
                    printf("Update ID of race player %s to %i\n", (*it)->m_sName.c_str(), p->getnetgame_id());
                    (*it)->m_iPlayer = p->getnetgame_id();
                    l_sName = (*it)->m_sName;
                    break;
                  }
                }
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::RacePlayer) {
                messages::CRacePlayer *p = reinterpret_cast<messages::CRacePlayer *>(l_pMsg);
                bool l_bAdd = true;
                for (auto l_pPlayer: m_pTournament->m_vPlayers) {
                  if (l_pPlayer->m_iPlayer == p->getplayerid()) {
                    l_bAdd = false;
                    break;
                  }
                }

                if (l_bAdd) {
                  gameclasses::SPlayer* l_pPlayer = new gameclasses::SPlayer(
                    p->getplayerid(),
                    -1,
                    p->getname(),
                    p->gettexture(),
                    "Network",
                    p->getshortname(),
                    data::SPlayerData::enAiHelp::Off,
                    nullptr,
                    data::enPlayerType::Network
                  );

                  m_pTournament->m_vPlayers.push_back(l_pPlayer);

                  updatePlayerList();
                }
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::PlayerRemoved) {
                messages::CPlayerRemoved *p = reinterpret_cast<messages::CPlayerRemoved *>(l_pMsg);

                for (std::vector<gameclasses::SPlayer *>::iterator it = m_pTournament->m_vPlayers.begin(); it != m_pTournament->m_vPlayers.end(); it++) {
                  if ((*it)->m_iPlayer == p->getplayerid()) {
                    m_pTournament->m_vPlayers.erase(it);
                    updatePlayerList();
                    break;
                  }
                }
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::ServerDisconnect) {
                m_pState->getGlobal()->setGlobal("message_text", m_pConnecting->isVisible() ? "Could not establish a connection to the server" : "The server has closed the connection.");
                createMenu("menu_message", m_pDevice, m_pManager, m_pState);
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::ConnectedToServer) {
                if (m_pConnecting != nullptr) m_pConnecting->setVisible(false);
                if (m_pMainFrame  != nullptr) m_pMainFrame ->setVisible(true );
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::ChangeState) {
                messages::CChangeState *p = reinterpret_cast<messages::CChangeState *>(l_pMsg);
                std::string l_sNewState = p->getnewstate();
                printf("Change state to \"%s\"\n", l_sNewState.c_str());

                std::sort(m_pTournament->m_vPlayers.begin(), m_pTournament->m_vPlayers.end(), [](gameclasses::SPlayer *l_cPlayer1, gameclasses::SPlayer *l_cPlayer2) {
                  return l_cPlayer1->m_iPlayer < l_cPlayer2->m_iPlayer;
                });

                createMenu(l_sNewState.c_str(), m_pDevice, m_pManager, m_pState);
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::SetTexture) {
                messages::CSetTexture *p = reinterpret_cast<messages::CSetTexture *>(l_pMsg);

                for (auto l_pPlayer: m_pTournament->m_vPlayers) {
                  if (l_pPlayer->m_iPlayer == p->getplayerid() && l_pPlayer->m_sTexture.find("number=") == std::string::npos) {
                    l_pPlayer->m_sTexture = p->gettexture();
                    updatePlayerList();
                    break;
                  }
                }
              }

              delete l_pMsg;
            }
          }

          return false;
        }

        /**
        * Does this menu handle the state change network message itself?
        * Might me necessary if some data needs to be written to a global
        * @return true it this menu handle the state change network message itself
        */
        virtual bool handlesNetworkStateChange() override {
          return true;
        }

      };


      IMenuHandler *createMenuJoinServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuJoinServer(a_pDevice, a_pManager, a_pState);
      }
  }
}