// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>
#include <scenenodes/CStartingGridSceneNode_Editor.h>
#include <scenenodes/CCheckpointNode_Editor.h>
#include <scenenodes/CRespawnNode_Editor.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CJointNode.h>
#include <scenenodes/CWorldNode.h>
#include <scenenodes/CDustbinId.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactoryEditor::CMarbleGPSceneNodeFactoryEditor(irr::scene::ISceneManager* a_pSmgr) : irr::scene::ISceneNodeFactory(), m_pSmgr(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactoryEditor::~CMarbleGPSceneNodeFactoryEditor() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      irr::scene::ISceneNode* p = nullptr;

      switch (a_eType) {
        case g_StartingGridScenenodeId:
          p = new CStartingGridSceneNode_Editor(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_WorldNodeId:
          p = new CWorldNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_PhysicsNodeId:
          p = new CPhysicsNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_CheckpointNodeId:
          p = new CCheckpointNode_Editor(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_RespawnNodeId:
          p = new CRespawnNode_Editor(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_JointNodeId:
          p = new CJointNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        default:
          break;
      }

      if (p != nullptr)
        p->drop();

      return p;
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      return addSceneNode(getTypeFromName(a_sTypeName), a_pParent);
    }

    irr::u32 CMarbleGPSceneNodeFactoryEditor::getCreatableSceneNodeTypeCount() const {
      return 6;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0: return g_StartingGridScenenodeName;
        case 1: return g_WorldName;
        case 2: return g_PhysicsNodeName;
        case 3: return g_CheckpointName;
        case 4: return g_RespawndName;
        case 5: return g_JointNodeName;
      }

      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0: return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
        case 1: return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
        case 2: return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
        case 3: return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
        case 4: return (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId;
        case 5: return (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId;
      }

      return irr::scene::ESNT_UNKNOWN;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      switch (a_eType) {
        case g_StartingGridScenenodeId:
          return g_StartingGridScenenodeName;

        case g_WorldNodeId:
          return g_WorldName;

        case g_PhysicsNodeId:
          return g_PhysicsNodeName;

        case g_CheckpointNodeId:
          return g_CheckpointName;

        case g_RespawnNodeId:
          return g_RespawndName;

        case g_JointNodeId:
          return g_JointNodeName;

        default:
          return nullptr;
      }
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getTypeFromName(const irr::c8* a_sName) {
      std::string l_sName = a_sName;

      if (l_sName == g_StartingGridScenenodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
      else if (l_sName == g_WorldName)
        return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
      else if (l_sName == g_PhysicsNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
      else if (l_sName == g_CheckpointName)
        return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
      else if (l_sName == g_RespawndName)
        return (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId;
      else if (l_sName == g_JointNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId;

      return irr::scene::ESNT_UNKNOWN;
    }
  }
}