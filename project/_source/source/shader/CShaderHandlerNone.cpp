// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <shader/CMyShaderNone.h>

namespace dustbin {
  namespace shader {

    CShaderHandlerNone::CShaderHandlerNone(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_cViewportSize) : CShaderHandlerBase(a_pDevice, a_cViewportSize) {
      m_cMainViewport = irr::core::recti(irr::core::vector2di(0, 0), m_pDrv->getScreenSize());
    }

    CShaderHandlerNone::~CShaderHandlerNone() {

    }

    void CShaderHandlerNone::renderScene(const irr::core::recti &a_cViewPort) {
      m_pDrv->setViewPort(a_cViewPort);
      m_pSmgr->drawAll();
      m_pDrv->setViewPort(m_cMainViewport);
  
    }

    void CShaderHandlerNone::removeNode(irr::scene::ISceneNode *a_pNode) {
    }

    void CShaderHandlerNone::addNode(irr::scene::ISceneNode *a_pNode) {
    }

    irr::video::E_MATERIAL_TYPE CShaderHandlerNone::getMaterialType() {
      return irr::video::EMT_SOLID;
    }

  }
}