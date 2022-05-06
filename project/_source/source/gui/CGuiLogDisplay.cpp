/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <gui/CGuiLogDisplay.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CGuiLogDisplay::CGuiLogDisplay(irr::gui::IGUIElement* a_pParent) :
      IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_GuiLogDisplayId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pFont(CGlobal::getInstance()->getGuiEnvironment()->getSkin()->getFont()),
      m_pDrv(CGlobal::getInstance()->getVideoDriver()),
      m_iLineHeight(0)
    {
      m_iLineHeight = 4 * m_pFont->getDimension(L"TesTexT").Height / 3;

      m_pScroll = CGlobal::getInstance()->getGuiEnvironment()->addScrollBar(
        false,
        irr::core::recti(
          AbsoluteClippingRect.LowerRightCorner.X - m_iLineHeight,
          AbsoluteClippingRect.UpperLeftCorner .Y,
          AbsoluteClippingRect.LowerRightCorner.X,
          AbsoluteClippingRect.LowerRightCorner.Y
        ),
        this
      );
    }

    void CGuiLogDisplay::setFont(irr::gui::IGUIFont* a_pFont) {
      m_pFont = a_pFont;
      m_iLineHeight = 4 * m_pFont->getDimension(L"TesTexT").Height / 3;
    }

    void CGuiLogDisplay::draw() {
      if (IsVisible) {
        irr::core::recti l_cText = AbsoluteClippingRect;
        l_cText.UpperLeftCorner  += irr::core::position2di(                m_iLineHeight / 4, m_iLineHeight / 4);
        l_cText.LowerRightCorner -= irr::core::position2di(m_iLineHeight + m_iLineHeight / 4, m_iLineHeight / 4);

        m_pDrv->draw2DRectangle(irr::video::SColor(224, 192, 192, 192), AbsoluteClippingRect);
        m_pDrv->draw2DRectangleOutline(AbsoluteClippingRect, irr::video::SColor(0xFF, 0, 0, 0));

        irr::s32 l_iPosY = l_cText.UpperLeftCorner.Y - (m_pScroll != nullptr ? m_pScroll->getPos() : 0);

        irr::core::recti l_cRect = irr::core::recti(
          irr::core::vector2di(l_cText.UpperLeftCorner .X, l_iPosY),
          irr::core::vector2di(l_cText.LowerRightCorner.X, l_iPosY + m_iLineHeight)
        );

        irr::u32 l_iCount = 0;

        for (std::vector<std::tuple<irr::ELOG_LEVEL, std::wstring>>::iterator it = m_vLogLines.begin(); it != m_vLogLines.end(); it++) {
          if (l_cRect.LowerRightCorner.Y >= l_cText.UpperLeftCorner.Y && l_cRect.UpperLeftCorner.Y <= l_cText.LowerRightCorner.Y) {
            m_pFont->draw(
              (L"[" + std::to_wstring(l_iCount) + L"] " + std::get<1>(*it)).c_str(),
              l_cRect, std::get<0>(*it) == irr::ELL_ERROR ? irr::video::SColor(0xFF, 0xFF, 128, 128) : std::get<0>(*it) == irr::ELL_WARNING ? irr::video::SColor(0xFF, 0xFF, 0xFF, 128) : irr::video::SColor(0xFF, 0, 0, 0),
              false, true, &l_cText
            );
          }

          l_cRect.UpperLeftCorner .Y += m_iLineHeight;
          l_cRect.LowerRightCorner.Y += m_iLineHeight;

          l_iCount++;
        }

        for (irr::core::list<irr::gui::IGUIElement*>::Iterator it = Children.begin(); it != Children.end(); it++)
          (*it)->draw();
      }
    }

    CGuiLogDisplay::~CGuiLogDisplay() {

    }

    /**
    * Add a line to the log
    * @param a_eLevel the log level
    * @param a_sLogLine the line to add
    */
    void CGuiLogDisplay::addLogLine(irr::ELOG_LEVEL a_eLevel, const std::wstring& a_sLogLine) {
      m_vLogLines.push_back(std::make_tuple(a_eLevel, a_sLogLine));

      if (m_pScroll != nullptr) {
        irr::s32 l_iTotalHeight = m_iLineHeight * ((irr::s32)m_vLogLines.size() + 1) - AbsoluteClippingRect.getHeight();
        m_pScroll->setMin(0);
        m_pScroll->setMax(l_iTotalHeight);
        m_pScroll->setPos(l_iTotalHeight);
      }
    }

    void CGuiLogDisplay::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      IGUIElement::serializeAttributes(a_pOut, a_pOptions);
    }

    void CGuiLogDisplay::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
    }
  }
}