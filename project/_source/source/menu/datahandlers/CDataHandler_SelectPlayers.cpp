#include <menu/datahandlers/CDataHandler_SelectPlayers.h>
#include <messages/CMessageHelpers.h>
#include <helpers/CStringHelpers.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gui/CReactiveLabel.h>
#include <menu/IMenuHandler.h>
#include <data/CDataStructs.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace menu {
    CDataHandler_SelectPlayers::CDataHandler_SelectPlayers(std::vector<data::SPlayerData> &a_vProfiles, const std::string &a_sSelected) :
      IMenuDataHandler(),
      m_vProfiles       (a_vProfiles),
      m_sSelected       (a_sSelected)
    {
      printf("Selected Players: \"%s\"\n", m_sSelected.c_str());
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
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          if (l_sCaller == "PlayerSelect") {

#ifdef _ANDROID
            if (reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked()) {
              std::vector<irr::gui::IGUIElement *> l_vPlayers;

              helpers::findAllElementsByNameAndType("PlayerSelect", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, m_pGui->getRootGUIElement(), l_vPlayers);

              for (auto l_cPlayer : l_vPlayers) {
                gui::CDustbinCheckbox *p = reinterpret_cast<gui::CDustbinCheckbox *>(l_cPlayer);
                p->setChecked(p == a_cEvent.GUIEvent.Caller);
              }
            }
#endif

            m_sSelected = "";

            std::vector<irr::gui::IGUIElement *> l_vTabs;
            helpers::findAllElementsByNameAndType("PlayerTab", irr::gui::EGUIET_TAB, CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), l_vTabs);

            for (auto l_pTab : l_vTabs) {
              gui::CDustbinCheckbox *l_pCheck = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("PlayerSelect", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pTab));
              if (l_pCheck != nullptr) {
                if (l_pCheck->isChecked()) {
                  gui::CReactiveLabel* l_pName = reinterpret_cast<gui::CReactiveLabel*>(helpers::findElementByNameAndType("PlayerName", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_ReactiveLabelId, l_pTab));

                  if (l_pName != nullptr) {
                    if (m_sSelected != "")
                      m_sSelected += ";";

                    m_sSelected += messages::urlEncode(helpers::ws2s(l_pName->getText()));
                  }
                }
              }
            }

            gui::CDustbinCheckbox *l_pBox = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller);
            irr::gui::IGUIStaticText *l_pName = reinterpret_cast<irr::gui::IGUIStaticText *>(helpers::findElementByNameAndType("PlayerName", irr::gui::EGUIET_STATIC_TEXT, l_pBox->getParent()));

            if (l_pName != nullptr) {
              std::string l_sName = helpers::ws2s(l_pName->getText());

              if (l_pBox->isChecked()) {
                printf("Add player \"%s\" to raceplayers\n", l_sName.c_str());
              }
              else {
                printf("Remove player \"%s\" from list\n", l_sName.c_str());
              }
            }
            else printf("Name field not found.\n");
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

    /**
    * Get the number of selected players
    * @return the number of selected players
    */
    int CDataHandler_SelectPlayers::getNumberOfSelectedPlayers() {
      return (int)helpers::splitString(m_sSelected, ';').size();
    }
  }
}