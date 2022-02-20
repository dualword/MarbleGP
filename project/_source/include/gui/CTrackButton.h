// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <gui/CButtonRenderer.h>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  namespace gui {
    const int g_TrackButtonId = MAKE_IRR_ID('d', 'g', 't', 'b');
    const irr::c8 g_TrackButtonName[] = "TrackButton";

    /**
    * @class CTrackButton
    * @author Christian Keimel
    * A special button for the track selection
    */
    class CTrackButton : public irr::gui::IGUIElement, public CButtonRenderer {
      private:
        irr::gui::IGUIEnvironment* m_pGui;
        irr::video::IVideoDriver* m_pDrv;
        irr::gui::IGUIFont* m_pFont;

        std::string m_sImage, /**< Optional image to be rendered */
                    m_sTrack; /**< The track folder */

        int m_iSpaceWidth,  /**< The width of the "space" character with the current font */
            m_iLineHeight,  /**< The height of a line */
            m_iTotalHeight; /**< The total height of the text lines */

        irr::video::SColor m_cOverlay,  /**< The overlay color if the button is hovered */
                           m_cClicked;  /**< The overlay color if the button is clicked */

        bool m_bHovered,  /**< Is the element hovered? */
             m_bLDown;    /**< Is the left mouse button pressed? */

        std::wstring m_sMyText; /**< This member stores the text of the button to detect a text change */

        std::vector<std::wstring> m_vLines; /**< The text lines */

        irr::video::ITexture* m_pImage;           /*< The optional image to be rendered */

        void calculateLines(const std::wstring& a_sText);

      public:
        CTrackButton(irr::gui::IGUIElement* a_pParent);
        virtual ~CTrackButton();

        virtual void draw();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        void setOverrideFont(irr::gui::IGUIFont* a_pFont);
        void setTexture(const std::string &a_sTexture);
        void setTrack(const std::string &a_sTrack);

        const std::string &getTrack();

        virtual bool OnEvent(const irr::SEvent& a_cEvent);
    };
  }
}
