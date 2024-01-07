#include <gui/CInGamePanelRenderer.h>
#include <gameclasses/SPlayer.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace gui {
    CInGamePanelRenderer::CInGamePanelRenderer(irr::IrrlichtDevice *a_pDevice, const std::vector<gameclasses::SPlayer *> &a_vPlayers, int a_iLapCount) :
      m_pDevice     (a_pDevice),
      m_pDrv        (a_pDevice->getVideoDriver   ()),
      m_pGui        (a_pDevice->getGUIEnvironment()),
      m_pFontOne    (nullptr),
      m_pFontTwo    (nullptr),
      m_iCurrentLap (0),
      m_iLapCount   (a_iLapCount),
      m_iCountDown  (4),
      m_pRaceInfoRtt(nullptr),
      m_pLapCountRtt(nullptr),
      m_bUpdate     (true)
    {
      m_pFontOne = CGlobal::getInstance()->getFont(enFont::Big  , irr::core::dimension2du(1920, 1080));
      m_pFontTwo = CGlobal::getInstance()->getFont(enFont::Small, irr::core::dimension2du(1920, 1080));

      for (auto l_pPlayer: a_vPlayers)
        m_vPlayers.push_back(l_pPlayer);
    }

    CInGamePanelRenderer::~CInGamePanelRenderer() {
    }

    /**
    * Update the current lap no
    * @param a_iCurrentLap the current lap
    */
    void CInGamePanelRenderer::updateCurrentLap(int a_iCurrentLap) {
      m_bUpdate = a_iCurrentLap < m_iCurrentLap;
      if (m_iCurrentLap < a_iCurrentLap)
        m_iCurrentLap = a_iCurrentLap;
    }

    /**
    * Update ranking
    * @param a_vPlayers the current ranking (sorted vector)
    */
    void CInGamePanelRenderer::updateRanking(const std::vector<gameclasses::SRaceData *> &a_vPlayers) {
      bool l_bUpdate = m_vPlayers.size() != a_vPlayers.size();
      int l_iIndex = 0;

      std::vector<gameclasses::SPlayer*>::const_iterator l_itOther = m_vPlayers.begin();
      for (std::vector<gameclasses::SRaceData *>::const_iterator l_itPlr = a_vPlayers.begin(); l_itPlr != a_vPlayers.end() && l_itOther != m_vPlayers.end(); l_itPlr++) {
        if (*l_itOther != (*l_itPlr)->m_pPlayer) {
          l_bUpdate = true;
          m_bUpdate = true;
          break;
        }

        if (m_aDiffAhead[l_iIndex] != (*l_itOther)->m_pRaceData->m_iDiffAhead) {
          l_bUpdate = true;
          m_aDiffAhead[l_iIndex] = (*l_itOther)->m_pRaceData->m_iDiffAhead;
        }

        if (m_aState[l_iIndex] != (*l_itOther)->m_pRaceData->m_iState) {
          l_bUpdate = true;
          m_aState[l_iIndex] = (*l_itOther)->m_pRaceData->m_iState;
        }

        l_iIndex++;
      }

      if (l_bUpdate) {
        m_vPlayers.clear();
        for (std::vector<gameclasses::SRaceData *>::const_iterator l_itPlr = a_vPlayers.begin(); l_itPlr != a_vPlayers.end(); l_itPlr++) {
          m_vPlayers.push_back((*l_itPlr)->m_pPlayer);
        }
      }
    }

    /**
    * Updates the texture if necessary
    */
    void CInGamePanelRenderer::updateTextureIfNecessary() {
      if (m_bUpdate) {
        m_bUpdate = false;
        updateRtt();
      }
    }

    /**
    * Get the render target to be applied to a mesh
    * @return the render target
    */
    irr::video::ITexture* CInGamePanelRenderer::getRaceInfoRTT() {
      if (m_pRaceInfoRtt == nullptr) {
        m_pRaceInfoRtt = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(512, 512), "InGameDisplay_RTT");

        irr::core::dimension2du l_cSize = m_pFontOne->getDimension(L"Lap 66 / 66");
        l_cSize.Width  = 5 * l_cSize.Width  / 4;
        l_cSize.Height = 5 * l_cSize.Height / 4;

        m_cRectLap = irr::core::recti(irr::core::vector2di(5, 5), l_cSize);

        m_cRect = irr::core::recti(irr::core::vector2di(0, 0), irr::core::dimension2du(512, 512));

        for (int i = 0; i < 16; i++) {
          m_aDiffAhead[i] = 0;
          m_aState    [i] = 0;
        }

        int l_iPosX = 32;
        int l_iPosY = m_cRectLap.LowerRightCorner.Y + 32;

        for (int l_iIndex = 0; l_iIndex < 16; l_iIndex++) {
          m_aRecPos[l_iIndex] = std::make_tuple(
            irr::core::recti(irr::core::vector2di(l_iPosX     , l_iPosY), irr::core::dimension2du( 32, 32)),
            irr::core::recti(irr::core::vector2di(l_iPosX + 32, l_iPosY), irr::core::dimension2du( 86, 32)),
            irr::core::recti(irr::core::vector2di(l_iPosX + 86, l_iPosY), irr::core::dimension2du(106, 32)),
            irr::core::recti(irr::core::vector2di(l_iPosX     , l_iPosY), irr::core::dimension2du(192, 32))
          );

          l_iPosX = l_iPosX > 256 ? 32 : 288;
          l_iPosY += 24;
        }
      }

      return m_pRaceInfoRtt;
    }

    /**
    * Get the texture for the lap counter
    * @return the render target
    */
    irr::video::ITexture *CInGamePanelRenderer::getLapCountRTT() {
      if (m_pLapCountRtt == nullptr) {
        irr::core::dimension2du l_cSize = m_pFontOne->getDimension(L"Lap 66 / 66");

        l_cSize.Width  = 5 * l_cSize.Width  / 4;
        l_cSize.Height = 5 * l_cSize.Height / 4;

        if (l_cSize.Width > 2 * l_cSize.Height)
          l_cSize.Height = l_cSize.Width / 2;
        else
          l_cSize.Width = 2 * l_cSize.Height;

        m_cLapCount = irr::core::recti(irr::core::vector2di(0, 0), l_cSize);

        m_pLapCountRtt = m_pDrv->addRenderTargetTexture(l_cSize, "__lapcount__rtt");
      }

      return m_pLapCountRtt;
    }

    /**
    * Render the current lap
    */
    void CInGamePanelRenderer::renderCurrentLap(const irr::core::recti &a_cRect, bool a_bCenter) {
      std::wstring s = L"";
      
      if (m_iCurrentLap <= 0) {
        switch (m_iCountDown) {
          case  4: s = L"Ready."; break;
          case  3: s = L"3"     ; break;
          case  2: s = L"2"     ; break; 
          case  1: s = L"1"     ; break;
          default: s = L"Go!!!" ; break;
        }
      }
      else if (m_iCurrentLap == m_iLapCount) {
        s = L"Last Lap";
      }
      else if (m_iCurrentLap > m_iLapCount)
        s = L"Finished";
      else
        s = L"Lap " + std::to_wstring(m_iCurrentLap) + L" / " + std::to_wstring(m_iLapCount);

      m_pFontOne->draw(s.c_str(), a_cRect, (a_bCenter && m_iLapCount == 1) ? irr::video::SColor(0xFF, 0xb8, 0xc8, 0xff) : irr::video::SColor(0xFF, 0, 0, 0), a_bCenter, true);
    }

    /**
    * Set the countdown tick
    * @param a_iTick the countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
    */
    void CInGamePanelRenderer::setCountdownTick(int a_iTick) {
      m_iCountDown = a_iTick;
      updateRtt();
    }

    /**
    * Update the render target
    */
    void CInGamePanelRenderer::updateRtt() {
      if (m_pRaceInfoRtt != nullptr) {
        m_pDrv->setRenderTarget(m_pRaceInfoRtt, true, true, irr::video::SColor(0xFF, 0xb8, 0xc8, 0xff));


        int l_iIndex = 0;

        renderCurrentLap(m_cRectLap, false);

        for (auto l_pPlayer : m_vPlayers) {
          m_pDrv->draw2DRectangleOutline(std::get<3>(m_aRecPos[l_iIndex]), irr::video::SColor(0xFF, 0, 0, 0));
          m_pDrv->draw2DRectangle(l_pPlayer->m_cBack, std::get<0>(m_aRecPos[l_iIndex]));

          if (l_pPlayer->m_pRaceData->m_iState == 1) {
            m_pDrv->draw2DRectangle(irr::video::SColor(128, 0, 0, 255), std::get<3>(m_aRecPos[l_iIndex]));
          }
          else if (l_pPlayer->m_pRaceData->m_iState == 2) {
            m_pDrv->draw2DRectangle(irr::video::SColor(128, 255, 0, 0), std::get<3>(m_aRecPos[l_iIndex]));
          }
          else if (l_pPlayer->m_pRaceData->m_iState == 3) {
            m_pDrv->draw2DRectangle(irr::video::SColor(128, 255, 255, 0), std::get<3>(m_aRecPos[l_iIndex]));
          }

          m_pDrv->draw2DRectangleOutline(std::get<0>(m_aRecPos[l_iIndex]), l_pPlayer->m_cFrme);

          std::wstring l_sName = L" " + l_pPlayer->m_wsShortName;
          std::wstring l_sDiff = L"-";

          if (l_pPlayer->m_pRaceData->m_iDiffAhead > 0) {
            double l_fTime = (double)l_pPlayer->m_pRaceData->m_iDiffAhead / 120.0;
            wchar_t s[50];
#ifdef _WINDOWS
            swprintf(s, L"%.2f Sec.", l_fTime);
#else
            swprintf(s, 50, L"%.2f Sec.", l_fTime);
#endif

            l_sDiff = s;
          }
          else if (l_pPlayer->m_pRaceData->m_iDiffAhead < 0) {
            l_sDiff = L"+" + std::to_wstring(-l_pPlayer->m_pRaceData->m_iDiffAhead) + L" Lap" + (l_pPlayer->m_pRaceData->m_iDiffAhead == - 1 ? L"" : L"s");
          }

          m_pFontTwo->draw(l_pPlayer->m_sNumber.c_str(), std::get<0>(m_aRecPos[l_iIndex]), l_pPlayer->m_cText               , true , true);
          m_pFontTwo->draw(l_sName.c_str()             , std::get<1>(m_aRecPos[l_iIndex]), irr::video::SColor(0xFF, 0, 0, 0), false, true);

          irr::core::recti l_cDiff = std::get<2>(m_aRecPos[l_iIndex]);
          irr::core::dimension2du l_cSize = m_pFontTwo->getDimension(l_sDiff.c_str());

          l_cDiff.UpperLeftCorner.X = l_cDiff.LowerRightCorner.X - l_cSize.Width - 8;

          m_pFontTwo->draw(l_sDiff.c_str(), l_cDiff, irr::video::SColor(0xFF, 0, 0, 0), false, true);

          l_iIndex++;
        }

        m_pDrv->setRenderTarget(nullptr);
      }

      if (m_pLapCountRtt != nullptr) {
        m_pDrv->setRenderTarget(m_pLapCountRtt, true, true, m_iLapCount == 1 ? irr::video::SColor(0xFF, 0, 0, 0) : irr::video::SColor(0xFF, 0xb8, 0xc8, 0xff));
        renderCurrentLap(m_cLapCount, true);
        m_pDrv->setRenderTarget(nullptr);
      }
    }
  }
}