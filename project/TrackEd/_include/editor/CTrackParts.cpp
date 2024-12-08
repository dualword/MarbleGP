// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackParts.h>
#include <bezier/bezier.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {


    CTrackPart_Bezier::CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr) :
      ITrackPart (a_pPrevious, a_cNormal, a_pColMgr)
    {
    }

    CTrackPart_Bezier::~CTrackPart_Bezier() {
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Bezier::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      std::vector<irr::core::vector3df> l_vCurve;
      double l_fLength = calculateCurve(l_vCurve);

      l_fLength = std::ceil(l_fLength);

      size_t l_iSteps = l_vCurve.size();

      if (getPrevious() == nullptr)
        l_iSteps--;

      double l_fTextureStep = l_fLength / (double)(l_iSteps);

      irr::f32 l_fWidth = 0.25f;

      irr::core::vector3df l_cOld     = m_vPoints[0];
      irr::core::vector3df l_cSideOld = -(m_vPoints[1] - m_vPoints[0]).crossProduct(getStartNormal()).normalize();

      irr::video::SColor l_cColor = getPrevious() == nullptr ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);
      
      irr::core::position2df l_aTextureTop[] = {
        irr::core::position2df(1.0f, 0.0f),
        irr::core::position2df(0.0f, 0.0f),
        irr::core::position2df(0.0f, (irr::f32)l_fTextureStep),
        irr::core::position2df(1.0f, (irr::f32)l_fTextureStep)
      };

      for (auto l_cPos: l_vCurve) {
        if (getPrevious() != nullptr || l_cPos != l_vCurve.front()) {
          irr::core::vector3df l_cSideNew = (l_cOld - l_cPos).crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f)).normalize();

          irr::core::vector3df l_aVertices[] = {
            (l_cOld + l_fWidth * l_cSideOld) + 0.05f * getNormal(),
            (l_cOld - l_fWidth * l_cSideOld) + 0.05f * getNormal(),
            (l_cPos - l_fWidth * l_cSideNew) + 0.05f * getNormal(),
            (l_cPos + l_fWidth * l_cSideNew) + 0.05f * getNormal()
          };

          if (l_cPos == l_vCurve.back()) {
            l_aTextureTop[2].Y = std::ceil(l_aTextureTop[2].Y);
            l_aTextureTop[3].Y = std::ceil(l_aTextureTop[3].Y);
          }

          addQuadToMeshBuffer(l_aVertices, l_aTextureTop, a_vBuffers[c_iBufferIdxRoad], l_cPos == l_vCurve.front());

          for (int i = 0; i < 4; i++)
            l_aTextureTop[i].Y += (irr::f32)l_fTextureStep;

          l_aVertices[0] = l_cOld - l_fWidth * l_cSideOld;
          l_aVertices[1] = l_cOld + l_fWidth * l_cSideOld;
          l_aVertices[2] = l_cPos + l_fWidth * l_cSideOld;
          l_aVertices[3] = l_cPos - l_fWidth * l_cSideOld;

          irr::core::position2df l_aTextureBottom[] = {
            irr::core::position2df(l_aVertices[0].X, l_aVertices[0].Z),
            irr::core::position2df(l_aVertices[1].X, l_aVertices[1].Z),
            irr::core::position2df(l_aVertices[2].X, l_aVertices[2].Z),
            irr::core::position2df(l_aVertices[3].X, l_aVertices[3].Z)
          };

          addQuadToMeshBuffer(l_aVertices, l_aTextureBottom, a_vBuffers[c_iBufferIdxBottom], false);

          l_aVertices[0] = l_cOld + l_fWidth * l_cSideOld;
          l_aVertices[1] = l_cOld + l_fWidth * l_cSideOld + 0.05f * getNormal();
          l_aVertices[2] = l_cPos + l_fWidth * l_cSideNew + 0.05f * getNormal();
          l_aVertices[3] = l_cPos + l_fWidth * l_cSideNew;

          irr::core::position2df l_aTextureSide[] = {
            irr::core::position2df(0.0f, 0.0f),
            irr::core::position2df(1.0f, 0.0f),
            irr::core::position2df(0.0f, 1.0f),
            irr::core::position2df(1.0f, 1.0f)
          };

          addQuadToMeshBuffer(l_aVertices, l_aTextureSide, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false);

          l_aVertices[3] = l_cOld - l_fWidth * l_cSideOld;
          l_aVertices[2] = l_cOld - l_fWidth * l_cSideOld + 0.05f * getNormal();
          l_aVertices[1] = l_cPos - l_fWidth * l_cSideNew + 0.05f * getNormal();
          l_aVertices[0] = l_cPos - l_fWidth * l_cSideNew;

          addQuadToMeshBuffer(l_aVertices, l_aTextureSide, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false);

          l_cSideOld = l_cSideNew;
          a_bBlack = !a_bBlack;
        }
      
        l_cOld = l_cPos;
      }
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Bezier::serialize(irr::io::IAttributes &a_cAttr) {
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Bezier::deserialize(const irr::io::IAttributes &a_cAttr) {
    }

    std::vector<double> CTrackPart_Bezier::fillIndexVector(int a_iSteps) {
      std::vector<double> l_vRet;

      for (int i = getPrevious() == nullptr ? 0 : 1; i < a_iSteps; i++) {
        l_vRet.push_back((double)i / (double)a_iSteps);
      }

      l_vRet.push_back(1.0);

      return l_vRet;
    }

    /**
    * Calculate the points of the curve, road center
    * @param a_cOut the vector to be filled
    */
    double CTrackPart_Bezier::calculateCurve(std::vector<irr::core::vector3df> &a_cOut) {
      m_vTrack.clear();

      std::vector<bezier::Point> l_vPoints;

      for (auto l_cPoint: m_vPoints) {
        l_vPoints.push_back(bezier::Point(l_cPoint.X, l_cPoint.Z));
      }

      double l_fRet = 0.0;

      switch (m_eType) {
        case enType::Bezier1: {
          bezier::Bezier<1> l_cBezier(l_vPoints);

          l_fRet = l_cBezier.length();
          int l_iSteps = (int)(l_fRet / 0.5);

          if (l_iSteps % 2 != 0) l_iSteps++;

          std::vector<double> l_vIndices = fillIndexVector(l_iSteps);

          for (auto d: l_vIndices) {
            a_cOut.push_back(irr::core::vector3df((irr::f32)l_cBezier.valueAt(d, 0), 0.0f, (irr::f32)l_cBezier.valueAt(d, 1)));
            m_vTrack.push_back(a_cOut.back());
          }

          break;
        }

        case enType::Bezier2: {
          bezier::Bezier<2> l_cBezier(l_vPoints);

          int l_iSteps = 32;

          std::vector<double> l_vIndices = fillIndexVector(l_iSteps);

          for (auto d: l_vIndices) {
            a_cOut.push_back(irr::core::vector3df((irr::f32)l_cBezier.valueAt(d, 0), 0.0f, (irr::f32)l_cBezier.valueAt(d, 1)));
            m_vTrack.push_back(a_cOut.back());
          }

          l_fRet = l_cBezier.length();
          break;
        }

        case enType::Bezier3: {
          bezier::Bezier<3> l_cBezier(l_vPoints);

          int l_iSteps = 32;

          std::vector<double> l_vIndices = fillIndexVector(l_iSteps);

          for (auto d: l_vIndices) {
            a_cOut.push_back(irr::core::vector3df((irr::f32)l_cBezier.valueAt(d, 0), 0.0f, (irr::f32)l_cBezier.valueAt(d, 1)));
            m_vTrack.push_back(a_cOut.back());
          }

          l_fRet = l_cBezier.length();
          break;
        }
      }


      return l_fRet;
    }

    /**
    * Set the trak part's reference points
    * @param a_cPoints a vector with the reference points
    */
    void CTrackPart_Bezier::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      m_vPoints.clear();

      m_eType = a_cPoints.size() == 2 ? enType::Bezier1 : a_cPoints.size() == 3 ? enType::Bezier2 : enType::Bezier3;

      switch (m_eType) {
        case enType::Bezier1: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); break;
        case enType::Bezier2: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); m_vPoints.push_back(a_cPoints[2]); break;
        case enType::Bezier3: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); m_vPoints.push_back(a_cPoints[2]); m_vPoints.push_back(a_cPoints[3]); break;
      }
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
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in. Only used if "true" is returned.
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

        return true;
      }
      else {
        a_cLine.start = irr::core::vector3df(0.0f);
        a_cLine.end   = a_cLine.start;

        return false;
      }
    }

    /**
    * Get the end position of this track part
    * @return the end position of this track part
    */
    const irr::core::vector3df &CTrackPart_Bezier::getEndPosition() {
      return m_vPoints.back();
    }

    /**
    * Get the next line defining the road to go (one side of the road to the other)
    * @param a_iIndex the index of the lines
    * @param a_cPos1 [out] the first position of the line
    * @param a_cPos2 [out] the second position of the line
    * @return true if a section line was found, false otherwise
    */
    bool CTrackPart_Bezier::getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) {
      if (m_vTrack.size() == 0) {
        std::vector<irr::core::vector3df> l_vCurve;
        calculateCurve(l_vCurve);
      }

      if (a_iIndex < (irr::u32)m_vTrack.size()) {
        irr::core::vector3df l_cDirection = (a_iIndex > 0 ? m_vTrack[a_iIndex - 1] - m_vTrack[a_iIndex] : m_vPoints[0] - m_vPoints[1]).normalize();
        irr::core::vector3df l_cNormal    = irr::core::vector3df(0.0f, 1.0f, 0.0f);
        irr::core::vector3df l_cSide      = l_cDirection.crossProduct(l_cNormal).normalize();

        a_cPos1 = m_vTrack[a_iIndex] + 0.25f * l_cSide;
        a_cPos2 = m_vTrack[a_iIndex] - 0.25f * l_cSide;

        return true;
      }
      else return false;
    }

    /**
    * Draw a preview if necessary
    * @param a_pDrv the Irrlicht video driver
    */
    void CTrackPart_Bezier::drawPreview(irr::video::IVideoDriver *a_pDrv) {
      std::vector<irr::core::vector3df> l_vCurve;
      calculateCurve(l_vCurve);

      irr::core::vector3df l_cOld = getPrevious() != nullptr ? getPrevious()->getEndPosition() : m_vPoints.front();
      irr::core::vector3df l_cSideOld = ((getPrevious() != nullptr ?  getPrevious()->getEndPosition() : m_vPoints.front()).crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f))).normalize();

      irr::video::SColor l_cColor = getPrevious() == nullptr ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);
      
      for (auto l_cPos: l_vCurve) {
        if (getPrevious() != nullptr || l_cPos != l_vCurve.front()) {
          irr::core::vector3df l_cSideNew = (l_cOld - l_cPos).crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f)).normalize();

          irr::core::triangle3df l_cTriangle = irr::core::triangle3df(
            50.0f * (l_cPos - 0.25f * l_cSideNew) + irr::core::vector3df(0.0f, 3.0f, 0.0f),
            50.0f * (l_cPos + 0.25f * l_cSideNew) + irr::core::vector3df(0.0f, 3.0f, 0.0f),
            50.0f * (l_cOld + 0.25f * l_cSideOld) + irr::core::vector3df(0.0f, 3.0f, 0.0f)
          );

          a_pDrv->draw3DTriangle(l_cTriangle, l_cColor);

          l_cTriangle = irr::core::triangle3df(
            50.0f * (l_cOld + 0.25f * l_cSideOld) + irr::core::vector3df(0.0f, 3.0f, 0.0f),
            50.0f * (l_cOld - 0.25f * l_cSideOld) + irr::core::vector3df(0.0f, 3.0f, 0.0f),
            50.0f * (l_cPos - 0.25f * l_cSideNew) + irr::core::vector3df(0.0f, 3.0f, 0.0f)
          );

          a_pDrv->draw3DTriangle(l_cTriangle, l_cColor);

          l_cSideOld = l_cSideNew;
        }
      
        l_cOld = l_cPos;
      }
    }

    /**
    * Get the direction of the end of this track part
    */
    const irr::core::vector3df CTrackPart_Bezier::getEndDirection() {
      irr::core::vector3df l_cRet;

      switch (m_eType) {
        case enType::Bezier1: l_cRet = (m_vPoints[1] - m_vPoints[0]).normalize(); break;
        default: {
          l_cRet = (m_vPoints[m_vPoints.size() - 1] - m_vPoints[m_vPoints.size() - 2]).normalize();
        }
      }

      return l_cRet;
    }

    /**
    * Set the selection flag of this road part
    * @param a_bSelected the new value
    */
    void CTrackPart_Bezier::setSelected(bool a_bSelected) {
      ITrackPart::setSelected(a_bSelected);
    }

    /**
    * Get the normal at the end point
    * @return the normal at the end point
    */
    const irr::core::vector3df &CTrackPart_Bezier::getNormal() {
      return getStartNormal();
    }

    /**
    * Handle an event if this part is selected
    * @param a_cEvent the event to handle
    * @return true if the event was handled
    */
    bool CTrackPart_Bezier::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;


      return l_bRet;
    }



    CTrackPart_Checkpoint::CTrackPart_Checkpoint(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr) : 
      ITrackPart    (a_pPrevious, a_cNormal, a_pColMgr),
      m_pCheckpoint (nullptr),
      m_bRespawnLeft(true)
    {
    }

    CTrackPart_Checkpoint::~CTrackPart_Checkpoint() {
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Checkpoint::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) {
      irr::core::vector3df l_cSide = (m_cStart - m_cEnd).crossProduct(getNormal()).normalize();

      irr::core::vector3df l_aRespawn[] = {
        m_bRespawnLeft ? m_cStart - 0.25f * l_cSide : m_cStart + 0.75f * l_cSide,
        m_bRespawnLeft ? m_cStart - 0.75f * l_cSide : m_cStart + 0.25f * l_cSide,
        m_bRespawnLeft ? m_cEnd   - 0.75f * l_cSide : m_cEnd   + 0.25f * l_cSide,
        m_bRespawnLeft ? m_cEnd   - 0.25f * l_cSide : m_cEnd   + 0.75f * l_cSide
      };

      irr::core::vector3df l_aVertices[] = {
        m_cStart + 0.25f * l_cSide + 0.05f * getNormal(),
        m_cStart - 0.25f * l_cSide + 0.05f * getNormal(),
        m_cEnd   - 0.25f * l_cSide + 0.05f * getNormal(),
        m_cEnd   + 0.25f * l_cSide + 0.05f * getNormal()
      };

      irr::core::position2df l_aTexture[] = {
        m_bRespawnLeft ? irr::core::position2df(0.0f, 0.0f) : irr::core::position2df(1.0f, 0.0f),
        m_bRespawnLeft ? irr::core::position2df(1.0f, 0.0f) : irr::core::position2df(0.0f, 0.0f),
        m_bRespawnLeft ? irr::core::position2df(1.0f, 1.0f) : irr::core::position2df(0.0f, 1.0f),
        m_bRespawnLeft ? irr::core::position2df(0.0f, 1.0f) : irr::core::position2df(1.0f, 1.0f)
      };

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxJoin], true);

      l_aVertices[0] = l_aRespawn[0] + 0.05f * getNormal();
      l_aVertices[1] = l_aRespawn[1] + 0.05f * getNormal();
      l_aVertices[2] = l_aRespawn[2] + 0.05f * getNormal();
      l_aVertices[3] = l_aRespawn[3] + 0.05f * getNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 0.0f; l_aTexture[1].Y = 1.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 1.0f; l_aTexture[3].Y = 0.0f;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxRoad], true);

      l_aVertices[1] = l_aRespawn[0];
      l_aVertices[0] = l_aRespawn[1];
      l_aVertices[3] = l_aRespawn[2];
      l_aVertices[2] = l_aRespawn[3];

      l_aTexture[0].X = l_aVertices[0].X; l_aTexture[0].Y = l_aVertices[0].Z;
      l_aTexture[1].X = l_aVertices[1].X; l_aTexture[1].Y = l_aVertices[1].Z;
      l_aTexture[2].X = l_aVertices[2].X; l_aTexture[2].Y = l_aVertices[2].Z;
      l_aTexture[3].X = l_aVertices[3].X; l_aTexture[3].Y = l_aVertices[3].Z;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxBottom], false);

      l_aVertices[0] = m_cStart - 0.25f * l_cSide;
      l_aVertices[1] = m_cStart + 0.25f * l_cSide;
      l_aVertices[2] = m_cEnd   + 0.25f * l_cSide;
      l_aVertices[3] = m_cEnd   - 0.25f * l_cSide;

      l_aTexture[0].X = l_aVertices[0].X; l_aTexture[0].Y = l_aVertices[0].Z;
      l_aTexture[1].X = l_aVertices[1].X; l_aTexture[1].Y = l_aVertices[1].Z;
      l_aTexture[2].X = l_aVertices[2].X; l_aTexture[2].Y = l_aVertices[2].Z;
      l_aTexture[3].X = l_aVertices[3].X; l_aTexture[3].Y = l_aVertices[3].Z;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[c_iBufferIdxBottom], false);

      l_aVertices[0] = m_bRespawnLeft ? m_cEnd   + 0.25f * l_cSide                       : m_cStart - 0.25f * l_cSide;
      l_aVertices[1] = m_bRespawnLeft ? m_cStart + 0.25f * l_cSide                       : m_cEnd   - 0.25f * l_cSide;
      l_aVertices[2] = m_bRespawnLeft ? m_cStart + 0.25f * l_cSide + 0.05f * getNormal() : m_cEnd   - 0.25f * l_cSide + 0.05f * getNormal();
      l_aVertices[3] = m_bRespawnLeft ? m_cEnd   + 0.25f * l_cSide + 0.05f * getNormal() : m_cStart - 0.25f * l_cSide + 0.05f * getNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false);

      l_aVertices[0] = m_bRespawnLeft ? l_aRespawn[1]                       : l_aRespawn[3];
      l_aVertices[1] = m_bRespawnLeft ? l_aRespawn[2]                       : l_aRespawn[0];
      l_aVertices[3] = m_bRespawnLeft ? l_aRespawn[1] + 0.05f * getNormal() : l_aRespawn[3] + 0.05f * getNormal();
      l_aVertices[2] = m_bRespawnLeft ? l_aRespawn[2] + 0.05f * getNormal() : l_aRespawn[0] + 0.05f * getNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxYellow : c_iBufferIdxBlack], false);

      l_aVertices[0] = l_aRespawn[0];
      l_aVertices[1] = l_aRespawn[1];
      l_aVertices[2] = l_aRespawn[1] + 0.05f * getNormal();
      l_aVertices[3] = l_aRespawn[0] + 0.05f * getNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false);

      l_aVertices[1] = l_aRespawn[3];
      l_aVertices[0] = l_aRespawn[2];
      l_aVertices[3] = l_aRespawn[2] + 0.05f * getNormal();
      l_aVertices[2] = l_aRespawn[3] + 0.05f * getNormal();

      l_aTexture[0].X = 0.0f; l_aTexture[0].Y = 0.0f;
      l_aTexture[1].X = 1.0f; l_aTexture[1].Y = 0.0f;
      l_aTexture[2].X = 1.0f; l_aTexture[2].Y = 1.0f;
      l_aTexture[3].X = 0.0f; l_aTexture[3].Y = 1.0f;

      addQuadToMeshBuffer(l_aVertices, l_aTexture, a_vBuffers[a_bBlack ? c_iBufferIdxBlack : c_iBufferIdxYellow], false);

      tracked::IGlobal *l_pGlobal = tracked::IGlobal::getInstance();

      if (m_pCheckpoint == nullptr) {
        m_pCheckpoint = l_pGlobal->getSceneManager()->addMeshSceneNode(
          l_pGlobal->getSceneManager()->getMesh("data/objects/checkpoint.dae"),
          l_pGlobal->getSceneManager()->getRootSceneNode(),
          -1,
          50.0f * m_cStart + 2.5f * getNormal(),
          (m_cEnd - m_cStart).getHorizontalAngle(),
          irr::core::vector3df(50.0f * 0.5f)
        );

        m_pCheckpoint->getMaterial(0).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/checkpoint_white.png"));
        m_pCheckpoint->getMaterial(1).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/warning.png"));
        m_pCheckpoint->getMaterial(2).setTexture(0, l_pGlobal->getVideoDriver()->getTexture("data/textures/warning.png"));

        m_pCheckpoint->getMaterial(0).Lighting = false;
      }
      else {
        m_pCheckpoint->setPosition(50.0f * m_cStart + 2.5f * getNormal());
        m_pCheckpoint->setRotation((m_cEnd - m_cStart).getHorizontalAngle());
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

        irr::core::vector3df l_cSide   = (m_cStart - m_cEnd).crossProduct(getNormal()).normalize();
        irr::core::vector3df l_cCenter = (m_cStart + m_cEnd) / 2.0f;

        irr::core::plane3df l_cPlane = irr::core::plane3df(l_cCenter, l_cSide);

        m_bRespawnLeft = l_cPlane.classifyPointRelation(a_cPoints[1]) == irr::core::ISREL3D_BACK;

        // m_bRespawnLeft = l_cPlane.isFrontFacing(a_cPoints[1]);

        printf("Respawn side: %s\n", m_bRespawnLeft ? "Left" : "Right");
      }
    }

    /**
    * Serialize the track part
    * @param a_cAttr the attributes to serialize the data to
    */
    void CTrackPart_Checkpoint::serialize(irr::io::IAttributes &a_cAttr) {
    }

    /**
    * De-serialize the track part
    * @param a_cAttr the attributes to read the data from
    */
    void CTrackPart_Checkpoint::deserialize(const irr::io::IAttributes &a_cAttr) {
    }

    /**
    * Get the end position of this track part
    * @return the end position of this track part
    */
    const irr::core::vector3df &CTrackPart_Checkpoint::getEndPosition() {
      return m_cEnd;
    }

    /**
    * Get the direction of the end of this track part
    */
    const irr::core::vector3df CTrackPart_Checkpoint::getEndDirection() {
      return (m_cEnd - m_cStart).normalize();
    }

    /**
    * Get the next line defining the road to go (one side of the road to the other)
    * @param a_iIndex the index of the lines
    * @param a_cPos1 [out] the first position of the line
    * @param a_cPos2 [out] the second position of the line
    * @return true if a section line was found, false otherwise
    */
    bool CTrackPart_Checkpoint::getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) {
      return false;
    }
      
    /**
    * Get the normal at the end point
    * @return the normal at the end point
    */
    const irr::core::vector3df &CTrackPart_Checkpoint::getNormal() {
      return getStartNormal();
    }

    /**
    * Can a reference point be moved freely in the 2d plane
    * @param a_iIndex index of the reference point
    * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
    * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
    * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in. Only used if "true" is returned.
    * @return true if the reference point can move freely, false otherwise
    */
    bool CTrackPart_Checkpoint::canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) {
      if (a_iIndex == 1) {
        irr::core::vector3df l_cCenter = (m_cStart + m_cEnd) / 2.0f;
        irr::core::vector3df l_cSide   = (m_cStart - m_cEnd).crossProduct(getNormal()).normalize();

        a_cLine.start = l_cCenter + l_cSide / 2.0f;
        a_cLine.end   = l_cCenter - l_cSide / 2.0f;
        
        a_cPlanePoint  = l_cCenter;
        a_cPlaneNormal = getNormal();
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

      irr::core::vector3df l_cSide = (m_cStart - m_cEnd).crossProduct(getNormal()).normalize();
      irr::core::vector3df l_cRspn = (m_cStart + m_cEnd) / 2.0f;

      if (m_bRespawnLeft)
        l_cRspn -= 0.5f * l_cSide;
      else
        l_cRspn += 0.5f * l_cSide;

      l_vRet.push_back(m_cStart);
      l_vRet.push_back(l_cRspn),
      l_vRet.push_back(m_cEnd);

      return l_vRet;
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