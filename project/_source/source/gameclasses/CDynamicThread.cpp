// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/lua/CLuaSingleton_physics.h>
#include <scenenodes/CStartingGridSceneNode.h>
#include <_generated/lua/CLuaScript_physics.h>
#include <_generated/messages/CMessages.h>
#include <scenenodes/CCheckpointNode.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <gameclasses/IGameLogic.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/COdeNodes.h>
#include <helpers/CStringHelpers.h>
#include <gfx/SViewPort.h>
#include <exception>
#include <CGlobal.h>
#include <ode/ode.h>
#include <algorithm>
#include <cstdlib>

constexpr auto MAX_CONTACTS = 16;
const double GRAD_PI = 180.0 / 3.1415926535897932384626433832795;

namespace dustbin {
  namespace gameclasses {

#ifdef _DEBUG
    int g_iCfm;
#endif

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

        CObject *l_pOdeNode1 = (CObject*)dGeomGetData(a_iGeom1);
        CObject *l_pOdeNode2 = (CObject*)dGeomGetData(a_iGeom2);

        if ((l_pOdeNode1->getType() != enObjectType::Marble && l_pOdeNode2->m_bMarbleOnly) || (l_pOdeNode2->getType() != enObjectType::Marble && l_pOdeNode1->m_bMarbleOnly))
          return;

        bool l_bMarbleCollision = l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() == enObjectType::Marble;

        // If two marbles collide we need some sort of "special" surface parameters
        if (l_bMarbleCollision) {
          for (irr::u32 i = 0; i < MAX_CONTACTS; i++) {
            l_cContact[i].surface.bounce = (dReal)0.5;
            l_cContact[i].surface.bounce_vel = 25.0;
            l_cContact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftCFM | dContactSoftERP;
            l_cContact[i].surface.mu = (dReal)0;
            l_cContact[i].surface.mu2 = (dReal)0;
            l_cContact[i].surface.soft_cfm = (dReal)0.001;
            l_cContact[i].surface.soft_erp = (dReal)0.2;
            l_cContact[i].surface.rho = (dReal)0.9;
            l_cContact[i].surface.rho2 = (dReal)0.9;
            l_cContact[i].surface.slip1 = (dReal)0.5;
            l_cContact[i].surface.slip2 = (dReal)0.5;
          }
        }
        else {
          CObjectMarble *l_pMarble = l_pOdeNode1->getType() == enObjectType::Marble ? reinterpret_cast<CObjectMarble *>(l_pOdeNode1) : l_pOdeNode2->getType() == enObjectType::Marble? reinterpret_cast<CObjectMarble *>(l_pOdeNode2) : nullptr;

          dReal l_fMaxERP = 1.0;

          if (l_pMarble != nullptr) {
            int l_iSinceLastContact = l_pWorld->m_iWorldStep - l_pMarble->m_iGroundContact;

            if (l_iSinceLastContact > 30) {
              l_pMarble->m_iLastJump = l_pWorld->m_iWorldStep;
              l_fMaxERP = 0.2;
            }
            else {
              int l_iSinceLast = l_pWorld->m_iWorldStep - l_pMarble->m_iLastJump;
              if (l_iSinceLast > 90) {
                l_fMaxERP = 1.0f;
              }
              else {
                l_fMaxERP = 0.2 + 0.8 * (dReal)l_iSinceLast / 90.0;
              }
            }
          }

          irr::core::vector3df l_cVel = irr::core::vector3df();

          bool l_bInCfm = false;

          if (l_pOdeNode1->m_cBody != 0)
            l_cVel = vectorOdeToIrr(dBodyGetLinearVel(l_pOdeNode1->m_cBody));
          else if (l_pOdeNode2->m_cBody != 0)
            l_cVel = vectorOdeToIrr(dBodyGetLinearVel(l_pOdeNode2->m_cBody));

          irr::f32 l_fVel = l_cVel.getLengthSQ();

          if (l_fVel < 50.0f)
            l_fVel = 50.0f;
          else if (l_fVel > 500.0f)
            l_fVel = 500.0f;

          l_fVel = (l_fVel - 50.0f) / 400.0f;

          dReal l_fSoftErp = std::min((float)0.8, (float)(l_fMaxERP * l_fVel));

          for (irr::u32 i = 0; i < MAX_CONTACTS; i++) {
            l_cContact[i].surface.bounce = (dReal)0.15;
            l_cContact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactRolling;

            if (l_fSoftErp > 0.0) l_cContact[i].surface.mode |= dContactSoftERP;

            if (l_pMarble != nullptr && l_pMarble->m_bInCfm) {
              l_cContact[i].surface.mode |= dContactSoftCFM;
              l_cContact[i].surface.soft_cfm = l_pMarble->m_fCfmZone;
            }
            else l_cContact[i].surface.soft_cfm = 0.0;

            l_cContact[i].surface.mu = (dReal)1500;
            l_cContact[i].surface.mu2 = (dReal)0;
            l_cContact[i].surface.bounce_vel = (dReal)25.0;
            
            l_cContact[i].surface.soft_erp = l_fSoftErp;
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
            CObject       *o = nullptr; // Trigger or respawn object
            CObject       *c = nullptr; // Checkpoint
            CObject       *x = nullptr; // Any colliding object

            if (l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);

              x = l_pOdeNode2;

              if (l_pOdeNode2->m_bTrigger || l_pOdeNode2->m_bRespawn || l_pOdeNode2->m_bMarbleTouch || l_pOdeNode2->m_bCfmEnter || l_pOdeNode2->m_bCfmExit)
                o = l_pOdeNode2;

              if (l_pWorld->m_mCheckpoints.find(l_pOdeNode2->m_iId) != l_pWorld->m_mCheckpoints.end())
                c = l_pOdeNode2;
            }
            else if (l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble) {
              p = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);

              x = l_pOdeNode1;

              if (l_pOdeNode1->m_bTrigger || l_pOdeNode1->m_bRespawn || l_pOdeNode1->m_bMarbleTouch || l_pOdeNode1->m_bCfmEnter || l_pOdeNode1->m_bCfmExit)
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
                    l_pWorld->handleTrigger(o->m_iTrigger, p->m_iId, p->m_vPosition, p->m_mStepTriggers[p->m_iSecondTrigger].find(o->m_iTrigger) == p->m_mStepTriggers[p->m_iSecondTrigger].end());

                    p->m_mStepTriggers[p->m_iActiveTrigger][o->m_iTrigger] = true;
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

                if (o->m_bCfmEnter && !p->m_bInCfm && p->m_eState == CObjectMarble::enMarbleState::Rolling) {
                  p->m_bInCfm   = true;
                  p->m_fCfmZone = o->m_fCfmValue;
#ifdef _DEBUG
                  g_iCfm++;
                  printf("Cfm: %i [+] - %.4f\n", g_iCfm, p->m_fCfmZone);
#endif
                }
                 
                if (o->m_bCfmExit && p->m_bInCfm) {
#ifdef _DEBUG
                  if (p->m_bInCfm) {
                    g_iCfm--;
                    printf("Cfm: %i [-]\n", g_iCfm);
                  }
#endif
                  p->m_bInCfm = false;
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
            p->m_iGroundContact = l_pWorld->m_iWorldStep;
            p->m_bHasContact = true;
            p->m_vContact = irr::core::vector3df((irr::f32)l_cContact[i].geom.pos[0], (irr::f32)l_cContact[i].geom.pos[1], (irr::f32)l_cContact[i].geom.pos[2]);
          }

          if (l_pOdeNode2 != nullptr && l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble && l_pOdeNode1->m_bCollides) {
            CObjectMarble* p = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);
            p->m_iGroundContact = l_pWorld->m_iWorldStep;
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
              printf("\n*** Create Trimesh: %s (%i)\n\n", l_pNode->getName(), l_pNode->getID());
              l_pObject = new CObjectTrimesh(l_pNode, m_pWorld, l_pNode->getName());
              break;
          }

          if (l_pObject != nullptr) {
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
          m_pOutputQueue->postMessage(l_pMsg);
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

              p->m_bHasContact = false;

              irr::f32 l_fVel = p->m_vVelocity.getLength();
              irr::f32 l_fFac = 1.0f;
              
              if (l_fVel > 140.0f) {
                l_fFac = 1.0f - (l_fVel - 140.0f) / 15.0f;
              }

              // Marble Class Param: Steer Power and Thrust
              irr::core::vector3df l_vTorque = (p->m_bHasContact ? 20.0f : p->m_fSteerPower) * l_vSteer.X * v + l_fFac * p->m_fThrustPower * l_vSteer.Y * p->m_vSideVector;

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
#ifdef _DEBUG
                  if (p->m_bInCfm) {
                    g_iCfm--;
                    printf("Cfm: %i [Respawn M]\n", g_iCfm);
                  }
#endif
                  p->m_iManualRespawn = -1;
                  p->m_iRespawnStart = m_pWorld->m_iWorldStep;
                  p->m_eState        = CObjectMarble::enMarbleState::Respawn1;
                  p->m_bInCfm        = false;
                  p->m_fCfmValue     = 0.01f;

                  sendPlayerrespawn(p->m_iId, 1, m_pOutputQueue);

                  if (m_pGameLogic != nullptr) {
                    m_pGameLogic->onRespawn(p->m_iId);
                  }

                  if (m_pLuaScript != nullptr) {
                    m_pLuaScript->onplayerrespawn(p->m_iId, 1);
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

            const dReal *l_aPos    = dBodyGetPosition(p->m_cBody);
            const dReal *l_aRot    = dBodyGetQuaternion(p->m_cBody);
            const dReal *l_aLinVel = dBodyGetLinearVel(p->m_cBody);
            const dReal *l_aAngVel = dBodyGetAngularVel(p->m_cBody);

            irr::core::vector3df l_vLinVel = vectorOdeToIrr(l_aLinVel);
            irr::core::vector3df l_vAngVel = vectorOdeToIrr(l_aAngVel);

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

                p->m_vCamera = p->m_vOffset;

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
              p->m_vCamera     = p->m_vOffset;
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

                if (m_pLuaScript != nullptr) {
                  m_pLuaScript->onplayerrespawn(p->m_iId, 2);
                }
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
                  p->m_vCamera       = p->m_vOffset;
                  p->m_vSideVector   = p->m_vOffset.crossProduct(p->m_vUpOffset);
                  p->m_vSideVector.normalize();

                  // Reset state of marble
                  p->m_eState  = CObjectMarble::enMarbleState::Rolling;
                  p->m_bActive = false;

                  // Send message to game state
                  sendPlayerrespawn(p->m_iId, 2, m_pOutputQueue);

                  if (m_pLuaScript != nullptr) {
                    m_pLuaScript->onplayerrespawn(p->m_iId, 3);
                  }
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
              vectorOdeToIrr(l_aAngVel),
              p->m_vCamera,
              p->m_vUpOffset, 
              p->m_iCtrlX,
              p->m_iCtrlY, 
              p->m_bHasContact,
              p->m_bBrake,
              p->m_bRearView,
              p->m_bRespawn,
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

            p->m_mStepTriggers[p->m_iSecondTrigger].clear();

            if (p->m_iActiveTrigger == 0) {
              p->m_iActiveTrigger = 1;
              p->m_iSecondTrigger = 0;
            }
            else {
              p->m_iActiveTrigger = 0;
              p->m_iSecondTrigger = 1;
            }
          }
        }

        // Send all other moving objects to the game state
        for (std::vector<CObject*>::iterator it = m_pWorld->m_vMoving.begin(); it != m_pWorld->m_vMoving.end(); it++) {
          const dReal *l_aPos    = dBodyGetPosition  ((*it)->m_cBody);
          const dReal *l_aRot    = dBodyGetQuaternion((*it)->m_cBody);
          const dReal *l_aLinVel = dBodyGetLinearVel ((*it)->m_cBody);
          const dReal *l_aAngVel = dBodyGetAngularVel((*it)->m_cBody);

          sendObjectmoved((*it)->m_iId, vectorOdeToIrr(l_aPos), quaternionToEuler(l_aRot), vectorOdeToIrr(l_aLinVel), vectorOdeToIrr(l_aAngVel), m_pOutputQueue);
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

        m_pWorld->m_iWorldStep++;

        if (m_pGameLogic != nullptr)
          m_pGameLogic->onStep(m_pWorld->m_iWorldStep);

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onstep(m_pWorld->m_iWorldStep);
      }

      m_cNextStep = m_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(8);
      // m_cNextStep = m_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(6);
      std::this_thread::sleep_until(m_cNextStep);
    }

    /**
    * This function receives messages of type "ObjectMoved"
    * @param a_ObjectId The ID of the object
    * @param a_Position The current position
    * @param a_Rotation The current rotation (Euler angles)
    * @param a_LinearVelocity The linear velocity
    * @param a_AngularVelocity The angualar (rotation) velocity
    */
    void CDynamicThread::onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity) {
      if (m_pWorld != nullptr) {
        for (auto& p: m_pWorld->m_vMoving) {
          if (p->m_iId == a_ObjectId) {
            dBodySetPosition  (p->m_cBody, (dReal)a_Position      .X, (dReal)a_Position      .Y, (dReal)a_Position      .Z);
            dBodySetLinearVel (p->m_cBody, (dReal)a_LinearVelocity.X, (dReal)a_LinearVelocity.Y, (dReal)a_LinearVelocity.Z);
            dBodySetAngularVel(p->m_cBody, (dReal)a_AngularVelocity.X, (dReal)a_AngularVelocity.Y, (dReal)a_AngularVelocity.Z);

            dMatrix3 l_cRotation;
            dRFromEulerAngles(l_cRotation, (dReal)a_Rotation.X, (dReal)a_Rotation.Y, (dReal)a_Rotation.Z);
            dBodySetRotation (p->m_cBody, l_cRotation);

            break;
          }
        }
      }
    }

    /**
    * This function receives messages of type "MarbleMoved"
    * @param a_ObjectId The ID of the object
    * @param a_Position The current position
    * @param a_Rotation The current rotation (Euler angles)
    * @param a_LinearVelocity The linear velocity
    * @param a_AngularVelocity The angualar (rotation) velocity
    * @param a_CameraPosition The position of the camera
    * @param a_CameraUp The Up-Vector of the camera
    * @param a_ControlX The marble's current controller state in X-Direction
    * @param a_ControlY The marble's current controller state in Y-Direction
    * @param a_Contact A Flag indicating whether or not the marble is in contact with another object
    * @param a_ControlBrake Flag indicating whether or not the marble's brake is active
    * @param a_ControlRearView Flag indicating whether or not the marble's player looks behind
    * @param a_ControlRespawn Flag indicating whether or not the manual respawn button is pressed 
    */
    void CDynamicThread::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, const irr::core::vector3df &a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      int l_iIndex = a_ObjectId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        // printf("Got marble update for   %i\n", l_iIndex - 10000);
        gameclasses::CObjectMarble *p = m_aMarbles[l_iIndex];

        p->m_bActive = true;

        dBodySetPosition  (p->m_cBody, (dReal)a_Position      .X, (dReal)a_Position      .Y, (dReal)a_Position      .Z);
        dBodySetLinearVel (p->m_cBody, (dReal)a_LinearVelocity.X, (dReal)a_LinearVelocity.Y, (dReal)a_LinearVelocity.Z);
        dBodySetAngularVel(p->m_cBody, (dReal)a_AngularVelocity.X, (dReal)a_AngularVelocity.Y, (dReal)a_AngularVelocity.Z);

        dMatrix3 l_cRotation;
        dRFromEulerAngles(l_cRotation, (dReal)a_Rotation.X, (dReal)a_Rotation.Y, (dReal)a_Rotation.Z);
        dBodySetRotation (p->m_cBody, l_cRotation);

        p->m_bBrake    = a_ControlBrake;
        p->m_bCollides = a_Contact;
        p->m_vCamera   = a_CameraPosition;
        p->m_bRearView = a_ControlRearView;
        p->m_bRespawn  = a_ControlRespawn;
        p->m_iCtrlX    = (int)a_ControlX;
        p->m_iCtrlY    = (int)a_ControlY;
      }
    }

    /**
    * This function receives messages of type "StepUpdate"
    * @param a_StepNo The step received from the server
    */
    void CDynamicThread::onStepupdate(irr::s32 a_StepNo) {
      if (m_pWorld != nullptr) {
        printf("Got step update: %i / %i\n", a_StepNo, m_pWorld->m_iWorldStep);
        m_pWorld->m_iWorldStep = a_StepNo;
      }
    }

    /**
    * This function receives messages of type "ServerDisconnect"
    */
    void CDynamicThread::onServerdisconnect() {
      messages::CServerDisconnect l_cMsg = messages::CServerDisconnect();
      m_pOutputQueue->postMessage(&l_cMsg);
    }

    void CDynamicThread::execute() {
      m_cNextStep = std::chrono::high_resolution_clock::now();

      while (!m_bStopThread) {
        run();
      }

      const std::vector<data::SRacePlayer *> l_vResult = m_pGameLogic->getRacePositions();

      int l_iDiff = 0;

      data::SRacePlayer *l_pOther = nullptr;

      for (std::vector<data::SRacePlayer*>::const_iterator it = l_vResult.begin(); it != l_vResult.end(); it++) {
        if (it != l_vResult.begin() && !(*it)->m_bFinished) {
          (*it)->finishPlayer(l_pOther);
        }

        l_pOther = *it;
      }

      for (std::vector<data::SRacePlayer*>::const_iterator it = l_vResult.begin(); it != l_vResult.end(); it++) {
        sendRaceresult((*it)->serialize(), m_pOutputQueue);
      }

      sendEndracestate(m_pOutputQueue);
      printf("Dynamics thread ends.\n");
    }

    CDynamicThread::CDynamicThread(bool a_bNetworkClient) :
      m_eGameState    (enGameState::Countdown),
      m_eAutoFinish   (data::SGameSettings::enAutoFinish::AllPlayers),
      m_pWorld        (nullptr),
      m_bPaused       (false),
      m_fGridAngle    (0.0f),
      m_iPlayers      (0),
      m_iHuman        (0),
      m_pGameLogic    (nullptr),
      m_pRostrumNode  (nullptr),
      m_pLuaScript    (nullptr),
      m_sLuaError     (""),
      m_bNetworkClient(a_bNetworkClient)
    {
#ifdef _DEBUG
      g_iCfm = 0;
#endif
    }

    bool CDynamicThread::setupGame(
      scenenodes::CWorldNode* a_pWorld, 
      scenenodes::CStartingGridSceneNode *a_pGrid,
      const std::vector<data::SPlayerData> &a_vPlayers, 
      int a_iLaps, 
      const std::string &a_sLuaScript,
      data::SGameSettings::enAutoFinish a_eAutoFinish
    )
    {
      helpers::addToDebugLog("    CDynamicThread::setupGame {");
      m_bStopThread = false;
      m_eAutoFinish = a_eAutoFinish;
      m_iPlayers    = (irr::s32)a_vPlayers.size();

      helpers::addToDebugLog("    Create phyics objects");
      createPhysicsObjects(a_pWorld);

      std::vector<const data::SPlayerData *> l_vPlayers;

      helpers::addToDebugLog("    Create player objects");
      for (std::vector<data::SPlayerData>::const_iterator it = a_vPlayers.begin(); it != a_vPlayers.end(); it++)
        l_vPlayers.push_back(&(*it));

      helpers::addToDebugLog("    Sort players");
      std::sort(l_vPlayers.begin(), l_vPlayers.end(), [](const data::SPlayerData *p1, const data::SPlayerData *p2) {
        return p1->m_iGridPos < p2->m_iGridPos;
      });

      helpers::addToDebugLog("    clear marble object array");
      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      if (m_pWorld != nullptr) {
        int l_iIndex = 0;

        for (std::vector<const data::SPlayerData *>::const_iterator it = l_vPlayers.begin(); it != l_vPlayers.end(); it++) {
         

          l_iIndex++;
        }
      }

      helpers::addToDebugLog("    Check auto finish");
      if (m_eAutoFinish == data::SGameSettings::enAutoFinish::SecondToLast && m_iHuman <= 2)
        m_eAutoFinish = data::SGameSettings::enAutoFinish::FirstPlayer;

      helpers::addToDebugLog("    Create game logic");

      m_pGameLogic = createGameLogic("");
      m_pGameLogic->setNumberOfLaps(a_iLaps);

      helpers::addToDebugLog("    Save marble IDs");

      if (a_sLuaScript != "") {
        helpers::addToDebugLog("    Create LUA script");
        m_pLuaScript = new lua::CLuaScript_physics(a_sLuaScript);

        if (m_pLuaScript->getError() == "") {
          m_pLuaScript->setWorld(m_pWorld, m_aMarbles, this);
          m_pLuaScript->initialize();
        }
        else {
          m_sLuaError = m_pLuaScript->getError();
          return false;
        }
      }

      // helpers::addToDebugLog("    Send race setup done");
      // sendRacesetupdone(m_pOutputQueue);

      helpers::addToDebugLog("    CDynamicThread::setupGame }");
      return true;
    }

    CDynamicThread::~CDynamicThread() {
      helpers::addToDebugLog("CDynamicThread::~CDynamicThread() {");
      if (m_pWorld != nullptr) {
        helpers::addToDebugLog("Delete world");
        delete m_pWorld;
      }

      if (m_pGameLogic != nullptr) {
        helpers::addToDebugLog("Delete game logic");
        delete m_pGameLogic;
      }

      if (m_pLuaScript != nullptr) {
        helpers::addToDebugLog("Delete LUA script");
        delete m_pLuaScript;
      }

      helpers::addToDebugLog("CDynamicThread::~CDynamicThread() }");
    }

    /**
    * Assign a player to a marble
    * @param a_pPlayer the player
    * @param a_pMarbleNode scene node of the marble
    */
    void CDynamicThread::assignPlayerToMarble(data::SPlayerData* a_pPlayer, irr::scene::ISceneNode* a_pMarbleNode) {
      if (a_pPlayer->m_eType != data::enPlayerType::Ai)
        m_iHuman++;

      a_pMarbleNode->updateAbsolutePosition();
      irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 0.0f, 10.0f);
      l_vOffset.rotateXZBy(m_fGridAngle);

      CObjectMarble* l_pMarble = new CObjectMarble(a_pMarbleNode, l_vOffset, m_pWorld, std::string("Marble #") + std::to_string(a_pPlayer->m_iGridPos));

      l_pMarble->m_vDirection  = l_vOffset;
      l_pMarble->m_vUpVector   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
      l_pMarble->m_vUpOffset   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
      l_pMarble->m_vContact    = irr::core::vector3df();
      l_pMarble->m_vSideVector = l_pMarble->m_vDirection.crossProduct(l_pMarble->m_vUpVector);
      l_pMarble->m_vOffset     = l_vOffset;
      l_pMarble->m_vCamera     = l_vOffset;
      l_pMarble->m_bAiPlayer   = a_pPlayer->m_eType == data::enPlayerType::Ai;

      l_pMarble->m_vSideVector.normalize();
      l_pMarble->m_vDirection .normalize();

      float l_fPowerFactor = 1.0f;

      if (a_pPlayer->m_eAiHelp == data::SPlayerData::enAiHelp::BotMb3 || a_pPlayer->m_sControls == "class=marble3") {
        l_fPowerFactor = 0.7f;
      }
      else if (a_pPlayer->m_eAiHelp == data::SPlayerData::enAiHelp::BotMb2 || a_pPlayer->m_sControls == "class=marble2") {
        l_fPowerFactor = 0.85f;
      }

      l_fPowerFactor += a_pPlayer->m_fDeviation;

      printf("Power Factor: %.5f, %.5f\n", l_fPowerFactor, a_pPlayer->m_fDeviation);

      l_pMarble->m_fSteerPower  = l_fPowerFactor * l_pMarble->m_fSteerPower;
      l_pMarble->m_fThrustPower = l_fPowerFactor * l_pMarble->m_fThrustPower;

      m_pWorld->m_vObjects.push_back(l_pMarble);

      for (int i = 0; i < 16; i++) {
        if (m_aMarbles[i] == nullptr) {
          m_aMarbles[i] = l_pMarble;
          m_pGameLogic->addMarble(m_aMarbles[i]->m_iId);
          printf("*** Marble with id %i stored in array index %i\n", a_pMarbleNode->getID(), i);
          break;
        }
      }

      for (std::map<irr::s32, CObjectCheckpoint*>::iterator it = m_pWorld->m_mCheckpoints.begin(); it != m_pWorld->m_mCheckpoints.end(); it++) {
        if (it->second->m_bLapStart)
          l_pMarble->m_vNextCheckpoints.push_back(it->first);
      }

      printf("Marble assignment: %i to player %s\n", a_pMarbleNode->getID(), a_pPlayer->m_sName.c_str());
      // sendPlayerassignmarble((*it)->m_iPlayerId, l_pMarbleNode->getID(), m_pOutputQueue);

      sendMarblemoved(l_pMarble->m_iId,
        a_pMarbleNode->getAbsolutePosition(),
        a_pMarbleNode->getRotation(),
        irr::core::vector3df(0.0f, 0.0f, 0.0f),
        irr::core::vector3df(0.0f, 0.0f, 0.0f),
        l_pMarble->m_vCamera,
        irr::core::vector3df(0.0f, 1.0f, 0.0f),
        0,
        0,
        false,
        false,
        false,
        false,
        m_pOutputQueue);
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

      }
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


    /**
     * This function receives messages of type "CancelRace"
     */
    void CDynamicThread::onCancelrace() {
      for (int i = 0; i < 16; i++) {
        if (m_aMarbles[i] != nullptr) {
          if (m_aMarbles[i]->m_eState != CObjectMarble::enMarbleState::Finished) {
#ifdef _DEBUG
            if (m_aMarbles[i]->m_bInCfm) {
              g_iCfm--;
              printf("Cfm: %i [Cancel]\n", g_iCfm);
            }
#endif
            m_aMarbles[i]->m_eState        = CObjectMarble::enMarbleState::Finished;
            m_aMarbles[i]->m_iStunnedStart = -1;
            m_aMarbles[i]->m_bInCfm        = false;
            m_aMarbles[i]->m_fCfmZone      = 0.01f;
            sendPlayerfinished(m_aMarbles[i]->m_iId, -1, m_aMarbles[i]->m_iLapNo, m_pOutputQueue);

            if (m_pLuaScript != nullptr)
              m_pLuaScript->onplayerfinished(m_aMarbles[i]->m_iId, -1, m_aMarbles[i]->m_iLapNo);
          }
        }
      }

      sendRacefinished(1, m_pOutputQueue);
      if (m_pLuaScript != nullptr)
        m_pLuaScript->onracefinished(true);
    }

    /**
     * This function receives messages of type "TogglePause"
     * @param a_MarbleID ID of the marble requesting the pause change
     */
    void CDynamicThread::onTogglepause(irr::s32 a_MarbleID) {
      if (m_eGameState == enGameState::Racing) {
        irr::s32 l_iIndex = a_MarbleID - 10000;

        if (l_iIndex >= 0 && l_iIndex < 16) {
          if (m_aMarbles[l_iIndex] != nullptr && m_aMarbles[l_iIndex]->m_eState != CObjectMarble::enMarbleState::Finished && m_aMarbles[l_iIndex]->m_eState != CObjectMarble::enMarbleState::Withdrawn) {
            m_bPaused = !m_bPaused;
            sendPausechanged(m_bPaused, m_pOutputQueue);
          }
        }
      }
    }

    /**
    * This function receives messages of type "PlayerWithdraw"
    * @param a_MarbleId ID of the marble
    */
    void CDynamicThread::onPlayerwithdraw(irr::s32 a_MarbleId) {
      irr::s32 l_iIndex = a_MarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
        if (m_aMarbles[l_iIndex]->m_eState == CObjectMarble::enMarbleState::Finished || m_aMarbles[l_iIndex]->m_eState == CObjectMarble::enMarbleState::Withdrawn) {
          sendRacefinished(1, m_pOutputQueue);

          if (m_pLuaScript != nullptr)
            m_pLuaScript->onracefinished(true);
        }
        else if (m_aMarbles[l_iIndex]->m_iWithdraw == -1) {
          if (!m_bPaused) {
            sendConfirmwithdraw(a_MarbleId, 120, m_pOutputQueue);
            m_aMarbles[l_iIndex]->m_iWithdraw = m_pWorld->m_iWorldStep + 120;
          }
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

            sendRaceposition(l_pPlayer->m_iId, l_pPlayer->m_iPos, l_pPlayer->getLapNo(), l_pPlayer->m_iDeficitA, l_pPlayer->m_iDeficitL, m_pOutputQueue);
            printf("Withdraw@dynamics: %i, pos: %i\n", l_pPlayer->m_iId, l_pPlayer->m_iPos);
            sendPlayerwithdrawn(a_MarbleId, m_pOutputQueue);

            if (m_pLuaScript != nullptr) {
              m_pLuaScript->onraceposition(l_pPlayer->m_iId, l_pPlayer->m_iPos, l_pPlayer->getLapNo(), l_pPlayer->m_iDeficitA, l_pPlayer->m_iDeficitL);
              m_pLuaScript->onplayerwithdrawn(a_MarbleId);
            }
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


    void CDynamicThread::handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition, bool a_bBroadcast) {
      if (a_bBroadcast)
        sendTrigger(a_iTrigger, a_iMarble, m_pOutputQueue);

      if (m_pLuaScript != nullptr)
        m_pLuaScript->ontrigger(a_iMarble, a_iTrigger);
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

#ifdef _DEBUG
        if (m_aMarbles[l_iId]->m_bInCfm) {
          g_iCfm--;
          printf("Cfm: %i [Respawn]\n", g_iCfm);
        }
#endif

        m_aMarbles[l_iId]->m_eState = CObjectMarble::enMarbleState::Respawn1;
        m_aMarbles[l_iId]->m_iRespawnStart = m_pWorld->m_iWorldStep;
        m_aMarbles[l_iId]->m_bInCfm = false;
        m_aMarbles[l_iId]->m_fCfmZone = 0.01f;

        sendPlayerrespawn(a_iMarble, 1, m_pOutputQueue);

        if (m_pGameLogic != nullptr) {
          m_pGameLogic->onRespawn(a_iMarble);
        }

        if (m_pLuaScript != nullptr) {
          m_pLuaScript->onplayerrespawn(a_iMarble, 1);
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
          if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {
            m_aMarbles[l_iIndex]->m_iPosition = l_pPlayer->m_iPos;
            sendRaceposition(l_pPlayer->m_iId, l_pPlayer->m_iPos, l_pPlayer->m_iLapNo, l_pPlayer->m_iDeficitA, l_pPlayer->m_iDeficitL, m_pOutputQueue);
          }
        }
      }

      if (m_pLuaScript != nullptr)
        m_pLuaScript->oncheckpoint(a_iMarbleId, a_iCheckpoint);
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
    }

    /**
    * Callback for finishing a player
    * @param a_iMarbleId the id of the marbles that has finished the race
    */
    void CDynamicThread::finishPlayer(int a_iMarbleId, int a_iRaceTime, int a_iLaps) {
      int l_iIndex = a_iMarbleId - 10000;
      if (l_iIndex >= 0 && l_iIndex < 16 && m_aMarbles[l_iIndex] != nullptr) {

        if (m_aMarbles[l_iIndex]->m_eState != CObjectMarble::enMarbleState::Withdrawn) {
          if (m_aMarbles[l_iIndex]->m_eState == CObjectMarble::enMarbleState::Stunned) {
            // Reset stunned state if a stunned player finishes the race
            m_aMarbles[l_iIndex]->m_iStunnedStart = -1;
            sendPlayerstunned(a_iMarbleId, 2, m_pOutputQueue);
          }

          m_aMarbles[l_iIndex]->m_eState      = CObjectMarble::enMarbleState::Finished;
          m_aMarbles[l_iIndex]->m_iFinishTime = m_pWorld->m_iWorldStep;
        }

#ifdef _DEBUG
        if (m_aMarbles[l_iIndex]->m_bInCfm) {
          g_iCfm--;
          printf("Cfm: %i [Finish]\n", g_iCfm);
        }
#endif
        m_aMarbles[l_iIndex]->m_bInCfm = false;
        m_aMarbles[l_iIndex]->m_fCfmZone = 0.01f;

        printf("Finished: %i\n", l_iIndex);
        sendPlayerfinished(a_iMarbleId, a_iRaceTime, a_iLaps, m_pOutputQueue);

        if (m_pLuaScript != nullptr)
          m_pLuaScript->onplayerfinished(a_iMarbleId, a_iRaceTime, a_iLaps);

        int l_iFinished = 0;
          

        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr && 
              (m_aMarbles[i]->m_eState == CObjectMarble::enMarbleState::Finished || m_aMarbles[i]->m_eState == CObjectMarble::enMarbleState::Withdrawn) && 
              (!m_aMarbles[i]->m_bAiPlayer || m_eAutoFinish == data::SGameSettings::enAutoFinish::AllAndAi)
            )
          {
            l_iFinished++;
          }
        }

        bool l_bFinish = false;

        switch (m_eAutoFinish) {
          case data::SGameSettings::enAutoFinish::AllPlayers  : l_bFinish = l_iFinished == m_iHuman    ; break;
          case data::SGameSettings::enAutoFinish::SecondToLast: l_bFinish = l_iFinished == m_iHuman - 1; break;
          case data::SGameSettings::enAutoFinish::FirstPlayer : l_bFinish = l_iFinished >  0           ; break;
          case data::SGameSettings::enAutoFinish::AllAndAi    : l_bFinish = l_iFinished == m_iPlayers  ; break;
        }

        if (l_bFinish) {
          sendRacefinished(0, m_pOutputQueue);
          if (m_pLuaScript != nullptr)
            m_pLuaScript->onracefinished(false);
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
    * A function for the physics LUA script to send data to the AI and scene scripts
    * @param a_iNumberOne first number
    * @param a_iNumberTwo other number
    * @param a_sData string data
    */
    void CDynamicThread::sendMessageFromLUA(int a_iNumberOne, int a_iNumbetTwo, const std::string& a_sData) {
      sendLuamessage(a_iNumberOne, a_iNumbetTwo, a_sData, m_pOutputQueue);
    }

    /**
    * Send a trigger message to the UI thread
    * @param a_iTriggerID ID of the trigger
    * @param a_iObjectID the object that has triggered
    */
    void CDynamicThread::sendTriggerToUI(int a_iTriggerID, int a_iObjectID) {
      sendTrigger(a_iObjectID, a_iTriggerID, m_pOutputQueue);
    }


    /**
    * Retrieve the world of the race
    * @return the world of the race
    */
    CWorld* CDynamicThread::getWorld() {
      return m_pWorld;
    }

    /**
    * Get the LUA error
    * @return the LUA error
    */
    const std::string& CDynamicThread::getLuaError() {
      return m_sLuaError;
    }
  }
}