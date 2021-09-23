// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CStartingGridSceneNode.h>
#include <gameclasses/CDynamicThread.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CWorldNode.h>
#include <gameclasses/COdeNodes.h>
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

      dWorldID      l_cWorld = l_pWorld->m_cWorld;
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
            l_cContact[i].surface.bounce = (dReal)0.25;
            l_cContact[i].surface.mode = dContactBounce | dContactSlip1 | dContactSlip2 | dContactSoftCFM | dContactSoftERP;
            l_cContact[i].surface.mu = (dReal)25;
            l_cContact[i].surface.mu2 = (dReal)0;
            l_cContact[i].surface.bounce_vel = (dReal)0.0001;
            l_cContact[i].surface.soft_cfm = (dReal)0.004;
            l_cContact[i].surface.soft_erp = (dReal)0.4;
            l_cContact[i].surface.rho = (dReal)0.9;
            l_cContact[i].surface.rho2 = (dReal)0.9;
            l_cContact[i].surface.slip1 = (dReal)0.05;
            l_cContact[i].surface.slip2 = (dReal)0.05;
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
            if (l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->m_bTrigger) {
              CObjectMarble* l_pMarble = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);

              const dReal* l_vOdePos = dBodyGetPosition(l_pOdeNode1->m_cBody);
              irr::core::vector3df l_vPos = irr::core::vector3df((irr::f32)l_vOdePos[0], (irr::f32)l_vOdePos[1], (irr::f32)l_vOdePos[2]);
              l_pWorld->handleTrigger(l_pOdeNode2->m_iTrigger, l_pOdeNode1->m_iId, l_vPos);

              if (l_pMarble->m_iManualRespawn != -1 && l_pOdeNode2->m_iTrigger != -1)
                l_pMarble->m_iManualRespawn = 120;
            }
            else if (l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->m_bTrigger) {
              CObjectMarble* l_pMarble = reinterpret_cast<CObjectMarble*>(l_pOdeNode2);

              const dReal* l_vOdePos = dBodyGetPosition(l_pOdeNode2->m_cBody);
              irr::core::vector3df l_vPos = irr::core::vector3df((irr::f32)l_vOdePos[0], (irr::f32)l_vOdePos[1], (irr::f32)l_vOdePos[2]);
              l_pWorld->handleTrigger(l_pOdeNode1->m_iTrigger, l_pOdeNode2->m_iId, l_vPos);

              if (l_pMarble->m_iManualRespawn != -1 && l_pOdeNode1->m_iTrigger != -1)
                l_pMarble->m_iManualRespawn = 120;
            }
          }

          if (!l_pOdeNode1->m_bCollides || !l_pOdeNode2->m_bCollides)
            return;
        }

        dJointID l_cJoints[MAX_CONTACTS];

        for (irr::u32 i = 0; i < numc; i++) {
          if (l_pOdeNode1->getType() == enObjectType::Marble && l_pOdeNode2->getType() != enObjectType::Marble) {
            CObjectMarble* p = reinterpret_cast<CObjectMarble*>(l_pOdeNode1);
            p->m_bHasContact = true;
            p->m_vContact = irr::core::vector3df((irr::f32)l_cContact[i].geom.pos[0], (irr::f32)l_cContact[i].geom.pos[1], (irr::f32)l_cContact[i].geom.pos[2]);
          }

          if (l_pOdeNode2->getType() == enObjectType::Marble && l_pOdeNode1->getType() != enObjectType::Marble) {
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
          m_pWorld = new CWorld();
        }
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_PhysicsNodeId) {
        if (m_pWorld != nullptr) {
          scenenodes::CPhysicsNode* l_pNode = reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode);

          switch (l_pNode->getNodeType()) {
            case scenenodes::CPhysicsNode::enNodeType::Box:
              m_pWorld->m_vObjects.push_back(new CObjectBox(l_pNode, m_pWorld));
              break;

            case scenenodes::CPhysicsNode::enNodeType::Sphere:
              m_pWorld->m_vObjects.push_back(new CObjectSphere(l_pNode, m_pWorld));
              break;

            case scenenodes::CPhysicsNode::enNodeType::Trimesh:
              m_pWorld->m_vObjects.push_back(new CObjectTrimesh(l_pNode, m_pWorld));
              break;
          }
        }
      }
      else if (a_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_StartingGridScenenodeId) {
        m_fGridAngle = reinterpret_cast<scenenodes::CStartingGridSceneNode*>(a_pNode)->getAngle();
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
          if (m_aMarbles[i] != nullptr) {
            irr::f32 l_fCtrlX = ((irr::f32)m_aMarbles[i]->m_iCtrlX) / 127.0f,
                     l_fCtrlY = ((irr::f32)m_aMarbles[i]->m_iCtrlY) / 127.0f;

            irr::core::vector3df v = m_aMarbles[i]->m_vPosition - m_aMarbles[i]->m_vCamera;

            irr::core::vector3df l_vTorque = -50.0f * l_fCtrlX * m_aMarbles[i]->m_vDirection - 40.0f * l_fCtrlY * m_aMarbles[i]->m_vSideVector;

            dBodyAddTorque(m_aMarbles[i]->m_cBody, (dReal)l_vTorque.X, (dReal)l_vTorque.Y, (dReal)l_vTorque.Z);

            if (m_aMarbles[i]->m_bBrake)
              dBodySetAngularDamping(m_aMarbles[i]->m_cBody, (dReal)0.05);
            else
              dBodySetAngularDamping(m_aMarbles[i]->m_cBody, (dReal)0.0015);
          }
        }

        dSpaceCollide(m_pWorld->m_cSpace, m_pWorld, &nearCollisionCallback);
        dWorldStep(m_pWorld->m_cWorld, (dReal)0.008);
        dJointGroupEmpty(m_pWorld->m_cContacts);

        m_iWorldStep++;

        // Update the states of all marbles and send the "marble moved" messages
        for (int i = 0; i < 16; i++) {
          if (m_aMarbles[i] != nullptr) {
            CObjectMarble* p = m_aMarbles[i];

            const dReal* l_aPos    = dBodyGetPosition(p->m_cBody),
                       * l_aRot    = dBodyGetQuaternion(p->m_cBody),
                       * l_aLinVel = dBodyGetLinearVel(p->m_cBody),
                       * l_aAngVel = dBodyGetAngularVel(p->m_cBody);

            irr::core::vector3df l_vLinVel = vectorOdeToIrr(l_aLinVel);

            p->m_vPosition = vectorOdeToIrr(l_aPos);

            if (p->m_bActive) {
              irr::f32 l_fLinVel = l_vLinVel.getLength();
              if (l_fLinVel > 0.5f) {
                irr::core::vector3df l_vNormVel = l_vLinVel,
                  l_vNormUp = p->m_vUpVector;

                l_vNormVel.normalize();
                l_vNormUp.normalize();

                irr::f32 l_fInterpolate = 1.0f - (l_fLinVel / 750.0f);

                if (p->m_bHasContact) {
                  irr::core::vector3df l_vUpVector = p->m_vPosition - p->m_vContact;
                  l_vUpVector.normalize();
                  p->m_vUpVector = l_vUpVector.interpolate(p->m_vUpVector, l_vUpVector, l_fInterpolate);
                }

                irr::f32 l_fFactor = l_fLinVel / 37.5f;

                if (l_fFactor > 2.0f)
                  l_fFactor = 2.0f;

                l_fFactor = (irr::f32)(sin(((l_fFactor - 1.0f) / 2.0f) * M_PI) + 1.0f) * 7.5f;

                irr::core::vector3df l_vOffset = (l_fFactor < 2.5f ? 2.5f : l_fFactor > 15.0f ? 15.0f : l_fFactor) * l_vNormVel;
                p->m_vDirection = l_vOffset;
                p->m_vDirection.normalize();

                if (l_vOffset.getLength() < 8.0f)
                  l_vOffset = 8.0f * l_vOffset.normalize();
                  
                p->m_vCamera = p->m_vCamera.interpolate(p->m_vCamera, p->m_vPosition - l_vOffset + (l_fLinVel < 10.0f ? 2.0f : l_fLinVel > 25.0f ? 5.0f : l_fLinVel / 5.0f) * p->m_vUpVector, l_fInterpolate);

                p->m_vSideVector = p->m_vDirection.crossProduct(p->m_vUpVector);
                p->m_vSideVector.normalize();

                p->m_bHasContact = false;
              }
            }
            else {
              p->m_vCamera = p->m_vPosition - p->m_vOffset + 5.0f * p->m_vUpVector;
            }
            
            sendMarblemoved(p->m_iId, 
              p->m_vPosition, 
              quaternionToEuler(l_aRot), 
              l_vLinVel, 
              vectorOdeToIrr(l_aAngVel).getLength(), 
              p->m_vCamera,
              p->m_vUpVector, 
              p->m_iCtrlX,
              p->m_iCtrlY, 
              false, 
              p->m_bBrake, 
              p->m_bRearView, 
              false, 
              m_pOutputQueue);
          }
        }
        sendStepmsg(m_iWorldStep, m_pOutputQueue);
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

    CDynamicThread::CDynamicThread(scenenodes::CWorldNode* a_pWorld, const std::vector<gameclasses::SPlayer*>& a_vPlayers) : m_pWorld(nullptr), m_bPaused(false), m_iWorldStep(0), m_fGridAngle(0.0f) {
      createPhysicsObjects(a_pWorld);

      for (int i = 0; i < 16; i++)
        m_aMarbles[i] = nullptr;

      if (m_pWorld != nullptr) {
        int l_iIndex = 0;
        for (std::vector<gameclasses::SPlayer*>::const_iterator it = a_vPlayers.begin(); it != a_vPlayers.end(); it++) {
          CObjectMarble* l_pMarble = new CObjectMarble((*it)->m_pMarble->m_pPositional, m_pWorld);

          irr::core::vector3df l_vOffset = irr::core::vector3df(0.0f, 0.0f, -15.0f);
          l_vOffset.rotateXZBy(m_fGridAngle);

          l_pMarble->m_vDirection  = l_vOffset;
          l_pMarble->m_vUpVector   = irr::core::vector3df(0.0f, 1.0f, 0.0f);
          l_pMarble->m_vContact    = irr::core::vector3df();
          l_pMarble->m_vSideVector = l_pMarble->m_vDirection.crossProduct(l_pMarble->m_vUpVector);
          l_pMarble->m_vOffset     = l_vOffset;
          l_pMarble->m_vCamera     = (*it)->m_pMarble->m_pPositional->getAbsolutePosition() + irr::core::vector3df(l_vOffset) + l_pMarble->m_vUpVector;

          l_pMarble->m_vSideVector.normalize();
          l_pMarble->m_vDirection .normalize();

          m_pWorld->m_vObjects.push_back(l_pMarble);
          m_aMarbles[l_iIndex] = l_pMarble;
          printf("*** Marble with id %i stored in array index %i\n", (*it)->m_pMarble->m_pPositional->getID(), l_iIndex);

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
    }

    CDynamicThread::~CDynamicThread() {
      if (m_pWorld != nullptr)
        delete m_pWorld;
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
          m_aMarbles[l_iIndex]->m_iCtrlX = a_CtrlX;
          m_aMarbles[l_iIndex]->m_iCtrlY = a_CtrlY;
          m_aMarbles[l_iIndex]->m_bBrake = a_Brake;
          m_aMarbles[l_iIndex]->m_bRespawn = a_RearView;
        }
        else {
          if (abs(a_CtrlX > 64) || abs(a_CtrlY > 64))
            m_aMarbles[l_iIndex]->m_bActive = true;
        }
      }
    }
  }
}