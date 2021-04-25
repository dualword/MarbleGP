// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CMarbleGPSceneNodes.h>
#include <irrlicht.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace scenenodes {
    const int g_s3dGuiButtonID = MAKE_IRR_ID('3', 'g', 'u', 'i');

    /**
     * @class CGui3dElement
     * @author Christian Keimel
     * @brief This is the base class for all 3d UI element data objects
     */
    class CGui3dElement {
      public:
        enum class enType {
          enDecoration,
          enLabel,
          enButton
        };

      private:
        enType m_eType; /**< The type of the derived class */

        irr::scene::ISceneNode *m_pNode;  /**< The scene node this element represents */

      public:
        /**
         * The constructor
         * @param a_eType the type of 3d UI element
         * @param a_pNode the node this UI element represents
         */
        CGui3dElement(enType a_eType, irr::scene::ISceneNode *a_pNode);

        virtual ~CGui3dElement() { }

        /**
         * Get the UI element type
         * @return the UI element type
         */
        enType getType();

        /**
         * Get the node assigned to this 3d UI element
         * @return the node assigned to this 3d UI element
         */
        irr::scene::ISceneNode *getNode();

        /**
         * Get the type name of this element
         * @return the type name of this element
         */
        virtual std::string getTypeName() = 0;

        /**
         * Serialize all attributes of the element
         * @param a_pOut the serialization object
         * @param a_sSuffix the suffix for the identifiers in the IAttributes object
         */
        virtual void serialize(irr::io::IAttributes *a_pOut, const std::string &a_sSuffix) const = 0;

        /**
        * Deserialize all attributes of the element
        * @param a_pIn the serialization object
        * @param a_sSuffix the suffix for the identifiers in the IAttributes object
        */
        virtual void deserialize(irr::io::IAttributes *a_pIn, const std::string &a_sSuffix) = 0;
    };

    /**
     * @class CGui3dDecoration
     * @authod Christian Keimel
     * @brief the data for a 3d decoration element, i.e. a 3d object that does not have any functionality but is just there
     */
    class CGui3dDecoration : public CGui3dElement {
      public:
        CGui3dDecoration(irr::scene::ISceneNode *a_pNode);
        virtual ~CGui3dDecoration() { }

        virtual void serialize(irr::io::IAttributes *a_pOut, const std::string &a_sSuffix) const;
        virtual void deserialize(irr::io::IAttributes *a_pIn, const std::string &a_sSuffix);

        virtual std::string getTypeName();
    };

    /**
     * @class C3dGuiButton
     * @author Christian Keimel
     * @brief This is the 3d GUI root class that converts it's children to a 3d UI
     */
    class CGui3dRoot : public irr::scene::ISceneNode, public CSceneNodeBase {
      private:

        irr::core::aabbox3df       m_cBox;          /**< The bounding box of the button */
        irr::video::SMaterial      m_cMaterial[2];  /**< The materials to render the button with */
        irr::scene::ISceneManager *m_pSmgr;         /**< The scene manager of the node */
        irr::gui::IGUIEnvironment *m_pGui;          /**< The active Irrlicht GUI environment */
        irr::video::IVideoDriver  *m_pDrv;          /**< The Irrlicht video driver */
        irr::video::ITexture      *m_pRtt;          /**< The render target texture for the button caption */
        irr::gui::IGUIFont        *m_pFont;         /**< The font to render the button text with */
        irr::core::dimension2di    m_cRttDim;       /**< Size of the render target */
        irr::scene::IMeshBuffer   *m_pBuffers[2];   /**< The mesh buffers */
        irr::core::vector3df       m_cDimension;    /**< The size of the button */

        static std::string m_sNodeTypeName;  /**< The node type name */
        
        std::map<int, CGui3dElement *> m_mChildElements; /**< All the data to handle the child elements */

        /**
         * Get a 3d element assigned to a node
         * @return the 3d element for the node, nullptr if the element was not found
         */
        CGui3dElement *getElement(irr::scene::ISceneNode *a_pNode) const;

        /**
         * Get the next ID in the map of elements
         * @return the next ID
         */
        int getNextId();

        void renderRTT();

      public:
        CGui3dRoot(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dRoot();

        virtual void OnRegisterSceneNode();
        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32> &getBoundingBox() const;
        virtual irr::u32 getMaterialCount();
        virtual irr::video::SMaterial &getMaterial(irr::u32 i);
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        irr::scene::ISceneNode *clone(irr::scene::ISceneNode *a_pNewParent, irr::scene::ISceneManager *a_pNewSmgr);

        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        static const std::string &getNodeTypeName();
        static const irr::scene::ESCENE_NODE_TYPE getNodeType();

        virtual void addChild (ISceneNode *a_pChild);
    };
  }
}