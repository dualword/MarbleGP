// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <menu/datahandlers/CDataHandler_ResumeCup.h>
#include <controller/CControllerFactory.h>
#include <controller/CControllerBase.h>
#include <gameclasses/SPlayer.h>
#include <CGlobal.h>

namespace dustbin {
  namespace menu {
    CDataHandler_ResumeCup::CDataHandler_ResumeCup() : IMenuDataHandler(), m_pTournament(nullptr), m_iNextChange(0) {
      m_pTournament = CGlobal::getInstance()->getTournament();
      
      if (m_pTournament != nullptr) {
        m_itRace = m_pTournament->m_vRaces.begin();
        updateTrackImage();

        controller::CControllerFactory l_cFactory = controller::CControllerFactory(nullptr);

        for (auto l_pPlayer : m_pTournament->m_vPlayers) {
          if (l_pPlayer->m_eType == data::enPlayerType::Local) {
            controller::CControllerBase l_cCtrl = controller::CControllerBase();
            l_cCtrl.deserialize(l_pPlayer->m_sController);
            l_cCtrl.resetJoystick();
            l_pPlayer->m_sController = l_cCtrl.serialize();
          }
        }
      }
    }

    CDataHandler_ResumeCup::~CDataHandler_ResumeCup() {
    }

    void CDataHandler_ResumeCup::updateTrackImage() {
      m_iNextChange = m_pDevice->getTimer()->getTime() + 2000;

      irr::gui::IGUIImage *l_pTrack = reinterpret_cast<irr::gui::IGUIImage *>(findElement("Thumbnail"   , irr::gui::EGUIET_IMAGE));
      irr::gui::IGUIImage *l_pDone  = reinterpret_cast<irr::gui::IGUIImage *>(findElement("racefinished", irr::gui::EGUIET_IMAGE));

      irr::gui::IGUIStaticText *l_pInfo = reinterpret_cast<irr::gui::IGUIStaticText *>(findElement("label_raceno", irr::gui::EGUIET_STATIC_TEXT));
      irr::gui::IGUIStaticText *l_pWinr = reinterpret_cast<irr::gui::IGUIStaticText *>(findElement("label_winner", irr::gui::EGUIET_STATIC_TEXT));

      if (l_pTrack != nullptr) {
        std::string l_sImg = "data/levels/" + (*m_itRace)->m_sTrack + "/thumbnail.png";
        irr::video::ITexture *l_pTexture = m_pDrv->getTexture(l_sImg.c_str());
        l_pTrack->setImage(l_pTexture);
      }

      if (l_pDone != nullptr)
        l_pDone->setVisible((*m_itRace)->m_vRanking.size() > 0);

      if (l_pInfo != nullptr) {
        int l_iNum = 1;

        auto l_itRace = m_pTournament->m_vRaces.begin();

        while (l_itRace != m_itRace && l_itRace != m_pTournament->m_vRaces.end()) {
          l_iNum++;
          l_itRace++;
        }

        std::wstring l_sInfo = L"Race " + std::to_wstring(l_iNum) + L" of " + std::to_wstring(m_pTournament->m_vRaces.size()) + L"\n" + helpers::s2ws((*m_itRace)->m_sName);
        l_sInfo += L"\n" + std::to_wstring((*m_itRace)->m_iLaps) + L" Lap" + ((*m_itRace)->m_iLaps > 1 ? L"s" : L"");
        l_pInfo->setText(l_sInfo.c_str());
      }

      if (l_pWinr != nullptr) {
        if ((*m_itRace)->m_vRanking.size() > 0) {
          int l_iPlayer = (*(*m_itRace)->m_vRanking.begin())->m_iPlayer;

          std::wstring s = L"";
          for (auto l_pPlayer : m_pTournament->m_vPlayers) {
            if (l_pPlayer->m_iPlayer == l_iPlayer) {
              s = L"Winner: " + l_pPlayer->m_sWName;
            }
          }

          l_pWinr->setText(s.c_str());
          l_pWinr->setVisible((*m_itRace)->m_vRanking.size() > 0 && s != L"");
        }
        else l_pWinr->setVisible(false);
      }
    }

    /**
    * This is the main method of this class. Irrlicht events
    * get forwarded and may be handled by the handler
    * @param a_cEvent the event to handle
    * @return true if the event was handled, false otherwise
    */
    bool CDataHandler_ResumeCup::handleIrrlichtEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

    /**
    * Gets called every frame, maybe it's necessary to implement something
    */
    void CDataHandler_ResumeCup::runDataHandler() {
      if (m_pTournament != nullptr && m_pTournament->m_vRaces.size() > 0) {
        if (m_pDevice->getTimer()->getTime() > m_iNextChange) {
          m_itRace++;
          if (m_itRace == m_pTournament->m_vRaces.end())
            m_itRace = m_pTournament->m_vRaces.begin();

          updateTrackImage();
        }
      }
    }

    /**
    * Get the tournament to be resumed
    * @return the tournament to be resumed
    */
    gameclasses::STournament* CDataHandler_ResumeCup::getTournament() {
      return m_pTournament;
    }
  }
}