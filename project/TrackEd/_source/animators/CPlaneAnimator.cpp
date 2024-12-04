// (w) 2024 by Dustbin::Games / Christian Keimel
#include <animators/CPlaneAnimator.h>

namespace dustbin {
  namespace animators {
    CPlaneAnimator::CPlaneAnimator(irr::scene::ISceneNode *a_pNode) : irr::scene::ISceneNodeAnimator() {
    }

    CPlaneAnimator::~CPlaneAnimator() {
    }

    irr::scene::ISceneNodeAnimator *CPlaneAnimator::createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pMgr) {
      return new CPlaneAnimator(a_pNode);
    }

    irr::scene::ESCENE_NODE_ANIMATOR_TYPE CPlaneAnimator::getType() const {
      return (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_PlaneAnimatorId;
    }

    void CPlaneAnimator::animateNode(irr::scene::ISceneNode* a_pNode, irr::u32 a_iTimeMs) {
      irr::core::vector3df l_cCamPos = a_pNode->getSceneManager()->getActiveCamera()->getAbsolutePosition();
      
      irr::core::vector3df l_cNewPos = irr::core::vector3df(
        300.0f * (irr::f32)((irr::s32)(l_cCamPos.X / 300.0f)),
        0.0f,
        300.0f * (irr::f32)((irr::s32)(l_cCamPos.Z / 300.0f))
      );

      a_pNode->setPosition(l_cNewPos);
    }
  }
}