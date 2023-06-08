// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <gui/CRankingElement.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CRankingElement::CRankingElement(int a_iPosition, const irr::core::recti& a_cRect, const irr::video::SColor &a_cBackground, irr::gui::IGUIFont* a_pFont, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui) :
      m_iDeficit   (0),
      m_sName      (L""),
      m_sPosition  (std::to_wstring(a_iPosition) + L": "),
      m_cBackground(a_cBackground),
      m_cOriginal  (a_cBackground),
      m_cTextColor (irr::video::SColor(0xFF, 0, 0, 0)),
      m_cHlColor   (irr::video::SColor(232, 0, 255, 0)),
      m_cBotColor  (irr::video::SColor(255, 255, 255, 255)),
      m_pFont      (a_pFont),
      m_bHighLight (false),
      m_bWithdrawn (false),
      m_bVisible   (true),
      m_pBotClass  (nullptr)
    {
      irr::core::dimension2du l_cDimPos = m_pFont->getDimension(L"66: ");
      l_cDimPos.Width  = 3 * l_cDimPos.Width / 2;
      l_cDimPos.Height = a_cRect.getHeight();

      m_cRect = a_cRect;

      if (m_cRect.UpperLeftCorner.X < (irr::s32)m_pDrv->getScreenSize().Width / 2) {
        m_cBot = irr::core::recti(
          irr::core::position2di(m_cRect.UpperLeftCorner.X - m_cRect.getHeight(), m_cRect.UpperLeftCorner.Y),
          irr::core::dimension2du(m_cRect.getHeight(), m_cRect.getHeight())
        );
      }
      else {
        m_cBot = irr::core::recti(
          irr::core::position2di(m_cRect.LowerRightCorner.X, m_cRect.UpperLeftCorner.Y),
          irr::core::dimension2du(m_cRect.getHeight(), m_cRect.getHeight())
        );
      }

      m_cPosition = irr::core::recti(a_cRect.UpperLeftCorner, a_cRect.UpperLeftCorner + irr::core::vector2di(l_cDimPos.Width, l_cDimPos.Height));

      irr::core::dimension2du l_cDimDeficit = m_pFont->getDimension(L"+666.66");
      l_cDimDeficit.Width  = 3 * l_cDimDeficit.Width / 2;
      l_cDimDeficit.Height = a_cRect.getHeight();

      m_cDeficit = irr::core::recti(a_cRect.LowerRightCorner.X - l_cDimDeficit.Width, a_cRect.UpperLeftCorner.Y, a_cRect.LowerRightCorner.X, a_cRect.LowerRightCorner.Y);

      m_iDeficit = 0;

      m_cName = irr::core::recti(m_cPosition.LowerRightCorner.X, a_cRect.UpperLeftCorner.Y, m_cDeficit.UpperLeftCorner.X, a_cRect.LowerRightCorner.Y);

      m_iRaster = a_cRect.getHeight() /  4;
      m_iBorder = a_cRect.getHeight() / 16;

      if (m_iRaster < 8) m_iRaster = 8;
      if (m_iBorder < 1) m_iBorder = 1;

      m_cHlSize = irr::core::dimension2du(m_cName.getHeight() / 2, m_cName.getHeight() / 2);

      double l_fRadius = (float)(m_cName.getHeight() / static_cast<double>(4));

      for (int y = -(int)l_fRadius; y < (int)l_fRadius; y++) {
        double l_fY = (float)y;
        double l_fX1 = sqrt(l_fRadius * l_fRadius - l_fY * l_fY);
        double l_fX2 = -l_fX1;
        m_vHightLight.push_back(irr::core::line2di(irr::core::vector2di((irr::s32)(l_fX1 + l_fRadius), (irr::s32)(l_fY + l_fRadius)), irr::core::vector2di((irr::s32)(l_fX2 + l_fRadius), (irr::s32)(l_fY + l_fRadius))));
      }

      m_cHighLight = irr::core::vector2di(m_cName.LowerRightCorner.X - m_cName.getHeight(), m_cName.UpperLeftCorner.Y);
    }

    CRankingElement::~CRankingElement() {

    }

    /**
    * Update the data
    * @param a_sName name of the player
    * @param a_iDeficit deficit to the leader
    */
    void CRankingElement::setData(const std::wstring& a_sName, int a_iDeficit, bool a_bWithdrawn) {
      std::wstring l_sName = a_sName;

      if (l_sName.find_last_of(L'|') != std::wstring::npos) {
        std::string l_sIcon = "data/images/bot_" + helpers::ws2s(l_sName.substr(l_sName.find_last_of(L'|') + 1)) + ".png";
        l_sName = l_sName.substr(0, l_sName.find_last_of(L'|'));
        m_pBotClass = m_pDrv->getTexture(l_sIcon.c_str());

        if (m_pBotClass != nullptr) {
          m_cBotSrc = irr::core::recti(irr::core::position2di(0, 0), m_pBotClass->getOriginalSize());
        }
      }
      else m_pBotClass = nullptr;

      m_sName      = helpers::fitString(l_sName, m_pFont, irr::core::dimension2du(m_cName.getWidth(), m_cName.getHeight()));
      m_iDeficit   = a_iDeficit;
      m_bWithdrawn = a_bWithdrawn;
    }

    void CRankingElement::draw() {
      if (m_bVisible) {
        if (m_pBotClass != nullptr) {
          irr::video::SColor l_cColors[] = {
            m_cBotColor,
            m_cBotColor,
            m_cBotColor,
            m_cBotColor
          };

          m_pDrv->draw2DImage(m_pBotClass, m_cBot, m_cBotSrc, nullptr, l_cColors, true);
        }
        renderBackground(m_cRect, m_cBackground);

        m_pFont->draw(m_sPosition.c_str(), m_cPosition, m_cTextColor, true, true, &m_cRect);

        if (m_iDeficit != 0) {
          if (m_bWithdrawn) {
            m_pFont->draw(L"DNF", m_cDeficit, m_cTextColor, true, true, &m_cRect);
          }
          else {
            m_pFont->draw((L"+" + helpers::convertToTime(m_iDeficit)).c_str(), m_cDeficit, m_cTextColor, true, true, &m_cRect);
          }
        }

        m_pFont->draw(m_sName.c_str(), m_cName, m_cTextColor, false, true, &m_cRect);

        if (m_bHighLight) {
          irr::core::vector2di l_cPos = irr::core::vector2di(m_cRect.LowerRightCorner.X - 3 * m_cHlSize.Width / 2, m_cRect.getCenter().Y - m_cHlSize.Height / 2);

          for (std::vector<irr::core::line2di>::const_iterator it = m_vHightLight.begin(); it != m_vHightLight.end(); it++) {
            m_pDrv->draw2DLine(l_cPos + (*it).start, l_cPos + (*it).end, m_cHlColor);
          }
        }
      }
    }


    /**
    * Set the alpha value
    * @param a_fAlpha the alpha value
    */
    void CRankingElement::setAlpha(irr::f32 a_fAlpha) {
      m_cBackground = m_cOriginal;
      irr::u32 l_iAlpha = (irr::u32)(((irr::f32)m_cOriginal.getAlpha()) * a_fAlpha);
      m_cBackground.setAlpha(l_iAlpha);
      m_cBorder    .setAlpha((irr::u32)(255.0f * a_fAlpha));
      m_cTextColor .setAlpha((irr::u32)(255.0f * a_fAlpha));
      m_cHlColor   .setAlpha((irr::u32)(232.0f * a_fAlpha));
      m_cBotColor  .setAlpha((irr::u32)(232.0f * a_fAlpha));
    }

    /**
    * Highlight this player (to mark the marble of the HUD in the starting grid)
    * @para a_bHighLight Hightlight or don't
    */
    void CRankingElement::highlight(bool a_bHighLight) {
      m_bHighLight = a_bHighLight;
    }

    /**
    * Set the item visible or invisible
    * @param a_bVisible visibility flag
    */
    void CRankingElement::setVisible(bool a_bVisible) {
      m_bVisible = a_bVisible;
    }
  }
}