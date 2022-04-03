// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <controller/ICustomEventReceiver.h>
#include <controller/CControllerMenu.h>
#include <sound/ISoundInterface.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace state {
    CMenuState::CMenuState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) : IState(a_pDevice, a_pGlobal), m_pMenu(nullptr), m_pController(nullptr), m_pTouchCtrl(nullptr) {
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
      m_eState = enState::None;
      std::string l_sState = popMenuStack();
      menu::IMenuHandler::createMenu(l_sState, m_pDevice, this, this);

      m_pGlobal->getSoundInterface()->startSoundtrack(enSoundTrack::enStMenu);
      m_pGlobal->getSoundInterface()->setSoundtrackFade(1.0f);
      m_pGlobal->getSoundInterface()->setMenuFlag(true);
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

      if (m_pController != nullptr)
        m_pController->update(a_cEvent);

      if (m_pTouchCtrl != nullptr)
        m_pTouchCtrl->handleEvent(a_cEvent);

      if (m_pMenu != nullptr)
        l_bRet = m_pMenu->OnEvent(a_cEvent);

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
        m_pDrv->beginScene(true, true);
        m_pSmgr->drawAll();

        if (m_pMenu != nullptr)
          m_pMenu->run();

        m_pGui->drawAll();

        if (m_pController != nullptr)
          m_pController->draw();

        m_pDrv->endScene();
      }
      else m_eState = enState::Quit;

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
        return "menu_main";
    }

    /**
    * Get the topmost entry of the menu stack and remove it
    * @return the topmost entry of the menu stack
    */
    std::string CMenuState::popMenuStack() {
      std::string l_sState = "menu_main";

      if (m_vMenuStack.size() > 0) {
        l_sState = m_vMenuStack.back();
        m_vMenuStack.pop_back();
      }

      return l_sState;
    }

    /**
    * Change the menu
    * @param a_pMenu the new active menu
    */
    menu::IMenuHandler *CMenuState::changeMenu(menu::IMenuHandler *a_pMenu) {
      menu::IMenuHandler *p = m_pMenu;
      m_pMenu = nullptr;

      if (p != nullptr)
        delete p;

      m_pMenu = a_pMenu;

      if (m_pController != nullptr) {
        m_pController->reset();
      }

      return m_pMenu;
    }
  }
}