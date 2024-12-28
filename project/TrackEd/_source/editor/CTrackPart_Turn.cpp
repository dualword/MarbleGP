// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Turn.h>
#include <CDefines.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Turn::CTrackPart_Turn(CTrackPart_Turn::enTurnAngle a_eAngle, bool a_bLeft, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      CTrackPart_FlatSegment(a_pPrevious, a_cNormal, c_sTypeTurn),
      m_eAngle(a_eAngle),
      m_bLeft (a_bLeft)
    {
      m_aDistance[0] = 2.0f * getWidth();
      m_aDistance[1] = 2.0f * getWidth();
      m_aDistance[2] = 2.0f * getWidth();

      updateReferencePoints();
    }

    CTrackPart_Turn::~CTrackPart_Turn() {
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in. Only used if "true" is returned.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Turn::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      a_cPlanePoint  = m_vPoints[a_iIndex];
      a_cPlaneNormal = getEndNormal();

      irr::core::vector3df l_cSide = m_bLeft ? getPrevious()->getEnd().start - getPrevious()->getEnd().end : getPrevious()->getEnd().end - getPrevious()->getEnd().start;
      irr::core::vector3df l_cDir  = getPrevious()->getEndDirection();

      l_cSide = l_cSide.normalize();
      l_cDir  = l_cDir .normalize();

      switch (m_eAngle) {
        case enTurnAngle::FourtyFive:
          if (a_iIndex == 1) {
            a_cLine = irr::core::line3df(m_vPoints[0] + 2.0f * getWidth() * getPrevious()->getEndDirection(), m_vPoints[0] + 1000.0f * getPrevious()->getEndDirection());
            return false;
          }
          else if (a_iIndex == 2) {
            irr::core::vector3df l_cDir45 = l_cDir.getInterpolated(l_cSide, 0.5).normalize();
            a_cLine = irr::core::line3df(m_vPoints[1] + getWidth() * l_cDir45, m_vPoints[1] + getWidth() * l_cDir45 + 1000.0f * l_cDir45);
            return false;
          }
          break;

        case enTurnAngle::Ninety:
          if (a_iIndex == 1) {
            a_cLine = irr::core::line3df(m_vPoints[0] + 2.0f * getWidth() * getPrevious()->getEndDirection(), m_vPoints[0] + 1000.0f * getPrevious()->getEndDirection());
            return false;
          }
          else if (a_iIndex == 2) {
            a_cLine = irr::core::line3df(m_vPoints[1] + getWidth() * l_cSide, m_vPoints[1] + 1000.0f * l_cSide);
            return false;
          }     
          break;

        case enTurnAngle::OneHundred35:
          if (a_iIndex == 1) {
            a_cLine = irr::core::line3df(m_vPoints[0] + 2.0f * getWidth() * getPrevious()->getEndDirection(), m_vPoints[0] + 1000.0f * getPrevious()->getEndDirection());
            return false;
          }
          else if (a_iIndex == 2) {
            a_cLine = irr::core::line3df(m_vPoints[1] + getWidth() * l_cSide, m_vPoints[1] + 1000.0f * l_cSide);
            return false;
          }
          else if (a_iIndex == 3) {
            irr::core::vector3df l_cDir45 = l_cSide.getInterpolated(-l_cDir, 0.5).normalize();
            a_cLine = irr::core::line3df(m_vPoints[2] + getWidth() * l_cDir45, m_vPoints[2] + getWidth() * l_cDir45 + 1000.0f * l_cDir45);
            return false;
          }
          break;

        case enTurnAngle::OneHundred80:
          if (a_iIndex == 1) {
            a_cLine = irr::core::line3df(m_vPoints[0] + 2.0f * getWidth() * getPrevious()->getEndDirection(), m_vPoints[0] + 1000.0f * getPrevious()->getEndDirection());
            return false;
          }
          else if (a_iIndex == 2) {
            a_cLine = irr::core::line3df(m_vPoints[1] + getWidth() * l_cSide, m_vPoints[1] + 1000.0f * l_cSide);
            return false;
          }
          else if (a_iIndex == 3) {
            a_cLine = irr::core::line3df(
              m_vPoints[2] - 2.0f * getWidth() * getPrevious()->getEndDirection(), 
              m_vPoints[2] - 1000.0f * getPrevious()->getEndDirection()
            );
            return false;
          }
          break;

      }

      a_cLine.start = irr::core::vector3df();
      a_cLine.end   = a_cLine.start;

      return false;
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    * @return the new reference points
    */
    void CTrackPart_Turn::updateReferencePoints() {
      CTrackPart_FlatSegment::setReferencePoints(calculateReferencePoints());
    }

    /**
    * Calculate the reference points from the distance values
    */
    std::vector<irr::core::vector3df> CTrackPart_Turn::calculateReferencePoints() {
      irr::core::vector3df l_cEnd  = getPrevious() != nullptr ? getPrevious()->getEnd().getMiddle() : irr::core::vector3df();
      irr::core::vector3df l_cDir  = getPrevious() != nullptr ? getPrevious()->getEndDirection() : irr::core::vector3df(0.0f, 0.0f, -1.0f);
      irr::core::vector3df l_cSide = getPrevious() != nullptr ? m_bLeft ? getPrevious()->getEnd().start - getPrevious()->getEnd().end : getPrevious()->getEnd().end - getPrevious()->getEnd().start : irr::core::vector3df(1.0f, 0.0f, 0.0f);

      irr::f32 l_fWidth  = getWidth();
      irr::f32 l_fFactor = m_bLeft ? -1.0f : 1.0f;

      l_cDir  = l_cDir .normalize();
      l_cSide = l_cSide.normalize();

      std::vector<irr::core::vector3df> l_vRef;

      switch (m_eAngle) {
        case enTurnAngle::FourtyFive: {
          l_vRef = {
            l_cEnd,
            l_cEnd + m_aDistance[0] * l_cDir,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cDir.getInterpolated(l_cSide, 0.5)
          };
          break;
        }

        case enTurnAngle::Ninety: {
          l_vRef = {
            l_cEnd,
            l_cEnd + m_aDistance[0] * l_cDir,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cSide
          };
          break;
        }

        case enTurnAngle::OneHundred35: {
          irr::core::vector3df l_cDir45 = l_cSide.getInterpolated(-l_cDir, 0.5).normalize();

          l_vRef = {
            l_cEnd,
            l_cEnd + m_aDistance[0] * l_cDir,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cSide,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cSide + m_aDistance[2] * l_cDir45
          };
          break;
        }

        case enTurnAngle::OneHundred80: {
          l_vRef = {
            l_cEnd,
            l_cEnd + m_aDistance[0] * l_cDir,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cSide,
            l_cEnd + m_aDistance[0] * l_cDir + m_aDistance[1] * l_cSide - m_aDistance[2] * l_cDir
          };
          break;
        }
      }

      return l_vRef;
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Turn::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      if (m_vPoints.size() >= 2 && a_cPoints.size() >= 2 && m_vPoints[1] != a_cPoints[1]) {
        m_aDistance[0] = a_cPoints[1].getDistanceFrom(a_cPoints[0]);
      }
      else if (m_vPoints.size() >= 3 && a_cPoints.size() >= 3 && m_vPoints[2] != a_cPoints[2]) {
        m_aDistance[1] = a_cPoints[2].getDistanceFrom(a_cPoints[1]);
      }
      else if (m_vPoints.size() >= 4 && a_cPoints.size() >= 4 && m_vPoints[3] != a_cPoints[3]) {
        m_aDistance[2] = a_cPoints[3].getDistanceFrom(a_cPoints[2]);
      }

      std::vector<irr::core::vector3df> l_vPoints = calculateReferencePoints();
      CTrackPart_FlatSegment::setReferencePoints(l_vPoints);
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Turn::refPointAdditionAllowed(int a_iIndex) {
      // For the turns with fixed angle we never allow the addition of more reference points
      return false;
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Turn::serialize(irr::io::IAttributes &a_cAttr) const {
      CTrackPart_FlatSegment::serialize(a_cAttr);

      a_cAttr.addBool("Left" , m_bLeft);
      a_cAttr.addInt ("Angle", (int)m_eAngle);

      for (int i = 0; i < 3; i++) {
        std::string l_sName = "Distance_" + std::to_string(i);
        a_cAttr.addFloat(l_sName.c_str(), m_aDistance[i]);
      }
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Turn::deserialize(const irr::io::IAttributes &a_cAttr) {
      CTrackPart_FlatSegment::deserialize(a_cAttr);

      m_bLeft = a_cAttr.getAttributeAsBool("Left", false);
      m_eAngle = (enTurnAngle)a_cAttr.getAttributeAsInt("Angle", 0);

      for (int i = 0; i < 3; i++) {
        std::string l_sName = "Distance_" + std::to_string(i);
        m_aDistance[i] = a_cAttr.getAttributeAsFloat(l_sName.c_str(), m_aDistance[i]);
      }

      updateReferencePoints();
    }
  }
}