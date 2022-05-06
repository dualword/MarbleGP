/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  namespace gui {
    const int g_GuiLogDisplayId = MAKE_IRR_ID('l', 'o', 'g', 'd');
    const irr::c8 g_GuiLogDisplayName[] = "LogDisplay";

    /**
    * @class CGuiLogDisplay
    * @author Christian Keimel
    * This GUI element is used to show the Irrlicht log
    */
    class CGuiLogDisplay : public irr::gui::IGUIElement {
      private:
        std::vector<std::tuple<irr::ELOG_LEVEL, std::wstring>> m_vLogLines;  /**< The lines shown */

        irr::video::IVideoDriver *m_pDrv;   /**< The video driver */
        irr::gui::IGUIFont       *m_pFont;  /**< The font */

        irr::u32 m_iLineHeight;   /**< Height of a line */

        irr::gui::IGUIScrollBar* m_pScroll; /**< The scrollbar to scroll the log */

      public:
        CGuiLogDisplay(irr::gui::IGUIElement* a_pParent);
        virtual ~CGuiLogDisplay();

        virtual void draw();

        void setFont(irr::gui::IGUIFont *a_pFont);

        /**
        * Add a line to the log
        * @param a_eLevel the log level
        * @param a_sLogLine the line to add
        */
        void addLogLine(irr::ELOG_LEVEL a_eLevel, const std::wstring& a_sLogLine);

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  }
}