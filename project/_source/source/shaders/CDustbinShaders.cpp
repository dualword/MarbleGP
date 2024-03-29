/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <shaders/CDustbinShaderCallbacks.h>
#include <shaders/CDustbinShaderDefines.h>
#include <scenenodes/CDustbinLight.h>
#include <shaders/CDustbinShaders.h>
#include <string>

namespace dustbin {
  namespace shaders {
    const irr::c8 c_sLightCameraDefaultName[] = "__DustbinShaderLightCamera";

    const irr::u32 c_iRenderTextureOne = (irr::u32)enShadowMap::Solid | (irr::u32)enShadowMap::Moving;

    /**
    * The constructor
    * @param a_pDevice the Irrlicht Device
    */
    CDustbinShaders::CDustbinShaders(irr::IrrlichtDevice* a_pDevice) :
      m_pDevice     (a_pDevice),
      m_pDrv        (a_pDevice->getVideoDriver()),
      m_pSmgr       (a_pDevice->getSceneManager()),
      m_pLightCamera(nullptr),
      m_pActive     (nullptr),
      m_pDataNode   (nullptr),
      m_pRttShadow1 (),
      m_pRttShadow2 (),
      m_pRttShadow3 (),
      m_pRttShadow4 (),
      m_pCallback   (nullptr),
      m_eMode       (enShadowMode::Off),
      m_eQuality    (enShadowQuality::LoMid)
    {

      // Initialize all shadow map textures with nullptr (will be created once used)
      for (int i = 0; i < (int)enShadowQuality::Count; i++) {
        m_pRttShadow1[i] = nullptr;
        m_pRttShadow2[i] = nullptr;
        m_pRttShadow3[i] = nullptr;
        m_pRttShadow4[i] = nullptr;
      }

      printf("Ready.\n");
    }

    /**
    * The destructor
    */
    CDustbinShaders::~CDustbinShaders() {
      if (m_pCallback != nullptr)
        m_pCallback->drop();

      m_vStatic.clear();
    }

    /**
    * Find a light camera in the scene
    * @param a_pNode the node to check
    * @return the light camera, nullptr if no light camera was found
    */
    irr::scene::ICameraSceneNode *CDustbinShaders::findLightCamera(irr::scene::ISceneNode *a_pNode) {
      if (a_pNode->getType() == irr::scene::ESNT_CAMERA) {
        std::string l_sName = a_pNode->getName();
        if (l_sName == c_sLightCameraDefaultName) {
          irr::scene::ICameraSceneNode *l_pRet = reinterpret_cast<irr::scene::ICameraSceneNode *>(a_pNode);
          return l_pRet;
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
      fillDataNode(m_pSmgr->getRootSceneNode());

      if (m_pDataNode == nullptr) {
        m_pDataNode = reinterpret_cast<scenenodes::CDustbinLight *>(m_pSmgr->addSceneNode(scenenodes::g_DustbinLightName, m_pSmgr->getRootSceneNode()));

        m_pDataNode->setPosition   (irr::core::vector3df(393.3f, 801.4f, -502.4f));
        m_pDataNode->setLightTarget(irr::core::vector3df(-392.85f, 800.65f, -501.83f));
        m_pDataNode->setNearValue  (5.0f);
        m_pDataNode->setFarValue   (2000.0f);
        m_pDataNode->setFieldOfView(2000.0f);
      }

      m_pLightCamera = findLightCamera(m_pSmgr->getRootSceneNode());

      if (m_pLightCamera == nullptr) {
        // Add a camera for the light and set it up
        m_pLightCamera = m_pSmgr->addCameraSceneNode(
          nullptr, 
          m_pDataNode->getPosition(),
          m_pDataNode->getLightTarget(),
          -1, 
          false
        );
      }

      irr::core::matrix4 l_cMatrix;
      l_cMatrix.buildProjectionMatrixOrthoLH(
      m_pDataNode->getFieldOfView(),
        m_pDataNode->getFieldOfView(),
        m_pDataNode->getNearValue(),
        m_pDataNode->getFarValue()
      );

      m_pLightCamera->setProjectionMatrix(l_cMatrix);

      m_pLightCamera->setPosition(m_pDataNode->getPosition());
      m_pLightCamera->setTarget  (m_pDataNode->getLightTarget());

      m_pLightCamera->setName(c_sLightCameraDefaultName);
      
      m_pLightCamera->setIsDebugObject(true);
      m_pCallback->updateLightCamera(m_pLightCamera);
    }

    /**
    * Go through all nodes in the list and set the material to the solid shadow material
    */
    void CDustbinShaders::setShadow1Material() {
      for (auto l_cNode : m_vStatic) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow1;
        }
      }

      for (auto l_cNode : m_vMoving) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow1;
        }
      }

      for (auto l_cNode : m_vMarble) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow1;
        }
      }
    }

    /**
    * Go through all nodes in the list and set the material to the transparent shadow material
    */
    void CDustbinShaders::setShadow2Material() {
      for (auto l_cNode : m_vStatic) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow2;
        }
      }

      for (auto l_cNode : m_vMoving) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow2;
        }
      }

      for (auto l_cNode : m_vMarble) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow2;
        }
      }
    }

    /**
    * Go through all nodes in the list and set the material to the transparent color material
    */
    void CDustbinShaders::setShadow3Material() {
      for (auto l_cNode : m_vStatic) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow3;
        }
      }

      for (auto l_cNode : m_vMoving) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow3;
        }
      }

      for (auto l_cNode : m_vMarble) {
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eShadow3;
        }
      }
    }

    /**
    * Set the render materials of the nodes
    */
    void CDustbinShaders::setRenderMaterial() {
      for (auto l_cNode : m_vStatic) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eRenderM;
        }
      }

      for (auto l_cNode : m_vMoving) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eRenderM;
        }
      }

      for (auto l_cNode : m_vMarble) {
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
      for (auto l_cNode : m_vStatic) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eOriginal;
        }
      }

      for (auto l_cNode : m_vMoving) {
        l_cNode.m_pNode->setVisible(l_cNode.m_bVisible);
        for (auto const &l_cMaterial : l_cNode.m_vMaterials) {
          l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).MaterialType = l_cMaterial.m_eOriginal;
        }
      }

      for (auto l_cNode : m_vMarble) {
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

        m_pDataNode->setPosition   (m_pLightCamera->getAbsolutePosition());
        m_pDataNode->setLightTarget(m_pLightCamera->getTarget());

        m_pCallback->updateLightCamera(m_pLightCamera);

        createLightMatrix();
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
    * Get the texture the marble shadows are currently rendered to
    * @return the texture the shadow map is currently rendered to
    */
    irr::video::ITexture* CDustbinShaders::getShadowTexture4() {
      return m_pRttShadow4[(int)m_eQuality];
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
    * Clear the shadow maps
    */
    void CDustbinShaders::clearShadowMaps() {
#ifdef _WINDOWS
      m_pDrv->setRenderTarget(m_pRttShadow1[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));
      m_pDrv->setRenderTarget(m_pRttShadow2[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));
      m_pDrv->setRenderTarget(m_pRttShadow3[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));
      m_pDrv->setRenderTarget(m_pRttShadow4[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));
      m_pDrv->setRenderTarget(nullptr);
#endif
    }

    /**
    * Render the shadow map
    * @param a_iRender a bitmap with enShadowMap values
    */
    void CDustbinShaders::renderShadowMap(irr::u32 a_iRender) {
#ifdef _WINDOWS
      if (a_iRender > 0) {
        setShadow1Material();

        if ((a_iRender & c_iRenderTextureOne) != (irr::u32)0)
          m_pDrv->setRenderTarget(m_pRttShadow1[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));

        if ((a_iRender & (irr::s32)enShadowMap::Solid) == (irr::s32)enShadowMap::Solid) {
          for (auto l_cNode : m_vStatic) {
            l_cNode.m_pNode->updateAbsolutePosition();
            l_cNode.m_pNode->render();
          }
        }

        if ((a_iRender & (irr::s32)enShadowMap::Moving) == (irr::s32)enShadowMap::Moving) {
          for (auto l_cNode : m_vMoving) {
            l_cNode.m_pNode->render();
          }
        }

        if ((a_iRender & (irr::s32)enShadowMap::Marbles) == (irr::s32)enShadowMap::Marbles) {
          m_pDrv->setRenderTarget(m_pRttShadow4[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xff, 0xff, 0xff));

          for (auto l_cNode : m_vMarble) {
            l_cNode.m_pNode->render();
          }
        }

        if ((a_iRender & (irr::s32)enShadowMap::Transparent) == (irr::s32)enShadowMap::Transparent) {
          m_pDrv->setRenderTarget(m_pRttShadow2[(int)m_eQuality], true, true, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
          setShadow2Material();

          for (auto l_cNode : m_vStatic) {
            l_cNode.m_pNode->render();
          }
        }

        if ((a_iRender & (irr::s32)enShadowMap::TranspColor) == (irr::s32)enShadowMap::TranspColor) {
          m_pDrv->setRenderTarget(m_pRttShadow3[(int)m_eQuality], true, true, irr::video::SColor(0, 0xFF, 0xFF, 0xFF));
          setShadow3Material();

          for (auto l_cNode : m_vStatic) {
            l_cNode.m_pNode->render();
          }
        }
      }
#endif
    }

    /**
    * Change the rendering options
    * @param a_eRender what to render
    * @param a_eShadowMode the new shadow mode
    * @param a_eQuality the new shadow quality
    */
    void CDustbinShaders::setRenderOptions(enShadowMode a_eShadowMode, enShadowQuality a_eQuality) {
      if (m_pCallback == nullptr) {
        // Create the callback and initialize the shaders
        m_pCallback = new CDustbinShaderCallback(m_pDevice->getVideoDriver(), shadowQualityToSize(a_eQuality));
        m_pCallback->initializeShaders();
        m_pCallback->setShadowMode(a_eShadowMode);
      }

#ifdef _WINDOWS
      if ((m_eMode == enShadowMode::Off && a_eShadowMode != enShadowMode::Off) || (m_eQuality != a_eQuality)) {
        if (m_pRttShadow1[(int)m_eQuality] != nullptr) { m_pDrv->removeTexture(m_pRttShadow1[(int)m_eQuality]); m_pRttShadow1[(int)m_eQuality] = nullptr; }
        if (m_pRttShadow2[(int)m_eQuality] != nullptr) { m_pDrv->removeTexture(m_pRttShadow2[(int)m_eQuality]); m_pRttShadow2[(int)m_eQuality] = nullptr; }
        if (m_pRttShadow3[(int)m_eQuality] != nullptr) { m_pDrv->removeTexture(m_pRttShadow3[(int)m_eQuality]); m_pRttShadow3[(int)m_eQuality] = nullptr; }
        if (m_pRttShadow4[(int)m_eQuality] != nullptr) { m_pDrv->removeTexture(m_pRttShadow4[(int)m_eQuality]); m_pRttShadow4[(int)m_eQuality] = nullptr; }
      }
#endif

      m_eMode    = a_eShadowMode;
      m_eQuality = a_eQuality;

      if (m_pCallback != nullptr) {
        m_pCallback->setShadowMode(m_eMode);
#ifdef _WINDOWS
        if (m_pRttShadow1[(int)m_eQuality] == nullptr && m_eMode != enShadowMode::Off) {
          irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
          std::string l_sName = "__shadow1TextureRtt_" + std::to_string((int)m_eQuality);

          m_pRttShadow1[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
        }

        if (m_pRttShadow4[(int)m_eQuality] == nullptr && m_eMode != enShadowMode::Off) {
          irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
          std::string l_sName = "__shadow4TextureRtt_" + std::to_string((int)m_eQuality);

          m_pRttShadow4[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
        }

        if (m_pRttShadow2[(int)m_eQuality] == nullptr && m_eMode >= enShadowMode::SolidTrans) {
          irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
          std::string l_sName = "__shadow2TextureRtt_" + std::to_string((int)m_eQuality);

          m_pRttShadow2[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
        }

        if (m_pRttShadow3[(int)m_eQuality] == nullptr && m_eMode >= enShadowMode::TransColor) {
          irr::core::dimension2du l_cSize = irr::core::dimension2du(shadowQualityToSize(m_eQuality), shadowQualityToSize(m_eQuality));
          std::string l_sName = "__shadow3TextureRtt_" + std::to_string((int)m_eQuality);

          m_pRttShadow3[(int)m_eQuality] = m_pDrv->addRenderTargetTexture(l_cSize, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
        }

        for (auto l_cNode : m_vStatic) {
          for (auto &l_cMaterial : l_cNode.m_vMaterials) {
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(4, m_pRttShadow4[(int)m_eQuality]);
          }
        }

        for (auto l_cNode : m_vMoving) {
          for (auto &l_cMaterial : l_cNode.m_vMaterials) {
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(4, m_pRttShadow4[(int)m_eQuality]);
          }
        }

        for (auto l_cNode : m_vMarble) {
          for (auto &l_cMaterial : l_cNode.m_vMaterials) {
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);
            l_cNode.m_pNode->getMaterial(l_cMaterial.m_iMaterial).setTexture(4, m_pRttShadow4[(int)m_eQuality]);
          }
        }

        m_pCallback->setShadowRttSize(shadowQualityToSize(m_eQuality));
#endif
      }
    }

    /**
    * Get the current shadow mode
    * @return the current shadow mode
    */
    enShadowMode CDustbinShaders::getShadowMode() {
      return m_eMode;
    }

    /**
    * Get the current shadow quality
    * @return the current shadow quality
    */
    enShadowQuality CDustbinShaders::getShadowQuality() {
      return m_eQuality;
    }


    /**
    * Set the material for the rendering pass of a node
    * @param a_pNode the node to adjust
    * @param a_iMaterial the material to adjust
    * @param a_eMaterial the new material
    */
    void CDustbinShaders::adjustNodeMaterial(irr::scene::IMeshSceneNode* a_pNode, irr::u32 a_iMaterial, irr::video::E_MATERIAL_TYPE a_eMaterial) {
      for (std::vector<SShadowNode>::iterator l_itNode = m_vStatic.begin(); l_itNode != m_vStatic.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          for (std::vector<SShadowNodeMaterial>::iterator l_itMaterial = (*l_itNode).m_vMaterials.begin(); l_itMaterial != (*l_itNode).m_vMaterials.end(); l_itMaterial++) {
            if ((*l_itMaterial).m_iMaterial == a_iMaterial) {
              (*l_itMaterial).m_eRenderM = a_eMaterial;
              return;
            }
          }
        }
      }

      for (std::vector<SShadowNode>::iterator l_itNode = m_vMoving.begin(); l_itNode != m_vMoving.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          for (std::vector<SShadowNodeMaterial>::iterator l_itMaterial = (*l_itNode).m_vMaterials.begin(); l_itMaterial != (*l_itNode).m_vMaterials.end(); l_itMaterial++) {
            if ((*l_itMaterial).m_iMaterial == a_iMaterial) {
              (*l_itMaterial).m_eRenderM = a_eMaterial;
              return;
            }
          }
        }
      }

      for (std::vector<SShadowNode>::iterator l_itNode = m_vMarble.begin(); l_itNode != m_vMarble.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          for (std::vector<SShadowNodeMaterial>::iterator l_itMaterial = (*l_itNode).m_vMaterials.begin(); l_itMaterial != (*l_itNode).m_vMaterials.end(); l_itMaterial++) {
            if ((*l_itMaterial).m_iMaterial == a_iMaterial) {
              (*l_itMaterial).m_eRenderM = a_eMaterial;
              return;
            }
          }
        }
      }
    }

    /**
    * Register a material of a node
    * @param a_pNode the node the material belongs to
    * @param a_iMaterial the index of the material
    * @param a_eNewMaterial the material type to be registered (could also be taken from the scene node)
    * @param a_bCastShadow does this material cast a shadow?
    * @param a_eType the type of node
    */
    void CDustbinShaders::addNodeMaterial(irr::scene::IMeshSceneNode* a_pNode, irr::u32 a_iMaterial, irr::video::E_MATERIAL_TYPE a_eNewMaterial, bool a_bCastShadow, enObjectType a_eType) {
      std::vector<SShadowNode>::iterator l_itNode;

      if (a_eType == enObjectType::Static) {
        l_itNode = m_vStatic.begin();

        // Check whether or not the node of the material is already in the list
        while (l_itNode != m_vStatic.end()) {
          if ((*l_itNode).m_pNode == a_pNode)
            break;

          l_itNode++;
        }

        // Node not found? Add it.
        if (l_itNode == m_vStatic.end()) {
          m_vStatic.push_back(SShadowNode(a_pNode, a_pNode->isVisible()));
          l_itNode = m_vStatic.end();
          l_itNode--;
        }
      }
      else if (a_eType == enObjectType::Moving) {
        l_itNode = m_vMoving.begin();

        while (l_itNode != m_vMoving.end()) {
          if ((*l_itNode).m_pNode == a_pNode)
            break;

          l_itNode++;
        }

        if (l_itNode == m_vMoving.end()) {
          m_vMoving.push_back(SShadowNode(a_pNode, a_pNode->isVisible()));
          l_itNode = m_vMoving.end();
          l_itNode--;
        }
      }
      else if (a_eType == enObjectType::Marble) {
        l_itNode = m_vMarble.begin();

        while (l_itNode != m_vMarble.end()) {
          if ((*l_itNode).m_pNode == a_pNode)
            break;

          l_itNode++;
        }

        if (l_itNode == m_vMarble.end()) {
          m_vMarble.push_back(SShadowNode(a_pNode, a_pNode->isVisible()));
          l_itNode = m_vMarble.end();
          l_itNode--;
        }
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

      std::string s = a_pNode->getName();

      if (l_itMaterial == (*l_itNode).m_vMaterials.end()) {
        // Add the shadow map textures to the material
        a_pNode->getMaterial(a_iMaterial).setTexture(7, m_pRttShadow1[(int)m_eQuality]);
        a_pNode->getMaterial(a_iMaterial).setTexture(6, m_pRttShadow2[(int)m_eQuality]);
        a_pNode->getMaterial(a_iMaterial).setTexture(5, m_pRttShadow3[(int)m_eQuality]);
        a_pNode->getMaterial(a_iMaterial).setTexture(4, m_pRttShadow4[(int)m_eQuality]);

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
    * Delete a node from the list
    * @param a_pNode the node to delete
    */
    void CDustbinShaders::deleteNode(irr::scene::ISceneNode* a_pNode) {
      for (std::vector<SShadowNode>::iterator l_itNode = m_vStatic.begin(); l_itNode != m_vStatic.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          m_vStatic.erase(l_itNode);
          return;
        }
      }

      for (std::vector<SShadowNode>::iterator l_itNode = m_vMoving.begin(); l_itNode != m_vMoving.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          m_vMoving.erase(l_itNode);
          return;
        }
      }

      for (std::vector<SShadowNode>::iterator l_itNode = m_vMarble.begin(); l_itNode != m_vMarble.end(); l_itNode++) {
        if ((*l_itNode).m_pNode == a_pNode) {
          m_vMarble.erase(l_itNode);
          return;
        }
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
      m_vStatic.clear();
      m_vMoving.clear();
      m_vMarble.clear();
      m_pLightCamera = nullptr;
      m_pDataNode    = nullptr;
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
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::Marble    ) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::Wall3     ) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::Wall2     ) ||
        a_eType == (irr::video::E_MATERIAL_TYPE)m_pCallback->getMaterial(enMaterialType::Wall1     )
      ; 
    }

    /**
    * Get the scene manager
    * @return the scene manager
    */
    irr::scene::ISceneManager *CDustbinShaders::getSceneManager() {
      return m_pSmgr;
    }

    /**
    * Fill the "m_pDataNode" member
    * @param a_pNode the node to check
    */
    void CDustbinShaders::fillDataNode(irr::scene::ISceneNode* a_pNode) {
      if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_DustbinLightId) {
        m_pDataNode = reinterpret_cast<scenenodes::CDustbinLight *>(a_pNode);
        return;
      }

      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator l_itChild = a_pNode->getChildren().begin(); l_itChild != a_pNode->getChildren().end(); l_itChild++) {
        fillDataNode(*l_itChild);

        if (m_pDataNode != nullptr)
          return;
      }
    }

    void CDustbinShaders::createLightMatrix() {
      // Make the camera ortographic
      irr::core::matrix4 l_cOrtho;
      l_cOrtho.buildProjectionMatrixOrthoLH(
        m_pDataNode->getFieldOfView(),
        m_pDataNode->getFieldOfView(),
        m_pDataNode->getNearValue(),
        m_pDataNode->getFarValue()
      );
      m_pLightCamera->setProjectionMatrix(l_cOrtho, true);
      m_pLightCamera->setPosition(m_pDataNode->getPosition());
      m_pLightCamera->setTarget(m_pDataNode->getLightTarget());
      m_pLightCamera->updateAbsolutePosition();
    }

    irr::f32 CDustbinShaders::getFieldOfView() {
      return m_pDataNode->getFieldOfView();
    }

    void CDustbinShaders::setFieldOfView(irr::f32 a_fFieldOfView) {
      m_pDataNode->setFieldOfView(a_fFieldOfView);
      createLightMatrix();
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
#ifdef _WINDOWS
      switch (a_eQuality) {
        case enShadowQuality::Top  : return 16384;
        case enShadowQuality::High : return  8192;
        case enShadowQuality::HiMid: return  4096;
        case enShadowQuality::LoMid: return  2048;
        case enShadowQuality::Low  : return  1024;

        default: return 512;
      }
#else
      switch (a_eQuality) {
        case enShadowQuality::Top  : return 4096;
        case enShadowQuality::High : return 2048;
        case enShadowQuality::HiMid: return 1024;
        case enShadowQuality::LoMid: return 512;
        case enShadowQuality::Low  : return 512;

        default: return 512;
      }
#endif
    }
  }
}
