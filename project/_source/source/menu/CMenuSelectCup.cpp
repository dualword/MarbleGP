// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <CGlobal.h>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSelectCup
    * @author Christian Keimel
    * This menu allows the user to select a cup for racing
    */
    class CMenuSelectCup : public IMenuHandler {
      private:
        std::vector<std::tuple<std::string, std::string>> m_vCups;    /**< The available cups */

      public:
        CMenuSelectCup(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_selectcup.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/cups/stock_cups.xml");

          if (l_pXml) {
            while (l_pXml->read()) {
              std::string l_sNode = l_pXml->getNodeName();

              if (l_sNode == "cup") {
                const irr::c8 *l_pCup = l_pXml->getAttributeValue("file");
                if (l_pCup != nullptr) {
                  std::string l_sCup = l_pCup;

                  printf("==> %s\n", l_sCup.c_str());
                }
                else printf("No cup defintion file found.\n");
              }
            }

            l_pXml->drop();
          }
        }

        virtual ~CMenuSelectCup() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }

          return l_bRet;
        }
    };

    IMenuHandler *createMenuSelectCup(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSelectCup(a_pDevice, a_pManager, a_pState);
    }
  }
}