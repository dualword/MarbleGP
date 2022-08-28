// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <network/CGameServer.h>
#include <gui/CMenuBackground.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <algorithm>
#include <random>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuFillGrid
    * @author Christian Keimel
    * This menu shows the complete grid for the upcoming races,
    * i.e. all local players, network players and AI marbles
    */
    class CMenuFillGrid : public IMenuHandler {
      private:
        network::CGameServer     *m_pServer;    /**< The game server */

      public:
        CMenuFillGrid(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pServer   (CGlobal::getInstance()->getGameServer()) 
        {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_fillgrid.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pState->setZLayer(1);

          data::SGameSettings l_cSettings = data::SGameSettings();
          l_cSettings.deserialize(m_pState->getGlobal()->getSetting("gamesetup"));

          data::SRacePlayers l_cPlayers = data::SRacePlayers();
          l_cPlayers.deserialize(m_pState->getGlobal()->getGlobal("raceplayers"));

          int l_iGridSize = 1;

          if (l_cSettings.m_iGridSize == 0)
            l_iGridSize = 2;
          else if (l_cSettings.m_iGridSize == 1)
            l_iGridSize = 4;
          else if (l_cSettings.m_iGridSize == 2)
            l_iGridSize = 8;
          else if (l_cSettings.m_iGridSize == 3)
            l_iGridSize = 12;
          else if (l_cSettings.m_iGridSize == 4)
            l_iGridSize = 16;

          // We fill a vector with the grid positions and ..
          std::vector<int> l_vGrid;

          for (int i = 0; i < (l_cPlayers.m_vPlayers.size() > l_iGridSize ? l_cPlayers.m_vPlayers.size() : l_iGridSize); i++)
            l_vGrid.push_back(i);

          // .. if necessary shuffle the vector
          if (l_cSettings.m_bRandomFirstRace) {
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };

            std::shuffle(l_vGrid.begin(), l_vGrid.end(), l_cRe);
          }

          int l_iCount = (int)l_cPlayers.m_vPlayers.size();

          for (std::vector<data::SPlayerData>::iterator it = l_cPlayers.m_vPlayers.begin(); it != l_cPlayers.m_vPlayers.end(); it++)
            if ((*it).m_iGridPos > l_iCount) {
              (*it).m_iGridPos = *l_vGrid.begin();

              // The starting position of the first race defines the number of each marble
              if ((*it).m_sTexture == "")
                (*it).m_sTexture = "default://number=" + std::to_string((*it).m_iGridPos + 1);
              else
                (*it).m_sTexture = (*it).m_sTexture + "&number=" + std::to_string((*it).m_iGridPos + 1);

              l_vGrid.erase(l_vGrid.begin());
            }

          if (l_cSettings.m_bFillGridAI) { // && m_pServer == nullptr) {
            while (l_cPlayers.m_vPlayers.size() < l_iGridSize && !l_vGrid.empty()) {
              l_iCount++;

              data::SPlayerData l_cData;
              l_cData.m_eType      = data::enPlayerType::Ai;
              l_cData.m_iGridPos   = *l_vGrid.begin();
              l_cData.m_iPlayerId  = l_iCount;
              l_cData.m_sName      = "AI Demo Player #" + std::to_string(l_cData.m_iGridPos + 1);
              l_cData.m_sControls  = "ai_player";
              l_cData.m_eAiHelp    = data::SPlayerData::enAiHelp::Off;
              l_cData.m_sTexture   = "default://number=" + std::to_string(l_cData.m_iGridPos + 1);
              l_cData.m_sShortName = "Ai#" + std::to_string(l_cData.m_iGridPos + 1);

              l_vGrid.erase(l_vGrid.begin());

              l_cPlayers.m_vPlayers.push_back(l_cData);

              if (m_pServer != nullptr) {
                messages::CRegisterPlayer l_cPlayer = messages::CRegisterPlayer(l_cData.m_sName, l_cData.m_sTexture, l_cData.m_iPlayerId);
                m_pServer->getInputQueue()->postMessage(&l_cPlayer);
              }
            }
          }

          printf("\n\n%s\n\n", l_cPlayers.toString().c_str());

          std::vector<data::SPlayerData> l_cData;

          for (std::vector<data::SPlayerData>::iterator it = l_cPlayers.m_vPlayers.begin(); it != l_cPlayers.m_vPlayers.end(); it++)
            l_cData.push_back(*it);

          std::sort(l_cData.begin(), l_cData.end(), [](data::SPlayerData p1, data::SPlayerData p2) {
            return p1.m_iGridPos < p2.m_iGridPos;
          });

          for (int i = 1; i <= 16; i++) {
            gui::CMenuBackground *p = reinterpret_cast<gui::CMenuBackground *>(findElementByNameAndType("player" + std::to_string(i), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (p != nullptr) {
              irr::gui::IGUIStaticText *l_pText = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, p));
              if (l_pText != nullptr) {
                l_pText->setText(std::to_wstring(i).c_str());
                if (l_cData.size() < i)
                  l_pText->setVisible(false);
              }

              l_pText = reinterpret_cast<irr::gui::IGUIStaticText *>(findElementByNameAndType("player_name", irr::gui::EGUIET_STATIC_TEXT, p));
              if (l_pText != nullptr) {
                if (l_cData.size() < i)
                  l_pText->setVisible(false);
                else
                  l_pText->setText(helpers::s2ws(l_cData[static_cast<std::vector<dustbin::data::SPlayerData, std::allocator<dustbin::data::SPlayerData>>::size_type>(i) - 1].m_sName).c_str());
              }

              gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p));
              if (l_pBtn != nullptr) {
                l_pBtn->setVisible(false);
              }

              l_pBtn = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("add_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p));
              if (l_pBtn != nullptr) {
                l_pBtn->setVisible(false);
              }
            }
          }

          m_pState->getGlobal()->setGlobal("raceplayers", l_cPlayers.serialize());

          data::SChampionship l_cChampionship = data::SChampionship(l_cSettings.m_iRaceClass, (int)l_cPlayers.m_vPlayers.size(), l_cSettings.m_iGridPos, l_cSettings.m_bReverseGrid);

          for (std::vector<data::SPlayerData>::iterator it = l_cPlayers.m_vPlayers.begin(); it != l_cPlayers.m_vPlayers.end(); it++) {
            l_cChampionship.m_vPlayers.push_back(data::SChampionshipPlayer((*it).m_iPlayerId, (*it).m_sName));
          }

          m_pState->getGlobal()->setGlobal("championship", l_cChampionship.serialize());
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

              if (l_sCaller == "ok") {
                if (m_pServer != nullptr) {
                  m_pServer->changeState("menu_netlobby");
                }
                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
              }
              else if (l_sCaller == "cancel") {
                m_pManager->clearMenuStack();
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
            }
          }

          return l_bRet;
        }
    };

    IMenuHandler *createMenuFillGrid(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuFillGrid(a_pDevice, a_pManager, a_pState);
    }
  }
}