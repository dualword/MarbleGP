#include <controller/touch/CControllerTouchSide_Left.h>

namespace dustbin {
  namespace controller {
    CControllerTouchSide::enControl CControllerTouchSide_Left::getTouchControl(const irr::core::vector2di& a_cTouch) {
      enControl l_eRet = enControl::Count;

      irr::s32 x = a_cTouch.X;
      irr::s32 y = a_cTouch.Y;

      if (x < m_cViewport.getWidth() / 3) {
        if (y < m_aControls[(int)enControl::Respawn].m_cDestination.LowerRightCorner.Y) {
          l_eRet = enControl::Respawn;
        }
        else {
          enControl l_aSteer[] = {
            enControl::Left,
            enControl::NeutralS,
            enControl::Right,
            enControl::Count
          };

          for (int i = 0; l_aSteer[i] != enControl::Count; i++) {
            if (x >= m_aControls[(int) l_aSteer[i]].m_cDestination.UpperLeftCorner.X &&
              x <= m_aControls[(int) l_aSteer[i]].m_cDestination.LowerRightCorner.X) {
              l_eRet = l_aSteer[i];
              break;
            }
          }
        }
      }

      if (l_eRet == enControl::Count && x > 2 * m_cViewport.getWidth() / 3) {
        if (y < m_aControls[(int)enControl::Rearview].m_cDestination.LowerRightCorner.Y) {
          l_eRet = enControl::Rearview;
        }
        else if (y > m_aControls[(int)enControl::Brake].m_cDestination.UpperLeftCorner.Y) {
          l_eRet = enControl::Brake;
        }
        else {
          enControl l_aThrottle[] = {
            enControl::Forward,
            enControl::NeutralV,
            enControl::Backward,
            enControl::Count
          };

          for (int i = 0; l_aThrottle[i] != enControl::Count; i++) {
            if (y > m_aControls[(int) l_aThrottle[i]].m_cDestination.UpperLeftCorner.Y) {
              l_eRet = l_aThrottle[i];
            }
          }
        }
      }

      return l_eRet;
    }

    CControllerTouchSide_Left::CControllerTouchSide_Left(irr::video::IVideoDriver* a_pDrv, const irr::core::recti& a_cViewport) : CControllerTouchSide(a_pDrv, a_cViewport) {
      irr::core::dimension2du l_cSize = irr::core::dimension2du (m_iThrottleHeight, m_iThrottleHeight);

      irr::s32 l_iRight  = m_cViewport.getWidth() - m_iThrottleHeight;
      irr::s32 l_iCenter = m_cViewport.getHeight() / 2 - m_iThrottleHeight / 2;

      addToControlMap(enControl::Forward , irr::core::recti(irr::core::vector2di(l_iRight             , l_iCenter - 3 * l_cSize.Height / 2), l_cSize), irr::video::SColor(128,   0, 255,   0), "data/images/ctrl_accelerate_off.png", "data/images/ctrl_accelerate.png");
      addToControlMap(enControl::NeutralV, irr::core::recti(irr::core::vector2di(l_iRight             , l_iCenter -     l_cSize.Height / 2), l_cSize), irr::video::SColor(128, 192, 192, 192), "data/images/ctrl_none_off.png", "data/images/ctrl_none.png");
      addToControlMap(enControl::Backward, irr::core::recti(irr::core::vector2di(l_iRight             , l_iCenter +     l_cSize.Height / 2), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_back_off.png", "data/images/ctrl_back.png");
      addToControlMap(enControl::Brake   , irr::core::recti(irr::core::vector2di(l_iRight             , l_iCenter + 3 * l_cSize.Height / 2), l_cSize), irr::video::SColor(128, 255,   0,   0), "data/images/ctrl_brake_off.png"     , "data/images/ctrl_brake.png");
      addToControlMap(enControl::Left    , irr::core::recti(irr::core::vector2di(                    0,                          l_iCenter), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_left_off.png"      , "data/images/ctrl_left.png");
      addToControlMap(enControl::NeutralS, irr::core::recti(irr::core::vector2di(    m_iThrottleHeight,                          l_iCenter), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_none_off.png"      , "data/images/ctrl_none.png");
      addToControlMap(enControl::Right   , irr::core::recti(irr::core::vector2di(2 * m_iThrottleHeight,                          l_iCenter), l_cSize), irr::video::SColor(128,   0,   0, 255), "data/images/ctrl_right_off.png"     , "data/images/ctrl_right.png");
      addToControlMap(enControl::Rearview, irr::core::recti(irr::core::vector2di(                    0,                                  0), l_cSize), irr::video::SColor(128, 255,   0, 255), "data/images/ctrl_rearview_off.png"  , "data/images/ctrl_rearview.png");
      addToControlMap(enControl::Respawn , irr::core::recti(irr::core::vector2di(l_iRight             ,                                  0), l_cSize), irr::video::SColor(128, 255,   0, 255), "data/images/ctrl_respawn_off.png"   , "data/images/ctrl_respawn.png");
    }

    CControllerTouchSide_Left::~CControllerTouchSide_Left() {

    }
  }
}
