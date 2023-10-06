// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessages.h>
#include <network/CDiscoveryClient.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSearchServer
    * @author Christian Keimel
    * This menu displays a message, e.g. an error
    */
    class CMenuSearchServer : public IMenuHandler {
      private:
        network::CDiscoveryClient *m_pClient;
        threads::CInputQueue      *m_pQueue;

        int m_iStep;

        irr::gui::IGUIStaticText *m_pStep;

        std::vector<std::tuple<int, int, std::string>> m_vServers;  /**< The found server (IP Address, PortNo, Server Name) */

        gui::CMenuButton *m_pButtons[4];

      public:
        CMenuSearchServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pClient(nullptr), 
          m_pQueue(nullptr), 
          m_iStep (0),
          m_pStep (nullptr)
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_searchserver.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pStep = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("LabelStep", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          m_pQueue = new threads::CInputQueue();

          m_pClient = new network::CDiscoveryClient();
          m_pClient->startThread();
          m_pClient->getOutputQueue()->addListener(m_pQueue);

          for (int i = 0; i < 4; i++) {
            m_pButtons[i] = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("GameServer" + std::to_string(i + 1), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          }
        }

        virtual ~CMenuSearchServer() {
          if (m_pQueue != nullptr) {
            delete m_pQueue;
            m_pQueue = nullptr;
          }

          if (m_pClient != nullptr) {
            m_pClient->stopThread();
            m_pClient->join();
            delete m_pClient;
            m_pClient = nullptr;
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
              else {
                printf("Button Clicked: %s (%i)\n", l_sSender.c_str(), a_cEvent.GUIEvent.Caller->getID());
                std::string l_sIdentifier = "GameServer";
                int l_iId = a_cEvent.GUIEvent.Caller->getID();

                if (l_sSender.substr(0, l_sIdentifier.length()) == l_sIdentifier && l_iId < m_vServers.size()) {
                  printf("Connect to Server %s on port %i (IP = %i)\n", std::get<2>(m_vServers[l_iId]).c_str(), std::get<1>(m_vServers[l_iId]), std::get<0>(m_vServers[l_iId]));
                  m_pState->getGlobal()->setGlobal("selected_server_ip"  , std::to_string(std::get<0>(m_vServers[l_iId])));
                  m_pState->getGlobal()->setGlobal("selected_server_port", std::to_string(std::get<1>(m_vServers[l_iId])));

                  std::string l_sNextState = m_pManager->popMenuStack();
                  createMenu(l_sNextState, m_pDevice, m_pManager, m_pState);
                }
              }
            }
          }


          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override {
          messages::IMessage *l_pMsg = m_pQueue->popMessage();

          while (l_pMsg != nullptr) {
            if (l_pMsg->getMessageId() == messages::enMessageIDs::DiscoveryStep) {
              m_iStep++;
              if (m_iStep >= 8) m_iStep = 0;

              std::wstring s = L"Searching for Game Servers ";

              for (int i = 0; i < m_iStep; i++)
                s += L".";

              if (m_pStep != nullptr)
                m_pStep->setText(s.c_str());
            }
            else if (l_pMsg->getMessageId() == messages::enMessageIDs::GameServerFound) {
              if (m_vServers.size() < 4) {
                messages::CGameServerFound *p = reinterpret_cast<messages::CGameServerFound *>(l_pMsg);
                m_vServers.push_back(std::make_tuple(p->getserver(), p->getportno(), p->getname()));

                if (m_pButtons[m_vServers.size() - 1] != nullptr) {
                  m_pButtons[m_vServers.size() - 1]->setText(helpers::s2ws(p->getname()).c_str());
                  m_pButtons[m_vServers.size() - 1]->setVisible(true);
                }
              }
            }

            delete l_pMsg;
            l_pMsg = m_pQueue->popMessage();
          }

          return false;
        }
    };

    IMenuHandler *createMenuSearchServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSearchServer(a_pDevice, a_pManager, a_pState);
    }
  }
}