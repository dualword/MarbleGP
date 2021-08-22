// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>
#include <scenenodes/CStartingGridSceneNode.h>
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
          return new CStartingGridSceneNode_Editor(a_pParent != nullptr ? a_pParent : m_pSmgr->getRootSceneNode(), m_pSmgr, -1);
          break;

        default:
          return nullptr;
          break;
      }
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      return addSceneNode(getTypeFromName(a_sTypeName));
    }

    irr::u32 CMarbleGPSceneNodeFactoryEditor::getCreatableSceneNodeTypeCount() const {
      return 1;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0:
          return g_StartingGridScenenodeName;
          break;
      }

      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      switch (a_iIdx) {
        case 0:
          return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
          break;
      }

      return irr::scene::ESNT_UNKNOWN;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      switch (a_eType) {
        case g_StartingGridScenenodeId:
          return g_StartingGridScenenodeName;
          break;

        default:
          return nullptr;
          break;
      }
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getTypeFromName(const irr::c8* a_sName) {
      std::string l_sName = a_sName;

      if (l_sName == g_StartingGridScenenodeName)
        return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;

      return irr::scene::ESNT_UNKNOWN;
    }
  }
}