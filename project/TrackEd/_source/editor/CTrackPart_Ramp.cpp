// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Ramp.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Ramp::CTrackPart_Ramp(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart_Bezier(c_sTypeRamp, a_pPrevious, a_cNormal) {
    }

    CTrackPart_Ramp::~CTrackPart_Ramp() {
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_Ramp::updateReferencePoints() {
      if (m_vPoints.size() == 3) {
        ITrackPart *l_pPrevious = getPrevious();

        if (l_pPrevious != nullptr) {
          std::vector<irr::core::vector3df> l_vRefPoints = getReferencePoints();

          irr::core::vector3df l_cDir = l_pPrevious->getEndDirection();
          irr::f32 l_fLength = (l_vRefPoints[1] - l_vRefPoints[0]).getLength();

          irr::f32 l_fDistXZ = irr::core::vector2df(l_vRefPoints[0].X, l_vRefPoints[0].Z).getDistanceFrom(irr::core::vector2df(l_vRefPoints[2].X, l_vRefPoints[2].Z));

          l_vRefPoints[0] = l_pPrevious->getEnd().getMiddle();
          l_vRefPoints[1] = l_vRefPoints[0] + (l_fLength * l_cDir.normalize());
          l_vRefPoints[2] = l_vRefPoints[0] + irr::core::vector3df(l_cDir.X, 0.0f, l_cDir.Z) * l_fDistXZ + irr::core::vector3df(0.0f, l_vRefPoints[2].Y, 0.0f);

          if (l_vRefPoints.size() > 3) {
            l_vRefPoints[3] = l_vRefPoints[0] + irr::core::vector3df(l_cDir.X, 0.0f, l_cDir.Z) * l_fDistXZ + irr::core::vector3df(0.0f, l_vRefPoints[3].Y, 0.0f);
          }

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
    bool CTrackPart_Ramp::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
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
      else {
        irr::core::vector3df l_cSide = ((m_vPoints[1] - m_vPoints[0]).crossProduct(getNormal())).normalize();

        a_cPlanePoint  = m_vPoints[1];
        a_cPlaneNormal = l_cSide;

        return true;
      }
    }
  }
}