/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#pragma once

#include <irrlicht.h>
#include <vector>

#include <shaders/CDustbinShaderDefines.h>

namespace dustbin {
  namespace scenenodes {
    class CDustbinLight;    /**< Forward declaration of the light scenenode */
  }
  namespace shaders {
    class CDustbinShaderCallback;   /**< Forward declaration of the callback class */

    /**
    * @class CDustbinShaders
    * @author Christian Keimel
    * This class handles everything about my shader
    * @see SShadowNode::m_vMaterials
    */
    class CDustbinShaders {
      private:
        /**
        * @class SShadowNodeMaterial
        * @author Christian Keimel
        * This data structure is used to store which materials a node uses
        * for normal and shadow map rendering
        */
        typedef struct SShadowNodeMaterial {
          irr::u32 m_iMaterial;   /**< The material index */

          irr::video::E_MATERIAL_TYPE m_eShadow1;   /**< The material used for shadow rendering */
          irr::video::E_MATERIAL_TYPE m_eShadow2;   /**< The material used for transparent shadow rendering */
          irr::video::E_MATERIAL_TYPE m_eShadow3;   /**< The material used for transparent color rendering */
          irr::video::E_MATERIAL_TYPE m_eRenderM;   /**< The material used for scene rendering */
          irr::video::E_MATERIAL_TYPE m_eOriginal;  /**< The original Irrlicht material */

          /**
          * The constructor
          * @param a_iMaterial the material index
          * @param a_eShadow1 the material material for the solid shadow pass
          * @param a_eShadow2 the material material for the transparent shadow pass
          * @param a_eShadow3 the material material for the transparent color pass
          * @param a_eRenderM the Dustbin Shader material used for rendering
          * @param a_eOriginal the original material
          */
          SShadowNodeMaterial(
            irr::u32 a_iMaterial, 
            irr::video::E_MATERIAL_TYPE a_eShadow1, 
            irr::video::E_MATERIAL_TYPE a_eShadow2, 
            irr::video::E_MATERIAL_TYPE a_eShadow3, 
            irr::video::E_MATERIAL_TYPE a_eRenderM,
            irr::video::E_MATERIAL_TYPE a_eOriginal
          );

          /**
          * Copy constructor
          * @param a_cOther the data struct to copy
          */
          SShadowNodeMaterial(const SShadowNodeMaterial &a_cOther);
        }
        SShadowNodeMaterial;

        /**
        * @class SShadowNode
        * @author Christian Keimel
        * This class stores data about a scene node and it's materials
        * for rendering the shadow map and the scene
        */
        typedef struct SShadowNode {
          irr::scene::ISceneNode *m_pNode;    /**< The scene node */

          bool m_bVisible;      /**< Is this node visible in the scene? */

          std::vector<SShadowNodeMaterial> m_vMaterials;    /**< The list of this node's materials */

          /**
          * The constructor
          * @param a_pNode the scene node
          * @param a_bVisible is this node visible in the scene?
          */
          SShadowNode(irr::scene::ISceneNode *a_pNode, bool a_bVisible);

          /**
          * Copy constructor
          * @param a_cOther the data struct to copy
          */
          SShadowNode(const SShadowNode &a_cOther);
        }
        SShadowNode;

        irr::IrrlichtDevice          *m_pDevice;        /**< The Irrlicht device */
        irr::video::IVideoDriver     *m_pDrv;           /**< The Irrlicht video driver */
        irr::scene::ISceneManager    *m_pSmgr;          /**< The Irrlicht scene manager */
        irr::scene::ICameraSceneNode *m_pLightCamera;   /**< The camera representing the light source */
        irr::scene::ICameraSceneNode *m_pActive;        /**< The active camera of the scene. Stored in "startShadowMaps" and restored in "endShadowMaps" */

        scenenodes::CDustbinLight *m_pDataNode;   /**< The scene node which stores the light data */

        irr::video::ITexture *m_pRttShadow1[(int)enMaterialType::Count];    /**< An array of textures for the various shadow map sizes */
        irr::video::ITexture *m_pRttShadow2[(int)enMaterialType::Count];    /**< An array of textures for the various shadow map sizes (transparent) */
        irr::video::ITexture *m_pRttShadow3[(int)enMaterialType::Count];    /**< An array of textures for the various shadow map sizes (transparent color) */
        irr::video::ITexture *m_pRttShadow4[(int)enMaterialType::Count];    /**< An array of textures for the various shadow map sizes (marble shadow) */

        CDustbinShaderCallback *m_pCallback;    /**< The shader callback */

        enShadowMode   m_eMode;   /**< The shadow rendering mode */
        enShadowRender m_eRender; /**< The rendering mode */

        enShadowQuality m_eQuality;   /**< The currently used shadow quality, i.e. size of the shadow map */

        std::vector<SShadowNode> m_vStatic;   /**< A list of nodes for rendering the shadow map */
        std::vector<SShadowNode> m_vMoving;   /**< A list of moving nodes for rendering the shadow map */
        std::vector<SShadowNode> m_vMarble;   /**< A list of marbles for rendering the shadow map */

        void setShadow1Material();    /**< Set the materials of all nodes in m_vNodes to the solid shadow material */
        void setShadow2Material();    /**< Set the materials of all nodes in m_vNodes to the transparent shadow material */
        void setShadow3Material();    /**< Set the materials of all nodes in m_vNodes to the transparent color material */
        void setRenderMaterial ();    /**< Set the render materials of the nodes */
        void restoreMaterials  ();    /**< Restore the original Irrlicht materials for the nodes */

        /**
        * Find a light camera in the scen
        * @param a_pNode the node to check
        * @return the light camera, nullptr if no light camera was found
        */
        irr::scene::ICameraSceneNode *findLightCamera(irr::scene::ISceneNode *a_pNode);

        void createLightMatrix();

        /**
        * Fill the "m_pDataNode" member
        * @param a_pNode the node to check
        */
        void fillDataNode(irr::scene::ISceneNode *a_pNode);

      public:
        /**
        * The constructor
        * @param a_pDevice the Irrlicht Device
        */
        CDustbinShaders(irr::IrrlichtDevice *a_pDevice);

        /**
        * The destructor
        */
        virtual ~CDustbinShaders();

        /**
        * Get the camera representing the light
        * @return the camera representing the light
        */
        irr::scene::ICameraSceneNode *getLightCamera();

        /**
        * Update the data sent to the shader using the light camera.
        * This method needs to be called if the light camera has been modified
        */
        void updateLightCamera();

        irr::f32 getFieldOfView();

        void setFieldOfView(irr::f32 a_fFieldOfView);

        /**
        * Get the texture the shadow map is currently rendered to
        * @return the texture the shadow map is currently rendered to
        */
        irr::video::ITexture *getShadowTexture();

        /**
        * Get the texture the transparent shadow map is currently rendered to
        * @return the texture the shadow map is currently rendered to
        */
        irr::video::ITexture *getShadowTexture2();

        /**
        * Get the texture the transparent color shadow map is currently rendered to
        * @return the texture the shadow map is currently rendered to
        */
        irr::video::ITexture *getShadowTexture3();

        /**
        * Start the rendering of on or more of the shadow maps
        */
        void startShadowMaps();

        /**
        * This method needs to be called after rendering
        * one or more of the shadow maps is done (material
        * and camera restoring is done here)
        */
        void endShadowMaps();

        /**
        * Render the scene. Some material modification is necessary here
        */
        void renderScene();

        /**
        * Render the shadow map
        * @param a_iRender a bitmap with enShadowMap values
        */
        void renderShadowMap(irr::u32 a_iRender);

        /**
        * Clear the shadow maps
        */
        void clearShadowMaps();

        /**
        * Register a material of a node
        * @param a_pNode the node the material belongs to
        * @param a_iMaterial the index of the material
        * @param a_eNewMaterial the material type to be registered (could also be taken from the scene node)
        * @param a_bCastShadow does this material cast a shadow?
        * @param a_eType the type of node
        */
        void addNodeMaterial(irr::scene::IMeshSceneNode *a_pNode, irr::u32 a_iMaterial, irr::video::E_MATERIAL_TYPE a_eNewMaterial, bool a_bCastShadow, enObjectType a_eType);

        /**
        * Set the material for the rendering pass of a node
        * @param a_pNode the node to adjust
        * @param a_iMaterial the material to adjust
        * @param a_eMaterial the new material
        */
        void adjustNodeMaterial(irr::scene::IMeshSceneNode *a_pNode, irr::u32 a_iMaterial, irr::video::E_MATERIAL_TYPE a_eMaterial);

        /**
        * Delete a node from the list
        * @param a_pNode the node to delete
        */
        void deleteNode(irr::scene::ISceneNode *a_pNode);

        /**
        * Change the rendering options
        * @param a_eRender what to render
        * @param a_eShadowMode the new shadow mode
        * @param a_eQuality the new shadow quality
        */
        void setRenderOptions(enShadowRender a_eRender, enShadowMode a_eShadowMode, enShadowQuality a_eQuality);

        /**
        * Get the render option
        * @return the render option
        */
        enShadowRender getRenderOption();

        /**
        * Get the current shadow mode
        * @return the current shadow mode
        */
        enShadowMode getShadowMode();

        /**
        * Get the current shadow quality
        * @return the current shadow quality
        */
        enShadowQuality getShadowQuality();

        /**
        * Convert a material from the enMaterialType enum to an Irrlicht material type enum value
        * @param a_eType the internal material type
        * @return the corresponding Irrlicht material type enum value
        * @see enMaterialType
        */
        irr::video::E_MATERIAL_TYPE getMaterial(enMaterialType a_eType);

        /**
        * Clear the node list
        */
        void clear();

        /**
        * Is the material a material provided by this shader?
        * @param a_eType the type of material
        * @return true if it's a material of this shader
        */
        bool isShaderMaterial(irr::video::E_MATERIAL_TYPE a_eType);

        /**
        * A method for the editor: make the camera null to prevent calling a deleted object
        */
        void removeCamera();

        /**
        * Add a new light camera
        */
        void addLightCamera();


        /**
        * Get the scene manager
        * @return the scene manager
        */
        irr::scene::ISceneManager *getSceneManager();
    };
  }
}
