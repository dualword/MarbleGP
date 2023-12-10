// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinId.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CCheckpointNode::CCheckpointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CPhysicsNode(a_pParent, a_pMgr, a_iId),
      m_bFirstInLap(false)
    {
      m_cBox.reset(getPosition());

      setScale(irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      sceneNodeIdUsed(a_iId);

      if (Parent != nullptr && Parent != a_pMgr->getRootSceneNode() && Parent->getType() == irr::scene::ESNT_MESH) {
        Parent->getMaterial(0).Lighting = false;
      }
    }

    CCheckpointNode::~CCheckpointNode() {
    }

    void CCheckpointNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CCheckpointNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CCheckpointNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
    }

    void CCheckpointNode::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CCheckpointNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("FirstInLap", m_bFirstInLap);

      int l_iNext = 1;

      for (std::vector<irr::s32>::const_iterator it = m_vLinks.begin(); it != m_vLinks.end(); it++) {
        std::string l_sName = "Next_" + std::to_string(l_iNext);
        a_pOut->addInt(l_sName.c_str(), *it);
        l_iNext++;
      }

      a_pOut->addInt((std::string("Next_") + std::to_string(l_iNext)).c_str(), -1);

      a_pOut->addInt("Respawn", m_iRespawn);
    }

    void CCheckpointNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      setScale(irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      if (a_pIn->existsAttribute("FirstInLap"))
        m_bFirstInLap = a_pIn->getAttributeAsBool("FirstInLap");

      
      int l_iNext = 1;

      m_vLinks.clear();

      while (true) {
        std::string l_sName = "Next_" + std::to_string(l_iNext++);

        if (a_pIn->existsAttribute(l_sName.c_str())) {
          int l_iId = a_pIn->getAttributeAsInt(l_sName.c_str());
          if (l_iId != -1) {
            m_vLinks.push_back(l_iId);
          }
        }
        else break;
      }

      m_iRespawn = a_pIn->getAttributeAsInt("Respawn");

      sceneNodeIdUsed(getID());
    }

    irr::scene::ISceneNode* CCheckpointNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent == nullptr) a_pNewParent = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CCheckpointNode* l_pNew = new CCheckpointNode(a_pNewParent, a_pNewManager, getNextSceneNodeId());

      l_pNew->m_bFirstInLap = m_bFirstInLap;

      return l_pNew;
    }
  }
}