// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCmdTrackName.h>
#include <commands/CCmdEditTrack.h>

#include <helpers/CStringHelpers.h>

#include <IGlobal.h>
#include <string>

namespace dustbin {
  namespace commands {

    /**
    * The constructor
    * @param a_pGui the Irrlicht GUI environment
    */
    CCmdTrackName::CCmdTrackName(irr::gui::IGUIEnvironment *a_pGui, CCmdEditTrack *a_pEditCmd) :
      ICommand    ("CmdTrackName"),
      m_pGui      (a_pGui),
      m_pParent   (nullptr),
      m_pCategory (nullptr),
      m_pTrackName(nullptr),
      m_pBtnOk    (nullptr),
      m_pBtnCancel(nullptr),
      m_pEditCmd  (a_pEditCmd)
    {
      onUiSetupModified();
    }

    CCmdTrackName::~CCmdTrackName() {
      clearUIElements();
    }

    /**
    * Clear the UI elements of this command
    */
    void CCmdTrackName::clearUIElements() {
      if (m_pParent != nullptr) {
        m_pParent->setVisible(false);
        m_pGui->addToDeletionQueue(m_pParent);
        m_pParent = nullptr;
      }
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdTrackName::isFinished() {
      return m_bFinished;
    }

    /**
    * Callback method for changes in the UI like modifying
    * font size or resizin window
    */
    void CCmdTrackName::onUiSetupModified() {
      clearUIElements();

      irr::core::dimension2du l_cSize = tracked::IGlobal::getInstance()->getVideoDriver()->getScreenSize();

      m_pParent = m_pGui->addTab(irr::core::recti(irr::core::vector2di(0, 0), l_cSize));
      m_pParent->setDrawBackground(true);
      m_pParent->setBackgroundColor(irr::video::SColor(128, 0xFF, 0xFF, 0xFF));

      irr::core::dimension2du l_cTotalSize = m_pGui->getSkin()->getFont()->getDimension(L"ThisStringIsLongEnoughForEverythingIHope");

      std::wstring l_sName     = helpers::s2ws(m_pEditCmd->getTrackName());
      std::wstring l_sCategory = L"Flat";

      if (l_sName.find_first_of(L':') != std::string::npos) {
        l_sCategory = l_sName.substr(0, l_sName.find_first_of(L':'));
        l_sName = l_sName.substr(l_sName.find_first_of(L':') + 1);
      }

      std::wstring l_sCategories[] = {
        L"Rookie",
        L"Flat",
        L"Jump",
        L"Loop",
        L"Steep",
        L"Complex",
        L"Moving",
        L""
      };

      irr::core::dimension2du l_cSizeText = m_pGui->getSkin()->getFont()->getDimension(L"Track Name: ");
      irr::core::dimension2du l_cSizeEdit = l_cTotalSize - irr::core::dimension2du(l_cSizeText.Width + l_cSizeText.Height, 0);

      l_cSizeEdit.Height = 5 * l_cSizeEdit.Height / 4;
      l_cSizeText.Height = 5 * l_cSizeText.Height / 4;

      irr::core::position2di l_cPosText = irr::core::position2di(l_cSize.Width / 2 - l_cTotalSize.Width / 2, l_cSize.Height / 2 - 3 * l_cTotalSize.Height);
      irr::core::position2di l_cPosEdit = l_cPosText + irr::core::position2di(l_cSizeText.Width + l_cSizeText.Height, 0);

      irr::gui::IGUIStaticText *l_pText = m_pGui->addStaticText(L"Category:", irr::core::recti(l_cPosText, l_cSizeText), false, false, m_pParent);
      l_pText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

      m_pCategory = m_pGui->addComboBox(irr::core::recti(l_cPosEdit, l_cSizeEdit), m_pParent);

      for (int i = 0; l_sCategories[i] != L""; i++) {
        m_pCategory->addItem(l_sCategories[i].c_str(), i);

        if (l_sCategory == l_sCategories[i])
          m_pCategory->setSelected(i);
      }

      l_cPosEdit.Y += 7 * l_cSizeText.Height / 3;
      l_cPosText.Y += 7 * l_cSizeText.Height / 3;

      l_pText = m_pGui->addStaticText(L"Track Name: ", irr::core::recti(l_cPosText, l_cSizeText), false, false, m_pParent);
      l_pText->setTextAlignment(irr::gui::EGUIA_LOWERRIGHT, irr::gui::EGUIA_CENTER);

      m_pTrackName = m_pGui->addEditBox(l_sName.c_str(), irr::core::recti(l_cPosEdit, l_cSizeEdit), true, m_pParent);

      l_cPosEdit.Y += 7 * l_cSizeText.Height / 3;

      irr::core::dimension2du l_cSizeBtn = irr::core::dimension2du(l_cSizeEdit.Width / 2, l_cSizeEdit.Height);

      m_pBtnOk = m_pGui->addButton(irr::core::recti(l_cPosEdit, l_cSizeBtn), m_pParent, -1, L"OK");

      l_cPosEdit.X += l_cSizeBtn.Width;
      m_pBtnCancel = m_pGui->addButton(irr::core::recti(l_cPosEdit, l_cSizeBtn), m_pParent, -1, L"Cancel");
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdTrackName::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (a_cEvent.GUIEvent.Caller == m_pBtnOk) {
            std::wstring l_sName = m_pCategory->getItem(m_pCategory->getSelected());
            l_sName += L": " + std::wstring(m_pTrackName->getText());

            m_pEditCmd->setTrackName(helpers::ws2s(l_sName));
            m_bFinished = true;
          }

          if (a_cEvent.GUIEvent.Caller == m_pBtnCancel) {
            m_bFinished = true;
          }
        }
      }

      return l_bRet;
    }
  }
}