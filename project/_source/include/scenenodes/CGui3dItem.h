// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CGui3dSceneNodes.h>

namespace dustbin {
  namespace scenenodes {
    /**
     * @class CGui3dItem
     * @author Christian Keimel
     * @brief This class adds event handling functionality to CGui3dItemBase
     * @see CGui3dItemBase
     */
    class CGui3dItem : public CGui3dItemBase {
      protected:
        bool m_bHovered;  /**< Is the element hovered? */

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
         * Update the text of the 3d UI item
         * @param a_cBackgroundColor the background color to use
         * @param a_cTextColor the text color to use
         */
        virtual void updateRttText(const irr::video::SColor &a_cBackgroundColor, const irr::video::SColor &a_cTextColor);
    };
  }
}