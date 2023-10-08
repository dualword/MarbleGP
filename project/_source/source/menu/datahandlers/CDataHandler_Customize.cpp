#include <menu/datahandlers/CDataHandler_Customize.h>
#include <gui/CDustbinCheckbox.h>
#include <helpers/CMenuLoader.h>
#include <gui/CSelector.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {

    CDataHandler_Customize::CDataHandler_Customize(data::SGameSettings a_cSettings) :
      IMenuDataHandler(),
      m_cSettings     (data::SGameSettings(a_cSettings))
    {
      irr::gui::IGUIEnvironment *l_pGui = CGlobal::getInstance()->getGuiEnvironment();

      gui::CSelector        *l_pSelector = nullptr;
      gui::CDustbinCheckbox *l_pCheckbox = nullptr;

      l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("CustomReverseGrid", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pGui->getRootGUIElement()));
      if (l_pCheckbox != nullptr)
        l_pCheckbox->setChecked(m_cSettings.m_bReverseGrid);

      l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("CustomFillGrid", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pGui->getRootGUIElement()));
      if (l_pCheckbox != nullptr)
        l_pCheckbox->setChecked(m_cSettings.m_bFillGridAI);

      l_pCheckbox = reinterpret_cast<gui::CDustbinCheckbox *>(helpers::findElementByNameAndType("CustomRandomizeFirst", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_DustbinCheckboxId, l_pGui->getRootGUIElement()));
      if (l_pCheckbox != nullptr)
        l_pCheckbox->setChecked(m_cSettings.m_bRandomFirstRace);

      l_pSelector = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("CustomGridPos", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pGui->getRootGUIElement()));
      if (l_pSelector != nullptr)
        l_pSelector->setSelected((int)m_cSettings.m_eGridPos);

      l_pSelector = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("CustomGridSize", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pGui->getRootGUIElement()));
      if (l_pSelector != nullptr)
        l_pSelector->setSelected(m_cSettings.m_iGridSize);

      l_pSelector = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("CustomAiClass", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pGui->getRootGUIElement()));
      if (l_pSelector != nullptr)
        l_pSelector->setSelected((int)m_cSettings.m_eRaceClass);

      l_pSelector = reinterpret_cast<gui::CSelector *>(helpers::findElementByNameAndType("CustomAutoFinish", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_SelectorId, l_pGui->getRootGUIElement()));
      if (l_pSelector != nullptr)
        l_pSelector->setSelected((int)m_cSettings.m_eAutoFinish);
    }

    CDataHandler_Customize::~CDataHandler_Customize() {
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_Customize::handleIrrlichtEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        std::string l_sCaller = a_cEvent.GUIEvent.Caller->getName();

        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          if (l_sCaller == "CustomReverseGrid") {
            m_cSettings.m_bReverseGrid = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked();
          }
          else if (l_sCaller == "CustomFillGrid") {
            m_cSettings.m_bFillGridAI = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked();
          }
          else if (l_sCaller == "CustomRandomizeFirst") {
            m_cSettings.m_bRandomFirstRace = reinterpret_cast<gui::CDustbinCheckbox *>(a_cEvent.GUIEvent.Caller)->isChecked();
          }
          else printf("Checkbox: %s\n", l_sCaller.c_str());
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_SCROLL_BAR_CHANGED) {
          if (l_sCaller == "CustomGridPos") {
            m_cSettings.m_eGridPos = (data::SGameSettings::enGridPos)reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected();
          }
          else if (l_sCaller == "CustomGridSize") {
            m_cSettings.m_iGridSize = reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected();
          }
          else if (l_sCaller == "CustomAiClass") {
            m_cSettings.m_eRaceClass = (data::SGameSettings::enRaceClass)reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected();
          }
          else if (l_sCaller == "CustomAutoFinish") {
            m_cSettings.m_eAutoFinish = (data::SGameSettings::enAutoFinish)reinterpret_cast<gui::CSelector *>(a_cEvent.GUIEvent.Caller)->getSelected();
          }
          else printf("Scrollbar: %s\n", l_sCaller.c_str());
        }
      }

      return l_bRet;
    }

    const data::SGameSettings &CDataHandler_Customize::getSettings() {
      return m_cSettings;
    }
  }
}