// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <shader/CShaderHandlerXEffect.h>

namespace dustbin {
  namespace shader {


    CShaderHandlerXEffect::CShaderHandlerXEffect(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient) : CShaderHandlerBase(a_pDevice, a_cViewportSize) {
      m_pEffect = new EffectHandler(m_pDevice, a_cViewportSize, true);

      m_iShadowSize = a_iShadowSize;
      m_iAmbient    = a_iAmbient;
    }

    CShaderHandlerXEffect::~CShaderHandlerXEffect() {
      if (m_pEffect != nullptr) delete m_pEffect;
    }

    void CShaderHandlerXEffect::addToShadow(irr::scene::ISceneNode *a_pNode) {
      if (a_pNode->isVisible()) {
        if (a_pNode->getType() == irr::scene::ESNT_MESH) {
          printf("Add node \"%s\" to shader\n", a_pNode->getName());
          m_pEffect->addShadowToNode(a_pNode, EFT_16PCF);

          irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(a_pNode);
          irr::scene::IMeshManipulator *l_pManipulator = m_pSmgr->getMeshManipulator();
          l_pManipulator->recalculateNormals(l_pNode->getMesh(), true);
        }
        else if (a_pNode->getType() == irr::scene::ESNT_LIGHT) {
          irr::scene::ILightSceneNode *p = reinterpret_cast<irr::scene::ILightSceneNode *>(a_pNode);
          p->updateAbsolutePosition();
          irr::core::vector3df v = p->getAbsolutePosition(), d = p->getRotation().rotationToDirection(irr::core::vector3df(0.0f, 0.0f, 1.0f));

          m_pEffect->addShadowLight(SShadowLight(m_iShadowSize, v, v + p->getRadius() * d, irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), 1, p->getRadius(), p->getLightData().OuterCone, false));
        }

        for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
          addToShadow(*it);
        }
      }
    }

    void CShaderHandlerXEffect::initialize() {
      m_pEffect->setClearColour(irr::video::SColor(255, 250, 100, 0));

      m_pEffect->addPostProcessingEffectFromFile(irr::core::stringc("shaders/BrightPass.glsl"));
      m_pEffect->addPostProcessingEffectFromFile(irr::core::stringc("shaders/BlurHP.glsl"));
      m_pEffect->addPostProcessingEffectFromFile(irr::core::stringc("shaders/BlurVP.glsl"));
      m_pEffect->addPostProcessingEffectFromFile(irr::core::stringc("shaders/BloomP.glsl"));

      m_pEffect->setAmbientColor(irr::video::SColor(255, m_iAmbient, m_iAmbient, m_iAmbient));

      addToShadow(m_pSmgr->getRootSceneNode());
    }

    void CShaderHandlerXEffect::renderScene(const irr::core::recti &a_cViewPort) {
      m_pEffect->update();
    }

    void CShaderHandlerXEffect::removeNode(irr::scene::ISceneNode *a_pNode) {
      removeChildren(a_pNode);
    }

    void CShaderHandlerXEffect::addNode(irr::scene::ISceneNode *a_pNode) {
      addToShadow(a_pNode);
    }

    irr::video::E_MATERIAL_TYPE CShaderHandlerXEffect::getMaterialType() {
      return irr::video::EMT_SOLID;
    }

    void CShaderHandlerXEffect::removeChildren(irr::scene::ISceneNode *a_pNode) {
      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
        removeChildren(*it);
      }
      m_pEffect->removeNodeFromDepthPass(a_pNode);
      m_pEffect->removeShadowFromNode(a_pNode);
    }

    void CShaderHandlerXEffect::setLightData(const irr::core::vector3df &a_cLightPos, const irr::core::vector3df &a_cLightTarget, irr::f32 a_fLightFOV) {
      CShaderHandlerBase::setLightData(a_cLightPos, a_cLightTarget, a_fLightFOV);

      if (m_pEffect->getShadowLightCount() > 0) {
        irr::core::matrix4 l_cProjMat;

        l_cProjMat.buildProjectionMatrixPerspectiveFovLH(a_fLightFOV, 1.0f, 110.0f, 1500.0f);

        m_pEffect->getShadowLight(0).setPosition(a_cLightPos   );
        m_pEffect->getShadowLight(0).setTarget  (a_cLightTarget);
        m_pEffect->getShadowLight(0).setProjectionMatrix(l_cProjMat);
      }
    }

  }
}