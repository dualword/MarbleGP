// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameServer.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
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

    public:
        CMenuStandings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_pServer(a_pState->getGlobal()->getGameServer()),
          m_pClient(a_pState->getGlobal()->getGameClient())
        {
          m_pGui ->clear();
          m_pSmgr->clear();

          helpers::loadMenuFromXML("data/menu/menu_standings.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));

          printf("\n\n%s\n\n", m_pState->getGlobal()->getGlobal("championship").c_str());

          irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("tab_result", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

          if (l_pTab != nullptr) {
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
              Positions
            };

#ifdef _ANDROID
            //                     width  headline    columd ID  height alignment
            std::vector<std::tuple<int, std::wstring, enColumn, int, irr::gui::EGUI_ALIGNMENT>> l_vColumns = {
              {  10, L"Pos"      , enColumn::Pos      , 2, irr::gui::EGUIA_CENTER     },
              {  30, L"Name"     , enColumn::Name     , 2, irr::gui::EGUIA_UPPERLEFT  },
              {  15, L"Points"   , enColumn::Points   , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"Respawn"  , enColumn::Respawn  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"Stunned"  , enColumn::Stunned  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  15, L"D.N.F."   , enColumn::Dnf      , 2, irr::gui::EGUIA_LOWERRIGHT }
            };
#else
            //                     width  headline    columd ID  height alignment
            std::vector<std::tuple<int, std::wstring, enColumn, int, irr::gui::EGUI_ALIGNMENT>> l_vColumns = {
              {   5, L"Pos"      , enColumn::Pos      , 2, irr::gui::EGUIA_CENTER     },
              {  25, L"Name"     , enColumn::Name     , 2, irr::gui::EGUIA_UPPERLEFT  },
              {   8, L"Points"   , enColumn::Points   , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"Respawn"  , enColumn::Respawn  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"Stunned"  , enColumn::Stunned  , 2, irr::gui::EGUIA_LOWERRIGHT },
              {   8, L"D.N.F."   , enColumn::Dnf      , 2, irr::gui::EGUIA_LOWERRIGHT },
              {  38, L"Positions", enColumn::Positions, 1, irr::gui::EGUIA_LOWERRIGHT },
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

            std::vector<data::SChampionshipPlayer> l_vStandings = l_cChampionship.getStandings();

            l_cRowPos.Y += l_iHeight;

            int l_iPos = 1;

            for (std::vector<data::SChampionshipPlayer>::iterator it = l_vStandings.begin(); it != l_vStandings.end(); it++) {
              l_cRowPos.Y += l_iHeight;
              l_cColPos = l_cRowPos;

              l_iHeight = 5 * l_pRegular->getDimension(L"Hello World").Height / 4;

              m_vTable.push_back(std::vector<irr::gui::IGUIStaticText *>());

              irr::core::dimension2du l_cDim = irr::core::dimension2du(25 * l_iWidth / 100, l_iHeight);
              std::wstring l_sName   = helpers::fitString(L" " + helpers::s2ws((*it).m_sName), l_pRegular, l_cDim);

              for (auto l_tColumn : l_vColumns) {
                std::wstring l_sText = 
                  std::get<2>(l_tColumn) == enColumn::Pos     ? L" " + std::to_wstring(l_iPos) :
                  std::get<2>(l_tColumn) == enColumn::Name    ? l_sName :
                  std::get<2>(l_tColumn) == enColumn::Points  ? std::to_wstring((*it).m_iPoints      ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Respawn ? std::to_wstring((*it).m_iRespawn     ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Stunned ? std::to_wstring((*it).m_iStunned     ) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Dnf     ? std::to_wstring((*it).m_iDidNotFinish) + L" " : L"";
                ;

                if (std::get<2>(l_tColumn) != enColumn::Positions) {
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
                else {
                  for (int i = 0; i < 16; i++) {
                    irr::gui::IGUIStaticText *p = m_pGui->addStaticText((*it).m_aResult[i] == 0 ? L"" : (std::to_wstring((*it).m_aResult[i]) + L" ").c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(l_iPosWidth, l_iHeight)), true, true);
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
            gui::CMenuButton *p = reinterpret_cast<gui::CMenuButton *>(findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, m_pGui->getRootGUIElement()));
            if (p != nullptr)
              p->setVisible(false);
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

              if (l_sButton == "ok") {
                if (m_pServer != nullptr) {
                  m_pServer->changeState("menu_netlobby");
                }

                createMenu(m_pManager->popMenuStack(), m_pDevice, m_pManager, m_pState);
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
      };



      IMenuHandler* createMenuStandings(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuStandings(a_pDevice, a_pManager, a_pState);
      }
  }
}
