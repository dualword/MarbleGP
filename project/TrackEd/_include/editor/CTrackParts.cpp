// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#include <editor/CTrackParts.h>
#include <bezier/bezier.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace tracked {

    int getIndexOfVertex(const irr::core::vector3df &a_cPosition, irr::scene::SMeshBuffer *a_pBuffer) {
      for (irr::u32 i = 0; i < a_pBuffer->Vertices.size(); i++) {
        if (a_pBuffer->Vertices[i].Pos == a_cPosition)
          return i;
      }

      return -1;
    }

    void addQuadToMeshBuffer(const irr::core::vector3df *a_aPositions, const irr::core::vector3df *a_aNormals, irr::scene::SMeshBuffer *a_pBuffer, irr::core::vector2df &a_cTexture, int a_iBufferIdx, irr::f32 a_fTextureOffset) {
      int l_aIdx[] = {
        getIndexOfVertex(a_aPositions[0], a_pBuffer),
        getIndexOfVertex(a_aPositions[1], a_pBuffer),
        getIndexOfVertex(a_aPositions[2], a_pBuffer),
        getIndexOfVertex(a_aPositions[3], a_pBuffer)
      };

      for (int i = 0; i < 4; i++) {
        irr::video::SColor   l_cColor   = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
        irr::core::vector3df l_cPos     = a_aPositions[i];
        irr::core::vector3df l_cNormal  = a_aNormals  [i];
        
        irr::core::vector2df l_cTexture;

        if (a_iBufferIdx == c_iBufferIdxBlack || a_iBufferIdx == c_iBufferIdxYellow) {
          l_cTexture = irr::core::vector2df(i == 0 || i == 1 ? 0.0f : 1.0f, i == 0 || i == 2 ? 0.0f : 1.0f);
        }
        else if (a_iBufferIdx == c_iBufferIdxBottom) {
          l_cTexture = irr::core::vector2df(l_cPos.X, l_cPos.Z);
        }
        else {
          l_cTexture = irr::core::vector2df(i == 0 || i == 2 ? 0.0f : 1.0f, i == 0 || i == 1 ? a_cTexture.Y : a_cTexture.Y + a_fTextureOffset);
        }

        if (l_aIdx[i] == -1) {
          l_aIdx[i] = a_pBuffer->Vertices.size();
          a_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cPos, l_cNormal, l_cColor, l_cTexture));
        }
      }

      if (a_iBufferIdx == c_iBufferIdxRoad || a_iBufferIdx == c_iBufferIdxArrow || a_iBufferIdx == c_iBufferIdxCross || a_iBufferIdx == c_iBufferIdxJoin)
        a_cTexture.Y += a_fTextureOffset;

      a_pBuffer->Indices.push_back(l_aIdx[0]);
      a_pBuffer->Indices.push_back(l_aIdx[1]);
      a_pBuffer->Indices.push_back(l_aIdx[2]);

      a_pBuffer->Indices.push_back(l_aIdx[1]);
      a_pBuffer->Indices.push_back(l_aIdx[3]);
      a_pBuffer->Indices.push_back(l_aIdx[2]);
    }

    CTrackPart_Bezier::CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cPosition,irr::scene::ISceneCollisionManager *a_pColMgr) :
      ITrackPart (),
      m_pColMgr  (a_pColMgr),
      m_pPrevious(a_pPrevious),
      m_pNext    (nullptr)
    {
    }

    CTrackPart_Bezier::~CTrackPart_Bezier() {
    }

    /**
    * Set the next track part
    * @param a_pNext the next track part
    */
    void CTrackPart_Bezier::setNext(ITrackPart *a_pNext) {
      m_pNext = a_pNext;
    }

    /**
    * Get the next track part
    * @return the next track part
    */
    ITrackPart *CTrackPart_Bezier::getNext() {
      return m_pNext;
    }

    /**
    * Create the track part
    * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
    */
    void CTrackPart_Bezier::createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers) {
      std::vector<irr::core::vector3df> l_vCurve;
      double l_fLength = calculateCurve(l_vCurve);

      bool l_bBlackSide = true;
      int l_iSteps = 32;

      irr::core::vector2df l_cTexture;
      irr::f32 l_fTextureOffset = ((irr::f32)l_fLength / (irr::f32)l_iSteps);
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

      for (int i = m_pPrevious == nullptr ? 0 : 1; i < a_iSteps; i++) {
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

    void CTrackPart_Bezier::setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) {
      m_eType = a_cPoints.size() == 2 ? enType::Bezier1 : a_cPoints.size() == 3 ? enType::Bezier2 : enType::Bezier3;

      switch (m_eType) {
        case enType::Bezier1: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); break;
        case enType::Bezier2: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); m_vPoints.push_back(a_cPoints[2]); break;
        case enType::Bezier3: m_vPoints.push_back(a_cPoints[0]); m_vPoints.push_back(a_cPoints[1]); m_vPoints.push_back(a_cPoints[2]); m_vPoints.push_back(a_cPoints[3]); break;
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

      irr::core::vector3df l_cOld = m_pPrevious != nullptr ? m_pPrevious->getEndPosition() : m_vPoints.front();
      irr::core::vector3df l_cSideOld = ((m_pPrevious != nullptr ?  m_pPrevious->getEndPosition() : m_vPoints.front()).crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f))).normalize();

      irr::video::SColor l_cColor = m_pPrevious == nullptr ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0);
      
      for (auto l_cPos: l_vCurve) {
        if (m_pPrevious != nullptr || l_cPos != l_vCurve.front()) {
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
    * Handle an event if this part is selected
    * @param a_cEvent the event to handle
    * @return true if the event was handled
    */
    bool CTrackPart_Bezier::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;


      return l_bRet;
    }
  }
}