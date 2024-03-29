// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <gameclasses/SPlayer.h>
#include <network/CGameServer.h>
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
    * @class CMenuStartServer
    * @author Christian Keimel
    * This menu is for when the server has started
    * and the players can connect
    */
    class CMenuStartServer : public IMenuHandler {
      private:
        threads::CInputQueue *m_pInputQueue;  /**< The input queue to receive messages from the server */
        network::CGameServer *m_pServer;      /**< The game server */

        std::vector<std::tuple<gui::CMenuBackground *, irr::gui::IGUITab *, irr::gui::IGUIStaticText *>> m_vPlayers; /**< The root elements and the name text elements for the players */

        void updatePlayerList() {
          for (std::vector<std::tuple<gui::CMenuBackground*, irr::gui::IGUITab*, irr::gui::IGUIStaticText*>>::iterator it = m_vPlayers.begin(); it != m_vPlayers.end(); it++) {
            std::get<1>(*it)->setVisible(false);
          }

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

          int i = 0;
          for (auto l_pPlayer: l_pTournament->m_vPlayers) {
            if (i < m_vPlayers.size()) {
              std::get<1>(m_vPlayers[i])->setVisible(true);
              std::get<2>(m_vPlayers[i])->setText(helpers::s2ws(l_pPlayer->m_sName).c_str());
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
        CMenuStartServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_startserver.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          data::SFreeGameSlots l_cSlots = data::SFreeGameSlots(m_pState->getGlobal()->getGlobal("free_game_slots"));

          a_pState->getGlobal()->startGameServer(l_cSlots.m_vSlots);
          m_pServer = a_pState->getGlobal()->getGameServer();

          for (int i = 0; i < 16; i++) {
            gui::CMenuBackground *l_pRoot = reinterpret_cast<gui::CMenuBackground *>(helpers::findElementByNameAndType(("player" + std::to_string(i + 1)).c_str(), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (l_pRoot != nullptr) {
              irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_name" , irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUIStaticText *l_pNum  = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, l_pRoot));
              irr::gui::IGUITab        *l_pTab  = reinterpret_cast<irr::gui::IGUITab        *>(helpers::findElementByNameAndType("player_tab"  , irr::gui::EGUIET_TAB        , l_pRoot));

              if (l_pName != nullptr && l_pNum != nullptr && l_pTab != nullptr) {
                l_pNum->setText(std::to_wstring(i + 1).c_str());
                m_vPlayers.push_back(std::make_tuple(l_pRoot, l_pTab, l_pName));
                l_pTab->setVisible(false);

                gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("add_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);

                l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, l_pRoot));
                if (l_pBtn != nullptr)
                  l_pBtn->setVisible(false);
              }
            }

            irr::gui::IGUIStaticText *l_pHead = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

            if (l_pHead != nullptr && m_pServer != nullptr) {
              std::wstring l_sHeadline = L"Server running on \"" + helpers::s2ws(m_pServer->getHostName()) + L"\"";
              l_pHead->setText(l_sHeadline.c_str());
            }
          }

          m_pInputQueue = new threads::CInputQueue();
          if (m_pServer != nullptr)
            m_pServer->getOutputQueue()->addListener(m_pInputQueue);

          hideAiAndNumber(m_pGui->getRootGUIElement());
          updatePlayerList();
        }

        virtual ~CMenuStartServer() {
          if (m_pServer != nullptr)
            m_pServer->getOutputQueue()->removeListener(m_pInputQueue);

          delete m_pInputQueue;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                m_pServer = nullptr;
                m_pState->getGlobal()->stopGameServer();
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
              else if (l_sSender == "ok") {
                m_pServer->setConnectionAllowed(false);
                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
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

                gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

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

                l_pTournament->m_vPlayers.push_back(l_pPlayer);

                printf("Player %s added to player list (id = %i)\n", l_pPlayer->m_sName.c_str(), p->getident());

                updatePlayerList();
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::PlayerRemoved) {
                messages::CPlayerRemoved *p = reinterpret_cast<messages::CPlayerRemoved *>(l_pMsg);

                gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

                for (std::vector<gameclasses::SPlayer *>::iterator l_itPlr = l_pTournament->m_vPlayers.begin(); l_itPlr != l_pTournament->m_vPlayers.end(); l_itPlr++) {
                  if ((*l_itPlr)->m_iPlayer == p->getplayerid()) {
                    l_pTournament->m_vPlayers.erase(l_itPlr);
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
    };


    IMenuHandler *createMenuStartServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuStartServer(a_pDevice, a_pManager, a_pState);
    }
  }
}