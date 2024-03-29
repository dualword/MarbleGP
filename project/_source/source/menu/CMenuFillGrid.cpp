// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <gameclasses/SPlayer.h>
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
        network::CGameServer *m_pServer;    /**< The game server */
        bool                  m_bWaiting;   /**< Waiting for the clients to be in network lobby */

        int getAiClass(int a_iIndex, int a_iSetting) {
          if (a_iSetting == 0)
            return 0;
          else if (a_iSetting == 1)
            return 1;
          else if (a_iSetting == 2)
            return 2;
          else
            return a_iIndex % 3;
        }

      public:
        CMenuFillGrid(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState), 
          m_pServer   (CGlobal::getInstance()->getGameServer()),
          m_bWaiting  (false)
        {

          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_fillgrid.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          data::SGameSettings l_cSettings = data::SGameSettings();
          l_cSettings.deserialize(m_pState->getGlobal()->getSetting("gamesetup"));

          // Tuple: "Name", "Short Name", "Texture Index", "Preferred Class One", "Preferred class Two"
          std::vector<std::tuple<std::string, std::string, std::string, int, int, float>> l_vAiPlayers;

          helpers::loadAiProfiles(l_vAiPlayers);

          int l_iGridSize = 1;

          if (l_cSettings.m_bFillGridAI) {
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
          }

          // We fill a vector with the grid positions
          std::vector<int> l_vGrid;

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

          for (int i = 0; i < (l_pTournament->m_vPlayers.size() > l_iGridSize ? l_pTournament->m_vPlayers.size() : l_iGridSize); i++)
            l_vGrid.push_back(i);

          // if necessary shuffle the vector
          if (l_cSettings.m_bRandomFirstRace) {
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };

            std::shuffle(l_vGrid.begin(), l_vGrid.end(), l_cRe);
          }

          int l_iCount = (int)l_pTournament->m_vPlayers.size();

          for (std::vector<gameclasses::SPlayer *>::iterator it = l_pTournament->m_vPlayers.begin(); it != l_pTournament->m_vPlayers.end(); it++) {
            int l_iGridPos = *l_vGrid.begin();

            (*it)->m_sNumber = std::to_wstring(l_iGridPos + 1);

            // The starting position of the first race defines the number of each marble
            if ((*it)->m_sTexture == "")
              (*it)->m_sTexture = "default://number=" + std::to_string(l_iGridPos + 1);
            else
              (*it)->m_sTexture = (*it)->m_sTexture + "&number=" + std::to_string(l_iGridPos + 1);
            
            if (m_pServer != nullptr) {
              messages::CSetTexture l_cNumber = messages::CSetTexture((*it)->m_iPlayer, (*it)->m_sTexture);
              m_pServer->getInputQueue()->postMessage(&l_cNumber);
            }

            l_vGrid.erase(l_vGrid.begin());
          }

          if (l_cSettings.m_bFillGridAI) { // && m_pServer == nullptr) {
            std::random_device l_cRd { };
            std::default_random_engine l_cRe { l_cRd() };
            std::shuffle(l_vAiPlayers.begin(), l_vAiPlayers.end(), l_cRe);

            std::vector<std::string> l_vAiClass;

            int l_iAiPlayers = l_iGridSize - (int)l_pTournament->m_vPlayers.size();
            printf("Add %i AI Players.\n", l_iAiPlayers);

            for (int i = 0; i < l_iAiPlayers; i++)
              switch (l_cSettings.m_eRaceClass) {
                // Marble 3
              case data::SGameSettings::enRaceClass::Marble3:
                  l_vAiClass.push_back("marble3");
                  break;

                // Marble 3 + 2
                case data::SGameSettings::enRaceClass::Marble3_2:
                  l_vAiClass.push_back(i % 2 == 0 ? "marble2" : "marble3");
                  break;

                // Marble 2
                case data::SGameSettings::enRaceClass::Marble2:
                  l_vAiClass.push_back("marble2");
                  break;

                // Marble 2 + GP
                case data::SGameSettings::enRaceClass::Marble2_GP:
                  l_vAiClass.push_back(i % 2 == 0 ? "marblegp" : "marble2");
                  break;

                // MarbleGP
                case data::SGameSettings::enRaceClass::MarbleGP:
                  l_vAiClass.push_back("marblegp");
                  break;

                // All Classes
                default:
                  l_vAiClass.push_back(i % 3 == 0 ? "marblegp" : i % 2 == 1 ? "marble2" : "marble3");
                  break;
              }

            std::shuffle(l_vAiClass.begin(), l_vAiClass.end(), l_cRe);

            int l_iPlayerId = 0;

            for (auto l_pPlayer: l_pTournament->m_vPlayers)
              if (l_pPlayer->m_iPlayer >= l_iPlayerId)
                l_iPlayerId = l_pPlayer->m_iPlayer + 1;

            while (l_pTournament->m_vPlayers.size() < l_iGridSize && !l_vGrid.empty()) {
              std::vector<std::tuple<std::string, std::string, std::string, int, int, float>>::iterator l_itAi = l_vAiPlayers.end();

              int l_iAiClass = *l_vAiClass.begin() == "marblegp" ? 0 : *l_vAiClass.begin() == "marble2" ? 1 : 2;

              for (l_itAi = l_vAiPlayers.begin(); l_itAi != l_vAiPlayers.end(); l_itAi++) {
                if (std::get<3>(*l_itAi) == l_iAiClass) {
                  break;
                }
              }

              if (l_itAi == l_vAiPlayers.end()) {
                for (l_itAi = l_vAiPlayers.begin(); l_itAi != l_vAiPlayers.end(); l_itAi++) {
                  if (std::get<4>(*l_itAi) == l_iAiClass) {
                    break;
                  }
                }
              }

              if (l_itAi == l_vAiPlayers.end())
                l_itAi = l_vAiPlayers.begin();

              l_iCount++;

              gameclasses::SPlayer* l_pAi = new gameclasses::SPlayer(
                l_iPlayerId,
                -1,
                std::get<0>(*l_itAi) + "|" + *l_vAiClass.begin(),
                std::get<2>(*l_itAi) + "&number=" + std::to_string(*l_vGrid.begin() + 1),
                "class=" + *l_vAiClass.begin() + "&deviation=" + std::to_string(std::get<5>(*l_itAi)),
                std::get<1>(*l_itAi),
                data::SPlayerData::enAiHelp::Off,
                nullptr,
                data::enPlayerType::Ai
              );

              l_iPlayerId++;

              l_pTournament->m_vPlayers.push_back(l_pAi);

              l_vAiClass.erase(l_vAiClass.begin());

              l_vGrid.erase(l_vGrid.begin());

              if (m_pServer != nullptr) {
                messages::CRegisterPlayer l_cPlayer = messages::CRegisterPlayer(l_pAi->m_sName, l_pAi->m_sTexture, l_pAi->m_iPlayer, l_pAi->m_sShortName);
                m_pServer->getInputQueue()->postMessage(&l_cPlayer);
              }

              l_vAiPlayers.erase(l_itAi);
            }

            printf(".\n");
          }

          std::sort(l_pTournament->m_vPlayers.begin(), l_pTournament->m_vPlayers.end(), [](gameclasses::SPlayer *p1, gameclasses::SPlayer *p2) {
            return std::wcstol(p1->m_sNumber.c_str(), nullptr, 10) < std::wcstol(p2->m_sNumber.c_str(), nullptr, 10);
          });

          std::vector<gameclasses::SPlayer *>::iterator l_itPlr = l_pTournament->m_vPlayers.begin();

          for (int i = 1; i <= 16; i++) {
            gui::CMenuBackground *p = reinterpret_cast<gui::CMenuBackground *>(helpers::findElementByNameAndType("player" + std::to_string(i), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuBackgroundId, m_pGui->getRootGUIElement()));
            if (p != nullptr) {
              std::wstring l_sBot = L"";

              irr::gui::IGUIStaticText *l_pText = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_label", irr::gui::EGUIET_STATIC_TEXT, p));
              if (l_pText != nullptr) {
                l_pText->setText(std::to_wstring(i).c_str());
                if (l_itPlr == l_pTournament->m_vPlayers.end())
                  l_pText->setVisible(false);
              }

              l_pText = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("player_name", irr::gui::EGUIET_STATIC_TEXT, p));
              if (l_pText != nullptr) {
                if (l_itPlr == l_pTournament->m_vPlayers.end())
                  l_pText->setVisible(false);
                else {
                  std::string l_sName  = "";
                  std::string l_sDummy = "";

                  
                  if (helpers::splitStringNameBot((*l_itPlr)->m_sName, l_sName, l_sDummy)) {
                    l_sBot = L"bot_" + helpers::s2ws(l_sDummy);
                  }

                  l_pText->setText(helpers::s2ws(l_sName).c_str());
                }
              }

              gui::CMenuButton *l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("remove_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p));
              if (l_pBtn != nullptr) {
                l_pBtn->setVisible(false);
              }

              l_pBtn = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("add_player", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, p));
              if (l_pBtn != nullptr) {
                l_pBtn->setVisible(false);
              }

              irr::gui::IGUIImage *l_pBot = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("ai_class", irr::gui::EGUIET_IMAGE, p));
              if (l_pBot != nullptr) {
                l_pBot->setVisible(l_sBot != L"");
                if (l_sBot != L"") {
                  std::string l_sIcon = helpers::getIconFolder(m_pDrv->getScreenSize().Height) + helpers::ws2s(l_sBot + L".png");
                  l_pBot->setImage(m_pDrv->getTexture(l_sIcon.c_str()));

                  if (l_sBot == L"bot_marble3")
                    l_pBot->setToolTipText(L"Marble3 AI Player");
                  else if (l_sBot  == L"bot_marble2")
                    l_pBot->setToolTipText(L"Marble2 AI Player");
                  else if (l_sBot  == L"bot_marblegp")
                    l_pBot->setToolTipText(L"MarbleGP AI Player");
                }
              }

              irr::gui::IGUIStaticText *l_pNumber = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("starting_number", irr::gui::EGUIET_STATIC_TEXT, p));

              if (l_pNumber != nullptr) {
#ifdef _ANDROID
                int l_iBorder = m_pState->getGlobal()->getRasterSize() / 3;
                if (l_iBorder < 2)
                  l_iBorder = 2;

                irr::core::recti l_cRect = l_pNumber->getRelativePosition();
                l_cRect.UpperLeftCorner  += irr::core::vector2di(l_iBorder);
                l_cRect.LowerRightCorner -= irr::core::vector2di(l_iBorder);
                l_pNumber->setRelativePosition(l_cRect);
#endif
                if (i <= l_pTournament->m_vPlayers.size()) {
                  std::string l_sTexture = (*l_itPlr)->m_sTexture;

                  if (l_sTexture != "") {
                    std::string l_sType = "";

                    std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, l_sTexture);

                    std::string l_sNumber = helpers::findTextureParameter(l_mParams, "number");
                    std::string l_sBack   = helpers::findTextureParameter(l_mParams, "numberback");
                    std::string l_sColor  = helpers::findTextureParameter(l_mParams, "numbercolor");

                    if (l_sNumber != "") l_pNumber->setText(helpers::s2ws(l_sNumber).c_str());
                    if (l_sBack   != "") {
                      irr::video::SColor l_cColor;
                      helpers::fillColorFromString(l_cColor, l_sBack);
                      l_pNumber->setBackgroundColor(l_cColor);
                    }
                    if (l_sColor != "") {
                      irr::video::SColor l_cColor;
                      helpers::fillColorFromString(l_cColor, l_sColor);
                      l_pNumber->setOverrideColor(l_cColor);
                    }
                  }
                  else l_pNumber->setVisible(false);
                }
                else l_pNumber->setVisible(false);
              }
            }

            if (l_itPlr != l_pTournament->m_vPlayers.end())
              l_itPlr++;
          }
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