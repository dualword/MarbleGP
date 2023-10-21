#pragma once

#include <irrlicht.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace gameclasses {
    struct SPlayer;   /**< Forward declaration of SPlayer */
  }

  namespace gui {
    /**
    * @class CInGamePanelRenderer
    * @author Christian Keimel
    * This class takes care of rendering the in-game panels
    */
    class CInGamePanelRenderer {
      private:
        irr::IrrlichtDevice       *m_pDevice;
        irr::video::IVideoDriver  *m_pDrv;
        irr::gui::IGUIEnvironment *m_pGui;

        irr::gui::IGUIFont *m_pFontOne;     /**< The font to use for rendering */
        irr::gui::IGUIFont *m_pFontTwo;     /**< The font to use for rendering */

        int m_iCurrentLap;    /**< The current lap */
        int m_iLapCount;      /**< The number of laps of the race */
        int m_iCountDown;     /**< The countdown tick (4 == Ready, 3, 2, 1, 0 == Go) */
        
        std::vector<gameclasses::SPlayer *> m_vPlayers;   /**< The positions in the race */

        irr::video::ITexture *m_pRaceInfoRtt;   /**< The race info render target texture */
        irr::video::ITexture *m_pLapCountRtt;   /**< The lap counter render target texture */

        irr::core::recti m_cRect;     /**< The clipping rect */
        irr::core::recti m_cRectLap;  /**< The render rect for the lap number */

        std::tuple<irr::core::recti, irr::core::recti, irr::core::recti, irr::core::recti> m_aRecPos[16];   /**< The rectangles for position rendering (0 == number, 1 == Short Name, 2 == deficit, 3 == total rect) */

        int m_aDiffAhead[16];   /**< The lag of the players */
        int m_aState    [16];   /**< The state of the marbles */

        bool m_bUpdate;   /**< Does the texture need an update? */

        irr::core::recti m_cLapCount;

        /**
        * Update the render target
        */
        void updateRtt();

        /**
        * Render the current lap
        */
        void renderCurrentLap(const irr::core::recti &a_cRect, bool a_bCenter);

      public:
        CInGamePanelRenderer(irr::IrrlichtDevice *a_pDevice, const std::vector<gameclasses::SPlayer *> &a_vPlayers, int a_iLapCount);
        ~CInGamePanelRenderer();

        /**
        * Update the current lap no
        * @param a_iCurrentLap the current lap
        */
        void updateCurrentLap(int a_iCurrentLap);

        /**
        * Update ranking
        * @param a_vPlayers the current ranking (sorted vector)
        */
        void updateRanking(const std::vector<gameclasses::SPlayer *> &a_vPlayers);

        /**
        * Updates the texture if necessary
        */
        void updateTextureIfNecessary();

        /**
        * Get the texture for the race information
        * @return the render target
        */
        irr::video::ITexture *getRaceInfoRTT();

        /**
        * Get the texture for the lap counter
        * @return the render target
        */
        irr::video::ITexture *getLapCountRTT();

        /**
        * Set the countdown tick
        * @param a_iTick the countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
        */
        void setCountdownTick(int a_iTick);
    };
  }
}
