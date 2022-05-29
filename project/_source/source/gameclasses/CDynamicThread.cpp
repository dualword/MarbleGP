// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CMarbleTouchNode.h>
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <gameclasses/IGameLogic.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/COdeNodes.h>
#include <gfx/SViewPort.h>
#include <exception>
#include <CGlobal.h>
#include <ode/ode.h>
#include <algorithm>

constexpr auto MAX_CONTACTS = 16;
const double GRAD_PI = 180.0 / 3.1415926535897932384626433832795;

namespace dustbin {
  namespace gameclasses {
    /**
    * Factory function for the game logic
    * @param a_sType type of game logic
    * @return an instance of the IGameLogic interface
    */
    IGameLogic *createGameLogic(const std::string &a_sType);

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

      dContact l_cContact[MAX_CONTACTS]{};

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
            CObjectMarble *p = nullptr; // The colliding marble
            CObject       *o = nullptr, // Trigger or respawn object
                          *c = nullptr, // Checkpoint
                          *x = nullptr; // Any colliding object

            if (l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);

              x = l_pOdeNode2;

              if (l_pOdeNode2->m_bTrigger || l_pOdeNode2->m_bRespawn || l_pOdeNode2->m_bMarbleTouch)
                o = l_pOdeNode2;

              if (l_pWorld->m_mCheckpoints.find(l_pOdeNode2->m_iId) != l_pWorld->m_mCheckpoints.end())
                c = l_pOdeNode2;
            }
            else if (l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);

              x = l_pOdeNode1;

              if (l_pOdeNode1->m_bTrigger || l_pOdeNode1->m_bRespawn || l_pOdeNode2->m_bMarbleTouch)
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

              // The marble collides with a non-marble object
              if (o != nullptr) {
                // The other objects triggers
                if (o->m_bTrigger) {
                  irr::core::vector3df l_vPos = irr::core::vector3df((irr::f32)l_aPos[0], (irr::f32)l_aPos[1], (irr::f32)l_aPos[2]);

                  irr::s32 l_iId = p->m_iId - 10000;
                  if (l_iId >= 0 && l_iId < 16) {
                    l_pWorld->handleTrigger(o->m_iTrigger, p->m_iId, p->m_vPosition);
                  }
                }

                // The other object starts a respawn
                if (o->m_bRespawn) {
                  l_pWorld->handleRespawn(p->m_iId);
                }

                // The other object has a marble-touch trigger
                if (o->m_bMarbleTouch) {
                  l_pWorld->handleMarbleTouch(p->m_iId, o->m_iId);
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

        dJointID l_cJoints[MAX_CONTACTS]{};

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

          CObject *l_pObject = nullptr;

          switch (l_pNode->getNodeType()) {
            case scenenodes::CPhysicsNode::enNodeType::Box:
              l_pObject = new CObjectBox(l_pNode, m_pWorld, l_pNode->getName());
              break;

            case scenenodes::CPhysicsNode::enNodeType::Sphere:
              l_pObject = new CObjectSphere(l_pNode, m_pWorld, l_pNode->getName());
              break;

            case scenenodes::CPhysicsNode::enNodeType::Trimesh:
              l_pObject = new CObjectTrimesh(l_pNode, m_pWorld, l_pNode->getName());
              break;
          }

          if (l_pObject != nullptr) {
            for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
              if ((*it)->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_MarbleTouchNodeId) {
                l_pObject->m_bMarbleTouch = true;
                printf("%i has marble touch!\n\n", l_pObject->m_iId);
                break;
              }
            }

            m_pWorld->m_vObjects.push_back(l_pObject);
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
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_MarbleTouchNodeId) {
        scenenodes::CMarbleTouchNode *p = reinterpret_cast<scenenodes::CMarbleTouchNode *>(a_pNode);

        m_mTouchMap[p->getID()] = std::vector<scenenodes::STriggerAction>();

        for (std::vector<scenenodes::STriggerAction>::iterator it = p->m_vActions.begin(); it != p->m_vActions.end(); it++) {
          if ((*it).m_eAction != scenenodes::enAction::None) {
            m_mTouchMap[p->getParent()->getID()].push_back(*it);
          }
        }
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RostrumNodeId) {
        m_pRostrumNode = reinterpret_cast<scenenodes::CRostrumNode *>(a_pNode);
      }

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
        createPhysicsObjects(*it);
      }
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

              l_vSteer.normalize();

              irr::core::vector3df v = p->m_vOffset;
              v.normalize();
              // Marble Class Param: Steer Power and Thrust
              irr::core::vector3df l_vTorque = 70.0f * l_vSteer.X * v + 60.0f * l_vSteer.Y * p->m_vSideVector;

              dBodyAddTorque(p->m_cBody, (dReal)l_vTorque.X, (dReal)l_vTorque.Y, (dReal)l_vTorque.Z);

              if (p->m_bBrake)
                dBodySetAngularDamping(p->m_cBody, (dReal)0.075);
              else
                dBodySetAngularDamping(p->m_cBody, p->m_fDamp);

              if (p->m_iManualRespawn == -1) {
                if (p->m_bRespawn)
                  p->m_iManualRespawn = m_pWorld->m_iWorldStep;
              }
              else {
                if (!p->m_bRespawn)
                  p->m_iManualRespawn = -1;
                else if (m_pWorld->m_iWorldStep - p->m_iManualRespawn > 120) {
                  p->m_iManualRespawn = -1;
                  p->m_iRespawnStart = m_pWorld->m_iWorldStep;
                  p->m_eState = CObjectMarble::enMarbleState::Respawn1;

                  sendPlayerrespawn(p->m_iId, 1, m_pOutputQueue);

                  if (m_pGameLogic != nullptr) {
                    m_pGameLogic->onRespawn(p->m_iId);
                  }
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

            if (p->m_iWithdraw != -1 && m_pWorld->m_iWorldStep > p->m_iWithdraw)
              p->m_iWithdraw = -1;

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
                p->m_iStunnedStart = m_pWorld->m_iWorldStep;
                sendPlayerstunned(p->m_iId, 1, m_pOutputQueue);

                if (m_pGameLogic != nullptr)
                  m_pGameLogic->onMarbleStunned(p->m_iId);
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
                  p->m_iLastContact = m_pWorld->m_iWorldStep;
                  p->m_fDamp = (dReal)0.0015;
                }
                else {
                  int l_iLastContact = m_pWorld->m_iWorldStep - p->m_iLastContact;

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
                p->m_bActive = true;
              }
            }
            
            // The marble is respawning
            if (p->m_iRespawnStart != -1) {
              if (p->m_eState == CObjectMarble::enMarbleState::Respawn1 && m_pWorld->m_iWorldStep - p->m_iRespawnStart >= 180) {
                // Reset up vector
                p->m_vUpVector = irr::core::vector3df(0.0f, 1.0f, 0.0f);
                p->m_vUpOffset = irr::core::vector3df(0.0f, 1.0f, 0.0f);

                // Set state to "Respawn 2"
                p->m_eState = CObjectMarble::enMarbleState::Respawn2;

                // Send message
                sendCamerarespawn(p->m_iId, p->m_vRespawnPos + p->m_vRespawnDir + 3.0f * p->m_vUpOffset, p->m_vRespawnPos, m_pOutputQueue);
              }
              else if (p->m_eState == CObjectMarble::enMarbleState::Respawn2 && m_pWorld->m_iWorldStep - p->m_iRespawnStart >= 360) {
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
            else if (p->m_iStunnedStart != -1 && m_pWorld->m_iWorldStep - p->m_iStunnedStart >= 360) {
              sendPlayerstunned(p->m_iId, 0, m_pOutputQueue);
              p->m_eState = CObjectMarble::enMarbleState::Rolling;
              p->m_iStunnedStart = -1;
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


            if (m_pRostrumNode != nullptr && (p->m_eState == CObjectMarble::enMarbleState::Finished || p->m_eState == CObjectMarble::enMarbleState::Withdrawn) && p->m_iFinishTime > 0) {
              if (m_pWorld->m_iWorldStep > p->m_iFinishTime + 240) {
                irr::core::vector3df l_cPos = m_pRostrumNode->getRostrumPosition(p->m_iPosition - 1);

                p->m_iFinishTime = -1;
                p->m_vPosition = l_cPos;

                dQuaternion q{};
                q[0] = -m_pRostrumNode->getAbsoluteTransformation().getRotationDegrees().Y * M_PI / 180.0;
                q[1] = 0.0;
                q[2] = 1.0;
                q[3] = 0.0;

                dBodySetLinearVel (p->m_cBody, 0.0, 0.0, 0.0);
                dBodySetQuaternion(p->m_cBody, q);
                dBodySetAngularVel(p->m_cBody, 0.0, 0.0, 0.0);
                dBodySetPosition(p->m_cBody, l_cPos.X, l_cPos.Y, l_cPos.Z);

                sendPlayerrostrum(p->m_iId, m_pOutputQueue);
              }
            }

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
          auto l_cOnCountDown = [](int a_iTick, int a_iStep) {
          };

          if (m_pWorld->m_iWorldStep == 0) {
            sendCountdown(4, m_pOutputQueue);
            l_cOnCountDown(4, m_pWorld->m_iWorldStep);
          }
          else {
            int l_iStep = m_pWorld->m_iWorldStep - 240;

            if (l_iStep == 120) {
              sendCountdown(3, m_pOutputQueue);
              l_cOnCountDown(3, m_pWorld->m_iWorldStep);
            }
            else if (l_iStep == 240) {
              sendCountdown(2, m_pOutputQueue);
              l_cOnCountDown(2, m_pWorld->m_iWorldStep);
            }
            else if (l_iStep == 360) {
              sendCountdown(1, m_pOutputQueue);
              l_cOnCountDown(1, m_pWorld->m_iWorldStep);
            }
            else if (l_iStep == 480) {
              sendCountdown(0, m_pOutputQueue);
              l_cOnCountDown(0, m_pWorld->m_iWorldStep);
              m_eGameState = enGameState::Racing;

              for (int i = 0; i < 16; i++)
                if (m_aMarbles[i] != nullptr)
                  startPlayer(i + 10000);
            }
          }
        }

        sendStepmsg(m_pWorld->m_iWorldStep, m_pOutputQueue);

        for (std::vector<scenenodes::STriggerVector>::iterator it = m_vTimerActions.begin(); it != m_vTimerActions.end(); it++) {
          if ((*it).m_vActions.size() > 0) {
            if ((*(*it).m_itAction).m_iStep <= m_pWorld->m_iWorldStep) {
              switch ((*(*it).m_itAction).m_eAction) {
                case scenenodes::enAction::InvertMotor: {
                  for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
                    if ((*it2)->m_iId == (*(*it).m_itAction).m_iNodeId) {
                      if ((*it2)->m_cJoint != nullptr) {
                        if ((*it2)->m_bSliderJoint) {
                          dReal v = dJointGetSliderParam((*it2)->m_cJoint, dParamVel);
                          dReal f = dJointGetSliderParam((*it2)->m_cJoint, dParamFMax);
                          dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, 0.0);
                          dJointSetSliderParam((*it2)->m_cJoint, dParamVel, -v);
                          dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, f);
                        }
                        else {
                          dReal v = dJointGetHingeParam((*it2)->m_cJoint, dParamVel);
                          dReal f = dJointGetHingeParam((*it2)->m_cJoint, dParamFMax);
                          dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, 0.0);
                          dJointSetHingeParam((*it2)->m_cJoint, dParamVel, -v);
                          dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, f);
                        }
                      }
                    }
                  }
                  break;
                }

                case scenenodes::enAction::SetMotorParams: {
                  for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
                    if ((*it2)->m_iId == (*(*it).m_itAction).m_iNodeId) {
                      if ((*it2)->m_cJoint != nullptr) {
                        if ((*it2)->m_bSliderJoint) {
                          dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, (*(*it).m_itAction).m_fForce   );
                          dJointSetSliderParam((*it2)->m_cJoint, dParamVel , (*(*it).m_itAction).m_fVelocity);
                        }
                        else {
                          dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, (*(*it).m_itAction).m_fForce   );
                          dJointSetHingeParam((*it2)->m_cJoint, dParamVel , (*(*it).m_itAction).m_fVelocity);
                        }
                      }
                    }
                  }
                  break;
                }

                case scenenodes::enAction::StopMotor:
                  for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
                    if ((*it2)->m_iId == (*(*it).m_itAction).m_iNodeId) {
                      if ((*it2)->m_cJoint != nullptr) {
                        if ((*it2)->m_bSliderJoint) {
                          dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, 0.0);
                          dJointSetSliderParam((*it2)->m_cJoint, dParamVel , 0.0);
                        }
                        else {
                          dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, 0.0);
                          dJointSetHingeParam((*it2)->m_cJoint, dParamVel , 0.0);
                        }
                      }
                    }
                  }
                  break;

                default:
                  break;
              }

              (*it).m_itAction++;

              if ((*it).m_itAction == (*it).m_vActions.end()) {
                (*it).m_itAction = (*it).m_vActions.begin();
              }

              (*(*it).m_itAction).m_iStep = m_pWorld->m_iWorldStep + (*(*it).m_itAction).m_iTimer;
            }
          }
        }

        m_pWorld->m_iWorldStep++;
        if (m_pGameLogic != nullptr)
          m_pGameLogic->onStep(m_pWorld->m_iWorldStep);
      }

      m_cNextStep = m_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(8);
      std::this_thread::sleep_until(m_cNextStep);
    }

    void CDynamicThread::execute() {
      m_cNextStep = std::chrono::high_resolution_clock::now();

      while (!m_bStopThread) {
        run();
      }

      const std::vector<data::SRacePlayer *> l_vResult = m_pGameLogic->getRacePositions();

      int l_iDiff = 0;

      for (std::vector<data::SRacePlayer*>::const_iterator it = l_vResult.begin(); it != l_vResult.end(); it++) {
        if (it != l_vResult.begin() && !(*it)->m_bFinished) {
          if ((*it)->m_iDeficitL > 0 || (*it)->m_iLapNo == (*(l_vResult.begin()))->m_iLapNo - 1) {
            if ((*it)->m_iDeficitL < l_iDiff) {
              l_iDiff += std::rand() % 480;
              (*it)->m_iDeficitL = l_iDiff;
            }
            else l_iDiff = (*it)->m_iDeficitL;
          }
        }

        sendRaceresult((*it)->serialize(), m_pOutputQueue);
      }

      sendEndracestate(m_pOutputQueue);
      printf("Dynamics thread ends.\n");
    }

    CDynamicThread::CDynamicThread() :
      m_eGameState  (enGameState::Countdown),
      m_eAutoFinish (enAutoFinish::AllPlayers),
      m_pWorld      (nullptr),
      m_bPaused     (false),
      m_fGridAngle  (0.0f),
      m_iPlayers    (0),
      m_iHuman      (0),
      m_pGameLogic  (nullptr),
      m_pRostrumNode(nullptr)
    {
    }

    void CDynamicThread::setupGame(
      scenenodes::CWorldNode* a_pWorld, 
      scenenodes::CStartingGridSceneNode *a_pGrid,
      const std::vector<data::SPlayerData> &a_vPlayers, 
      int a_iLaps, 
      std::vector<scenenodes::STriggerVector> a_vTimerActions, 
      std::vector<gameclasses::CMarbleCounter> a_vMarbleCounters, 
      enAutoFinish a_eAutoFinish
    )
    {
      m_bStopThread = false;
      m_eAutoFinish = a_eAutoFinish;
      m_iPlayers    = (irr::s32)a_vPlayers.size();

      createPhysicsObjects(a_pWorld);

      std::vector<const data::SPlayerData *> l_vPlayers;

      for (std::vector<data::SPlayerData>::const_iterator it = a_vPlayers.begin(); it != a_vPlayers.end(); it++)
        l_vPlayers.push_back(&(*it));

      std::sort(l_vPlayers.begin(), l_vPlayers.end(), [](const data::SPlayerData *p1, const data::SPlayerData *p2) {
        return p1->m_iGridPos < p2->m_iGridPos;
      });

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      if (m_pWorld != nullptr) {
        int l_iIndex = 0;
        for (std::vector<const data::SPlayerData *>::const_iterator it = l_vPlayers.begin(); it != l_vPlayers.end(); it++) {
          if ((*it)->m_eType != data::enPlayerType::Ai)
            m_iHuman++;

          irr::scene::ISceneNode *l_pMarbleNode = a_pGrid->getMarbleById(l_iIndex + 10000);

          l_pMarbleNode->updateAbsolutePosition();
          irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 0.0f, 10.0f);
          l_vOffset.rotateXZBy(m_fGridAngle);

          CObjectMarble* l_pMarble = new CObjectMarble(l_pMarbleNode, l_vOffset, m_pWorld, std::string("Marble #") + std::to_string(l_iIndex + 1));

          l_pMarble->m_vDirection  = l_vOffset;
          l_pMarble->m_vUpVector   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_pMarble->m_vUpOffset   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_pMarble->m_vContact    = irr::core::vector3df();
          l_pMarble->m_vSideVector = l_pMarble->m_vDirection.crossProduct(l_pMarble->m_vUpVector);
          l_pMarble->m_vOffset     = l_vOffset;
          l_pMarble->m_vCamera     = l_pMarbleNode->getAbsolutePosition() - l_vOffset + 3.0f * l_pMarble->m_vUpVector;
          l_pMarble->m_vRearview   = l_pMarbleNode->getAbsolutePosition() + l_vOffset + 3.0f * l_pMarble->m_vUpVector;
          l_pMarble->m_bAiPlayer   = (*it)->m_eType == data::enPlayerType::Ai;

          l_pMarble->m_vSideVector.normalize();
          l_pMarble->m_vDirection .normalize();

          m_pWorld->m_vObjects.push_back(l_pMarble);
          m_aMarbles[l_iIndex] = l_pMarble;
          printf("*** Marble with id %i stored in array index %i\n", l_pMarbleNode->getID(), l_iIndex);

          for (std::map<irr::s32, CObjectCheckpoint*>::iterator it = m_pWorld->m_mCheckpoints.begin(); it != m_pWorld->m_mCheckpoints.end(); it++) {
            if (it->second->m_bLapStart)
              l_pMarble->m_vNextCheckpoints.push_back(it->first);
          }

          printf("Marble assignment: %i to player %s\n", l_pMarbleNode->getID(), (*it)->m_sName.c_str()); 
          sendPlayerassignmarble((*it)->m_iPlayerId, l_pMarbleNode->getID(), m_pOutputQueue);

          sendMarblemoved(l_pMarble->m_iId,
            l_pMarbleNode->getAbsolutePosition(),
            l_pMarbleNode->getRotation(),
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

      if (m_eAutoFinish == enAutoFinish::SecondToLast && m_iHuman <= 2)
        m_eAutoFinish = enAutoFinish::FirstPlayer;

      std::vector<int> l_vMarbleIDs;

      m_pGameLogic = createGameLogic("");
      m_pGameLogic->setNumberOfLaps(a_iLaps);

      for (int i = 0; i < 16; i++)
        if (m_aMarbles[i] != nullptr) {
          l_vMarbleIDs.push_back (m_aMarbles[i]->m_iId);
          m_pGameLogic->addMarble(m_aMarbles[i]->m_iId);
        }

      for (std::vector<scenenodes::STriggerVector>::iterator it = a_vTimerActions.begin(); it != a_vTimerActions.end(); it++)
        m_vTimerActions.push_back(scenenodes::STriggerVector((*it).m_vActions));

      for (std::vector<gameclasses::CMarbleCounter>::iterator it = a_vMarbleCounters.begin(); it != a_vMarbleCounters.end(); it++)
        m_vMarbleCounters.push_back(gameclasses::CMarbleCounter(*it, m_pWorld));

      sendRacesetupdone(m_pOutputQueue);
    }

    CDynamicThread::~CDynamicThread() {
      if (m_pWorld != nullptr)
        delete m_pWorld;

      if (m_pGameLogic != nullptr)
        delete m_pGameLogic;
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
          if (true) { // abs(a_CtrlX > 32) || abs(a_CtrlY > 32)) {
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

            for (std::vector<gameclasses::CMarbleCounter>::iterator it = m_vMarbleCounters.begin(); it != m_vMarbleCounters.end(); it++)
              (*it).marbleRespawn(m_aMarbles[i]->m_iId);
          }
        }
      }

      sendRacefinished(1, m_pOutputQueue);
    }

    /**
     * This function receives messages of type "TogglePause"
     */
    void CDynamicThread::onTogglepause() {
      if (m_eGameState == enGameState::Racing) {
        m_bPaused = !m_bPaused;
        sendPausechanged(m_bPaused, m_pOutputQueue);
      }
    }

    /**
    * This function receives messages of type "PlayerWithdraw"
    * @param a_MarbleId ID of the marble
    */
    void CDynamicThread::onPlayerwithdraw(irr::s32 a_MarbleId) {
      irr::s32 l_iIndex = a_MarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        if (m_aMarbles[l_iIndex]->m_eState == CObjectMarble::enMarbleState::Finished) {
          sendRacefinished(1, m_pOutputQueue);
        }
        else if (m_aMarbles[l_iIndex]->m_iWithdraw == -1) {
          sendConfirmwithdraw(a_MarbleId, 120, m_pOutputQueue);
          m_aMarbles[l_iIndex]->m_iWithdraw = m_pWorld->m_iWorldStep + 120;
        }
        else {
          if (m_pWorld->m_iWorldStep < m_aMarbles[l_iIndex]->m_iWithdraw && m_aMarbles[l_iIndex]->m_eState != CObjectMarble::enMarbleState::Withdrawn) {
            m_aMarbles[l_iIndex]->m_eState = CObjectMarble::enMarbleState::Withdrawn;
            m_aMarbles[l_iIndex]->m_iFinishTime = m_pWorld->m_iWorldStep;
            data::SRacePlayer *l_pPlayer = m_pGameLogic->withdrawPlayer(a_MarbleId, m_pWorld->m_iWorldStep);
            finishPlayer(a_MarbleId, -1, m_aMarbles[l_iIndex]->m_iLapNo);

            if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
              m_aMarbles[l_iIndex]->m_iPosition = l_pPlayer->m_iPos;
            }

            sendRaceposition(l_pPlayer->m_iId, l_pPlayer->m_iPos, l_pPlayer->m_iLapNo, l_pPlayer->m_iDeficitA, l_pPlayer->m_iDeficitL, m_pOutputQueue);
            sendPlayerwithdrawn(a_MarbleId, m_pOutputQueue);
          }
        }
      }
    }

    /**
    * This function receives messages of type "PlayerRemoved"
    * @param a_playerid ID of the removed player
    */
    void CDynamicThread::onPlayerremoved(irr::s32 a_playerid) {
      sendPlayerremoved(a_playerid, m_pOutputQueue);
    }


    void CDynamicThread::handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition) {
      int l_iId = a_iMarble - 10000;

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId]->m_eState != CObjectMarble::enMarbleState::Finished && m_aMarbles[l_iId]->m_eState != CObjectMarble::enMarbleState::Respawn1 && m_aMarbles[l_iId]->m_eState != CObjectMarble::enMarbleState::Respawn2) {
        for (std::vector<gameclasses::CMarbleCounter>::iterator it = m_vMarbleCounters.begin(); it != m_vMarbleCounters.end(); it++) {
          (*it).handleTrigger(a_iTrigger, a_iMarble);
        }
      }
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
        }

        m_aMarbles[l_iId]->m_eState = CObjectMarble::enMarbleState::Respawn1;
        m_aMarbles[l_iId]->m_iRespawnStart = m_pWorld->m_iWorldStep;

        sendPlayerrespawn(a_iMarble, 1, m_pOutputQueue);

        for (std::vector<gameclasses::CMarbleCounter>::iterator it = m_vMarbleCounters.begin(); it != m_vMarbleCounters.end(); it++)
          (*it).marbleRespawn(a_iMarble);

        if (m_pGameLogic != nullptr) {
          m_pGameLogic->onRespawn(a_iMarble);
        }
      }
    }

    /**
    * Callback for sending a "Checkpoint" message
    * @param a_iMarble Id of the marble
    * @param a_iCheckpoint Checkpoint id
    */
    void CDynamicThread::handleCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      sendCheckpoint(a_iMarbleId, a_iCheckpoint, m_pOutputQueue);
      if (m_pGameLogic != nullptr) {
        data::SRacePlayer *l_pPlayer = m_pGameLogic->onCheckpoint(a_iMarbleId, a_iCheckpoint, m_pWorld->m_iWorldStep);
        if (l_pPlayer != nullptr) {
          int l_iIndex = l_pPlayer->m_iId - 10000;
          if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr)
            m_aMarbles[l_iIndex]->m_iPosition = l_pPlayer->m_iPos;

          sendRaceposition(l_pPlayer->m_iId, l_pPlayer->m_iPos, l_pPlayer->m_iLapNo, l_pPlayer->m_iDeficitA, l_pPlayer->m_iDeficitL, m_pOutputQueue);
        }
      }
    }

    /**
    * Callback for sending a "LapStart" message
    * @param a_iMarbleId Id of the marble
    * @param a_iLapNo Number of the started lap
    */
    void CDynamicThread::handleLapStart(int a_iMarbleId, int a_iLapNo) {
      sendLapstart(a_iMarbleId, a_iLapNo, m_pOutputQueue);
      if (m_pGameLogic != nullptr)
        if (m_pGameLogic->onLapStart(a_iMarbleId)) {
          finishPlayer(a_iMarbleId, m_pWorld->m_iWorldStep, a_iLapNo);
        }
    }


    /**
    * Callback for "Marble Touch" Triggers
    * @param a_iMarbleId the ID of the marble
    * @param a_iTouchId the ID of the touched trigger
    */
    void CDynamicThread::handleMarbleTouch(int a_iMarbleId, int a_iTouchId) {
      int l_iId = a_iMarbleId - 10000;

      if (l_iId >= 0 && l_iId < 16 && m_aMarbles[l_iId] != nullptr && m_mTouchMap.find(a_iTouchId) != m_mTouchMap.end()) {
        for (std::vector<scenenodes::STriggerAction>::iterator it = m_mTouchMap[a_iTouchId].begin(); it != m_mTouchMap[a_iTouchId].end(); it++) {
          if ((*it).m_eAction == scenenodes::enAction::CameraUpVector) {
            m_aMarbles[l_iId]->m_vUpVector = (*it).m_vTarget;
          }
        }
      }
    }

    /**
    * Callback for finishing a player
    * @param a_iMarbleId the id of the marbles that has finished the race
    */
    void CDynamicThread::finishPlayer(int a_iMarbleId, int a_iRaceTime, int a_iLaps) {
      int l_iIndex = a_iMarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {

        if (m_aMarbles[l_iIndex]->m_eState != CObjectMarble::enMarbleState::Withdrawn) {
          m_aMarbles[l_iIndex]->m_eState = CObjectMarble::enMarbleState::Finished;
          m_aMarbles[l_iIndex]->m_iFinishTime = m_pWorld->m_iWorldStep;
        }

        sendPlayerfinished(a_iMarbleId, a_iRaceTime, a_iLaps, m_pOutputQueue);

        for (std::vector<gameclasses::CMarbleCounter>::iterator it = m_vMarbleCounters.begin(); it != m_vMarbleCounters.end(); it++)
          (*it).marbleRespawn(a_iMarbleId);

        int l_iFinished = 0;
          

        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr && 
              (m_aMarbles[i]->m_eState == CObjectMarble::enMarbleState::Finished || m_aMarbles[i]->m_eState == CObjectMarble::enMarbleState::Withdrawn) && 
              (!m_aMarbles[i]->m_bAiPlayer || m_eAutoFinish == enAutoFinish::PlayersAndAI)
            )
          {
            l_iFinished++;
          }
        }

        bool l_bFinish = false;

        switch (m_eAutoFinish) {
          case enAutoFinish::AllPlayers  : l_bFinish = l_iFinished == m_iHuman    ; break;
          case enAutoFinish::SecondToLast: l_bFinish = l_iFinished == m_iHuman - 1; break;
          case enAutoFinish::FirstPlayer : l_bFinish = l_iFinished >  0           ; break;
          case enAutoFinish::PlayersAndAI: l_bFinish = l_iFinished == m_iPlayers  ; break;
        }

        if (l_bFinish) {
          sendRacefinished(0, m_pOutputQueue);
        }
      }
    }

    /**
    * LUA callback for starting a player
    * @param a_iMarble the marble id
    */
    void CDynamicThread::startPlayer(int a_iMarble) {
      int l_iIndex = a_iMarble - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        if (m_aMarbles[l_iIndex]->m_eState != gameclasses::CObjectMarble::enMarbleState::Withdrawn)
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