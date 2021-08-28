// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>
#include <scenenodes/CStartingGridSceneNode_Editor.h>
#include <scenenodes/CPhysicsNode.h>
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
      switch (a_eType) {
        case g_StartingGridScenenodeId:
          return new CStartingGridSceneNode_Editor(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_WorldNodeId:
          return new CWorldNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());
          break;

        case g_PhysicsNodeId:
          return new CPhysicsNode(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, getNextSceneNodeId());

        default:
          return nullptr;
          break;
      }
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      return addSceneNode(getTypeFromName(a_sTypeName), a_pParent);
    }

    irr::u32 CMarbleGPSceneNodeFactoryEditor::getCreatableSceneNodeTypeCount() const {
      return 3;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0:
          return g_StartingGridScenenodeName;
          break;
          
        case 1:
          return g_WorldName;
          break;

        case 2:
          return g_PhysicsNodeName;
          break;
      }

      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0:
          return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
          break;

        case 1:
          return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
          break;

        case 2:
          return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
      }

      return irr::scene::ESNT_UNKNOWN;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      switch (a_eType) {
        case g_StartingGridScenenodeId:
          return g_StartingGridScenenodeName;
          break;

        case g_WorldNodeId:
          return g_WorldName;
          break;

        case g_PhysicsNodeId:
          return g_PhysicsNodeName;

        default:
          return nullptr;
          break;
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

      return irr::scene::ESNT_UNKNOWN;
    }
  }
}