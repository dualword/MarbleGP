// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <gui/CButtonRenderer.h>

namespace dustbin {
  namespace gui {
    const int g_MenuBackgroundId = MAKE_IRR_ID('d', 'g', 'b', 'a');
    const irr::c8 g_MenuBackgroundName[] = "MenuBackground";

    class CMenuBackground : public irr::gui::IGUIElement, public CButtonRenderer {
      protected:
        irr::video::IVideoDriver* m_pDrv;

      private:
        irr::video::SColor        m_cOverrideColor;
        bool                      m_bOverrideColor;

      public:
        CMenuBackground(irr::gui::IGUIElement *a_pParent, irr::gui::EGUI_ELEMENT_TYPE a_eType = (irr::gui::EGUI_ELEMENT_TYPE)g_MenuBackgroundId);
        virtual ~CMenuBackground();

        virtual void draw();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  }
}
