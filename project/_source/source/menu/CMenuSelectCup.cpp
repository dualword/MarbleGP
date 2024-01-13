// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gameclasses/SPlayer.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <gui/CSelector.h>
#include <CGlobal.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace menu {
    /**
    * @class CMenuSelectCup
    * @author Christian Keimel
    * This menu allows the user to select a cup for racing
    */
    class CMenuSelectCup : public IMenuHandler {
      private:
        gui::CSelector *m_pCups;
        gui::CSelector *m_pLaps;
        int m_iSelectedCup;
        int m_iCurrent;

        std::vector<dustbin::data::SMarbleGpCup> m_vCups;
        std::map<std::string, std::string> m_mTrackNames;

        irr::ITimer *m_pTimer;
        irr::u32 m_iTime;

        gui::CDustbinCheckbox *m_aMode[3];
        int m_iMode;

        void updateCupData() {
          if (m_pCups != nullptr && m_iSelectedCup >= 0 && m_iSelectedCup < m_vCups.size()) {
            if (m_iSelectedCup >= 0 && m_iSelectedCup < m_vCups.size()) {
              std::tuple<std::string, std::string> l_aData[] = {
                std::make_tuple("label_name"       , m_vCups[m_iSelectedCup].m_sName),
                std::make_tuple("label_description", m_vCups[m_iSelectedCup].m_sDescription),
                std::make_tuple("", "")
              };

              for (int i = 0; std::get<0>(l_aData[i]) != ""; i++) {
                irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType(std::get<0>(l_aData[i]).c_str(), irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));
                if (p != nullptr)
                  p->setText(helpers::s2ws(std::get<1>(l_aData[i])).c_str());
              }
            }

            m_iCurrent = -1;
            m_iTime = 0;
          }
        }

        void updateCurrentTrack() {
          std::string l_sTrack = std::get<0>(m_vCups[m_iSelectedCup].m_vRaces[m_iCurrent]);

          std::wstring s = L"Race " + std::to_wstring(m_iCurrent + 1) + L" of " + std::to_wstring(m_vCups[m_iSelectedCup].m_vRaces.size()) + L": ";
          s += helpers::s2ws(m_mTrackNames[l_sTrack]) + L" (";

          bool l_bSingleLap = false;

          switch (m_iMode) {
            case 0: s += std::to_wstring(std::get<1>(m_vCups[m_iSelectedCup].m_vRaces[m_iCurrent])); l_bSingleLap = std::get<1>(m_vCups[m_iSelectedCup].m_vRaces[m_iCurrent]) == 1; break;
            case 1: s += L"1"; l_bSingleLap = true; break;
            case 2:
              if (m_pLaps != nullptr)
                s += m_pLaps->getSelectedItem();
              else
                s += L"X";

              l_bSingleLap = m_pLaps->getSelected() == 0;
              break;
          }

          if (l_bSingleLap)
            s += L" Lap)";
          else
            s += L" Laps)";

          irr::gui::IGUIStaticText *p = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("label_raceno", irr::gui::EGUIET_STATIC_TEXT, m_pGui->getRootGUIElement()));

          if (p != nullptr)
            p->setText(s.c_str());

          irr::gui::IGUIImage *l_pImg = reinterpret_cast<irr::gui::IGUIImage *>(helpers::findElementByNameAndType("Thumbnail", irr::gui::EGUIET_IMAGE, m_pGui->getRootGUIElement()));
          if (l_pImg != nullptr) {
            std::string l_sImg = "data/levels/" + std::get<0>(m_vCups[m_iSelectedCup].m_vRaces[m_iCurrent]) + "/thumbnail.png";
            if (m_pFs->existFile(l_sImg.c_str()))
              l_pImg->setImage(m_pDrv->getTexture(l_sImg.c_str()));
            else
              l_pImg->setImage(m_pDrv->getTexture("data/images/no_image.png"));
          }
        }

      public:
        CMenuSelectCup(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState) : IMenuHandler(a_pDevice, a_pManager, a_pState), m_pCups(nullptr), m_pLaps(nullptr), m_iSelectedCup(0), m_iCurrent(0), m_pTimer(nullptr), m_iTime(0), m_iMode(0) {
          m_pState->getGlobal()->clearGui();

          helpers::loadMenuFromXML("data/menu/menu_selectcup.xml", m_pGui->getRootGUIElement(), m_pGui);
          m_pSmgr->clear();
          m_pSmgr->loadScene("data/scenes/skybox.xml");
          m_pSmgr->addCameraSceneNode();

          m_mTrackNames = helpers::getTrackNameMap();

          m_vCups = helpers::loadCupDefinitions();

          m_iSelectedCup = std::atoi(CGlobal::getInstance()->getSetting("selectedcup").c_str());;
          m_pCups = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("CupSelector", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

          if (m_pCups != nullptr) {
            for (auto l_cCup : m_vCups) {
              m_pCups->addItem(helpers::s2ws(l_cCup.m_sName).c_str());
            }
            m_pCups->setSelected(m_iSelectedCup);
            updateCupData();
          }

          m_pLaps = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("nolaps", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, m_pGui->getRootGUIElement()));

          m_iMode = std::atoi(CGlobal::getInstance()->getSetting("cupmode").c_str());

          if (m_pLaps != nullptr) {
            std::string l_sLaps = m_pState->getGlobal()->getSetting("laps");
            m_pLaps->setSelected(std::atoi(l_sLaps.c_str()) - 1);
            m_pLaps->setEnabled(m_iMode == 2);
          }

          for (int i = 0; i < 3; i++) {
            m_aMode[i] = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("cm_" + std::to_string(i), (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement()));
            if (m_aMode[i] != nullptr)
              m_aMode[i]->setChecked(i == m_iMode);
          }

          m_pTimer = m_pDevice->getTimer();
          m_pTimer->start();

          m_iTime = m_pTimer->getRealTime();
        }

        virtual ~CMenuSelectCup() {
        }

        /**
        * This method is called every frame after "scenemanager::drawall" is called
        * @return true if drawing was done by the menu, false otherwise
        */
        virtual bool run() override {
          irr::u32 l_iTime = m_pTimer->getRealTime();

          if ((m_iCurrent < 0 || l_iTime - m_iTime > 2000) && m_iSelectedCup >= 0 && m_iSelectedCup < m_vCups.size()) {
            m_iCurrent++;
            if (m_iCurrent < 0 || m_iCurrent >= m_vCups[m_iSelectedCup].m_vRaces.size())
              m_iCurrent = 0;

            if (m_iCurrent < m_vCups[m_iSelectedCup].m_vRaces.size()) {
              updateCurrentTrack();
            }
            m_iTime = l_iTime;
          }

          return false;
        }

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override {
          bool l_bRet = false;

          if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
            std::string l_sSender = a_cEvent.GUIEvent.Caller->getName();

            if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
              if (l_sSender == "cancel") {
                l_bRet = true;
                createMenu("menu_main", m_pDevice, m_pManager, m_pState);
              }
              else if (l_sSender == "ok") {
                if (m_iSelectedCup >= 0 && m_iSelectedCup < m_vCups.size()) {
                  if (m_iMode == 1 || m_iMode == 2) {
                    for (auto& l_cRace : m_vCups[m_iSelectedCup].m_vRaces) {
                      std::get<1>(l_cRace) = m_iMode == 1 ? 1 : m_pLaps != nullptr ? m_pLaps->getSelected() + 1 : 2;
                    }
                  }

                  std::string l_sSerialized = m_vCups[m_iSelectedCup].serialize();

                  gameclasses::STournament *l_pTournament = m_pState->getGlobal()->getTournament();

                  int l_iNum = 1;
                  for (auto l_tRace : m_vCups[m_iSelectedCup].m_vRaces) {
                    int l_iLaps = m_iMode == 1 ? 1 : (m_iMode == 2 && m_pLaps != nullptr) ? m_pLaps->getSelected() + 1 : std::get<1>(l_tRace);
                    l_pTournament->m_vRaces.push_back(
                      new gameclasses::SRace(
                        std::get<0>(l_tRace),
                        "\"" + m_vCups[m_iSelectedCup].m_sName + "\" Race #" + std::to_string(l_iNum) + " of " + std::to_string(m_vCups[m_iSelectedCup].m_vRaces.size()),
                        l_iLaps,
                        l_pTournament
                      )
                    );
                    l_iNum++;
                  }

                  printf("\n\n%s\n\n", l_sSerialized.c_str());
                  m_pState->getGlobal()->setGlobal("current_cup", l_sSerialized);
                  createMenu("menu_nextrace", m_pDevice, m_pManager, m_pState);
                }
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
              if (l_sSender == "CupSelector" && m_pCups != nullptr) {
                m_iSelectedCup = m_pCups->getSelected();
                CGlobal::getInstance()->setSetting("selectedcup", std::to_string(m_iSelectedCup));
                updateCupData();
              }
              else if (l_sSender == "nolaps") {
                updateCurrentTrack();
              }
            }
            else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
              if (l_sSender.substr(0, 3) == "cm_") {
                for (int i = 0; i < 3; i++) {
                  if (m_aMode[i] != nullptr) {
                    bool l_bChecked = l_sSender == m_aMode[i]->getName();
                    m_aMode[i]->setChecked(l_bChecked);
                    if (l_bChecked) m_iMode = i;
                    updateCurrentTrack();
                  }

                  CGlobal::getInstance()->setSetting("cupmode", std::to_string(m_iMode));

                  if (m_pLaps != nullptr)
                    m_pLaps->setEnabled(m_iMode == 2);
                }
              }
            }
          }

          return l_bRet;
        }
    };

    IMenuHandler *createMenuSelectCup(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState) {
      return new CMenuSelectCup(a_pDevice, a_pManager, a_pState);
    }
  }
}