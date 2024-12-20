// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Bezier.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>
#include <cmath>

namespace dustbin {
  namespace tracked {
    CTrackPart_Bezier::CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart (c_sTypeBezier, a_pPrevious, a_cNormal) {
    }

    CTrackPart_Bezier::~CTrackPart_Bezier() {
    }

    /**
     * Get the next point using the Bezier1 algorithm
     * @param p array with the two points for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df CTrackPart_Bezier::getBezier1(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return (1 - a_fStep) * a_aPoints[0] + a_fStep * a_aPoints[1];
    }

    /**
     * Get the next point using the Bezier2 algorithm
     * @param p array with the three points (start, help, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df CTrackPart_Bezier::getBezier2(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return ((1 - a_fStep) * (1 - a_fStep)) * a_aPoints[0] + 2 * a_fStep * (1 - a_fStep) * a_aPoints[1] + a_fStep * a_fStep * a_aPoints[2];
    }

    /**
     * Get the next point using the Bezier3 algorithm
     * @param p array with the four points (start, help1, help2, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df CTrackPart_Bezier::getBezier3(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return (-a_aPoints[0] + 3.0f * a_aPoints[1] - 3.0f * a_aPoints[2] + a_aPoints[3]) * a_fStep * a_fStep * a_fStep + (3.0f * a_aPoints[0] - 6.0f * a_aPoints[1] + 3.0f * a_aPoints[2]) * a_fStep * a_fStep + (-3.0f * a_aPoints[0] + 3.0f * a_aPoints[1]) * a_fStep + a_aPoints[0];
    }

    /**
    * A little helper functions to fill the indices for the Bezier calculation
    * @param a_iSteps the number of steps
    * @param a_pPrevious the previous track part
    * @return a vector with the indices ranging from 0 to 1
    */
    std::vector<irr::f32> CTrackPart_Bezier::fillIndexVector(int a_iSteps, ITrackPart *a_pPrevious) {
      std::vector<irr::f32> l_vRet;

      for (int i = a_pPrevious == nullptr ? 0 : 1; i < a_iSteps; i++) {
        l_vRet.push_back((irr::f32)i / (irr::f32)a_iSteps);
      }

      l_vRet.push_back(1.0);

      return l_vRet;
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Bezier::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      double l_fLength = calculateCurve();
      l_fLength = std::ceil(l_fLength);

      irr::f32 l_fWidth = m_fWidth / 2.0f;
      std::vector<irr::core::line3df> l_vBottom;

      irr::core::vector3df l_cOld;

      for (auto l_cPos : m_vTrack) {
        if (l_cPos != m_vTrack.front() || getPrevious() == nullptr) {
          irr::core::vector3df l_cDirection;

          if (l_cPos != m_vTrack.front()) {
            l_cDirection = (l_cPos - l_cOld).normalize();
          }
          else {
            l_cDirection = (m_vPoints[1] - m_vPoints[0]).normalize();
          }

          irr::core::vector3df l_cSide = (l_cDirection.crossProduct(getNormal())).normalize();

          if (l_cPos != m_vTrack.back() || getNext() == nullptr || getNext()->getPrevious() != nullptr)
            l_vBottom.push_back(irr::core::line3df(l_cPos + l_fWidth * l_cSide, l_cPos - l_fWidth * l_cSide));
          else
            l_vBottom.push_back(getNext()->getStart());
        }
        else {
          // First segment: take last segment of previous track part
          l_vBottom.push_back(getPrevious()->getEnd());
        }

        l_cOld = l_cPos;
      }

      if (l_vBottom.size() > 0) {
        setStart(l_vBottom.front());
        setEnd  (l_vBottom.back ());
      }

      size_t l_iSteps = l_vBottom.size() - 1;

      double l_fTextureStep = l_fLength / (double)(l_iSteps);

      irr::core::position2df l_aTextureTop[] = {
        irr::core::position2df(1.0f, 0.0f),
        irr::core::position2df(0.0f, 0.0f),
        irr::core::position2df(0.0f, (irr::f32)l_fTextureStep),
        irr::core::position2df(1.0f, (irr::f32)l_fTextureStep)
      };

      irr::core::line3df l_cOldLine;

      for (auto &l_cLine : l_vBottom) {
        if (l_cLine != l_vBottom.front()) {
          irr::core::vector3df l_aVerts[] = {
            l_cOldLine.end + 0.05f * getNormal(),
            l_cOldLine.start + 0.05f * getNormal(),
            l_cLine   .start + 0.05f * getNormal(),
            l_cLine   .end + 0.05f * getNormal()
          };

          if (l_cLine == l_vBottom.back()) {
            l_aTextureTop[2].Y = std::ceil(l_aTextureTop[2].Y);
            l_aTextureTop[3].Y = std::ceil(l_aTextureTop[3].Y);
          }

          if (a_vBuffers.size() > c_iBufferIdxRoad) {
            addQuadToMeshBuffer(l_aVerts, l_aTextureTop, a_vBuffers[c_iBufferIdxRoad], l_cOldLine == l_vBottom.front(), getNext() != nullptr && getNext()->getPrevious() == nullptr);
          }

          for (int i = 0; i < 4; i++)
            l_aTextureTop[i].Y += (irr::f32)l_fTextureStep;

          l_aVerts[0] = l_cOldLine.start;
          l_aVerts[1] = l_cOldLine.end;
          l_aVerts[2] = l_cLine   .end;
          l_aVerts[3] = l_cLine   .start;

          irr::core::position2df l_aTextureBottom[] = {
            irr::core::position2df(l_aVerts[0].X, l_aVerts[0].Z),
            irr::core::position2df(l_aVerts[1].X, l_aVerts[1].Z),
            irr::core::position2df(l_aVerts[2].X, l_aVerts[2].Z),
            irr::core::position2df(l_aVerts[3].X, l_aVerts[3].Z)
          };

          if (a_vBuffers.size() > c_iBufferIdxBottom)
            addQuadToMeshBuffer(l_aVerts, l_aTextureBottom, a_vBuffers[c_iBufferIdxBottom], false, false);

          l_aVerts[0] = l_cOldLine.end + 0.05f * getNormal();
          l_aVerts[1] = l_cLine   .end + 0.05f * getNormal();
          l_aVerts[2] = l_cLine   .end;
          l_aVerts[3] = l_cOldLine.end;

          irr::core::position2df l_aTextureSide[] = {
            irr::core::position2df(0.0f, 0.0f),
            irr::core::position2df(1.0f, 0.0f),
            irr::core::position2df(1.0f, 1.0f),
            irr::core::position2df(0.0f, 1.0f)
          };

          if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
            addQuadToMeshBuffer(l_aVerts, l_aTextureSide, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false, false);

          l_aVerts[1] = l_cOldLine.start + 0.05f * getNormal();
          l_aVerts[0] = l_cLine   .start + 0.05f * getNormal();
          l_aVerts[3] = l_cLine   .start;
          l_aVerts[2] = l_cOldLine.start;

          if ((a_bBlack && a_vBuffers.size() > c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > c_iBufferIdxYellow))
            addQuadToMeshBuffer(l_aVerts, l_aTextureSide, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false, false);

          a_bBlack = !a_bBlack;
        }

        l_cOldLine = l_cLine;
      }
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_Bezier::updateReferencePoints() {
      if (m_vPoints.size() >= 2) {
        ITrackPart *l_pPrevious = getPrevious();

        if (l_pPrevious != nullptr) {
          std::vector<irr::core::vector3df> l_vRefPoints = getReferencePoints();

          irr::core::vector3df l_cDir = l_pPrevious->getEndDirection();
          irr::f32 l_fLength = (l_vRefPoints[1] - l_vRefPoints[0]).getLength();

          l_vRefPoints[0] = l_pPrevious->getEnd().getMiddle();
          l_vRefPoints[1] = l_vRefPoints[0] + (l_fLength * l_cDir.normalize());

          setReferencePoints(l_vRefPoints);
        }
      }
    }

    /**
    * Does this reference point allow addition of new track parts?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Bezier::partAdditionAllowed(int a_iIndex) {
      return a_iIndex == m_vPoints.size() - 1 && getNext() == nullptr;
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Bezier::refPointAdditionAllowed(int a_iIndex) {
      return a_iIndex == m_vPoints.size() - 1 && m_vPoints.size() < 4 && getPrevious() != nullptr;
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Bezier::serialize(irr::io::IAttributes &a_cAttr) const {
      ITrackPart::serialize(a_cAttr);

      a_cAttr.addInt("number_of_points", (irr::s32)m_vPoints.size());

      for (int i = 0; i < (int)m_vPoints.size(); i++) {
        std::string l_sName = "point_" + std::to_string(i);
        a_cAttr.addVector3d(l_sName.c_str(), m_vPoints[i]);
      }
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Bezier::deserialize(const irr::io::IAttributes &a_cAttr) {
      ITrackPart::deserialize(a_cAttr);

      irr::s32 l_iCount = a_cAttr.getAttributeAsInt("number_of_points");

      for (irr::s32 i = 0; i < l_iCount; i++) {
        std::string l_sName = "point_" + std::to_string(i);
        m_vPoints.push_back(a_cAttr.getAttributeAsVector3d(l_sName.c_str()));
      }
    }

    /**
    * Get the point of the bezier curve
    * @param a_vPoints vector with the reference points
    * @param a_fStep step number (0..1)
    * @return the next point
    */
    irr::core::vector3df CTrackPart_Bezier::getBezier(std::vector<irr::core::vector3df> &a_vPoints, irr::f32 a_fStep) {
      switch (a_vPoints.size()) {
        case 2: return getBezier1(a_vPoints.data(), a_fStep);
        case 3: return getBezier2(a_vPoints.data(), a_fStep);
        case 4: return getBezier3(a_vPoints.data(), a_fStep);
      }
    }

    /**
    * Calculate the points of the curve, road center
    */
    irr::f32 CTrackPart_Bezier::calculateCurve() {
      m_vTrack.clear();

      std::vector<irr::f32> l_vIndices = fillIndexVector(32, getPrevious());

      for (auto l_fStep: l_vIndices) {
        m_vTrack.push_back(getBezier(m_vPoints, l_fStep));
      }

      irr::f32 l_fRet = 0.0f;

      for (std::vector<irr::core::vector3df>::iterator l_itTrack = m_vTrack.begin() + 1; l_itTrack != m_vTrack.end(); l_itTrack++) {
        l_fRet += (*l_itTrack).getDistanceFrom(*(l_itTrack - 1));
      }

      return l_fRet;
    }

    /**
    * Set the trak part's reference points
    * @param a_vPoints a vector with the reference points
    */
    void CTrackPart_Bezier::setReferencePoints(const std::vector<irr::core::vector3df> &a_vPoints) {
      m_vPoints.clear();

      m_vPoints.clear();

      for (auto l_cPoint : a_vPoints)
        m_vPoints.push_back(l_cPoint);

      std::vector<irr::scene::SMeshBuffer *> l_vDummy = std::vector<irr::scene::SMeshBuffer *>();
      bool l_bBlack = true;
      createTrackPart(l_vDummy, l_bBlack);

      if (getNext() != nullptr && getNext()->getPrevious() != nullptr)
        getNext()->updateReferencePoints();
    }

    /**
    * Get the part's reference points
    * @return a vector with the part's reference points
    */
    const std::vector<irr::core::vector3df> CTrackPart_Bezier::getReferencePoints() {
      return m_vPoints;
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Bezier::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      if (getPrevious() == nullptr) {
        a_cLine.start = irr::core::vector3df(0.0f);
        a_cLine.end   = a_cLine.start;

        return false;
      }
      else if (a_iIndex == 1) {
        a_cLine.start = m_vPoints[0];
        a_cLine.end   = a_cLine.start + 1000.0f * (m_vPoints[1] - m_vPoints[0]).normalize();

        a_cPlanePoint  = m_vPoints[0];
        a_cPlaneNormal = getNormal();

        return false;
      }
      else if (a_iIndex == 2 || a_iIndex == 3) {
        a_cPlanePoint  = m_vPoints[a_iIndex];
        a_cPlaneNormal = getNormal();

        printf("Index: %i, next: %s, next->previous: %s\n", a_iIndex, getNext() == nullptr ? "nullptr" : "ok", getNext() == nullptr ? "" : getNext()->getPrevious() == nullptr ? "nullptr" : "ok");
        if (a_iIndex == 2 && getNext() != nullptr && getNext()->isStartSegment()) {
          a_cLine.start = irr::core::vector3df();
          a_cLine.end   = irr::core::vector3df(0.0f, 0.0f, -1000.0f);

          return false;
        }

        return true;
      }
      else {
        a_cLine.start = irr::core::vector3df(0.0f);
        a_cLine.end   = a_cLine.start;

        return false;
      }
    }

    /**
    * Get the direction of the end of this track part
    */
    const irr::core::vector3df CTrackPart_Bezier::getEndDirection() {
      return (m_vPoints[m_vPoints.size() - 1] - m_vPoints[m_vPoints.size() - 2]).normalize();
    }

    /**
    * Get the normal at the end point
    * @return the normal at the end point
    */
    const irr::core::vector3df &CTrackPart_Bezier::getNormal() {
      return getStartNormal();
    }

    /**
    * Add this track part to the AI path node
    * @param a_pAi the AI path node
    * @param a_pLast the last AI path section
    */
    void CTrackPart_Bezier::addToAiNode(scenenodes::CAiPathNode *a_pAi, scenenodes::CAiPathNode::SAiPathSection *a_pLast) {
      if (m_vTrack.size() == 0) {
        calculateCurve();
      }

      int l_iIndex = 0;

      int l_iMax = (int)m_vTrack.size() - 4;

      while (l_iIndex < l_iMax) {
        irr::core::vector3df l_cDirection = (l_iIndex > 0 ? m_vTrack[l_iIndex - 1] - m_vTrack[l_iIndex] : m_vPoints[0] - m_vPoints[1]).normalize();
        irr::core::vector3df l_cNormal    = irr::core::vector3df(0.0f, 1.0f, 0.0f);
        irr::core::vector3df l_cSide      = l_cDirection.crossProduct(l_cNormal).normalize();

        scenenodes::CAiPathNode::SAiPathSection *l_pThis = new scenenodes::CAiPathNode::SAiPathSection();

        l_pThis->m_cPosition   = 50.0f * m_vTrack[l_iIndex] + 2.51f * l_cNormal;
        l_pThis->m_cSideVector = l_cSide;
        l_pThis->m_fWidth      = 25.0f * m_fWidth;
        l_pThis->m_cNormal     = getNormal();
        l_pThis->m_fFactor     = 0.90f;

        if (a_pLast != nullptr) {
          a_pLast->m_vNextSegments.push_back(l_pThis);
          l_pThis->m_vPrevSegments.push_back(a_pLast);
        }

        a_pAi->addPathSection(l_pThis);

        a_pLast = l_pThis;

        l_iIndex += 2;
      }

      if (getNext() != nullptr && getNext()->getPrevious() != nullptr) {
        getNext()->addToAiNode(a_pAi, a_pLast);
      }
      else if (getNext() != nullptr) {
        scenenodes::CAiPathNode::SAiPathSection *l_pFirst = a_pAi->getFirstSection();

        if (l_pFirst != nullptr) {
          a_pLast->m_vNextSegments.push_back(l_pFirst);
          l_pFirst->m_vPrevSegments.push_back(a_pLast);
        }
      }
    }
  }
}