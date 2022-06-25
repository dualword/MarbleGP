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
          m_pSmgr->loadScene("data/scenes/tournament_result.xml");
          irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->addCameraSceneNode();
          l_pCam->setPosition(irr::core::vector3df(0.0f, 0.0f, -20.0f));
          l_pCam->setTarget(irr::core::vector3df(0.0f, 0.0f, 0.0f));

          m_pState->setZLayer(1);

          CGlobal::getInstance()->stopGameClient();

          irr::scene::IMeshSceneNode *l_pTrophy = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("trophy"));
          irr::scene::IMeshSceneNode *l_pSilver = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("silver_name"));
          irr::scene::IMeshSceneNode *l_pBronze = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("bronze_name"));

          irr::scene::ISceneNode *l_pSilverRoot = m_pSmgr->getSceneNodeFromName("silver_root");
          irr::scene::ISceneNode *l_pBronzeRoot = m_pSmgr->getSceneNodeFromName("bronze_root");

          data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

          std::vector<data::SChampionshipPlayer> l_vStandings = l_cChampionship.getStandings();

          irr::gui::IGUIFont *l_pFontBig   = CGlobal::getInstance()->getFontBySize(104);
          irr::gui::IGUIFont *l_pFontSmall = CGlobal::getInstance()->getFontBySize(72);

          if (l_pFontBig != nullptr && l_pFontSmall != nullptr) {
            if (l_vStandings.size() > 0 && l_pTrophy != nullptr) {
              data::SChampionshipPlayer l_cPlayer = l_vStandings[0];
              std::string l_sName = l_cPlayer.m_sName;
              helpers::fitString(helpers::s2ws(l_sName), l_pFontBig, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontBig->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pTrophy->getMaterialCount() > 1)
                l_pTrophy->getMaterial(1).setTexture(0, p);
            }

            if (l_vStandings.size() > 1 && l_pSilver != nullptr) {
              data::SChampionshipPlayer l_cPlayer = l_vStandings[1];
              std::string l_sName = l_cPlayer.m_sName;
              helpers::fitString(helpers::s2ws(l_sName), l_pFontSmall, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontSmall->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pSilver->getMaterialCount() > 0)
                l_pSilver->getMaterial(0).setTexture(0, p);
            }
            else if (l_pSilverRoot != nullptr) l_pSilverRoot->setVisible(false);

            if (l_vStandings.size() > 2 && l_pBronze != nullptr) {
              data::SChampionshipPlayer l_cPlayer = l_vStandings[2];
              std::string l_sName = l_cPlayer.m_sName;
              helpers::fitString(helpers::s2ws(l_sName), l_pFontSmall, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontSmall->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pBronze->getMaterialCount() > 0)
                l_pBronze->getMaterial(0).setTexture(0, p);
            }
            else if (l_pBronzeRoot != nullptr) l_pBronzeRoot->setVisible(false);
          }

          printf("Ready.\n");
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