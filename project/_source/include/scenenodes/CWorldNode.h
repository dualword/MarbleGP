// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    const int g_WorldNodeId = MAKE_IRR_ID('w', 'r', 'l', 'd');
    const irr::c8 g_WorldName[] = "WorldNode";

    class CWorldNode : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df m_cBox;

      public:
        CWorldNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CWorldNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);
    };
  }
}
