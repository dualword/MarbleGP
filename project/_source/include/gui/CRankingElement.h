// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CButtonRenderer.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace gui {
    /**
    * @class CRankingElement
    * @author Christian Keimel
    * This GUI element renders the final standings
    * in the game HUD
    */
    class CRankingElement : public irr::gui::IGUIElement, public gui::CButtonRenderer {
      private:
        int          m_iDeficit;    /**< The deficit on the leader */
        std::wstring m_sName;       /**< The player name of the position */
        std::wstring m_sPosition;   /**< The position this element is for */

        irr::core::recti m_cPosition;   /**< Rendering rect for the position */
        irr::core::recti m_cName;       /**< Rendering rect for the name */
        irr::core::recti m_cDeficit;    /**< Rendering rect for the deficit */

        irr::video::SColor m_cBackground;   /**< The background color */

        irr::gui::IGUIFont *m_pFont;  /**< The font to use */

      public:
        CRankingElement(int a_iPosition, const irr::core::recti &a_cRect, const irr::video::SColor &a_cBackground, irr::gui::IGUIFont *a_pFont, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CRankingElement();

        /**
        * Update the data
        * @param a_sName name of the player
        * @param a_iDeficit deficit to the leader
        */
        void setData(const std::wstring &a_sName, int a_iDeficit);

        virtual void draw();
    };
  }
}
