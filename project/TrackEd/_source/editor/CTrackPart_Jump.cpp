// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackPart_Jump.h>
#include <CDefines.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Jump::CTrackPart_Jump(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart_Bezier(c_sTypeJump, a_pPrevious, a_cNormal)
    {
      irr::core::vector3df l_cDir  = a_pPrevious != nullptr ? a_pPrevious->getEndDirection   () : irr::core::vector3df(0.0f, 0.0f, 1.0f);
      irr::core::vector3df l_cEnd  = a_pPrevious != nullptr ? a_pPrevious->getEnd().getMiddle() : irr::core::vector3df(0.0f, 0.0f, 0.0f);

      m_cSide = a_pPrevious != nullptr ? a_pPrevious->getEndSide() : irr::core::vector3df(1.0f, 0.0f, 0.0f);

      m_vRamp.push_back(l_cEnd);
      m_vRamp.push_back(l_cEnd +        getWidth() * l_cDir);
      m_vRamp.push_back(l_cEnd + 2.0f * getWidth() * l_cDir + 0.5f * getWidth() * irr::core::vector3df(0.0f, 1.0f, 0.0f));

      m_vLand.push_back(l_cEnd + 4.0f * getWidth() * l_cDir);
      m_vLand.push_back(l_cEnd + 8.0f * getWidth() * l_cDir);

      for (auto l_cPoint : m_vRamp) m_vPoints.push_back(l_cPoint);
      for (auto l_cPoint : m_vLand) m_vPoints.push_back(l_cPoint);

      setEnd(irr::core::line3df(m_vLand.back() + getWidth() / 2.0f * m_cSide, m_vLand.back() - getWidth() / 2.0f * m_cSide));
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

      irr::f32 l_aLength[] = {
        calculateCurve(m_vRamp, &m_vTrackPoints[0]),
        calculateCurve(m_vLand, &m_vTrackPoints[1], true)
      };

      irr::core::line3df l_cOld = getPrevious()->getEnd();

      for (int j = 0; j < 2; j++) {
        irr::f32 l_fCurrentLength = 0.0f;

        int l_iSteps = j == 0 ?std::max((int)(l_aLength[j] / m_fWidth) - 1, 1) : std::max((int)(l_aLength[j] / m_fWidth), 1);

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

            l_aTextureTop[2].Y = (irr::f32)l_iSteps * l_fCurrentLength / l_aLength[j];
            l_aTextureTop[3].Y = (irr::f32)l_iSteps * l_fCurrentLength / l_aLength[j];

            irr::core::vector3df l_aVertTop[] = {
              l_aVertBottom[1] - 0.05f * l_aNormals[1],
              l_aVertBottom[0] - 0.05f * l_aNormals[0],
              l_aVertBottom[3] - 0.05f * l_aNormals[3],
              l_aVertBottom[2] - 0.05f * l_aNormals[2]
            };

            if (a_vBuffers.size() > tracked::c_iBufferIdxRoad)
              addQuadToMeshBuffer(l_aVertTop, l_aTextureTop, a_vBuffers[tracked::c_iBufferIdxRoad], i == 0, false, nullptr);

            l_aTextureTop[0].Y = l_aTextureTop[2].Y;
            l_aTextureTop[1].Y = l_aTextureTop[3].Y;

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

    /**
    * Get the information on the route for the marbles
    * @return a vector with positions to pass for the marbles
    */
    std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> CTrackPart_Jump::getRoute() {
      std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> l_vRet;

      return l_vRet;
    }

    /**
    * Get the part's reference points
    * @return a vector with the part's reference points
    */
    const std::vector<irr::core::vector3df> CTrackPart_Jump::getReferencePoints() {
      std::vector<irr::core::vector3df> l_vRet;

      for (auto l_cPoint : m_vRamp)
        l_vRet.push_back(l_cPoint);

      for (auto l_cPoint: m_vLand)
        l_vRet.push_back(l_cPoint);

      return l_vRet;
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Jump::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
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
        case 2: return (m_vRamp[2] - m_vRamp[0]).crossProduct(getSideOfPoint(2)).normalize();
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
      return irr::core::vector3df();
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
  }
}