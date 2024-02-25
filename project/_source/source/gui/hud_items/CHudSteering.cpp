// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudSteering.h>
#include <helpers/CDataHelpers.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    const int c_iIndexLeft       = 0;
    const int c_iIndexRight      = 1;
    const int c_iIndexAccelerate = 2;
    const int c_iIndexBack       = 3;
    const int c_iIndexBrake      = 4;
    const int c_iIndexRespawn    = 5;
    const int c_iIndexNoThrottle = 6;

    CHudSteering::CHudSteering(irr::video::IVideoDriver* a_pDrv, const irr::core::recti& a_cViewport) : m_pDrv(a_pDrv), m_iHeight(0) {
      // We use the "big" font to determine the size of the controls
      irr::gui::IGUIFont *l_pFont = CGlobal::getInstance()->getFont(dustbin::enFont::Big, irr::core::dimension2du(a_cViewport.getWidth(), a_cViewport.getHeight()));

      if (l_pFont == nullptr)
        l_pFont = CGlobal::getInstance()->getGuiEnvironment()->getSkin()->getFont();

      irr::u32 l_iHeight = l_pFont != nullptr ? l_pFont->getDimension(L"Test").Height : a_cViewport.getHeight() / 64;

      m_cRender = irr::core::dimension2du(l_iHeight, l_iHeight);

      m_cSize.Width  = a_cViewport.getWidth () / 4;
      m_cSize.Height = l_pFont != nullptr ? 4 * l_pFont->getDimension(L"Test").Height / 3 : a_cViewport.getHeight() / 56;

      std::string l_sFolder = helpers::getIconFolder(a_cViewport.getHeight());

      std::string l_sIcons[] = {
        l_sFolder + "ctrl_left",
        l_sFolder + "ctrl_right",
        l_sFolder + "ctrl_accelerate",
        l_sFolder + "ctrl_back",
        l_sFolder + "ctrl_brake",
        l_sFolder + "ctrl_respawn",
        l_sFolder + "ctrl_nothrottle"
      };

      std::string l_sPostfix[] = {
        "_off.png",
        ".png"
      };

      for (int l_iIcon = 0; l_iIcon < 7; l_iIcon++) {
        for (int l_iPostfix = 0; l_iPostfix < 2; l_iPostfix++) {
          m_pIcons[l_iIcon][l_iPostfix] = m_pDrv->getTexture((l_sIcons[l_iIcon] + l_sPostfix[l_iPostfix]).c_str());

          if (m_pIcons[l_iIcon][l_iPostfix] != nullptr)
            m_cSource = irr::core::recti(irr::core::position2di(0, 0), m_pIcons[l_iIcon][l_iPostfix]->getOriginalSize());
        }
      }

      if (m_cSize.Height < 6)
        m_cSize.Height = 6;

      irr::s32 l_iTop  = (m_cSize.Height - m_cRender.Height) / 2;

      m_cPositions[0] = irr::core::position2di(-(irr::s32)(        m_cSize  .Width / 2), l_iTop);
      m_cPositions[1] = irr::core::position2di(-(irr::s32)(    5 * m_cRender.Width / 2), l_iTop);
      m_cPositions[2] = irr::core::position2di(-(irr::s32)(        m_cRender.Width / 2), l_iTop);
      m_cPositions[3] = irr::core::position2di(                3 * m_cRender.Width / 2 , l_iTop);
      m_cPositions[4] = irr::core::position2di(m_cSize.Width / 2 - m_cRender.Width     , l_iTop);

      int l_iOffset = m_cSize.Width / 64;

      m_cLeft  = irr::core::recti(m_cPositions[0].X + m_cRender.Width + l_iOffset, m_cPositions[0].Y, m_cPositions[1].X - l_iOffset, m_cPositions[0].Y + m_cRender.Height);
      m_cBack  = irr::core::recti(m_cPositions[1].X + m_cRender.Width + l_iOffset, m_cPositions[1].Y, m_cPositions[2].X - l_iOffset, m_cPositions[1].Y + m_cRender.Height);
      m_cAccel = irr::core::recti(m_cPositions[2].X + m_cRender.Width + l_iOffset, m_cPositions[2].Y, m_cPositions[3].X - l_iOffset, m_cPositions[2].Y + m_cRender.Height);
      m_cRight = irr::core::recti(m_cPositions[3].X + m_cRender.Width + l_iOffset, m_cPositions[3].Y, m_cPositions[4].X - l_iOffset, m_cPositions[3].Y + m_cRender.Height);

      l_iOffset = m_cLeft.getHeight() / 4;
      m_cLeft.UpperLeftCorner .Y += l_iOffset;
      m_cLeft.LowerRightCorner.Y -= l_iOffset;

      l_iOffset = m_cBack.getWidth() / 4;
      m_cBack.UpperLeftCorner .X += l_iOffset;
      m_cBack.LowerRightCorner.X -= l_iOffset;

      l_iOffset = m_cAccel.getWidth() / 4;
      m_cAccel.UpperLeftCorner .X += l_iOffset;
      m_cAccel.LowerRightCorner.X -= l_iOffset;

      l_iOffset = m_cRight.getHeight() / 4;
      m_cRight.UpperLeftCorner .Y += l_iOffset;
      m_cRight.LowerRightCorner.Y -= l_iOffset;

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
      /*irr::core::recti l_cRect = irr::core::recti(
        a_cPos, a_cPos + irr::core::vector2di((irr::s32)((irr::f32)m_cSize.Width * a_fFactor / 2.0f), m_iHeight)
      );

      if (l_cRect.UpperLeftCorner.X > l_cRect.LowerRightCorner.X) {
        irr::s32 l_iDummy = l_cRect.UpperLeftCorner.X;
        l_cRect.UpperLeftCorner .X = l_cRect.LowerRightCorner.X;
        l_cRect.LowerRightCorner.X = l_iDummy;
      }

      m_pDrv->draw2DRectangle(a_cColor, l_cRect, &a_cClip);*/
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
      for (int i = 0; i < 5; i++) {
        irr::video::ITexture *l_pTexture = nullptr;

        switch (i) {
          case 0: l_pTexture = a_fSteer    < 0.0f ? m_pIcons[c_iIndexLeft      ][1] :                      m_pIcons[c_iIndexLeft   ][0]                                  ; break;
          case 1: l_pTexture = a_bBrake           ? m_pIcons[c_iIndexBrake     ][1] :                      m_pIcons[c_iIndexBrake  ][0]                                  ; break;
          case 2: l_pTexture = a_fThrottle > 0.0f ? m_pIcons[c_iIndexAccelerate][1] : a_fThrottle < 0.0f ? m_pIcons[c_iIndexBack   ][1] : m_pIcons[c_iIndexNoThrottle][1]; break;
          case 3: l_pTexture = a_bRespawn         ? m_pIcons[c_iIndexRespawn   ][1] :                      m_pIcons[c_iIndexRespawn][0]                                  ; break;
          case 4: l_pTexture = a_fSteer    > 0.0f ? m_pIcons[c_iIndexRight     ][1] :                      m_pIcons[c_iIndexRight  ][0]                                  ; break;
        }

        if (l_pTexture != nullptr)
          m_pDrv->draw2DImage(l_pTexture, irr::core::recti(m_cPositions[i] + a_cPos, m_cRender), m_cSource, &a_cClip, nullptr, true);
      }

      if (a_fSteer < 0.0f) {
        irr::core::recti l_cRect = m_cLeft  + a_cPos; 
        l_cRect.UpperLeftCorner.X = (irr::s32)((irr::f32)l_cRect.LowerRightCorner.X + a_fSteer * (irr::f32)l_cRect.getWidth());
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 0, 0, 255), l_cRect, &a_cClip);
      }

      if (a_fThrottle > 0.0f) {
        irr::core::recti l_cRect = m_cAccel + a_cPos; 
        l_cRect.UpperLeftCorner.Y = (irr::s32)((irr::f32)l_cRect.LowerRightCorner.Y - a_fThrottle * (irr::f32)l_cRect.getHeight());
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 0, 255, 0), l_cRect, &a_cClip);
      }

      if (a_fThrottle < 0.0f) {
        irr::core::recti l_cRect = m_cBack  + a_cPos;
        l_cRect.LowerRightCorner.Y = (irr::s32)((irr::f32)l_cRect.UpperLeftCorner.Y - a_fThrottle * (irr::f32)l_cRect.getHeight());
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 255, 255, 0), l_cRect, &a_cClip);
      }

      if (a_fSteer > 0.0f) {
        irr::core::recti l_cRect = m_cRight + a_cPos;
        l_cRect.LowerRightCorner.X = (irr::s32)((irr::f32)l_cRect.UpperLeftCorner.X + a_fSteer * (irr::f32)l_cRect.getWidth());
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 0, 0, 255), l_cRect, &a_cClip);
      }

      return m_cSize.Height;
    }
  }
}