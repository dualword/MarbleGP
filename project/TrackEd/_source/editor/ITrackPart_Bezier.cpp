// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/ITrackPart_Bezier.h>
#include <scenenodes/CDustbinId.h>
#include <CDefines.h>
#include <IGlobal.h>
#include <cmath>

namespace dustbin {
  namespace tracked {
    ITrackPart_Bezier::ITrackPart_Bezier(const std::string &a_sIdentifier, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) :
      ITrackPart (a_sIdentifier, a_pPrevious, a_cNormal) {
    }

    ITrackPart_Bezier::~ITrackPart_Bezier() {
    }

    /**
     * Get the next point using the Bezier1 algorithm
     * @param p array with the two points for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df ITrackPart_Bezier::getBezier1(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return (1 - a_fStep) * a_aPoints[0] + a_fStep * a_aPoints[1];
    }

    /**
     * Get the next point using the Bezier2 algorithm
     * @param p array with the three points (start, help, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df ITrackPart_Bezier::getBezier2(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return ((1 - a_fStep) * (1 - a_fStep)) * a_aPoints[0] + 2 * a_fStep * (1 - a_fStep) * a_aPoints[1] + a_fStep * a_fStep * a_aPoints[2];
    }

    /**
     * Get the next point using the Bezier3 algorithm
     * @param p array with the four points (start, help1, help2, end) for the algorithm
     * @param fStep step number (0..1)
     * @return the next point
     */
    irr::core::vector3df ITrackPart_Bezier::getBezier3(irr::core::vector3df a_aPoints[], irr::f32 a_fStep) {
      return (-a_aPoints[0] + 3.0f * a_aPoints[1] - 3.0f * a_aPoints[2] + a_aPoints[3]) * a_fStep * a_fStep * a_fStep + (3.0f * a_aPoints[0] - 6.0f * a_aPoints[1] + 3.0f * a_aPoints[2]) * a_fStep * a_fStep + (-3.0f * a_aPoints[0] + 3.0f * a_aPoints[1]) * a_fStep + a_aPoints[0];
    }

    /**
    * A little helper functions to fill the indices for the Bezier calculation
    * @param a_iSteps the number of steps
    * @param a_pPrevious the previous track part
    * @return a vector with the indices ranging from 0 to 1
    */
    std::vector<irr::f32> ITrackPart_Bezier::fillIndexVector(int a_iSteps, ITrackPart *a_pPrevious) {
      std::vector<irr::f32> l_vRet;

      for (int i = a_pPrevious == nullptr ? 0 : 1; i < a_iSteps; i++) {
        l_vRet.push_back((irr::f32)i / (irr::f32)a_iSteps);
      }

      l_vRet.push_back(1.0);

      return l_vRet;
    }

    /**
    * Get the information on the route for the marbles
    * @return a vector with positions to pass for the marbles
    */
    std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> ITrackPart_Bezier::getRoute() {
      if (m_vTrack.size() == 0)
        calculateCurve();

      std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> l_vRet;

      for (size_t i = 0; i < m_vTrack.size(); i++) {
        l_vRet.push_back(std::make_tuple(m_vTrack[i] + 0.1f * getNormalAtIndex((int)i), getNormalAtIndex((int)i)));
      }

      return l_vRet;
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void ITrackPart_Bezier::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      irr::f32 l_fLength = calculateCurve();
      irr::f32 l_fWidth  = m_fWidth / 2.0f;

      int l_iSteps = std::max((int)(l_fLength / m_fWidth), 1);
      
      irr::f32 l_fTextureStep = (irr::f32)l_iSteps / (irr::f32)(m_vTrack.size() - 1);
      
      printf("Road Length: %.4f, Texture Count: %i, Texture Step: %.4f\n", l_fLength, l_iSteps, l_fTextureStep);
      
      irr::core::vector2df l_aTextureTop[] = {
        irr::core::vector2df(0.0f, 0.0f),
        irr::core::vector2df(1.0f, 0.0f),
        irr::core::vector2df(1.0f, l_fTextureStep),
        irr::core::vector2df(0.0f, l_fTextureStep)
      };

      irr::core::line3df l_cOld = getPrevious() != nullptr ? getPrevious()->getEnd() : irr::core::line3df(m_vPoints[0] + l_fWidth * getSideOfPoint(0), m_vPoints[0] - l_fWidth * getSideOfPoint(0));

      for (size_t i = 0; i < m_vTrack.size(); i++) {
        irr::core::line3df l_cNew = irr::core::line3df(m_vTrack[i] - l_fWidth * getSideAtIndex((int)i), m_vTrack[i] + l_fWidth * getSideAtIndex((int)i));

        if (i == 0)
          setStart(l_cNew);

        bool l_bForceAdd2nd = i == m_vTrack.size() - 1 && getNext() != nullptr && getNext()->getPrevious() == nullptr;

        if (l_bForceAdd2nd) {
          l_cNew = getNext()->getStart();
        }

        irr::core::vector3df l_aVertBottom[] = {
          l_cOld.start,
          l_cOld.end,
          l_cNew.end,
          l_cNew.start
        };
      
        irr::core::vector2df t[] = {
          irr::core::vector2df(l_aVertBottom[0].X, l_aVertBottom[0].Z),
          irr::core::vector2df(l_aVertBottom[1].X, l_aVertBottom[1].Z),
          irr::core::vector2df(l_aVertBottom[2].X, l_aVertBottom[2].Z),
          irr::core::vector2df(l_aVertBottom[3].X, l_aVertBottom[3].Z)
        };
      
        irr::core::vector3df l_cNormals[4];
      
        if (a_vBuffers.size() > tracked::c_iBufferIdxBottom) {
          addQuadToMeshBuffer(l_aVertBottom, t, a_vBuffers[tracked::c_iBufferIdxBottom], false, l_bForceAdd2nd, l_cNormals);
        }
      
        irr::core::vector3df l_aVertTop[4];
      
        // The normal points downwards so we subtract it to get the road coordinate
        l_aVertTop[0] = l_aVertBottom[1] - 0.05f * l_cNormals[1];
        l_aVertTop[1] = l_aVertBottom[0] - 0.05f * l_cNormals[0];
        l_aVertTop[2] = l_aVertBottom[3] - 0.05f * l_cNormals[3];
        l_aVertTop[3] = l_aVertBottom[2] - 0.05f * l_cNormals[2];
      
        if (a_vBuffers.size() > tracked::c_iBufferIdxRoad) {
          addQuadToMeshBuffer(l_aVertTop, l_aTextureTop, a_vBuffers[tracked::c_iBufferIdxRoad], true, false, nullptr);
        }
      
        for (int j = 0; j < 4; j++)
          l_aTextureTop[j].Y += l_fTextureStep;
      
        if (i == m_vTrack.size()) {
          l_aTextureTop[2].Y = (irr::f32)l_iSteps;
          l_aTextureTop[3].Y = (irr::f32)l_iSteps;
        }
      
        for (int j = 0; j < 2; j++) {
          irr::core::vector3df l_cSide[] = {
            l_aVertBottom[j == 0 ? 1 : 3],
            l_aVertTop   [j == 0 ? 0 : 2],
            l_aVertTop   [j == 0 ? 3 : 1],
            l_aVertBottom[j == 0 ? 2 : 0]
          };
      
          irr::core::vector2df l_aTextureSide[] = {
            irr::core::vector2df(0.0f, 0.0f),
            irr::core::vector2df(0.0f, 1.0f),
            irr::core::vector2df(1.0f, 1.0f),
            irr::core::vector2df(1.0f, 0.0f)
          };
      
          if ((a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxBlack) || (!a_bBlack && a_vBuffers.size() > tracked::c_iBufferIdxYellow)) {
            addQuadToMeshBuffer(l_cSide, l_aTextureSide, a_vBuffers[a_bBlack ? tracked::c_iBufferIdxBlack : tracked::c_iBufferIdxYellow], false, false, nullptr);
          }
        }
        a_bBlack = !a_bBlack;
      
        l_cOld = l_cNew;
      }

      setEnd(l_cOld);
    }

    /**
    * Get the number of sections in this track part
    * @return the number of sections in this track part
    */
    int ITrackPart_Bezier::getCount() {
      if (m_vTrack.size() == 0)
        calculateCurve();

      return (int)m_vTrack.size();
    }

    /**
    * Does this reference point allow addition of new track parts?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool ITrackPart_Bezier::partAdditionAllowed(int a_iIndex) {
      return a_iIndex == m_vPoints.size() - 1 && getNext() == nullptr;
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool ITrackPart_Bezier::refPointAdditionAllowed(int a_iIndex) {
      return a_iIndex == m_vPoints.size() - 1 && m_vPoints.size() < 4 && getPrevious() != nullptr;
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void ITrackPart_Bezier::serialize(irr::io::IAttributes &a_cAttr) const {
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
    void ITrackPart_Bezier::deserialize(const irr::io::IAttributes &a_cAttr) {
      ITrackPart::deserialize(a_cAttr);

      irr::s32 l_iCount = a_cAttr.getAttributeAsInt("number_of_points");

      for (irr::s32 i = 0; i < l_iCount; i++) {
        std::string l_sName = "point_" + std::to_string(i);
        m_vPoints.push_back(a_cAttr.getAttributeAsVector3d(l_sName.c_str()));
      }

      setEndNormal(getNormalOfPoint(l_iCount - 1));
    }

    /**
    * Get the point of the bezier curve
    * @param a_vPoints vector with the reference points
    * @param a_fStep step number (0..1)
    * @return the next point
    */
    irr::core::vector3df ITrackPart_Bezier::getBezier(std::vector<irr::core::vector3df> &a_vPoints, irr::f32 a_fStep) {
      switch (a_vPoints.size()) {
        case 2: return getBezier1(a_vPoints.data(), a_fStep);
        case 3: return getBezier2(a_vPoints.data(), a_fStep);
        case 4: return getBezier3(a_vPoints.data(), a_fStep);
        default: return m_vPoints.front();
      }
    }

    /**
    * Calculate the points of the curve, road center
    */
    irr::f32 ITrackPart_Bezier::calculateCurve() {
      m_vTrack.clear();

      int l_iSteps = 32;

      if (m_vPoints.size() == 2) {
        l_iSteps = (int)(std::max(1.0f, m_vPoints[0].getDistanceFrom(m_vPoints[1])));
      }

      std::vector<irr::f32> l_vIndices = fillIndexVector(l_iSteps, getPrevious());

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
    void ITrackPart_Bezier::setReferencePoints(const std::vector<irr::core::vector3df> &a_vPoints) {
      m_vPoints.clear();

      m_vPoints.clear();

      for (auto l_cPoint : a_vPoints)
        m_vPoints.push_back(l_cPoint);

      std::vector<irr::scene::SMeshBuffer *> l_vDummy = std::vector<irr::scene::SMeshBuffer *>();
      bool l_bBlack = true;
      createTrackPart(l_vDummy, l_bBlack);

      setEnd(irr::core::line3df(
        m_vPoints.back() - (m_fWidth / 2.0f) * getSideOfPoint((int)m_vPoints.size() - 1),
        m_vPoints.back() + (m_fWidth / 2.0f) * getSideOfPoint((int)m_vPoints.size() - 1)
      ));

      if (getNext() != nullptr && getNext()->getPrevious() != nullptr)
        getNext()->updateReferencePoints();
    }

    /**
    * Get the part's reference points
    * @return a vector with the part's reference points
    */
    const std::vector<irr::core::vector3df> ITrackPart_Bezier::getReferencePoints() {
      return m_vPoints;
    }

    /**
    * Get the direction of the end of this track part
    */
    const irr::core::vector3df ITrackPart_Bezier::getEndDirection() {
      return (m_vPoints[m_vPoints.size() - 1] - m_vPoints[m_vPoints.size() - 2]).normalize();
    }

    void ITrackPart_Bezier::drawReferencePoint(irr::video::IVideoDriver *a_pDrv) {
      irr::core::vector3df l_cNormal = getPrevious() != nullptr ? getPrevious()->getEndNormal() : irr::core::vector3df(0.0f, 1.0f, 0.0f);

      if (m_vPoints.size() > 1) {
        for (size_t i = 0; i < m_vPoints.size() - 1; i++) {
          a_pDrv->draw3DLine(50.0f * m_vPoints[i], 50.0f * m_vPoints[i + 1], irr::video::SColor(0xFF, 0xFF, 0, 0));
        }

        for (size_t i = 0; i < m_vPoints.size(); i++) {
          a_pDrv->draw3DLine(50.0f * m_vPoints[i], 50.0f * m_vPoints[i] + 25.0f * getNormalOfPoint((int)i), irr::video::SColor(0xFF, 0, 0xFF, 0));
          a_pDrv->draw3DLine(50.0f * m_vPoints[i], 50.0f * m_vPoints[i] + 25.0f * getSideOfPoint  ((int)i), irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF));
        }
      }
    }
  }
}