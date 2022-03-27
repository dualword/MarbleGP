// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CMarbleCountSceneNode.h>
#include <scenenodes/CTriggerTimerNode.h>
#include <scenenodes/CMarbleTouchNode.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CRespawnNode.h>
#include <scenenodes/CWorldNode.h>
#include <scenenodes/CJointNode.h>
#include <scenenodes/CDustbinId.h>
#include <scenenodes/CAiNode.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactory::CMarbleGPSceneNodeFactory(irr::scene::ISceneManager* a_pSmgr) : irr::scene::ISceneNodeFactory(), m_pSmgr(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactory::~CMarbleGPSceneNodeFactory() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactory::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      irr::scene::ISceneNode* p = nullptr;

      switch (a_eType) {
        case (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId:
          p = new CStartingGridSceneNode(a_pParent, m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId:
          p = new CCheckpointNode(a_pParent, m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId:
          p = new CRespawnNode(a_pParent, m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId:
          p = new CWorldNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId:
          p = new CPhysicsNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId:
          p = new CJointNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_TrigerTimerNodeId:
          p = new CTriggerTimeNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_MarbleCountNodeId:
          p = new CMarbleCountSceneNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_MarbleTouchNodeId:
          p = new CMarbleTouchNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId:
          p = new CAiNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        default:
          break;
      }

      if (p != nullptr)
        p->drop();
      
      return p;
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactory::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      return addSceneNode(getTypeFromName(a_sTypeName), a_pParent);
    }

    irr::u32 CMarbleGPSceneNodeFactory::getCreatableSceneNodeTypeCount() const {
      return 10;
    }

    const irr::c8* CMarbleGPSceneNodeFactory::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0: return g_StartingGridScenenodeName;
        case 1: return g_WorldName;
        case 2: return g_PhysicsNodeName;
        case 3: return g_CheckpointName;
        case 4: return g_RespawndName;
        case 5: return g_JointNodeName;
        case 6: return g_TriggerTimerNodeName;
        case 7: return g_MarbleCountNodeName;
        case 8: return g_MarbleTouchNodeName;
        case 9: return g_AiNodeName;
      }

      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactory::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0: return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
        case 1: return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
        case 2: return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
        case 3: return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
        case 4: return (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId;
        case 5: return (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId;
        case 6: return (irr::scene::ESCENE_NODE_TYPE)g_TrigerTimerNodeId;
        case 7: return (irr::scene::ESCENE_NODE_TYPE)g_MarbleCountNodeId;
        case 8: return (irr::scene::ESCENE_NODE_TYPE)g_MarbleTouchNodeId;
        case 9: return (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId;
      }

      return irr::scene::ESNT_UNKNOWN;
    }

    const irr::c8* CMarbleGPSceneNodeFactory::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      switch (a_eType) {
        case (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId:
          return g_StartingGridScenenodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId:
          return g_WorldName;

        case (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId:
          return g_PhysicsNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId:
          return g_CheckpointName;

        case (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId:
          return g_RespawndName;

        case (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId:
          return g_JointNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_TrigerTimerNodeId:
          return g_TriggerTimerNodeName;
          
        case (irr::scene::ESCENE_NODE_TYPE)g_MarbleCountNodeId:
          return g_MarbleCountNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_MarbleTouchNodeId:
          return g_MarbleTouchNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId:
          return g_AiNodeName;

        default:
          return nullptr;
      }
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactory::getTypeFromName(const irr::c8* a_sName) {
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
      else if (l_sName == g_TriggerTimerNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_TrigerTimerNodeId;
      else if (l_sName == g_MarbleCountNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_MarbleCountNodeId;
      else if (l_sName == g_MarbleTouchNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_MarbleTouchNodeId;
      else if (l_sName == g_AiNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId;

      return irr::scene::ESNT_UNKNOWN;
    }
  }
}