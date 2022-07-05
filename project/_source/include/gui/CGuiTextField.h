// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <gui/CButtonRenderer.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace gui {
    const int g_TextFieldId = MAKE_IRR_ID('d', 'g', 't', 'f');
    const irr::c8 g_TextFieldName[] = "TextField";

    /**
    * @class CGuiTextField
    * @author Christian Keimel
    * This is a multiline scrollable text field
    */
    class CGuiTextField : public irr::gui::IGUIElement {
      private:
        enum enTextItem {
          Text,
          Space,
          Newline
        };

        struct STextItem {
          enTextItem           m_eType;
          std::wstring         m_sText;
          irr::core::vector2di m_cSize;

          STextItem(enTextItem a_eType, const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont);
          STextItem(const STextItem &a_cOther);
        };

        irr::video::IVideoDriver *m_pDrv;
        irr::gui::IGUIScrollBar  *m_pScroll;
        irr::gui::IGUIFont       *m_pFont;
        irr::core::recti          m_cInner;
        irr::core::position2di    m_cMouse;
        bool                      m_bMouse;
        int                       m_iScroll;
        int                       m_iMax;

        std::wstring m_sFullText;

        std::vector<STextItem> m_vItems;

      public:
        CGuiTextField(irr::gui::IGUIElement* a_pParent);
        virtual ~CGuiTextField();

        virtual void draw() override;

        /**
        * Set the text of the element
        * @param a_sText the new text of the element
        */
        void setNewText(const std::string &a_sText);

        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;
    };
  } // namespace gui
}   // namespace dustbin
