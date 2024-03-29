// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <gui/CButtonRenderer.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace gui {
    const int g_SelectorId = MAKE_IRR_ID('d', 'g', 's', 'c');
    const irr::c8 g_SelectorName[] = "Selector";

    /**
    * @class CSelector
    * @author Christian Keimel
    * This is a replacement for the ComboBox. It holds a list of options and two buttons (left / right)
    * to allow iterating through the list
    */
    class CSelector : public irr::gui::IGUIElement {
      private:
        irr::video::IVideoDriver* m_pDrv;

        bool m_bCenterH;  /**< Center text horizontally? */
        bool m_bCenterV;  /**< Center text vertically? */
        bool m_bHoverL;   /**< Left arrow hovered? */
        bool m_bHoverR;   /**< Right arrow hovered? */
        bool m_bHoverT;   /**< Text rect hovered */
        bool m_bLeftBtn;  /**< Left mouse button pressed? */
        bool m_bSelected; /**< Selected by the menu controller */
        bool m_bInside;   /**< Is the mouse pointer inside the element? */
        bool m_bOnText;   /**< Was the mouse pressed in the text area? */

        irr::video::SColor m_cBackgroundColor;  /**< The background color */
        irr::video::SColor m_cHoverColor;       /**< The color when the mouse hoveres the element */
        irr::video::SColor m_cClickColor;       /**< The color when the mouse button is clicked on the element */
        irr::video::SColor m_cDisabledColor;    /**< Background color when the item is disabled */

        irr::core::position2di m_cMousePos;   /**< The mouse position */
        irr::core::position2di m_cTextDown;   /**< Mouse down position in the central text area */

        std::vector<std::wstring> m_vItems;   /**< A vector with the items of the selector */

        irr::core::recti m_cBoxL; /**< The left box (reduce index by 1) */
        irr::core::recti m_cBoxR; /**< The right box (increase index by 1) */
        irr::core::recti m_cText; /**< The text outbox */

        irr::gui::IGUIFont* m_pFont;  /**< The font to use for drawing the selected item */

        int m_iSelected;  /**< Index of the selected item */

        /**
        * Pass a "scrollbar changed" event on to the parent
        */
        void distributeEvent();

      public:
        CSelector(irr::gui::IGUIElement* a_pParent);
        virtual ~CSelector();

        virtual void draw();

        /**
        * Clear the items of the selector
        */
        void clear();

        /**
        * Add an item to the selector
        * @param a_sItem the new item
        */
        void addItem(const std::wstring& a_sItem);

        /**
        * Get an item by it's index
        * @param a_iIndex the index of the requested item
        * @return the item, empty string if the index is out of bounds
        */
        std::wstring getItem(int a_iIndex);

        /**
        * Get the index of the selected item
        * @return the index of the selected item
        */
        int getSelected();

        /**
        * Get the number of items in the selector
        * @return the number of items in the selector
        */
        unsigned getItemCount();

        /**
        * Set the selected item
        * @param a_iIndex the new index
        */
        void setSelected(int a_iIndex);

        /**
        * Get the selected Item
        * @return the selected Item
        */
        std::wstring getSelectedItem();

        /**
        * Set the selected item
        * @param a_sItem the item to select
        */
        void setSelectedItem(const std::wstring &a_sItem);

        virtual irr::gui::EGUI_ELEMENT_TYPE getType();

        virtual bool OnEvent(const irr::SEvent& a_cEvent);

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions);
    };
  } // namespace gui
}   // namespace dustbin
