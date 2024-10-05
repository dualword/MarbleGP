/// (w) 2024 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <animators/CMyTextureAnimator.h>
#include <string>

namespace dustbin {
  namespace animators {

    CMyTextureAnimator::CMyTextureAnimator(irr::scene::ISceneNode *a_pNode,irr::video::IVideoDriver *a_pDrv) : m_iTexture(-1), m_iLastTick(0), m_pNode(a_pNode), m_pDrv(a_pDrv) {
    }

    CMyTextureAnimator::~CMyTextureAnimator() {
    }

    void CMyTextureAnimator::animateNode(irr::scene::ISceneNode* a_pNode, irr::u32 a_iTimeMs) {
      if (IsEnabled && m_vTextures.size() > 0) {
        if (m_pNode == nullptr)
          m_pNode = a_pNode;

        if ((int)a_iTimeMs - m_iLastTick > std::get<2>(*m_itTexture)) {
          m_itTexture++;

          if (m_itTexture == m_vTextures.end())
            m_itTexture = m_vTextures.begin();

          m_iLastTick = a_iTimeMs;

          if (m_pNode != nullptr && m_pNode->getMaterialCount() > 0 && m_pNode->getMaterialCount() >= (irr::u32)m_iTexture && std::get<1>(*m_itTexture) != nullptr)
            m_pNode->getMaterial(m_iTexture).setTexture(0, std::get<1>(*m_itTexture));
        }
      }
    }

    irr::scene::ISceneNodeAnimator* CMyTextureAnimator::createClone(irr::scene::ISceneNode* a_pNode, irr::scene::ISceneManager* a_pNewManager) {
      return new CMyTextureAnimator(a_pNode, m_pDrv);
    }

    irr::scene::ESCENE_NODE_ANIMATOR_TYPE CMyTextureAnimator::getType() const {
      return (irr::scene::ESCENE_NODE_ANIMATOR_TYPE)g_TextureAnimatorId;
    }

    bool CMyTextureAnimator::hasFinished() const {
      return false;
    }

    bool CMyTextureAnimator::isEventReceiverEnabled() const {
      return false;
    }

    void CMyTextureAnimator::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      if (a_pIn->existsAttribute("TextureIndex"))
        m_iTexture = a_pIn->getAttributeAsInt("TextureIndex");

      int l_iIndex = 0;
      std::string l_sName1 = "Texture_" + std::to_string(l_iIndex);
      std::string l_sName2 = "Timeout_" + std::to_string(l_iIndex);

      m_vTextures.clear();

      while (a_pIn->existsAttribute(l_sName1.c_str()) && a_pIn->existsAttribute(l_sName2.c_str())) {
        std::string l_sTexture = a_pIn->getAttributeAsString(l_sName1.c_str()).c_str();
        int         l_iTimeout = a_pIn->getAttributeAsInt   (l_sName2.c_str());

        if (l_sTexture != "" && l_iTimeout > 0) {
          m_vTextures.push_back(std::make_tuple(l_sTexture, m_pDrv->getTexture(l_sTexture.c_str()), l_iTimeout));
        }

        l_iIndex++;

        l_sName1 = "Texture_" + std::to_string(l_iIndex);
        l_sName2 = "Timeout_" + std::to_string(l_iIndex);
      }

      m_itTexture = m_vTextures.begin();
    }

    void CMyTextureAnimator::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      a_pOut->addInt("TextureIndex", m_iTexture);

      int l_iIndex = 0;
      for (auto& l_itTexture : m_vTextures) {
        if (std::get<0>(l_itTexture) != "" && std::get<2>(l_itTexture) > 0) {
          std::string l_sName = "Texture_" + std::to_string(l_iIndex);
          a_pOut->addString(l_sName.c_str(), std::get<0>(l_itTexture).c_str());
          l_sName = "Timeout_" + std::to_string(l_iIndex);
          a_pOut->addInt(l_sName.c_str(), std::get<2>(l_itTexture));

          l_iIndex++;
        }
      }

      std::string l_sName = "Texture_" + std::to_string(l_iIndex);
      a_pOut->addString(l_sName.c_str(), "");
      l_sName = "Timeout_" + std::to_string(l_iIndex);
      a_pOut->addInt(l_sName.c_str(), 500);
    }
  }
}