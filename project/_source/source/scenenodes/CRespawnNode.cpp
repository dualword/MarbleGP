// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <scenenodes/CRespawnNode.h>
#include <scenenodes/CDustbinId.h>

namespace dustbin {
  namespace scenenodes {
    CRespawnNode::CRespawnNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId) {
      sceneNodeIdUsed(a_iId);
    }

    CRespawnNode::~CRespawnNode() {

    }

    void CRespawnNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CRespawnNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CRespawnNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId;
    }

    irr::scene::ISceneNode* CRespawnNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent == nullptr) a_pNewParent = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CRespawnNode* l_pNew = new CRespawnNode(a_pNewParent, a_pNewManager, getNextSceneNodeId());

      l_pNew->setPosition(getPosition());
      l_pNew->setRotation(getRotation());
      l_pNew->setScale(getScale());

      return l_pNew;
    }
  }
}
