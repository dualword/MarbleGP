// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Jump.h>
#include <CDefines.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Jump::CTrackPart_Jump(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart_Bezier(c_sTypeJump, a_pPrevious, a_cNormal)
    {
      std::vector<irr::core::vector3df> l_vRef;

      irr::core::vector3df l_cDir  = getPrevious() != nullptr ? getPrevious()->getEndDirection   () : irr::core::vector3df(0.0f, 0.0f, 1.0f);
      irr::core::vector3df l_cEnd  = getPrevious() != nullptr ? getPrevious()->getEnd().getMiddle() : irr::core::vector3df(0.0f, 0.0f, 0.0f);

      m_cSide = getPrevious() != nullptr ? getPrevious()->getEndSide() : irr::core::vector3df(1.0f, 0.0f, 0.0f);

      printf("Previous: %s\n", getPrevious() == nullptr ? "nullptr" : "ok");

      l_vRef.push_back(l_cEnd);
      l_vRef.push_back(l_cEnd +        getWidth() * l_cDir);
      l_vRef.push_back(l_cEnd + 2.0f * getWidth() * l_cDir + 0.5f * getWidth() * irr::core::vector3df(0.0f, 1.0f, 0.0f));

      l_vRef.push_back(l_cEnd + 4.0f * getWidth() * l_cDir);
      l_vRef.push_back(l_cEnd + 8.0f * getWidth() * l_cDir);

      setReferencePoints(l_vRef);
    }

    CTrackPart_Jump::~CTrackPart_Jump() {
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Jump::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      m_vTrackPoints[0].clear();
      m_vTrackPoints[1].clear();

      if (m_vPoints.size () == 5) {
        std::vector<irr::core::vector3df> l_vRamp;
        std::vector<irr::core::vector3df> l_vLand;

        l_vRamp.push_back(m_vPoints[0]);
        l_vRamp.push_back(m_vPoints[1]);
        l_vRamp.push_back(m_vPoints[2]);

        l_vLand.push_back(m_vPoints[3]);
        l_vLand.push_back(m_vPoints[4]);

        irr::f32 l_aLength[] = {
          calculateCurve(l_vRamp, &m_vTrackPoints[0]),
          calculateCurve(l_vLand, &m_vTrackPoints[1], true)
        };

        irr::core::line3df l_cOld = getPrevious() != nullptr ? getPrevious()->getEnd() : irr::core::line3df(m_fWidth / 2.0f, 0.0f, 0.0f, -m_fWidth / 2.0f, 0.0f, 0.0f);

        for (int j = 0; j < 2; j++) {
          if (j == 1)
            m_vNormals.push_back(irr::core::vector3df(0.0f, 1.0f, 0.0f));

          irr::f32 l_fCurrentLength = 0.0f;

          int l_iSteps = std::max((int)(l_aLength[j] / m_fWidth), 1);

          irr::core::vector2df l_aTextureTop[] = {
            irr::core::vector2df(0.0f, 0.0f),
            irr::core::vector2df(1.0f, 0.0f),
            irr::core::vector2df(1.0f, 0.0f),
            irr::core::vector2df(0.0f, 0.0f)
          };

          irr::core::vector3df l_aEnd[4];

          for (size_t i = 0; i < m_vTrackPoints[j].size(); i++) {
            irr::core::line3df l_cNew = irr::core::line3df(m_vTrackPoints[j][i] - getWidth() / 2.0f * m_cSide, m_vTrackPoints[j][i] + getWidth() / 2.0f * m_cSide);

            if (j == 0 || i != 0) {
              irr::core::vector3df l_aVertBottom[] = { l_cOld.start, l_cOld.end, l_cNew.end, l_cNew.start };

              irr::core::vector2df t[] = {
                irr::core::vector2df(l_aVertBottom[0].X, l_aVertBottom[0].Z),
                irr::core::vector2df(l_aVertBottom[1].X, l_aVertBottom[1].Z),
                irr::core::vector2df(l_aVertBottom[2].X, l_aVertBottom[2].Z),
                irr::core::vector2df(l_aVertBottom[3].X, l_aVertBottom[3].Z)
              };

              irr::core::vector3df l_aNormals[4];

              if (a_vBuffers.size() >= c_iBufferIdxBottom)
                addQuadToMeshBuffer(l_aVertBottom, t, a_vBuffers[c_iBufferIdxBottom], false, false, l_aNormals);

              l_fCurrentLength += (l_cNew.getMiddle() - l_cOld.getMiddle()).getLength();

              l_aTextureTop[2].Y = -(irr::f32)l_iSteps * l_fCurrentLength / l_aLength[j];
              l_aTextureTop[3].Y = -(irr::f32)l_iSteps * l_fCurrentLength / l_aLength[j];

              irr::core::vector3df l_aVertTop[] = {
                l_aVertBottom[1] - 0.05f * l_aNormals[1],
                l_aVertBottom[0] - 0.05f * l_aNormals[0],
                l_aVertBottom[3] - 0.05f * l_aNormals[3],
                l_aVertBottom[2] - 0.05f * l_aNormals[2]
              };

              if (a_vBuffers.size() > tracked::c_iBufferIdxRoad)
                addQuadToMeshBuffer(l_aVertTop, l_aTextureTop, a_vBuffers[j == 0 ? tracked::c_iBufferIdxArrow : tracked::c_iBufferIdxRoad], i == 0, false, nullptr);

              l_aTextureTop[0].Y = l_aTextureTop[2].Y;
              l_aTextureTop[1].Y = l_aTextureTop[3].Y;

              if (j == 0)
                m_vNormals.push_back(-l_aNormals[2]);

              l_aEnd[0] = l_aVertBottom[3];
              l_aEnd[1] = l_aVertBottom[2];
              l_aEnd[2] = l_aVertTop[3];
              l_aEnd[3] = l_aVertTop[2];

              for (int j = 0; j < 2; j++) {
                irr::core::vector3df l_cSide[] = {
                  l_aVertBottom[j == 0 ? 1 : 3],
                  l_aVertTop[j == 0 ? 0 : 2],
                  l_aVertTop[j == 0 ? 3 : 1],
                  l_aVertBottom[j == 0 ? 2 : 0]
                };

                irr::core::vector2df l_aTextureSide[] = {
                  irr::core::vector2df(0.0f, 0.0f),
                  irr::core::vector2df(0.0f, 1.0f),
                  irr::core::vector2df(1.0f, 1.0f),
                  irr::core::vector2df(1.0f, 0.0f)
                };

                if ((a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxYellow) || (!a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxBlack)) {
                  addQuadToMeshBuffer(l_cSide, l_aTextureSide, a_vBuffers[a_bBlack ? tracked::c_iBufferIdxYellow : tracked::c_iBufferIdxBlack], false, false, nullptr);
                }
              }
              a_bBlack = !a_bBlack;
            }

            l_cOld = l_cNew;
          }

          if (j == 0) {
            irr::core::vector2df l_aTextureEnd[] = {
              irr::core::vector2df(0.0f, 0.0f),
              irr::core::vector2df(0.0f, 1.0f),
              irr::core::vector2df(1.0f, 1.0f),
              irr::core::vector2df(1.0f, 0.0f)
            };

            if ((a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxYellow) || (!a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxBlack)) {
              addQuadToMeshBuffer(l_aEnd, l_aTextureEnd, a_vBuffers[a_bBlack ? tracked::c_iBufferIdxYellow : c_iBufferIdxBlack], false, false, nullptr);
            }

            irr::core::vector3df l_aLand[] = {
              m_vTrackPoints[1].front() + 0.5f * getWidth() * m_cSide,
              m_vTrackPoints[1].front() - 0.5f * getWidth() * m_cSide,
              m_vTrackPoints[1].front() - 0.5f * getWidth() * m_cSide + irr::core::vector3df(0.0f, 0.05f, 0.0f),
              m_vTrackPoints[1].front() + 0.5f * getWidth() * m_cSide + irr::core::vector3df(0.0f, 0.05f, 0.0f)
            };

            if ((a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxYellow) || (!a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxBlack)) {
              addQuadToMeshBuffer(l_aLand, l_aTextureEnd, a_vBuffers[a_bBlack ? tracked::c_iBufferIdxYellow : c_iBufferIdxBlack], false, false, nullptr);
            }
          }

          setEnd(l_cOld);
        }

        setEndNormal(irr::core::vector3df(0.0f, 1.0f, 0.0f));
      }
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Jump::serialize(irr::io::IAttributes &a_cAttr) const {
      ITrackPart_Bezier::serialize(a_cAttr);
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Jump::deserialize(const irr::io::IAttributes &a_cAttr) {
      ITrackPart_Bezier::deserialize(a_cAttr);
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Jump::refPointAdditionAllowed(int a_iIndex) {
      // no further reference points for the jump
      return false;
    }

    /**
    * Get the (normalized) normal vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) normal vector of a point
    */
    irr::core::vector3df CTrackPart_Jump::getNormalOfPoint(int a_iIndex) const {
      switch (a_iIndex) {
        case 0: return irr::core::vector3df(0.0f, 1.0f, 0.0f);
        case 1: return irr::core::vector3df(0.0f, 1.0f, 0.0f);
        case 2: return (m_vPoints[2] - m_vPoints[0]).crossProduct(getSideOfPoint(2)).normalize();
        case 3: return irr::core::vector3df(0.0f, 1.0f, 0.0f);
        case 4: return irr::core::vector3df(0.0f, 1.0f, 0.0f);

        default:
          return irr::core::vector3df(0.0f, 1.0f, 0.0f);
      }
    }

    /**
    * Get the (normalized) side vector of a point
    * @param a_iIndex the point index
    * @return the (normalized) side vector of a point
    */
    irr::core::vector3df CTrackPart_Jump::getSideOfPoint(int a_iIndex) const {
      return m_cSide;
    }

    /**
    * Get the side vector at a specific index of the road
    * @param a_iIndex the index
    * @return the side vector at the index
    */
    irr::core::vector3df CTrackPart_Jump::getSideAtIndex(int a_iIndex) {
      return m_cSide;
    }

    /**
    * Get the normal at a specific index of the road
    * @param a_iIndex the index
    * @return the normal at the index
    */
    irr::core::vector3df CTrackPart_Jump::getNormalAtIndex(int a_iIndex) {
      return a_iIndex >= 0 && a_iIndex < m_vNormals.size() ? m_vNormals[a_iIndex] : irr::core::vector3df(0.0f, 1.0f, 0.0f);
    }

    /**
    * Get the side vector of the end of this track part
    * @return the side vector of the end of this track part
    */
    irr::core::vector3df CTrackPart_Jump::getEndSide() const {
      return m_cSide;
    }

    /**
    * This method is called when the reference points of the previous track part have changed
    */
    void CTrackPart_Jump::updateReferencePoints() {
      ITrackPart *l_pPrevious = getPrevious();

      if (l_pPrevious != nullptr && m_vPoints.size() == 5) {
        irr::core::vector3df l_cDir = l_pPrevious->getEndDirection();
        irr::core::vector3df l_cPos = l_pPrevious->getEnd().getMiddle();

        irr::f32 l_fDistXZ = irr::core::vector3df(m_vPoints[2].X, 0.0f, m_vPoints[2].Z).getDistanceFrom(irr::core::vector3df(m_vPoints[0].X, 0.0f, m_vPoints[0].Z));

        std::vector<irr::core::vector3df> l_cPoints;

        l_cPoints.push_back(l_cPos);
        l_cPoints.push_back(l_cPos + m_vPoints[1].getDistanceFrom(m_vPoints[0]) * l_cDir);
        l_cPoints.push_back(l_cPos + l_fDistXZ * l_cDir + (m_vPoints[2].Y - m_vPoints[0].Y) * irr::core::vector3df(0.0f, 1.0f, 0.0f));

        l_fDistXZ = irr::core::vector3df(m_vPoints[3].X, 0.0f, m_vPoints[3].Z).getDistanceFrom(irr::core::vector3df(m_vPoints[0].X, 0.0f, m_vPoints[0].Z));

        l_cPoints.push_back(l_cPos + l_fDistXZ * l_cDir + (m_vPoints[3].Y - m_vPoints[0].Y) * irr::core::vector3df(0.0f, 1.0f, 0.0f));
        l_cPoints.push_back(l_cPoints.back() + l_cDir * m_vPoints[4].getDistanceFrom(m_vPoints[3]));

        for (auto &l_cPoint : l_cPoints)
          if (l_cPoint.Y < 0.0f)
            l_cPoint.Y = 0.0f;

        m_cSide = l_pPrevious->getEndSide();

        setReferencePoints(l_cPoints);
      }
      else m_cSide = irr::core::vector3df(1.0f, 0.0f, 0.0f);
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Jump::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      std::vector<irr::core::vector3df> l_cPoints = a_cPoints;

      if (l_cPoints.size() == 5 && m_vPoints.size() == 5) {
        if (l_cPoints[3] != m_vPoints[3]) {
          l_cPoints[4].Y = l_cPoints[3].Y;
        }

        if (l_cPoints[1] != m_vPoints[1]) {
          irr::f32 l_fDist = m_vPoints[1].getDistanceFrom(irr::core::vector3df(m_vPoints[2].X, m_vPoints[1].Y, m_vPoints[2].Z));
          printf("==> %.2f\n", l_fDist);

          irr::core::vector2df l_cDirXZ = irr::core::vector2df(m_vPoints[2].X - m_vPoints[1].X, m_vPoints[2].Z - m_vPoints[1].Z).normalize();
          l_cPoints[2] = irr::core::vector3df(l_cPoints[1].X + l_fDist * l_cDirXZ.X, m_vPoints[2].Y, l_cPoints[1].Z + l_fDist * l_cDirXZ.Y);
        }
      }

      ITrackPart_Bezier::setReferencePoints(l_cPoints);
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Jump::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      a_cLine.end    = a_cLine.start;
      a_cPlanePoint  = m_vPoints.size() > 0 ? m_vPoints[0] : irr::core::vector3df(0.0f);
      a_cPlaneNormal = m_cSide;

      if (m_vPoints.size() == 5) {
        if (a_iIndex == 0) {
          a_cLine.end = a_cLine.start;
          return false;
        }
        else if (a_iIndex == 1) {
          a_cLine.start = m_vPoints[0];
          a_cLine.end   = m_vPoints[0] + 1000.0f * (m_vPoints[1] - m_vPoints[0]).normalize();

          return false;
        }
        else if (a_iIndex == 2) {
          a_cPlanePoint = m_vPoints[2];

          return true;
        }
        else if (a_iIndex == 3) {
          a_cPlanePoint = m_vPoints[3];

          return true;
        }
        else if (a_iIndex == 4) {
          a_cLine.start = m_vPoints[3];
          a_cLine.end   = m_vPoints[3] + 1000.0f * (m_vPoints[4] - m_vPoints[3]).normalize();

          return false;
        }
      }
      

      return false;
    }

    /**
    * Does this reference point allow addition of new track parts?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Jump::partAdditionAllowed(int a_iIndex) {
      return a_iIndex == 4;
    }

    /**
    * Get the information on the route for the marbles
    * @return a vector with positions to pass for the marbles
    */
    std::vector<SAiPath> CTrackPart_Jump::getAiPathData() {
      if (m_vPoints.size() == 5) {
        std::vector<irr::core::vector3df> l_vRamp;
        std::vector<irr::core::vector3df> l_vLand;

        l_vRamp.push_back(m_vPoints[0]);
        l_vRamp.push_back(m_vPoints[1]);
        l_vRamp.push_back(m_vPoints[2]);

        l_vLand.push_back(m_vPoints[3]);
        l_vLand.push_back(m_vPoints[4]);

        calculateCurve(l_vRamp, &m_vTrackPoints[0]);
        calculateCurve(l_vLand, &m_vTrackPoints[1], true);
     }

      std::vector<SAiPath> l_vRet;

      l_vRet.push_back(SAiPath(enPathType::Standard, m_vPoints[0] + 0.05f * getNormalOfPoint(0), getNormalOfPoint(0), getSideOfPoint(0), getWidth(), this));
      l_vRet.push_back(SAiPath(enPathType::Jump    , m_vPoints[2] + 0.05f * getNormalOfPoint(2), getNormalOfPoint(2), getSideOfPoint(2), getWidth(), this)); l_vRet.back().m_bCalMinVel = true;
      l_vRet.push_back(SAiPath(enPathType::Standard, m_vPoints[3] + 0.05f * getNormalOfPoint(3), getNormalOfPoint(3), getSideOfPoint(3), getWidth(), this));
      l_vRet.push_back(SAiPath(enPathType::Standard, m_vPoints[4] + 0.05f * getNormalOfPoint(4), getNormalOfPoint(4), getSideOfPoint(4), getWidth(), this));

      return l_vRet;
    }
  }
}