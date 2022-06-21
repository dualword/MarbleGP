// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <menu/CMenuFactory.h>
#include <menu/IMenuHandler.h>
#include <menu/IMenuManager.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <Defines.h>

namespace dustbin {
  namespace menu {
    /**
    * Find an element by id and type
    * @param a_iId the ID of the element to find
    * @param a_eType the type of the element to find
    * @param a_pElement the element to check (all children will be checked as well)
    * @return the queried element or "nullptr" if it was not found
    */
    irr::gui::IGUIElement* findElementByIdAndType(irr::s32 a_iId, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement *a_pElement) {
      if (a_pElement->getID() == a_iId && a_pElement->getType() == a_eType)
        return a_pElement;

      irr::gui::IGUIElement *p = nullptr;

      for (irr::core::list<irr::gui::IGUIElement*>::ConstIterator it = a_pElement->getChildren().begin(); it != a_pElement->getChildren().end(); it++) {
        p = findElementByIdAndType(a_iId, a_eType, *it);
        if (p != nullptr)
          return p;
      }

      return p;
    }

    irr::gui::IGUIElement* findElementByNameAndType(const std::string& a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType, irr::gui::IGUIElement* a_pElement) {
      std::string l_sName = a_pElement->getName();
      if (l_sName == a_sName && a_pElement->getType() == a_eType)
        return a_pElement;

      irr::gui::IGUIElement *p = nullptr;

      for (irr::core::list<irr::gui::IGUIElement*>::ConstIterator it = a_pElement->getChildren().begin(); it != a_pElement->getChildren().end(); it++) {
        p = findElementByNameAndType(a_sName, a_eType, *it);
        if (p != nullptr)
          return p;
      }

      return p;
    }

    IMenuHandler::IMenuHandler(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) :
      m_pDevice (a_pDevice),
      m_pFs     (a_pDevice->getFileSystem    ()),
      m_pGui    (a_pDevice->getGUIEnvironment()),
      m_pDrv    (a_pDevice->getVideoDriver   ()),
      m_pSmgr   (a_pDevice->getSceneManager  ()),
      m_pManager(a_pManager),
      m_pState  (a_pState)
    {
    }

    IMenuHandler::~IMenuHandler() {

    }

    /**
    * Get the menu manager
    * @return the menu manager
    */
    IMenuManager *IMenuHandler::getMenuManager() {
      return m_pManager;
    }

    /**
    * Does this menu handle the state change network message itself?
    * Might me necessary if some data needs to be written to a global
    * @return true it this menu handle the state change network message itself
    */
    bool IMenuHandler::handlesNetworkStateChange() {
      return false;
    }

    void IMenuHandler::changeZLayer(int a_iZLayer) {
      irr::SEvent l_cEvent;

      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventChangeZLayer;
      l_cEvent.UserEvent.UserData2 = a_iZLayer;

      m_pDevice->postEventFromUser(l_cEvent);
    }

    /**
    * This static method creates the requested menu handler
    * @param a_sName string identifier of the menu
    * @param a_pDevice the Irrlicht device
    * @param a_pManager the menu manager
    * @return "true" on success, "false" otherwise
    */
    IMenuHandler *IMenuHandler::createMenu(const std::string& a_sName, irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) {
      IMenuHandler *l_pRet = nullptr;

      printf("Create Menu \"%s\"\n", a_sName.c_str());

      a_pManager->beforeChangeMenu();

      if (a_sName == "menu_main") {
        l_pRet = a_pManager->changeMenu(createMenuMain(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_settings") {
        l_pRet = a_pManager->changeMenu(createMenuSettings(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_profiles") {
        l_pRet = a_pManager->changeMenu(createMenuProfiles(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_setupgame") {
        l_pRet = a_pManager->changeMenu(createMenuSetupGame(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_selecttrack") {
        l_pRet = a_pManager->changeMenu(createMenuSelectTrack(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_raceresult") {
        l_pRet = a_pManager->changeMenu(createMenuRaceResult(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_standings") {
        l_pRet = a_pManager->changeMenu(createMenuStandings(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_startserver") {
        l_pRet = a_pManager->changeMenu(createMenuStartServer(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_joinserver") {
        l_pRet = a_pManager->changeMenu(createMenuJoinServer(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_message") {
        l_pRet = a_pManager->changeMenu(createMenuMessage(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_netlobby") {
        l_pRet = a_pManager->changeMenu(createMenuNetLobby(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_searchserver") {
        l_pRet = a_pManager->changeMenu(createMenuSearchServer(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_fillgrid") {
        l_pRet = a_pManager->changeMenu(createMenuFillGrid(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_finalresult") {
        l_pRet = a_pManager->changeMenu(createMenuFinalResult(a_pDevice, a_pManager, a_pState));
      }

      if (l_pRet == nullptr)
        printf("No menu handler for menu \"%s\" found.\n", a_sName.c_str());
      else {
        a_pState->menuChanged(a_sName);
      }

      return l_pRet;
    }
  }
}