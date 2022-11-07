// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudBanner.h>
#include <string>

namespace dustbin {
  namespace gui {
    CHudBanner::CHudBanner(irr::video::IVideoDriver* a_pDrv, irr::gui::IGUIEnvironment *a_pGui, irr::gui::IGUIFont *a_pFont, irr::gui::IGUIFont *a_pHuge, const irr::core::recti& a_cViewport) : 
      m_pDrv     (a_pDrv), 
      m_eState   (enBanners::CountdownReady), 
      m_fFade    (1.0f),
      m_pWithdraw(nullptr),
      m_pHugeFont(a_pHuge),
      m_iFinished(-1)
    {
      m_cViewport = a_cViewport;

      m_pBanners[(int)enBanners::CountdownReady] = m_pDrv->getTexture("data/images/countdown_ready.png");
      m_pBanners[(int)enBanners::Countdown3    ] = m_pDrv->getTexture("data/images/countdown_three.png");
      m_pBanners[(int)enBanners::Countdown2    ] = m_pDrv->getTexture("data/images/countdown_two.png");
      m_pBanners[(int)enBanners::Countdown1    ] = m_pDrv->getTexture("data/images/countdown_one.png");
      m_pBanners[(int)enBanners::CountdownGo   ] = m_pDrv->getTexture("data/images/countdown_go.png");
      m_pBanners[(int)enBanners::Stunned       ] = m_pDrv->getTexture("data/images/text_stunned.png");
      m_pBanners[(int)enBanners::Respawn       ] = m_pDrv->getTexture("data/images/text_respawn.png");
      m_pBanners[(int)enBanners::Finished      ] = m_pDrv->getTexture("data/images/text_finished.png");
      m_pBanners[(int)enBanners::Paused        ] = m_pDrv->getTexture("data/images/pause.png");

      
      m_pLaurel[0] = m_pDrv->getTexture("data/images/laurel_gold.png");
      m_pLaurel[1] = m_pDrv->getTexture("data/images/laurel_silver.png");
      m_pLaurel[2] = m_pDrv->getTexture("data/images/laurel_bronze.png");
      m_pLaurel[3] = m_pDrv->getTexture("data/images/laurel_rest.png");

      if (m_pBanners[0] != nullptr) {
        m_cSource = irr::core::recti(irr::core::vector2di(0, 0), m_pBanners[0]->getOriginalSize());

        irr::core::dimension2du l_cCntSize = irr::core::dimension2du(
          a_cViewport.getWidth () * a_cViewport.getWidth () / 3840,
          a_cViewport.getHeight() * a_cViewport.getHeight() / 2160
        );

        l_cCntSize.Height = 250 * l_cCntSize.Width / 2560;

        irr::core::position2di  l_cPos    = a_cViewport.UpperLeftCorner;

        irr::core::position2di l_cCntPos = irr::core::position2di(
          l_cPos.X + a_cViewport.getWidth () / 2 - l_cCntSize.Width / 2,
          l_cPos.Y + a_cViewport.getHeight() / 4 - l_cCntSize.Height / 2
        );

        m_cRect = irr::core::recti(l_cCntPos, l_cCntSize);

        if (m_pLaurel[0] != nullptr) {
          irr::core::dimension2di l_cDim = irr::core::dimension2di(m_cRect.getHeight(), m_cRect.getHeight());

          m_cLaurelLft = irr::core::recti(m_cRect.UpperLeftCorner                                                                   , l_cDim);
          m_cLaurelRgt = irr::core::recti(irr::core::vector2di(m_cRect.LowerRightCorner.X - l_cDim.Width, m_cRect.UpperLeftCorner.Y), l_cDim);

          m_cLaurelSrc = irr::core::recti(irr::core::vector2di(0, 0), m_pLaurel[0]->getOriginalSize());
        }

        m_pWithdraw = a_pGui->addStaticText(L"Withdraw from Race? Click again!", m_cRect);
        m_pWithdraw->setOverrideFont(a_pFont);
        m_pWithdraw->setTextAlignment(irr::gui::EGUIA_CENTER, irr::gui::EGUIA_CENTER);
        m_pWithdraw->setVisible(false);
      }
    }

    CHudBanner::~CHudBanner() {

    }

    /**
    * Set the state, i.e. which banner to render
    * @param a_eState the new banner to render. Set to "enBanners::Count" to render nothing
    */
    void CHudBanner::setState(CHudBanner::enBanners a_eState) {
      m_eState = a_eState;
    }

    /**
    * Render the banner
    * @param a_cPos the (center) position to render to
    */
    void CHudBanner::render(const irr::core::recti &a_cClip) {
      switch (m_eState) {
      case enBanners::Stunned:
        m_pDrv->draw2DRectangle(irr::video::SColor(128, 0, 0, 255), m_cViewport);
        break;

      default:
        break;
      }

      if (m_eState != enBanners::Count && !m_pWithdraw->isVisible()) {
        if (m_eState == enBanners::CountdownGo) {
          irr::video::SColor l_aColor[] = {
            irr::video::SColor((irr::u32)(255.0f * m_fFade), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fFade), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fFade), 255, 255, 255),
            irr::video::SColor((irr::u32)(255.0f * m_fFade), 255, 255, 255)
          };

          m_pDrv->draw2DImage(m_pBanners[(int)enBanners::CountdownGo], m_cRect, m_cSource, nullptr, l_aColor, true);
        }
        else m_pDrv->draw2DImage(m_pBanners[(int)m_eState], m_cRect, m_cSource, &a_cClip, nullptr, true);
      }

      if (m_iFinished >= 0 && m_eState == enBanners::Finished) {
        m_pDrv->draw2DImage(m_pLaurel[m_iFinished < 3 ? m_iFinished : 3], m_cLaurelLft, m_cLaurelSrc, nullptr, nullptr, true);
        m_pDrv->draw2DImage(m_pLaurel[m_iFinished < 3 ? m_iFinished : 3], m_cLaurelRgt, m_cLaurelSrc, nullptr, nullptr, true);

        if (m_pHugeFont != nullptr) {
          m_pHugeFont->draw(std::to_wstring(m_iFinished + 1).c_str(), m_cLaurelLft, irr::video::SColor(0xFF, 0, 0, 0), true, true);
          m_pHugeFont->draw(std::to_wstring(m_iFinished + 1).c_str(), m_cLaurelRgt, irr::video::SColor(0xFF, 0, 0, 0), true, true);
        }
      }
    }


    /**
    * Set the fading value of the countdown items
    * @param a_fFade the new fade value [0..1]
    */
    void CHudBanner::setFade(irr::f32 a_fFade) {
      m_fFade = a_fFade;
    }


    /**
    * Show or hide the "withdraw from race" banner
    * @param a_bShow Visible or not?
    */
    void CHudBanner::showConfirmWithdraw(bool a_bShow) {
      m_pWithdraw->setVisible(a_bShow);
    }

    /**
    * Set the position of the player
    * @param a_iPosition the position of the player
    */
    void CHudBanner::setPosition(int a_iPosition) {
      m_iFinished = a_iPosition;
    }
  }
}