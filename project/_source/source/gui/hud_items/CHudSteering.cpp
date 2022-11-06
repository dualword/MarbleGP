// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudSteering.h>

namespace dustbin {
  namespace gui {
    CHudSteering::CHudSteering(irr::video::IVideoDriver* a_pDrv, const irr::core::recti& a_cViewport) : m_pDrv(a_pDrv), m_iHeight(0) {
      m_cSize.Width  = a_cViewport.getWidth () / 4;
      m_cSize.Height = a_cViewport.getHeight() / 64;

      if (m_cSize.Height < 6)
        m_cSize.Height = 6;

      m_iHeight = m_cSize.Height / 3;
    }

    CHudSteering::~CHudSteering() {
    }

    /**
    * Draw steering or throttle
    * @param a_cPos the (central) position to draw to
    * @param a_fFactor the steering value (-1 .. 1)
    * @param a_cColor the color to use
    * @param a_cClip the clipping rectangle
    */
    void CHudSteering::drawControlItem(const irr::core::vector2di& a_cPos, irr::f32 a_fFactor, const irr::video::SColor& a_cColor, const irr::core::recti &a_cClip) {
      irr::core::recti l_cRect = irr::core::recti(
        a_cPos, a_cPos + irr::core::vector2di((irr::s32)((irr::f32)m_cSize.Width * a_fFactor / 2.0f), m_iHeight)
      );

      if (l_cRect.UpperLeftCorner.X > l_cRect.LowerRightCorner.X) {
        irr::s32 l_iDummy = l_cRect.UpperLeftCorner.X;
        l_cRect.UpperLeftCorner .X = l_cRect.LowerRightCorner.X;
        l_cRect.LowerRightCorner.X = l_iDummy;
      }

      m_pDrv->draw2DRectangle(a_cColor, l_cRect, &a_cClip);
    }

    /**
    * Render the HUD item
    * @param a_cPos the (center) position to render to
    * @param a_fSteer the current steering input (-1 .. 1)
    * @param a_fThrottle the current throttle input (-1 .. 1)
    * @param a_bBrake is the brake active?
    * @param a_bRespawn is the manual respawn button pressed?
    * @param a_cClip the clipping rectangle
    */
    int CHudSteering::render(const irr::core::vector2di &a_cPos,irr::f32 a_fSteer, irr::f32 a_fThrottle, bool a_bBrake, bool a_bRespawn, const irr::core::recti &a_cClip) {
      irr::core::vector2di l_cPos = a_cPos;

      irr::video::SColor l_cColor = a_fThrottle > 0.0f ? irr::video::SColor(0xFF, 0, 0xFF, 0) : a_fThrottle < 0.0f ? irr::video::SColor(0xFF, 0xFF, 0x80, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);

      drawControlItem(l_cPos, a_fSteer   , irr::video::SColor(0xFF, 0, 0, 0xFF), a_cClip); l_cPos.Y += m_iHeight;
      drawControlItem(l_cPos, a_fThrottle, l_cColor                            , a_cClip);

      l_cPos.Y += m_iHeight;

      if (a_bBrake)
        m_pDrv->draw2DRectangle(irr::video::SColor(0xFF, 0xFF, 0, 0), irr::core::recti(l_cPos - irr::core::vector2di(m_cSize.Width / 2, 0), irr::core::dimension2du(m_cSize.Width, m_iHeight)), &a_cClip);

      return m_cSize.Height;
    }
  }
}