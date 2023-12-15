// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CDustbinCamera.h>
#include <scenenodes/CDustbinLight.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CRespawnNode.h>
#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CAiPathNode.h>
#include <scenenodes/CWorldNode.h>
#include <scenenodes/CJointNode.h>
#include <scenenodes/CDustbinId.h>
#include <scenenodes/CSpeedNode.h>
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

        case (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId:
          p = new CAiNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_DustbinCameraId:
          p = new CDustbinCamera(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_RostrumNodeId:
          p = new CRostrumNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_AiPathNodeId:
          p = new CAiPathNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_SpeedNodeId:
          p = new CSpeedNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        case (irr::scene::ESCENE_NODE_TYPE)g_DustbinLightId:
          p = new CDustbinLight(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
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
      return 12;
    }

    const irr::c8* CMarbleGPSceneNodeFactory::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case  0: return g_StartingGridScenenodeName;
        case  1: return g_WorldName;
        case  2: return g_PhysicsNodeName;
        case  3: return g_CheckpointName;
        case  4: return g_RespawndName;
        case  5: return g_JointNodeName;
        case  6: return g_AiNodeName;
        case  7: return g_DustbinCameraNodeName;
        case  8: return g_RostrumNodeName;
        case  9: return g_AiPathNodeName;
        case 10: return g_SpeedNodeName;
        case 11: return g_DustbinLightName;
      }

      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactory::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case  0: return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
        case  1: return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
        case  2: return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
        case  3: return (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId;
        case  4: return (irr::scene::ESCENE_NODE_TYPE)g_RespawnNodeId;
        case  5: return (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId;
        case  6: return (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId;
        case  7: return (irr::scene::ESCENE_NODE_TYPE)g_DustbinCameraId;
        case  8: return (irr::scene::ESCENE_NODE_TYPE)g_RostrumNodeId;
        case  9: return (irr::scene::ESCENE_NODE_TYPE)g_AiPathNodeId;
        case 10: return (irr::scene::ESCENE_NODE_TYPE)g_SpeedNodeId;
        case 11: return (irr::scene::ESCENE_NODE_TYPE)g_DustbinLightId;
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

        case (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId:
          return g_AiNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_DustbinCameraId:
          return g_DustbinCameraNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_RostrumNodeId:
          return g_RostrumNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_AiPathNodeId:
          return g_AiPathNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_SpeedNodeId:
          return g_SpeedNodeName;

        case (irr::scene::ESCENE_NODE_TYPE)g_DustbinLightId:
          return g_DustbinLightName;

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
      else if (l_sName == g_AiNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_AiNodeId;
      else if (l_sName == g_DustbinCameraNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_DustbinCameraId;
      else if (l_sName == g_RostrumNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_RostrumNodeId;
      else if (l_sName == g_AiPathNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_AiPathNodeId;
      else if (l_sName == g_SpeedNodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_SpeedNodeId;
      else if (l_sName == g_DustbinLightName)
        return (irr::scene::ESCENE_NODE_TYPE)g_DustbinLightId;

      return irr::scene::ESNT_UNKNOWN;
    }
  }
}