// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace scenenodes {
    class CMyBillboard;       /**< Forward declaration of the modified billboard */
    class CMyBillboardText;   /**< Forward declaration of the modified text billboard */
  }

  namespace gui {
    /**
    * @class STextElement
    * @author Christian Keimel
    * This structure collects all information
    * about the textual elements of the HUD
    */
    struct STextElement {
      irr::core::recti          m_cThisRect;    /**< Screen rect of the element */
      irr::core::recti          m_cClip;        /**< The clipping rectangle */
      std::wstring              m_sText;        /**< Text of the element */
      irr::gui::IGUIFont       *m_pFont;        /**< Font of the text element */
      irr::video::SColor        m_cBackground;  /**< The background color*/
      irr::video::SColor        m_cTextColor;   /**< The text color */
      irr::video::IVideoDriver *m_pDrv;         /**< The video driver */
      irr::gui::EGUI_ALIGNMENT  m_eAlignV;      /**< Vertical alignment*/
      irr::gui::EGUI_ALIGNMENT  m_eAlignH;      /**< Horizontal alignment */
      bool                      m_bVisible;     /**< Is this text element visible? */

      STextElement(const irr::core::recti &a_cRect, const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont, const irr::video::SColor &a_cBackground, const irr::video::SColor &a_cTextColor, const irr::core::recti &a_cClip, irr::video::IVideoDriver *a_pDrv);

      STextElement();

      void render();
      void setPosition(const irr::core::position2di &a_cPos);
    };

    /**
    * @class SHighLight
    * @author Christian Keimel
    * This data struct holds all data
    * necessary for highlighting the
    * players that are also shown in
    * the ranking display
    */
    struct SHighLight {
      int  m_iMarbleId;     /**< ID of the highlighted marble */
      int  m_iPosition;     /**< Position of the marble */
      bool m_bVisible;      /**< Visibility flag */
      bool m_bFinished;     /**< Has this player finished? */
      bool m_bViewport;     /**< Visible in the current viewport? */

      scenenodes::CMyBillboard     *m_pArrow;    /**< The arrow billboard node */

      SHighLight();
    };

    /**
    * An enum with all text elements used
    */
    enum class enTextElements {
      Name,       /**< The player's name */
      LapHead,    /**< Header of the lap distplay */
      Lap,        /**< The lap display */
      PosHead,    /**< Header of the position display */
      Pos,        /**< The position display */
      Ahead,      /**< The player ahead */
      Behind,     /**< The player behind */
      TimeAhead,  /**< The time difference to the marble in from */
      TimeBehind  /**< The time difference to the marble behind */
    };
  }
}
