// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html

#include <helpers/CTrackEdHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdThumbnail.h>

namespace dustbin {
  namespace commands {
    /**
    * The constructor
    * @param a_pSmgr the Irrlicht scene manager
    * @param a_pGui the Irrlicht GUI environment
    */
    CCmdThumbnail::CCmdThumbnail(irr::scene::ISceneManager *a_pSmgr, irr::gui::IGUIEnvironment *a_pGui, CCmdEditTrack *a_pEditCmd) :
      ICommand    ("CmdThumbnail"),
      m_pGui      (a_pGui),
      m_pBtnOk    (nullptr),
      m_pBtnCancel(nullptr),
      m_pSmgr     (a_pSmgr),
      m_pEditCmd  (a_pEditCmd),
      m_bFinished (false)
    {
      m_pPattern[0] = nullptr;
      m_pPattern[1] = nullptr;

      onUiSetupModified();
    }

    CCmdThumbnail::~CCmdThumbnail() {
      clearUIElements();
    }

    void CCmdThumbnail::clearUIElements() {
      for (int i = 0; i < 2; i++)
        if (m_pPattern[i] != nullptr) {
          m_pPattern[i]->setVisible(false);
          m_pGui->addToDeletionQueue(m_pPattern[i]);
          m_pPattern[i] = nullptr;
        }

      if (m_pBtnOk     != nullptr) { m_pBtnOk    ->setVisible(false); m_pGui->addToDeletionQueue(m_pBtnOk    ); m_pBtnOk     = nullptr; }
      if (m_pBtnCancel != nullptr) { m_pBtnCancel->setVisible(false); m_pGui->addToDeletionQueue(m_pBtnCancel); m_pBtnCancel = nullptr; }
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdThumbnail::isFinished() {
      return m_bFinished;
    }

    /**
    * Callback method for changes in the UI like modifying
    * font size or resizin window
    */
    void CCmdThumbnail::onUiSetupModified() {
      clearUIElements();

      irr::core::dimension2du l_cSize = m_pGui->getVideoDriver()->getScreenSize();

      if (l_cSize.Width > l_cSize.Height) {
        irr::u32 l_iOffset = l_cSize.Width - l_cSize.Height;

        m_pPattern[0] = m_pGui->addTab(irr::core::recti(                            0, 0, l_iOffset / 2, l_cSize.Height));
        m_pPattern[1] = m_pGui->addTab(irr::core::recti(l_cSize.Width - l_iOffset / 2, 0, l_cSize.Width, l_cSize.Height));
      }
      else if (l_cSize.Height > l_cSize.Width) {
        irr::u32 l_iOffset = l_cSize.Height - l_cSize.Width;

        m_pPattern[0] = m_pGui->addTab(irr::core::recti(0,                              0, l_cSize.Width, l_iOffset / 2 ));
        m_pPattern[1] = m_pGui->addTab(irr::core::recti(0, l_cSize.Height - l_iOffset / 2, l_cSize.Width, l_cSize.Height));
      }

      for (int i = 0; i < 2; i++) {
        m_pPattern[i]->setDrawBackground(true);
        m_pPattern[i]->setBackgroundColor(irr::video::SColor(0xFF, 0, 0, 0));
      }

      irr::core::dimension2du l_cBtn = m_pGui->getSkin()->getFont()->getDimension(L"Cancel");

      l_cBtn.Width  = 5 * l_cBtn.Width  / 4;
      l_cBtn.Height = 5 * l_cBtn.Height / 4;

      m_pBtnCancel = m_pGui->addButton(irr::core::recti(irr::core::vector2di(l_cSize.Width -     l_cBtn.Width - l_cBtn.Height / 4, l_cSize.Height - 7 * l_cBtn.Height / 6), l_cBtn), m_pGui->getRootGUIElement(), -1, L"Cancel");
      m_pBtnOk     = m_pGui->addButton(irr::core::recti(irr::core::vector2di(l_cSize.Width - 2 * l_cBtn.Width - l_cBtn.Height / 2, l_cSize.Height - 7 * l_cBtn.Height / 6), l_cBtn), m_pGui->getRootGUIElement(), -1, L"OK");
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdThumbnail::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (a_cEvent.GUIEvent.Caller == m_pBtnOk) {
            irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->getActiveCamera();

            l_pCam->updateAbsolutePosition();
            m_pEditCmd->setThumbnailCamera(l_pCam->getAbsolutePosition(), l_pCam->getTarget());

            m_bFinished = true;
            l_bRet = true;

            helpers::addLogMessage(L"Thumbnail camera saved.", irr::ELL_INFORMATION, 2000);
          }
          else if (a_cEvent.GUIEvent.Caller == m_pBtnCancel) {
            m_bFinished = true;
            l_bRet = true;
          }
        }
      }

      return l_bRet;
    }
  }
}