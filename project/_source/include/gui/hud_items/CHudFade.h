// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  class CGlobal;

  namespace gui {
    /**
    * @class CHudFade
    * @author Christian Keimel
    * This class encapsulates all color overlays and fade effects for the HUD.
    * The render method is called three times per frame to put the effect
    * to the right Z position
    */
    class CHudFade {
      public:
        /**
        * This enum defines the call position
        */
        enum class enCall {
          Start,          /**< Called before anything else of the HUD is rendered */
          BeforeBanners,  /**< Called before the banners are rendered, all text items are already rendered */
          End             /**< Called at the end of the HUD's render method*/
        };

        /**
        * Enum for the bitfield to show what to render
        * @see m_iFade
        */
        enum class enFade {
          NextRace    =   1,     /**< The next race banner */
          RaceStart   =   2,     /**< Fading in before race start */
          RespawnOut  =   4,     /**< Respawn fade out */
          RaceGrey    =   8,     /**< The grey overlay shown at race start */
          RespawnIn   =  16,     /**< Respawn fade in */
          Stunned     =  32,     /**< Blue overlay for stunned player */
          FinishedOut =  64,     /**< The fade out when the camera is switched to the rostrum */
          FinishedIn  = 128,     /**< The fade in when the camera is switched to the rostrum */
          RaceEnd     = 256,     /**< Fade out at the end of the race */
          GamePaused  = 512,     /**< The game is paused */
          Rostrum     = 1024,    /**< Rostrum shown and faded in */

          Count
        };

      private:
        int                       m_iFade;            /**< A bitfield for fading. @see enFade */
        int                       m_iStep;            /**< The current simulation step */
        CGlobal                  *m_pGlobal;          /**< Pointer to the global singleton */
        irr::core::recti          m_cRect;            /**< The viewport rect */
        irr::video::IVideoDriver *m_pDrv;             /**< The Irrlicht video driver */
        int                       m_iAlphaStep[8];    /**< The step when an alpha flag was changed (0 == NextRace, 1 == RaceStart, 2 == RespawnOut, 3 == RespawnIn, 4 == RaceEnd..) */

      public:
        /**
        * The constructor
        * @param a_pDrv the Irrlicht video driver
        * @param a_iMarble the marble ID
        * @param a_cRect the vireport rectangle
        */
        CHudFade(irr::video::IVideoDriver *a_pDrv, int a_iMarble, const irr::core::recti &a_cRect);
        ~CHudFade();

        /**
        * Mofify the m_iFade bitfield
        * @param a_eFade the state to activate or deactivate
        * @param a_bActivate true if the state needs to be activated, false otherwise
        */
        void setFadeFlag(enFade a_eFade, bool a_bActivate);

        /**
        * Get a fade flag
        * @param a_eFade which fade do we want to know?
        */
        bool getFadeFlag(enFade a_eFade);

       /**
        * Tell the fade instance about a simulation step
        * @param a_iStep the current step number
        */
        void onStep(int a_iStep);

        /**
        * Render whatever is necessary
        * @param a_eCall the call position
        * @see enCall
        */
        void render(enCall a_eCall);
    };
  }
}