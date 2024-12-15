// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCommandHandler.h>
#include <commands/CCmdExportMesh.h>
#include <commands/CCmdSaveTrack.h>
#include <commands/CCmdLoadTrack.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdMainMenu.h>

namespace dustbin {
  namespace commands {
    CCmdMainMenu::CCmdMainMenu(CCommandHandler* a_pHandler, irr::gui::IGUIEnvironment* a_pGui) :
      ICommand    ("CmdMainMenu"),
      m_pHandler  (a_pHandler),
      m_pEditTrack(nullptr),
      m_pGui      (a_pGui),
      m_pEditMode (nullptr),
      m_pWarning  (nullptr)
    {
      onUiSetupModified();
    }

    CCmdMainMenu::~CCmdMainMenu() {

    }

    /**
    * Set the edit mode text
    * @param a_sText the new edit mode text
    */
    void CCmdMainMenu::setEditMode(const std::wstring &a_sText) {
      if (m_pEditMode != nullptr)
        m_pEditMode->setText(a_sText.c_str());
    }

    /**
    * Set the warning text
    * @param a_sWarning the new warning text
    */
    void CCmdMainMenu::setWarningText(const std::wstring &a_sWarning) {
      if (m_pWarning != nullptr) {
        m_pWarning->setText(a_sWarning.c_str());
        m_pWarning->setVisible(true);
      }
    }

    /**
    * Hide the warning label
    */
    void CCmdMainMenu::hideWarning() {
      if (m_pWarning != nullptr)
        m_pWarning->setVisible(false);
    }

    /**
    * Set the edit track command
    * @param a_pCmd the edit track command
    */
    void CCmdMainMenu::setEditTrackCommand(CCmdEditTrack *a_pCmd) {
      m_pEditTrack = a_pCmd;
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdMainMenu::isFinished() {
      return false;
    }

    const std::wstring CCmdMainMenu::getButtonText(enMenuItem a_eButton) {
      switch (a_eButton) {
        case enMenuItem::NewTrack  : return L"New Track";
        case enMenuItem::Load      : return L"Load Track";
        case enMenuItem::Save      : return L"Save Track";
        case enMenuItem::SaveAs    : return L"Save Track as";
        case enMenuItem::Base      : return L"Create Base";
        case enMenuItem::ExportMesh: return L"Export as Mesh";

        default:
          return L"--";
      }
    }

    /**
    * Callback method for changes in the UI like modifying
    * font size or resizin window
    */
    void CCmdMainMenu::onUiSetupModified() {
      if (m_pEditMode != nullptr) {
        m_pEditMode->setVisible(false);
        m_pGui->addToDeletionQueue(m_pEditMode);
      }

      if (m_pWarning != nullptr) {
        m_pWarning->setVisible(false);
        m_pGui->addToDeletionQueue(m_pWarning);
      }

      for (auto l_tButton: m_mButtons) {
        l_tButton.second->setVisible(false);
        m_pGui->addToDeletionQueue(l_tButton.second);
      }

      m_mButtons.clear();

      irr::core::vector2di l_cBtnPos = irr::core::vector2di();

      irr::s32 l_iSpace = m_pGui->getSkin()->getFont()->getDimension(L"-").Width;

      for (int i = 0; i < (int)enMenuItem::Count; i++) {
        std::wstring l_sText = getButtonText((enMenuItem)i);

        if (l_sText != L"--") {
          irr::core::dimension2du l_cBtnSize = m_pGui->getSkin()->getFont()->getDimension(l_sText.c_str());

          l_cBtnSize.Width  = 5 * l_cBtnSize.Width / 4;
          l_cBtnSize.Height = 5 * l_cBtnSize.Height / 4;

          irr::gui::IGUIButton *l_pBtn = m_pGui->addButton(
            irr::core::recti(l_cBtnPos, l_cBtnSize),
            m_pGui->getRootGUIElement(),
            -1,
            l_sText.c_str()
          );

          l_cBtnPos.X += l_cBtnSize.Width + l_iSpace;

          m_mButtons[(enMenuItem)i] = l_pBtn;
        }

        if (l_sText == L"--")
          l_cBtnPos.X += 2 * l_iSpace;
      }

      irr::core::dimension2du l_cSize = m_pGui->getSkin()->getFont()->getDimension(L"Edit Mode: This Text is longer than needed.");
      irr::core::vector2di    l_cPos  = irr::core::vector2di();

      irr::s32 l_iHeight = 5 * l_cSize.Height / 4;
      l_cPos.Y = l_iHeight;

      m_pEditMode = m_pGui->addStaticText(L"Edit Mode: Default", irr::core::recti(l_cPos, l_cSize));
      
      l_cPos.Y += 6 * l_cSize.Height / 5;

      m_pWarning = m_pGui->addStaticText(L"Warning Label", irr::core::recti(l_cPos, l_cSize));
      m_pWarning->setOverrideColor(irr::video::SColor(0xFF, 0x80, 0x20, 0x20));
      m_pWarning->setVisible(false);
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdMainMenu::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          for (auto l_tBtn: m_mButtons) {
            if (a_cEvent.GUIEvent.Caller == l_tBtn.second) {
              switch (l_tBtn.first) {
                case enMenuItem::NewTrack: printf("New Track\n"); break;
                case enMenuItem::Load: {
                  commands::CCmdLoadTrack *l_pCmd = new commands::CCmdLoadTrack("testtrack.dbt", m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }
                case enMenuItem::Save: {
                  commands::CCmdSaveTrack *l_pCmd = new commands::CCmdSaveTrack("testtrack.dbt", m_pEditTrack->getTrack());
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                case enMenuItem::SaveAs  : printf("Save Track as\n"); break;

                case enMenuItem::Base:
                  m_pEditTrack->setBaseParameters(2.5f, 2.5f);
                  break;

                case enMenuItem::ExportMesh: {
                  commands::CCmdExportMesh *l_pCmd = new commands::CCmdExportMesh("testtrack.stl", m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                default:
                  printf("--\n");
              }
            }
          }
        }
      }

      return l_bRet;
    }

  }
}