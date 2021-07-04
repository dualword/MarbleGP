// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <gui/CButtonRenderer.h>

namespace dustbin {
  namespace gui {
    const int g_MenuBackgroundId = MAKE_IRR_ID('d', 'g', 'b', 'a');
    const irr::c8 g_MenuBackgroundName[] = "MenuBackground";

    class CMenuBackground : public irr::gui::IGUIElement, public CButtonRenderer {
      private:
        irr::video::IVideoDriver* m_pDrv;
        irr::video::SColor        m_cOverrideColor;
        bool                      m_bOverrideColor;

      public:
        CMenuBackground(irr::gui::IGUIElement *a_pParent);
        virtual ~CMenuBackground();

        virtual void draw();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  }
}
