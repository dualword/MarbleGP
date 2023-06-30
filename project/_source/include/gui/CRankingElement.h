// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CButtonRenderer.h>
#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace gui {
    /**
    * @class CRankingElement
    * @author Christian Keimel
    * This GUI element renders the final standings
    * in the game HUD
    */
    class CRankingElement : public gui::CButtonRenderer {
      private:
        int          m_iDeficit;    /**< The deficit on the leader */
        std::wstring m_sName;       /**< The player name of the position */
        std::wstring m_sPosition;   /**< The position this element is for */

        irr::core::recti m_cPosition;   /**< Rendering rect for the position */
        irr::core::recti m_cName;       /**< Rendering rect for the name */
        irr::core::recti m_cDeficit;    /**< Rendering rect for the deficit */
        irr::core::recti m_cRect;       /**< The rendering rectangle */
        irr::core::recti m_cBot;        /**< The target rect of the bot icon */
        irr::core::recti m_cBotSrc;     /**< The source rect of the bot icon */

        irr::video::SColor m_cBackground;   /**< The background color */
        irr::video::SColor m_cOriginal;     /**< The original background color */
        irr::video::SColor m_cTextColor;    /**< The text color */
        irr::video::SColor m_cBotColor;     /**< The color to fade out the bot icon */

        irr::gui::IGUIFont *m_pFont;  /**< The font to use */

        bool m_bWithdrawn;    /**< did the player withdraw from the race (true) or finish normally (false)? */
        bool m_bVisible;      /**< Visibility flag */

        irr::video::ITexture            *m_pBotClass;    /**< Texture of the bot class */

        irr::video::SColor m_cNumberBack;   /**< The background color of the number icon */
        irr::video::SColor m_cNumberText;   /**< The text color of the number icon */
        irr::video::SColor m_cNumberFrame;  /**< The border color of the number icon */

        irr::core::recti m_cNumberRect;     /**< The rectangle for the number icon */

        std::wstring m_sNumber;     /**< The number to show in the number icon */

      public:
        CRankingElement(int a_iPosition, const irr::core::recti &a_cRect, const irr::video::SColor &a_cBackground, irr::gui::IGUIFont *a_pFont, irr::gui::IGUIElement *a_pParent, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CRankingElement();

        /**
        * Update the data
        * @param a_sName name of the player
        * @param a_iDeficit deficit to the leader
        * @param a_bWithdrawn did the player withdraw from the race (true) or finish normally (false)?
        * @param a_cBack the background color of the number icon
        * @param a_cNumber the number color of the number icon
        * @param a_cFrame the frame color of the number icon
        * @param a_sNumber the starting number for the number icon
        */
        void setData(const std::wstring &a_sName, int a_iDeficit, bool a_bWithdrawn, const irr::video::SColor &a_cBack, const irr::video::SColor &a_cNumber, const irr::video::SColor &a_cFrame, const std::wstring &a_sNumber);

        void draw();

        /**
        * Set the alpha value
        * @param a_fAlpha the alpha value
        */
        void setAlpha(irr::f32 a_fAlpha);

        /**
        * Set the item visible or invisible
        * @param a_bVisible visibility flag
        */
        void setVisible(bool a_bVisible);
    };
  }
}
