// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameServer.h>
#include <platform/CPlatform.h>
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
        int m_iClientState;  /**< Is a server active and we are waiting for a "global data set" responsw? */

        network::CGameServer *m_pServer;  /**< The game server */
        network::CGameClient *m_pClient;  /**< The game client */

      public:
        CMenuNextRace(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_iClientState(0),
          m_pServer     (a_pState->getGlobal()->getGameServer()),
          m_pClient     (a_pState->getGlobal()->getGameClient())
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_nextrace.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          std::string l_sHeadline = m_pState->getGlobal()->getGlobal("message_headline");
          if (l_sHeadline != "") {
            irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_headline", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(l_sHeadline).c_str());

            m_pState->getGlobal()->setGlobal("message_headline", "");
          }

          std::string l_sText = m_pState->getGlobal()->getGlobal("message_text");
          if (l_sText != "") {
            irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("label_message", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setText(helpers::s2ws(l_sText).c_str());

            m_pState->getGlobal()->setGlobal("message_text", "");
          }
        }

        virtual ~CMenuNextRace() {
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;


          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "ok") {
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

                  data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

                  data::SGameData l_cData(data::SGameData::enType::Local, l_sTrack, l_iLaps, l_cChampionship.m_iClass);
                  m_pState->getGlobal()->setGlobal("gamedata", l_cData.serialize());
                  m_pState->getGlobal()->initNextRaceScreen();

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
            }
          }


          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override { 
          if (m_pServer != nullptr) {
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

          return false;
        }
    };

    IMenuHandler *createMenuNextRace(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuNextRace(a_pDevice, a_pManager, a_pState);
    }
  }
}