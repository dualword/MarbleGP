#include <animators/CMarbleGPTextureAnimator.h>
#include <animators/CMyTextureAnimator.h>
#include <animators/CAnimatorFactory.h>

namespace dustbin {
  namespace animators {
    CAnimatorFactory::CAnimatorFactory(irr::IrrlichtDevice *a_pDevice) : m_pDevice(a_pDevice) {
      m_mNameType["MarbleGpTexture"       ] = (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_AnimatorId;
      m_mNameType["DustbinTextureAnimator"] = (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_TextureAnimatorId;

      m_mTypeName[(irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_AnimatorId       ] = "MarbleGpTexture";
      m_mTypeName[(irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_TextureAnimatorId] = "DustbinTextureAnimator";
    }

    CAnimatorFactory::~CAnimatorFactory() {
    }

    irr::scene::ISceneNodeAnimator *CAnimatorFactory::createSceneNodeAnimator(irr::scene::ESCENE_NODE_ANIMATOR_TYPE a_eType, irr::scene::ISceneNode* a_pTarget) {
      irr::scene::ISceneNodeAnimator *l_pRet = nullptr;

      switch (a_eType) {
        case (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_AnimatorId       : l_pRet = new CMarbleGPTextureAnimator(a_pTarget                             ); break;
        case (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_TextureAnimatorId: l_pRet = new CMyTextureAnimator      (a_pTarget, m_pDevice->getVideoDriver()); break;
        default: break;
      }

      if (l_pRet != nullptr && a_pTarget != nullptr)
        a_pTarget->addAnimator(l_pRet);

      return l_pRet;
    }

    irr::scene::ISceneNodeAnimator* CAnimatorFactory::createSceneNodeAnimator(const irr::c8 *a_sTypeName, irr::scene::ISceneNode *a_pTarget) {
      if (m_mNameType.find(a_sTypeName) != m_mNameType.end()) {
        return createSceneNodeAnimator((irr::scene::ESCENE_NODE_ANIMATOR_TYPE)m_mNameType[a_sTypeName], a_pTarget);
      }
      else return nullptr;
    }

    irr::u32 CAnimatorFactory::getCreatableSceneNodeAnimatorTypeCount() const {
      return 2;
    }

    irr::scene::ESCENE_NODE_ANIMATOR_TYPE CAnimatorFactory::getCreateableSceneNodeAnimatorType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0:
          return (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_AnimatorId;
          break;

        case 1:
          return (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_TextureAnimatorId;
          break;

        default:
          return irr::scene::ESNAT_UNKNOWN;
      }
    }

    const irr::c8* CAnimatorFactory::getCreateableSceneNodeAnimatorTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0: return m_mTypeName.at(g_AnimatorId       ).c_str();
        case 1: return m_mTypeName.at(g_TextureAnimatorId).c_str();
      }

      return nullptr;
    }

    const irr::c8* CAnimatorFactory::getCreateableSceneNodeAnimatorTypeName(irr::scene::ESCENE_NODE_ANIMATOR_TYPE a_eType) const {
      if (m_mTypeName.find(a_eType) != m_mTypeName.end())
        return m_mTypeName.at(a_eType).c_str();
      else
        return nullptr;
    }
  }
}