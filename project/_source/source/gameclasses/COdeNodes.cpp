// (w) 2021 by Dustbin::Games / Christian Keimel
#include <gameclasses/ITriggerHandler.h>
#include <scenenodes/CCheckpointNode.h>
#include <scenenodes/CPhysicsNode.h>
#include <scenenodes/CRespawnNode.h>
#include <gameclasses/COdeNodes.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gameclasses {
    /**
    * This function converts Irrlicht's Euler angles to ODE Quaternions
    * @param v the Irrlicht rotation in Euler angles
    * @param q the ODE rotation as Quaternion
    */
    void eulerToQuaternion(const irr::core::vector3df v, dQuaternion q) {
      double heading = v.Z * irr::core::DEGTORAD / 2.0f;
      double attitude = v.Y * irr::core::DEGTORAD / 2.0f;
      double bank = v.X * irr::core::DEGTORAD / 2.0f;
      double c1 = cos(heading);
      double s1 = sin(heading);
      double c2 = cos(attitude);
      double s2 = sin(attitude);
      double c3 = cos(bank);
      double s3 = sin(bank);

      q[0] = (dReal)(c1 * c2 * c3 + s1 * s2 * s3); //w
      q[1] = (dReal)(c1 * c2 * s3 - s1 * s2 * c3); //x
      q[2] = (dReal)(c1 * s2 * c3 + s1 * c2 * s3); //y
      q[3] = (dReal)(s1 * c2 * c3 - c1 * s2 * s3); //z
    }

    CObject::CObject(enObjectType a_eType, scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName, int a_iMaterial) :
      m_iId(a_pNode != nullptr ? a_pNode->getID() : -1),
      m_pWorld(a_pWorld),
      m_bCollides(true),
      m_bTrigger(false),
      m_bRespawn(false),
      m_sName(a_sName),
      m_eType(a_eType),
      m_bStatic(true),
      m_iTrigger(0),
      m_cGeom(0),
      m_cBody(0)
    {
      if (a_pNode == nullptr && m_eType != enObjectType::Marble) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "No scene node found while creating trimesh object.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
        throw std::exception();
      }

      if (a_pNode != nullptr) {
        irr::io::IAttributes* l_pAttr = CGlobal::getInstance()->getFileSystem()->createEmptyAttributes();

        a_pNode->serializeAttributes(l_pAttr);

        if (l_pAttr->existsAttribute("static"                                                  )) m_bStatic   = l_pAttr->getAttributeAsBool("static");
        if (l_pAttr->existsAttribute("collides"                                                )) m_bCollides = l_pAttr->getAttributeAsBool("collides");
        if (l_pAttr->existsAttribute(("DoesTrigger_" + std::to_string(a_iMaterial + 1)).c_str())) m_bTrigger  = l_pAttr->getAttributeAsBool(("DoesTrigger_" + std::to_string(a_iMaterial + 1)).c_str());
        if (l_pAttr->existsAttribute(("Trigger_"     + std::to_string(a_iMaterial + 1)).c_str())) m_iTrigger  = l_pAttr->getAttributeAsInt (("Trigger_"     + std::to_string(a_iMaterial + 1)).c_str());
        if (l_pAttr->existsAttribute(("Respawn_"     + std::to_string(a_iMaterial + 1)).c_str())) m_bRespawn  = l_pAttr->getAttributeAsBool(("Respawn_"     + std::to_string(a_iMaterial + 1)).c_str());

        l_pAttr->drop();
      }
    }

    CObject::~CObject() {
      if (m_cBody != nullptr) dBodyDestroy(m_cBody);
      if (m_cGeom != nullptr) dGeomDestroy(m_cGeom);
    }

    enObjectType CObject::getType() {
      return m_eType;
    }

    CObjectBox::CObjectBox(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName) : CObject(enObjectType::Box, a_pNode, a_pWorld, a_sName) {
      irr::core::aabbox3df l_cBox      = a_pNode->getParent()->getBoundingBox();
      irr::core::vector3df l_cScale    = a_pNode->getParent()->getAbsoluteTransformation().getScale(),
                           l_cRotate   = a_pNode->getParent()->getAbsoluteTransformation().getRotationDegrees(),
                           l_cPosition = a_pNode->getParent()->getAbsoluteTransformation().getTranslation() + l_cBox.getCenter() * l_cScale;

      m_cGeom = dCreateBox(m_pWorld->m_cSpace, l_cScale.X * l_cBox.getExtent().X, l_cScale.Y * l_cBox.getExtent().Y, l_cScale.Z * l_cBox.getExtent().Z);
      dQuaternion l_vRot;
      eulerToQuaternion(l_cRotate, l_vRot);
      dGeomSetQuaternion(m_cGeom, l_vRot);
      dGeomSetPosition(m_cGeom, l_cPosition.X, l_cPosition.Y, l_cPosition.Z);
      dGeomSetData(m_cGeom, this);

      // a_pNode->getSceneManager()->addCubeSceneNode(1.0f, a_pNode->getSceneManager()->getRootSceneNode(), -1, l_cPosition, l_cRotate, l_cScale * l_cBox.getExtent());
    }

    CObjectBox::~CObjectBox() {
    }

    CObjectSphere::CObjectSphere(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName) : CObject(enObjectType::Sphere, a_pNode, a_pWorld, a_sName) {
    }

    CObjectSphere::~CObjectSphere() {
    }

    CObjectCheckpoint::CObjectCheckpoint(scenenodes::CCheckpointNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName) :
      CObject(enObjectType::Checkpoint, reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode), a_pWorld, a_sName),
      m_vRespawnDir(irr::core::vector3df()),
      m_vRespawnPos(irr::core::vector3df()),
      m_bHasRespawn(false),
      m_bLapStart(false)
    {
      a_pWorld->m_vObjects.push_back(new CObjectTrimesh(reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode), a_pWorld, a_sName + "_trimesh"));
      
      CObjectBox* p = new CObjectBox(reinterpret_cast<scenenodes::CPhysicsNode*>(a_pNode), a_pWorld, a_sName + "_box");
      p->m_bCollides = false;

      a_pWorld->m_vObjects.push_back(p);
      a_pWorld->m_mCheckpoints[a_pNode->getID()] = this;

      m_bLapStart = a_pNode->m_bFirstInLap;

      for (std::vector<int>::iterator it = a_pNode->m_vLinks.begin(); it != a_pNode->m_vLinks.end(); it++)
        m_vNext.push_back(*it);

      irr::scene::ISceneNode *l_pNode = a_pNode->getSceneManager()->getSceneNodeFromId(a_pNode->m_iRespawn);

      if (l_pNode != nullptr && l_pNode->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RespawnNodeId) {
        m_bHasRespawn = true;

        scenenodes::CRespawnNode* l_pRespawn = reinterpret_cast<scenenodes::CRespawnNode*>(l_pNode);

        l_pRespawn->updateAbsolutePosition();
        m_vRespawnPos = l_pRespawn->getAbsolutePosition() + irr::core::vector3df(0.0f, 2.25f, 0.0f);
        m_vRespawnDir = l_pRespawn->getRotation().rotationToDirection(irr::core::vector3df(0.0f, 0.0f, 10.0f));
      }
    }

    CObjectCheckpoint::~CObjectCheckpoint() {
    }

    CObjectTrimesh::CObjectTrimesh(scenenodes::CPhysicsNode* a_pNode, CWorld* a_pWorld, const std::string& a_sName, int a_iMaterial) : 
      CObject(enObjectType::Trimesh, a_pNode, a_pWorld, a_sName + " #" + std::to_string(a_iMaterial), a_iMaterial)
    {
      if (a_pNode->getParent()->getType() == irr::scene::ESNT_MESH) {
        irr::scene::IMeshSceneNode* l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode*>(a_pNode->getParent());

        if (a_iMaterial < 0 || a_iMaterial >= (int)l_pNode->getMaterialCount()) {
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", std::string("Got invalid material index #") + std::to_string(a_iMaterial) + " on Trimesh creation.");
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
          throw std::exception();
        }

        dQuaternion l_aQuaternion;
        dQSetIdentity(l_aQuaternion);

        irr::scene::IMeshBuffer* l_pBuffer = l_pNode->getMesh()->getMeshBuffer(a_iMaterial);

        irr::video::S3DVertex* l_pVertices = (irr::video::S3DVertex*)l_pBuffer->getVertices();

        irr::u32 l_iIndexV = 0;

        irr::core::CMatrix4<irr::f32> l_cMatrix = l_pNode->getAbsoluteTransformation();

        irr::core::vector3df l_vScale = l_pNode->getScale();

        for (irr::u32 i = 0; i < l_pBuffer->getVertexCount(); i++) {
          irr::core::vector3df l_cVec = l_pVertices[l_iIndexV].Pos;

          l_cMatrix.rotateVect(l_cVec);

          dVector3 v;

          m_vVertices.push_back((dReal)l_cVec.X);
          m_vVertices.push_back((dReal)l_cVec.Y);
          m_vVertices.push_back((dReal)l_cVec.Z);
          m_vVertices.push_back(0.0);

          l_iIndexV++;
        }

        irr::u32 l_iIndexI = 0;
        irr::u16* l_pIndices = l_pBuffer->getIndices();

        for (irr::u32 i = 0; i < l_pBuffer->getIndexCount(); i++) {
          m_vIndices.push_back(l_pIndices[i]);
        }

        m_cTrimeshData = dGeomTriMeshDataCreate();

        dGeomTriMeshDataBuildSimple(m_cTrimeshData, m_vVertices.data(), l_pBuffer->getVertexCount(), m_vIndices.data(), l_pBuffer->getIndexCount());
        m_cGeom = dCreateTriMesh(m_pWorld->m_cSpace, m_cTrimeshData, 0, 0, 0);
        dGeomSetData(m_cGeom, this);
        dGeomSetPosition(m_cGeom, (dReal)a_pNode->getAbsolutePosition().X, (dReal)a_pNode->getAbsolutePosition().Y, (dReal)a_pNode->getAbsolutePosition().Z);

        if (a_iMaterial < (int)l_pNode->getMaterialCount() - 1)
          a_pWorld->m_vObjects.push_back(new CObjectTrimesh(a_pNode, a_pWorld, a_sName, a_iMaterial + 1));
      }
      else {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", "Trimesh physics objects can only be built from mesh scene nodes.");
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while initializing game physics.");
        throw std::exception();
      }
    }

    CObjectTrimesh::~CObjectTrimesh() {
    }

    CObjectMarble::CObjectMarble(irr::scene::ISceneNode* a_pNode, const irr::core::vector3df& a_cDirection, CWorld* a_pWorld, const std::string& a_sName) :
      CObject(enObjectType::Marble, nullptr, a_pWorld, a_sName),
      m_vRespawnPos(irr::core::vector3df(1.0f, 0.0f, 0.0f)),
      m_vSideVector(irr::core::vector3df(1.0f, 0.0f, 0.0f)),
      m_vUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f)),
      m_vUpOffset(irr::core::vector3df(0.0f, 1.0f, 0.0f)),
      m_vDirection(irr::core::vector3df()),
      m_vRearview(irr::core::vector3df()),
      m_vPosition(irr::core::vector3df()),
      m_vVelocity(irr::core::vector3df()),
      m_vContact(irr::core::vector3df()),
      m_eState(enMarbleState::Countdown),
      m_vCamera(irr::core::vector3df()),
      m_vRespawnDir(a_cDirection),
      m_fDamp((dReal)0.0015),
      m_iManualRespawn(-1),
      m_bHasContact(false),
      m_iRespawnStart(-1),
      m_iStunnedStart(-1),
      m_bRearView(false),
      m_iLastTrigger(-1),
      m_iLastContact(0),
      m_bRespawn(false),
      m_bActive(false),
      m_bBrake(false),
      m_iLastCp(0),
      m_iLapNo(0),
      m_iCtrlX(0),
      m_iCtrlY(0)
    {
      m_bStatic = false;

      m_iId = a_pNode->getID();

      a_pNode->updateAbsolutePosition();
      m_cGeom = dCreateSphere(m_pWorld->m_cSpace, 2.0);
      dGeomSetData(m_cGeom, this);

      m_cBody = dBodyCreate(m_pWorld->m_cWorld);
      dMass l_cMass;
      dMassSetZero(&l_cMass);
      // Marble Class Param: Mass
      dMassSetSphereTotal(&l_cMass, 1.0f, 1.0f);
      dBodySetAngularDamping(m_cBody, (dReal)0.0015);
      dGeomSetBody(m_cGeom, m_cBody);
      dBodySetPosition(m_cBody, (dReal)a_pNode->getAbsolutePosition().X, (dReal)a_pNode->getAbsolutePosition().Y, (dReal)a_pNode->getAbsolutePosition().Z);

      dQuaternion l_vRot;
      eulerToQuaternion(a_pNode->getRotation(), l_vRot);
      dBodySetQuaternion(m_cBody, l_vRot);

      m_vRespawnPos = a_pNode->getAbsolutePosition();
    }

    CObjectMarble::~CObjectMarble() {
    }

    bool CObjectMarble::canBeStunned() {
      return m_eState == enMarbleState::Rolling;
    }

    bool CObjectMarble::canRespawn() {
      return m_eState == enMarbleState::Rolling || m_eState == enMarbleState::Stunned;
    }

    CWorld::CWorld(ITriggerHandler* a_pTriggerHandler) : m_cWorld(nullptr), m_cSpace(nullptr), m_cContacts(nullptr), m_pTriggerHandler(a_pTriggerHandler) {
      dInitODE2(0);

      m_cWorld = dWorldCreate();
      dWorldSetGravity(m_cWorld, 0, -10, 0);
      dWorldSetMaxAngularSpeed(m_cWorld, 150.0);

      m_cSpace    = dSimpleSpaceCreate(0);
      m_cContacts = dJointGroupCreate (0);
    }

    void CWorld::handleRespawn(int a_iMarble) {
      if (m_pTriggerHandler != nullptr)
        m_pTriggerHandler->handleRespawn(a_iMarble);
    }

    void CWorld::handleCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (m_pTriggerHandler != nullptr)
        m_pTriggerHandler->handleCheckpoint(a_iMarbleId, a_iCheckpoint);
    }

    void CWorld::handleLapStart(int a_iMarbleId, int a_iLapNo) {
      if (m_pTriggerHandler != nullptr)
        m_pTriggerHandler->handleLapStart(a_iMarbleId, a_iLapNo);
    }

    void CWorld::handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_cPos) {
      if (m_pTriggerHandler != nullptr)
        m_pTriggerHandler->handleTrigger(a_iTrigger, a_iMarble, a_cPos);
    }

    CWorld::~CWorld() {
      for (std::vector<CObject*>::iterator it = m_vObjects.begin(); it != m_vObjects.end(); it++)
        delete* it;

      m_vObjects.clear();

      m_mCheckpoints.clear();

      if (m_cContacts != nullptr) dJointGroupEmpty(m_cContacts);
      if (m_cWorld != nullptr) dWorldDestroy(m_cWorld);

      dCloseODE();
    }
  }
}