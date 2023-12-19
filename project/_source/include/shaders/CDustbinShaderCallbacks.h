/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#pragma once

#include <irrlicht.h>

#include <shaders/CDustbinShaderDefines.h>

namespace dustbin {
  namespace shaders {
    /**
    * @class CDustbinShaderCallback
    * @author Christian Keimel
    * This class implements the Irrlicht shader set constants callback for my shader
    */
    class CDustbinShaderCallback : public irr::video::IShaderConstantSetCallBack {
      protected:
        /**
        * We render two passes
        */
        enum class enPass {
          Shadow,       /**< First pass: render the shadow map */
          TransColors,  /**< Second pass: the colors for the transparent shadows */
          Scene,        /**< Thirs pass: render the scene */

          Count
        };

        /**
        * This enum is for the shader constants
        */
        enum class enShaderConst {
          WorldViewProjection,    // The world view projection matrix
          LightMatrix,            // The light matrix
          World,                  // The world matrix
          MaxDepth,               // The maximum depth of the shadow map
          RttSize,                // The size of the shadow map
          Material,               // The material type (@see enMaterialType)
          ShadowMode,             // The flag defining whether or not shadows are rendered
          Texture1,               // Texture No 1
          Texture2,               // Second texture
          Texture3,               // Texture three
          Shadow,                 // The shadow texture
          Shadow2,                // The transparent shadow texture
          Shadow3,                // The transparent color texture
          Shadow4,                // The marble shadow map
          Light,                  // The light direction
          NoTextures,             // The number of textures used
          
          Count
        };

        irr::video::IVideoDriver *m_pDrv;   /**< The Irrlicht video driver */

        irr::s32 m_aMaterial[(int)enMaterialType::Count];   /**< The material identifiers for all supported materials */
        irr::s32 m_aTextures[(int)enMaterialType::Count];   /**< The texture indices for all supported materials */

        irr::u32 m_iRttSize;    /**< Size of the shadow map (always square) */
        
        enShadowMode m_eMode;   /**< The shadow rendering mode */

        irr::f32 m_fCameraFar;  /**< The far value of the camera */
        irr::f32 m_fVertical;   /**< "Flag" to notify the shader when rendering a vertical structure (maybe this shoud be a irr::s32??) */
        irr::f32 m_fMinY;       /**< The minimum Y value of a vertical structure, necessary to align the hexagon texture */

        irr::core::matrix4 m_cProjection;   /**< The light's projection matrix */
        irr::core::matrix4 m_cViewMatrix;   /**< The light's view matrix */

        irr::core::vector3df m_cLight;  /**< The light direction */

        irr::scene::ICameraSceneNode *m_pLight;   /**< The camera representing the light */

        irr::s32 m_aShaderConsts[(int)enPass::Count][(int)enShaderConst::Count];  /**< The indices of all shader variables for all rendering passes (will be filled on first frame) */

      public:
        /**
        * The constructor
        * @param a_pDrv the Irrlicht video driver
        * @param a_iRttSize initial size of the shadow map textures
        */
        CDustbinShaderCallback(irr::video::IVideoDriver *a_pDrv, int a_iRttSize);

        /**
        * The destructor
        */
        virtual ~CDustbinShaderCallback();

        /**
        * Get a material identifier from the material enum
        * @param a_eMaterial the material ID of my shader
        * @return the Irrlicht material identifier
        * @see enMaterialType
        */
        irr::s32 getMaterial(enMaterialType a_eMaterial);

        /**
        * Irrlicht shader callback
        */
        virtual void OnSetConstants(irr::video::IMaterialRendererServices *a_pServices, irr::s32 a_iUserData) override;

        /**
        * Irrlicht callback called when a material is set for rendering
        */
        virtual void OnSetMaterial(const irr::video::SMaterial &a_cMaterial) override;

        /**
        * Load the shaders
        */
        void initializeShaders();

        /**
        * Update the light data
        * @param a_pLight the camera scene node representing the light
        */
        void updateLightCamera(irr::scene::ISceneNode *a_pLight);

        /**
        * Change the shadow rendering mode
        * @param a_eMode the new shadow rendering mode
        */
        void setShadowMode(enShadowMode a_eMode);

        /**
        * Change the size of the shadow map texture
        * @param a_iRttSize new size of the shadow map texture (which is always square)
        */
        void setShadowRttSize(irr::s32 a_iRttSize);
    };
  }
}
