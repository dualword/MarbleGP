// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuFinalResult
    * @author Christian Keimel
    * This menu displays a message, e.g. an error
    */
    class CMenuFinalResult : public IMenuHandler {
      private:
        std::string m_sNewState;  /**< The next state important if we are a network game server */

        network::CGameServer *m_pServer;  /**< The game server */
        network::CGameClient *m_pClient;  /**< The game client */

      public:
        CMenuFinalResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_sNewState(""), 
          m_pServer(a_pState->getGlobal()->getGameServer()),
          m_pClient(a_pState->getGlobal()->getGameClient())
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_finalresult.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pState->setZLayer(1);

          CGlobal::getInstance()->stopGameClient();
        }

        virtual ~CMenuFinalResult() {
          if (CGlobal::getInstance() != nullptr)
            CGlobal::getInstance()->stopGameServer();
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }


          return l_bRet;
        }
    };

    IMenuHandler *createMenuFinalResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuFinalResult(a_pDevice, a_pManager, a_pState);
    }
  }
}