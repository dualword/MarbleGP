// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
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
            else if (l_sButton == "free_racing") {
              platform::consumeBackEvent(true);
              createMenu("menu_newgamewizard", m_pDevice, m_pManager, m_pState);
            }
            else if (l_sButton == "credits") {
              platform::consumeBackEvent(true);
              createMenu("menu_credits", m_pDevice, m_pManager, m_pState);
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