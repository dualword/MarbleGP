// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/messages/CMessages.h>
#include <helpers/CStringHelpers.h>
#include <threads/CMessageQueue.h>
#include <gui/CTutorialHUD.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CTutorialHUD::CTutorialHUD(gameclasses::SPlayer* a_pPlayer, const irr::core::recti& a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment* a_pGui, std::vector<gameclasses::SPlayer*>* a_vRanking, threads::COutputQueue *a_pQueue) :
      CGameHUD(a_pPlayer, a_cRect, a_iLapCnt, a_pGui, a_vRanking), m_iCurrent(0), m_pRoot(nullptr), m_pHint(nullptr), m_pQueue(a_pQueue)
    {
      int l_iCount = 0;

      do {
        std::string l_sFile = "data/tutorials/tutorial" + std::to_string(l_iCount) + ".txt";
        irr::io::IReadFile *l_pFile = CGlobal::getInstance()->getFileSystem()->createAndOpenFile(l_sFile.c_str());
        if (l_pFile != nullptr) {
          long l_iSize = l_pFile->getSize();
          char *l_pBuffer = new char[l_iSize + 1];
          memset(l_pBuffer, 0, l_iSize + 1);

          l_pFile->read(l_pBuffer, l_iSize);

          m_mHints[l_iCount] = std::string(l_pBuffer);

          delete []l_pBuffer;
          l_iCount++;
          l_pFile->drop();
        }
        else break;
      }
      while (true);

      irr::core::recti l_cRect = a_cRect;
      irr::s32 l_iOffset = l_cRect.getHeight() / 10;

      l_cRect.UpperLeftCorner  += irr::core::vector2di(l_iOffset);
      l_cRect.LowerRightCorner -= irr::core::vector2di(l_iOffset);

      m_pRoot = CGlobal::getInstance()->getGuiEnvironment()->addTab(l_cRect);
      m_pRoot->setBackgroundColor(irr::video::SColor(224, 224, 224, 224));
      m_pRoot->setDrawBackground(true);
      m_pRoot->setVisible(false);

      std::wstring l_sContinue = L"Press the Pause key to continue with the turorial!";
      irr::core::dimension2du l_cSize = CGlobal::getInstance()->getFont(dustbin::enFont::Regular, irr::core::dimension2du(a_cRect.getSize().Width, a_cRect.getSize().Height))->getDimension(l_sContinue.c_str());

      l_cRect = irr::core::recti(irr::core::vector2di(l_iOffset, l_iOffset), m_pRoot->getAbsoluteClippingRect().getSize() - irr::core::dimension2di(2 * l_iOffset, 2 * l_iOffset + l_cSize.Height));

      m_pHint = CGlobal::getInstance()->getGuiEnvironment()->addStaticText(L"", l_cRect, false, true, m_pRoot);
      m_pHint->setTextAlignment(irr::gui::EGUIA_UPPERLEFT, irr::gui::EGUIA_CENTER);

      if (m_mHints.find(0) != m_mHints.end())
        m_pHint->setText(helpers::s2ws(m_mHints[0]).c_str());

      l_cRect = irr::core::recti(irr::core::vector2di(0, m_pRoot->getAbsoluteClippingRect().getHeight() - l_cSize.Height), irr::core::dimension2du(m_pRoot->getAbsoluteClippingRect().getWidth(), l_cSize.Height));

      irr::gui::IGUIStaticText *l_pContinue = CGlobal::getInstance()->getGuiEnvironment()->addStaticText(l_sContinue.c_str(), l_cRect, false, true, m_pRoot);
      l_pContinue->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
    }

    CTutorialHUD::~CTutorialHUD() {

    }

    void CTutorialHUD::pauseGame() {
      messages::CTogglePause  l_cPause = messages::CTogglePause(m_iMarble);
      m_pQueue->postMessage(&l_cPause);
    }

    /**
    * Callback if a trigger was triggered. This method is only
    * implemented in the tutorial HUD
    * @param a_iObjectId the triggering object
    * @param a_iTriggerId the ID of the trigger
    * @see CGameHUD::triggerCallback
    */
    void CTutorialHUD::triggerCallback(int a_iObjectId, int a_iTriggerId) {
      if (a_iObjectId == m_iMarble && m_pHint != nullptr) {
        if (a_iTriggerId == m_iCurrent + 1) {
          m_iCurrent = a_iTriggerId;
          if (m_mHints.find(m_iCurrent) != m_mHints.end()) {
            m_pHint->setText(helpers::s2ws(m_mHints[m_iCurrent]).c_str());
            pauseGame();
          }
        }
        else if (a_iTriggerId == 1 && m_iCurrent == 7 && m_mHints.find(8) != m_mHints.end()) {
          m_pHint->setText(helpers::s2ws(m_mHints[8]).c_str());
          m_iCurrent = 8;
          pauseGame();
        }
      }
    }

    /**
    * This function receives messages of type "PauseChanged"
    * @param a_Paused The current paused state
    */
    void CTutorialHUD::onPausechanged(bool a_Paused) {
      m_pRoot->setVisible(a_Paused);
    }
  }
}