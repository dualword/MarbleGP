// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace scenenodes {
    const int g_PhysicsNodeId = MAKE_IRR_ID('p', 'h', 'y', 's');
    const irr::c8 g_PhysicsNodeName[] = "PhysicsNode";

    /**
    * @class CPhysicsNode
    * @author Christian Keimel
    * This node holds the data for a physical object. It must be a child node of a Mesh Scene Node to work properly
    */
    class CPhysicsNode : public irr::scene::ISceneNode {
      public:
        enum class enNodeType {
          Trimesh,
          Sphere,
          Box
        };

      private:
        std::vector<std::tuple<bool, irr::u8> > m_vTrigger; /**< Every material of the parent node can produce a trigger */
        std::vector<bool>                       m_vRespawn; /**< Respawn flag for every material */

        bool m_bCollides, /**< Does this object collide. If not it can still produce triggers */
             m_bStatic;   /**< Is this a static object? */

        float m_fMass;  /**< Mass of the object, only used for non-static objects */

        irr::core::aabbox3df m_cBox;

        enNodeType m_eType; /**< The type of physics body */

      public:
        CPhysicsNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CPhysicsNode();

        //*** Virtual method inherited from irr::scene::ISceneNode
        virtual irr::u32 getMaterialCount() const;
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual void OnRegisterSceneNode();

        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32>& getBoundingBox() const;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);

        virtual irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent = 0, irr::scene::ISceneManager* a_pNewManager = 0);

        enNodeType getNodeType();
        bool isStatic();
        irr::f32 getMass();
    };
  }
}
