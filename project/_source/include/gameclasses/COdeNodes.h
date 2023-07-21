// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <ode/ode.h>
#include <string>
#include <vector>
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

      protected:
        void createJoint(irr::scene::ISceneNode* a_pNode);

      public:
        int m_iId;

        bool m_bStatic;       /**< Is this a static object? */
        bool m_bCollides;     /**< Does this object collide with others. Hint: triggers do not have to */
        bool m_bTrigger;      /**< Does this object trigger? */
        bool m_bRespawn;      /**< Does the collision of a marble with this object respawn the marble? */
        bool m_bSliderJoint;  /**< Is the joint attached to the object (if any) a slider? */
        bool m_bMarbleTouch;  /**< This object has a "Marble Touch" trigger */
        bool m_bMarbleOnly;   /**< This object does only collide with marbles */
        bool m_bCfmEnter;     /**< Is this object an entry of a CFM zone? */
        bool m_bCfmExit;      /**< Is this object an exit of a CFM zone? */

        int m_iTrigger;
        int m_iJoint;         /**< The joint type (0 == hinge, 1 == slider) */

        CWorld* m_pWorld;

        dGeomID  m_cGeom;
        dBodyID  m_cBody;
        dJointID m_cJoint;

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

        std::vector<dReal    > m_vVertices;
        std::vector<dTriIndex> m_vIndices;

        /**
        * Add a mesh buffer of the scene node to the trimesh
        * @param a_pBuffer the mesh buffer to add
        * @param a_cMatrix the transformation matrix of the scene node
        * @param a_iIndexV the vertex index
        */
        void addMeshBuffer(irr::scene::IMeshBuffer* a_pBuffer, const irr::core::CMatrix4<irr::f32> &a_cMatrix, irr::u32 &a_iIndexV);

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

        std::vector<int> m_vNext;   /**< The IDs of the next checkpoints */
        bool m_bLapStart;           /**< Does this checkpoint mark the start of a lap? */
        bool m_bHasRespawn;         /**< Was a respawn node found? */

        irr::core::vector3df m_vRespawnPos, /**< Respawn position (if m_bHasRespawn == true) */
                             m_vRespawnDir; /**< Respawn direction (if m_bHasRespawn == true) */
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
          Countdown,  /**< Countdown is going on */
          Rolling,    /**< Racing normally */
          Respawn1,   /**< First part of the respawn */
          Respawn2,   /**< Second part of the respawn */
          Stunned,    /**< The player is stunned */
          Finished,   /**< The player has finished the race */
          Withdrawn   /**< The player has cancelled his race */
        };

        int m_iManualRespawn;
        int m_iLastTrigger;
        int m_iLastContact;   /**< The step no. of the last contact */
        int m_iRespawnStart;  /**< The step when the respawn of the marble started */
        int m_iStunnedStart;  /**< The step when the stunned state started */
        int m_iLapNo;         /**< The current lap of the marble */
        int m_iFinishTime;    /**< Finish time of the marble */
        int m_iPosition;      /**< Position in the race */
        int m_iWithdraw;      /**< Time (in simulation steps) for the user to confirm a withdraw (-1 == no withdraw requested) */
        int m_iGroundContact; /**< The last step with ground contact (used to identify if a marble is jumping) */
        int m_iLastJump;      /**< The end of the last jump */

        bool m_bActive;     /**< The player has already shown some activity */
        bool m_bAiPlayer;   /**< Is this an AI player? */
        bool m_bInCfm;      /**< Is this marble inside a CFM zone? */

        irr::core::vector3df m_vCamera;     /**< The standard camera position. Depending on the "rearview" flag this value or "m_vRearview" is sent */
        irr::core::vector3df m_vOffset;
        irr::core::vector3df m_vUpVector;   /**< The camera up-vector */
        irr::core::vector3df m_vUpOffset;   /**< This is generally the same as m_vUpVector, but when the marble has no contact this is not interpolated to keep an up-offset for the camera */
        irr::core::vector3df m_vContact;    /**< The last contact point. This is used for up-vector interpolation even when currently no contact exists */
        irr::core::vector3df m_vSideVector; /**< The camera side-vector. This is used for acceleration, torque is applied around this axis */
        irr::core::vector3df m_vDirection;  /**< The direction the camera is pointing to. Torque is applied around this axis for steering */
        irr::core::vector3df m_vPosition;   /**< The current position of the marble */
        irr::core::vector3df m_vVelocity;   /**< The linear velocity. This is only used to determine whether or not a marble gets stunned */

        std::map<int, bool> m_mStepTriggers[2];   /**< Using this array of maps we make sure a "trigger hit" message is only sent once */
        int                 m_iActiveTrigger;     /**< The current trigger map */
        int                 m_iSecondTrigger;     /**< The other trigger map */

        dReal m_fDamp;  /**< The damping of the marble's body */

        int m_iCtrlX;   /**< The X-Control state updated in "onMarblecontrol" */
        int m_iCtrlY;   /**< The Y-Control state updated in "onMarblecontrol" */
        int m_iLastCp;  /**< The last Checkpoint the marble has passed */

        bool m_bHasContact;
        bool m_bBrake;         /**< The Brake Control state updated in "onMarblecontrol" */
        bool m_bRearView;      /**< The Rearview Control state updated in "onMarblecontrol" */
        bool m_bRespawn;       /**< The Respawn Control state updated in "onMarblecontrol" */

        float m_fThrustPower;   /**< The power of acceleration (only used for AI marbles ) */
        float m_fSteerPower;    /**< The power of steering (only used for AI marbles ) */

        irr::core::vector3df m_vRespawnPos;   /**< The respawn position taken from the last checkpoint */
        irr::core::vector3df m_vRespawnDir;   /**< The respawn direction taken from the last checkpoint */

        enMarbleState m_eState; /**< The state of the marble */

        std::vector<int> m_vNextCheckpoints; /**< A list of the next checkpoint options */

        CObjectMarble(irr::scene::ISceneNode* a_pNode, const irr::core::vector3df &a_cDirection, CWorld* a_pWorld, const std::string& a_sName);
        virtual ~CObjectMarble();

        bool canBeStunned();
        bool canRespawn();

        std::string toString();
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
        std::vector<CObject*> m_vMoving;
        std::vector<CObject*> m_vTriggers;

        int m_iWorldStep;

        std::map<irr::s32, CObjectCheckpoint*> m_mCheckpoints;

        ITriggerHandler* m_pTriggerHandler;

        CWorld(ITriggerHandler *a_pTriggerHandler);

        virtual ~CWorld();

        void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_cPos, bool a_bBroadcast);
        void handleRespawn(int a_iMarble);
        void handleCheckpoint(int a_iMarbleId, int a_iCheckpoint);
        void handleLapStart(int a_iMarbleId, int a_iLapNo);
        void handleMarbleTouch(int a_iMarbleId, int a_iTouchId);
    };
  }
}
