// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CMarbleGPSceneNodes.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace scenenodes {
    const int g_i3dGuiRootID    = MAKE_IRR_ID('3', 'g', 'u', 'i');  /**< The type id of the 3d UI root node */
    const int g_i3dGuiItemID = MAKE_IRR_ID('3', 'e', 'l', 'm');  /**< The type id of the 3d UI element node */

    /**
     * @class CGui3dItem
     * @author Christian Keimel
     * @brief This node turns it's parent (which must be a mesh scene node) into a 3d UI element
     */
    class CGui3dItem : public irr::scene::ISceneNode, public CSceneNodeBase {
      public:
        enum class enGui3dType {
          None = 0,
          Decoration = 1,
          Label = 2,
          Button = 3,
          IconButton = 4
        };

        enum class enGui3dAlign {
          Left = 0,
          Center = 1,
          Right = 2
        };

      private:
        irr::core::aabbox3df       m_cBox;          /**< The bounding  */
        irr::video::SMaterial      m_cMaterial;     /**< The material  */
        irr::scene::ISceneManager *m_pSmgr;         /**< The scene manager */
        irr::video::ITexture      *m_pRttTexture;   /**< The Render to Target Texture */
        irr::gui::IGUIFont        *m_pFont;         /**< The font to use for rendering */
        irr::core::rect<irr::s32>  m_cRect;         /**< The rectangle to draw text to */
        irr::video::SColor         m_cBackground,   /**< The background color */
                                   m_cHoverColor,   /**< The "mouse over" color */
                                   m_cTextColor;    /**< The text color */

        bool m_bHovered;  /**< Is the cursor currently hovering this item? */

        int m_iRttTexture;  /**< The texture number of the parent that will be modified */

        std::wstring m_sText;   /**< The text of the UI element */

        static std::string m_sNodeTypeName;  /**< The node type name */

        std::map<enGui3dType, std::vector<std::string> > m_mSerializerMap;  /**< This map shows which UI type serializes which properties */

        irr::core::dimension2di m_cRttSize; /**< The size of the render target texture */

        enGui3dType  m_eType;
        enGui3dAlign m_eAlign;

        std::string typeToString(enGui3dType a_eType);
        std::string alignToString(enGui3dAlign a_eAlign);

        enGui3dAlign stringToAlign(const std::string &a_sAlign);
        
        void updateRttText();
        void updateRttTexture();

      public:
        CGui3dItem(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dItem();

        virtual void OnRegisterSceneNode();
        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32> &getBoundingBox() const;
        virtual irr::u32 getMaterialCount();
        virtual irr::video::SMaterial &getMaterial(irr::u32 i);
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode *a_pNewParent, irr::scene::ISceneManager *a_pNewSmgr);

        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        static const std::string &getNodeTypeName();
        static const irr::scene::ESCENE_NODE_TYPE getNodeType();

        void itemEntered(); /**< The cursor has entered the item */
        void itemLeft();    /**< The cursor has left the item */
    };

    /**
     * @class C3dGuiRoot
     * @author Christian Keimel
     * @brief This is the 3d GUI root class that converts it's children to a 3d UI
     */
    class CGui3dRoot : public irr::scene::ISceneNode, public CSceneNodeBase {
      private:
        irr::core::aabbox3df                m_cBox;       /**< The bounding  */
        irr::video::SMaterial               m_cMaterial;  /**< The material  */
        irr::scene::ISceneManager          *m_pSmgr;      /**< The scene manager */
        irr::gui::ICursorControl           *m_pCursor;    /**< The cursor control to handle events */
        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< The scene collision manager necessary to detect hovers and such */
        irr::scene::IMetaTriangleSelector  *m_pSelector;  /**< A meta triangle selector that combines all triangle selectors of the menu */

        CGui3dItem *m_pHover;        /**< The hovered node */

        static std::string m_sNodeTypeName;  /**< The node type name */

        bool m_bLeftDown; /**< Is the left mouse button down? */

        std::map<irr::scene::ISceneNode *, CGui3dItem *> m_mItemScenenodeMap; /**< This map is used to connect scenenodes with the 3d Gui items */


        /**
         * Get all children of type "CGui3dItem"
         * @param a_pParent the parent node to iterate the children
         * @param a_pItems a vector which will be filles
         */
        void getGuiItems(irr::scene::ISceneNode *a_pParent, std::vector<dustbin::scenenodes::CGui3dItem *> &a_vItems);

      public:
        CGui3dRoot(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dRoot();

        virtual void OnRegisterSceneNode();
        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32> &getBoundingBox() const;
        virtual irr::u32 getMaterialCount();
        virtual irr::video::SMaterial &getMaterial(irr::u32 i);
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode *a_pNewParent, irr::scene::ISceneManager *a_pNewSmgr);

        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        static const std::string &getNodeTypeName();
        static const irr::scene::ESCENE_NODE_TYPE getNodeType();

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