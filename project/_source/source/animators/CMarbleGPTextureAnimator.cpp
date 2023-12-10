#include <animators/CMarbleGPTextureAnimator.h>
#include <animators/IAnimatorTextureProvider.h>
#include <string>

namespace dustbin {
  namespace animators {
    const irr::c8* const g_aMGPTextureAnimType[] = {
      "Lapcount",
      "Standings",
      0
    };

    CMarbleGPTextureAnimator::CMarbleGPTextureAnimator(irr::scene::ISceneNode* a_pNode) : irr::scene::ISceneNodeAnimator(), m_eType(enAnimatorType::Lapcount), m_iTexture(0), m_pNode(a_pNode) {
      if (a_pNode != nullptr && a_pNode->getType() == irr::scene::ESNT_MESH) {
        a_pNode->getMaterial(0).Lighting = false;
      }
    }

    CMarbleGPTextureAnimator::~CMarbleGPTextureAnimator() {
    }

    irr::scene::ISceneNodeAnimator* CMarbleGPTextureAnimator::createClone(irr::scene::ISceneNode* a_pNode, irr::scene::ISceneManager* a_pMgr) {
      return new CMarbleGPTextureAnimator(m_pNode);
    }

    irr::scene::ESCENE_NODE_ANIMATOR_TYPE CMarbleGPTextureAnimator::getType() const {
      return (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_AnimatorId;
    }

    void CMarbleGPTextureAnimator::animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs) {
      // Nothing to do here, we just set the textures in the constructor,
      // the rest is done from the game state class
    }

    void CMarbleGPTextureAnimator::serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const {
      ISceneNodeAnimator::serializeAttributes(a_pOut, a_pOptions);
      a_pOut->addEnum("Information", (irr::s32)m_eType, g_aMGPTextureAnimType);
      a_pOut->addInt("Texture", m_iTexture);
    }

    void CMarbleGPTextureAnimator::deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions) {
      ISceneNodeAnimator::deserializeAttributes(a_pIn, a_pOptions);
      m_iTexture = a_pIn->getAttributeAsInt("Texture");
      if (a_pIn->existsAttribute("Information")) {
        const irr::c8 *s = a_pIn->getAttributeAsEnumeration("Information");
        std::string l_sType = s;

        for (int i = 0; g_aMGPTextureAnimType[i] != 0; i++) {
          if (l_sType == g_aMGPTextureAnimType[i])
            m_eType = (enAnimatorType)i;
        }
      }

      IAnimatorTextureProvider *l_pProvider = IAnimatorTextureProvider::getTextureProviderInstance();

      if (l_pProvider != nullptr && m_pNode != nullptr) {
        switch (m_eType) {
          case enAnimatorType::Lapcount : if (m_iTexture < m_pNode->getMaterialCount()) m_pNode->getMaterial(m_iTexture).setTexture(0, l_pProvider->getLapCountTexture()); break;
          case enAnimatorType::Standings: if (m_iTexture < m_pNode->getMaterialCount()) m_pNode->getMaterial(m_iTexture).setTexture(0, l_pProvider->getRaceInfoTexture()); break;
        }
        
      }
    }
  }
}