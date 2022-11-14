// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudFade.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    const int c_iAlphaIdxNextRace    = 0;       /**< The index for enFade::NextRace in the m_fAlpha array */
    const int c_iAlphaIdxRaceGrey    = 1;       /**< The index for enFade::RaceGrey in the m_fAlpha array */
    const int c_iAlphaIdxRaceStart   = 2;       /**< The index for enFade::RaceStart in the m_fAlpha array */
    const int c_iAlphaIdxRespawnOut  = 3;       /**< The index for enFade::RespawnOut in the m_fAlpha array */
    const int c_iAlphaIdxRespawnIn   = 4;       /**< The index for enFade::RespawnIn in the m_fAlpha array */
    const int c_iAlphaIdxFinishedOut = 5;       /**< The index for enFade::FinishedOut in the m_fAlpha array */
    const int c_iAlphaIdxFinishedIn  = 6;       /**< The index for enFade::FinishedIn in the m_fAlpha array */
    const int c_iAlphaIdxRaceEnd     = 7;       /**< The index for enFade::RaceEnd in the m_fAlpha array */

    /**
    * The constructor
    * @param a_pDrv the Irrlicht video driver
    * @param a_iMarble the marble ID
    * @param a_cRect the vireport rectangle
    */
    CHudFade::CHudFade(irr::video::IVideoDriver *a_pDrv, int a_iMarble, const irr::core::recti& a_cRect) :
      m_iMarble(a_iMarble),
      m_iFade  ((int)enFade::NextRace | (int)enFade::RaceGrey | (int)enFade::RaceStart),
      m_iStep  (0),
      m_pGlobal(CGlobal::getInstance()),
      m_cRect  (a_cRect),
      m_pDrv   (a_pDrv)
    {
      for (int i = 0; i <= c_iAlphaIdxRaceEnd; i++) {
        m_iAlphaStep[i] = 0;
      }
    }

    CHudFade::~CHudFade() {
    }

    /**
    * Mofify the m_iFade bitfield
    * @param a_eFade the state to activate or deactivate
    * @param a_bActivate true if the state needs to be activated, false otherwise
    */
    void CHudFade::setFadeFlag(enFade a_eFade, bool a_bActivate) {
      if (a_bActivate)
        m_iFade |= (int)a_eFade;
      else
        m_iFade &= ~(int)a_eFade;

      for (int i = 0; i < 8; i++)
        printf("%i", (m_iFade & i) == i ? 1 : 0);

      printf("@step #%i\n", m_iStep);

      switch (a_eFade) {
        case enFade::NextRace   : m_iAlphaStep[c_iAlphaIdxNextRace   ] = m_iStep; break;
        case enFade::RaceGrey   : m_iAlphaStep[c_iAlphaIdxRaceGrey   ] = m_iStep; break;
        case enFade::RaceStart  : m_iAlphaStep[c_iAlphaIdxRaceStart  ] = m_iStep; break;
        case enFade::RespawnOut : m_iAlphaStep[c_iAlphaIdxRespawnOut ] = m_iStep; break;
        case enFade::RespawnIn  : m_iAlphaStep[c_iAlphaIdxRespawnIn  ] = m_iStep; break;
        case enFade::FinishedOut: m_iAlphaStep[c_iAlphaIdxFinishedOut] = m_iStep; break;
        case enFade::FinishedIn : m_iAlphaStep[c_iAlphaIdxFinishedIn ] = m_iStep; break;
        case enFade::RaceEnd    : m_iAlphaStep[c_iAlphaIdxRaceEnd    ] = m_iStep; break;
        default:
          // The remaining fade state "enFade::Stunned" does not fade
          break;
      }
    }

    /**
    * Tell the fade instance about a simulation step
    * @param a_iStep the current step number
    */
    void CHudFade::onStep(int a_iStep) {
      m_iStep = a_iStep;
    }

    /**
    * Render whatever is necessary
    * @param a_eCall the call position
    * @see enCall
    */
    void CHudFade::render(enCall a_eCall) {
      switch (a_eCall) {
        case enCall::Start: {
          // The grey overlay at race start
          if ((m_iFade & (int)enFade::RaceGrey) == (int)enFade::RaceGrey) {
            irr::f32 l_fAlpha = m_iAlphaStep[c_iAlphaIdxRaceGrey] == 0 ? 1.0f : (1.0f - ((irr::f32)m_iStep - m_iAlphaStep[c_iAlphaIdxRaceGrey]) / 120.0f);

            if (l_fAlpha >= 0) {
              m_pDrv->draw2DRectangle(irr::video::SColor((irr::s32)(l_fAlpha * 96.0f), 192, 192, 192), m_cRect);
            }
          }
          break;
        }

        case enCall::BeforeBanners: {
          // Stunned overlay is below all other rendered banner items
          if ((m_iFade & (int)enFade::Stunned) == (int)enFade::Stunned)
            m_pDrv->draw2DRectangle(irr::video::SColor(128, 0, 0, 255), m_cRect);

          // Fade in after finished
          if ((m_iFade & (int)enFade::FinishedIn)) {
            int l_iSince = m_iStep - m_iAlphaStep[c_iAlphaIdxFinishedIn];

            irr::f32 l_fFactor = 1.0f - ((irr::f32)(l_iSince - 60) / 100.0f);
            l_fFactor = std::max(0.0f, std::min(1.0f, l_fFactor));

            if (l_fFactor != 0.0f)
              m_pDrv->draw2DRectangle(irr::video::SColor((irr::s32)(l_fFactor * 255.0f), 0, 0, 0), m_cRect);
          }

          // Fade out because of a respawn
          if ((m_iFade & (int)enFade::RespawnOut) == (int)enFade::RespawnOut) {
            int l_iStepSince = m_iStep - m_iAlphaStep[c_iAlphaIdxRespawnOut];
            irr::f32 l_fFactor = 1.0f;

            if (l_iStepSince < 120) {
              l_fFactor = ((irr::f32)l_iStepSince) / 120.0f;
              if (l_fFactor > 1.0f)
                l_fFactor = 1.0f;
            }

            m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * l_fFactor), 0, 0, 0), m_cRect);
          }

          // Fade in because of a respawn
          if ((m_iFade & (int)enFade::RespawnIn) == (int)enFade::RespawnIn) {
            int l_iStepSince = m_iStep - m_iAlphaStep[c_iAlphaIdxRespawnIn];
            irr::f32 l_fFactor = 1.0f;

            if (l_iStepSince < 120) {
              l_fFactor = ((irr::f32)l_iStepSince) / 120.0f;
              if (l_fFactor > 1.0f)
                l_fFactor = 1.0f;
            }

            m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * (1.0f - l_fFactor)), 0, 0, 0), m_cRect);
          }

          // Fade out after finished
          if ((m_iFade & (int)enFade::FinishedOut)) {
            int l_iSince = m_iStep - m_iAlphaStep[c_iAlphaIdxFinishedOut];

            irr::f32 l_fFactor = (l_iSince < 60) ? 0.0f : ((irr::f32)(l_iSince - 60) / 100.0f);
            l_fFactor = std::min(1.0f, l_fFactor);

            m_pDrv->draw2DRectangle(irr::video::SColor((irr::s32)(l_fFactor * 255.0f), 0, 0, 0), m_cRect);
          }


          // Fade in after finished (grey overlay needs be be rendered below everything else)
          if ((m_iFade & (int)enFade::FinishedIn) == (int)enFade::FinishedIn || (m_iFade & (int)enFade::GamePaused) == (int)enFade::GamePaused) {
            m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), m_cRect);
          }
          break;
        }

        case enCall::End: {
          if ((m_iFade & (int)enFade::NextRace) == (int)enFade::NextRace) {
            irr::f32 l_fFade = (m_iStep < 120) ? 1.0f : 1.0f - (((irr::f32)m_iStep - 120.0f) / 120.0f);

            if (l_fFade <= 0) {
              setFadeFlag(enFade::NextRace, false);
            }
            else {
              m_pGlobal->drawNextRaceScreen(l_fFade);
            }
          }

          if ((m_iFade & (int)enFade::RaceEnd) == (int)enFade::RaceEnd) {
            int l_iSince = m_iStep - m_iAlphaStep[c_iAlphaIdxRaceEnd];

            irr::f32 l_fFactor = std::min(1.0f, ((irr::f32)l_iSince) / 60.0f);
            m_pDrv->draw2DRectangle(irr::video::SColor((irr::s32)(l_fFactor * 255.0f), 0, 0, 0), m_cRect);
          }
          break;
        }
      }
    }
  }
}