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

          if (m_vControls.size() >= std::get<2>(l_tItem)) {
            std::string l_sItemName = m_vControls[std::get<2>(l_tItem)].m_sName;

            if (l_sItemName == "Forward") {
              l_cLabelPos.X -= l_cLabelDim.Width / 2;
              l_cLabelPos.Y -= l_cLabelDim.Height;
              l_bAdd = true;
            } else if (l_sItemName == "Backward") {
              l_cLabelPos.X -= l_cLabelDim.Width / 2;
              l_bAdd = true;
            } else if (l_sItemName == "Right") {
              l_cLabelPos.Y -= l_cLabelDim.Height / 2;
              l_bAdd = true;
            } else if (l_sItemName == "Left") {
              l_cLabelPos.X -= l_cLabelDim.Width;
              l_cLabelPos.Y -= l_cLabelDim.Height / 2;
              l_bAdd = true;
            } else {
              if (l_sItemName == "Brake") {
                l_bAdd = true;
              } else if (l_sItemName == "Rearview") {
                l_bAdd = true;
              } else if (l_sItemName == "Respawn") {
                l_bAdd = true;
              } else if (l_sItemName == "Pause") {
                l_bAdd = true;
              } else if (l_sItemName == "Cancel Race") {
                l_bAdd = true;
              }

              if (l_bAdd)
                l_cLabelPos.Y -= l_cLabelDim.Height / 2;
            }

            if (l_bAdd) {
              m_mLabels[std::get<0>(l_tItem)] = std::make_tuple(
                irr::core::recti(l_cLabelPos, l_cLabelDim), l_cLabelTxt, false, false, false);
            }
          }
        }
      }
    }

    irr::gui::EGUI_ELEMENT_TYPE CControllerUi_Game::getType() {
      return (irr::gui::EGUI_ELEMENT_TYPE)g_ControllerUiGameId;
    }


    /**
    * Fill the controller from a serialized string. If the vector of controllers is empty
    * it will be filled, otherwise the corresponding items will be updated
    * @param a_sData the serialized string to load the data from
    */
    void CControllerUi_Game::deserialize(const std::string a_sData) {
      std::string l_sData = a_sData;
      if (l_sData == "" || l_sData.substr(0, std::string("DustbinController").size()) != "DustbinController") {
#ifdef _ANDROID
        l_sData = "DustbinController;control;f%3bl%3bForward%3bh%3bb%3bn%3bM%3br%3ba%3bt%3ba%3bx%3bc%3bD%3b-b;control;f%3bl%3bBackward%3bh%3bb%3bn%3bO%3br%3ba%3bt%3ba%3bx%3bc%3bD%3bb;control;f%3bl%3bLeft%3bh%3bb%3bn%3bL%3br%3ba%3bt%3ba%3bx%3ba%3bD%3b-b;control;f%3bl%3bRight%3bh%3bb%3bn%3bN%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bBrake%3bh%3bc%3bn%3bG%3br%3ba%3bt%3ba%3bx%3ba%3bD%3bb;control;f%3bl%3bRearview%3bh%3bc%3bn%3bj%3br%3ba%3bt%3be%3bx%3ba%3bD%3bb;control;f%3bl%3bRespawn%3bh%3bc%3bn%3bn%3br%3ba%3bt%3bf%3bx%3ba%3bD%3bb;control;f%3bl%3bPause%3bh%3bc%3bn%3bt%3br%3ba%3bt%3bg%3bx%3ba%3bD%3bb;control;f%3bl%3bCancel%2520Race%3bh%3bc%3bn%3bB%3br%3ba%3bt%3bh%3bx%3ba%3bD%3bb";
#else
        // Default controls for new player
        l_sData = "DustbinController;control;Key;Forward;Controller%20%28GAME%20FOR%20WINDOWS%29;M;a;a;a;-0md;b;control;Key;Backward;Controller%20%28GAME%20FOR%20WINDOWS%29;O;a;a;c;-0md;b;control;Key;Left;Controller%20%28GAME%20FOR%20WINDOWS%29;L;a;a;a;-0md;-b;control;Key;Right;Controller%20%28GAME%20FOR%20WINDOWS%29;N;a;a;a;-0md;b;control;Key;Brake;Controller%20%28GAME%20FOR%20WINDOWS%29;G;a;a;a;-0md;b;control;Key;Rearview;Controller%20%28GAME%20FOR%20WINDOWS%29;j;a;e;a;-0md;b;control;Key;Respawn;Controller%20%28GAME%20FOR%20WINDOWS%29;n;a;f;a;-0md;b";
#endif
      }
      CControllerBase::deserialize(l_sData);
    }
  }
}