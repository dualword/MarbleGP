// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <string>

namespace dustbin {
  namespace gui {
    const int g_MenuButtonId = MAKE_IRR_ID('d', 'g', 'm', 'b');
    const irr::c8 g_MenuButtonName[] = "MenuButton";

    /**
    * @class CMenuButton
    * @author Christian Keimel
    * This is the button used most of the time in MarbleGP
    */
    class CMenuButton : public irr::gui::IGUIElement {
      private:
        irr::gui::IGUIEnvironment* m_pGui;
        irr::video::IVideoDriver * m_pDrv;
        irr::gui::IGUIFont       * m_pFont;

        std::string m_sImage; /**< Optional image to be rendered */

        bool m_bHovered,  /**< Is the element hovered? */
             m_bLDown;    /**< Is the left mouse button pressed? */

        irr::video::ITexture* m_pBackgrounds[3],  /*< The background textures (0 == off, 1 == hover, 2 == click) */
                            * m_pImage;           /*< The optional image to be rendered */

      public:
        CMenuButton(irr::gui::IGUIElement* a_pParent);
        virtual ~CMenuButton();

        virtual void draw();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        void setOverrideFont(irr::gui::IGUIFont* a_pFont);

        virtual bool OnEvent(const irr::SEvent& a_cEvent);
		};
  }
}