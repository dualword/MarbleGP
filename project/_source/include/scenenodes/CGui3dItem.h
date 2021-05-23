// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CGui3dSceneNodes.h>

namespace dustbin {
  namespace state {
    class IState;
  }

  namespace scenenodes {
    /**
     * @class CGui3dItem
     * @author Christian Keimel
     * @brief This class adds event handling functionality to CGui3dItemBase
     * @see CGui3dItemBase
     */
    class CGui3dItem : public CGui3dItemBase {
      protected:
        state::IState *m_pState;  /**< The current avtive state. Event messages are posted to this state */

        bool m_bHovered,      /**< Is the element hovered? */
             m_bSelected,     /**< Is the element selected? */
             m_bDragging,     /**< Is the element dragged? */
             m_bInitialized;  /**< Is the drag data initialized? */

        irr::core::plane3df  m_cPlane;      /**< The plane for dragging */
        irr::core::line3df   m_cLine;       /**< The dragging line */
        irr::core::vector3df m_cStart,      /**< The dragging start */
                             m_cDirection,  /**< The dragging direction */
                             m_cMin,        /**< The minimum position of the slider */
                             m_cMax;        /**< The maximum position of the slide */

        irr::f32 m_fSliderOffset; /**< Offset for slider if the item was not clicked in the center */

      public:
        CGui3dItem(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dItem();

        /**
         * This method is called when the mouse cursor starts hovering the item
         */
        void itemEntered();

        /**
         * A callback to notify the element that the mouse no longer hovers the item
         */
        void itemLeft();

        /**
         * A callback to notify the element that the left mouse button is down while it is hovered
         */
        void itemLeftButtonDown();

        /**
         * This callback is called when the left mouse button was released while the element is hovered
         */
        void itemLeftButtonUp();

        /**
         * This callback is used to notify the element that is was clicked
         */
        void itemClicked();

        /**
        * Change the text of the item
        * @param a_sText the new text
        */
        void setText(const std::wstring &a_sText);

        /**
        * Get the text of the item
        * @return the text of the item
        */
        const std::wstring getText() const;

        /**
        * Change the background color of the item
        * @param a_cColor the new background color
        */
        void setBackgroundColor(const irr::video::SColor &a_cColor);

        /**
         * Update the text of the 3d UI item
         * @param a_cBackgroundColor the background color to use
         * @param a_cTextColor the text color to use
         */
        virtual void updateRttText(const irr::video::SColor &a_cBackgroundColor, const irr::video::SColor &a_cTextColor);

        /**
        * This method defines whether or not an item can be dragged
        * @return true if item can be dragged, false otherwise
        */
        bool canBeDragged();

        /**
        * Initialize internal data for dragging
        */
        void startDragging(const irr::core::line3df &a_cRay);

        /**
        * This function is called when dragging is ended
        */
        void stopDragging();

        /**
        * While an element (which can be dragged) is dragged this method is called on every step
        * @param a_cRay the ray calculated by the collision manager from the mouse cursor position
        */
        void drag(const irr::core::line3df &a_cRay);

        /**
        * Set the value of this element (necessary for the Slider)
        * @param a_fValue the new value
        */
        void setValue(float a_fValue);

        /**
        * Get the value of this element (for the Slider)
        * @return the value of this element
        */
        float getValue();

        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        virtual void OnRegisterSceneNode();

        virtual void render();
    };
  }
}