// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CRespawnNode.h>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CRespawnNode_Editor
    * @author Christian Keimel
    * This is the respawn node for the editor. It adds a
    * mesh to show position and direction whereas the
    * node for the game remains empty
    */
    class CRespawnNode_Editor : public CRespawnNode {
      public:
        CRespawnNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CRespawnNode_Editor();

        virtual void OnRegisterSceneNode();
        irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager);
    };
  }
}