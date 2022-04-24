// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CMenuLoader.h>
#include <data/CDataStructs.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
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

      public:
        CMenuRaceResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState) {
          m_pGui ->clear();
          m_pSmgr->clear();

          helpers::loadMenuFromXML("data/menu/menu_raceresult.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_pState->setZLayer(0);

          data::SChampionship l_cChampionship = data::SChampionship(m_pState->getGlobal()->getGlobal("championship"));
          data::SChampionshipRace *l_pRace = l_cChampionship.getLastRace();

          printf("\n\n%s\n\n", l_cChampionship.to_string().c_str());

          irr::gui::IGUIStaticText *l_pTrack = reinterpret_cast<irr::gui::IGUIStaticText*>(findElementByNameAndType("label_trackname", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

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

          irr::gui::IGUIStaticText *l_pLaps = reinterpret_cast<irr::gui::IGUIStaticText*>(findElementByNameAndType("label_laps", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pLaps != nullptr) {
            l_pLaps->setText(std::to_wstring(l_pRace->m_iLaps).c_str());
          }

          irr::gui::IGUIStaticText *l_pPlayers = reinterpret_cast<irr::gui::IGUIStaticText*>(findElementByNameAndType("label_players", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (l_pPlayers != nullptr) {
            l_pPlayers->setText(std::to_wstring(l_pRace->m_iPlayers).c_str());
          }

          irr::gui::IGUIImage *l_pThumbnail = reinterpret_cast<irr::gui::IGUIImage *>(findElementByNameAndType("thumbnail", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
          if (l_pThumbnail != nullptr) {
            irr::io::IFileSystem *l_pFs = m_pGui->getFileSystem();
            std::string l_sPath = "data/levels/" + l_pRace->m_sTrack + "/thumbnail.png";
            if (l_pFs->existFile(l_sPath.c_str())) {
              l_pThumbnail->setImage(m_pDrv->getTexture(l_sPath.c_str()));
            }
          }

          irr::gui::IGUITab *l_pTab = reinterpret_cast<irr::gui::IGUITab *>(findElementByNameAndType("tab_result", irr::gui::EGUIET_TAB, m_pGui->getRootGUIElement()));

          if (l_pTab != nullptr) {
            irr::core::position2di l_cRowPos = l_pTab->getAbsoluteClippingRect().UpperLeftCorner;
            irr::core::position2di l_cColPos = l_cRowPos;

#ifdef _ANDROID
            irr::gui::IGUIFont *l_pFont  = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
#else
            irr::gui::IGUIFont *l_pFont  = m_pState->getGlobal()->getFont(enFont::Small, m_pDrv->getScreenSize());
#endif

            irr::u32 l_iHeight = 5 * l_pFont->getDimension(L"Hello World").Height / 4;
            irr::u32 l_iWidth  = l_pTab->getAbsoluteClippingRect().getWidth();

            irr::gui::IGUIStaticText *p = m_pGui->addStaticText(L"Pos", irr::core::recti(l_cColPos, irr::core::dimension2du( 5 * l_iWidth / 100, l_iHeight)), true, true);
            p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            p->setDrawBackground(true);
            p->setOverrideFont(l_pFont);
            p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
            l_cColPos.X += 5 * l_iWidth / 100;

            p = m_pGui->addStaticText(L"Name", irr::core::recti(l_cColPos, irr::core::dimension2du(45 * l_iWidth / 100, l_iHeight)), true, true);
            p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            p->setDrawBackground(true);
            p->setOverrideFont(l_pFont);
            p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
            l_cColPos.X += 45 * l_iWidth / 100;

            p = m_pGui->addStaticText(L"Deficit", irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
            p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            p->setDrawBackground(true);
            p->setOverrideFont(l_pFont);
            p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
            l_cColPos.X += 15 * l_iWidth / 100;

            p = m_pGui->addStaticText(L"Stunned", irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
            p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            p->setDrawBackground(true);
            p->setOverrideFont(l_pFont);
            p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
            l_cColPos.X += 15 * l_iWidth / 100;

            p = m_pGui->addStaticText(L"Respawn", irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
            p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
            p->setDrawBackground(true);
            p->setOverrideFont(l_pFont);
            p->setBackgroundColor(irr::video::SColor(0xFF, 192, 192, 192));
            l_cColPos.X += 15 * l_iWidth / 100;


            for (int i = 0; i < l_pRace->m_iPlayers && i < 16; i++) {
              l_cRowPos.Y += l_iHeight;
              l_cColPos = l_cRowPos;

              m_vTable.push_back(std::vector<irr::gui::IGUIStaticText *>());

              std::wstring l_sName = L"-";

              if (l_pRace->m_mAssignment.find(l_pRace->m_aResult[i].m_iId) != l_pRace->m_mAssignment.end()) {
                int l_iPlayer = l_pRace->m_mAssignment[l_pRace->m_aResult[i].m_iId];
                for (std::vector<data::SChampionshipPlayer>::iterator it = l_cChampionship.m_vPlayers.begin(); it != l_cChampionship.m_vPlayers.end(); it++) {
                  if ((*it).m_iPlayerId == l_iPlayer) {
                    l_sName = helpers::s2ws((*it).m_sName);
                    break;
                  }
                }
              }

              std::wstring l_sDeficit = L"";

              if (i > 0) {
                if (l_pRace->m_aResult[i].m_bWithdrawn)
                  l_sDeficit = L"D.N.F.";
                else if (l_pRace->m_aResult[i].m_iDeficitL < 0) {
                  if (l_pRace->m_aResult[i].m_iDeficitL == -1)
                    l_sDeficit = L"+1 Lap";
                  else {
                    l_sDeficit = L"+" + std::to_wstring(std::abs(l_pRace->m_aResult[i].m_iDeficitL)) + L" Laps";
                  }
                }
                else {
                  wchar_t s[0xFF];
                  swprintf(s, 0xFF, L"+%.2f sec", ((irr::f32)l_pRace->m_aResult[i].m_iDeficitL) / 120.0f);
                  l_sDeficit = s;
                }
              }

              p = m_pGui->addStaticText(std::to_wstring(i + 1).c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(5 * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
              p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
              p->setDrawBackground(false);
              p->setOverrideFont(l_pFont);
              l_cColPos.X += 5 * l_iWidth / 100;

              m_vTable.back().push_back(p);

              p = m_pGui->addStaticText((L" " + l_sName).c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(45 * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);
              p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
              p->setDrawBackground(false);
              p->setOverrideFont(l_pFont);
              l_cColPos.X += 45 * l_iWidth / 100;

              m_vTable.back().push_back(p);

              p = m_pGui->addStaticText((l_sDeficit + L" ").c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
              p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
              p->setDrawBackground(false);
              p->setOverrideFont(l_pFont);
              l_cColPos.X += 15 * l_iWidth / 100;

              m_vTable.back().push_back(p);

              p = m_pGui->addStaticText((std::to_wstring(l_pRace->m_aResult[i].m_iStunned) + L" ").c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
              p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
              p->setDrawBackground(false);
              p->setOverrideFont(l_pFont);
              l_cColPos.X += 15 * l_iWidth / 100;

              m_vTable.back().push_back(p);

              p = m_pGui->addStaticText((std::to_wstring(l_pRace->m_aResult[i].m_iRespawn) + L" ").c_str(), irr::core::recti(l_cColPos, irr::core::dimension2du(15 * l_iWidth / 100, l_iHeight)), true, true);
              p->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);
              p->setBackgroundColor(irr::video::SColor(128, 192, 192, 96));
              p->setDrawBackground(false);
              p->setOverrideFont(l_pFont);
              l_cColPos.X += 15 * l_iWidth / 100;

              m_vTable.back().push_back(p);
            }
          }
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              std::string l_sButton = a_cEvent.GUIEvent.Caller->getName();

              if (l_sButton == "ok") {
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



      IMenuHandler* createMenuRaceResult(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
        return new CMenuRaceResult(a_pDevice, a_pManager, a_pState);
      }
  }
}