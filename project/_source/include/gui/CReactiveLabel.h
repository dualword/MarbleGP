// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace gui {
    const int g_ReactiveLabelId = MAKE_IRR_ID('d', 'g', 'r', 'l');
    const irr::c8 g_ReactiveLabelName[] = "ReactiveLabel";

    /**
    * @class CReactiveLabel
    * @author Christian Keimel
    * A label (similar to the Irrlicht static text) that reacts to mouse input
    */
    class CReactiveLabel : public irr::gui::IGUIElement {
      private:
        irr::gui::IGUIEnvironment* m_pGui;
        irr::video::IVideoDriver * m_pDrv;
        irr::gui::IGUIFont       * m_pFont;

        bool m_bCenterH;    /**< Center the text horizontally? */
        bool m_bCenterV;    /**< Center the text vertically? */
        bool m_bHovered;    /**< Is the element hovered? */
        bool m_bLDown;      /**< Is the left mouse button pressed? */
        bool m_bBorder;     /**< Draw border? */
        bool m_bBackground; /**< Draw background when not hovered or clicked? */

        irr::video::SColor m_cBorder;     /**< The border color */
        irr::video::SColor m_cBackground; /**< The background color */
        irr::video::SColor m_cHovered;    /**< The background color when the label is hovered */
        irr::video::SColor m_cClicked;    /**< The background color when the label is clicked */
        irr::video::SColor m_cText;       /**< The text color */

      public:
        CReactiveLabel(irr::gui::IGUIElement* a_pParent);
        virtual ~CReactiveLabel();

        virtual void draw();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        void setOverrideFont(irr::gui::IGUIFont* a_pFont);

        const irr::video::SColor &getBackgroundColor();

        virtual bool OnEvent(const irr::SEvent& a_cEvent);
    };
  }
}
