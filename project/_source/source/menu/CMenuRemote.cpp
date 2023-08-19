// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <webserver/CWebServerBase.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuRemote
    * @author Christian Keimel
    * This menu starts a webserver and allows some remote configuration
    */
    class CMenuRemote : public IMenuHandler {
      private:
        webserver::CWebServerBase *m_pServer;

      public:
        CMenuRemote(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), m_pServer(nullptr) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_remote.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pServer = new webserver::CWebServerBase(4693);

          irr::gui::IGUIStaticText *l_pInfo = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_server", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          m_pServer->startServer();

          if (l_pInfo != nullptr) {
            std::wstring l_sText = L"Use a webbrowser and connect to \"http://" + helpers::s2ws(m_pServer->hostName()) + L":4693\".";
            l_pInfo->setText(l_sText.c_str());
          }
        }

        virtual ~CMenuRemote() {
          if (m_pServer != nullptr) {
            m_pServer->stopServer();
            m_pServer->join();
            delete m_pServer;
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                l_bRet = true;
                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
              }
            }
          }


          return l_bRet;
        }
      };

      IMenuHandler *createMenuRemote(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuRemote(a_pDevice, a_pManager, a_pState);
      }
  }
}