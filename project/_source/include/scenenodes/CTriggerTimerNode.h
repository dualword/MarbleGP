/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <scenenodes/STriggerAction.h>
#include <irrlicht.h>
#include <vector>
#include <string>

namespace dustbin {
  namespace scenenodes {
    const int g_TrigerTimerNodeId = MAKE_IRR_ID('t', 't', 's', 'n');
    const irr::c8 g_TriggerTimerNodeName[] = "TriggerTimer";

    /**
    * @class CTriggerTimeNode
    * @authod Christian Keimel
    * This node is used to define timer-based actions on a racetrack
    */
    class CTriggerTimeNode : public irr::scene::ISceneNode {
      public:
        std::vector<STriggerAction> m_vActions;
        irr::core::aabbox3df m_cBox;

        CTriggerTimeNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CTriggerTimeNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        virtual void OnRegisterSceneNode();

        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
    };
  }
}