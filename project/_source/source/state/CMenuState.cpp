// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/ICustomEventReceiver.h>
#include <controller/CControllerMenu.h>
#include <threads/CMessageQueue.h>
#include <sound/ISoundInterface.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameClient.h>
#include <menu/IMenuHandler.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <gui/CMenuButton.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace state {
    CMenuState::CMenuState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) : IState(a_pDevice, a_pGlobal), m_pMenu(nullptr), m_bMenuCtrl(true), m_pController(nullptr), m_pTouchCtrl(nullptr), m_pClient(nullptr), m_pInputQueue(nullptr) {
      m_pController = new controller::CControllerMenu(-1);

      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventSettingsChanged;
      m_pController->update(l_cEvent);
    }

    CMenuState::~CMenuState() {
      m_vMenuStack.clear();

      if (m_pMenu != nullptr) {
        delete m_pMenu;
        m_pMenu = nullptr;
      }

      if (m_pController != nullptr) {
        delete m_pController;
        m_pController = nullptr;
      }
    }

    /**
    * This method is called when the state is activated
    */
    void CMenuState::activate() {
      std::vector<data::SPlayerData> l_vProfiles = data::SPlayerData::createPlayerVector(m_pGlobal->getSetting("profiles"));
      if (l_vProfiles.size() == 0) {
        CGlobal::getInstance()->setGlobal("edit_profile", "commit_profile");
        pushToMenuStack("menu_profilewizard");
      }

      m_eState = enState::None;
      std::string l_sState = popMenuStack();
      menu::IMenuHandler::createMenu(l_sState, m_pDevice, this, this);

      m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStMenu);
      m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f);
      m_pGlobal->getSoundInterface()->setMenuFlag(true);

      if (m_pInputQueue != nullptr && m_pClient != nullptr)
        m_pClient->getOutputQueue()->addListener(m_pInputQueue);
    }

    /**
    * This method is called when the state is deactivated
    */
    void CMenuState::deactivate() {
      printf("Menu State deactivated.\n");
      if (m_pMenu != nullptr) {
        delete m_pMenu;
        m_pMenu = nullptr;
      }

      if (m_pInputQueue != nullptr) {
        if (m_pClient != nullptr)
          m_pClient->getOutputQueue()->removeListener(m_pInputQueue);

        delete m_pInputQueue;
        m_pInputQueue = nullptr;
      }

      m_pClient = nullptr;
    }

    /**
    * Change the Z-Layer for the Menu Controller
    * @param a_iZLayer the new Z-Layer
    */
    void CMenuState::setZLayer(int a_iZLayer) {
      if (m_pController != nullptr)
        m_pController->setZLayer(a_iZLayer);

    }

    /**
    * Return the state's ID
    */
    enState CMenuState::getId() {
      return enState::Menu;
    }

    /**
    * Event handling method. The main class passes all Irrlicht events to this method
    */
    bool CMenuState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (m_pController != nullptr && m_bMenuCtrl)
        l_bRet = m_pController->update(a_cEvent);

      if (!l_bRet && m_pTouchCtrl != nullptr)
        l_bRet = m_pTouchCtrl->handleEvent(a_cEvent);

      if (!l_bRet && m_pMenu != nullptr)
        l_bRet = m_pMenu->OnEvent(a_cEvent);

      if (!l_bRet) {
        if (a_cEvent.EventType == irr::EET_USER_EVENT) {
          if (a_cEvent.UserEvent.UserData1 == c_iEventHideCursor) {
            if (m_pDevice->getCursorControl() != nullptr)
              m_pDevice->getCursorControl()->setVisible(a_cEvent.UserEvent.UserData2 == 0 && m_pController != nullptr);

            if (m_pController != nullptr) {
              m_pController->setVisible(a_cEvent.UserEvent.UserData2 == 0);
            }

            l_bRet = true;
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iEventOkClicked) {
            irr::gui::IGUIElement *p = menu::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement());
            if (p != nullptr && m_pMenu != nullptr) {
              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
              l_cEvent.GUIEvent.Caller    = p;
              l_cEvent.GUIEvent.Element   = p;
              m_pMenu->OnEvent(l_cEvent);
            }
            l_bRet = true;
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iEventCancelClicked) {
            irr::gui::IGUIElement *p = menu::findElementByNameAndType("cancel", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement());
            if (p != nullptr && m_pMenu != nullptr) {
              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
              l_cEvent.GUIEvent.Caller    = p;
              l_cEvent.GUIEvent.Element   = p;
              m_pMenu->OnEvent(l_cEvent);
            }
            l_bRet = true;
          }
        }
#ifdef _ANDROID
        if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
          if (a_cEvent.KeyInput.Key == irr::KEY_BACK) {
            if (m_pMenu != nullptr && a_cEvent.KeyInput.PressedDown) {
              irr::gui::IGUIElement *p = m_pGui->getRootGUIElement()->getElementFromId(20001);
              if (p != nullptr) {
                irr::SEvent l_cEvent;

                l_cEvent.EventType          = irr::EET_GUI_EVENT;
                l_cEvent.GUIEvent.Caller    = p;
                l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

                CGlobal::getInstance()->OnEvent(l_cEvent);
              }
            }
            l_bRet = true;
          }
        }
#endif
      }

      return l_bRet;
    }

    /**
    * Set a controller to handle touch input
    */
    void CMenuState::setTouchController(controller::ICustomEventReceiver* a_pTouchCtrl) {
      m_pTouchCtrl = a_pTouchCtrl;
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CMenuState::run() {
      if (m_pDevice->run()) {
        bool b = false;

        if (m_pMenu != nullptr)
          b = m_pMenu->run();

        if (!b) {
          m_pDrv->beginScene(true, true);
          m_pSmgr->drawAll();
          m_pGui->drawAll();

          if (m_pController != nullptr)
            m_pController->draw();

          m_pDrv->endScene();
        }
      }
      else m_eState = enState::Quit;

      if (m_pInputQueue != nullptr) {
        messages::IMessage *l_pMsg = m_pInputQueue->popMessage();
        if (l_pMsg != nullptr) {
          if (l_pMsg->getMessageId() == messages::enMessageIDs::ChangeState) {
            if (!m_pMenu->handlesNetworkStateChange()) {
              messages::CChangeState *p = reinterpret_cast<messages::CChangeState *>(l_pMsg);
              std::string l_sNewState = p->getnewstate();
              printf("Change state to \"%s\"\n", l_sNewState.c_str());

              // Special handling if we start a game
              if (l_sNewState == "state_game") {
                m_pGlobal->initNextRaceScreen();
                setState(state::enState::Game);
                pushToMenuStack("menu_raceresult");
              }
              else {
                m_pMenu->createMenu(l_sNewState, m_pDevice, m_pMenu->getMenuManager(), this);
              }
            }
          }
          else if (l_pMsg->getMessageId() == messages::enMessageIDs::ServerDisconnect) {
            m_pGlobal->setGlobal("message_headline", "Server Disconnected");
            m_pGlobal->setGlobal("message_text"    , "The server has closed the connection.");
            m_pMenu->createMenu("menu_message", m_pDevice, m_pMenu->getMenuManager(), this);
          }
          else {
            handleMessage(l_pMsg);
          }

          delete l_pMsg;
        }
      }

      return m_pGlobal->getSettingData().m_bGfxChange ? enState::Restart : m_eState;
    }

    /**
    * Push a menu to the menu stack
    * @param a_sMenu the name of the menu
    */
    void CMenuState::pushToMenuStack(const std::string& a_sMenu) {
      m_vMenuStack.push_back(a_sMenu);
    }

    /**
    * Clear the menu stack
    */
    void CMenuState::clearMenuStack() {
      m_vMenuStack.clear();
    }

    /**
    * Get the topmost entry of the menu stack without removing it
    * @return the topmost entry of the menu stack
    */
    std::string CMenuState::peekMenuStack() {
      if (m_vMenuStack.size() > 0)
        return m_vMenuStack.back();
      else
        if (CGlobal::getInstance()->getGlobal("message_headline") != "" && CGlobal::getInstance()->getGlobal("label_message") != "")
          return "menu_message";
        else
          return "menu_main";
    }

    /**
    * Get the topmost entry of the menu stack and remove it
    * @return the topmost entry of the menu stack
    */
    std::string CMenuState::popMenuStack() {
      std::string l_sState = "menu_main";

      if (CGlobal::getInstance()->getGlobal("message_headline") != "" && CGlobal::getInstance()->getGlobal("label_message") != "")
        l_sState = "menu_message";

      if (m_vMenuStack.size() > 0) {
        l_sState = m_vMenuStack.back();
        m_vMenuStack.pop_back();
      }

      return l_sState;
    }

    /**
    * Does this state want the virtual keyboard?
    */
    bool CMenuState::showVirtualKeyboard() {
#ifdef _ANDROID
      return true;
#endif
      return m_pController != nullptr && m_pGlobal->getSettingData().m_bVirtualKeys;
    }

    /**
    * Callback before a menu is changed, deletes the current menu
    */
    void CMenuState::beforeChangeMenu() {
      menu::IMenuHandler *p = m_pMenu;
      m_pMenu = nullptr;

      if (p != nullptr)
        delete p;
    }

    /**
    * Change the menu
    * @param a_pMenu the new active menu
    */
    menu::IMenuHandler *CMenuState::changeMenu(menu::IMenuHandler *a_pMenu) {
      m_pMenu = a_pMenu;

      if (m_pController != nullptr) {
        m_pController->reset();
      }

      if (m_pMenu != nullptr)
        m_pMenu->activate();

      return m_pMenu;
    }

    /**
    * A callback for the menu state to get informed about a menu change
    * @param a_sMenu the loaded menu
    */
    void CMenuState::menuChanged(const std::string &a_sMenu) { 
      m_pClient = m_pGlobal->getGameClient();

      if (m_pClient != nullptr) {
        if (m_pInputQueue == nullptr) {
          m_pInputQueue = new threads::CInputQueue();
          m_pClient->getOutputQueue()->addListener(m_pInputQueue);
        }

        m_pClient->stateChanged(a_sMenu);
      }
    }

    /**
    * Turn the menu controller on or off
    * @param a_bFlag on / off flag
    */
    void CMenuState::enableMenuController(bool a_bFlag) {
      m_bMenuCtrl = a_bFlag;
    }
  }
}