// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CRespawnNode.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CMarbleGPSceneNodeFactory::CMarbleGPSceneNodeFactory(irr::scene::ISceneManager* a_pSmgr) : CMarbleGPSceneNodeFactoryEditor(a_pSmgr) {
    }

    CMarbleGPSceneNodeFactory::~CMarbleGPSceneNodeFactory() {
    }

    irr::scene::ISceneNode* CMarbleGPSceneNodeFactory::addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode* a_pParent) {
      irr::scene::ISceneNode* p = nullptr;

      switch (a_eType) {
        case g_StartingGridScenenodeId:
          p = new CStartingGridSceneNode(a_pParent, m_pSmgr, -1);
          break;

        case g_CheckpointNodeId:
          p = new CCheckpointNode(a_pParent, m_pSmgr, -1);
          break;

        case g_RespawnNodeId:
          p = new CRespawnNode(a_pParent, m_pSmgr, -1);
          break;

        default:
          // We exit directly if we create a scennode from the parent object
          // as the parent factory drops the scene node, and dropping twice is
          // not a good idea
          return CMarbleGPSceneNodeFactoryEditor::addSceneNode(a_eType, a_pParent);
      }

      if (p != nullptr)
        p->drop();
      
      return p;
    }
  }
}