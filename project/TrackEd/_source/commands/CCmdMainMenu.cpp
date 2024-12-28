// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCommandHandler.h>
#include <commands/CCmdAddMarbleGP.h>
#include <commands/CCmdSaveTrackAs.h>
#include <commands/CCmdExportMesh.h>
#include <commands/CCmdSaveTrack.h>
#include <commands/CCmdLoadTrack.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdThumbnail.h>
#include <commands/CCmdTrackName.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdMainMenu.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    CCmdMainMenu::CCmdMainMenu(CCommandHandler* a_pHandler, irr::gui::IGUIEnvironment* a_pGui) :
      ICommand    ("CmdMainMenu"),
      m_pHandler  (a_pHandler),
      m_pEditTrack(nullptr),
      m_pGui      (a_pGui),
      m_pTimer    (tracked::IGlobal::getInstance()->getDevice()->getTimer()),
      m_pEditMode (nullptr)
    {
      onUiSetupModified();
    }

    CCmdMainMenu::~CCmdMainMenu() {
      m_vLog.clear();
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
    void CCmdMainMenu::addLogMessage(const std::wstring &a_sMessage, irr::ELOG_LEVEL a_eLevel, irr::u32 a_iTtl) {
      
      std::string  l_sMessage = helpers::ws2s(a_sMessage);
      std::wstring l_sMsgLong = a_sMessage;

      irr::core::dimension2du l_cTextSize = m_pGui->getSkin()->getFont()->getDimension(a_sMessage.c_str());
      irr::core::dimension2du l_cScreen   = m_pGui->getVideoDriver()->getScreenSize();

      while (l_cTextSize.Width > l_cScreen.Width / 5) {
        l_sMsgLong = l_sMsgLong.substr(0, l_sMessage.size() - 3) + L"..";
        l_cTextSize = m_pGui->getSkin()->getFont()->getDimension(a_sMessage.c_str());
      }

      irr::core::position2di l_cPos = (m_vLog.size() > 0 && std::get<1>(m_vLog.back()) != nullptr) ? irr::core::position2di(m_cLogPos.X, std::get<1>(m_vLog.back())->getAbsoluteClippingRect().LowerRightCorner.Y) : m_cLogPos;

      irr::gui::IGUIStaticText *l_pText = m_pGui->addStaticText(l_sMsgLong.c_str(), irr::core::recti(l_cPos, l_cTextSize));

      switch (a_eLevel) {
        case irr::ELL_WARNING: l_pText->setOverrideColor(irr::video::SColor(0xFF, 0xFF, 0xFF, 0x80)); break;
        case irr::ELL_ERROR  : l_pText->setOverrideColor(irr::video::SColor(0xFF, 0xFF,    0,    0)); break;
        default:
          break;
      }

      m_vLog.push_back(std::make_tuple(a_sMessage, l_pText, a_eLevel, a_iTtl, m_pTimer->getTime()));

      irr::SEvent a_cEvent;
      a_cEvent.EventType      = irr::EET_LOG_TEXT_EVENT;
      a_cEvent.LogEvent.Level = a_eLevel;
      a_cEvent.LogEvent.Text  = l_sMessage.c_str();
      tracked::IGlobal::getInstance()->getDevice()->postEventFromUser(a_cEvent);

    }

    /**
    * Set the edit track command
    * @param a_pCmd the edit track command
    */
    void CCmdMainMenu::setEditTrackCommand(CCmdEditTrack *a_pCmd) {
      m_pEditTrack = a_pCmd;
    }

    /**
    * This method is called on every frame.
    */
    void CCmdMainMenu::run() {
      if (m_vLog.size() > 0) {
        irr::u32 l_iTime = m_pTimer->getTime();

        std::vector<std::tuple<std::wstring, irr::gui::IGUIStaticText *, irr::ELOG_LEVEL, irr::u32, irr::u32>>::iterator l_itLog = m_vLog.end();
        do {
          l_itLog--;

          irr::u32 l_iTtl = std::get<3>(*l_itLog) + std::get<4>(*l_itLog);
          if (l_iTtl < l_iTime) {
            if (std::get<1>(*l_itLog) != nullptr) {
              std::get<1>(*l_itLog)->setVisible(false);

              irr::s32 l_iHeight = std::get<1>(*l_itLog)->getAbsoluteClippingRect().getHeight();

              for (auto l_tLog2 = l_itLog + 1; l_tLog2 != m_vLog.end(); l_tLog2++) {
                if (std::get<1>(*l_tLog2) != nullptr) {
                  std::get<1>(*l_tLog2)->setRelativePosition(std::get<1>(*l_tLog2)->getAbsolutePosition().UpperLeftCorner - irr::core::vector2di(0, l_iHeight));
                }
              }

              m_pGui->addToDeletionQueue(std::get<1>(*l_itLog));
              std::get<1>(*l_itLog) = nullptr;

              m_vLog.erase(l_itLog);
              l_itLog = m_vLog.end();

              if (m_vLog.size() == 0) break;
            }
          }
        } 
        while (l_itLog != m_vLog.begin());
      }
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
        case enMenuItem::NewTrack      : return L"New Track";
        case enMenuItem::Load          : return L"Load Track";
        case enMenuItem::Save          : return L"Save Track";
        case enMenuItem::SaveAs        : return L"Save Track as";
        case enMenuItem::Base          : return L"Create Base";
        case enMenuItem::ExportMesh    : return L"Export as Mesh";
        case enMenuItem::ExportMarbleGP: return L"Add to MarbleGP";
        case enMenuItem::Thumbnail     : return L"Thumbnail";
        case enMenuItem::TrackName     : return L"Set Track Name";
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

      for (auto l_tLog: m_vLog) {
        if (std::get<1>(l_tLog) != nullptr) {
          std::get<1>(l_tLog)->setVisible(false);
          m_pGui->addToDeletionQueue(std::get<1>(l_tLog));
          std::get<1>(l_tLog) = nullptr;
        }
      }

      for (auto l_tButton: m_mButtons) {
        l_tButton.second->setVisible(false);
        m_pGui->addToDeletionQueue(l_tButton.second);
      }

      m_mButtons.clear();

      irr::core::vector2di l_cBtnPos = irr::core::vector2di();

      irr::s32 l_iSpace = m_pGui->getSkin()->getFont()->getDimension(L"-").Width;

      irr::core::dimension2du l_cBtnSize;

      irr::core::dimension2du l_cScreen = m_pGui->getVideoDriver()->getScreenSize();

      for (int i = 0; i < (int)enMenuItem::Count; i++) {
        std::wstring l_sText = getButtonText((enMenuItem)i);

        if (l_sText != L"--") {
          irr::core::dimension2du l_cSize = m_pGui->getSkin()->getFont()->getDimension(l_sText.c_str());

          l_cSize.Width  = 5 * l_cSize.Width  / 4;
          l_cSize.Height = 5 * l_cSize.Height / 4;

          if (l_cSize.Width  > l_cBtnSize.Width ) l_cBtnSize.Width  = l_cSize.Width;
          if (l_cSize.Height > l_cBtnSize.Height) l_cBtnSize.Height = l_cSize.Height;
        }
      }

      for (int i = 0; i < (int)enMenuItem::Count; i++) {
        std::wstring l_sText = getButtonText((enMenuItem)i);

        if (l_sText != L"--") {
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

        if (l_cBtnPos.X + l_cBtnSize.Width >= l_cScreen.Width) {
          l_cBtnPos.X = 0;
          l_cBtnPos.Y += 5 * l_cBtnSize.Height / 4;
        }
      }

      irr::core::dimension2du l_cSize = m_pGui->getSkin()->getFont()->getDimension(L"Edit Mode: This Text is longer than needed.");
      irr::core::vector2di    l_cPos  = irr::core::vector2di(0, l_cBtnPos.Y + 5 * l_cBtnSize.Height / 4);

      m_pEditMode = m_pGui->addStaticText(L"Edit Mode: Default", irr::core::recti(l_cPos, l_cSize));
      m_cLogPos = irr::core::position2di(l_cPos.X, l_cPos.Y + l_cSize.Height);
      
      l_cPos.Y += 6 * l_cSize.Height / 5;
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
                  commands::CCmdLoadTrack *l_pCmd = new commands::CCmdLoadTrack(m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }
                case enMenuItem::Save: {
                  if (tracked::IGlobal::getInstance()->getGlobal("openfile") == "") {
                    commands::CCmdSaveTrackAs *l_pCmd = new commands::CCmdSaveTrackAs(m_pEditTrack, m_pHandler);
                    m_pHandler->addCommand(l_pCmd);
                  }
                  else {
                    commands::CCmdSaveTrack *l_pCmd = new commands::CCmdSaveTrack(m_pEditTrack);
                    m_pHandler->addCommand(l_pCmd);
                  }
                  break;
                }

                case enMenuItem::SaveAs: {
                  commands::CCmdSaveTrackAs *l_pCmd = new commands::CCmdSaveTrackAs(m_pEditTrack, m_pHandler);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                case enMenuItem::Base:
                  m_pEditTrack->setBaseParameters(2.5f, 2.5f);
                  break;

                case enMenuItem::ExportMarbleGP: {
                  commands::CCmdAddMarbleGP *l_pCmd = new commands::CCmdAddMarbleGP(m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                case enMenuItem::ExportMesh: {
                  commands::CCmdExportMesh *l_pCmd = new commands::CCmdExportMesh("testtrack.obj", m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                case enMenuItem::Thumbnail: {
                  commands::CCmdThumbnail *l_pCmd = new commands::CCmdThumbnail(tracked::IGlobal::getInstance()->getSceneManager(), m_pGui, m_pEditTrack);
                  m_pHandler->addCommand(l_pCmd);
                  break;
                }

                case enMenuItem::TrackName: {
                  commands::CCmdTrackName *l_pCmd = new commands::CCmdTrackName(m_pGui, m_pEditTrack);
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