/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <shaders/CDustbinShaderCallbacks.h>

namespace dustbin {
  namespace shaders {
    /**
    * The constructor
    * @param a_pDrv the Irrlicht video driver
    * @param a_iRttSize initial size of the shadow map textures
    */
    CDustbinShaderCallback::CDustbinShaderCallback(irr::video::IVideoDriver *a_pDrv, int a_iRttSize) : 
      m_pDrv       (a_pDrv),
      m_aMaterial  (),
      m_aTextures  (),
      m_iRttSize   (a_iRttSize),
      m_fCameraFar (2000.0f), 
      m_fVertical  (0.0f),
      m_fMinY      (0.0f),
      m_eMode      (enShadowMode::Off),
      m_pLight     (nullptr)
    {
      for (int i = 0; i < (int)enMaterialType::Count; i++)
        m_aMaterial[i] = -1;

      m_aTextures[(int)enMaterialType::Marble    ] = 0;
      m_aTextures[(int)enMaterialType::SolidOne  ] = 0;
      m_aTextures[(int)enMaterialType::SolidTwo  ] = 1;
      m_aTextures[(int)enMaterialType::SolidThree] = 2;
      m_aTextures[(int)enMaterialType::Wall1     ] = 0;
      m_aTextures[(int)enMaterialType::Wall2     ] = 1;
      m_aTextures[(int)enMaterialType::Wall3     ] = 2;
      m_aTextures[(int)enMaterialType::ShadowMap ] = 7;
      m_aTextures[(int)enMaterialType::ShadowMap2] = 6;
      m_aTextures[(int)enMaterialType::ShadowMap3] = 5;

      for (int i = 0; i < (int)enShaderConst::Count; i++)
        for (int j = 0; j < (int)enPass::Count; j++)
          m_aShaderConsts[j][i] = -1;
    }

    /**
    * The destructor
    */
    CDustbinShaderCallback::~CDustbinShaderCallback() {
    }

    /**
    * Get a material identifier from the material enum
    * @param a_eMaterial the material ID of my shader
    * @return the Irrlicht material identifier
    * @see enMaterialType
    */
    irr::s32 CDustbinShaderCallback::getMaterial(enMaterialType a_eMaterial) {
      return a_eMaterial != enMaterialType::Count ? m_aMaterial[(int)a_eMaterial] : -1;
    }

    /**
    * Irrlicht shader callback
    */
    void CDustbinShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices *a_pServices, irr::s32 a_iUserData) {
      int l_iPass = 
        (a_iUserData == (int)enMaterialType::ShadowMap || a_iUserData == (int)enMaterialType::ShadowMap2) ? (int)enPass::Shadow :
        a_iUserData == (int)enMaterialType::ShadowMap3 ? (int)enPass::TransColors : 
        (int)enPass::Scene;

      if (m_aShaderConsts[l_iPass][0] == -1) {
        m_aShaderConsts[l_iPass][(int)enShaderConst::LightMatrix        ] = a_pServices->getPixelShaderConstantID ("mLightMatrix"  );
        m_aShaderConsts[l_iPass][(int)enShaderConst::World              ] = a_pServices->getVertexShaderConstantID("mWorld"        );
        m_aShaderConsts[l_iPass][(int)enShaderConst::MaxDepth           ] = a_pServices->getVertexShaderConstantID("fMaxDepth"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::RttSize            ] = a_pServices->getVertexShaderConstantID("iRttSize"      );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Material           ] = a_pServices->getPixelShaderConstantID ("iMaterial"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::ShadowMode         ] = a_pServices->getPixelShaderConstantID ("iShadowMode"   );
        m_aShaderConsts[l_iPass][(int)enShaderConst::WorldViewProjection] = a_pServices->getVertexShaderConstantID("mWorldViewProj");
        m_aShaderConsts[l_iPass][(int)enShaderConst::Texture1           ] = a_pServices->getPixelShaderConstantID ("tTexture1"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Texture2           ] = a_pServices->getPixelShaderConstantID ("tTexture2"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Texture3           ] = a_pServices->getPixelShaderConstantID ("tTexture3"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow             ] = a_pServices->getPixelShaderConstantID ("tShadow"       );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow2            ] = a_pServices->getPixelShaderConstantID ("tShadow2"      );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow3            ] = a_pServices->getPixelShaderConstantID ("tShadow3"      );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Light              ] = a_pServices->getPixelShaderConstantID ("vLight"        );
        m_aShaderConsts[l_iPass][(int)enShaderConst::NoTextures         ] = a_pServices->getPixelShaderConstantID ("iNoTextures"   );
        m_aShaderConsts[l_iPass][(int)enShaderConst::Vertical           ] = a_pServices->getPixelShaderConstantID ("fVertical"     );
        m_aShaderConsts[l_iPass][(int)enShaderConst::MinY               ] = a_pServices->getPixelShaderConstantID ("fMinY"         );
      }

      enMaterialType l_eType = (enMaterialType)a_iUserData;
      irr::video::IVideoDriver *l_pDrv = a_pServices->getVideoDriver();

      irr::core::matrix4 l_cWorld;
      l_cWorld = l_pDrv->getTransform(irr::video::ETS_WORLD);

      irr::core::matrix4 l_cLight = m_cProjection;
      l_cLight *= m_cViewMatrix;
      l_cLight *= l_pDrv->getTransform(irr::video::ETS_WORLD);

      a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::LightMatrix], l_cLight.pointer(), 16);

      a_pServices->setVertexShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::World   ], l_cWorld.pointer(), 16);
      a_pServices->setVertexShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::MaxDepth], &m_fCameraFar     , 1);
      a_pServices->setVertexShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::RttSize ], &m_iRttSize         , 1);
      a_pServices->setPixelShaderConstant (m_aShaderConsts[l_iPass][(int)enShaderConst::Material], &a_iUserData        , 1);

      irr::s32 l_iShadows = (irr::s32)m_eMode;

      a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::ShadowMode], &l_iShadows, 1);

      if (l_iPass == (int)enPass::TransColors || l_iPass == (int)enPass::Scene) {
        irr::core::matrix4 l_cWorldViewMtx;

        l_cWorldViewMtx  = l_pDrv->getTransform(irr::video::ETS_PROJECTION);			
        l_cWorldViewMtx *= l_pDrv->getTransform(irr::video::ETS_VIEW);
        l_cWorldViewMtx *= l_pDrv->getTransform(irr::video::ETS_WORLD);

        a_pServices->setVertexShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::WorldViewProjection], l_cWorldViewMtx.pointer(), 16);

        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Texture1], &m_aTextures[(int)enMaterialType::SolidOne  ], 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Texture2], &m_aTextures[(int)enMaterialType::SolidTwo  ], 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Texture3], &m_aTextures[(int)enMaterialType::SolidThree], 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow  ], &m_aTextures[(int)enMaterialType::ShadowMap ], 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow2 ], &m_aTextures[(int)enMaterialType::ShadowMap2], 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Shadow3 ], &m_aTextures[(int)enMaterialType::ShadowMap3], 1);
        
        irr::f32 l_aValue[3];
        m_cLight.getAs3Values(l_aValue);

        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Light], l_aValue, 3);

        irr::s32 l_iNoTextures = 
          l_eType == enMaterialType::SolidThree ? 3 : 
          l_eType == enMaterialType::SolidTwo   ? 2 : 
          l_eType == enMaterialType::Wall3      ? 3 :
          l_eType == enMaterialType::Wall2      ? 2 : 1;

        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::NoTextures], &l_iNoTextures, 1);

        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::Vertical], &m_fVertical, 1);
        a_pServices->setPixelShaderConstant(m_aShaderConsts[l_iPass][(int)enShaderConst::MinY    ], &m_fMinY    , 1);
      }
    }

    /**
    * Irrlicht callback called when a material is set for rendering
    */
    void CDustbinShaderCallback::OnSetMaterial(const irr::video::SMaterial& a_cMaterial) {
      m_fVertical = a_cMaterial.MaterialTypeParam;
      m_fMinY     = a_cMaterial.MaterialTypeParam2;
    }

    /**
    * Change the shadow rendering mode
    * @param a_eMode the new shadow rendering mode
    */
    void CDustbinShaderCallback::setShadowMode(enShadowMode a_eMode) {
      m_eMode = a_eMode;
    }

    /**
    * Change the size of the shadow map texture
    * @param a_iRttSize new size of the shadow map texture (which is always square)
    */
    void CDustbinShaderCallback::setShadowRttSize(irr::s32 a_iRttSize) {
      m_iRttSize = a_iRttSize;
    }

    /**
    * Update the light data
    * @param a_pLight the camera scene node representing the light
    */
    void CDustbinShaderCallback::updateLightCamera(irr::scene::ISceneNode* a_pLight) {
      if (a_pLight->getType() == irr::scene::ESNT_CAMERA) {
        m_pLight = reinterpret_cast<irr::scene::ICameraSceneNode*>(a_pLight);

        if (m_pLight != nullptr) {
          m_pLight->updateMatrices();

          m_cProjection = m_pLight->getProjectionMatrix();
          m_cViewMatrix = m_pLight->getViewMatrix      ();

          irr::core::vector3df l_cPos = m_pLight->getAbsolutePosition();
          irr::core::vector3df l_cTgt = m_pLight->getTarget();

          m_cLight = (l_cPos - l_cTgt).normalize();
        }
      }
    }

    /**
    * Load the shaders
    */
    void CDustbinShaderCallback::initializeShaders() {
      // The shader for the depth map
      m_aMaterial[(int)enMaterialType::ShadowMap] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_depthmap.vert",
        "data/shaders/dustbin_shader_depthmap.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::ShadowMap   // This identifies that we are rendering a the shadow map
      );

      // The shader for materials which do not cast a shadow
      m_aMaterial[(int)enMaterialType::ShadowMapNo] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_depthmap.vert",
        "data/shaders/dustbin_shader_depthmap_no.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::ShadowMap   // This identifies that we are rendering nothing to the shadow map
      );

      // The shader for transparent color materials
      m_aMaterial[(int)enMaterialType::ShadowMap3] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_tcolor.vert",
        "data/shaders/dustbin_shader_tcolor.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::ShadowMap3   // This identifies that we are rendering to a shadow map
      );

      // The shader for the marble material
      m_aMaterial[(int)enMaterialType::Marble] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::Marble    // This is used to identify the material as marble material
      );

      // The shader for the wall material
      m_aMaterial[(int)enMaterialType::Wall1] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::Wall1 // This is used to identify the material as wall material
      );

      // The shader for the wall material
      m_aMaterial[(int)enMaterialType::Wall2] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::Wall2 // This is used to identify the material as wall material
      );

      // The shader for the wall material
      m_aMaterial[(int)enMaterialType::Wall3] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::Wall3 // This is used to identify the material as wall material
      );

      // The shader for a single texture
      m_aMaterial[(int)enMaterialType::SolidOne] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::SolidOne    // Userdata to identify the material as single texture
      );

      // The shader for two materials stacked upon each other
      m_aMaterial[(int)enMaterialType::SolidTwo] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::SolidTwo    // Identify the material
      );

      // The shader for the three textures material
      m_aMaterial[(int)enMaterialType::SolidThree] = m_pDrv->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(
        "data/shaders/dustbin_shader_solid.vert",
        "data/shaders/dustbin_shader_solid.frag", 
        this, 
        irr::video::EMT_SOLID, 
        (irr::s32)enMaterialType::SolidThree    // This identifies the shader as the three texture shader
      );
    }
  }
}