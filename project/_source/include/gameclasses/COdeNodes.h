// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <ode/ode.h>

namespace dustbin {
  namespace scenenodes {
    class CPhysicsNode;
  }

  namespace gameclasses {
    /**
    * This function converts Irrlicht's Euler angles to ODE Quaternions
    * @param v the Irrlicht rotation in Euler angles
    * @param q the ODE rotation as Quaternion
    */
    void eulerToQuaternion(const irr::core::vector3df v, dQuaternion q);

    /**
    * This enum defines the supported object types
    */
    enum class enObjectType {
      Box,        /**< A box for the physics */
      Sphere,     /**< A sphere. Note that the marbles are defined as a type of their own */
      Trimesh,    /**< A Trimes object */
      Marble,     /**< This is a sphere that is used as a in-game marble */
      Checkpoint  /**< Checkpoints are also defined as objects of their own */
    };

    class CWorld;

    /**
    * @class CObject
    * @author Christian Keimel
    * This is the base class for all physics objects
    */
    class CObject {
      private:
        enObjectType m_eType;

      public:
        int m_iId;

        bool m_bStatic,
          m_bCollides,
          m_bTrigger;

        int m_iTrigger;

        CWorld* m_pWorld;

        dGeomID m_cGeom;
        dBodyID m_cBody;

        CObject(enObjectType a_eType, scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, int a_iMaterial = 0);

        virtual ~CObject();

        enObjectType getType();


    };

    /**
    * @class CObjectBox
    * @author Christian Keimel
    * This is a physics box
    */
    class CObjectBox : public CObject {
      public:
        CObjectBox(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld);

        virtual ~CObjectBox();
    };

    /**
    * @class CObjectSphere
    * @author Christian Keimel
    * Spheres for the physics simulation
    */
    class CObjectSphere : public CObject {
      public:
        CObjectSphere(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld);

        virtual ~CObjectSphere();
    };

    /**
    * @class CObjectTrimesh
    * @author Christian Keimel
    * This is a trimesh physics object
    */
    class CObjectTrimesh : public CObject {
      private:
        dTriMeshDataID m_cTrimeshData;

        dVector3 * m_aVertices;
        dTriIndex* m_aIndices;

        std::vector<dReal    > m_vVertices;
        std::vector<dTriIndex> m_vIndices;

      public:
        CObjectTrimesh(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, int a_iMaterial = 0);

        virtual ~CObjectTrimesh();
    };

    /**
    * @class CObjectMarble
    * @author Christian Keimel
    * This is the class that stores all marbles
    * for a game
    */
    class CObjectMarble : public CObject {
      public:
        int m_iManualRespawn,
            m_iLastTrigger;

        bool m_bActive; /**< The player has already shown some activity */

        irr::core::vector3df m_vContact,
                             m_vCamera,
                             m_vUpVector,
                             m_vOffset,
                             m_vSideVector,
                             m_vDirection;

        bool m_bHasContact;

        CObjectMarble(irr::scene::ISceneNode* a_pNode, CWorld* a_pWorld);
        virtual ~CObjectMarble();
    };

    /**
    * @class CWorld
    * @author Christian Keimel
    * This is the simulation world. It is defined here to strictly separate
    * the physics core from the rest of the code, i.e. I just have to touch
    * this one .cpp file if I want to swith to another physics engine later
    */
    class CWorld {
      public:
        dWorldID       m_cWorld;
        dSpaceID       m_cSpace;
        dJointGroupID  m_cContacts;

        std::vector<CObject*> m_vObjects;

        CWorld();

        virtual ~CWorld();

        void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_cPos);
    };
  }
}
