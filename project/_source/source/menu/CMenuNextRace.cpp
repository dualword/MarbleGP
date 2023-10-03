// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameServer.h>
#include <platform/CPlatform.h>
#include <helpers/CAutoMenu.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuNextRace
    * @author Christian Keimel
    * Start the next race of a cup
    */
    class CMenuNextRace : public IMenuHandler {
      private:
        int m_iClientState;  /**< Is a server active and we are waiting for a "global data set" response? */

        network::CGameServer *m_pServer;  /**< The game server */
        network::CGameClient *m_pClient;  /**< The game client */

        helpers::CAutoMenu *m_pAuto;    /**< Auto menu (for debugging) */

        std::string m_sTrack;   /**< The next race track */
        std::string m_sInfo;    /**< Information about the next race */

        bool m_bInfoSent;     /**< The information about the next race was sent to the clients (if this is a network game) */

      public:
        CMenuNextRace(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_iClientState(0),
          m_pServer     (a_pState->getGlobal()->getGameServer()),
          m_pClient     (a_pState->getGlobal()->getGameClient()),
          m_pAuto       (nullptr),
          m_bInfoSent   (false),
          m_sTrack      (""),
          m_sInfo       ("")
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_nextrace.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          data::SMarbleGpCup l_cCup = data::SMarbleGpCup(m_pState->getGlobal()->getGlobal("current_cup"));

          std::map<std::string, std::string> l_mTrackNames = helpers::getTrackNameMap();

          irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_race", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr && l_cCup.m_vRaces.size() > 0) {
            std::string l_sTrack = std::get<0>(*l_cCup.m_vRaces.begin());

            if (l_mTrackNames.find(l_sTrack) != l_mTrackNames.end())
              l_sTrack = l_mTrackNames[l_sTrack];

            p->setText(helpers::s2ws(l_sTrack).c_str());
          }

          p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_details", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
          if (p != nullptr) {
            int l_iLaps = l_cCup.m_vRaces.size() > 0 ? std::get<1>(*l_cCup.m_vRaces.begin()) : 0;

            std::wstring s = 
              L"\"" + helpers::s2ws(l_cCup.m_sName) +
              L"\" Race " + 
              std::to_wstring(l_cCup.m_iRaceCount - l_cCup.m_vRaces.size() + 1) + 
              L" of " + 
              std::to_wstring(l_cCup.m_iRaceCount) +
              L" (" + 
              std::to_wstring(l_iLaps) +
              L" Lap";

            if (l_iLaps != 1)
              s += L"s)";
            else
              s += L")";

            p->setText(s.c_str());

            m_sInfo = helpers::ws2s(s);
          }

          irr::gui::IGUIImage *l_pThumbnail = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("Thumbnail", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
          if (l_pThumbnail != nullptr) {
            m_sTrack = std::get<0>(*l_cCup.m_vRaces.begin());
            std::string l_sImg = "data/levels/" + m_sTrack + "/thumbnail.png";
            if (m_pFs->existFile(l_sImg.c_str()))
              l_pThumbnail->setImage(m_pDrv->getTexture(l_sImg.c_str()));
            else
              l_pThumbnail->setImage(m_pDrv->getTexture("data/images/no_image.png"));
          }

          m_pAuto = new helpers::CAutoMenu(m_pDevice,  this);
        }

        virtual ~CMenuNextRace() {
          if (m_pAuto != nullptr) {
            delete m_pAuto;
            m_pAuto = nullptr;
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
                if (m_pAuto != nullptr) {
                  delete m_pAuto;
                  m_pAuto = nullptr;
                }

                l_bRet = true;
                
                data::SMarbleGpCup l_cCup = data::SMarbleGpCup(m_pState->getGlobal()->getGlobal("current_cup"));

                if (l_cCup.m_vRaces.size() > 0) {
                  std::string l_sTrack = std::get<0>(*l_cCup.m_vRaces.begin());
                  int         l_iLaps  = std::get<1>(*l_cCup.m_vRaces.begin());

                  l_cCup.m_vRaces.erase(l_cCup.m_vRaces.begin());

                  if (l_cCup.m_vRaces.size() > 0) {
                    m_pManager->pushToMenuStack("menu_nextrace");
                    m_pState->getGlobal()->setGlobal("current_cup", l_cCup.serialize());
                  }
                  else {
                    m_pManager->pushToMenuStack("menu_finalresult");
                    m_pState->getGlobal()->setGlobal("current_cup", "");
                  }

                  m_pManager->pushToMenuStack("menu_standings"  );
                  m_pManager->pushToMenuStack("menu_raceresult" );

                  m_pState->getGlobal()->setSetting("track", l_sTrack);
                  m_pState->getGlobal()->setSetting("laps" , std::to_string(l_iLaps));

                  helpers::prepareNextRace(l_sTrack, l_iLaps);

                  platform::saveSettings();
                  if (m_pServer != nullptr) {
                    m_pServer->sendGlobalData("gamedata");
                    m_iClientState = 1;
                  }
                  else {
                    m_pState->getGlobal()->initNextRaceScreen();
                    m_pState->setState(state::enState::Game);
                  }
                }
              }
              else if (l_sSender == "cancel") {
                m_pManager->clearMenuStack();
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
                l_bRet = true;
              }
            }
          }


          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override { 
          if (m_pAuto != nullptr)
            m_pAuto->process();

          if (m_pServer != nullptr) {
            if (!m_bInfoSent) {
              if (m_pServer->allClientsAreInState("menu_netlobby")) {
                printf("Send race information to clients.\n");
                messages::CUpdateRaceInfo l_cMsg = messages::CUpdateRaceInfo(m_sTrack, m_sInfo);
                m_pServer->getInputQueue()->postMessage(&l_cMsg);
                m_bInfoSent = true;
              }
            }
            else {
              if (m_iClientState == 1) {
                if (m_pServer->allClientsAreInState("gamedata")) {
                  m_iClientState = 2;

                  m_pServer->changeState("state_game");
                  printf("Ready to go, start game.\n");
                }

                m_pState->getGlobal()->drawNextRaceScreen(1.0f);
              }
              else if (m_iClientState == 2) {
                m_pState->getGlobal()->drawNextRaceScreen(1.0f);

                if (m_pServer->allClientsAreInState("state_game")) {
                  m_pState->setState(state::enState::Game);
                }
              }
            }
          }

          return false;
        }
    };

    IMenuHandler *createMenuNextRace(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNextRace(a_pDevice, a_pManager, a_pState);
    }
  }
}