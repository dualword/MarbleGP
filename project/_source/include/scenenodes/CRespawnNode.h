// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace scenenodes {
    const int g_RespawnNodeId = MAKE_IRR_ID('r', 's', 'p', 'n');
    const irr::c8 g_RespawndName[] = "RespawnNode";

    /**
    * @class CRespawnNode
    * @author Christian Keimel
    * This node defines respawn nodes for MarbleGP
    */
    class CRespawnNode : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df m_cBox;

      public:
        CRespawnNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CRespawnNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);
    };
  }
}
