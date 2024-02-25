// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <scenenodes/CPhysicsNode.h>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CJointNode
    * @author Christian Keimel
    * This node defines a joint for a physical object. It must be a child node of the 
    * physical object it will attach (to the world for now)
    */
    const int g_JointNodeId = MAKE_IRR_ID('j', 'o', 'n', 't');
    const irr::c8 g_JointNodeName[] = "JointNode";

    const irr::c8* const g_JointTypes[] = {
      "Hinge",
      "Slider",
      0
    };

    class CJointNode : public irr::scene::ISceneNode {
      protected:
        irr::core::aabbox3df m_cBox;

      public:
        irr::core::vector3df m_vAxis;         /**< The axis of the joint */
        int                  m_iType;         /**< The type of joint (hinge or slider) */
        irr::f32             m_fHiStop,       /**< The hi-stop of the joint */
                             m_fLoStop,       /**< The lo-stop of the joint */
                             m_fForce,        /**< The force of the joint's motor */
                             m_fVelocity;     /**< The desired velocity of the joint's motor */
        bool                 m_bUseHiStop,    /**< Does the joint have a hi-stop? */
                             m_bUseLoStop,    /**< Does the joint have a lo-stop? */
                             m_bEnableMotor;  /**< Motor enabled? */

      public:
        CJointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CJointNode();

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