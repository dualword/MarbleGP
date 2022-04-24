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
      if (a_sName == "menu_main") {
        return a_pManager->changeMenu(createMenuMain(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_settings") {
        return a_pManager->changeMenu(createMenuSettings(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_profiles") {
        return a_pManager->changeMenu(createMenuProfiles(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_setupgame") {
        return a_pManager->changeMenu(createMenuSetupGame(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_selecttrack") {
        return a_pManager->changeMenu(createMenuSelectTrack(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_raceresult") {
        return a_pManager->changeMenu(createMenuRaceResult(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_standings") {
        return a_pManager->changeMenu(createMenuStandings(a_pDevice, a_pManager, a_pState));
      }

      printf("No menu handler for menu \"%s\" found.\n", a_sName.c_str());

      return nullptr;
    }
  }
}