// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <gameclasses/SPlayer.h>
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

        irr::core::recti m_cViewport;   /**< The viewport */

      public:
        CMenuFinalResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_sNewState(""), 
          m_pServer  (a_pState->getGlobal()->getGameServer()),
          m_pClient  (a_pState->getGlobal()->getGameClient())
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_finalresult.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/tournament_result.xml");
          irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->addCameraSceneNode();
          l_pCam->setPosition(irr::core::vector3df(0.0f, 0.0f, -20.0f));
          l_pCam->setTarget(irr::core::vector3df(0.0f, 0.0f, 0.0f));

#ifdef _OPENGL_ES
          helpers::adjustNodeMaterials(m_pSmgr->getRootSceneNode());
#endif
          CGlobal::getInstance()->stopGameClient();

          irr::scene::IMeshSceneNode *l_pTrophy = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("trophy"));
          irr::scene::IMeshSceneNode *l_pSilver = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("silver_name"));
          irr::scene::IMeshSceneNode *l_pBronze = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("bronze_name"));

          irr::scene::IMeshSceneNode *l_pMarble1 = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("marble1"));
          irr::scene::IMeshSceneNode *l_pMarble2 = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("marble2"));
          irr::scene::IMeshSceneNode *l_pMarble3 = reinterpret_cast<irr::scene::IMeshSceneNode *>(m_pSmgr->getSceneNodeFromName("marble3"));

          irr::scene::ISceneNode *l_pSilverRoot = m_pSmgr->getSceneNodeFromName("silver_root");
          irr::scene::ISceneNode *l_pBronzeRoot = m_pSmgr->getSceneNodeFromName("bronze_root");

          irr::scene::ISceneNode *l_pRank2 = m_pSmgr->getSceneNodeFromName("second_marble");
          irr::scene::ISceneNode *l_pRank3 = m_pSmgr->getSceneNodeFromName("third_marble" );

          irr::gui::IGUIFont *l_pFontBig   = CGlobal::getInstance()->getFontBySize(104);
          irr::gui::IGUIFont *l_pFontSmall = CGlobal::getInstance()->getFontBySize(72);

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

          gameclasses::SPlayer *l_pPodium[3] = { nullptr, nullptr, nullptr };
          int l_iIdx = 0;

          for (auto l_cStanding : l_pTournament->m_vStandings) {
            for (auto l_pPlr : l_pTournament->m_vPlayers) {
              if (l_cStanding.m_iPlayer == l_pPlr->m_iPlayer) {
                l_pPodium[l_iIdx] = l_pPlr;
                break;
              }
            }
            l_iIdx++;

            if (l_iIdx >= 3)
              break;
          }

          if (l_pFontBig != nullptr && l_pFontSmall != nullptr) {
            if (l_pPodium[0] != nullptr && l_pTrophy != nullptr) {
              std::string l_sName = "";
              std::string l_sBot  = "";

              helpers::splitStringNameBot(l_pPodium[0]->m_sName, l_sName, l_sBot);

              helpers::fitString(helpers::s2ws(l_sName), l_pFontBig, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontBig->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pTrophy->getMaterialCount() > 1)
                l_pTrophy->getMaterial(1).setTexture(0, p);

              if (l_pMarble1 != nullptr) {
                l_pMarble1->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(l_pPodium[0]->m_sTexture));
              }
            }

            if (l_pPodium[1] && l_pSilver != nullptr) {
              std::string l_sName = "";
              std::string l_sBot  = "";

              helpers::splitStringNameBot(l_pPodium[1]->m_sName, l_sName, l_sBot);

              helpers::fitString(helpers::s2ws(l_sName), l_pFontSmall, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontSmall->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pSilver->getMaterialCount() > 0)
                l_pSilver->getMaterial(0).setTexture(0, p);

              if (l_pMarble2 != nullptr) {
                l_pMarble2->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(l_pPodium[1]->m_sTexture));
              }
            }
            else {
              if (l_pSilverRoot != nullptr) l_pSilverRoot->setVisible(false);
              if (l_pRank2      != nullptr) l_pRank2     ->setVisible(false);
            }

            if (l_pPodium[2] != nullptr && l_pBronze != nullptr) {
              std::string l_sName = "";
              std::string l_sBot  = "";

              helpers::splitStringNameBot(l_pPodium[2]->m_sName, l_sName, l_sBot);

              helpers::fitString(helpers::s2ws(l_sName), l_pFontSmall, irr::core::dimension2du(1024, 128));

              irr::video::ITexture *p = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(1024, 128), "rtt_winner");
              m_pDrv->setRenderTarget(p, true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
              l_pFontSmall->draw(helpers::s2ws(l_sName).c_str(), irr::core::recti(0, 0, 1024, 128), irr::video::SColor(0xFF, 0, 0, 0), true, true);
              m_pDrv->setRenderTarget(nullptr, true, true);

              if (l_pBronze->getMaterialCount() > 0)
                l_pBronze->getMaterial(0).setTexture(0, p);

              if (l_pMarble3 != nullptr) {
                l_pMarble3->getMaterial(0).setTexture(0, CGlobal::getInstance()->createTexture(l_pPodium[2]->m_sTexture));
              }
            }
            else {
              if (l_pBronzeRoot != nullptr) l_pBronzeRoot->setVisible(false);
              if (l_pRank3      != nullptr) l_pRank3     ->setVisible(false);
            }
          }

          CGlobal *l_pGlobal = CGlobal::getInstance();

          data::SSettings l_cSettings = l_pGlobal->getSettingData();

          irr::u32 l_iAmbient = 196;

          switch (l_cSettings.m_iAmbient) {
            case 0: l_iAmbient = 32; break;
            case 1: l_iAmbient = 64; break;
            case 2: l_iAmbient = 96; break;
            case 3: l_iAmbient = 128; break;
            case 4: l_iAmbient = 160; break;
          }

          irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();

          m_cViewport = irr::core::recti(irr::core::position2di(0, 0), l_cScreen);

          printf("Ready.\n");
        }

        virtual bool run() override {
          m_pDrv->beginScene();
          m_pSmgr->drawAll();
          m_pGui->drawAll();
          m_pDrv->endScene();

          return true;
        }

        virtual ~CMenuFinalResult() {
          if (CGlobal::getInstance() != nullptr)
            CGlobal::getInstance()->stopGameServer();
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
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