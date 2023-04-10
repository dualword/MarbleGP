// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CControllerUi_Menu.h>

namespace dustbin {
  namespace gui {
    CControllerUi_Menu::CControllerUi_Menu(irr::gui::IGUIElement* a_pParent) : CControllerUi(a_pParent, (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiMenuId) {
      m_pBackground = m_pDrv->getTexture("data/images/ctrl_config_menu.png");

      if (m_pBackground != nullptr) {
        m_cSource = irr::core::recti(irr::core::vector2di(0, 0), m_pBackground->getSize());
      }
    }

    CControllerUi_Menu::~CControllerUi_Menu() {
    }

    /**
    * This method builds the UI for the controller
    * @param a_pParent the parent element. The outbox of this element will be used for the UI
    */
    void CControllerUi_Menu::buildUi(irr::gui::IGUIElement* a_pParent) {
      printf("Build UI: %i, %i - %i, %i\n", AbsoluteClippingRect.UpperLeftCorner.X, AbsoluteClippingRect.UpperLeftCorner.Y, AbsoluteClippingRect.LowerRightCorner.X, AbsoluteClippingRect.LowerRightCorner.Y);

      if (m_pBackground != nullptr) {
        // Calculate the drawing rect
        irr::f32 l_fRatio  = (irr::f32)m_pBackground->getSize().Width / (irr::f32)m_pBackground->getSize().Height;
        irr::f32 l_fWidth  = 0.8f * (irr::f32)AbsoluteClippingRect.getWidth ();
        irr::f32 l_fHeight = 0.8f * (irr::f32)AbsoluteClippingRect.getHeight();
        irr::f32 l_fFactX  = l_fWidth  / m_pBackground->getSize().Width;
        irr::f32 l_fFactY  = l_fHeight / m_pBackground->getSize().Height;

        m_cDraw = irr::core::recti(
          10 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100 - (irr::s32)l_fHeight + AbsoluteClippingRect.UpperLeftCorner.Y,
          90 * AbsoluteClippingRect.getWidth () / 100                       + AbsoluteClippingRect.UpperLeftCorner.X,
          90 * AbsoluteClippingRect.getHeight() / 100                       + AbsoluteClippingRect.UpperLeftCorner.Y
        );

        std::vector<std::tuple<std::string, irr::core::vector2df, int>> l_vItems = {
          std::make_tuple("Up"    , irr::core::vector2df(497.0f,  27.0f), 0),  // 0 == up
          std::make_tuple("Down"  , irr::core::vector2df(497.0f, 393.0f), 1),  // 1 == down
          std::make_tuple("Left"  , irr::core::vector2df(304.0f, 211.0f), 2),  // 2 == left
          std::make_tuple("Right" , irr::core::vector2df(675.0f, 211.0f), 3),  // 3 == right
          std::make_tuple("Enter" , irr::core::vector2df( 71.0f,  29.0f), 4),  // 4 == Enter
          std::make_tuple("Ok"    , irr::core::vector2df( 71.0f, 328.0f), 5),  // 5 == ok
          std::make_tuple("Cancel", irr::core::vector2df( 71.0f, 392.0f), 6)   // 6 == cancel
        };

        for (std::tuple<std::string, irr::core::vector2df, int> l_tItem: l_vItems) {
          irr::core::position2di  l_cLabelPos = irr::core::vector2di((irr::s32)(std::get<1>(l_tItem).X * l_fFactX), (irr::s32)(std::get<1>(l_tItem).Y * l_fFactY)) + m_cDraw.UpperLeftCorner;
          std::wstring            l_cLabelTxt = getControlText(&m_vControls[std::get<2>(l_tItem)]);
          irr::core::dimension2du l_cLabelDim = m_pFont->getDimension(l_cLabelTxt.c_str());

          l_cLabelDim.Width  = 5 * l_cLabelDim.Width  / 4;
          l_cLabelDim.Height = 5 * l_cLabelDim.Height / 4;

          bool l_bAdd = false;

          if (m_vControls[std::get<2>(l_tItem)].m_sName == "Up") {
            l_cLabelPos.X -= l_cLabelDim.Width  / 2;
            l_cLabelPos.Y -= l_cLabelDim.Height;
            l_bAdd = true;
          }
          else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Down") {
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
            if (m_vControls[std::get<2>(l_tItem)].m_sName == "Enter") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Ok") {
              l_bAdd = true;
            }
            else if (m_vControls[std::get<2>(l_tItem)].m_sName == "Cancel") {
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

    irr::gui::EGUI_ELEMENT_TYPE CControllerUi_Menu::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiMenuId;
    }

  }
}