// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <helpers/CStringHelpers.h>
#include <helpers/CDataHelpers.h>
#include <gui/CRankingElement.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CRankingElement::CRankingElement(int a_iPosition, const irr::core::recti& a_cRect, const irr::video::SColor &a_cBackground, irr::gui::IGUIFont* a_pFont, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment* a_pGui, const irr::core::recti &a_cViewport) :
      m_iDeficit   (0),
      m_sName      (L""),
      m_sPosition  (std::to_wstring(a_iPosition) + L": "),
      m_cBackground(a_cBackground),
      m_cOriginal  (a_cBackground),
      m_cViewport  (a_cViewport),
      m_cTextColor (irr::video::SColor(0xFF, 0, 0, 0)),
      m_cBotColor  (irr::video::SColor(255, 255, 255, 255)),
      m_pFont      (a_pFont),
      m_bWithdrawn (false),
      m_bVisible   (true),
      m_pBotClass  (nullptr),
      m_sNumber    (L"")
    {
      irr::core::dimension2du l_cDimPos = m_pFont->getDimension(L"66: ");
      l_cDimPos.Width  = 3 * l_cDimPos.Width / 2;
      l_cDimPos.Height = a_cRect.getHeight();

      m_cRect = a_cRect;

      m_cBot = irr::core::recti(
        irr::core::position2di(m_cRect.LowerRightCorner.X - m_cRect.getHeight(), m_cRect.UpperLeftCorner.Y),
        irr::core::dimension2du(m_cRect.getHeight(), m_cRect.getHeight())
      );

      m_cPosition = irr::core::recti(a_cRect.UpperLeftCorner, a_cRect.UpperLeftCorner + irr::core::vector2di(l_cDimPos.Width, l_cDimPos.Height));

      irr::core::dimension2du l_cDimDeficit = m_pFont->getDimension(L"+666.66");
      l_cDimDeficit.Width  = 3 * l_cDimDeficit.Width / 2;
      l_cDimDeficit.Height = a_cRect.getHeight();

      m_cDeficit = irr::core::recti(a_cRect.LowerRightCorner.X - l_cDimDeficit.Width - 3 * m_cRect.getHeight(), a_cRect.UpperLeftCorner.Y, a_cRect.LowerRightCorner.X - m_cRect.getHeight(), a_cRect.LowerRightCorner.Y);

      m_iDeficit = 0;

      m_cName = irr::core::recti(m_cPosition.LowerRightCorner.X, a_cRect.UpperLeftCorner.Y, m_cDeficit.UpperLeftCorner.X, a_cRect.LowerRightCorner.Y);

      m_iRaster = a_cRect.getHeight() /  4;
      m_iBorder = a_cRect.getHeight() / 16;

      if (m_iRaster < 8) m_iRaster = 8;
      if (m_iBorder < 1) m_iBorder = 1;

      m_cNumberRect = irr::core::recti(m_cRect.LowerRightCorner.X - 2 * m_cRect.getHeight() + 2 * m_iBorder, m_cRect.UpperLeftCorner.Y + 2 * m_iBorder, m_cRect.LowerRightCorner.X - 2 * m_iBorder - m_cRect.getHeight(), m_cRect.LowerRightCorner.Y - 2 * m_iBorder);
    }

    CRankingElement::~CRankingElement() {

    }

    /**
    * Update the data
    * Update the data
    * @param a_sName name of the player
    * @param a_iDeficit deficit to the leader
    * @param a_bWithdrawn did the player withdraw from the race (true) or finish normally (false)?
    * @param a_cBack the background color of the number icon
    * @param a_cNumber the number color of the number icon
    * @param a_cFFrame the frame color of the number icon
    * @param a_sNumber the starting number for the number icon
    */
    void CRankingElement::setData(const std::wstring& a_sName, int a_iDeficit, bool a_bWithdrawn, const irr::video::SColor &a_cBack, const irr::video::SColor &a_cNumber, const irr::video::SColor &a_cFrame, const std::wstring &a_sNumber) {
      std::wstring l_sName = L"";
      std::wstring l_sBot  = L"";

      if (helpers::splitStringNameBot(a_sName, l_sName, l_sBot)) {
        std::string l_sIcon = helpers::getIconFolder(m_cViewport.getHeight()) + "bot_" + helpers::ws2s(l_sBot) + ".png";
        m_pBotClass = m_pDrv->getTexture(l_sIcon.c_str());

        if (m_pBotClass != nullptr) {
          m_cBotSrc = irr::core::recti(irr::core::position2di(0, 0), m_pBotClass->getOriginalSize());
        }
      }
      else m_pBotClass = nullptr;

      m_sName       = helpers::fitString(l_sName, m_pFont, irr::core::dimension2du(m_cName.getWidth(), m_cName.getHeight()));
      m_iDeficit    = a_iDeficit;
      m_bWithdrawn  = a_bWithdrawn;

      m_cNumberBack  = a_cBack;
      m_cNumberText  = a_cNumber;
      m_cNumberFrame = a_cFrame;
      m_sNumber      = a_sNumber;
    }

    void CRankingElement::draw() {
      if (m_bVisible) {
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

        renderBackground(m_cNumberRect, m_cNumberBack, &m_cNumberFrame);
        m_pFont->draw(m_sNumber.c_str(), m_cNumberRect, m_cNumberText, true, true, &m_cNumberRect);

        if (m_pBotClass != nullptr) {
          irr::video::SColor l_cColors[] = {
            m_cBotColor,
            m_cBotColor,
            m_cBotColor,
            m_cBotColor
          };

          m_pDrv->draw2DImage(m_pBotClass, m_cBot, m_cBotSrc, nullptr, l_cColors, true);
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
      m_cBackground .setAlpha(l_iAlpha);
      m_cBorder     .setAlpha((irr::u32)(255.0f * a_fAlpha));
      m_cTextColor  .setAlpha((irr::u32)(255.0f * a_fAlpha));
      m_cBotColor   .setAlpha((irr::u32)(232.0f * a_fAlpha));
      m_cNumberBack .setAlpha((irr::u32)(232.0f * a_fAlpha));
      m_cNumberText .setAlpha((irr::u32)(232.0f * a_fAlpha));
      m_cNumberFrame.setAlpha((irr::u32)(232.0f * a_fAlpha));
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