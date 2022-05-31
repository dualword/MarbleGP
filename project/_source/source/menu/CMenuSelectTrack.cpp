// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <network/CGameServer.h>
#include <helpers/CMenuLoader.h>
#include <platform/CPlatform.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <gui/CGuiImageList.h>
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
        
        std::vector<std::tuple<int, std::string, std::string>>::iterator m_itStart;   /**< The current start of the shown tracks */

        gui::CMenuButton *m_pLeft;    /**< The "Left" button */
        gui::CMenuButton *m_pRight;   /**< The "Right" button */
        gui::CMenuButton *m_pOk;      /**< The "OK" button */

        std::string m_sTrackFilter;   /**< The filter string of the tracks (category). The track name has to start with the filter to be visible */

        gui::CGuiImageList *m_pTrackList; /**< The track list for selection */

        int m_iClientState;  /**< Is a server active and we are waiting for a "global data set" responsw? */

        network::CGameServer *m_pServer;  /**< The game server (if running) */
        
        /**
        * Fill the vector of track names
        */
        void fillTrackNames() {
          irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile("data/levels/tracks.dat");

          long l_iSize = l_pFile->getSize();
          char *s = new char[l_iSize + 1];
          memset(s, 0, l_iSize + 1);
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
            { "flat"   , 0 },
            { "jump"   , 1 },
            { "loop"   , 2 },
            { "complex", 3 },
            { "moving" , 4 }
          };

          std::sort(m_vTracks.begin(), m_vTracks.end(), [=](std::tuple<int, std::string, std::string> e1, std::tuple<int, std::string, std::string> e2) {
            std::string l_sCategory1 = std::get<2>(e1);
            std::string l_sCategory2 = std::get<2>(e2);

            std::transform(l_sCategory1.begin(), l_sCategory1.end(), l_sCategory1.begin(), [](wchar_t c){ return std::tolower(c); });
            std::transform(l_sCategory2.begin(), l_sCategory2.end(), l_sCategory2.begin(), [](wchar_t c){ return std::tolower(c); });

            if (l_sCategory1.find_first_of(':') != std::string::npos) l_sCategory1 = l_sCategory1.substr(0, l_sCategory1.find_first_of(':'));
            if (l_sCategory2.find_first_of(':') != std::string::npos) l_sCategory2 = l_sCategory2.substr(0, l_sCategory2.find_first_of(':'));

            if (l_sCategory1 != l_sCategory2) {
              bool b1 = l_mCategoryScore.find(l_sCategory1) != l_mCategoryScore.end();
              bool b2 = l_mCategoryScore.find(l_sCategory2) != l_mCategoryScore.end();

              if (b1 && b2)
                return l_mCategoryScore.find(l_sCategory1)->second < l_mCategoryScore.find(l_sCategory2)->second;
              else if (b1)
                return true;
              else if (b2)
                return false;
              else
                return l_sCategory1 < l_sCategory2;
            }
            else return std::get<0>(e1) < std::get<0>(e2);
          });

          m_itStart = m_vTracks.begin();

          if (m_pTrackList != nullptr) {
            std::vector<gui::CGuiImageList::SListImage> l_vTracks;

            for (std::vector<std::tuple<int, std::string, std::string>>::iterator it = m_vTracks.begin(); it != m_vTracks.end(); it++) {
              std::string l_sPath = "data/levels/" + std::get<1>(*it) + "/thumbnail.png";

              if (!m_pFs->existFile(l_sPath.c_str()))
                l_sPath = "data/images/no_image.png";

              l_vTracks.push_back(gui::CGuiImageList::SListImage(
                l_sPath,
                std::get<2>(*it),
                std::get<1>(*it)
              ));
            }

            m_pTrackList->setImageList(l_vTracks);
            m_pTrackList->setSelected(m_pState->getGlobal()->getSetting("track"));

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
          IMenuHandler  (a_pDevice, a_pManager, a_pState), 
          m_pLeft       (nullptr), 
          m_pRight      (nullptr),
          m_pOk         (nullptr),
          m_sTrackFilter(""),
          m_pTrackList  (nullptr),
          m_iClientState(0),
          m_pServer     (a_pState->getGlobal()->getGameServer())
        {
          m_pGui->clear();

          helpers::loadMenuFromXML("data/menu/menu_selecttrack.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pTrackList = reinterpret_cast<gui::CGuiImageList *>(findElementByNameAndType("TrackList", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ImageListId, m_pGui->getRootGUIElement()));

          m_pLeft  = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("btn_left" , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pRight = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("btn_right", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
          m_pOk    = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("ok"       , (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));

          if (m_pOk != nullptr)
            m_pOk->setVisible(false);

          fillTrackNames();
          updateTrackFilter();

          if (m_vTracks.size() > 0)
            m_itStart = m_vTracks.begin();
          else
            m_itStart = m_vTracks.end();

          gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));
          if (l_pLaps != nullptr) {
            std::string l_sLaps = m_pState->getGlobal()->getSetting("laps");
            l_pLaps->setSelected(std::atoi(l_sLaps.c_str()) - 1);
          }

          m_pState->setZLayer(0);

          data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));
          std::vector<data::SChampionshipPlayer> l_vStanding = l_cChampionship.getStandings();

          printf("\n********************************");
          printf(  "**** Championship Standings ****\n");
          printf("\n********************************\n");

          printf("   |      Name       | Pts |");

          for (int i = 0; i < l_cChampionship.m_vPlayers.size(); i++)
            printf("%2i |", i + 1);

          printf(" Rs | St | Fl | DNF |\n");

          printf("---+-----------------+-----+");

          for (int i = 0; i < l_cChampionship.m_vPlayers.size(); i++)
            printf("---+");

          printf("----+----+----+\n");

          int l_iPos = 1;
          for (std::vector<data::SChampionshipPlayer>::iterator it = l_vStanding.begin(); it != l_vStanding.end(); it++) {
            std::string s = (*it).m_sName;

            if (s.length() > 15)
              s = s.substr(s.length() - 15);

            while (s.length() < 15)
              s += " ";

            printf("%2i | %s | %3i |", l_iPos, s.c_str(), (*it).m_iPoints);

            for (int i = 0; i < l_cChampionship.m_vPlayers.size(); i++)
              if ((*it).m_aResult[i] != 0)
                printf("%2i |", (*it).m_aResult[i]);
              else
                printf("   |");

            printf(" %2i | %2i | %2i |\n", (*it).m_iRespawn, (*it).m_iStunned, (*it).m_iFastestLaps);

            l_iPos++;
          }

          printf("\n********************************\n");

          printf("Path: \"%s\"\n", helpers::ws2s(platform::portableGetDataPath()).c_str());
        }

        virtual ~CMenuSelectTrack() {
          printf("CMenuSelectTrack Deleted.\n");
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sCaller == "cancel") {
                std::string l_sScript = m_pManager->peekMenuStack();
                m_pManager->pushToMenuStack("menu_selecttrack");
                createMenu(l_sScript, m_pDevice, m_pManager, m_pState);
              }
              else if (l_sCaller == "ok" && (m_pOk == nullptr || m_pOk->isVisible())) {
                if (m_pTrackList != nullptr) {
                  printf("Track selected: \"%s\"\n", m_pTrackList->getSelectedName().c_str());

                  int l_iLaps = 2;

                  gui::CSelector *l_pLaps = reinterpret_cast<gui::CSelector *>(findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

                  if (l_pLaps)
                    l_iLaps = l_pLaps->getSelected() + 1;

                  printf("Laps: %i\n", l_iLaps);

                  m_pManager->pushToMenuStack("menu_selecttrack");
                  m_pManager->pushToMenuStack("menu_standings"  );
                  m_pManager->pushToMenuStack("menu_raceresult" );

                  m_pState->getGlobal()->setSetting("track", m_pTrackList->getSelectedData());
                  m_pState->getGlobal()->setSetting("laps" , std::to_string(l_iLaps));

                  data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

                  data::SGameData l_cData(data::SGameData::enType::Local, m_pTrackList->getSelectedData(), l_iLaps, l_cChampionship.m_iClass);
                  m_pState->getGlobal()->setGlobal("gamedata", l_cData.serialize());
                  m_pState->getGlobal()->initNextRaceScreen();

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
              printf("Scrollbar changed: \"%s\"\n", l_sCaller.c_str());
            }
          }
          else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
            if (m_pTrackList != nullptr && 
              (
                (a_cEvent.UserEvent.UserData1 == c_iEventImageSelected   && a_cEvent.UserEvent.UserData2 == c_iEventImageSelected  ) ||
                (a_cEvent.UserEvent.UserData1 == c_iEventImagePosChanged && a_cEvent.UserEvent.UserData2 == c_iEventImagePosChanged)
              )
            ) {
              if (m_pOk != nullptr)
                m_pOk->setVisible(true);
            }
          }

          return l_bRet;
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        */
        virtual void run() { 
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
        }
    };

    IMenuHandler* createMenuSelectTrack(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSelectTrack(a_pDevice, a_pManager, a_pState);
    }
  }
}