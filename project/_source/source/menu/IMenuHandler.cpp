// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <menu/datahandlers/IMenuDataHandler.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
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

    IMenuHandler::IMenuHandler(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) :
      m_pDevice     (a_pDevice),
      m_pFs         (a_pDevice->getFileSystem    ()),
      m_pGui        (a_pDevice->getGUIEnvironment()),
      m_pDrv        (a_pDevice->getVideoDriver   ()),
      m_pSmgr       (a_pDevice->getSceneManager  ()),
      m_pManager    (a_pManager),
      m_pState      (a_pState),
      m_pDataHandler(nullptr)
    {
    }

    IMenuHandler::~IMenuHandler() {
      if (m_pDataHandler != nullptr)
        delete m_pDataHandler;
    }

    /**
    * Find a gui element by name
    * @param a_sName name of the queried element
    * @param a_eType type of the element, the type doesn't matter if the type is "irr::gui::EGUIET_COUNT"
    * @return the element, nullptr if no match was found
    */
    irr::gui::IGUIElement* IMenuHandler::findElement(const std::string& a_sName, irr::gui::EGUI_ELEMENT_TYPE a_eType) {
      if (a_eType == irr::gui::EGUIET_COUNT)
        return helpers::findElementByName(a_sName, m_pGui->getRootGUIElement());
      else
        return helpers::findElementByNameAndType(a_sName, a_eType, m_pGui->getRootGUIElement());
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

    /**
    * This method is called every frame after "scenemanager::drawall" is called
    * @return true if drawing was done by the menu, false otherwise
    */
    bool IMenuHandler::run() {
      if (m_pDataHandler != nullptr)
        m_pDataHandler->runDataHandler();

      return false;
    }

    bool IMenuHandler::OnEvent(const irr::SEvent& a_cEvent) {
      return m_pDataHandler != nullptr ? m_pDataHandler->handleIrrlichtEvent(a_cEvent) : false;
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
      else if (a_sName == "menu_credits") {
        l_pRet = a_pManager->changeMenu(createMenuCredits(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_selectcup") {
        l_pRet = a_pManager->changeMenu(createMenuSelectCup(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_remote") {
        l_pRet = a_pManager->changeMenu(createMenuRemote(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_nextrace") {
        l_pRet = a_pManager->changeMenu(createMenuNextRace(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_newgamewizard") {
        l_pRet = a_pManager->changeMenu(createMenuNewGameWizard(a_pDevice, a_pManager, a_pState));
      }
      else if (a_sName == "menu_intro") {
        l_pRet = a_pManager->changeMenu(createMenuIntroduction(a_pDevice, a_pManager, a_pState));
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