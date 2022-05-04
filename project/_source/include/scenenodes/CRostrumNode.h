// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    const int g_RostrumNodeId = MAKE_IRR_ID('r', 's', 't', 'r');
    const irr::c8 g_RostrumNodeName[] = "RostrumNode";

    /**
    * @class CPhysicsNode
    * @author Christian Keimel
    * This node holds the data for a physical object. It must be a child node of a Mesh Scene Node to work properly
    */
    class CRostrumNode : public irr::scene::ISceneNode {
      private:
        irr::core::aabbox3df m_cBox;

      public:
        CRostrumNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CRostrumNode();

        //*** Virtual method inherited from irr::scene::ISceneNode
        virtual irr::u32 getMaterialCount() const override;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const override;
        virtual void OnRegisterSceneNode() override;

        virtual void render() override;
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) override;

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0) override;

        irr::core::vector3df getRostrumPosition(int a_iPosition);
        irr::core::vector3df getCameraPosition();
    };
  }
}
