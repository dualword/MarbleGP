// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <scenenodes/CPhysicsNode.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CCheckpointNode
    * @author Christian Keimel
    * This node turns a mesh scene node into a checkpoint. It must be a child of the node
    */
    const int g_CheckpointNodeId = MAKE_IRR_ID('c', 'h', 'p', 'n');
    const irr::c8 g_CheckpointName[] = "CheckpointNode";

    class CCheckpointNode : public CPhysicsNode {
      protected:
        irr::core::aabbox3df m_cBox;

      public:
        std::vector<irr::s32> m_vLinks;   /**< The next checkpoints after this one is passed */
        irr::s32 m_iRespawn;              /**< The respawn node associated with this checkpoint */
        bool m_bFirstInLap;               /**< Flag to define the checkpoint(s) for starting a lap */

      public:
        CCheckpointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CCheckpointNode();

        virtual void render() override;
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const override;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const override;
        virtual void OnRegisterSceneNode() override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) override;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0) override;
    };
  }
}