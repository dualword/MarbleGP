// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gameclasses/SPlayer.h>
#include <helpers/CMenuLoader.h>
#include <network/CGameServer.h>
#include <network/CGameClient.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <helpers/CAutoMenu.h>
#include <gui/CMenuButton.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuRaceResult
    * @author Christian Keimel
    * This menu shows result of the last race
    */
    class CMenuRaceResult : public IMenuHandler {
      private:
        std::vector<std::vector<irr::gui::IGUIStaticText *>> m_vTable;

        network::CGameServer *m_pServer;
        network::CGameClient *m_pClient;

        helpers::CAutoMenu *m_pAuto;

      public:
        CMenuRaceResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : 
          IMenuHandler(a_pDevice, a_pManager, a_pState),
          m_pServer   (a_pState->getGlobal()->getGameServer()),
          m_pClient   (a_pState->getGlobal()->getGameClient()),
          m_pAuto     (nullptr)
        {
          m_pGui ->clear();
          m_pSmgr->clear();

          helpers::loadMenuFromXML("data/menu/menu_raceresult.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();
          gameclasses::SRace       *l_pRace       = l_pTournament->getRace();

          irr::gui::IGUIStaticText *l_pTrack = reinterpret_cast<irr::gui::IGUIStaticText*>(helpers::findElementByNameAndType("label_trackname", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pTrack != nullptr) {
            irr::io::IFileSystem *l_pFs = m_pGui->getFileSystem();
            std::string l_sPath = "data/levels/" + l_pRace->m_sTrack + "/info.xml";
            std::string l_sName = l_pRace->m_sTrack;

            if (l_pFs->existFile(l_sPath.c_str())) {
              irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8(l_sPath.c_str());
              if (l_pXml) {
                bool l_bName = false;

                while (l_pXml->read()) {
                  std::string l_sNode = l_pXml->getNodeName();

                  if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                    if (l_sNode == "name")
                      l_bName = true;
                  }
                  else if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                    if (l_bName)
                      l_sName = l_pXml->getNodeData();
                  }
                  else if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
                    if (l_sNode == "name")
                      l_bName = false;
                  }
                }
                l_pXml->drop();
              }
            }

            l_pTrack->setText(helpers::s2ws(l_sName).c_str());
          }

          irr::gui::IGUIStaticText *l_pLaps = reinterpret_cast<irr::gui::IGUIStaticText*>(helpers::findElementByNameAndType("label_laps", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pLaps != nullptr) {
            l_pLaps->setText(std::to_wstring(l_pRace->m_iLaps).c_str());
          }

          irr::gui::IGUIStaticText *l_pPlayers = reinterpret_cast<irr::gui::IGUIStaticText*>(helpers::findElementByNameAndType("label_players", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pPlayers != nullptr) {
            l_pPlayers->setText(std::to_wstring((int)l_pRace->m_vPlayers.size()).c_str());
          }

          irr::gui::IGUIImage *l_pThumbnail = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("thumbnail", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
          if (l_pThumbnail != nullptr) {
            irr::io::IFileSystem *l_pFs = m_pGui->getFileSystem();
            std::string l_sPath = "data/levels/" + l_pRace->m_sTrack + "/thumbnail.png";
            if (l_pFs->existFile(l_sPath.c_str())) {
              l_pThumbnail->setImage(m_pDrv->getTexture(l_sPath.c_str()));
            }
          }

          irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(helpers::findElementByNameAndType("tab_result", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

          if (l_pTab != nullptr) {
            irr::core::position2di l_cRowPos = l_pTab->getAbsoluteClippingRect().UpperLeftCorner;
            irr::core::position2di l_cColPos = l_cRowPos;

#ifdef _ANDROID
            irr::gui::IGUIFont *l_pFont  = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
#else
            irr::gui::IGUIFont *l_pFont  = m_pState->getGlobal()->getFont(enFont::Regular, m_pDrv->getScreenSize());
#endif

            irr::u32 l_iHeight = 5 * l_pFont->getDimension(L"Hello World").Height / 4;
            irr::u32 l_iWidth  = l_pTab->getAbsoluteClippingRect().getWidth();

            enum class enColumn {
              Pos,
              Name,
              Racetime,
              BestLap,
              BestLapNo,
              Respawn,
              Stunned,
              AiClass
            };

            std::vector<std::tuple<irr::s32, std::wstring, enColumn, irr::gui::EGUI_ALIGNMENT>> l_vColums = {
              {  5, L"Pos"             , enColumn::Pos      , irr::gui::EGUIA_CENTER     },
              { 31, L"Name"            , enColumn::Name     , irr::gui::EGUIA_UPPERLEFT  },
              { 15, L"Racetime"        , enColumn::Racetime , irr::gui::EGUIA_LOWERRIGHT },
              { 15, L"Fastest Lap Time", enColumn::BestLap  , irr::gui::EGUIA_LOWERRIGHT },
              { 10, L"Fastest Lap"     , enColumn::BestLapNo, irr::gui::EGUIA_LOWERRIGHT },
              { 10, L"Respawn"         , enColumn::Respawn  , irr::gui::EGUIA_LOWERRIGHT },
              { 10, L"Stunned"         , enColumn::Stunned  , irr::gui::EGUIA_LOWERRIGHT },
              {  3, L"Ai"              , enColumn::AiClass  , irr::gui::EGUIA_CENTER     }
            };

            for (auto l_tColumn : l_vColums) {
              irr::gui::IGUIStaticText *p = m_pGui->addStaticText(std::get<1>(l_tColumn).c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
              p->setDrawBackground(true);
              p->setOverrideFont(l_pFont);
              p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
              l_cColPos.X += std::get<0>(l_tColumn) * l_iWidth / 100;
            }

            int l_iPos = 0;

            for (auto l_pResult: l_pRace->m_vRanking) {
              l_cRowPos.Y += l_iHeight;
              l_cColPos = l_cRowPos;

              m_vTable.push_back(std::vector<irr::gui::IGUIStaticText *>());

              std::wstring l_sName   = L"-";
              int          l_iAi     = -1;

              gameclasses::SPlayer *l_pPlayer = nullptr;

              for (auto p : l_pTournament->m_vPlayers) {
                if (p->m_iPlayer == l_pResult->m_iPlayer) {
                  l_pPlayer = p;
                  l_sName = helpers::s2ws(l_pPlayer->m_sName);
                  std::wstring l_sAiClass= L"";

                  if (helpers::splitStringNameBot(helpers::s2ws(l_pPlayer->m_sName), l_sName, l_sAiClass)) {
                    if (l_sAiClass == L"marble3")
                      l_iAi = 2;
                    else if (l_sAiClass == L"marble2")
                      l_iAi = 1;
                    else if (l_sAiClass == L"marblegp")
                      l_iAi = 0;
                  }
                  break;
                }
              }

              if (l_pPlayer == nullptr) {
                printf("Oops\n");
                return;
              }

              gameclasses::SRaceData *l_pData = nullptr;

              for (auto p : l_pRace->m_vRanking) {
                if (p->m_iPlayer == l_pPlayer->m_iPlayer) {
                  l_pData = p;
                  break;
                }
              }

              if (l_pData == nullptr) {
                printf("No race data found for player \"%s\".\n", l_pPlayer->m_sName.c_str());
                return;
              }

              std::wstring l_sDeficit = L"";

              if (l_iPos == 0) {
                l_sDeficit = helpers::convertToTime(l_pData->m_vLapCheckpoints.back().back());
              }
              else {
                if (l_pData->m_bWithdrawn)
                  l_sDeficit = L"D.N.F.";
                else if (l_pData->m_iDiffLeader < 0) {
                  if (l_pData->m_iDiffLeader == -1)
                    l_sDeficit = L"+1 Lap";
                  else {
                    l_sDeficit = L"+" + std::to_wstring(std::abs(l_pData->m_iDiffLeader)) + L" Laps";
                  }
                }
                else {
                  l_sDeficit = L"+" + helpers::convertToTime(l_pData->m_iDiffLeader);
                }
              }

              for (auto l_tColumn : l_vColums) {
                std::wstring l_sText =
                  std::get<2>(l_tColumn) == enColumn::Pos       ? std::to_wstring(l_iPos + 1) :
                  std::get<2>(l_tColumn) == enColumn::Name      ? l_sName    :
                  std::get<2>(l_tColumn) == enColumn::Racetime  ? l_sDeficit :
                  std::get<2>(l_tColumn) == enColumn::BestLap   ? helpers::convertToTime(l_pData->m_iFastestLap) :
                  std::get<2>(l_tColumn) == enColumn::BestLapNo ? std::to_wstring(l_pData->m_iFastestLapNo) + L" of " + std::to_wstring(l_pData->m_vLapCheckpoints.size() - 1) :
                  std::get<2>(l_tColumn) == enColumn::Respawn   ? std::to_wstring((int)l_pData->m_vRespawn.size()) + L" " :
                  std::get<2>(l_tColumn) == enColumn::Stunned   ? std::to_wstring((int)l_pData->m_vStunned.size()) + L" " : L"";

                if (std::get<2>(l_tColumn) == enColumn::Name) {
                  irr::core::recti l_cRect = irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100 - 3 * l_iHeight / 2, l_iHeight));

                  irr::gui::IGUIStaticText *p = m_pGui->addStaticText(l_sText.c_str(), l_cRect, true, true);
                  p->setTextAlignment(std::get<3>(l_tColumn), irr::gui::EGUIA_CENTER);
                  p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
                  p->setDrawBackground(false);
                  p->setOverrideFont(l_pFont);
                  m_vTable.back().push_back(p);

                  irr::core::recti l_cNumber = irr::core::recti(l_cRect.LowerRightCorner.X, l_cRect.UpperLeftCorner.Y, l_cRect.LowerRightCorner.X + 3 * l_iHeight / 2, l_cRect.LowerRightCorner.Y);
                  p = m_pGui->addStaticText(L"66", l_cNumber, true, true);
                  p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
                  p->setOverrideFont(l_pFont);

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
                else {
                  irr::gui::IGUIStaticText *p = m_pGui->addStaticText(l_sText.c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(std::get<0>(l_tColumn) * l_iWidth / 100, l_iHeight)), true, true);
                  p->setTextAlignment(std::get<3>(l_tColumn), irr::gui::EGUIA_CENTER);
                  p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
                  p->setDrawBackground(false);
                  p->setOverrideFont(l_pFont);
                  l_cColPos.X += std::get<0>(l_tColumn) * l_iWidth / 100;

                  if (std::get<2>(l_tColumn) == enColumn::AiClass) {
                    switch (l_iAi) {
                      case 0: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0x20, 0x00)); break;
                      case 1: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0x80, 0x00)); break;
                      case 2: p->setBackgroundColor(irr::video::SColor(0x80, 0xff, 0xff, 0x00)); break;
                    }
                  }
                  else {
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

          m_pAuto = new helpers::CAutoMenu(m_pDevice, this);
        }

        virtual ~CMenuRaceResult() {
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

                if (m_pServer != nullptr) {
                  m_pServer->changeState(m_pManager->peekMenuStack());
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

        virtual bool run() override { 
          if (m_pAuto != nullptr)
            m_pAuto->process();

          return false;
        }
      };



      IMenuHandler* createMenuRaceResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuRaceResult(a_pDevice, a_pManager, a_pState);
      }
  }
}