// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <gui/CMenuBackground.h>
#include <network/CGameClient.h>
#include <helpers/CMenuLoader.h>
#include <gui/CMenuBackground.h>
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

        data::SRacePlayers m_cPlayers;    /**< The players */

        irr::gui::IGUIStaticText *m_pConnecting;    /**< The "connecting" label */
        gui::CMenuBackground     *m_pMainFrame;     /**< The main frame */

        std::vector<std::tuple<gui::CMenuBackground *, irr::gui::IGUITab *, irr::gui::IGUIStaticText *>> m_vPlayers; /**< The root elements and the name text elements for the players */

        void updatePlayerList() {
          for (std::vector<std::tuple<gui::CMenuBackground*, irr::gui::IGUITab*, irr::gui::IGUIStaticText*>>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
            std::get<1>(*it)->setVisible(false);
          }

          int i = 0;
          for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
            if (i < m_vPlayers.size()) {
              std::get<1>(m_vPlayers[i])->setVisible(true);
              std::get<2>(m_vPlayers[i])->setText(helpers::s2ws((*it).m_sName).c_str());
            }

            i++;
          }
        }

      public:
        CMenuJoinServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), m_pConnecting(nullptr), m_pMainFrame(nullptr) {
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_joinserver.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pConnecting = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_connecting", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (m_pConnecting)
            m_pConnecting->setVisible(true);

          m_pMainFrame = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType("main_frame", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));

          if (m_pMainFrame != nullptr)
            m_pMainFrame->setVisible(false);

          m_pState->setZLayer(0);

          m_pInputQueue = new threads::CInputQueue();
          a_pState->getGlobal()->startGameClient("Saratoga", 4693, m_pInputQueue);
          
          m_pClient = a_pState->getGlobal()->getGameClient();

          for (int i = 0; i < 16; i++) {
            gui::CMenuBackground *l_pRoot = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType(("player" + std::to_string(i + 1)).c_str(), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (l_pRoot != nullptr) {
              irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_name" , irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUIStaticText *l_pNum  = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUITab        *l_pTab  = reinterpret_cast<irr::gui::IGUITab        *>(findElementByNameAndType("player_tab"  , irr::gui::EGUIET_TAB        , l_pRoot));

              if (l_pName != nullptr && l_pNum != nullptr && l_pTab != nullptr) {
                l_pNum->setText(std::to_wstring(i + 1).c_str());
                m_vPlayers.push_back(std::make_tuple(l_pRoot, l_pTab, l_pName));
                l_pTab->setVisible(false);

                gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("add_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);

                l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);
              }
            }
          }

          updatePlayerList();
        }

        virtual ~CMenuJoinServer() {
          if (m_pClient!= nullptr)
            m_pClient->getOutputQueue()->removeListener(m_pInputQueue);

          delete m_pInputQueue;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
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
        virtual void run() { 
          if (m_pInputQueue != nullptr) {
            messages::IMessage *l_pMsg = m_pInputQueue->popMessage();

            if (l_pMsg != nullptr) {
              if (l_pMsg->getMessageId() == messages::enMessageIDs::RegisterPlayer) {
                messages::CRegisterPlayer *p = reinterpret_cast<messages::CRegisterPlayer *>(l_pMsg);

                data::SPlayerData l_cPlayer = data::SPlayerData();

                l_cPlayer.m_sName    = p->getname   ();
                l_cPlayer.m_sTexture = p->gettexture();

                m_cPlayers.m_vPlayers.push_back(l_cPlayer);

                updatePlayerList();
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::UpdatePlayerId) {
                messages::CUpdatePlayerId *p = reinterpret_cast<messages::CUpdatePlayerId *>(l_pMsg);
                for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
                  if ((*it).m_iPlayerId == p->getoriginal_id()) {
                    printf("Update ID of player %s to %i\n", (*it).m_sName.c_str(), p->getnetgame_id());
                    (*it).m_iPlayerId = p->getnetgame_id();
                  }
                }
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::RacePlayer) {
                messages::CRacePlayer *p = reinterpret_cast<messages::CRacePlayer *>(l_pMsg);
                bool l_bAdd = true;
                for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
                  if ((*it).m_iPlayerId == p->getplayerid()) {
                    l_bAdd = false;
                    break;
                  }
                }

                if (l_bAdd) {
                  data::SPlayerData l_cNewPlayer;
                  l_cNewPlayer.m_eAiHelp   = data::SPlayerData::enAiHelp::Off;
                  l_cNewPlayer.m_eType     = data::enPlayerType::Network;
                  l_cNewPlayer.m_iPlayerId = p->getplayerid();
                  l_cNewPlayer.m_sControls = "Network";
                  l_cNewPlayer.m_sName     = p->getname();
                  l_cNewPlayer.m_sTexture  = p->gettexture();

                  m_cPlayers.m_vPlayers.push_back(l_cNewPlayer);

                  printf("\n******************\n%s\n", m_cPlayers.toString().c_str());

                  updatePlayerList();
                }
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::PlayerRemoved) {
                messages::CPlayerRemoved *p = reinterpret_cast<messages::CPlayerRemoved *>(l_pMsg);

                int l_iPlayerId = m_cPlayers.m_vPlayers[p->getplayerid()].m_iPlayerId;

                for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
                  if ((*it).m_iPlayerId == l_iPlayerId) {
                    m_cPlayers.m_vPlayers.erase(it);

                    printf("\n******************\n%s\n", m_cPlayers.toString().c_str());

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

              delete l_pMsg;
            }
          }
        }
      };


      IMenuHandler *createMenuJoinServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuJoinServer(a_pDevice, a_pManager, a_pState);
      }
  }
}