// (w) 2021 by Dustbin::Games / Christian Keimel

// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>
#include <scenenodes/CGui3dSceneNodes.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactoryEditor::CMarbleGPSceneNodeFactoryEditor(irr::scene::ISceneManager* a_pSmgr) : irr::scene::ISceneNodeFactory(), m_pSmgr(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactoryEditor::~CMarbleGPSceneNodeFactoryEditor() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      switch (a_eType) {
        case g_i3dGuiRootID:
          return new CGui3dRootBase(a_pParent, m_pSmgr, -1);
          break;

        case g_i3dGuiItemID:
          return new CGui3dItemBase(a_pParent, m_pSmgr, -1);
          break;

        default:
          return nullptr;
          break;
      }
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      irr::scene::ESCENE_NODE_TYPE l_eType = getTypeFromName(a_sTypeName);

      if (l_eType != irr::scene::ESNT_UNKNOWN) {
        return addSceneNode(l_eType, a_pParent);
      }
      else return nullptr;
    }

    irr::u32 CMarbleGPSceneNodeFactoryEditor::getCreatableSceneNodeTypeCount() const {
      return 2;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
      case 0:
        return dustbin::scenenodes::CGui3dRootBase::getNodeTypeName().c_str();
        break;

      case 1:
        return dustbin::scenenodes::CGui3dItemBase::getNodeTypeName().c_str();
        break;

      default:
        return nullptr;
        break;
      }
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
      case 0:
        return dustbin::scenenodes::CGui3dRootBase::getNodeType();
        break;

      case 1:
        return dustbin::scenenodes::CGui3dItemBase::getNodeType();
        break;

      default:
        return irr::scene::ESNT_UNKNOWN;
        break;
      }
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      switch (a_eType) {
      case g_i3dGuiRootID:
        return dustbin::scenenodes::CGui3dRootBase::getNodeTypeName().c_str();
        break;

      case g_i3dGuiItemID:
        return dustbin::scenenodes::CGui3dItemBase::getNodeTypeName().c_str();
        break;

      default:
        return nullptr;
        break;
      }
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getTypeFromName(const irr::c8* a_sName) {
      std::string l_sName = a_sName;

      if (l_sName == dustbin::scenenodes::CGui3dRootBase::getNodeTypeName()) {
        return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiRootID;
      }
      else if (l_sName == dustbin::scenenodes::CGui3dItemBase::getNodeTypeName()) {
        return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiItemID;
      }
      else return irr::scene::ESNT_UNKNOWN;
    }
  }
}