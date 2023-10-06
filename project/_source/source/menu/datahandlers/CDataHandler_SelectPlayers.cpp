#include <menu/datahandlers/CDataHandler_SelectPlayers.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace menu {
    CDataHandler_SelectPlayers::CDataHandler_SelectPlayers(data::SRacePlayers *a_pPlayers, data::SChampionship *a_pChampionship, const std::string &a_sSelected) :
      IMenuDataHandler(),
      m_pPlayers      (a_pPlayers),
      m_pChampionship(a_pChampionship),
      m_sSelected    (a_sSelected)
    {
    }

    CDataHandler_SelectPlayers::~CDataHandler_SelectPlayers() {
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_SelectPlayers::handleIrrlichtEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (l_sCaller == "ok") {
            CGlobal::getInstance()->setSetting("selectedplayers", m_sSelected);
          }
          else printf("Button clicked: \"%s\"\n", l_sCaller.c_str());
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          if (l_sCaller == "PlayerSelect") {
            m_sSelected = "";

            std::vector<irr::gui::IGUIElement *> l_vTabs;
            helpers::findAllElementsByNameAndType("PlayerTab", irr::gui::EGUIET_TAB, CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), l_vTabs);

            for (auto l_pTab : l_vTabs) {
              gui::CDustbinCheckbox *l_pCheck = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("PlayerSelect", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pTab));
              if (l_pCheck != nullptr && l_pCheck->isChecked()) {
                irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerName", irr::gui::EGUIET_STATIC_TEXT, l_pTab));

                if (l_pName != nullptr) {
                  if (m_sSelected != "")
                    m_sSelected += ";";

                  m_sSelected += messages::urlEncode(helpers::ws2s(l_pName->getText()));
                }
              }
            }

            printf("Selected Players: \"%s\"\n", m_sSelected.c_str());
            l_bRet = true;
          }
          else printf("Checkbox changed: \"%s\"\n", l_sCaller.c_str());
        }
      }

      return l_bRet;
    }

    /**
    * Get a vector with the names of the selected players
    * @return a vector with the names of the selected players
    */
    std::vector<std::string> CDataHandler_SelectPlayers::getSelectedPlayers() {
      std::vector<std::string> l_vDummy = helpers::splitString(m_sSelected, ';');
      std::vector<std::string> l_vReturn;

      for (auto s : l_vDummy) {
        l_vReturn.push_back(messages::urlDecode(s));
      }

      return l_vReturn;
    }
  }
}