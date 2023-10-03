// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
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

        data::SRacePlayers  m_cPlayers;       /**< The players */
        data::SChampionship m_cChampionship;  /**< The championship */

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

            irr::gui::IGUIStaticText *l_pHead = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

            if (l_pHead != nullptr && m_pServer != nullptr) {
              std::wstring l_sHeadline = L"Server running on \"" + helpers::s2ws(m_pServer->getHostName()) + L"\"";
              l_pHead->setText(l_sHeadline.c_str());
            }
          }

          m_pInputQueue = new threads::CInputQueue();
          if (m_pServer != nullptr)
            m_pServer->getOutputQueue()->addListener(m_pInputQueue);

          m_cPlayers.deserialize(m_pState->getGlobal()->getGlobal("raceplayers"));

          m_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

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
                std::string l_sPlayers = m_cPlayers.serialize();
                m_pState->getGlobal()->setGlobal("raceplayers", l_sPlayers);

                m_pState->getGlobal()->setGlobal("championship", m_cChampionship.serialize());

                printf("\n*********\n");
                printf("%s\n", m_cPlayers.toString().c_str());
                printf("\n*********\n");

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

                data::SPlayerData l_cPlayer = data::SPlayerData();

                l_cPlayer.m_sName      = p->getname     ();
                l_cPlayer.m_sTexture   = p->gettexture  ();
                l_cPlayer.m_iPlayerId  = p->getident    ();
                l_cPlayer.m_sShortName = p->getshortname();
                l_cPlayer.m_sControls  = "Network";
                l_cPlayer.m_eType      = data::enPlayerType::Network;

                m_cPlayers.m_vPlayers.push_back(l_cPlayer);

                printf("Player %s added to player list (id = %i)\n", l_cPlayer.m_sName.c_str(), p->getident());

                m_cChampionship.m_vPlayers.push_back(data::SChampionshipPlayer(
                  l_cPlayer.m_iPlayerId,
                  l_cPlayer.m_sName
                ));

                m_cChampionship.m_iGridSize = (int)m_cChampionship.m_vPlayers.size();

                updatePlayerList();
              }
              else if (l_pMsg->getMessageId() == messages::enMessageIDs::PlayerRemoved) {
                messages::CPlayerRemoved *p = reinterpret_cast<messages::CPlayerRemoved *>(l_pMsg);

                for (std::vector<data::SPlayerData>::iterator it = m_cPlayers.m_vPlayers.begin(); it != m_cPlayers.m_vPlayers.end(); it++) {
                  if ((*it).m_iPlayerId == p->getplayerid()) {
                    m_cPlayers.m_vPlayers.erase(it);
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