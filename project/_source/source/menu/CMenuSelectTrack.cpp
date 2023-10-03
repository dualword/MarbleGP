// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <helpers/CDataHelpers.h>
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

        data::SChampionship m_cChampionship;    /**< The current championship */

        std::map<std::string, std::string> m_mTracks;   /**< Map with the track names (key == track identifier) */

        /**
        * Fill the vector of track names
        */
        void fillTrackNames() {
          irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile("data/levels/tracks.dat");

          long l_iSize = l_pFile->getSize();
          char *s = new char[l_iSize + 1];
          memset(s, 0, static_cast<size_t>(l_iSize) + 1);
          l_pFile->read(s, l_iSize);
          std::vector<std::string> l_vTracks = helpers::splitString(s, '\n');
          delete []s;

          for (std::vector<std::string>::iterator it = l_vTracks.begin(); it != l_vTracks.end(); it++) {
            std::string l_sTrack = *it;
            if (l_sTrack.substr(l_sTrack.size() - 1) == "\r")
              l_sTrack = l_sTrack.substr(0, l_sTrack.size() - 1);

            std::string l_sFile = "data/levels/" + l_sTrack + "/track.xml";

            if (m_pFs->existFile(l_sFile.c_str())) {
              std::string l_sXml = "data/levels/" + l_sTrack + "/info.xml", l_sName = l_sTrack;
              int l_iPos = 9999;

              if (m_pFs->existFile(l_sXml.c_str())) {
                irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8(l_sXml.c_str());
                if (l_pXml) {
                  bool l_bName = false;
                  bool l_bPos  = false;

                  while (l_pXml->read()) {
                    std::string l_sNode = l_pXml->getNodeName();

                    if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                      if (l_sNode == "name")
                        l_bName = true;
                      else if (l_sNode == "position")
                        l_bPos = true;
                    }
                    else if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                      if (l_bName)
                        l_sName = l_pXml->getNodeData();
                      else if (l_bPos)
                        l_iPos = std::atoi(l_pXml->getNodeData());
                    }
                    else if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
                      if (l_sNode == "name")
                        l_bName = false;
                    }
                  }
                  l_pXml->drop();
                }
              }

              m_vTracks.push_back(std::make_tuple(l_iPos, l_sTrack, l_sName));
            }
          }

          l_pFile->drop();
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
            m_pTrackList->setSelected(m_pState->getGlobal()->getSetting("track"), true);
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

          m_pTrackList = reinterpret_cast<gui::CGuiTrackSelect *>(findElementByNameAndType("TrackList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_TrackSelectId, m_pGui->getRootGUIElement()));

          m_pLeft  = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("btn_left" , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pRight = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("btn_right", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pOk    = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("ok"       , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

          if (m_pOk != nullptr)
            m_pOk->setVisible(false);

          fillTrackNames();
          updateTrackFilter();

          gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (l_pLaps != nullptr) {
            std::string l_sLaps = m_pState->getGlobal()->getSetting("laps");
            l_pLaps->setSelected(std::atoi(l_sLaps.c_str()) - 1);
            m_iLaps = l_pLaps->getSelected() + 1;
            printf("%i laps selected.\n", m_iLaps);
          }

          m_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));
          std::vector<data::SChampionshipPlayer> l_vStanding = m_cChampionship.getStandings();

          printf("\n********************************");
          printf(  "**** Championship Standings ****\n");
          printf("\n********************************\n");

          printf("   |      Name       | Pts |");

          for (int i = 0; i < m_cChampionship.m_vPlayers.size(); i++)
            printf("%2i |", i + 1);

          printf(" Rs | St | Bf | Fl | DNF |\n");

          printf("---+-----------------+-----+");

          for (int i = 0; i < m_cChampionship.m_vPlayers.size(); i++)
            printf("---+");

          printf("----+----+----+----+-----+\n");

          int l_iPos = 1;
          for (std::vector<data::SChampionshipPlayer>::iterator it = l_vStanding.begin(); it != l_vStanding.end(); it++) {
            std::string s = (*it).m_sName;

            if (s.length() > 15)
              s = s.substr(s.length() - 15);

            while (s.length() < 15)
              s += " ";

            printf("%2i | %s | %3i |", l_iPos, s.c_str(), (*it).m_iPoints);

            for (int i = 0; i < m_cChampionship.m_vPlayers.size(); i++)
              if ((*it).m_aResult[i] != 0)
                printf("%2i |", (*it).m_aResult[i]);
              else
                printf("   |");

            printf(" %2i | %2i | %2i | %2i | %3i |\n", (*it).m_iRespawn, (*it).m_iStunned, (*it).m_iBestFinish, (*it).m_iFastestLaps, (*it).m_iDidNotFinish);

            l_iPos++;
          }

          printf("\n********************************\n");

          printf("Path: \"%s\"\n", helpers::ws2s(platform::portableGetDataPath()).c_str());

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
                data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

                std::string l_sNext = (l_cChampionship.m_vRaces.size() > 0 && l_cChampionship.m_vPlayers.size() > 1) ? "menu_finalresult" : "menu_main";

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

                  gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                  if (l_pLaps)
                    l_iLaps = l_pLaps->getSelected() + 1;

                  printf("Laps: %i\n", l_iLaps);

                  data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

                  m_pManager->pushToMenuStack("menu_selecttrack");

                  if (l_cChampionship.m_vPlayers.size() > 1) {
                    m_pManager->pushToMenuStack("menu_standings"  );
                    m_pManager->pushToMenuStack("menu_raceresult" );
                  }

                  m_pState->getGlobal()->setSetting("track", m_pTrackList->getSelectedData());
                  m_pState->getGlobal()->setSetting("laps" , std::to_string(l_iLaps));

                  helpers::prepareNextRace(m_pTrackList->getSelectedData(), l_iLaps);

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
                    l_sTrack = m_mTracks[l_sTrack];
                  }

                  std::string l_sInfo = "Race " + std::to_string(m_cChampionship.m_vRaces.size() + 1) + ": " + l_sTrack + ", " + std::to_string(m_iLaps) + " Lap" + (m_iLaps == 1 ? "" : "s");

                  messages::CUpdateRaceInfo l_cMsg(m_sTrack, l_sInfo);
                  m_pServer->getInputQueue()->postMessage(&l_cMsg);
                  m_bRaceInfoSent = true;
                }
              }
            }
            else if (m_iClientState == 1) {
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

    IMenuHandler* createMenuSelectTrack(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSelectTrack(a_pDevice, a_pManager, a_pState);
    }
  }
}