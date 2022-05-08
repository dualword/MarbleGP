// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameClient.h>
#include <menu/IMenuHandler.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuNetworkLobby
    * @author Christian Keimel
    * In this menu the clients wait for the server to start a race
    */
    class CMenuNetworkLobby : public IMenuHandler {
      private:
        threads::CInputQueue *m_pInputQueue;
        network::CGameClient *m_pClient;

      public:
        CMenuNetworkLobby(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pInputQueue(new threads::CInputQueue()),
          m_pClient    (a_pState->getGlobal()->getGameClient())
        {
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_lobby.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pState->setZLayer(0);

          irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr)
            p->setText(L"MarbleGP Network Lobby");

          p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_message", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr)
            p->setText(L"Waiting for the server to select a race.");

          if (m_pClient != nullptr) {
            m_pClient->getOutputQueue()->addListener(m_pInputQueue);
            m_pClient->stateChanged("menu_netlobby");
          }
          else {
            m_pState->getGlobal()->setGlobal("message_headline", "Netgame Error");
            m_pState->getGlobal()->setGlobal("message_text"    , "Game Client not initialized.");
            createMenu("menu_message", m_pDevice, m_pManager, m_pState);
          }
        }

        virtual ~CMenuNetworkLobby() {
          if (m_pClient != nullptr) {
            m_pClient->getOutputQueue()->removeListener(m_pInputQueue);
          }

          if (m_pInputQueue != nullptr) {
            delete m_pInputQueue;
            m_pInputQueue = nullptr;
          }
        }
        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual void run() { 
          messages::IMessage *l_pMsg = m_pInputQueue->popMessage();
          if (l_pMsg != nullptr) {
            if (l_pMsg->getMessageId() == messages::enMessageIDs::ChangeState) {
              messages::CChangeState *p = reinterpret_cast<messages::CChangeState *>(l_pMsg);
              std::string l_sNewState = p->getnewstate();
              printf("Change state to \"%s\"\n", l_sNewState.c_str());

              if (l_sNewState == "state_game") {
                m_pState->setState(state::enState::Game);
                m_pManager->pushToMenuStack("menu_raceresult");
              }
              else {
                createMenu(l_sNewState, m_pDevice, m_pManager, m_pState);
              }
            }
            delete l_pMsg;
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                if (m_pClient != nullptr) {
                  m_pState->getGlobal()->stopGameClient();
                  m_pClient = nullptr;
                }

                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }


          return l_bRet;
        }
      };

    IMenuHandler *createMenuNetLobby(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNetworkLobby(a_pDevice, a_pManager, a_pState);
    }
}
}