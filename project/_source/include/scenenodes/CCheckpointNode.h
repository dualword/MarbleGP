// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

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

        bool m_bFirstInLap; /**< Flag to define the checkpoint(s) for starting a lap */

      public:
        std::map<irr::s32, std::vector<irr::s32> > m_mLinks;    /**< The links to the next checkpoint. The key defines the previous checkpoint, the vector of ints defines the optional next checkpoints */
        std::vector<int> m_vFinishLapIDs; /**< IDs of the previous checkpoints that trigger a "lap done" if m_bFirstInLap == true. "0" always triggers the next lap */

      public:
        CCheckpointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CCheckpointNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        /**
        * Register a previous node
        * @param a_iId ID of the previous node
        */
        virtual void addPreviousNode(int a_iId);

        /**
        * Is this checkpoint marked as lap start?
        * @return true if the checkpoint is a lap start, false otherwise
        */
        bool isFirstInLap();
    };
  }
}