/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <shaders/CDustbinShaderCallbacks.h>
#include <shaders/CDustbinShaderDefines.h>
#include <shaders/CDustbinShaders.h>
#include <string>

namespace dustbin {
  namespace shaders {
    const irr::c8 c_sLightCameraDefaultName[] = "__DustbinShaderLightCamera";
    /**
    * The constructor
    * @param a_pDevice the Irrlicht Device
    * @param a_eQuality the initial shadow quality i.e. size of the shadow map texture
    */
    CDustbinShaders::CDustbinShaders(irr::IrrlichtDevice* a_pDevice, enShadowQuality a_eQuality) :
      m_pDevice     (a_pDevice),
      m_pDrv        (a_pDevice->getVideoDriver()),
      m_pSmgr       (a_pDevice->getSceneManager()),
      m_pLightCamera(nullptr),
      m_pActive     (nullptr),
      m_pRttShadow1 (),
      m_pRttShadow2 (),
      m_pRttShadow3 (),
      m_pCallback   (nullptr),
      m_eMode       (enShadowMode::TransColor),
      m_eQuality    (enShadowQuality::High)
    {
      // Create the callback and initialize the shaders
      m_pCallback = new CDustbinShaderCallback(m_pDevice->getVideoDriver(), shadowQualityToSize(a_eQuality));
      m_pCallback->initializeShaders();
      m_pCallback->setShadowMode(m_eMode);

      addLightCamera();

      // Make the camera ortographic
      irr::core::matrix4 l_cOrtho;
      l_cOrtho.buildProjectionMatrixOrthoLH(1500.0f, 1500.0f, 5.0f, 2000.0f);
      m_pLightCamera->setProjectionMatrix(l_cOrtho, true);

      // Notify the callback about the camera
      m_pCallback->updateLightCamera(m_pLightCamera);

      // Initialize all shadow map textures with nullptr (will be created once used)
      for (int i = 0; i < (int)enShadowQuality::Count; i++)
        m_pRttShadow1[i] = nullptr;

      // Create the shadow map for the initial setting
      irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
      std::string l_sName1 = "__shadow1TextureRtt_" + std::to_string((int)m_eQuality);
      std::string l_sName2 = "__shadow2TextureRtt_" + std::to_string((int)m_eQuality);
      std::string l_sName3 = "__shadow3TextureRtt_" + std::to_string((int)m_eQuality);

      m_pRttShadow1[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName1.c_str(), irr::video::ECF_A8R8G8B8);
      m_pRttShadow2[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName2.c_str(), irr::video::ECF_A8R8G8B8);
      m_pRttShadow3[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName3.c_str(), irr::video::ECF_A8R8G8B8);

      printf("Ready.\n");
    }

    /**
    * The destructor
    */
    CDustbinShaders::~CDustbinShaders() {
      if (m_pCallback != nullptr)
        m_pCallback->drop();

      m_vNodes.clear();
    }

    /**
    * Find a light camera in the scen
    * @param a_pNode the node to check
    * @return the light camera, nullptr if no light camera was found
    */
    irr::scene::ICameraSceneNode *CDustbinShaders::findLightCamera(irr::scene::ISceneNode *a_pNode) {
      if (a_pNode->getType() == irr::scene::ESNT_CAMERA) {
        std::string l_sName = a_pNode->getName();
        if (l_sName == c_sLightCameraDefaultName) {
          return reinterpret_cast<irr::scene::ICameraSceneNode *>(a_pNode);
        }
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itChild = a_pNode->getChildren().begin(); l_itChild != a_pNode->getChildren().end(); l_itChild++) {
        irr::scene::ICameraSceneNode *p = findLightCamera(*l_itChild);
        if (p != nullptr)
          return p;
      }

      return nullptr;
    }

    /**
    * Add a new light camera
    */
    void CDustbinShaders::addLightCamera() {
      m_pLightCamera = findLightCamera(m_pSmgr->getRootSceneNode());

      if (m_pLightCamera == nullptr) {
        // Add a camera for the light and set it up
        m_pLightCamera = m_pSmgr->addCameraSceneNode(
          nullptr, 
          irr::core::vector3df(393.3f, 801.4f, -502.4f), 
          irr::core::vector3df(-392.85f, 800.65f, -501.83f), 
          -1, 
          false
        );
        m_pLightCamera->setFarValue(2000.0f);
        m_pLightCamera->setNearValue(5.0f);

        m_pLightCamera->setPosition(irr::core::vector3df(393.3f, 801.4f, -502.4f));
        m_pLightCamera->setTarget  (irr::core::vector3df(0.0f, 0.0f, 0.0f));

        m_pLightCamera->setName(c_sLightCameraDefaultName);
      }
    }

    /**
    * Go through all nodes in the list and set the material to the solid shadow material
    */
    void CDustbinShaders::setShadow1Material() {
      for (auto l_cNode : m_vNodes) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow1;
        }
      }
    }

    /**
    * Go through all nodes in the list and set the material to the transparent shadow material
    */
    void CDustbinShaders::setShadow2Material() {
      for (auto l_cNode : m_vNodes) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow2;
        }
      }
    }

    /**
    * Go through all nodes in the list and set the material to the transparent color material
    */
    void CDustbinShaders::setShadow3Material() {
      for (auto l_cNode : m_vNodes) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow3;
        }
      }
    }

    /**
    * Set the render materials of the nodes
    */
    void CDustbinShaders::setRenderMaterial() {
      for (auto l_cNode : m_vNodes) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eRenderM;
        }
      }
    }

    /**
    * Restore the original Irrlicht materials for the nodes 
    */
    void CDustbinShaders::restoreMaterials() {
      for (auto l_cNode : m_vNodes) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eOriginal;
        }
      }
    }

    /**
    * Get the camera representing the light
    * @return the camera representing the light
    */
    irr::scene::ICameraSceneNode* CDustbinShaders::getLightCamera() {
      return m_pLightCamera;
    }

    /**
    * Update the data sent to the shader using the light camera.
    * This method needs to be called if the light camera has been modified
    */
    void CDustbinShaders::updateLightCamera() {
      if (m_pLightCamera != nullptr) {
        m_pLightCamera->updateAbsolutePosition();
        m_pCallback->updateLightCamera(m_pLightCamera);
      }
    }

    /**
    * Get the texture the shadow map is currently rendered to
    * @return the texture the shadow map is currently rendered to
    */
    irr::video::ITexture* CDustbinShaders::getShadowTexture() {
      return m_pRttShadow1[(int)m_eQuality];
    }

    /**
    * Get the texture the transparent shadow map is currently rendered to
    * @return the texture the shadow map is currently rendered to
    */
    irr::video::ITexture* CDustbinShaders::getShadowTexture2() {
      return m_pRttShadow2[(int)m_eQuality];
    }

    /**
    * Get the texture the transparent color shadow map is currently rendered to
    * @return the texture the shadow map is currently rendered to
    */
    irr::video::ITexture* CDustbinShaders::getShadowTexture3() {
      return m_pRttShadow3[(int)m_eQuality];
    }

    /**
    * Start the rendering of on or more of the shadow maps
    */
    void CDustbinShaders::startShadowMaps() {
      if (m_pLightCamera != nullptr) {
        m_pActive = m_pSmgr->getActiveCamera();
        m_pSmgr->setActiveCamera(m_pLightCamera);
      }
    }

    /**
    * A method for the editor: make the camera null to prevent calling a deleted object
    */
    void CDustbinShaders::removeCamera() {
      m_pLightCamera = nullptr;
    }

    /**
    * This method needs to be called after rendering
    * one or more of the shadow maps is done (material
    * and camera restoring is done here)
    */
    void CDustbinShaders::endShadowMaps() {
      // We assume that no shadow maps were rendered
      // if "m_pActive" is null
      if (m_pActive != nullptr) {
        m_pSmgr->setActiveCamera(m_pActive);
        m_pDrv->setRenderTarget(nullptr);
        m_pActive = nullptr;
      }
    }

    /**
    * Render the scene. Some material modification is necessary here
    */
    void CDustbinShaders::renderScene() {
      setRenderMaterial();
      m_pSmgr->drawAll();
      restoreMaterials();
    }

    /**
    * Render the shadow map
    * @param a_eToRender which shadow map shall be rendered
    */
    void CDustbinShaders::renderShadowMap(enShadowMap a_eToRender) {
      if (m_eMode > enShadowMode::Off && a_eToRender == enShadowMap::Solid) {
        m_pDrv->setRenderTarget(m_pRttShadow1[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));
        setShadow1Material();

        for (auto l_cNode : m_vNodes) {
          l_cNode.m_pNode->render();
        }
        
      }
      if (m_eMode > enShadowMode::Solid && a_eToRender == enShadowMap::Transparent) {
        m_pDrv->setRenderTarget(m_pRttShadow2[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
        setShadow2Material();

        for (auto l_cNode : m_vNodes) {
          l_cNode.m_pNode->render();
        }
      }

      if (m_eMode > enShadowMode::SolidTrans && a_eToRender == enShadowMap::TranspColor) {
        m_pDrv->setRenderTarget(m_pRttShadow3[(int)m_eQuality], true, true, irr::video::SColor(0, 0xFF, 0xFF, 0xFF));
        setShadow3Material();

        for (auto l_cNode : m_vNodes) {
          l_cNode.m_pNode->render();
        }
      }
    }

    /**
    * Change the shadow rendering mode
    * @param a_eMode the new shadow rendering mode
    */
    void CDustbinShaders::setShadowMode(enShadowMode a_eMode) {
      m_eMode = a_eMode;
      m_pCallback->setShadowMode(m_eMode);
    }

    /**
    * Get the current shadow mode
    * @return the current shadow mode
    */
    enShadowMode CDustbinShaders::getShadowMode() {
      return m_eMode;
    }

    /**
    * Set the shadow quality i.e. resolution of the shadow map texture
    * @param a_eQuality the new shadow quality
    */
    void CDustbinShaders::setShadowQuality(enShadowQuality a_eQuality) {
      if (m_pRttShadow1[(int)m_eQuality] != nullptr) m_pDrv->removeTexture(m_pRttShadow1[(int)m_eQuality]);
      if (m_pRttShadow2[(int)m_eQuality] != nullptr) m_pDrv->removeTexture(m_pRttShadow2[(int)m_eQuality]);
      if (m_pRttShadow3[(int)m_eQuality] != nullptr) m_pDrv->removeTexture(m_pRttShadow3[(int)m_eQuality]);

      m_eQuality = a_eQuality;

      if (m_pRttShadow1[(int)m_eQuality] == nullptr) {
        irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
        std::string l_sName = "__shadow1TextureRtt_" + std::to_string((int)m_eQuality);

        m_pRttShadow1[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
      }

      if (m_pRttShadow2[(int)m_eQuality] == nullptr) {
        irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
        std::string l_sName = "__shadow2TextureRtt_" + std::to_string((int)m_eQuality);

        m_pRttShadow2[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
      }

      if (m_pRttShadow3[(int)m_eQuality] == nullptr) {
        irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
        std::string l_sName = "__shadow3TextureRtt_" + std::to_string((int)m_eQuality);

        m_pRttShadow3[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
      }

      for (auto l_cNode : m_vNodes) {
        for (auto &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);
        }
      }

      m_pCallback->setShadowRttSize(shadowQualityToSize(m_eQuality));
    }

    /**
    * Register a material of a node
    * @param a_pNode the node the material belongs to
    * @param a_iMaterial the index of the material
    * @param a_eNewMaterial the material type to be registered (could also be taken from the scene node)
    * @param a_bCastShadow does this material cast a shadow?
    */
    void CDustbinShaders::addNodeMaterial(irr::scene::IMeshSceneNode* a_pNode, irr::u32 a_iMaterial, irr::video::E_MATERIAL_TYPE a_eNewMaterial, bool a_bCastShadow) {
      std::vector<SShadowNode>::iterator l_itNode = m_vNodes.begin();

      // Check whether or not the node of the material is already in the list
      while (l_itNode != m_vNodes.end()) {
        if ((*l_itNode).m_pNode == a_pNode)
          break;

        l_itNode++;
      }

      // Node not found? Add it.
      if (l_itNode == m_vNodes.end()) {
        m_vNodes.push_back(SShadowNode(a_pNode, a_pNode->isVisible()));
        l_itNode = m_vNodes.end();
        l_itNode--;
      }

      std::vector<SShadowNodeMaterial>::iterator l_itMaterial = (*l_itNode).m_vMaterials.begin();

      // Check whether or not the material is already in the list,
      // stop the method if the material is found
      while (l_itMaterial != (*l_itNode).m_vMaterials.end()) {
        if ((*l_itMaterial).m_iMaterial == a_iMaterial) {
          break;
        }
        l_itMaterial++;
      }

      if (l_itMaterial == (*l_itNode).m_vMaterials.end()) {
        // Add the shadow map textures to the material
        a_pNode->getMaterial(a_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
        a_pNode->getMaterial(a_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
        a_pNode->getMaterial(a_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);

        // Add the material to the shadow material vector
        (*l_itNode).m_vMaterials.push_back(SShadowNodeMaterial(
          a_iMaterial, 
          a_bCastShadow && a_eNewMaterial != irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap ) : irr::video::EMT_TRANSPARENT_ADD_COLOR,
          a_bCastShadow && a_eNewMaterial == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap ) : irr::video::EMT_TRANSPARENT_ADD_COLOR,
          a_bCastShadow && a_eNewMaterial == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap3) : irr::video::EMT_TRANSPARENT_ADD_COLOR,
          a_eNewMaterial,
          a_pNode->getMaterial(a_iMaterial).MaterialType
        ));
      }
      else {
        (*l_itMaterial).m_eShadow1  = a_bCastShadow && a_eNewMaterial != irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap ) : irr::video::EMT_TRANSPARENT_ADD_COLOR;
        (*l_itMaterial).m_eShadow2  = a_bCastShadow && a_eNewMaterial == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap ) : irr::video::EMT_TRANSPARENT_ADD_COLOR;
        (*l_itMaterial).m_eShadow3  = a_bCastShadow && a_eNewMaterial == irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL ? (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::ShadowMap3) : irr::video::EMT_TRANSPARENT_ADD_COLOR;
        (*l_itMaterial).m_eRenderM  = a_eNewMaterial;
        (*l_itMaterial).m_eOriginal = a_pNode->getMaterial(a_iMaterial).MaterialType;
      }
    }

    /**
    * Convert a material from the enMaterialType enum to an Irrlicht material type enum value
    * @param a_eType the internal material type
    * @return the corresponding Irrlicht material type enum value
    * @see enMaterialType
    */
    irr::video::E_MATERIAL_TYPE CDustbinShaders::getMaterial(enMaterialType a_eType) {
      return (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(a_eType);
    }

    /**
    * Clear the node list
    */
    void CDustbinShaders::clear() {
      m_vNodes.clear();
    }

    /**
    * Is the material a material provided by this shader?
    * @param a_eType the type of material
    * @return true if it's a material of this shader
    */
    bool CDustbinShaders::isShaderMaterial(irr::video::E_MATERIAL_TYPE a_eType) {
      return 
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::SolidOne  ) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::SolidTwo  ) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::SolidThree) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::Marble    )
      ; 
    }

    /**
    * The constructor
    * @param a_iMaterial the material index
    * @param a_eShadow1 the material material for the solid shadow pass
    * @param a_eShadow2 the material material for the transparent shadow pass
    * @param a_eShadow3 the material material for the transparent color pass
    * @param a_eOriginal the original material
    */
    CDustbinShaders::SShadowNodeMaterial::SShadowNodeMaterial(
      irr::u32                    a_iMaterial, 
      irr::video::E_MATERIAL_TYPE a_eShadow1, 
      irr::video::E_MATERIAL_TYPE a_eShadow2, 
      irr::video::E_MATERIAL_TYPE a_eShadow3, 
      irr::video::E_MATERIAL_TYPE a_eRenderM,
      irr::video::E_MATERIAL_TYPE a_eOriginal
    ) :
      m_iMaterial(a_iMaterial),
      m_eShadow1 (a_eShadow1 ),
      m_eShadow2 (a_eShadow2 ),
      m_eShadow3 (a_eShadow3 ),
      m_eRenderM (a_eRenderM ),
      m_eOriginal(a_eOriginal)
    {
    }

    /**
    * Copy constructor
    * @param a_cOther the data struct to copy
    */
    CDustbinShaders::SShadowNodeMaterial::SShadowNodeMaterial(const SShadowNodeMaterial& a_cOther) : 
      m_iMaterial(a_cOther.m_iMaterial),
      m_eShadow1 (a_cOther.m_eShadow1 ),
      m_eShadow2 (a_cOther.m_eShadow2 ),
      m_eShadow3 (a_cOther.m_eShadow3 ),
      m_eRenderM (a_cOther.m_eRenderM ),
      m_eOriginal(a_cOther.m_eOriginal)
    {
    }

    /**
    * The constructor
    * @param a_pNode the scene node
    * @param a_bVisible is this node visible in the scene?
    * @param a_bCastShadow Does this node cast a shadow?
    */
    CDustbinShaders::SShadowNode::SShadowNode(irr::scene::ISceneNode* a_pNode, bool a_bVisible) :
      m_pNode   (a_pNode),
      m_bVisible(a_bVisible)
    {
    }

    /**
    * Copy constructor
    * @param a_cOther the data struct to copy
    */
    CDustbinShaders::SShadowNode::SShadowNode(const SShadowNode& a_cOther) :
      m_pNode   (a_cOther.m_pNode      ),
      m_bVisible(a_cOther.m_bVisible   )
    {
      for (auto const &l_cMaterial: a_cOther.m_vMaterials)
        m_vMaterials.push_back(SShadowNodeMaterial(l_cMaterial));
    }

    /**
    * Helper function converting a shadow quality enum value to a resolution
    * (Note: we always use square textures for the shadow map)
    * @param a_eQuality the quality setting we request the shadow map size for
    * @return the shadow map size for a given shadow quality
    * @see enShadowQuality
    */
    irr::s32 shadowQualityToSize(enShadowQuality a_eQuality) {
      switch (a_eQuality) {
        case enShadowQuality::Top  : return 16384;
        case enShadowQuality::High : return  8192;
        case enShadowQuality::HiMid: return  4096;
        case enShadowQuality::LoMid: return  2049;
        case enShadowQuality::Low  : return  1024;

        default: return 512;
      }
    }

    /**
    * Get the scene manager
    * @return the scene manager
    */
    irr::scene::ISceneManager *CDustbinShaders::getSceneManager() {
      return m_pSmgr;
    }
  }
}
