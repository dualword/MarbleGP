// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_FlatSegment.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>
#include <cmath>

namespace dustbin {
  namespace tracked {
    CTrackPart_FlatSegment::CTrackPart_FlatSegment(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart_Bezier(c_sTypeFlat, a_pPrevious, a_cNormal) 
    {
      setEndNormal(a_cNormal);
    }

    CTrackPart_FlatSegment::~CTrackPart_FlatSegment() {
    }

    /**
    * Get the (normalized) normal vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) normal vector of a point
    */
    irr::core::vector3df CTrackPart_FlatSegment::getNormalOfPoint(int a_iIndex) const {
      // For a flat segment the normal is always the same
      return getStartNormal();
    }

    /**
    * Get the (normalized) side vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) side vector of a point
    */
    irr::core::vector3df CTrackPart_FlatSegment::getSideOfPoint(int a_iIndex) const {
      if (a_iIndex == 0) {
        return (m_vPoints[0] - m_vPoints[1]).crossProduct(getNormalOfPoint(0)).normalize();
      }
      else if (a_iIndex == (int)m_vPoints.size() - 1) {
        return (m_vPoints[a_iIndex - 1] - m_vPoints[a_iIndex]).crossProduct(getNormalOfPoint(a_iIndex)).normalize();
      }
      else {
        irr::core::vector3df v1 = (m_vPoints[a_iIndex    ] - m_vPoints[a_iIndex + 1]).crossProduct(getNormalOfPoint(a_iIndex)).normalize();
        irr::core::vector3df v2 = (m_vPoints[a_iIndex - 1] - m_vPoints[a_iIndex    ]).crossProduct(getNormalOfPoint(a_iIndex)).normalize();

        return v1.getInterpolated(v2, 0.5);
      }
    }

    /**
    * Get the side vector at a specific index of the road
    * @param a_iIndex the index
    * @return the side vector at the index
    */
    irr::core::vector3df CTrackPart_FlatSegment::getSideAtIndex(int a_iIndex) {
      if (m_vTrack.size() > 0) {
        irr::core::vector3df l_cNormal    = getNormalOfPoint(0);
        irr::core::vector3df l_cDirection = (a_iIndex > 0 ? m_vTrack[a_iIndex] - m_vTrack[a_iIndex - 1] : m_vPoints[1] - m_vPoints[0]).normalize();

        return l_cNormal.crossProduct(l_cDirection).normalize();
      }
      return irr::core::vector3df(1.0f, 0.0f, 0.0f);
    }
 
    /**
    * Get the side vector of the end of this track part
    * @return the side vector of the end of this track part
    */
    irr::core::vector3df CTrackPart_FlatSegment::getEndSide() const {
      return getSideOfPoint((int)m_vPoints.size() - 1);
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_FlatSegment::updateReferencePoints() {
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
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_FlatSegment::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      if (getPrevious() == nullptr) {
        a_cLine.start = irr::core::vector3df(0.0f);
        a_cLine.end   = a_cLine.start;

        return false;
      }
      else if (a_iIndex == 1) {
        a_cLine.start = m_vPoints[0];
        a_cLine.end   = a_cLine.start + 1000.0f * (m_vPoints[1] - m_vPoints[0]).normalize();

        a_cPlanePoint  = m_vPoints[0];
        a_cPlaneNormal = getEndNormal();

        return false;
      }
      else if (a_iIndex == 2 || a_iIndex == 3) {
        a_cPlanePoint  = m_vPoints[a_iIndex];
        a_cPlaneNormal = getEndNormal();

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
  }
}