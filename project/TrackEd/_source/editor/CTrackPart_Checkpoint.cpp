// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Checkpoint.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Checkpoint::CTrackPart_Checkpoint(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart    (c_sTypeCheckpoint, a_pPrevious, a_cNormal),
      m_pCheckpoint (nullptr),
      m_pRespawn    (nullptr),
      m_bRespawnLeft(true) 
    {
      setEndNormal(a_cNormal);
    }

    CTrackPart_Checkpoint::~CTrackPart_Checkpoint() {
      if (m_pCheckpoint != nullptr) {
        m_pCheckpoint->setVisible(false);
        m_pCheckpoint->getSceneManager()->addToDeletionQueue(m_pCheckpoint);
        m_pCheckpoint = nullptr;
      }

      if (m_pRespawn != nullptr) {
        m_pRespawn->setVisible(false);
        m_pRespawn->getSceneManager()->addToDeletionQueue(m_pRespawn);
        m_pRespawn = nullptr;
      }
    }

    /**
    * Get the checkpoint scene node
    * @return the checkpoint scene node
    */
    irr::scene::ISceneNode *CTrackPart_Checkpoint::getCheckpointNode() {
      return m_pCheckpoint;
    }

    /**
    * Get the respawn scene node
    * @return the respawn scene node
    */
    irr::scene::ISceneNode *CTrackPart_Checkpoint::getRespawnNode() {
      return m_pRespawn;
    }

    /**
    * Get the side vector of the end of this track part
    * @return the side vector of the end of this track part
    */
    irr::core::vector3df CTrackPart_Checkpoint::getEndSide() const {
      return (m_cStart - m_cEnd).crossProduct(getEndNormal()).normalize();
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Checkpoint::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      irr::core::vector3df l_cSide = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();

      irr::f32 f1 = m_fWidth / 2.0f;
      irr::f32 f2 = 3.0f * m_fWidth / 2.0f;

      irr::core::vector3df l_aRespawn[] = {
        m_bRespawnLeft ? m_cStart - f1 * l_cSide : m_cStart + f2 * l_cSide,
        m_bRespawnLeft ? m_cStart - f2 * l_cSide : m_cStart + f1 * l_cSide,
        m_bRespawnLeft ? m_cEnd - f2 * l_cSide : m_cEnd + f2 * l_cSide,
        m_bRespawnLeft ? m_cEnd - f1 * l_cSide : m_cEnd + f2 * l_cSide
      };

      irr::core::vector3df l_aVertices[] = {
        getPrevious()->getEnd().end   + 0.05f * getStartNormal(),  // +0.25
        getPrevious()->getEnd().start + 0.05f * getStartNormal(),  // -0.25
        m_cEnd - f1 * l_cSide + 0.05f * getStartNormal(),
        m_cEnd + f1 * l_cSide + 0.05f * getStartNormal()
      };

      irr::core::position2df l_aTexture[] = {
        m_bRespawnLeft ? irr::core::position2df(0.0f, 0.0f) : irr::core::position2df(1.0f, 0.0f),
        m_bRespawnLeft ? irr::core::position2df(1.0f, 0.0f) : irr::core::position2df(0.0f, 0.0f),
        m_bRespawnLeft ? irr::core::position2df(1.0f, 1.0f) : irr::core::position2df(0.0f, 1.0f),
        m_bRespawnLeft ? irr::core::position2df(0.0f, 1.0f) : irr::core::position2df(1.0f, 1.0f)
      };

      if (a_vBuffers.size() > c_iBufferIdxJoin)
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxJoin], true, false, nullptr);

      setStart(irr::core::line3df(l_aVertices[0], l_aVertices[1]));

      l_aVertices[0] = l_aRespawn[0] + 0.05f * getStartNormal();
      l_aVertices[1] = l_aRespawn[1] + 0.05f * getStartNormal();
      l_aVertices[2] = l_aRespawn[2] + 0.05f * getStartNormal();
      l_aVertices[3] = l_aRespawn[3] + 0.05f * getStartNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 0.0f; l_aTexture[1].Y = 1.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 1.0f; l_aTexture[3].Y = 0.0f;

      if (a_vBuffers.size() > c_iBufferIdxRoad)
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxRoad], true, false, nullptr);

      l_aVertices[1] = l_aRespawn[0];
      l_aVertices[0] = l_aRespawn[1];
      l_aVertices[3] = l_aRespawn[2];
      l_aVertices[2] = l_aRespawn[3];

      l_aTexture[0].X = l_aVertices[0].X; l_aTexture[0].Y = l_aVertices[0].Z;
      l_aTexture[1].X = l_aVertices[1].X; l_aTexture[1].Y = l_aVertices[1].Z;
      l_aTexture[2].X = l_aVertices[2].X; l_aTexture[2].Y = l_aVertices[2].Z;
      l_aTexture[3].X = l_aVertices[3].X; l_aTexture[3].Y = l_aVertices[3].Z;

      if (a_vBuffers.size() > c_iBufferIdxBottom)
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxBottom], false, false, nullptr);

      l_aVertices[0] = getPrevious()->getEnd().start;
      l_aVertices[1] = getPrevious()->getEnd().end;
      l_aVertices[2] = m_cEnd + f1 * l_cSide;
      l_aVertices[3] = m_cEnd - f1 * l_cSide;

      l_aTexture[0].X = l_aVertices[0].X; l_aTexture[0].Y = l_aVertices[0].Z;
      l_aTexture[1].X = l_aVertices[1].X; l_aTexture[1].Y = l_aVertices[1].Z;
      l_aTexture[2].X = l_aVertices[2].X; l_aTexture[2].Y = l_aVertices[2].Z;
      l_aTexture[3].X = l_aVertices[3].X; l_aTexture[3].Y = l_aVertices[3].Z;

      setEnd(irr::core::line3df(l_aVertices[3], l_aVertices[2]));

      if (a_vBuffers.size() > c_iBufferIdxBottom)
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxBottom], false, false, nullptr);

      l_aVertices[0] = m_bRespawnLeft ? m_cEnd + f1 * l_cSide : m_cStart - f1 * l_cSide;
      l_aVertices[1] = m_bRespawnLeft ? getPrevious()->getEnd().end : m_cEnd - f1 * l_cSide;
      l_aVertices[2] = m_bRespawnLeft ? getPrevious()->getEnd().end + 0.05f * getStartNormal() : m_cEnd - f1 * l_cSide + 0.05f * getStartNormal();
      l_aVertices[3] = m_bRespawnLeft ? m_cEnd + f1 * l_cSide + 0.05f * getStartNormal() : m_cStart - f1 * l_cSide + 0.05f * getStartNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false, false, nullptr);

      l_aVertices[0] = m_bRespawnLeft ? l_aRespawn[1] : l_aRespawn[3];
      l_aVertices[1] = m_bRespawnLeft ? l_aRespawn[2] : l_aRespawn[0];
      l_aVertices[3] = m_bRespawnLeft ? l_aRespawn[1] + 0.05f * getStartNormal() : l_aRespawn[3] + 0.05f * getStartNormal();
      l_aVertices[2] = m_bRespawnLeft ? l_aRespawn[2] + 0.05f * getStartNormal() : l_aRespawn[0] + 0.05f * getStartNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxYellow : c_iBufferIdxBlack], false, false, nullptr);

      l_aVertices[0] = l_aRespawn[0];
      l_aVertices[1] = l_aRespawn[1];
      l_aVertices[2] = l_aRespawn[1] + 0.05f * getStartNormal();
      l_aVertices[3] = l_aRespawn[0] + 0.05f * getStartNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false, false, nullptr);

      l_aVertices[1] = l_aRespawn[3];
      l_aVertices[0] = l_aRespawn[2];
      l_aVertices[3] = l_aRespawn[2] + 0.05f * getStartNormal();
      l_aVertices[2] = l_aRespawn[3] + 0.05f * getStartNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
        addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false, false, nullptr);

      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();

      if (m_pCheckpoint == nullptr) {
        m_pCheckpoint = l_pGlobal->getSceneManager()->addMeshSceneNode(
          l_pGlobal->getSceneManager()->getMesh("data/objects/checkpoint.dae"),
          l_pGlobal->getSceneManager()->getRootSceneNode(),
          scenenodes::getNextSceneNodeId(),
          50.0f * m_cStart + 2.5f * getStartNormal(),
          (m_cEnd - m_cStart).getHorizontalAngle(),
          irr::core::vector3df(50.0f * m_fWidth)
        );

        m_pCheckpoint->getMaterial(0).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/checkpoint_white.png"));
        m_pCheckpoint->getMaterial(1).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/warning.png"));
        m_pCheckpoint->getMaterial(2).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/warning.png"));

        m_pCheckpoint->getMaterial(0).Lighting = false;
      }
      else {
        m_pCheckpoint->setPosition(50.0f * m_cStart + 2.5f * getStartNormal());
        m_pCheckpoint->setRotation((m_cEnd - m_cStart).getHorizontalAngle());
      }

      l_cSide = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();
      irr::core::vector3df l_cRspPos = 50.0f * m_cStart + 2.5f * getStartNormal() + (m_bRespawnLeft ? -22.5f * l_cSide + 7.5f * getEndDirection() : 22.5f * l_cSide + 7.5f * getEndDirection());
      irr::core::vector3df l_cRspAng = (m_cStart - m_cEnd).getHorizontalAngle() + irr::core::vector3df(0.0f, m_bRespawnLeft ? 45.0f : -45.0f, 0.0f);

      if (m_pRespawn == nullptr) {
        m_pRespawn = l_pGlobal->getSceneManager()->addMeshSceneNode(
          l_pGlobal->getSceneManager()->getMesh("data/objects/respawn_editor.3ds"),
          l_pGlobal->getSceneManager()->getRootSceneNode(),
          scenenodes::getNextSceneNodeId(),
          l_cRspPos,
          l_cRspAng,
          irr::core::vector3df(5.0f)
        );

        m_pRespawn->getMaterial(0).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/white.png"));
        m_pRespawn->getMaterial(0).Lighting = false;
      }
      else {
        m_pRespawn->setPosition(l_cRspPos);
        m_pRespawn->setRotation(l_cRspAng);
      }

      a_bBlack = !a_bBlack;
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Checkpoint::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      if (a_cPoints.size() == 2) {
        m_cStart = a_cPoints[0];
        m_cEnd   = a_cPoints[1];
      }
      else if (a_cPoints.size() == 3) {
        m_cStart = a_cPoints[0];
        m_cEnd   = a_cPoints[2];

        irr::core::vector3df l_cSide   = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();
        irr::core::vector3df l_cCenter = (m_cStart + m_cEnd) / 2.0f;

        irr::core::plane3df l_cPlane = irr::core::plane3df(l_cCenter, l_cSide);

        m_bRespawnLeft = l_cPlane.classifyPointRelation(a_cPoints[1]) == irr::core::ISREL3D_BACK;
      }

      std::vector<irr::scene::SMeshBuffer *> l_vDummy = std::vector<irr::scene::SMeshBuffer *>();
      bool l_bBlack = true;
      createTrackPart(l_vDummy, l_bBlack);

      if (getNext() != nullptr)
        getNext()->updateReferencePoints();
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_Checkpoint::updateReferencePoints() {
      ITrackPart *l_pPrevious = getPrevious();

      if (l_pPrevious != nullptr) {
        std::vector<irr::core::vector3df> l_vRefPoints;

        l_vRefPoints.push_back(m_cStart);
        l_vRefPoints.push_back(irr::core::vector3df());
        l_vRefPoints.push_back(m_cEnd);

        irr::core::vector3df l_vDir = l_pPrevious->getEndDirection();
        irr::f32 l_fLength = (l_vRefPoints.back() - l_vRefPoints.front()).getLength();

        l_vRefPoints[0] = l_pPrevious->getReferencePoints().back();
        l_vRefPoints[2] = l_vRefPoints.front() + l_fLength * l_vDir.normalize();

        irr::core::vector3df l_cSide = (l_vRefPoints.front() - l_vRefPoints.back()).crossProduct(getStartNormal()).normalize();

        l_vRefPoints[1] = (l_vRefPoints[0] + l_vRefPoints[2]) / 2.0f;

        if (m_bRespawnLeft)
          l_vRefPoints[1] -= l_cSide;
        else
          l_vRefPoints[1] += l_cSide;

        setReferencePoints(l_vRefPoints);
      }
    }

    /**
    * Does this reference point allow addition of new track parts?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Checkpoint::partAdditionAllowed(int a_iIndex) {
      return a_iIndex = 2 && getNext() == nullptr;
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Checkpoint::refPointAdditionAllowed(int a_iIndex) {
      return false;
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Checkpoint::serialize(irr::io::IAttributes &a_cAttr)const {
      ITrackPart::serialize(a_cAttr);

      a_cAttr.addVector3d("start", m_cStart);
      a_cAttr.addVector3d("end", m_cEnd);
      a_cAttr.addBool    ("respawn_left", m_bRespawnLeft);
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Checkpoint::deserialize(const irr::io::IAttributes &a_cAttr) {
      ITrackPart::deserialize(a_cAttr);

      m_cStart       = a_cAttr.getAttributeAsVector3d("start");
      m_cEnd         = a_cAttr.getAttributeAsVector3d("end");
      m_bRespawnLeft = a_cAttr.getAttributeAsBool    ("respawn_left");
    }

    /**
    * Get the direction of the end of this track part
    */
    const irr::core::vector3df CTrackPart_Checkpoint::getEndDirection() {
      return (m_cEnd - m_cStart).normalize();
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Checkpoint::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      if (a_iIndex == 1) {
        irr::core::vector3df l_cCenter = (m_cStart + m_cEnd) / 2.0f;
        irr::core::vector3df l_cSide   = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();

        a_cLine.start = l_cCenter + l_cSide / 2.0f;
        a_cLine.end   = l_cCenter - l_cSide / 2.0f;

        a_cPlanePoint  = l_cCenter;
        a_cPlaneNormal = getStartNormal();
      }
      else {
        a_cLine.start = irr::core::vector3df();
        a_cLine.end   = a_cLine.start;
      }

      return false;
    }

    /**
    * Get the part's reference points
    * @return a vector with the part's reference points
    */
    const std::vector<irr::core::vector3df> CTrackPart_Checkpoint::getReferencePoints() {
      std::vector<irr::core::vector3df> l_vRet;

      irr::core::vector3df l_cSide = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();
      irr::core::vector3df l_cRspn = (m_cStart + m_cEnd) / 2.0f;

      if (m_bRespawnLeft)
        l_cRspn -= m_fWidth * l_cSide;
      else
        l_cRspn += m_fWidth * l_cSide;

      l_vRet.push_back(m_cStart);
      l_vRet.push_back(l_cRspn),
        l_vRet.push_back(m_cEnd);

      return l_vRet;
    }

    /**
    * Add this track part to the AI path node
    * @param a_pAi the AI path node
    * @param a_pLast the last AI path section
    */
    void CTrackPart_Checkpoint::addToAiNode(scenenodes::CAiPathNode *a_pAi, scenenodes::CAiPathNode::SAiPathSection *a_pLast) {
      scenenodes::CAiPathNode::SAiPathSection *l_pThis = new scenenodes::CAiPathNode::SAiPathSection();

      irr::core::vector3df l_cSide   = (m_cStart - m_cEnd).crossProduct(getStartNormal()).normalize();

      l_pThis->m_cPosition   = 50.0f * (m_cStart + 0.055f * getStartNormal());
      l_pThis->m_cSideVector = l_cSide;
      l_pThis->m_fWidth      = 25.0f * m_fWidth;
      l_pThis->m_cNormal     = getStartNormal();
      l_pThis->m_eType       = scenenodes::CAiPathNode::enSegmentType::Respawn;
      l_pThis->m_iRespawn    = m_pRespawn->getID();
      l_pThis->m_fFactor     = 0.85f;

      if (a_pLast != nullptr) {
        a_pLast->m_vNextSegments.push_back(l_pThis);
        l_pThis->m_vPrevSegments.push_back(a_pLast);
      }

      a_pAi->addPathSection(l_pThis);

      a_pLast = l_pThis;

      l_pThis = new scenenodes::CAiPathNode::SAiPathSection();

      l_pThis->m_cPosition   = 50.0f * (m_cEnd + 0.055f * getStartNormal());
      l_pThis->m_cSideVector = l_cSide;
      l_pThis->m_fWidth      = 25.0f * m_fWidth;
      l_pThis->m_cNormal     = getStartNormal();
      l_pThis->m_eType       = scenenodes::CAiPathNode::enSegmentType::Default;
      l_pThis->m_iRespawn    = m_pRespawn->getID();
      l_pThis->m_fFactor     = 0.85f;

      if (a_pLast != nullptr) {
        a_pLast->m_vNextSegments.push_back(l_pThis);
        l_pThis->m_vPrevSegments.push_back(a_pLast);
      }

      a_pAi->addPathSection(l_pThis);


      if (getNext() != nullptr && getNext()->getPrevious() != nullptr) {
        getNext()->addToAiNode(a_pAi, l_pThis);
      }
      else if (getNext() != nullptr) {
        scenenodes::CAiPathNode::SAiPathSection *l_pFirst = a_pAi->getFirstSection();

        if (l_pFirst != nullptr) {
          l_pThis->m_vNextSegments.push_back(l_pFirst);
          l_pFirst->m_vPrevSegments.push_back(l_pThis);
        }
      }
    }

    /**
    * Handle an event if this part is selected
    * @param a_cEvent the event to handle
    * @return true if the event was handled
    */
    bool CTrackPart_Checkpoint::OnEvent(const irr::SEvent &a_cEvent) {
      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iSceneWillBeCleared) {
          m_pCheckpoint = nullptr;
        }
      }
      return false;
    }
  }
}