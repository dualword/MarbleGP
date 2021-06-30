// (w) 2021 by Dustbin::Games / Christian Keimel

// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactoryEditor::CMarbleGPSceneNodeFactoryEditor(irr::scene::ISceneManager* a_pSmgr) : irr::scene::ISceneNodeFactory(), m_pSmgr(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactoryEditor::~CMarbleGPSceneNodeFactoryEditor() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      return nullptr;
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactoryEditor::addSceneNode(const irr::c8* a_sTypeName, irr::scene::ISceneNode* a_pParent) {
      return nullptr;
    }

    irr::u32 CMarbleGPSceneNodeFactoryEditor::getCreatableSceneNodeTypeCount() const {
      return 2;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const {
      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeType(irr::u32 a_iIdx) const {
      return irr::scene::ESNT_UNKNOWN;
    }

    const irr::c8* CMarbleGPSceneNodeFactoryEditor::getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const {
      return nullptr;
    }

    irr::scene::ESCENE_NODE_TYPE CMarbleGPSceneNodeFactoryEditor::getTypeFromName(const irr::c8* a_sName) {
      return irr::scene::ESNT_UNKNOWN;
    }
  }
}