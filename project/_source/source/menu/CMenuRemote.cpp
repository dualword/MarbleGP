// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/messages/IWebLogReceiver.h>
#include <_generated/messages/CMessages.h>
#include <webserver/CWebServerBase.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <gui/CGuiLogDisplay.h>
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
    class CMenuRemote : public IMenuHandler, public messages::IWebLogReceiver {
      private:
        webserver::CWebServerBase *m_pServer;

        gui::CGuiLogDisplay *m_pLog;

        threads::CInputQueue *m_pQueue;

      protected:
        /**
        * This function receives messages of type "WebLogMessage"
        * @param a_Data The log message
        */
        virtual void onWeblogmessage(irr::s32 a_Level, const std::string &a_Data) override {
          if (m_pLog != nullptr) {
            m_pLog->addLogLine((irr::ELOG_LEVEL)a_Level, helpers::s2ws(a_Data));
          }
        }

      public:
        CMenuRemote(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), m_pServer(nullptr), m_pLog(nullptr), m_pQueue(nullptr) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_remote.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pQueue = new threads::CInputQueue();

          m_pServer = new webserver::CWebServerBase(4693, m_pQueue);

          irr::gui::IGUIStaticText *l_pInfo = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_server", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          m_pLog = reinterpret_cast<gui::CGuiLogDisplay *>(findElementByNameAndType("log_display", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_GuiLogDisplayId, m_pGui->getRootGUIElement()));

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

          if (m_pQueue != nullptr) {
            delete m_pQueue;
            m_pQueue = nullptr;
          }
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        * @return true if drawing was done by the menu, false otherwise
        */
        virtual bool run() override {
          if (m_pQueue != nullptr) {
            messages::IMessage *l_pMsg = m_pQueue->popMessage();
            if (l_pMsg != nullptr) {
              handleMessage(l_pMsg, true);
            }
          }

          return false;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                l_bRet = true;
                platform::saveSettings();
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