// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gameclasses/SPlayer.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameServer.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <helpers/CAutoMenu.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuStandings
    * @author Christian Keimel
    * The menu showing the championship standings
    */
    class CMenuStandings : public IMenuHandler {
      private:
        std::vector<std::vector<irr::gui::IGUIStaticText *>> m_vTable;

        network::CGameServer *m_pServer;  /**< The game server */
        network::CGameClient *m_pClient;  /**< The game client */

        helpers::CAutoMenu *m_pAuto;

    public:
        CMenuStandings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_pServer(a_pState->getGlobal()->getGameServer()),
          m_pClient(a_pState->getGlobal()->getGameClient()),
          m_pAuto  (nullptr)
        {
          m_pGui ->clear();
          m_pSmgr->clear();

          helpers::loadMenuFromXML("data/menu/menu_standings.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();
          gameclasses::SRace       *l_pLastRace   = l_pTournament->getRace();

          irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("tab_result", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

          if (l_pTab != nullptr && l_pTournament != nullptr) {
            irr::core::position2di l_cRowPos = l_pTab->getAbsoluteClippingRect().UpperLeftCorner;
            irr::core::position2di l_cColPos = l_cRowPos;

#ifdef _ANDROID
            irr::gui::IGUIFont *l_pBig     = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
            irr::gui::IGUIFont *l_pRegular = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
            irr::gui::IGUIFont *l_pSmall   = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
#else
            irr::gui::IGUIFont *l_pBig     = m_pState->getGlobal()->getFont(enFont::Big    , m_pDrv->getScreenSize());
            irr::gui::IGUIFont *l_pRegular = m_pState->getGlobal()->getFont(enFont::Regular, m_pDrv->getScreenSize());
            irr::gui::IGUIFont *l_pSmall   = m_pState->getGlobal()->getFont(enFont::Small  , m_pDrv->getScreenSize());
#endif

            irr::u32 l_iHeight = 5 * l_pRegular->getDimension(L"Hello World").Height / 4;
            irr::u32 l_iWidth  = l_pTab->getAbsoluteClippingRect().getWidth();

            enum class enColumn {
              Pos,
              Name,
              Points,
              Respawn,
              Stunned,
              Dnf,
              Positions,
              AiClass
            };

#ifdef _ANDROID
            //                     width  headline    columd ID  height alignment
            std::vector<std::tuple<int, std::wstring, enColumn, int, irr::gui::EGUI_ALIGNMENT>> l_vColumns = {
              {   9, L"Pos"      , enColumn::Pos      , 2, irr::gui::EGUIA_CENTER     },
              {  29, L"Name"     , enColumn::Name     , 2, irr::gui::EGUIA_UPPERLEFT  },
              {  14, L"Points"   , enColumn::Points   , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"Respawn"  , enColumn::Respawn  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"Stunned"  , enColumn::Stunned  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"D.N.F."   , enColumn::Dnf      , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   3, L"Ai"       , enColumn::AiClass  , 2, irr::gui::EGUIA_CENTER     }
            };
#else
            //                     width  headline    columd ID  height alignment
            std::vector<std::tuple<int, std::wstring, enColumn, int, irr::gui::EGUI_ALIGNMENT>> l_vColumns = {
              {   4, L"Pos"      , enColumn::Pos      , 2, irr::gui::EGUIA_CENTER     },
              {  24, L"Name"     , enColumn::Name     , 2, irr::gui::EGUIA_UPPERLEFT  },
              {   7, L"Points"   , enColumn::Points   , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"Respawn"  , enColumn::Respawn  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"Stunned"  , enColumn::Stunned  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"D.N.F."   , enColumn::Dnf      , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  38, L"Positions", enColumn::Positions, 1, irr::gui::EGUIA_LOWERRIGHT },
              {   3, L"Ai"       , enColumn::AiClass  , 2, irr::gui::EGUIA_CENTER     }
            };
#endif

            int l_iPosWidth = 0;

            for (auto l_tColumn : l_vColumns) {
              irr::gui::IGUIStaticText *p = m_pGui->addStaticText(std::get<1>(l_tColumn).c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100, std::get<3>(l_tColumn) * l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
              p->setDrawBackground(true);
              p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));

              if (std::get<2>(l_tColumn) == enColumn::Positions) {
                l_iPosWidth = (std::get<0>(l_tColumn) * l_iWidth / 100) / 16;
                irr::core::dimension2du l_cPosDim = irr::core::dimension2du(l_iPosWidth, l_iHeight);

                for (int i = 1; i <= 16; i++) {
                  p = m_pGui->addStaticText(std::to_wstring(i).c_str(), irr::core::recti(l_cColPos + irr::core::position2di(0, l_iHeight), l_cPosDim), true);
                  p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
                  p->setDrawBackground(true);
                  p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));

                  l_cColPos.X += l_iPosWidth;
                }
              }
              else l_cColPos.X += std::get<0>(l_tColumn) * l_iWidth / 100;
            }

            l_cRowPos.Y += l_iHeight;

            int l_iPos = 1;

            for (auto &l_cStanding: l_pLastRace->m_vStandings) {
              gameclasses::SPlayer *l_pPlayer = nullptr;

              for (auto p : l_pTournament->m_vPlayers) {
                if (p->m_iPlayer == l_cStanding.m_iPlayer) {
                  l_pPlayer = p;
                  break;
                }
              }

              if (l_pPlayer == nullptr) {
                printf("Player %i not found.\n", l_cStanding.m_iPlayer);
                return;
              }

              l_cRowPos.Y += l_iHeight;
              l_cColPos = l_cRowPos;

              l_iHeight = 5 * l_pRegular->getDimension(L"Hello World").Height / 4;

              m_vTable.push_back(std::vector<irr::gui::IGUIStaticText *>());

              irr::core::dimension2du l_cDim = irr::core::dimension2du(25 * l_iWidth / 100 - 5 * l_iHeight / 2, l_iHeight);
              std::wstring l_sName    = L"";
              std::wstring l_sAiClass = L"";
              int l_iAi = -1;

              if (helpers::splitStringNameBot(helpers::s2ws(l_pPlayer->m_sName), l_sName, l_sAiClass)) {
                if (l_sAiClass == L"marble3")
                  l_iAi = 2;
                else if (l_sAiClass == L"marble2")
                  l_iAi = 1;
                else if (l_sAiClass == L"marblegp")
                  l_iAi = 0;
              }

              l_sName = helpers::fitString(l_sName, l_pRegular, l_cDim);

              for (auto l_tColumn : l_vColumns) {
                std::wstring l_sText = 
                  std::get<2>(l_tColumn) == enColumn::Pos     ? L" " + std::to_wstring(l_iPos) :
                  std::get<2>(l_tColumn) == enColumn::Name    ? l_sName :
                  std::get<2>(l_tColumn) == enColumn::Points  ? std::to_wstring(l_cStanding.m_iScore   ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Respawn ? std::to_wstring(l_cStanding.m_iRespawn ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Stunned ? std::to_wstring(l_cStanding.m_iStunned ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Dnf     ? std::to_wstring(l_cStanding.m_iNoFinish) + L" " : L"";
                ;

                if (std::get<2>(l_tColumn) != enColumn::Positions && std::get<2>(l_tColumn) != enColumn::AiClass && std::get<2>(l_tColumn) != enColumn::Name) {
                  irr::gui::IGUIStaticText *p = m_pGui->addStaticText(l_sText.c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100, l_iHeight)), true, true);
                  p->setTextAlignment(std::get<4>(l_tColumn), irr::gui::EGUIA_CENTER);
                  p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
                  p->setDrawBackground(false);
                  p->setOverrideFont(l_pRegular);

                  if (std::get<2>(l_tColumn) == enColumn::Pos) {
                    if (l_iPos > 3)
                      m_vTable.back().push_back(p);
                    else {
                      if (l_iPos == 1) p->setBackgroundColor(irr::video::SColor(128, 255, 215,   0));
                      if (l_iPos == 2) p->setBackgroundColor(irr::video::SColor(128, 192, 192, 192));
                      if (l_iPos == 3) p->setBackgroundColor(irr::video::SColor(128, 191, 137, 112));

                      p->setDrawBackground(true);
                    }
                  }
                  else m_vTable.back().push_back(p);

                  l_cColPos.X += std::get<0>(l_tColumn) * l_iWidth / 100;
                }
                else if (std::get<2>(l_tColumn) == enColumn::Name) {
                  irr::core::recti l_cRect = irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100 - 3 * l_iHeight / 2, l_iHeight));

                  std::wstring s = helpers::fitString(l_sText, l_pRegular, irr::core::dimension2du(l_cRect.getSize()));

                  irr::gui::IGUIStaticText *p = m_pGui->addStaticText(s.c_str(), l_cRect, true, true);
                  p->setTextAlignment(std::get<4>(l_tColumn), irr::gui::EGUIA_CENTER);
                  p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
                  p->setDrawBackground(false);
                  p->setOverrideFont(l_pRegular);

                  m_vTable.back().push_back(p);

                  irr::core::recti l_cNumber = irr::core::recti(irr::core::vector2di(l_cColPos.X + std::get<0>(l_tColumn) * l_iWidth / 100 - 3 * l_iHeight / 2, l_cColPos.Y), irr::core::dimension2du(3 * l_iHeight / 2, l_iHeight));

                  p = m_pGui->addStaticText(L"", l_cNumber, true, true);
                  p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
                  p->setOverrideFont(l_pRegular);

                  std::string l_sType = "";

                  std::map<std::string, std::string> l_mParams = helpers::parseParameters(l_sType, l_pPlayer->m_sTexture);

                  std::string l_sNumber = helpers::findTextureParameter(l_mParams, "number");
                  std::string l_sBack   = helpers::findTextureParameter(l_mParams, "numberback");
                  std::string l_sColor  = helpers::findTextureParameter(l_mParams, "numbercolor");

                  if (l_sNumber != "") p->setText(helpers::s2ws(l_sNumber).c_str());
                  if (l_sBack   != "") {
                    irr::video::SColor l_cColor;
                    helpers::fillColorFromString(l_cColor, l_sBack);
                    p->setBackgroundColor(l_cColor);
                  }
                  if (l_sColor != "") {
                    irr::video::SColor l_cColor;
                    helpers::fillColorFromString(l_cColor, l_sColor);
                    p->setOverrideColor(l_cColor);
                  }

                  l_cColPos.X += std::get<0>(l_tColumn) * l_iWidth / 100;
                }
                else if (std::get<2>(l_tColumn) == enColumn::AiClass) {
                  irr::gui::IGUIStaticText *p = m_pGui->addStaticText(L"", irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100, l_iHeight)), true, true);

                  switch (l_iAi) {
                    case 0: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0x20, 0x00)); break;
                    case 1: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0x80, 0x00)); break;
                    case 2: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0xff, 0x00)); break;
                  }
                }
                else {
                  for (int i = 0; i < 16; i++) {
                    irr::gui::IGUIStaticText *p = m_pGui->addStaticText(l_cStanding.m_aResult[i] == 0 ? L"" : (std::to_wstring(l_cStanding.m_aResult[i]) + L" ").c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(l_iPosWidth, l_iHeight)), true, true);
                    p->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
                    p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
                    p->setDrawBackground(false);
                    p->setOverrideFont(l_pSmall);
                    l_cColPos.X += l_iPosWidth;

                    m_vTable.back().push_back(p);
                  }
                }

              }

              l_iPos++;
            }
          }

          if (m_pClient != nullptr) {
            gui::CMenuButton *p = reinterpret_cast<gui::CMenuButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setVisible(false);
          }

          if (l_pTournament != nullptr)
            l_pTournament->saveToJSON();

          m_pAuto = new helpers::CAutoMenu(m_pDevice, this);
        }

        virtual ~CMenuStandings() {
          if (m_pAuto != nullptr) {
            delete m_pAuto;
            m_pAuto = nullptr;
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

              if (l_sButton == "ok") {
                if (m_pAuto != nullptr) {
                  delete m_pAuto;
                  m_pAuto = nullptr;
                }

                std::string l_sNext = m_pManager->popMenuStack();
                if (m_pServer != nullptr) {
                  m_pServer->changeState(l_sNext == "menu_finalresult" ? "menu_finalresult" : "menu_netlobby");
                }

                createMenu(l_sNext, m_pDevice, m_pManager, m_pState);
                l_bRet = true;
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT && a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_STATIC_TEXT) {
              irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(a_cEvent.GUIEvent.Caller);

              for (std::vector<std::vector<irr::gui::IGUIStaticText*>>::iterator it = m_vTable.begin(); it != m_vTable.end(); it++) {
                for (std::vector<irr::gui::IGUIStaticText*>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
                  if (*it2 == l_pLabel) {
                    for (std::vector<irr::gui::IGUIStaticText*>::iterator it3 = (*it).begin(); it3 != (*it).end(); it3++) {
                      (*it3)->setDrawBackground(false);
                    }
                    break;
                  }
                }
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED && a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_STATIC_TEXT) {
              irr::gui::IGUIStaticText *l_pLabel = reinterpret_cast<irr::gui::IGUIStaticText *>(a_cEvent.GUIEvent.Caller);

              for (std::vector<std::vector<irr::gui::IGUIStaticText*>>::iterator it = m_vTable.begin(); it != m_vTable.end(); it++) {
                for (std::vector<irr::gui::IGUIStaticText*>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
                  if (*it2 == l_pLabel) {
                    for (std::vector<irr::gui::IGUIStaticText*>::iterator it3 = (*it).begin(); it3 != (*it).end(); it3++) {
                      (*it3)->setDrawBackground(true);
                    }
                    break;
                  }
                }
              }
            }
          }

          return l_bRet;
        }

        virtual bool run() override { 
          if (m_pAuto != nullptr)
            m_pAuto->process();

          return false;
        }
      };



      IMenuHandler* createMenuStandings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuStandings(a_pDevice, a_pManager, a_pState);
      }
  }
}
