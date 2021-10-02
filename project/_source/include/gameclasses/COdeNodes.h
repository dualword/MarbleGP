// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <ode/ode.h>
#include <string>
#include <map>

namespace dustbin {
  namespace scenenodes {
    class CCheckpointNode;
    class CPhysicsNode;
  }

  namespace gameclasses {
    class ITriggerHandler;

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
             m_bTrigger,
             m_bRespawn;

        int m_iTrigger;

        CWorld* m_pWorld;

        dGeomID m_cGeom;
        dBodyID m_cBody;

        std::string m_sName;

        CObject(enObjectType a_eType, scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string &a_sName, int a_iMaterial = 0);

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
        CObjectBox(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string &a_sName);

        virtual ~CObjectBox();
    };

    /**
    * @class CObjectSphere
    * @author Christian Keimel
    * Spheres for the physics simulation
    */
    class CObjectSphere : public CObject {
      public:
        CObjectSphere(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName);

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
        CObjectTrimesh(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName, int a_iMaterial = 0);

        virtual ~CObjectTrimesh();
    };

    /**
    * @class CObjectCheckpoint
    * @author Christian Keimel
    * Physics object for the checkpoints
    */
    class CObjectCheckpoint : public CObject {
      public:
        CObjectCheckpoint(scenenodes::CCheckpointNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName);
        virtual ~CObjectCheckpoint();

        std::map<int, std::vector<int>> m_mNext;
        bool m_bLapStart;
        std::vector<int> m_vFinishLapIDs;
    };

    /**
    * @class CObjectMarble
    * @author Christian Keimel
    * This is the class that stores all marbles
    * for a game
    */
    class CObjectMarble : public CObject {
      public:
        enum class enMarbleState {
          Countdown,
          Rolling,
          Respawn1,
          Respawn2,
          Stunned,
          Finished
        };

        int m_iManualRespawn,
            m_iLastTrigger,
            m_iLastContact,   /**< The step no. of the last contact */
            m_iRespawnStart,  /**< The step when the respawn of the marble started */
            m_iStunnedStart,  /**< The step when the stunned state started */
            m_iLapNo;         /**< The current lap of the marble */

        bool m_bActive; /**< The player has already shown some activity */

        irr::core::vector3df m_vCamera,     /**< The standard camera position. Depending on the "rearview" flag this value or "m_vRearview" is sent */
                             m_vRearview,   /**< The camera position for the rearview. Depending on the "rearview" flag this or "m_vCamera" is sent */
                             m_vOffset,
                             m_vUpVector,   /**< The camera up-vector */
                             m_vUpOffset,   /**< This is generally the same as m_vUpVector, but when the marble has no contact this is not interpolated to keep an up-offset for the camera */
                             m_vContact,    /**< The last contact point. This is used for up-vector interpolation even when currently no contact exists */
                             m_vSideVector, /**< The camera side-vector. This is used for acceleration, torque is applied around this axis */
                             m_vDirection,  /**< The direction the camera is pointing to. Torque is applied around this axis for steering */
                             m_vPosition,   /**< The current position of the marble */
                             m_vVelocity;   /**< The linear velocity. This is only used to determine whether or not a marble gets stunned */

        dReal m_fDamp;  /**< The damping of the marble's body */

        int m_iCtrlX,   /**< The X-Control state updated in "onMarblecontrol" */
            m_iCtrlY,   /**< The Y-Control state updated in "onMarblecontrol" */
            m_iLastCp;  /**< The last Checkpoint the marble has passed */

        bool m_bHasContact,
             m_bBrake,         /**< The Brake Control state updated in "onMarblecontrol" */
             m_bRearView,      /**< The Rearview Control state updated in "onMarblecontrol" */
             m_bRespawn;       /**< The Respawn Control state updated in "onMarblecontrol" */

        irr::core::vector3df m_vRespawnPos,   /**< The respawn position taken from the last checkpoint */
                             m_vRespawnDir;   /**< The respawn direction taken from the last checkpoint */

        enMarbleState m_eState; /**< The state of the marble */

        std::vector<int> m_vNextCheckpoints; /**< A list of the next checkpoint options */

        CObjectMarble(irr::scene::ISceneNode* a_pNode, const irr::core::vector3df &a_cDirection, CWorld* a_pWorld, const std::string& a_sName);
        virtual ~CObjectMarble();

        bool canBeStunned();
        bool canRespawn();
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

        std::map<irr::s32, CObjectCheckpoint*> m_mCheckpoints;

        ITriggerHandler* m_pTriggerHandler;

        CWorld(ITriggerHandler *a_pTriggerHandler);

        virtual ~CWorld();

        void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_cPos);
        void handleRespawn(int a_iMarble);
        void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint);
        void handleLapStart(int a_iMarbleId, int a_iLapNo);
    };
  }
}
