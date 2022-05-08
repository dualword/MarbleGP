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
        network::CGameClient *m_pClient;

      public:
        CMenuNetworkLobby(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
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

          m_pManager->pushToMenuStack("menu_raceresult");
        }

        virtual ~CMenuNetworkLobby() {
        }
        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual void run() { 
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                if (m_pClient != nullptr) {
                  m_pState->getGlobal()->stopGameClient();
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