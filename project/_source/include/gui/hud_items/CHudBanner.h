// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    /**
    * @class CHudBanner
    * @author Christian Keimel
    * A rendering class for the banners (countdown, stunned, finished...)
    */
    class CHudBanner {
      public:
        enum class enBanners {
          CountdownReady,
          Countdown3,
          Countdown2,
          Countdown1,
          CountdownGo,
          Respawn,
          Stunned,
          Finished,
          Paused,

          Count
        };

      private:
        irr::video::IVideoDriver *m_pDrv;                             /**< The Irrlicht video driver */
        irr::video::ITexture     *m_pBanners[(int)enBanners::Count];  /**< The textures for the banner */
        irr::video::ITexture     *m_pLaurel[4];                       /**< The laurel wreath images */
        enBanners                 m_eState;                           /**< The state, i.e. which banner to render */
        irr::core::recti          m_cRect;                            /**< The rect to render the banners to */
        irr::core::recti          m_cViewport;                        /**< The vireport */
        irr::core::recti          m_cSource;                          /**< The source rect */
        irr::f32                  m_fFade;                            /**< The alpha value to fade out the countdown banner */
        irr::core::recti          m_cLaurelLft;                       /**< Recti for the left laurel wreath */
        irr::core::recti          m_cLaurelRgt;                       /**< Recti for the right laurel wreath */
        irr::core::recti          m_cLaurelSrc;                       /**< Source rect for rendering the laurel wreaths */
        irr::gui::IGUIStaticText *m_pWithdraw;                        /**< The "Confirm Withdraw" static text */
        irr::gui::IGUIFont       *m_pHugeFont;                        /**< The huge font */
        int                       m_iFinished;                        /**< Index of the visible laurels */

      public:
        CHudBanner(irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui, irr::gui::IGUIFont *a_pFont, irr::gui::IGUIFont *a_pHuge, const irr::core::recti &a_cViewport);
        ~CHudBanner();

        /**
        * Set the state, i.e. which banner to render
        * @param a_eState the new banner to render. Set to "enBanners::Count" to render nothing
        */
        void setState(CHudBanner::enBanners a_eState);

        /**
        * Render the banner
        * @param a_cClip the clipping rectangle
        */
        void render(const irr::core::recti &a_cClip);

        /**
        * Set the fading value of the countdown items
        * @param a_fFade the new fade value [0..1]
        */
        void setFade(irr::f32 a_fFade);

        /**
        * Show or hide the "withdraw from race" banner
        * @param a_bShow Visible or not?
        */
        void showConfirmWithdraw(bool a_bShow);

        /**
        * Set the position of the player
        * @param a_iPosition the position of the player
        */
        void setRacePosition(int a_iPosition);
      };
  }
}