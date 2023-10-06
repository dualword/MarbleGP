#include <helpers/CMenuLoader.h>
#include <helpers/CAutoMenu.h>
#include <menu/IMenuHandler.h>
#include <gui/CMenuButton.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace helpers {
    CAutoMenu::CAutoMenu(irr::IrrlichtDevice* a_pDevice, menu::IMenuHandler* a_pRecveiver) : m_pTimer(a_pDevice->getTimer()), m_iTime(0), m_bBotsOnly(true), m_pReciever(a_pRecveiver) {
      m_iTime = m_pTimer->getRealTime();

      data::SRacePlayers l_cPlayers;
      std::string l_sPlayers = CGlobal::getInstance()->getGlobal("raceplayers");
      l_cPlayers.deserialize(l_sPlayers);

      for (auto l_cPlayer : l_cPlayers.m_vPlayers) {
        if (l_cPlayer.m_eType == data::enPlayerType::Local &&
          l_cPlayer.m_eAiHelp != data::SPlayerData::enAiHelp::BotMgp &&
          l_cPlayer.m_eAiHelp != data::SPlayerData::enAiHelp::BotMb2 &&
          l_cPlayer.m_eAiHelp != data::SPlayerData::enAiHelp::BotMb3)
        {
          m_bBotsOnly = false;
          break;
        }
      }
    }

    CAutoMenu::~CAutoMenu() {
    }

    /**
    * This method is called every "run" event of the menu and
    * sends a "button OK clicked" event to the receiver if
    * only bots are racing and 5 seconds have passed
    */
    void CAutoMenu::process() {
      irr::u32 l_iTime = m_pTimer->getRealTime();
      if (m_bBotsOnly && l_iTime - m_iTime > 5000 && m_pReciever != nullptr) {
        irr::gui::IGUIButton *l_pBtn = reinterpret_cast<irr::gui::IGUIButton *>(helpers::findElementByNameAndType("ok", (irr::gui::EGUI_ELEMENT_TYPE)gui::g_MenuButtonId, CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement()));

        if (l_pBtn != nullptr) {
          irr::SEvent l_cEvent;
          l_cEvent.EventType = irr::EET_GUI_EVENT;
          l_cEvent.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
          l_cEvent.GUIEvent.Caller    = l_pBtn;
          l_cEvent.GUIEvent.Element   = l_pBtn;

          m_pReciever->OnEvent(l_cEvent);
        }
      }
    }
  }
}