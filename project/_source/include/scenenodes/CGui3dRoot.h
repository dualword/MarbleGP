// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CGui3dSceneNodes.h>
#include <scenenodes/CGui3dItem.h>

class CLuaScript_dialog;

namespace dustbin {
  namespace state {
    class IState;
  }

  namespace scenenodes {
    /**
     * @class CGui3dRoot
     * @author Christian Keimel
     * @brief The 3d UI root scene node with the functions to handle user input
     * @see CGui3dRootBase
     */
    class CGui3dRoot : public CGui3dRootBase {
      protected:
        CGui3dItem *m_pHover,   /**< The item hovered by the cursor */
                   *m_pClick,   /**< The clicked item. Will remain clicked until the left mouse button is released */
                   *m_pSelect;  /**< The selected item. Is updated when an item is clicked and will remain until another item is selected */

        bool m_bLeftButtonDown; /**< Is the left mouse button down at the moment? */

        irr::gui::ICursorControl *m_pCursor;    /**< The cursor control to handle events */

        irr::scene::IMetaTriangleSelector *m_pSelector;  /**< A meta triangle selector that combines all triangle selectors of the menu */

        std::map<irr::scene::ISceneNode *, CGui3dItemBase *> m_mItemScenenodeMap; /**< This map is used to connect scenenodes with the 3d Gui items */

        CLuaScript_dialog *m_pLuaScript;  /**< The LUA script that handles user input */

        state::IState *m_pState; /**< The active state */

        /**
         * Get all children which are CGui3dItem instances
         * @param a_pParent this node is checked for the type and it's children are iterated to find more
         * @param a_vItems this vectr is filled with the children found
         * @see CGui3dItem
         */
        void getGuiItems(irr::scene::ISceneNode* a_pParent, std::vector<dustbin::scenenodes::CGui3dItem *>& a_vItems);

    public:
        CGui3dRoot(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dRoot();

        /**
         * Initialize the 3d GUI members for handling events
         * @param a_pCursor the Irrlicht cursor control
         */
        void initGui3d();

        /**
         * Call this function to react to user input
         */
        void step();

        /**
         * Set the Irrlicht Cursor Control instance
         * @param a_pCursor the Irrlicht Cursor Control instance
         */
        void setCursorControl(irr::gui::ICursorControl *a_pCursor);
    };
  }
}
