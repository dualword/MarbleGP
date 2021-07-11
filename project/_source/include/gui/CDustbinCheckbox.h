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
    const int g_DustbinCheckboxId = MAKE_IRR_ID('d', 'g', 'c', 'b');
    const irr::c8 g_DustbinCheckboxName[] = "DustbinCheckbox";

    /**
    * @class CDustbinCheckbox
    * @author Christian Keimel
    * My personal checkbox implementation
    */
    class CDustbinCheckbox : public irr::gui::IGUIElement {
      private:
        irr::video::IVideoDriver* m_pDrv;
        bool                      m_bChecked;
        int                       m_iBorder;

      public:
        CDustbinCheckbox(irr::gui::IGUIElement* a_pParent);
        virtual ~CDustbinCheckbox();

        virtual void draw();

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        virtual bool OnEvent(const irr::SEvent& a_cEvent);

        void setChecked(bool a_bChecked);
        bool isChecked();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  }
}