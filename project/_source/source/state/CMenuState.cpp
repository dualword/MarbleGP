// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/ICustomEventReceiver.h>
#include <controller/CControllerMenu.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <sound/ISoundInterface.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameClient.h>
#include <platform/CPlatform.h>
#include <menu/IMenuHandler.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <gui/CMenuButton.h>
#include <CGlobal.h>
#include <Defines.h>
#include <algorithm>

namespace dustbin {
  namespace state {
    CMenuState::CMenuState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) : IState(a_pDevice, a_pGlobal), m_pMenu(nullptr), m_bMenuCtrl(true), m_pController(nullptr), m_pTouchCtrl(nullptr), m_pClient(nullptr), m_pInputQueue(nullptr), m_sMenu("") {
      m_pController = new controller::CControllerMenu();

      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventSettingsChanged;
      m_pController->update(l_cEvent);
      if (m_pController->hasError()) {
        delete m_pController;
        m_pController = new controller::CControllerMenu();
      }
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
      if (m_pGlobal->getGlobal("FirstStart") == "true") {
        m_pGlobal->setGlobal("FirstStart", "false");
        clearMenuStack();
        pushToMenuStack("menu_intro");
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

      if (!l_bRet && m_pTouchCtrl != nullptr)
        l_bRet = m_pTouchCtrl->handleEvent(a_cEvent);

      if (!l_bRet && m_pMenu != nullptr)
        l_bRet = m_pMenu->OnEvent(a_cEvent);

      if (!l_bRet && m_pController != nullptr && m_bMenuCtrl)
        l_bRet = m_pController->update(a_cEvent);

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
            irr::gui::IGUIElement *p = helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement());
            if (p != nullptr && m_pMenu != nullptr) {
              irr::SEvent l_cEvent{};
              l_cEvent.EventType = irr::EET_GUI_EVENT;
              l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
              l_cEvent.GUIEvent.Caller    = p;
              l_cEvent.GUIEvent.Element   = p;
              m_pMenu->OnEvent(l_cEvent);
            }
            l_bRet = true;
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iEventCancelClicked) {
            irr::gui::IGUIElement *p = helpers::findElementByNameAndType("cancel", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement());
            if (p != nullptr && m_pMenu != nullptr) {
              irr::SEvent l_cEvent{};
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
#else
#ifdef _DEBUG
        if (a_cEvent.EventType == irr::EET_KEY_INPUT_EVENT) {
          if (a_cEvent.KeyInput.Key == irr::KEY_SNAPSHOT) {


            std::string l_sName = m_sMenu;

            if (l_sName.find_last_of('/') != std::string::npos) {
              l_sName = l_sName.substr(l_sName.find_last_of('/'));
            }

            l_sName = l_sName;

            std::string l_sBase = l_sName;

            int l_iIndex = 0;

            while (m_pFs->existFile((helpers::ws2s(platform::portableGetDataPath()) + l_sName + ".png").c_str())) {
              l_iIndex++;
              l_sName = l_sBase + "_" + std::to_string(l_iIndex);
            }

            printf("Screenshot: %s\n", l_sName.c_str());

            irr::video::IImage *l_pScreenshot = m_pDrv->createScreenShot();
            m_pDrv->writeImageToFile(l_pScreenshot, (helpers::ws2s(platform::portableGetDataPath()) + l_sName + ".png").c_str());
            l_pScreenshot->drop();

            std::vector<std::tuple<std::string, std::string, irr::core::rect<irr::f32>>> l_vData;
            std::vector<std::string> l_vElements;

            createManualData(m_pGui->getRootGUIElement(), l_vData, l_vElements);

            std::string l_sHtml = "";
            std::string l_sCss  = "";

            l_sHtml  = "  <body>\n";
            l_sHtml += "    <p>\n";
            l_sHtml += "      <div class=\"container\">\n";
            l_sHtml += "        <img src=\"" + l_sName + ".png\" alt=\"" + l_sName + "\" class=\"screenshot\" >\n";

            l_sCss  = "      .screenshot {\n";
            l_sCss += "         width: 100%;\n";
            l_sCss += "         height: auto;\n";
            l_sCss += "         display: block;\n";
            l_sCss += "         margin-left: auto;\n";
            l_sCss += "         margin-right: auto;\n";
            l_sCss += "       }\n";
            l_sCss += "         \n";
            l_sCss += "       .container {\n";
            l_sCss += "         position: relative;\n";
            l_sCss += "         width: 70%;\n";
            l_sCss += "       }\n";
            l_sCss += "       \n";
            l_sCss += "       .overlay, .overlay_subtext {\n";
            l_sCss += "         position: absolute;\n";
            l_sCss += "       }\n";
            l_sCss += "       \n";
            l_sCss += "       .overlay {\n";
            l_sCss += "         background-color: rgba(192, 255, 192, 0.5)\n";
            l_sCss += "       }\n";
            l_sCss += "       \n";
            l_sCss += "       .overlay_subtext {\n";
            l_sCss += "         visibility: hidden;\n";
            l_sCss += "         width: 80%;\n";
            l_sCss += "         left: 10%;\n";
            l_sCss += "         height: 3em;\n";
            l_sCss += "       }\n\n";

            for (auto l_tData: l_vData) {
              printf("Element %s [%s] ((%.2f, %.2f) - (%.2f, %.2f)\n", 
                std::get<0>(l_tData).c_str(), 
                std::get<1>(l_tData).c_str(),
                std::get<2>(l_tData).UpperLeftCorner .X,
                std::get<2>(l_tData).UpperLeftCorner .Y,
                std::get<2>(l_tData).LowerRightCorner.X,
                std::get<2>(l_tData).LowerRightCorner.Y
              );

              std::string l_sId = "overlay_" + l_sName + "_" + std::get<0>(l_tData);

              char l_sDummy[8192];

              sprintf_s(l_sDummy, 8192, "       #%s { top: %.2f%%; left: %.2f%%; width: %.2f%%; height: %.2f%%; }\n", 
                l_sId.c_str(),
                std::get<2>(l_tData).UpperLeftCorner .Y,
                std::get<2>(l_tData).UpperLeftCorner .X,
                std::get<2>(l_tData).getWidth (),
                std::get<2>(l_tData).getHeight()
              );

              l_sCss += l_sDummy;

              l_sHtml += "        <div class=\"overlay\" id=\"" + l_sId + "\" title=\"" + std::get<1>(l_tData) + "\">&nbsp;</div>\n";
            }

            l_sHtml += "      </div>\n";
            l_sHtml += "      <br />\n";
            l_sHtml += "      <p>" + m_sMenu + "</p>\n";
            l_sHtml += "    </p>\n";
            l_sHtml += "  </body>";
            l_sHtml += "</html>\n";

            l_sHtml = std::string("<html>\n") +
                      "  <head>\n" + 
                      "    <style>\n\n" + l_sCss + "\n" +
                      "    </style>\n\n" +
                      "    <link href=\"jquery-ui.min.css\" rel=\"stylesheet\">\n" +
                      "    <link href=\"jquery-ui.structure.min.css\" rel=\"stylesheet\">\n" +
                      "    <link href=\"jquery-ui.theme.min.css\" rel=\"stylesheet\">\n" +
                      "    <link href=\"jquery.treemenu.min.css\" rel=\"stylesheet\">\n" +
                      "    \n" +
                      "    <script src=\"jquery-3.6.3.min.js\"></script>\n" +
                      "    <script src=\"jquery-ui.min.js\"></script>\n" +
                      "    <script src=\"jquery.treemenu.min.js\"></script>\n" +
                      "    <script>\n" +
                      "      \n" +
                      "      $(document).ready(function() {\n" + 
                      "        $(document).tooltip({ position: { my: \"center\", at: \"right center\" } });\n" +
                      "      });\n"
                      "      \n" + 
                      "    </script>\n" +
                      "  </head>\n" + l_sHtml;

            irr::io::IWriteFile *l_pHtml = m_pFs->createAndWriteFile((helpers::ws2s(platform::portableGetDataPath()) + l_sName + ".html").c_str());
            l_pHtml->write(l_sHtml.c_str(), l_sHtml.size());
            l_pHtml->drop();

            l_bRet = true;
          }
        }
#endif
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

          if (l_pMsg->getMessageId() == messages::enMessageIDs::ServerDisconnect) {
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
      m_sMenu = a_sMenu;

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

    /**
    * Is the menu controlled by gamepad?
    * @return true if the menu is controlled by a gamepad
    */
    bool CMenuState::isMenuControlledByGamepad() {
      std::string l_sCtrl = m_pGlobal->getSettingData().m_sMenuCtrl;
      return l_sCtrl == "Gamepad" && m_pController->isJoystickAssigned();
    }

#ifdef _DEBUG
    /**
    * Create the CSS and JavaScript data necessary for the currently active menu
    * @param a_pElement the element to add to the manual data
    * @param a_sData the manual data (0 == Name of the element, 1 == tooltip text, 2 == the rect of the element in percent)
    * @param a_sElements Names that are already stored in the data vector
    */
    void CMenuState::createManualData(irr::gui::IGUIElement *l_pElement, std::vector<std::tuple<std::string, std::string, irr::core::rect<irr::f32>>> &a_sData, std::vector<std::string> &a_sElements) {
      if (!l_pElement->isVisible())
        return;

      std::string l_sName = l_pElement->getName();

      if (l_sName == "" && l_pElement->getID() > 0)
        l_sName = std::to_string(l_pElement->getID());

      if (l_sName != "") {
        std::string l_sHint = helpers::ws2s(l_pElement->getToolTipText().c_str());

        if (l_sHint != "") {
          int l_iIndex = 0;

          std::string l_sBase = l_sName;

          while (std::find(a_sElements.begin(), a_sElements.end(), l_sName) != a_sElements.end()) {
            l_iIndex++;
            l_sName = l_sBase + "_" + std::to_string(l_iIndex);
          }

          irr::core::dimension2du l_cSize = m_pDrv->getScreenSize();
          irr::core::recti        l_cRect = l_pElement->getAbsolutePosition();

          irr::core::position2di l_cPos = l_cRect.UpperLeftCorner;

          irr::core::rect<irr::f32> l_cPercent = irr::core::rect<irr::f32>(
            irr::core::position2df(
              100.0f * (irr::f32)l_cPos.X / (irr::f32)l_cSize.Width,
              100.0f * (irr::f32)l_cPos.Y / (irr::f32)l_cSize.Height
            ),
            irr::core::dimension2df(
              100.0f * (irr::f32)l_cRect.getWidth () / (irr::f32)l_cSize.Width,
              100.0f * (irr::f32)l_cRect.getHeight() / (irr::f32)l_cSize.Height
            )
          );

          a_sData.push_back(std::make_tuple(l_sName, l_sHint, l_cPercent));
          a_sElements.push_back(l_sName);
        }
      }

      for (irr::core::list<irr::gui::IGUIElement *>::ConstIterator l_itChild = l_pElement->getChildren().begin(); l_itChild != l_pElement->getChildren().end(); l_itChild++)
        createManualData(*l_itChild, a_sData, a_sElements);
    }
#endif
  }
}

