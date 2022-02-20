// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

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
        bool                      m_bChecked,
                                  m_bMouseL,
                                  m_bHover,
                                  m_bInside;
        int                       m_iBorder;
        irr::core::position2di    m_cMousePos;

        irr::video::SColor m_cBackgroundColor,  /**< The background color */
                           m_cHoverColor,       /**< The color when the mouse hoveres the element */
                           m_cClickColor,       /**< The color when the mouse button is clicked on the element */
                           m_cDisabledColor;    /**< Background color if the item is disabled */


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
