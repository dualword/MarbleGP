// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactory::CMarbleGPSceneNodeFactory(irr::scene::ISceneManager* a_pSmgr) : CMarbleGPSceneNodeFactoryEditor(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactory::~CMarbleGPSceneNodeFactory() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactory::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      return nullptr;
    }
  }
}