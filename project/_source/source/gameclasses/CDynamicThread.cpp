// (w) 2021 by Dustbin::Games / Christian Keimel

#include <_generated/lua/CLuaScript_gamelogic.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <_generated/lua/CLuaScript_physics.h>
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CDynamicThread.h>
#include <_generated/lua/lua_tables.h>
#include <lua/CLuaSingleton_system.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/COdeNodes.h>
#include <lua/CLuaHelpers.h>
#include <gfx/SViewPort.h>
#include <exception>
#include <CGlobal.h>
#include <ode/ode.h>

#define MAX_CONTACTS 16
const double GRAD_PI = 180.0 / 3.1415926535897932384626433832795;

namespace dustbin {
  namespace gameclasses {
    irr::core::vector3df vectorOdeToIrr(const dReal* a_pOde) {
      return irr::core::vector3df((irr::f32)a_pOde[0], (irr::f32)a_pOde[1], (irr::f32)a_pOde[2]);
    }

    irr::core::vector3df quaternionToEuler(const dQuaternion a_aQuaternion) {
      dReal w, x, y, z;

      w = a_aQuaternion[0];
      x = a_aQuaternion[1];
      y = a_aQuaternion[2];
      z = a_aQuaternion[3];

      double sqw = w * w;
      double sqx = x * x;
      double sqy = y * y;
      double sqz = z * z;

      irr::core::vector3df l_vEuler;

      l_vEuler.Z = (irr::f32)(atan2(2.0 * (x * y + z * w), (sqx - sqy - sqz + sqw)) * GRAD_PI);
      l_vEuler.X = (irr::f32)(atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw)) * GRAD_PI);
      l_vEuler.Y = (irr::f32)(asin(-2.0 * (x * z - y * w)) * GRAD_PI);

      return l_vEuler;
    }

    void nearCollisionCallback(void* a_pData, dGeomID a_iGeom1, dGeomID a_iGeom2) {
      dBodyID l_pBody1 = dGeomGetBody(a_iGeom1);
      dBodyID l_pBody2 = dGeomGetBody(a_iGeom2);

      CWorld* l_pWorld = (CWorld*)a_pData;

      dWorldID      l_cWorld      = l_pWorld->m_cWorld;
      dJointGroupID l_cJointGroup = l_pWorld->m_cContacts;

      dContact l_cContact[MAX_CONTACTS];

      if (dGeomIsSpace(a_iGeom1) || dGeomIsSpace(a_iGeom2)) {
        // colliding a space with something :
        dSpaceCollide2(a_iGeom1, a_iGeom2, a_pData, &nearCollisionCallback);

        // collide all geoms internal to the space(s)
        if (dGeomIsSpace(a_iGeom1)) dSpaceCollide((dSpaceID)a_iGeom1, a_pData, &nearCollisionCallback);
        if (dGeomIsSpace(a_iGeom2)) dSpaceCollide((dSpaceID)a_iGeom2, a_pData, &nearCollisionCallback);
      }
      else {
        if (l_pBody1 == 0 && l_pBody2 == 0) return;

        CObject* l_pOdeNode1 = (CObject*)dGeomGetData(a_iGeom1),
               * l_pOdeNode2 = (CObject*)dGeomGetData(a_iGeom2);

        bool l_bMarbleCollision = l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() == enObjectType::Marble;

        // If two marbles collide we need some sort of "special" surface parameters
        if (l_bMarbleCollision) {
          for (irr::u32 i = 0; i < MAX_CONTACTS; i++) {
            l_cContact[i].surface.bounce = (dReal)0.5;
            l_cContact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftCFM | dContactSoftERP;
            l_cContact[i].surface.mu = (dReal)0;
            l_cContact[i].surface.mu2 = (dReal)0;
            l_cContact[i].surface.bounce_vel = (dReal)0.00001;
            l_cContact[i].surface.soft_cfm = (dReal)0.004;
            l_cContact[i].surface.soft_erp = (dReal)0.4;
            l_cContact[i].surface.rho = (dReal)0.9;
            l_cContact[i].surface.rho2 = (dReal)0.9;
            l_cContact[i].surface.slip1 = (dReal)0.5;
            l_cContact[i].surface.slip2 = (dReal)0.5;
          }
        }
        else {
          for (irr::u32 i = 0; i < MAX_CONTACTS; i++) {
            l_cContact[i].surface.bounce = (dReal)0.15;
            l_cContact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2; // | dContactSoftCFM | dContactSoftERP;
            l_cContact[i].surface.mu = (dReal)1500;
            l_cContact[i].surface.mu2 = (dReal)0;
            l_cContact[i].surface.bounce_vel = (dReal)0.0001;
            l_cContact[i].surface.soft_cfm = (dReal)0.001;
            l_cContact[i].surface.soft_erp = (dReal)0.2;
            l_cContact[i].surface.rho = (dReal)0.9;
            l_cContact[i].surface.rho2 = (dReal)0.9;
            l_cContact[i].surface.slip1 = (dReal)0.05;
            l_cContact[i].surface.slip2 = (dReal)0.05;
          }
        }

        irr::u32 numc = dCollide(a_iGeom1, a_iGeom2, MAX_CONTACTS, &l_cContact[0].geom, sizeof(dContact));

        if (!l_bMarbleCollision) {
          if (numc > 0) {
            CObjectMarble* p = nullptr; // The colliding marble
            CObject      * o = nullptr, // Trigger or respawn object
                         * c = nullptr, // Checkpoint
                         * x = nullptr; // Any colliding object

            if (l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);

              x = l_pOdeNode2;

              if (l_pOdeNode2->m_bTrigger || l_pOdeNode2->m_bRespawn)
                o = l_pOdeNode2;

              if (l_pWorld->m_mCheckpoints.find(l_pOdeNode2->m_iId) != l_pWorld->m_mCheckpoints.end())
                c = l_pOdeNode2;
            }
            else if (l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);

              x = l_pOdeNode1;

              if (l_pOdeNode1->m_bTrigger || l_pOdeNode1->m_bRespawn)
                o = l_pOdeNode1;

              if (l_pWorld->m_mCheckpoints.find(l_pOdeNode1->m_iId) != l_pWorld->m_mCheckpoints.end())
                c = l_pOdeNode1;
            }

            // We have a near collision of a marble
            if (p != nullptr) {
              const dReal* l_aPos = dBodyGetPosition(p->m_cBody);

              if (x != nullptr && x->m_bCollides) {
                // Update the up-vector
                p->m_vUpVector = vectorOdeToIrr(l_aPos) - vectorOdeToIrr(l_cContact[0].geom.pos);
              }

              // The marble collides witha  non-marble object
              if (o != nullptr) {
                // The other objects triggers
                if (o->m_bTrigger) {
                  irr::core::vector3df l_vPos = irr::core::vector3df((irr::f32)l_aPos[0], (irr::f32)l_aPos[1], (irr::f32)l_aPos[2]);

                  l_pWorld->handleTrigger(o->m_iTrigger, p->m_iId, l_vPos);
                }

                // The other object starts a respawn
                if (o->m_bRespawn) {
                  l_pWorld->handleRespawn(p->m_iId);
                }
              }

              // Checkpoint hit
              if (c != nullptr) {
                // First we see whether or not this checkpoint is in the list of checkpoints ...
                for (std::vector<int>::iterator it = p->m_vNextCheckpoints.begin(); it != p->m_vNextCheckpoints.end(); it++) {
                  if (*it == c->m_iId && l_pWorld->m_mCheckpoints.find(c->m_iId) != l_pWorld->m_mCheckpoints.end()) {
                    CObjectCheckpoint* l_pCp = l_pWorld->m_mCheckpoints[c->m_iId];

                    p->m_iLastCp = c->m_iId;

                    if (l_pCp->m_bLapStart) {
                      p->m_iLapNo++;
                      l_pWorld->handleLapStart(p->m_iId, p->m_iLapNo);
                    }
                    l_pWorld->handleCheckpoint(p->m_iId, c->m_iId);

                    p->m_vNextCheckpoints.clear();

                    if (l_pCp->m_bHasRespawn) {
                      p->m_vRespawnPos = l_pCp->m_vRespawnPos;
                      p->m_vRespawnDir = l_pCp->m_vRespawnDir;
                    }

                    for (std::vector<int>::iterator it2 = l_pCp->m_vNext.begin(); it2 != l_pCp->m_vNext.end(); it2++)
                      p->m_vNextCheckpoints.push_back(*it2);

                    break;
                  }
                }
              }
            }
          }

          // We have a non-colliding object, so we stop here
          if ((l_pOdeNode1 == nullptr || !l_pOdeNode1->m_bCollides) || (l_pOdeNode2 == nullptr || !l_pOdeNode2->m_bCollides))
            return;
        }

        dJointID l_cJoints[MAX_CONTACTS];

        for (irr::u32 i = 0; i < numc; i++) {
          if (l_pOdeNode1 != nullptr && l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() != enObjectType::Marble && l_pOdeNode2->m_bCollides) {
            CObjectMarble* p = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);
            p->m_bHasContact = true;
            p->m_vContact = irr::core::vector3df((irr::f32)l_cContact[i].geom.pos[0], (irr::f32)l_cContact[i].geom.pos[1], (irr::f32)l_cContact[i].geom.pos[2]);
          }

          if (l_pOdeNode2 != nullptr && l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble && l_pOdeNode1->m_bCollides) {
            CObjectMarble* p = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);
            p->m_bHasContact = true;
            p->m_vContact = irr::core::vector3df((irr::f32)l_cContact[i].geom.pos[0], (irr::f32)l_cContact[i].geom.pos[1], (irr::f32)l_cContact[i].geom.pos[2]);
          }

          l_cJoints[i] = dJointCreateContact(l_cWorld, l_cJointGroup, &l_cContact[i]);
          dJointAttach(l_cJoints[i], l_pBody1, l_pBody2);
        }
      }
    }

    void CDynamicThread::createPhysicsObjects(irr::scene::ISceneNode* a_pNode) {
      if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_WorldNodeId) {
        if (m_pWorld == nullptr) {
          m_pWorld = new CWorld(this);
        }
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_PhysicsNodeId) {
        if (m_pWorld != nullptr) {
          scenenodes::CPhysicsNode* l_pNode = reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode);

          switch (l_pNode->getNodeType()) {
            case scenenodes::CPhysicsNode::enNodeType::Box:
              m_pWorld->m_vObjects.push_back(new CObjectBox(l_pNode, m_pWorld, l_pNode->getName()));
              break;

            case scenenodes::CPhysicsNode::enNodeType::Sphere:
              m_pWorld->m_vObjects.push_back(new CObjectSphere(l_pNode, m_pWorld, l_pNode->getName()));
              break;

            case scenenodes::CPhysicsNode::enNodeType::Trimesh:
              m_pWorld->m_vObjects.push_back(new CObjectTrimesh(l_pNode, m_pWorld, l_pNode->getName()));
              break;
          }
        }
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId) {
        m_fGridAngle = reinterpret_cast<scenenodes::CStartingGridSceneNode*>(a_pNode)->getAngle();
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_CheckpointNodeId) {
        CObjectCheckpoint* p = new CObjectCheckpoint(reinterpret_cast<scenenodes::CCheckpointNode*>(a_pNode), m_pWorld, a_pNode->getName());
        m_pWorld->m_vObjects.push_back(p);
        m_pWorld->m_mCheckpoints[a_pNode->getID()] = p;
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++)
        createPhysicsObjects(*it);
    }

    void CDynamicThread::run() {
      do {
        messages::IMessage* l_pMsg = m_pInputQueue->popMessage();
        if (l_pMsg != nullptr) {
          if (!handleMessage(l_pMsg)) 
            delete l_pMsg;
        }
        else break;
      } 
      while (true);

      if (!m_bPaused) {
        // Apply the marble controls
        for (int i = 0; i < 16; i++) {
          CObjectMarble* p = m_aMarbles[i];

          if (p != nullptr) {
            if (p->m_eState == CObjectMarble::enMarbleState::Rolling) {
              irr::f32 l_fCtrlX = ((irr::f32)p->m_iCtrlX) / 127.0f,
                       l_fCtrlY = ((irr::f32)p->m_iCtrlY) / 127.0f;

              // Marble Class Param: Steer Factor and Thrust Factor
              irr::core::vector2df l_vSteer = irr::core::vector2df(1.0f * l_fCtrlX, 0.5f * l_fCtrlY);

              // Marble Class Param: Max Steer Speed
              irr::f32 l_fSpeed  = m_aMarbles[i]->m_vVelocity.getLength(),
                       l_fFactor = (l_fSpeed / 30.0f) + 0.2f;

              if (l_fFactor > 1.0f)
                l_fFactor = 1.0f;

              l_vSteer.X *= l_fFactor;

              l_vSteer.normalize();

              irr::core::vector3df v = p->m_vOffset;
              v.normalize();
              // Marble Class Param: Steer Power and Thrust
              irr::core::vector3df l_vTorque = 70.0f * l_vSteer.X * v + 60.0f * l_vSteer.Y * p->m_vSideVector;

              dBodyAddTorque(p->m_cBody, (dReal)l_vTorque.X, (dReal)l_vTorque.Y, (dReal)l_vTorque.Z);

              if (p->m_bBrake)
                dBodySetAngularDamping(p->m_cBody, (dReal)0.05);
              else
                dBodySetAngularDamping(p->m_cBody, p->m_fDamp);

              if (p->m_iManualRespawn == -1) {
                if (p->m_bRespawn)
                  p->m_iManualRespawn = m_iWorldStep;
              }
              else {
                if (!p->m_bRespawn)
                  p->m_iManualRespawn = -1;
                else if (m_iWorldStep - p->m_iManualRespawn > 180) {
                  p->m_iManualRespawn = -1;
                  p->m_iRespawnStart = m_iWorldStep;
                  p->m_eState = CObjectMarble::enMarbleState::Respawn1;

                  sendPlayerrespawn(p->m_iId, 1, m_pOutputQueue);

                  if (m_pGameLogic != nullptr)
                    m_pGameLogic->onPlayerRespawn(p->m_iId, m_iWorldStep);

                  if (m_pPhysicsScript != nullptr)
                    m_pPhysicsScript->onPlayerRespawn(p->m_iId, m_iWorldStep);
                }
              }
            }
            else if (p->m_eState == CObjectMarble::enMarbleState::Countdown) {
              dBodySetAngularDamping(p->m_cBody, (dReal)0.9);
            }
            else if (p->m_eState == CObjectMarble::enMarbleState::Finished) {
              dBodySetAngularDamping(p->m_cBody, (dReal)0.1);
            }
          }
        }

        dSpaceCollide(m_pWorld->m_cSpace, m_pWorld, &nearCollisionCallback);
        dWorldStep(m_pWorld->m_cWorld, (dReal)0.008);
        dJointGroupEmpty(m_pWorld->m_cContacts);

        // Update the states of all marbles and send the "marble moved" messages
        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr) {
            CObjectMarble* p = m_aMarbles[i];

            const dReal* l_aPos    = dBodyGetPosition(p->m_cBody),
                       * l_aRot    = dBodyGetQuaternion(p->m_cBody),
                       * l_aLinVel = dBodyGetLinearVel(p->m_cBody),
                       * l_aAngVel = dBodyGetAngularVel(p->m_cBody);

            irr::core::vector3df l_vLinVel = vectorOdeToIrr(l_aLinVel),
                                 l_vAngVel = vectorOdeToIrr(l_aAngVel);

            if (p->m_eState == CObjectMarble::enMarbleState::Rolling) {
              irr::core::vector3df l_vDiff = l_vLinVel - p->m_vVelocity;
              irr::f32 l_fDot = l_vLinVel.dotProduct(p->m_vVelocity);

              if (l_vDiff.getLength() > 30.0f && l_fDot < 0.0f) {
                p->m_eState = CObjectMarble::enMarbleState::Stunned;
                p->m_iStunnedStart = m_iWorldStep;
                sendPlayerstunned(p->m_iId, 1, m_pOutputQueue);

                if (m_pGameLogic != nullptr)
                  m_pGameLogic->onPlayerStunned(p->m_iId, 1, m_iWorldStep);

                if (m_pPhysicsScript != nullptr)
                  m_pPhysicsScript->onPlayerStunned(p->m_iId, 1, m_iWorldStep);
              }

              p->m_vVelocity = l_vLinVel;
            }

            p->m_vPosition = vectorOdeToIrr(l_aPos);

            irr::f32 l_fLinVel = l_vLinVel.getLength();

            if (p->m_bActive) {
              if (l_fLinVel > 0.5f) {
                irr::f32 l_fFactor = l_fLinVel / 750.0f;

                p->m_vDirection = l_vLinVel;
                p->m_vDirection.normalize();

                irr::core::vector3df l_vDirection = -(l_fLinVel < 5.0f ? 5.0f : l_fLinVel > 15.0f ? 15.0f : l_fLinVel) * p->m_vDirection;

                p->m_vOffset     = p->m_vOffset  .interpolate(l_vDirection  , p->m_vOffset  , l_fFactor);
                p->m_vUpOffset   = p->m_vUpOffset.interpolate(p->m_vUpVector, p->m_vUpOffset, l_fFactor);
                p->m_vSideVector = p->m_vOffset.crossProduct(p->m_vUpOffset);
                p->m_vSideVector.normalize();

                p->m_vCamera   = p->m_vPosition + p->m_vOffset + 3.0f * p->m_vUpOffset;
                p->m_vRearview = p->m_vPosition - p->m_vOffset + 3.0f * p->m_vUpOffset;

                if (p->m_bHasContact) {
                  p->m_iLastContact = m_iWorldStep;
                  p->m_fDamp = (dReal)0.0015;
                }
                else {
                  int l_iLastContact = m_iWorldStep - p->m_iLastContact;

                  p->m_fDamp = (dReal)0.01;

                  // Marble Class Param: Damp Time Offset
                  if (l_iLastContact > 10) {
                    dReal l_fFact = (dReal)(l_fLinVel / l_vAngVel.getLength());
                    if (l_fFact > 1.5) {
                      l_fFact -= 1.5;
                    }

                    if (l_fFact < 0.0) l_fFact = -l_fFact;
                    // Marble Class Param: Damp Factor
                    p->m_fDamp /= (dReal)(0.65 * l_fFact);
                    // Marble Class Param: Max Damp
                    if (p->m_fDamp > 0.75) p->m_fDamp = 0.75;
                  }
                }
              }
              else p->m_fDamp = (dReal)0.0015;
            }
            else {
              p->m_vCamera     = p->m_vPosition + p->m_vOffset + 3.0f * p->m_vUpOffset;
              p->m_vRearview   = p->m_vPosition - p->m_vOffset + 3.0f * p->m_vUpOffset;
              p->m_vSideVector = p->m_vOffset.crossProduct(p->m_vUpOffset);
              p->m_vSideVector.normalize();

              if (l_fLinVel > 5.0f) {
                printf("Activate (1)\n");
                p->m_bActive = true;
              }
            }
            
            // The marble is respawning
            if (p->m_iRespawnStart != -1) {
              if (p->m_eState == CObjectMarble::enMarbleState::Respawn1 && m_iWorldStep - p->m_iRespawnStart >= 180) {
                // Reset up vector
                p->m_vUpVector = irr::core::vector3df(0.0f, 1.0f, 0.0f);
                p->m_vUpOffset = irr::core::vector3df(0.0f, 1.0f, 0.0f);

                // Set state to "Respawn 2"
                p->m_eState = CObjectMarble::enMarbleState::Respawn2;

                // Send message
                sendCamerarespawn(p->m_iId, p->m_vRespawnPos + p->m_vRespawnDir + 3.0f * p->m_vUpOffset, p->m_vRespawnPos, m_pOutputQueue);
              }
              else if (p->m_eState == CObjectMarble::enMarbleState::Respawn2 && m_iWorldStep - p->m_iRespawnStart >= 360) {
                // We need to check for possible collisions before we continue with the respawn
                bool l_bRespawn = true;

                for (int i = 0; i < 16; i++) {
                  if (m_aMarbles[i] != nullptr && p != m_aMarbles[i]) {
                    if ((m_aMarbles[i]->m_vPosition - p->m_vRespawnPos).getLengthSQ() < 100)  // 10 meters minimum distance (100 = 10²)
                      l_bRespawn = false;
                  }
                }

                if (l_bRespawn) {
                  // Reset position of marble and stop all movement
                  dBodySetLinearVel(p->m_cBody, 0.0, 0.0, 0.0);
                  dBodySetAngularVel(p->m_cBody, 0.0, 0.0, 0.0);
                  dBodySetPosition(p->m_cBody, p->m_vRespawnPos.X, p->m_vRespawnPos.Y, p->m_vRespawnPos.Z);

                  // Update marble data
                  p->m_iRespawnStart = -1;
                  p->m_iStunnedStart = -1;
                  p->m_vPosition     = p->m_vRespawnPos;
                  p->m_vOffset       = p->m_vRespawnDir;
                  p->m_vVelocity     = irr::core::vector3df(0.0f);
                  p->m_vUpVector     = irr::core::vector3df(0.0f, 1.0f, 0.0f);
                  p->m_vUpOffset     = irr::core::vector3df(0.0f, 1.0f, 0.0f);
                  p->m_vCamera       = p->m_vPosition + p->m_vOffset + 3.0f * p->m_vUpOffset;
                  p->m_vRearview     = p->m_vPosition - p->m_vOffset + 3.0f * p->m_vUpOffset;
                  p->m_vSideVector   = p->m_vOffset.crossProduct(p->m_vUpOffset);
                  p->m_vSideVector.normalize();

                  // Reset state of marble
                  p->m_eState  = CObjectMarble::enMarbleState::Rolling;
                  p->m_bActive = false;

                  // Send message to game state
                  sendPlayerrespawn(p->m_iId, 2, m_pOutputQueue);
                }
              }
            }
            else if (p->m_iStunnedStart != -1 && m_iWorldStep - p->m_iStunnedStart >= 360) {
              sendPlayerstunned(p->m_iId, 0, m_pOutputQueue);
              p->m_eState = CObjectMarble::enMarbleState::Rolling;
              p->m_iStunnedStart = -1;

              if (p->m_iManualRespawn != -1)
                p->m_iManualRespawn = m_iWorldStep;

              if (m_pPhysicsScript != nullptr)
                m_pPhysicsScript->onPlayerStunned(p->m_iId, 0, m_iWorldStep);
            }

            sendMarblemoved(p->m_iId,
              p->m_vPosition, 
              quaternionToEuler(l_aRot), 
              l_vLinVel, 
              vectorOdeToIrr(l_aAngVel).getLength(), 
              p->m_bRearView ? p->m_vRearview : p->m_vCamera,
              p->m_vUpOffset, 
              p->m_iCtrlX,
              p->m_iCtrlY, 
              p->m_bHasContact,
              p->m_bBrake,
              p->m_bRearView,
              false,
              m_pOutputQueue);

            p->m_bHasContact = false;
          }
        }

        // Send all other moving objects to the game state
        for (std::vector<CObject*>::iterator it = m_pWorld->m_vMoving.begin(); it != m_pWorld->m_vMoving.end(); it++) {
          const dReal* l_aPos    = dBodyGetPosition  ((*it)->m_cBody),
                     * l_aRot    = dBodyGetQuaternion((*it)->m_cBody),
                     * l_aLinVel = dBodyGetLinearVel ((*it)->m_cBody),
                     * l_aAngVel = dBodyGetAngularVel((*it)->m_cBody);

          sendObjectmoved((*it)->m_iId, vectorOdeToIrr(l_aPos), quaternionToEuler(l_aRot), vectorOdeToIrr(l_aLinVel), vectorOdeToIrr(l_aAngVel).getLength(), m_pOutputQueue);
        }

        if (m_eGameState == enGameState::Countdown) {
          auto l_cLuaCountdown = [](int a_iTick, int a_iStep, CLuaScript_gamelogic *a_pScript, CLuaScript_physics *a_pPhysicsScript) {
            if (a_pScript != nullptr)
              a_pScript->onCountdown(a_iTick, a_iStep);

            if (a_pPhysicsScript != nullptr)
              a_pPhysicsScript->onCountdown(a_iTick, a_iStep);
          };

          if (m_iWorldStep == 0) {
            sendCountdown(4, m_pOutputQueue);
            l_cLuaCountdown(4, m_iWorldStep, m_pGameLogic, m_pPhysicsScript);
          }
          else {
            int l_iStep = m_iWorldStep - 360;

            if (l_iStep == 120) {
              sendCountdown(3, m_pOutputQueue);
              l_cLuaCountdown(3, m_iWorldStep, m_pGameLogic, m_pPhysicsScript);
            }
            else if (l_iStep == 240) {
              sendCountdown(2, m_pOutputQueue);
              l_cLuaCountdown(2, m_iWorldStep, m_pGameLogic, m_pPhysicsScript);
            }
            else if (l_iStep == 360) {
              sendCountdown(1, m_pOutputQueue);
              l_cLuaCountdown(1, m_iWorldStep, m_pGameLogic, m_pPhysicsScript);
            }
            else if (l_iStep == 480) {
              sendCountdown(0, m_pOutputQueue);
              l_cLuaCountdown(0, m_iWorldStep, m_pGameLogic, m_pPhysicsScript);
              m_eGameState = enGameState::Racing;
            }
          }
        }

        sendStepmsg(m_iWorldStep, m_pOutputQueue);

        m_iWorldStep++;
      }

      m_cNextStep = m_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(8);
      std::this_thread::sleep_until(m_cNextStep);
    }

    void CDynamicThread::execute() {
      m_cNextStep = std::chrono::high_resolution_clock::now();

      while (!m_bStopThread) {
        run();
      }

      printf("Dynamics thread ends.\n");
    }

    CDynamicThread::CDynamicThread(scenenodes::CWorldNode* a_pWorld, const std::vector<gameclasses::SPlayer*>& a_vPlayers, int a_iLaps, const std::string& a_sLuaTrackScript) :
      m_eGameState(enGameState::Countdown),
      m_pPhysicsScript(nullptr),
      m_pGameLogic(nullptr),
      m_fGridAngle(0.0f),
      m_pWorld(nullptr),
      m_bPaused(false),
      m_iWorldStep(0)
    {
      createPhysicsObjects(a_pWorld);

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      if (m_pWorld != nullptr) {
        int l_iIndex = 0;
        for (std::vector<gameclasses::SPlayer*>::const_iterator it = a_vPlayers.begin(); it != a_vPlayers.end(); it++) {
          irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 0.0f, 10.0f);
          l_vOffset.rotateXZBy(m_fGridAngle);

          CObjectMarble* l_pMarble = new CObjectMarble((*it)->m_pMarble->m_pPositional, l_vOffset, m_pWorld, std::string("Marble #") + std::to_string(l_iIndex + 1));

          l_pMarble->m_vDirection  = l_vOffset;
          l_pMarble->m_vUpVector   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_pMarble->m_vUpOffset   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_pMarble->m_vContact    = irr::core::vector3df();
          l_pMarble->m_vSideVector = l_pMarble->m_vDirection.crossProduct(l_pMarble->m_vUpVector);
          l_pMarble->m_vOffset     = l_vOffset;
          l_pMarble->m_vCamera     = (*it)->m_pMarble->m_pPositional->getAbsolutePosition() - l_vOffset + 3.0f * l_pMarble->m_vUpVector;
          l_pMarble->m_vRearview   = (*it)->m_pMarble->m_pPositional->getAbsolutePosition() + l_vOffset + 3.0f * l_pMarble->m_vUpVector;

          l_pMarble->m_vSideVector.normalize();
          l_pMarble->m_vDirection .normalize();

          m_pWorld->m_vObjects.push_back(l_pMarble);
          m_aMarbles[l_iIndex] = l_pMarble;
          printf("*** Marble with id %i stored in array index %i\n", (*it)->m_pMarble->m_pPositional->getID(), l_iIndex);

          for (std::map<irr::s32, CObjectCheckpoint*>::iterator it = m_pWorld->m_mCheckpoints.begin(); it != m_pWorld->m_mCheckpoints.end(); it++) {
            if (it->second->m_bLapStart)
              l_pMarble->m_vNextCheckpoints.push_back(it->first);
          }

          sendMarblemoved(l_pMarble->m_iId,
            (*it)->m_pMarble->m_pPositional->getAbsolutePosition(),
            (*it)->m_pMarble->m_pPositional->getRotation(),
            irr::core::vector3df(0.0f, 0.0f, 0.0f),
            0.0f,
            l_pMarble->m_vCamera,
            irr::core::vector3df(0.0f, 1.0f, 0.0f),
            0,
            0,
            false,
            false,
            false,
            false,
            m_pOutputQueue);

          l_iIndex++;
        }
      }

      std::vector<int> l_vMarbleIDs;

      for (int i = 0; i < 16; i++)
        if (m_aMarbles[i] != nullptr)
          l_vMarbleIDs.push_back(m_aMarbles[i]->m_iId);

      std::string l_sScript = lua::loadLuaScript("data/lua/gamelogics.lua");

      if (l_sScript != "") {
        m_pGameLogic = new CLuaScript_gamelogic(l_sScript);
        m_pGameLogic->setDynamicsThread(this);
        
        m_pGameLogic->initialize(l_vMarbleIDs, a_iLaps);
      }

      if (a_sLuaTrackScript != "") {
        m_pPhysicsScript = new CLuaScript_physics(a_sLuaTrackScript);
        m_pPhysicsScript->setDynamicsThread(this);

        std::map<std::string, int> l_mObjects;

        for (std::vector<CObject*>::iterator it = m_pWorld->m_vObjects.begin(); it != m_pWorld->m_vObjects.end(); it++) {
          l_mObjects[(*it)->m_sName] = (*it)->m_iId;
        }

        m_pPhysicsScript->initialize(l_vMarbleIDs, l_mObjects);
      }
    }

    CDynamicThread::~CDynamicThread() {
      if (m_pWorld != nullptr)
        delete m_pWorld;

      if (m_pGameLogic != nullptr)
        delete m_pGameLogic;

      if (m_pPhysicsScript != nullptr)
        delete m_pPhysicsScript;
    }

    /**
     * This function receives messages of type "MarbleControl"
     * @param a_ObjectId The ID of the Marble
     * @param a_CtrlX The X Control (steer)
     * @param a_CtrlY The Y Control (throttle)
     * @param a_Brake Is the brake active?
     * @param a_RearView Does the player want to look back?
     * @param a_Respawn Is the manual respawn button pressed?
     */
    void CDynamicThread::onMarblecontrol(irr::s32 a_ObjectId, irr::s8 a_CtrlX, irr::s8 a_CtrlY, bool a_Brake, bool a_RearView, bool a_Respawn) {
      irr::s32 l_iIndex = a_ObjectId - 10000;
      
      if (m_aMarbles[l_iIndex] != nullptr) {
        // Rearview is possible even if the marble is not yet active
        m_aMarbles[l_iIndex]->m_bRearView = a_RearView;

        if (m_aMarbles[l_iIndex]->m_bActive) {
          // Update the controls of the marble. Will be
          // applied before the simulation step.
          m_aMarbles[l_iIndex]->m_iCtrlX    = a_CtrlX;
          m_aMarbles[l_iIndex]->m_iCtrlY    = a_CtrlY;
          m_aMarbles[l_iIndex]->m_bBrake    = a_Brake;
          m_aMarbles[l_iIndex]->m_bRespawn  = a_Respawn;
          m_aMarbles[l_iIndex]->m_bRearView = a_RearView;
        }
        else {
          if (abs(a_CtrlX > 64) || abs(a_CtrlY > 64)) {
            printf("Activate (2)\n");
            m_aMarbles[l_iIndex]->m_bActive = true;
          }
        }
      }
    }


    /**
     * This function receives messages of type "CancelRace"
     */
    void CDynamicThread::onCancelrace() {
      for (int i = 0; i < 16; i++) {
        if (m_aMarbles[i] != nullptr) {
          if (m_aMarbles[i]->m_eState != CObjectMarble::enMarbleState::Finished) {
            m_aMarbles[i]->m_eState = CObjectMarble::enMarbleState::Finished;
            sendPlayerfinished(m_aMarbles[i]->m_iId, -1, m_aMarbles[i]->m_iLapNo, m_pOutputQueue);
          }
        }
      }

      sendRacefinished(1, m_pOutputQueue);
    }

    /**
     * This function receives messages of type "TogglePause"
     */
    void CDynamicThread::onTogglepause() {
      m_bPaused = !m_bPaused;
      sendPausechanged(m_bPaused, m_pOutputQueue);
    }

    void CDynamicThread::handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition) {
      if (m_pPhysicsScript != nullptr)
        m_pPhysicsScript->onTrigger(a_iMarble, a_iTrigger);
    }

    /**
    * Callback to start respawn of a marble
    * @param a_iMarble Id of the marble to respawn
    */
    void CDynamicThread::handleRespawn(int a_iMarble) {
      int l_iId = a_iMarble - 10000;

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr && m_aMarbles[l_iId]->canRespawn()) {
        // Respawn overrides stunned
        if (m_aMarbles[l_iId]->m_eState == CObjectMarble::enMarbleState::Stunned) {
          m_aMarbles[l_iId]->m_iStunnedStart = -1;
          sendPlayerstunned(m_aMarbles[l_iId]->m_iId, 0, m_pOutputQueue);

          if (m_pGameLogic != nullptr)
            m_pGameLogic->onPlayerStunned(a_iMarble, 0, m_iWorldStep);

          if (m_pPhysicsScript != nullptr)
            m_pPhysicsScript->onPlayerStunned(a_iMarble, 0, m_iWorldStep);
        }

        m_aMarbles[l_iId]->m_eState = CObjectMarble::enMarbleState::Respawn1;
        m_aMarbles[l_iId]->m_iRespawnStart = m_iWorldStep;

        sendPlayerrespawn(a_iMarble, 1, m_pOutputQueue);

        if (m_pGameLogic != nullptr)
          m_pGameLogic->onPlayerRespawn(a_iMarble, m_iWorldStep);

        if (m_pPhysicsScript != nullptr)
          m_pPhysicsScript->onPlayerRespawn(a_iMarble, m_iWorldStep);
      }
    }

    /**
    * Callback for sending a "Checkpoint" message
    * @param a_iMarble Id of the marble
    * @param a_iCheckpoint Checkpoint id
    */
    void CDynamicThread::handleCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      sendCheckpoint(a_iMarbleId, a_iCheckpoint, m_pOutputQueue);

      if (m_pGameLogic != nullptr)
        m_pGameLogic->onCheckpoint(a_iMarbleId, a_iCheckpoint, m_iWorldStep);

      if (m_pPhysicsScript != nullptr)
        m_pPhysicsScript->onCheckpoint(a_iMarbleId, a_iCheckpoint, m_iWorldStep);
    }

    /**
    * Callback for sending a "LapStart" message
    * @param a_iMarbleId Id of the marble
    * @param a_iLapNo Number of the started lap
    */
    void CDynamicThread::handleLapStart(int a_iMarbleId, int a_iLapNo) {
      sendLapstart(a_iMarbleId, a_iLapNo, m_pOutputQueue);

      if (m_pGameLogic != nullptr)
        m_pGameLogic->onLapStart(a_iMarbleId, a_iLapNo, m_iWorldStep);

      if (m_pPhysicsScript != nullptr)
        m_pPhysicsScript->onLapStart(a_iMarbleId, a_iLapNo, m_iWorldStep);
    }

    /**
    * LUA callback for finishing a player
    * @param a_iMarbleId the id of the marbles that has finished the race
    */
    void CDynamicThread::finishPlayer(int a_iMarbleId, int a_iRaceTime, int a_iLaps) {
      int l_iIndex = a_iMarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        m_aMarbles[l_iIndex]->m_eState = CObjectMarble::enMarbleState::Finished;
        sendPlayerfinished(a_iMarbleId, a_iRaceTime, a_iLaps, m_pOutputQueue);

        bool l_bAllFinished = true;

        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr && m_aMarbles[i]->m_eState != CObjectMarble::enMarbleState::Finished)
            l_bAllFinished = false;
        }

        if (m_pPhysicsScript != nullptr)
          m_pPhysicsScript->onPlayerFinished(a_iMarbleId);

        if (l_bAllFinished)
          sendRacefinished(0, m_pOutputQueue);
      }
    }

    /**
    * LUA callback for starting a player
    * @param a_iMarble the marble id
    */
    void CDynamicThread::startPlayer(int a_iMarble) {
      int l_iIndex = a_iMarble - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        m_aMarbles[l_iIndex]->m_eState = gameclasses::CObjectMarble::enMarbleState::Rolling;
      }
    }

    /**
    * Retrieve the world of the race
    * @return the world of the race
    */
    CWorld* CDynamicThread::getWorld() {
      return m_pWorld;
    }
  }
}