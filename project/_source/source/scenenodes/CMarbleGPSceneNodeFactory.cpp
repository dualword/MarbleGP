// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <scenenodes/CGui3dSceneNodes.h>
#include <scenenodes/CGui3dItem.h>
#include <scenenodes/CGui3dRoot.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactory::CMarbleGPSceneNodeFactory(irr::scene::ISceneManager* a_pSmgr) : CMarbleGPSceneNodeFactoryEditor(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactory::~CMarbleGPSceneNodeFactory() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactory::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      switch (a_eType) {
        case g_i3dGuiRootID:
          return new CGui3dRoot(a_pParent, m_pSmgr, -1);
          break;

        case g_i3dGuiItemID:
          return new CGui3dItem(a_pParent, m_pSmgr, -1);
          break;

        default:
          return CMarbleGPSceneNodeFactoryEditor::addSceneNode(a_eType, a_pParent);
          break;
      }
    }
  }
}