// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CDataHelpers.h>
#include <gameclasses/SPlayer.h>
#include <network/CGameServer.h>
#include <helpers/CMenuLoader.h>
#include <gui/CGuiTrackSelect.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <gui/CMenuButton.h>
#include <gui/CSelector.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <Defines.h>
#include <CGlobal.h>
#include <algorithm>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSelectTrack
    * @author Christian Keimel
    * This is the track selection menu
    */
    class CMenuSelectTrack : public IMenuHandler {
      private:
        std::vector<std::tuple<int, std::string, std::string>> m_vTracks;         /**< The name of the installed tracks */
        
        gui::CMenuButton *m_pLeft;    /**< The "Left" button */
        gui::CMenuButton *m_pRight;   /**< The "Right" button */
        gui::CMenuButton *m_pOk;      /**< The "OK" button */

        gui::CGuiTrackSelect *m_pTrackList; /**< The track list for selection */

        int m_iClientState;   /**< Is a server active and we are waiting for a "global data set" response? */
        int m_iLaps;          /**< The currently selected number of laps */

        std::string m_sTrack;   /**< The currently selected racetrack */

        bool m_bRaceInfoSent;   /**< Was the information about the currently selected race transferred? */

        network::CGameServer *m_pServer;  /**< The game server */
        network::CGameClient *m_pClient;  /**< The game client */

        std::map<std::string, std::tuple<std::string, int>> m_mTracks;   /**< Map with the track names (key == track identifier) */

        /**
        * Fill the vector of track names
        */
        void fillTrackNames() {
          printf("Bla");

          std::map<std::string, std::tuple<std::string, int>> l_mTracks = helpers::getTrackNameMap();

          for (auto l_tTrack: l_mTracks)
            m_vTracks.push_back(std::make_tuple(std::get<1>(l_tTrack.second), l_tTrack.first, std::get<0>(l_tTrack.second)));
        }

        void updateTrackFilter() {
          std::map<std::string, int> l_mCategoryScore = {
            { "rookie" , 0 },
            { "flat"   , 1 },
            { "jump"   , 2 },
            { "loop"   , 3 },
            { "steep"  , 4 },
            { "complex", 5 },
            { "moving" , 6 }
          };

          if (m_pTrackList != nullptr) {
            m_pTrackList->setCategoryRanking(l_mCategoryScore);

            std::vector<gui::CGuiImageList::SListImage> l_vTracks;

            for (std::vector<std::tuple<int, std::string, std::string>>::iterator it = m_vTracks.begin(); it != m_vTracks.end(); it++) {
              std::string l_sPath = "data/levels/" + std::get<1>(*it) + "/thumbnail.png";

              if (!m_pFs->existFile(l_sPath.c_str()))
                l_sPath = "data/images/no_image.png";

              l_vTracks.push_back(gui::CGuiImageList::SListImage(
                l_sPath,
                std::get<2>(*it),
                std::get<1>(*it),
                std::get<0>(*it)
              ));
            }

            m_pTrackList->setImageList(l_vTracks);
            m_pTrackList->setSelected(m_pState->getGlobal()->getSetting("singlerace_track"), true);
            m_sTrack = m_pTrackList->getSelectedData();
            printf("Track %s\n", m_sTrack.c_str());

            std::string l_sData = m_pTrackList->getSelectedData();

            for (std::vector<std::tuple<int, std::string, std::string>>::iterator it = m_vTracks.begin(); it != m_vTracks.end(); it++) {
              if (std::get<1>(*it) == l_sData) {
                if (m_pOk != nullptr)
                  m_pOk->setVisible(true);

                break;
              }
            }
          }
        }

      public:
        CMenuSelectTrack(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler   (a_pDevice, a_pManager, a_pState), 
          m_pLeft        (nullptr), 
          m_pRight       (nullptr),
          m_pOk          (nullptr),
          m_pTrackList   (nullptr),
          m_iClientState (0),
          m_pServer      (a_pState->getGlobal()->getGameServer()),
          m_pClient      (a_pState->getGlobal()->getGameClient()),
          m_bRaceInfoSent(false)
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_selecttrack.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pTrackList = reinterpret_cast<gui::CGuiTrackSelect *>(helpers::findElementByNameAndType("TrackList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_TrackSelectId, m_pGui->getRootGUIElement()));

          m_pLeft  = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("btn_left" , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pRight = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("btn_right", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pOk    = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok"       , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

          if (m_pOk != nullptr)
            m_pOk->setVisible(false);

          fillTrackNames();
          updateTrackFilter();

          gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (l_pLaps != nullptr) {
            std::string l_sLaps = m_pState->getGlobal()->getSetting("singlerace_laps");
            l_pLaps->setSelected(std::atoi(l_sLaps.c_str()) - 1);
            m_iLaps = l_pLaps->getSelected() + 1;
            printf("%i laps selected.\n", m_iLaps);
          }

          m_mTracks = helpers::getTrackNameMap();
        }

        virtual ~CMenuSelectTrack() {
          printf("CMenuSelectTrack Deleted.\n");
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sCaller == "cancel") {
                gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

                std::string l_sNext = (l_pTournament->m_vRaces.size() > 0 && l_pTournament->m_vPlayers.size() > 1) ? "menu_finalresult" : "menu_main";

                if (m_pServer != nullptr) {
                  m_pServer->changeState(l_sNext);
                  createMenu(l_sNext, m_pDevice, m_pManager, m_pState);
                }
                else {
                  m_pManager->clearMenuStack();
                  createMenu(l_sNext, m_pDevice, m_pManager, m_pState);
                }
              }
              else if (l_sCaller == "ok" && (m_pOk == nullptr || m_pOk->isVisible())) {
                if (m_pTrackList != nullptr) {
                  printf("Track selected: \"%s\"\n", m_pTrackList->getSelectedData().c_str());

                  int l_iLaps = 2;

                  gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                  if (l_pLaps)
                    l_iLaps = l_pLaps->getSelected() + 1;

                  printf("Laps: %i\n", l_iLaps);

                  m_pManager->pushToMenuStack("menu_selecttrack");

                  m_pState->getGlobal()->setSetting("track", m_pTrackList->getSelectedData());
                  m_pState->getGlobal()->setSetting("laps" , std::to_string(l_iLaps));

                  m_pState->getGlobal()->setSetting("singlerace_track", m_pTrackList->getSelectedData());
                  m_pState->getGlobal()->setSetting("singlerace_laps" , std::to_string(l_iLaps));

                  gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

                  if (l_pTournament->m_vPlayers.size() > 1) {
                    m_pManager->pushToMenuStack("menu_standings"  );
                    m_pManager->pushToMenuStack("menu_raceresult" );
                  }

                  l_pTournament->m_vRaces.push_back(new gameclasses::SRace(
                    m_pTrackList->getSelectedData(),
                    m_pState->getGlobal()->getTrackName(m_pTrackList->getSelectedData()),
                    "Free Racing Race #" + std::to_string(l_pTournament->m_vRaces.size() + 1),
                    l_iLaps, 
                    l_pTournament)
                  );
                  l_pTournament->startRace();

                  m_pState->getGlobal()->initNextRaceScreen();

                  platform::saveSettings();
                  if (m_pServer != nullptr) {
                    m_pServer->sendGlobalData("gamedata", l_pTournament->getRace()->serialize());
                    m_iClientState = 1;
                  }
                  else {
                    m_pState->getGlobal()->initNextRaceScreen();
                    m_pState->setState(state::enState::Game);
                  }
                }
              }
              else printf("Button clicked: \"%s\"\n", l_sCaller.c_str());
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
              if (l_sCaller == "nolaps") {
                gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller);
                m_iLaps = l_pLaps->getSelected() + 1;
                m_bRaceInfoSent = false;
              }
              else printf("Scrollbar changed: \"%s\"\n", l_sCaller.c_str());
            }
          }
          else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
            if (m_pTrackList != nullptr && 
              (
                (a_cEvent.UserEvent.UserData1 == c_iEventImageSelected   && a_cEvent.UserEvent.UserData2 == c_iEventImageSelected  ) ||
                (a_cEvent.UserEvent.UserData1 == c_iEventImagePosChanged && a_cEvent.UserEvent.UserData2 == c_iEventImagePosChanged)
              )
            ) {
              m_sTrack = m_pTrackList->getSelectedData();
              m_bRaceInfoSent = false;
              if (m_pOk != nullptr)
                m_pOk->setVisible(true);
            }
          }

          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual bool run() override { 
          if (m_pServer != nullptr) {
            if (m_iClientState == 0) {
              if (!m_bRaceInfoSent) {
                if (m_pServer->allClientsAreInState("menu_netlobby")) {
                  std::string l_sTrack = m_sTrack;

                  if (m_mTracks.find(l_sTrack) != m_mTracks.end()) {
                    l_sTrack = std::get<0>(m_mTracks[l_sTrack]);
                  }

                  gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();
                  std::string l_sInfo = "Free Racing / Race " + std::to_string(l_pTournament->m_vRaces.size() + 1) + " (" + std::to_string(m_iLaps) + " Lap" + (m_iLaps == 1 ? "" : "s") + ")";

                  messages::CUpdateRaceInfo l_cMsg(m_sTrack, l_sInfo);
                  m_pServer->getInputQueue()->postMessage(&l_cMsg);
                  m_bRaceInfoSent = true;
                }
              }
            }
            else {
              CGlobal *l_pGlobal = CGlobal::getInstance();

              m_pDrv->setRenderTarget(nullptr, false, false);

              l_pGlobal->progressSetCurrentRange(L"Waiting for clients...", 0, 100, 100);
              l_pGlobal->onProgress();

              if (m_iClientState == 1) {
                if (m_pServer->allClientsAreInState("gamedata")) {
                  m_iClientState = 2;

                  m_pServer->changeState("state_game");
                  printf("Ready to go, start game.\n");
                }
              }
              else if (m_iClientState == 2) {
                if (m_pServer->allClientsAreInState("state_game")) {
                  m_pState->setState(state::enState::Game);
                }
              }
            }
          }

          return m_iClientState != 0;
        }
    };

    IMenuHandler* createMenuSelectTrack(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSelectTrack(a_pDevice, a_pManager, a_pState);
    }
  }
}