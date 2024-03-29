// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <gui/hud_items/CHudAiHelp.h>
#include <helpers/CDataHelpers.h>
#include <CGlobal.h>
#include <string>
#include <tuple>

namespace dustbin {
  namespace gui {

    /**
    * The constructor
    * @param a_pDrv the video driver
    * @param a_cRect the rect to render to
    * @param a_eAiHelp the AI help level for the player
    */
    CHudAiHelp::CHudAiHelp(irr::video::IVideoDriver* a_pDrv, const irr::core::recti& a_cRect, data::SPlayerData::enAiHelp a_eAiHelp) : m_pDrv(a_pDrv) {
      std::string l_sPath = helpers::getIconFolder(a_cRect.getHeight());

      /*
      Left      = 0,
      Forward   = 1,
      Brake     = 2,
      Automatic = 3,
      Respawn   = 4,
      Right     = 5,
      Backward  = 6,
      */
      // The base filenames
      std::tuple<data::SPlayerData::enAiHelp, std::string> l_aFiles[] = {
        std::make_tuple(data::SPlayerData::enAiHelp::High, l_sPath + "aihelp_left"      ),
        std::make_tuple(data::SPlayerData::enAiHelp::Low , l_sPath + "aihelp_accelerate"),
        std::make_tuple(data::SPlayerData::enAiHelp::Low , l_sPath + "aihelp_brake"     ),
        std::make_tuple(data::SPlayerData::enAiHelp::Off , l_sPath + "aihelp_automatic" ),
        std::make_tuple(data::SPlayerData::enAiHelp::Low , l_sPath + "aihelp_respawn"   ),
        std::make_tuple(data::SPlayerData::enAiHelp::Low , l_sPath + "aihelp_back"      ),
        std::make_tuple(data::SPlayerData::enAiHelp::High, l_sPath + "aihelp_right"     ),
        std::make_tuple(data::SPlayerData::enAiHelp::Off , ""                              )
      };

      // The postfix strings for the filenames
      std::string l_aPostfix[] = {
        ".png",
        "_off.png",
        ""
      };

      // Initialize array with null
      for (int i = 0; i < 7; i++)
        for (int j = 0; j < 2; j++)
          m_aIcons[i][j] = nullptr;

      // Load the textures
      for (int l_iFile = 0; std::get<1>(l_aFiles[l_iFile]) != ""; l_iFile++) {
        for (int l_iPostfix = 0; l_aPostfix[l_iPostfix] != ""; l_iPostfix++) {
          if ((int)a_eAiHelp <= (int)std::get<0>(l_aFiles[l_iFile]) || std::get<0>(l_aFiles[l_iFile]) == data::SPlayerData::enAiHelp::Off) {
            m_aIcons[l_iFile][l_iPostfix] = a_pDrv->getTexture((std::get<1>(l_aFiles[l_iFile]) + l_aPostfix[l_iPostfix]).c_str());

            // We set the source rect every time to make sure it's valid
            if (m_aIcons[l_iFile][l_iPostfix] != nullptr) {
              m_cSource = irr::core::recti(irr::core::vector2di(0, 0), m_aIcons[l_iFile][l_iPostfix]->getSize());
            }
          }
        }

        m_aState[l_iFile] = enState::Inactive;
      }

      int l_iRaster = CGlobal::getInstance()->getRasterSize();

#ifdef _ANDROID
      irr::core::dimension2du l_cSize = irr::core::dimension2du(2 * l_iRaster, 2 * l_iRaster);
#else
      irr::core::dimension2du l_cSize = irr::core::dimension2du(3 * l_iRaster, 3 * l_iRaster);
#endif

      int l_iStartX = -(3 * (int)l_cSize.Width + (int)l_cSize.Width / 2);

      irr::core::position2di l_cPos = irr::core::position2di(l_iStartX, l_cSize.Height);

      irr::core::recti l_cTotal;

      if (a_eAiHelp == data::SPlayerData::enAiHelp::High || a_eAiHelp == data::SPlayerData::enAiHelp::Medium) {
        l_cPos.Y += l_cSize.Width;
      }

      for (int l_iIndex = 0; l_iIndex < (int)enIcons::Count; l_iIndex++) {
        m_aRects[l_iIndex] = irr::core::recti(l_cPos, l_cSize);

        l_cTotal.addInternalPoint(m_aRects[l_iIndex].UpperLeftCorner);
        l_cTotal.addInternalPoint(m_aRects[l_iIndex].LowerRightCorner);

        l_cPos.X += l_cSize.Width;
      }

      m_cOffset.X = 0;
      m_cOffset.Y = -l_cTotal.getHeight();
    }

    CHudAiHelp::~CHudAiHelp() {
    }

    /**
    * Change the state of an icon
    * @param a_eIcons the icon to change
    * @param a_eState the new state of the icon
    */
    void CHudAiHelp::setIconState(enIcons a_eIcon, enState a_eState) {
      m_aState[(int)a_eIcon] = a_eState;
    }

    /**
    * Render the AI help
    * @param l_cPosition the position (lower center) to render to
    * @param a_cClip the clipping rectangle
    */
    void CHudAiHelp::render(const irr::core::position2di a_cPosition, const irr::core::recti &a_cClip) {
      int l_iSteer[] = {
        (int)enIcons::Left,
        (int)enIcons::Right,
        -1
      };

      for (int i = 0; l_iSteer[i] != -1; i++)
        if (m_aIcons[l_iSteer[i]][(int)m_aState[i]] != nullptr)
          m_pDrv->draw2DImage(m_aIcons[l_iSteer[i]][(int)m_aState[l_iSteer[i]]], m_aRects[l_iSteer[i]] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);

      if (m_aState[(int)enIcons::Automatic] == enState::Active)
        m_pDrv->draw2DImage(m_aIcons[(int)enIcons::Automatic][0], m_aRects[(int)enIcons::Automatic] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);
      else if (m_aState[(int)enIcons::Forward] == enState::Active)
        m_pDrv->draw2DImage(m_aIcons[(int)enIcons::Forward][0], m_aRects[(int)enIcons::Automatic] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);
      else if (m_aState[(int)enIcons::Backward] == enState::Active)
        m_pDrv->draw2DImage(m_aIcons[(int)enIcons::Backward][0], m_aRects[(int)enIcons::Automatic] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);
      else
        m_pDrv->draw2DImage(m_aIcons[(int)enIcons::Automatic][1], m_aRects[(int)enIcons::Automatic] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);

      /*for (int i = 0; i < (int)enIcons::Count; i++) {
        if (m_aIcons[i][(int)m_aState[i]] != nullptr) {
          m_pDrv->draw2DImage(m_aIcons[i][(int)m_aState[i]], m_aRects[i] + m_cOffset + a_cPosition, m_cSource, &a_cClip, nullptr, true);
        }
      }*/
    }
  }
}