// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CCheckpointNode.h>
#include <map>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CCheckpointNode_editor
    * @author Christian Keimel
    * This is the editor version of the Checkpoint Scenenode
    */
    class CCheckpointNode_Editor : public CCheckpointNode {
      protected:
        std::map<irr::s32, std::vector<irr::scene::ISceneNode*>> m_mLinkMeshes;

        int m_iShowLinks;   /**< Show the links this checkpoint has when this node was passed before */

      public:
        CCheckpointNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CCheckpointNode_Editor();

        virtual void render();

        virtual void OnRegisterSceneNode();

        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;

        /**
        * Remove a previous node
        * @param a_iId ID of the previous node
        */
        virtual void removePreviousNode(int a_iId);
    };
  }
}