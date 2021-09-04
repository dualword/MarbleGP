// (w) 2021 by Dustbin::Games / Christian Keimel
#include <shader/CShaderHandlerBase.h>
#include <string>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace shader {


    bool CShaderHandlerBase::findLightCamera(irr::scene::ISceneNode *a_pNode) {
      if (a_pNode->getType() == irr::scene::ESNT_CAMERA && std::string(a_pNode->getName()) == "LightCamera") {
        a_pNode->updateAbsolutePosition();

        irr::scene::ICameraSceneNode *l_pNode = reinterpret_cast<irr::scene::ICameraSceneNode *>(a_pNode);

        m_cLightPos = l_pNode->getAbsolutePosition();
        m_cLightTgt = l_pNode->getTarget();
        m_fLightFOV = l_pNode->getFOV();
        return true;
      }

      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
        if (findLightCamera(*it))
          return true;

      return false;
    }

    CShaderHandlerBase::CShaderHandlerBase(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_cViewportSize) {
      m_pDevice = a_pDevice;
      m_pSmgr   = m_pDevice->getSceneManager();
      m_pDrv    = m_pDevice->getVideoDriver(); 

      m_cLightPos = irr::core::vector3df(200.0f, 500.0f, 0.0f);
      m_cLightTgt = irr::core::vector3df(100.0f,   0.0f, 0.0f);

      m_fLightFOV = (irr::f32)(100.0f * M_PI / 180.0f);

      if (!findLightCamera(a_pDevice->getSceneManager()->getRootSceneNode())) {
        irr::scene::ISceneNode *l_pLight = m_pSmgr->getSceneNodeFromName("lightpos");

        if (l_pLight != nullptr) {
          l_pLight->updateAbsolutePosition();
          m_cLightPos = l_pLight->getAbsolutePosition();
          m_cLightTgt = irr::core::vector3df(0.0f, 0.0f, 0.0f);
        }
      }
    }

    CShaderHandlerBase::~CShaderHandlerBase() {

    }

    void CShaderHandlerBase::initialize() {
      // Nothing to do here .. implement if necessary
    }

    void CShaderHandlerBase::beginScene() {
      // Nothing to do here .. implement if necessary
    }

    void CShaderHandlerBase::getLightData(irr::core::vector3df &a_cLightPos, irr::core::vector3df &a_cLightTarget, irr::f32 &a_fLightFOV) {
      a_cLightPos    = m_cLightPos;
      a_cLightTarget = m_cLightTgt;
      a_fLightFOV    = m_fLightFOV;
    }

    void CShaderHandlerBase::setLightData(const irr::core::vector3df &a_cLightPos, const irr::core::vector3df &a_cLightTarget, irr::f32 a_fLightFOV) {
      m_cLightPos = a_cLightPos;
      m_cLightTgt = a_cLightTarget;
      m_fLightFOV = a_fLightFOV;
    }

  }
}