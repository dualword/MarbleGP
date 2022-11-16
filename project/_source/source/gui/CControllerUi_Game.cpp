// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CControllerUi_Game.h>

namespace dustbin {
  namespace gui {
    CControllerUi_Game::CControllerUi_Game(irr::gui::IGUIElement* a_pParent) : CControllerUi(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiGameId) {
      m_pBackground = m_pDrv->getTexture("data/images/ctrl_config_joykey.png");

      if (m_pBackground != nullptr) {
        m_cSource = irr::core::recti(irr::core::vector2di(0, 0), m_pBackground->getSize());
      }
    }

    CControllerUi_Game::~CControllerUi_Game() {
    }

    /**
    * This method builds the UI for the controller
    * @param a_pParent the parent element. The outbox of this element will be used for the UI
    */
    void CControllerUi_Game::buildUi(irr::gui::IGUIElement* a_pParent) {
      printf("Build UI: %i, %i - %i, %i\n", AbsoluteClippingRect.UpperLeftCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);

      if (m_pBackground != nullptr) {
        // Calculate the drawing rect
        irr::f32 l_fRatio  = (irr::f32)m_pBackground->getSize().Width / (irr::f32)m_pBackground->getSize().Height;
        irr::f32 l_fWidth  = 0.8f * (irr::f32)AbsoluteClippingRect.getWidth();
        irr::f32 l_fHeight = l_fWidth  / l_fRatio;
        irr::f32 l_fFactX  = l_fWidth  / m_pBackground->getSize().Width;
        irr::f32 l_fFactY  = l_fHeight / m_pBackground->getSize().Height;

        m_cDraw = irr::core::recti(
          10 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100 - (irr::s32)l_fHeight + AbsoluteClippingRect.UpperLeftCorner.Y,
          90 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100                       + AbsoluteClippingRect.UpperLeftCorner.Y
        );

        std::vector<std::tuple<std::string, irr::core::vector2df, int>> l_vItems = {
          std::make_tuple("Forward"    , irr::core::vector2df(497.0f,  27.0f), 0),  // 0 == forward
          std::make_tuple("Backward"   , irr::core::vector2df(497.0f, 393.0f), 1),  // 1 == backward
          std::make_tuple("Left"       , irr::core::vector2df(304.0f, 211.0f), 2),  // 2 == left
          std::make_tuple("Right"      , irr::core::vector2df(675.0f, 211.0f), 3),  // 3 == right
          std::make_tuple("Brake"      , irr::core::vector2df( 71.0f,  29.0f), 4),  // 4 == brake
          std::make_tuple("Rearview"   , irr::core::vector2df( 71.0f,  93.0f), 5),  // 5 == rearview
          std::make_tuple("Respawn"    , irr::core::vector2df( 71.0f, 156.0f), 6),  // 6 == respawn
          std::make_tuple("Pause"      , irr::core::vector2df( 71.0f, 328.0f), 7),  // 7 == pause
          std::make_tuple("Cancel Race", irr::core::vector2df( 71.0f, 392.0f), 8)   // 8 == cancel race
        };

        for (std::tuple<std::string, irr::core::vector2df, int> l_tItem: l_vItems) {
          irr::core::position2di  l_cLabelPos = irr::core::vector2di((irr::s32)(std::get<1>(l_tItem).X * l_fFactX), (irr::s32)(std::get<1>(l_tItem).Y * l_fFactY)) + m_cDraw.UpperLeftCorner;
          std::wstring            l_cLabelTxt = getControlText(&m_vControls[std::get<2>(l_tItem)]);
          irr::core::dimension2du l_cLabelDim = m_pFont->getDimension(l_cLabelTxt.c_str());

          l_cLabelDim.Width  = 5 * l_cLabelDim.Width  / 4;
          l_cLabelDim.Height = 5 * l_cLabelDim.Height / 4;

          bool l_bAdd = false;

          if (m_vControls[std::get<2>(l_tItem)].m_sName == "Forward") {
            l_cLabelPos.X -= l_cLabelDim.Width  / 2;
            l_cLabelPos.Y -= l_cLabelDim.Height;
            l_bAdd = true;
          }
          else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Backward") {
            l_cLabelPos.X -= l_cLabelDim.Width  / 2;
            l_bAdd = true;
          }
          else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Right") {
            l_cLabelPos.Y -= l_cLabelDim.Height / 2;
            l_bAdd = true;
          }
          else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Left") {
            l_cLabelPos.X -= l_cLabelDim.Width;
            l_cLabelPos.Y -= l_cLabelDim.Height / 2;
            l_bAdd = true;
          }
          else {
            if (m_vControls[std::get<2>(l_tItem)].m_sName == "Brake") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Rearview") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Respawn") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Pause") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Cancel Race") {
              l_bAdd = true;
            }

            if (l_bAdd)
              l_cLabelPos.Y -= l_cLabelDim.Height / 2;
          }

          if (l_bAdd) {
            m_mLabels[std::get<0>(l_tItem)] = std::make_tuple(irr::core::recti(l_cLabelPos, l_cLabelDim), l_cLabelTxt, false, false, false);
          }
        }
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CControllerUi_Game::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiGameId;
    }

  }
}