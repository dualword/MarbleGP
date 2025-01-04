// (w) 2024 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>
#include <scenenodes/CDustbinLight.h>
#include <animators/CPlaneAnimator.h>
#include <editor/CPointMoveManager.h>
#include <messages/CMessageHelpers.h>
#include <scenenodes/CRostrumNode.h>
#include <helpers/CTrackEdHelpers.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdMainMenu.h>
#include <scenenodes/CSpeedNode.h>
#include <editor/ITrackPart.h>
#include <platform/uuid_v4.h>
#include <data/CMemoryFile.h>
#include <editor/CAiPath.h>
#include <CDefines.h>
#include <algorithm>
#include <IGlobal.h>
#include <cmath>

#include <editor/CTrackPart_FlatSegment.h>
#include <editor/CTrackPart_Checkpoint.h>
#include <editor/CTrackPart_Straight.h>
#include <editor/ITrackPart_Bezier.h>
#include <editor/CTrackPart_Jump.h>
#include <editor/CTrackPart_Turn.h>
#include <editor/CTrackPart_Ramp.h>

namespace dustbin {

  namespace commands {
    const irr::u32 c_iIdxRoad    = 0;
    const irr::u32 c_iIdxArrow   = 1;
    const irr::u32 c_iIdxCross   = 2;
    const irr::u32 c_iIdxPlane   = 3;
    const irr::u32 c_iIdxBlack   = 4;
    const irr::u32 c_iIdxYellw   = 5;
    const irr::u32 c_iIdxWarn    = 6;
    const irr::u32 c_iIdxGround  = 7;
    const irr::u32 c_iIdxWallOut = 8;
    const irr::u32 c_iIdxWall    = 9;
    const irr::u32 c_iIdxCount   = 10;

    CCmdEditTrack::CCmdEditTrack(irr::scene::ISceneManager *a_pSmgr, irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui, CCmdMainMenu *a_pMainMenu) :
      ICommand      ("CmdEditTrack"),
      m_eEditMode   (enEditMode::Default),
      m_pSmgr       (a_pSmgr),
      m_pGui        (a_pGui),
      m_pDrv        (a_pDrv),
      m_pWorld      (nullptr),
      m_pHover      (nullptr),
      m_pSelected   (nullptr),
      m_pPickPoint  (nullptr),
      m_pTrack      (nullptr),
      m_pZeroPlane  (nullptr),
      m_pMesh       (nullptr),
      m_pMoveMgr    (nullptr),
      m_pColMgr     (nullptr),
      m_pClick      (nullptr),
      m_pMainMenu   (a_pMainMenu),
      m_bFinishTrack(false),
      m_bFinished   (false),
      m_fBaseOffset (-1.0f),
      m_fBaseHeight (-1.0f),
      m_pAiPath     (nullptr),
      m_sTrackName  ("Flat: TrackEd Track"),
      m_sTrackUUID  ("")
    {
      m_pColMgr  = m_pSmgr->getSceneCollisionManager();
      m_pMoveMgr = new tracked::CPointMoveManager(m_pColMgr);

      ensureUUID();
    }
        
    CCmdEditTrack::~CCmdEditTrack() {
    }

    /**
    * Update the reference points after the track was modified
    */
    void CCmdEditTrack::updateReferencePoints() {
      for (auto &l_tPoint: m_mReferencePoints) {
        irr::scene::ISceneNode *l_pRefPoint  = l_tPoint.first;
        tracked::ITrackPart    *l_pTrackPart = std::get<0>(l_tPoint.second);
        int                     l_iPointIdx  = std::get<1>(l_tPoint.second);

        std::vector<irr::core::vector3df> l_vRefPoints = l_pTrackPart->getReferencePoints();
        if (l_vRefPoints.size() > l_iPointIdx)
          l_pRefPoint->setPosition(50.0f * (l_vRefPoints[l_iPointIdx]));
      }
    }

    /**
    * Clear the reference points before loading a track
    */
    void CCmdEditTrack::clearReferencePoints() {
      for (auto &l_tPoint : m_mReferencePoints) {
        l_tPoint.first->setVisible(false);
        m_pSmgr->addToDeletionQueue(l_tPoint.first);
      }

      m_mReferencePoints.clear();
    }

    /**
    * Add the editable reference points of a track part
    * @param a_pPart the track part
    */
    void CCmdEditTrack::addReferencePoints(tracked::ITrackPart *a_pPart) {
      int l_iIndex = 0;

      for (auto &l_cPoint: a_pPart->getReferencePoints()) {
        if (l_cPoint != a_pPart->getReferencePoints().front() || a_pPart->getPrevious() == nullptr) {
          bool l_bAdd = true;

          for (auto &l_tPoint : m_mReferencePoints) {
            if (std::get<0>(l_tPoint.second) == a_pPart && std::get<1>(l_tPoint.second) == l_iIndex) {
              printf("Reference point already in.\n");
              l_bAdd = false;
            }
          }

          if (l_bAdd) {
            irr::scene::IMeshSceneNode *l_pNode = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/box.3ds"), m_pSmgr->getRootSceneNode(), -1, 50.0f * l_cPoint, irr::core::vector3df(), irr::core::vector3df(1.0f, 5.0f, 1.0f));
            l_pNode->getMaterial(0).setTexture(0, m_pDrv->getTexture(l_iIndex == 0 ? "data/textures/blue.png" : "data/textures/yellow.png"));

            irr::scene::ITriangleSelector *l_pSelector = m_pSmgr->createTriangleSelector(l_pNode->getMesh(), l_pNode);
            l_pNode->setTriangleSelector(l_pSelector);
            l_pSelector->drop();

            m_mReferencePoints[l_pNode] = std::make_tuple(a_pPart, l_iIndex);
          }
        }
        l_iIndex++;
      }
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdEditTrack::isFinished() {
      return false;
    }

    /**
    * Build the basement of the track (only done if m_fBaseOffset and m_fBaseHeight are greater than zero)
    */
    void CCmdEditTrack::createBasement() {
      if (m_fBaseHeight > 0.0f && m_fBaseOffset > 0.0f) {
        irr::video::SColor l_cCol = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);

        m_vBuffers[tracked::c_iBufferIdxRoad]->recalculateBoundingBox();

        irr::core::aabbox3df l_cBox = m_vBuffers[tracked::c_iBufferIdxRoad]->BoundingBox;

        irr::core::vector3df l_cUpperLeft  = l_cBox.MinEdge;
        irr::core::vector3df l_cLowerRight = l_cBox.MaxEdge;

        l_cUpperLeft .Y = 0.0f;
        l_cLowerRight.Y = 0.0f;

        l_cUpperLeft  -= irr::core::vector3df(m_fBaseOffset, 0.0f, m_fBaseOffset);
        l_cLowerRight += irr::core::vector3df(m_fBaseOffset, 0.0f, m_fBaseOffset);

        irr::u32 l_iIdx[] = { tracked::c_iBufferIdxGround, tracked::c_iBufferIdxBottom, 0xFFFF };

        std::vector<irr::core::vector3df> l_vPos;

        // Make the two ground planes
        for (int i = 0; l_iIdx[i] < 0xFFFF; i++) {
          irr::core::vector2df t = irr::core::vector2df(i == 0 || i == 3 ? 0.0f : 1.0f, i == 0 || i == 1 ? 0.0f : 1.0f);
          irr::core::vector3df l_cNorm = irr::core::vector3df(0.0f, 1.0f, 0.0f);

          irr::u32 l_iIndex = l_iIdx[i];
          irr::scene::SMeshBuffer *l_pBuffer = m_vBuffers[l_iIndex];

          l_vPos.push_back(                     l_cUpperLeft                                     ); l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_vPos.back(), l_cNorm, l_cCol, t));
          l_vPos.push_back(irr::core::vector3df(l_cLowerRight.X, l_cUpperLeft.Y, l_cUpperLeft .Z)); l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_vPos.back(), l_cNorm, l_cCol, t));
          l_vPos.push_back(                     l_cLowerRight                                    ); l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_vPos.back(), l_cNorm, l_cCol, t));
          l_vPos.push_back(irr::core::vector3df(l_cUpperLeft .X, l_cUpperLeft.Y, l_cLowerRight.Z)); l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_vPos.back(), l_cNorm, l_cCol, t));

          l_pBuffer->Indices.push_back(i == 0 ? 1 : l_pBuffer->Vertices.size() - 4);
          l_pBuffer->Indices.push_back(i == 0 ? 0 : l_pBuffer->Vertices.size() - 3);
          l_pBuffer->Indices.push_back(i == 0 ? 2 : l_pBuffer->Vertices.size() - 2);

          l_pBuffer->Indices.push_back(i == 0 ? 2 : l_pBuffer->Vertices.size() - 4);
          l_pBuffer->Indices.push_back(i == 0 ? 0 : l_pBuffer->Vertices.size() - 2);
          l_pBuffer->Indices.push_back(i == 0 ? 3 : l_pBuffer->Vertices.size() - 1);

          l_cUpperLeft  -= irr::core::vector3df(0.1f,  0.5f, 0.1f);
          l_cLowerRight += irr::core::vector3df(0.1f, -0.5f, 0.1f);
        }

        // Make the side walls
        if (l_vPos.size() == 8) {
          std::vector<std::tuple<irr::u32, irr::u32, irr::core::vector3df>> l_vBuffers = {
            std::make_tuple(0, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(1, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(1, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)), 
            std::make_tuple(0, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)),
            
            std::make_tuple(1, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(2, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(2, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)), 
            std::make_tuple(1, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)),
            
            std::make_tuple(2, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(3, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(3, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)), 
            std::make_tuple(2, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)),
            
            std::make_tuple(3, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(0, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(0, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)), 
            std::make_tuple(3, tracked::c_iBufferIdxWallInner, irr::core::vector3df(0.0f, m_fBaseHeight, 0.0f)),

            std::make_tuple(5, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(4, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(4, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            std::make_tuple(5, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)), 
            
            std::make_tuple(6, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(5, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(5, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            std::make_tuple(6, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)), 
            
            std::make_tuple(7, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(6, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(6, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            std::make_tuple(7, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)), 
            
            std::make_tuple(4, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(7, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, 0.0f, 0.0f)), 
            std::make_tuple(7, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            std::make_tuple(4, tracked::c_iBufferIdxWallOuter, irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)) 
          };

          size_t l_iIdx = 0;

          while (l_iIdx < l_vBuffers.size()) {
            irr::scene::SMeshBuffer *l_pBuffer = m_vBuffers[std::get<1>(l_vBuffers[l_iIdx])];

            irr::core::triangle3df l_cTriangle1 = irr::core::triangle3df(
              l_vPos[std::get<0>(l_vBuffers[l_iIdx    ])] + std::get<2>(l_vBuffers[l_iIdx]),
              l_vPos[std::get<0>(l_vBuffers[l_iIdx + 2])] + std::get<2>(l_vBuffers[l_iIdx + 1]),
              l_vPos[std::get<0>(l_vBuffers[l_iIdx + 1])] + std::get<2>(l_vBuffers[l_iIdx + 2])
            );

            irr::core::triangle3df l_cTriangle2 = irr::core::triangle3df(
              l_vPos[std::get<0>(l_vBuffers[l_iIdx    ])] + std::get<2>(l_vBuffers[l_iIdx]),
              l_vPos[std::get<0>(l_vBuffers[l_iIdx + 2])] + std::get<2>(l_vBuffers[l_iIdx + 2]),
              l_vPos[std::get<0>(l_vBuffers[l_iIdx + 3])] + std::get<2>(l_vBuffers[l_iIdx + 3])
            );

            irr::s32 l_iCountY = (irr::s32)(std::ceil((l_cTriangle1.pointB - l_cTriangle1.pointC).getLength()) / 0.5f);
            irr::s32 l_iCountX = (irr::s32)(std::ceil((l_cTriangle1.pointA - l_cTriangle1.pointB).getLength()) / 0.5f);

            irr::core::vector3df l_cNormal = l_cTriangle1.getNormal();

            irr::core::vector2df l_aTex[] = {
              irr::core::vector2df(0.0f               , 0.0f               ),
              irr::core::vector2df(0.0f               , (irr::f32)l_iCountY),
              irr::core::vector2df((irr::f32)l_iCountX, (irr::f32)l_iCountY),
              irr::core::vector2df((irr::f32)l_iCountX, 0.0f               )
            };

            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size());
            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size() + 1);
            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size() + 2);
            
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle1.pointA, l_cNormal, l_cCol, l_aTex[0]));
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle1.pointB, l_cNormal, l_cCol, l_aTex[3]));
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle1.pointC, l_cNormal, l_cCol, l_aTex[2]));

            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size());
            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size() + 1);
            l_pBuffer->Indices.push_back(l_pBuffer->Vertices.size() + 2);
            
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle2.pointA, l_cNormal, l_cCol, l_aTex[0]));
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle2.pointB, l_cNormal, l_cCol, l_aTex[2]));
            l_pBuffer->Vertices.push_back(irr::video::S3DVertex(l_cTriangle2.pointC, l_cNormal, l_cCol, l_aTex[1]));

            l_iIdx += 4;
          }

          // Make the top closure
          std::vector<irr::core::vector3df> l_vTop = {
            irr::core::vector3df(l_vPos[0] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[1] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[5] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            irr::core::vector3df(l_vPos[4] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),

            irr::core::vector3df(l_vPos[1] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[2] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[6] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            irr::core::vector3df(l_vPos[5] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),

            irr::core::vector3df(l_vPos[2] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[3] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[7] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            irr::core::vector3df(l_vPos[6] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),

            irr::core::vector3df(l_vPos[3] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[0] + irr::core::vector3df(0.0f, m_fBaseHeight       , 0.0f)),
            irr::core::vector3df(l_vPos[4] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f)),
            irr::core::vector3df(l_vPos[7] + irr::core::vector3df(0.0f, m_fBaseHeight + 0.5f, 0.0f))

          };

          l_iIdx = 0;

          while (l_iIdx < l_vTop.size()) {
            irr::core::vector3df l_cNormal = irr::core::vector3df(0.0f, 1.0f, 0.0f);

            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size());
            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size() + 1);
            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size() + 2);

            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size());
            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size() + 2);
            m_vBuffers[tracked::c_iBufferIdxBottom]->Indices.push_back(m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.size() + 3);

            for (int i = 0; i < 4; i++) {
              irr::core::vector2df l_cTexture = irr::core::vector2df(l_vTop[l_iIdx + i].X, l_vTop[l_iIdx + i].Z);
              m_vBuffers[tracked::c_iBufferIdxBottom]->Vertices.push_back(irr::video::S3DVertex(l_vTop[l_iIdx + i], l_cNormal, l_cCol, l_cTexture));
            }
            l_iIdx += 4;
          }
        }

        if (m_pZeroPlane != nullptr)
          m_pZeroPlane->setVisible(false);
      }
    }

    /**
    * (Re) create the track
    */
    void CCmdEditTrack::createTrack() {
      if (m_vParts.size() > 0)
        m_vParts.front()->updateReferencePoints();
      else
        return;

      if (m_pAiPath != nullptr) {
        delete m_pAiPath;
        m_pAiPath = nullptr;
      }

      m_pAiPath = new tracked::CAiPath(m_vParts.front());

      // We need some mesh buffers
      while (m_vBuffers.size() < tracked::c_iBufferCount)
        m_vBuffers.push_back(new irr::scene::SMeshBuffer());

      for (auto l_pBuffer : m_vBuffers) {
        l_pBuffer->Vertices.clear();
        l_pBuffer->Indices .clear();
      }

      m_vBuffers[tracked::c_iBufferIdxRoad]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/road_back.png"));
      m_vBuffers[tracked::c_iBufferIdxRoad]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_256.png"));
      m_vBuffers[tracked::c_iBufferIdxRoad]->Material.setTexture(2, m_pSmgr->getVideoDriver()->getTexture("data/textures/road_trans.png"));

      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/road_back.png"));
      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_256.png"));
      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(2, m_pSmgr->getVideoDriver()->getTexture("data/textures/join_trans.png"));

      m_vBuffers[tracked::c_iBufferIdxArrow]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/road_back.png"));
      m_vBuffers[tracked::c_iBufferIdxArrow]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_256.png"));
      m_vBuffers[tracked::c_iBufferIdxArrow]->Material.setTexture(2, m_pSmgr->getVideoDriver()->getTexture("data/textures/arrow_trans.png"));

      m_vBuffers[tracked::c_iBufferIdxBottom]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

      m_vBuffers[tracked::c_iBufferIdxYellow]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/yellow.png"));
      m_vBuffers[tracked::c_iBufferIdxBlack]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/black.png"));

      m_vBuffers[tracked::c_iBufferIdxGround]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/ground_back.png"));
      m_vBuffers[tracked::c_iBufferIdxGround]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_ground.png"));

      m_vBuffers[tracked::c_iBufferIdxWallInner]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/wall_outer.png"));

      m_vBuffers[tracked::c_iBufferIdxWallOuter]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/wall.png"));

      if (m_pMesh == nullptr) {
        m_pMesh = new irr::scene::SMesh();
      }

      m_pMesh->MeshBuffers.clear();

      bool l_bBlack = true;

      printf("**** Create Track\n");
      for (auto l_pPart : m_vParts) {
        l_pPart->createTrackPart(m_vBuffers, l_bBlack);
      }

      createBasement();

      m_mBufferMap.clear();

      int l_iCount = 0;
      for (int i = 0; i < tracked::c_iBufferCount; i++) {
        irr::scene::SMeshBuffer *l_pBuffer = m_vBuffers[i];

        if (l_pBuffer->Vertices.size() > 0 && l_pBuffer->Indices.size() > 0) {
          l_pBuffer->recalculateBoundingBox();
          m_mBufferMap[i] = m_pMesh->getMeshBufferCount();
          m_pMesh->addMeshBuffer(l_pBuffer);
        }
        l_iCount++;
      }

      m_pMesh->recalculateBoundingBox();
      m_pMesh->setDirty();

      if (m_pTrack == nullptr) {
        m_pTrack = m_pSmgr->addMeshSceneNode(m_pMesh, m_pWorld, -1, irr::core::vector3df(0.0f), irr::core::vector3df(0.0f), irr::core::vector3df(50.0f));
        m_pTrack->setName("GeneratedTrack");
      }
      else {
        m_pTrack->setMesh(m_pMesh);
        m_pTrack->getMesh()->setDirty();
      }

      m_pAiPath->addSpeedIndicators(m_pWorld, m_pSmgr);

      irr::SEvent l_cEvent{};
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iUpdateShader;
      l_cEvent.UserEvent.UserData2 = c_iUpdateShader;
      tracked::IGlobal::getInstance()->getDevice()->postEventFromUser(l_cEvent);
    }

    /**
    * Let this command draw something (if necessary)
    * @param a_pDrv the video driver
    */
    void CCmdEditTrack::draw(irr::video::IVideoDriver *a_pDrv) {
      if (m_pAiPath != nullptr)
        m_pAiPath->drawDebugData(a_pDrv, nullptr);

      for (auto l_cPart: m_vParts) {
        l_cPart->drawReferencePoint(a_pDrv);
      }

      if (m_pMoveMgr != nullptr && m_pMoveMgr->getMode() == tracked::CPointMoveManager::enMoveMode::Line) {
        a_pDrv->draw3DLine(m_pMoveMgr->getMoveLine().start, m_pMoveMgr->getMoveLine().end, irr::video::SColor(0xFF, 0, 0, 0));
      }
      else if (m_pMoveMgr != nullptr && m_pMoveMgr->getMode() == tracked::CPointMoveManager::enMoveMode::Plane) {
        if (m_pSelected != nullptr) {
          irr::core::vector3df l_cPoint;
          irr::core::vector3df l_cNormal;

          m_pMoveMgr->getMovePlane(l_cPoint, l_cNormal);

          helpers::drawPlane3df(a_pDrv, l_cPoint, l_cNormal);
        }
      }
    }

    /**
    * Set the edit mode and update the info text
    * @param a_eEditMode the new edit mode
    */
    void CCmdEditTrack::setEditMode(enEditMode a_eMode) {
      m_eEditMode = a_eMode;

      if (m_pMainMenu != nullptr) {
        switch (m_eEditMode) {
          case enEditMode::Default          : m_pMainMenu->setEditMode(L"Edit Mode: Default"             ); break;
          case enEditMode::AddReferencePoint: m_pMainMenu->setEditMode(L"Edit Mode: Add Reference Point" ); break;
          case enEditMode::MovePointOnLine  : m_pMainMenu->setEditMode(L"Edit Mode: Move Reference Point"); break;
          case enEditMode::MovePointInPlane : m_pMainMenu->setEditMode(L"Edit Mode: Move Reference Point"); break;
        }
      }
    }

    /**
    * Get the track defining vector
    * @return the track defining vector
    */
    const std::vector<tracked::ITrackPart *> &CCmdEditTrack::getTrack() {
      return m_vParts;
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdEditTrack::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      {
        if (m_mReferencePoints.find(m_pSelected) != m_mReferencePoints.end()) {
          tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pSelected]);
          l_bRet = l_pPart->OnEvent(a_cEvent);
        }
      }

      if (!l_bRet) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          irr::core::vector2di   l_cMouse = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
          irr::gui::IGUIElement *l_pHover = m_pGui->getRootGUIElement()->getElementFromPoint(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

          if (m_eEditMode == enEditMode::Default || m_eEditMode == enEditMode::AddReferencePoint) {
            irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());

            irr::core::vector3df v;
            irr::core::triangle3df t;

            irr::scene::ISceneNode *l_pObject = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cRay, v, t, 0);

            if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
              m_pHover = nullptr;

              for (auto &l_tPoints : m_mReferencePoints) {
                if (m_pSelected != l_tPoints.first)
                  l_tPoints.first->getMaterial(0).setTexture(0, m_pDrv->getTexture(std::get<1>(l_tPoints.second) == 0 ? "data/textures/blue.png" : "data/textures/yellow.png"));
              }

              if (l_pHover == m_pGui->getRootGUIElement()) {
                if (m_mReferencePoints.find(l_pObject) != m_mReferencePoints.end()) {
                  m_pHover = l_pObject;
                  if (m_pSelected != l_pObject)
                    l_pObject->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/wall_outer_back.png"));
                }
              }

              m_bFinishTrack = false;

              if (m_vParts.size() > 0) {
                irr::core::vector3df l_cDir = m_vParts.back()->getEndDirection();
                irr::core::vector3df l_cEnd = m_vParts.back()->getEnd().getMiddle() * 50.0f;

                if (m_eEditMode == enEditMode::AddReferencePoint) {
                  m_pPickPoint->setPosition(m_pMoveMgr->updateMovement(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y)));
                }
              }
            }
          }
          else if (m_eEditMode == enEditMode::MovePointOnLine || m_eEditMode == enEditMode::MovePointInPlane) {
            if (m_pMoveMgr != nullptr) {
              m_pSelected->setPosition(m_pMoveMgr->updateMovement(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y)));
            }

            l_bRet = true;
          }
          
          if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_cDownPoint[0] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            if (m_eEditMode == enEditMode::Default) {
              if (l_pHover != nullptr && m_mReferencePoints.find(m_pHover) != m_mReferencePoints.end()) {
                irr::core::vector3df l_cPlanePoint;
                irr::core::vector3df l_cPlaneNormal;

                irr::core::line3df l_cLine;

                bool l_bCanMove = std::get<0>(m_mReferencePoints[m_pHover])->canPointMoveFreely(std::get<1>(m_mReferencePoints[m_pHover]), l_cLine, l_cPlanePoint, l_cPlaneNormal);

                if (l_bCanMove) {
                  setEditMode(enEditMode::MovePointInPlane);
                  m_pSelected  = m_pHover;

                  if (m_pMoveMgr == nullptr)
                    m_pMoveMgr = new tracked::CPointMoveManager(m_pColMgr);

                  m_pMoveMgr->setMoveInPlane(m_pSelected->getAbsolutePosition(), irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSelected->getAbsolutePosition(), l_cPlaneNormal);
                }
                else {
                  if (l_cLine.getLength() > 0.0f) {
                    setEditMode(enEditMode::MovePointOnLine);
                    m_pSelected = m_pHover;

                    l_cLine.start = 50.0f * l_cLine.start;
                    l_cLine.end   = 50.0f * l_cLine.end;

                    if (m_pMoveMgr == nullptr)
                      m_pMoveMgr = new tracked::CPointMoveManager(m_pColMgr);

                    m_pMoveMgr->setMoveOnLine(m_pSelected->getAbsolutePosition(), irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), l_cLine, 50.0f * l_cPlanePoint, l_cPlaneNormal);
                  }
                  else {
                    if (m_pMainMenu != nullptr) {
                      m_pMainMenu->addLogMessage(L"This point cannot be moved!", irr::ELL_ERROR, 2000);
                    }
                  }
                }

                l_bRet = true;
              }
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            if (m_eEditMode == enEditMode::Default) {
              if (m_cDownPoint[0].getDistanceFrom(l_cMouse) < 2) {
                if (m_pHover != nullptr) {
                  m_pSelected = m_pHover;
                  m_pSelected->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/green.png"));
                }
              }
            }
            else if (m_eEditMode == enEditMode::AddReferencePoint) {
              if (m_mReferencePoints.find(m_pSelected) != m_mReferencePoints.end()) {
                tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pSelected]);
              
                std::vector<irr::core::vector3df> l_vRefPoints = l_pPart->getReferencePoints();
                l_vRefPoints.push_back(m_pMoveMgr->updateMovement(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y)) / 50.0f);
                l_pPart->setReferencePoints(l_vRefPoints);
                addReferencePoints(l_pPart);
                m_pPickPoint->setVisible(false);
              
                createTrack();
              
                setEditMode(enEditMode::Default);
                m_pMoveMgr->endMovement();
              }
              else printf("Reference point not found!\n");
            }
            else if (m_eEditMode == enEditMode::MovePointInPlane) {
              irr::core::vector3df l_cNewPoint = m_pMoveMgr->updateMovement(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

              std::vector<irr::core::vector3df> l_vRefPoints = std::get<0>(m_mReferencePoints[m_pSelected])->getReferencePoints();
              l_vRefPoints[std::get<1>(m_mReferencePoints[m_pSelected])] = l_cNewPoint / 50.0f;

              std::get<0>(m_mReferencePoints[m_pSelected])->setReferencePoints(l_vRefPoints);

              createTrack();
              setEditMode(enEditMode::Default);
              updateReferencePoints();
              m_pSelected = nullptr;
              m_pMoveMgr->endMovement();

              l_bRet = true;
            }
            else if (m_eEditMode == enEditMode::MovePointOnLine) {
              irr::core::vector3df l_cNewPoint = m_pMoveMgr->updateMovement(irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

              std::vector<irr::core::vector3df> l_vRefPoints = std::get<0>(m_mReferencePoints[m_pSelected])->getReferencePoints();
              l_vRefPoints[std::get<1>(m_mReferencePoints[m_pSelected])] = l_cNewPoint / 50.0f;

              std::get<0>(m_mReferencePoints[m_pSelected])->setReferencePoints(l_vRefPoints);

              createTrack();
              setEditMode(enEditMode::Default);
              updateReferencePoints();
              m_pSelected = nullptr;
              m_pMoveMgr->endMovement();

              l_bRet = true;
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN) {
            m_cDownPoint[1] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP) {
            if (m_cDownPoint[1].getDistanceFrom(l_cMouse) < 2) {
              if (m_pHover != nullptr) {
                if (m_eEditMode == enEditMode::Default) {
                  std::vector<std::wstring> l_vItems;

                  tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pHover]);

                  if (l_pPart->partAdditionAllowed(std::get<1>(m_mReferencePoints[m_pHover]))) {
                    l_vItems.push_back(c_sAddStraight);

                    l_vItems.push_back(c_sAddTurnLeft);
                    l_vItems.push_back(c_sAddTurnRight);

                    l_vItems.push_back(c_sAddCheckpoint );
                    l_vItems.push_back(c_sAddRamp       );
                    l_vItems.push_back(c_sAddJump       );
                    l_vItems.push_back(c_sAddFlatSegment);
                  }

                  if (l_pPart->getPrevious() == nullptr) {
                    l_vItems.push_back(c_sFinishTrack);
                  }

                  if (l_pPart->refPointAdditionAllowed(std::get<1>(m_mReferencePoints[m_pHover]))) {
                    l_vItems.push_back(c_sAddReference);
                  }

                  irr::core::dimension2du l_cSize = irr::core::dimension2du(0, 0);

                  irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();

                  for (auto &l_sItem : l_vItems) {
                    irr::core::dimension2du l_cDim = m_pGui->getSkin()->getFont()->getDimension(l_sItem.c_str());

                    l_cSize.Height += l_cDim.Height;
                    if (l_cDim.Width > l_cSize.Width)
                      l_cSize.Width = l_cDim.Width;
                  }

                  l_cSize.Width  = 6 * l_cSize.Width / 5;
                  l_cSize.Height = 6 * l_cSize.Height / 5;

                  irr::gui::IGUIContextMenu *l_pMenu = m_pGui->addContextMenu(
                    irr::core::recti(
                      irr::core::vector2di(
                        (irr::u32)a_cEvent.MouseInput.X < l_cScreen.Width - l_cSize.Width ? a_cEvent.MouseInput.X : l_cScreen.Width - l_cSize.Width,
                        (irr::u32)a_cEvent.MouseInput.Y < l_cScreen.Height - l_cSize.Height ? a_cEvent.MouseInput.Y : l_cScreen.Height - l_cSize.Height
                      ),
                      irr::core::dimension2du(l_cSize.Width, l_cSize.Height)
                    )
                  );

                  m_pSelected = m_pHover;

                  for (auto &l_sItem: l_vItems) {
                    l_pMenu->addItem(l_sItem.c_str());
                  }

                  std::vector<std::wstring> l_vPartItems = l_pPart->contextMenuEntries(std::get<1>(m_mReferencePoints[m_pHover]));
                  for (auto &l_sItem: l_vPartItems) {
                    l_pMenu->addItem(l_sItem.c_str());
                  }
                }
              }
            }
          }
        }
        else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
          if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_MENU_ITEM_SELECTED) {
            if (a_cEvent.GUIEvent.Caller->getType() == irr::gui::EGUIET_CONTEXT_MENU) {
              irr::gui::IGUIContextMenu *l_pMenu = reinterpret_cast<irr::gui::IGUIContextMenu *>(a_cEvent.GUIEvent.Caller);
              std::wstring l_sItem = l_pMenu->getItemText(l_pMenu->getSelectedItem());

              if (l_sItem == c_sAddFlatSegment) {
                tracked::CTrackPart_FlatSegment *l_pNew = new tracked::CTrackPart_FlatSegment(m_vParts.back(), m_vParts.back()->getEndNormal());

                irr::core::vector3df l_cDirection = m_vParts.back()->getEndDirection();
                
                std::vector<irr::core::vector3df> l_vRefPoints = {
                  m_vParts.back()->getReferencePoints().back(),
                  m_vParts.back()->getReferencePoints().back() + 2.5f * l_cDirection.normalize()
                };

                l_pNew->setReferencePoints(l_vRefPoints);
                m_vParts.push_back(l_pNew);

                addReferencePoints(l_pNew);

                createTrack();
              }
              else if (l_sItem == c_sAddCheckpoint) {
                tracked::CTrackPart_Checkpoint *l_pNew = new tracked::CTrackPart_Checkpoint(m_vParts.back(), m_vParts.back()->getEndNormal());
                
                irr::core::vector3df l_cDirection = m_vParts.back()->getEndDirection();
                
                std::vector<irr::core::vector3df> l_vRefPoints = {
                  m_vParts.back()->getReferencePoints().back(),
                  m_vParts.back()->getReferencePoints().back() + l_pNew->getWidth() * l_cDirection.normalize()
                };

                l_pNew->setReferencePoints(l_vRefPoints);
                m_vParts.push_back(l_pNew);

                addReferencePoints(l_pNew);

                createTrack();
              }
              else if (l_sItem == c_sAddRamp) {
                tracked::CTrackPart_Ramp *l_pNew = new tracked::CTrackPart_Ramp(m_vParts.back(), m_vParts.back()->getEndNormal());

                irr::core::vector3df l_cDirection = m_vParts.back()->getEndDirection();
                irr::core::vector3df l_cLastPoint = m_vParts.back()->getEnd().getMiddle();

                std::vector<irr::core::vector3df> l_vRefPoints = {
                  l_cLastPoint,
                  l_cLastPoint + 2.0f * m_vParts.back()->getWidth() * l_cDirection,
                  l_cLastPoint + 2.0f * m_vParts.back()->getWidth() * l_cDirection + m_vParts.back()->getWidth() * irr::core::vector3df(0.0f, 1.0f, 0.0f),
                  l_cLastPoint + 2.0f * m_vParts.back()->getWidth() * l_cDirection + m_vParts.back()->getWidth() * irr::core::vector3df(0.0f, 1.0f, 0.0f) + 2.0f * m_vParts.back()->getWidth() * l_cDirection
                };

                l_pNew->setReferencePoints(l_vRefPoints);
                m_vParts.push_back(l_pNew);

                addReferencePoints(l_pNew);

                createTrack();
              }
              else if (l_sItem == c_sAddReference) {
                if (m_pSelected != nullptr) {
                  if (m_mReferencePoints.find(m_pSelected) != m_mReferencePoints.end()) {
                    tracked::ITrackPart_Bezier *l_pPart = dynamic_cast<tracked::ITrackPart_Bezier*>(std::get<0>(m_mReferencePoints[m_pSelected]));
                    if (l_pPart != nullptr && l_pPart->getReferencePoints().size() < 4) {
                      setEditMode(enEditMode::AddReferencePoint);
                      
                      m_pMoveMgr->setMoveInPlane(
                        tracked::IGlobal::getInstance()->getDevice()->getCursorControl()->getPosition(),
                        50.0f * l_pPart->getReferencePoints().back(),
                        l_pPart->getEndNormal()
                      );

                      for (auto &l_tPoint: m_mReferencePoints) {
                        l_tPoint.first->setVisible(m_pSelected == l_tPoint.first);
                      }
                  
                      m_pPickPoint->setVisible(true);
                    }
                  }
                }
              }
              else if (l_sItem == c_sFinishTrack) {
                tracked::ITrackPart *l_pLast  = nullptr;
                tracked::ITrackPart *l_pFirst = nullptr;

                for (auto l_pPart : m_vParts) {
                  if (l_pPart->getNext() == nullptr) {
                    l_pLast = l_pPart;
                  }

                  if (l_pPart->getPrevious() == nullptr) {
                    l_pFirst = l_pPart;
                  }
                }

                if (l_pLast != nullptr && l_pFirst != nullptr && l_pLast != l_pFirst) {
                  irr::core::vector3df l_cEndDir = l_pLast->getEndDirection();
                  irr::core::vector3df l_cEndPos = l_pLast->getEnd().getMiddle();
                  irr::core::vector3df l_cStrDir = -l_pFirst->getEndDirection();
                  irr::core::vector3df l_cStrPos = irr::core::vector3df(0.0f);

                  tracked::CTrackPart_FlatSegment *l_pNew = new tracked::CTrackPart_FlatSegment(l_pLast, l_pLast->getEndNormal());

                  std::vector<irr::core::vector3df> l_cRef = {
                    l_cEndPos,
                    l_cEndPos + (l_cEndPos - l_cStrPos).getLength() * l_cEndDir,
                    l_cStrPos + (l_cStrPos - l_cEndPos).getLength() * l_cStrDir,
                    l_cStrPos
                  };

                  l_pNew->setReferencePoints(l_cRef);
                  l_pNew->setNext(l_pFirst);

                  m_vParts.push_back(l_pNew);

                  addReferencePoints(l_pNew);
                  createTrack();
                }
              }
              else if (l_sItem == c_sAddStraight) {
                tracked::CTrackPart_Straight *l_pNew = new tracked::CTrackPart_Straight(m_vParts.back(), m_vParts.back()->getEndNormal());

                irr::core::vector3df l_cDirection = m_vParts.back()->getEndDirection();
                
                std::vector<irr::core::vector3df> l_vRefPoints = {
                  m_vParts.back()->getReferencePoints().back(),
                  m_vParts.back()->getReferencePoints().back() + 2.5f * l_cDirection.normalize()
                };

                l_pNew->setReferencePoints(l_vRefPoints);
                m_vParts.push_back(l_pNew);

                addReferencePoints(l_pNew);

                createTrack();
              }
              else if (l_sItem == c_sAddJump) {
                tracked::CTrackPart_Jump *l_pNew = new tracked::CTrackPart_Jump(m_vParts.back(), m_vParts.back()->getEndNormal());
                addReferencePoints(l_pNew);
                m_vParts.push_back(l_pNew);

                createTrack();
              }
              else if (l_sItem == c_sAddTurnLeft || l_sItem == c_sAddTurnRight) {
                irr::core::vector2di l_cMouse = tracked::IGlobal::getInstance()->getDevice()->getCursorControl()->getPosition();

                irr::gui::IGUIContextMenu *l_pMenu = m_pGui->addContextMenu(irr::core::recti(l_cMouse, irr::core::dimension2du(50, 50)));

                irr::s32 l_iTag = l_sItem == c_sAddTurnLeft ? 0 : 1;

                std::vector<std::wstring> l_sItems = {
                  c_sAddTurn45,
                  c_sAddTurn90,
                  c_sAddTurn135,
                  c_sAddTurn180,
                  c_sAddTurnFree
                };

                for (auto &l_sItem: l_sItems) {
                  l_pMenu->addItem(l_sItem.c_str(), l_iTag);
                }
              }
              else {
                int l_iTurn = 0;

                std::map<std::wstring, int> l_mTurn = {
                  { c_sAddTurn45  ,  45 },
                  { c_sAddTurn90  ,  90 },
                  { c_sAddTurn135 , 135 },
                  { c_sAddTurn180 , 180 },
                  { c_sAddTurnFree, 666 }
                };

                if (l_mTurn.find(l_sItem) != l_mTurn.end()) {
                  l_iTurn = l_mTurn[l_sItem];
                  
                  if (l_iTurn == 666) {
                    tracked::CTrackPart_FlatSegment *l_pNew = new tracked::CTrackPart_FlatSegment(m_vParts.back(), m_vParts.back()->getEndNormal());

                    tracked::ITrackPart *p = m_vParts.back();

                    std::vector<irr::core::vector3df> l_vRef = {
                      p->getEnd().getMiddle(),
                      p->getEnd().getMiddle() + p->getWidth() * 2.0f * p->getEndDirection(),
                      p->getEnd().getMiddle() + p->getWidth() * 4.0f * p->getEndDirection() + (l_pMenu->getItemCommandId(l_pMenu->getSelectedItem()) == 0 ? -4.0f : 4.0f) * p->getWidth() * p->getEndSide()
                    };

                    l_pNew->setReferencePoints(l_vRef);
                    m_vParts.push_back(l_pNew);
                    addReferencePoints(l_pNew);
                    createTrack();
                  }
                  else {
                    tracked::ITrackPart *l_pNew = new tracked::CTrackPart_Turn(
                      l_iTurn ==  45 ? tracked::CTrackPart_Turn::enTurnAngle::FourtyFive : 
                      l_iTurn ==  90 ? tracked::CTrackPart_Turn::enTurnAngle::Ninety : 
                      l_iTurn == 135 ? tracked::CTrackPart_Turn::enTurnAngle::OneHundred35 :
                                       tracked::CTrackPart_Turn::enTurnAngle::OneHundred80,
                      l_pMenu->getItemCommandId(l_pMenu->getSelectedItem()) == 0,
                      m_vParts.back(),
                      m_vParts.back()->getEndNormal()
                    );
                    m_vParts.push_back(l_pNew);
                    addReferencePoints(l_pNew);
                    createTrack();
                  }

                  l_bRet = true;
                }

                if (!l_bRet)
                  printf("Item Selected: %i (%ls)\n", l_pMenu->getSelectedItem(), l_sItem.c_str());
              }
            }
          }
        }
        else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
          if (a_cEvent.UserEvent.UserData1 == c_iStartupFinished) {
            prepareScene();

            tracked::CTrackPart_FlatSegment *l_pStart = new tracked::CTrackPart_FlatSegment(nullptr, irr::core::vector3df(0.0f, 1.0f, 0.0f));
            l_pStart->setReferencePoints({ irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 3.5f) });

            m_vParts.push_back(l_pStart);
            addReferencePoints(l_pStart);
            createTrack();

            irr::SEvent l_cEvent{};
            l_cEvent.EventType = irr::EET_USER_EVENT;
            l_cEvent.UserEvent.UserData1 = c_iSceneLoaded;
            l_cEvent.UserEvent.UserData2 = c_iSceneLoaded;
            tracked::IGlobal::getInstance()->getDevice()->postEventFromUser(l_cEvent);
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iRecalcTrack) {
            createTrack();
            updateReferencePoints();
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iSceneWillBeCleared) {
            m_pWorld = nullptr;
            m_pHover = nullptr;
            m_pTrack = nullptr;

            delete m_pAiPath;
            m_pAiPath = nullptr;

            clearTrack();
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iSceneLoaded) {
            prepareScene();

            if (m_vParts.size() > 0)
              m_vParts.front()->updateReferencePoints();

            createTrack();
          }
        }
      }

      return l_bRet;
    }

    /**
    * Prepare the scene for editing
    */
    void CCmdEditTrack::prepareScene() {
      m_pSmgr->loadScene("data/scenes/skybox.xml");

      m_pWorld = m_pSmgr->addSceneNode("WorldNode", m_pSmgr->getRootSceneNode());
      dustbin::scenenodes::CStartingGridSceneNode *l_pGrid = reinterpret_cast<dustbin::scenenodes::CStartingGridSceneNode *>(m_pSmgr->addSceneNode("StartingGridScenenode", m_pWorld));
      l_pGrid->setPosition(irr::core::vector3df(0.0f, 2.5f, 150.0f));
      l_pGrid->setAngle(180.0f);

      if (m_fBaseHeight == -1 && m_fBaseOffset == -1) {
        irr::scene::IMesh *l_pPlane = m_pSmgr->getMesh("data/objects/plane.3ds");

        m_pZeroPlane = m_pSmgr->addMeshSceneNode(
          l_pPlane,
          m_pSmgr->getRootSceneNode(),
          -1,
          irr::core::vector3df(0.0f),
          irr::core::vector3df(0.0f),
          irr::core::vector3df(150.0f, 5.0f, 150.0f)
        );

        m_pZeroPlane->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/editor_cross.png"));
        m_pZeroPlane->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
        m_pZeroPlane->getMaterial(0).Lighting = false;

        m_pZeroPlane->addAnimator(new animators::CPlaneAnimator(m_pZeroPlane));
      }
      else m_pZeroPlane = nullptr;

      m_pPickPoint = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/box.3ds"));
      m_pPickPoint->setScale(irr::core::vector3df(1.0f, 5.0f, 1.0f));
      m_pPickPoint->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/yellow.png"));
      m_pPickPoint->setVisible(false);

      scenenodes::CDustbinLight *l_pLight = reinterpret_cast<scenenodes::CDustbinLight *>(m_pSmgr->addSceneNode(scenenodes::g_DustbinLightName));
      if (l_pLight != nullptr) {
        l_pLight->setPosition(irr::core::vector3df(-100.55f, 75.72f, 143.74f));
        l_pLight->setLightTarget(irr::core::vector3df(0.0f));
        l_pLight->setNearValue(5.0f);
        l_pLight->setFarValue(1000.0f);
        l_pLight->setFieldOfView(2000.0f);
      }

      scenenodes::CRostrumNode *l_pRostrum = reinterpret_cast<scenenodes::CRostrumNode *>(m_pSmgr->addSceneNode(scenenodes::g_RostrumNodeName));
      if (l_pRostrum != nullptr) {
        l_pRostrum->setPosition(irr::core::vector3df(100.0f, 30.5f, 25.0f));
        l_pRostrum->setRotation(irr::core::vector3df(0.0f, -45.0f, 0.0f));
      }
    }

    /**
    * Clear the track
    */
    void CCmdEditTrack::clearTrack() {
      clearReferencePoints();

      while (m_vParts.size() > 0) {
        tracked::ITrackPart *p = m_vParts.front();
        m_vParts.erase(m_vParts.begin());
        delete p;
      }

      irr::scene::ISceneNode *l_pDecoration = helpers::getDecorationRootNode(m_pSmgr->getRootSceneNode(), false);

      if (l_pDecoration != nullptr) m_pSmgr->addToDeletionQueue(l_pDecoration);
    }

    /**
    * Set the parameters for the track base
    * @param a_fOffset the offset of the walls
    * @param a_fHeight the height of the walls
    */
    void CCmdEditTrack::setBaseParameters(irr::f32 a_fOffset, irr::f32 a_fHeight) {
      m_fBaseOffset = a_fOffset;
      m_fBaseHeight = a_fHeight;

      createTrack();
    }

    /**
    * Add a track part
    * @param a_pPart the new track part
    */
    void CCmdEditTrack::addTrackPart(tracked::ITrackPart *a_pPart) {
      for (auto l_pPart: m_vParts) {
        if (l_pPart->getPreviousID() == a_pPart->getId()) l_pPart->setPrevious(a_pPart);
        if (l_pPart->getNextID    () == a_pPart->getId()) l_pPart->setNext    (a_pPart);
        if (a_pPart->getPreviousID() == l_pPart->getId()) a_pPart->setPrevious(l_pPart);
        if (a_pPart->getNextID    () == l_pPart->getId()) a_pPart->setNext    (l_pPart);
      }

      m_vParts.push_back(a_pPart);
      addReferencePoints(a_pPart);
    }
    
    /**
    * Get the index of a mesh buffer (mesh buffer indices found in "tracked::c_iBufferIdx..."
    * @param a_iBuffer the buffer index
    * @return the index of the buffer in the track mesh, 0xFFFF if the buffer was not added
    */
    irr::u32 CCmdEditTrack::getBufferIndex(irr::u32 a_iBuffer) {
      return m_mBufferMap.find(a_iBuffer) != m_mBufferMap.end() ? m_mBufferMap[a_iBuffer] : 0xFFFF;
    }

    /**
    * Get the track mesh
    * @return the track mesh
    */
    irr::scene::IMeshSceneNode *CCmdEditTrack::getTrackMesh() {
      return m_pTrack;
    }

    /**
    * Set the data of the thumbnail camera
    * @param a_cPos the new position of the thumbnail camera
    * @param a_cTgt the new target of the thumbnail camera
    */
    void CCmdEditTrack::setThumbnailCamera(const irr::core::vector3df &a_cPos, const irr::core::vector3df &a_cTgt) {
      m_cThumbnailPos = a_cPos;
      m_cThumbnailTgt = a_cTgt;
    }

    /**
    * Get the thumbnail camera parameters
    * @param a_cPos [out] position of the thumbnail camera
    * @param a_cTgt [out] target of the thumbnail camera
    */
    void CCmdEditTrack::getThumbnailCamera(irr::core::vector3df &a_cPos, irr::core::vector3df &a_cTgt) const {
      a_cPos = m_cThumbnailPos;
      a_cTgt = m_cThumbnailTgt;
    }

    /**
    * Save the track information
    * @param a_cAttr the IAttributes to save the information to
    */
    void CCmdEditTrack::saveTrackInformation(irr::io::IAttributes &a_cAttr) {
      a_cAttr.addVector3d("ThumbnailPosition", m_cThumbnailPos);
      a_cAttr.addVector3d("ThumbnailTarget"  , m_cThumbnailTgt);

      a_cAttr.addString("TrackName", m_sTrackName.c_str());
      a_cAttr.addString("Uuid"     , m_sTrackUUID.c_str());

      irr::scene::ISceneNode *l_pDecoration = helpers::getDecorationRootNode(m_pSmgr->getRootSceneNode(), false);

      if (l_pDecoration != nullptr) {
        data::CMemoryWriteFile *l_pFile = new data::CMemoryWriteFile("__DecorationData");
        if (l_pFile) {
          irr::io::IXMLWriter *l_pXml = m_pSmgr->getFileSystem()->createXMLWriter(l_pFile);
          if (l_pXml) {
            m_pSmgr->saveScene(l_pXml, m_pGui->getFileSystem()->getWorkingDirectory(), nullptr, l_pDecoration);
            l_pXml->drop();
          }

          std::string l_sDecoration = messages::base64Encode(l_pFile->getData().data(), l_pFile->getData().size(), false);

          a_cAttr.addString("decoration", l_sDecoration.c_str());

          l_pFile->drop();
        }
      }
    }

    /**
    * Load the track information
    * @param a_cAttr the IAttributes to load the information from
    */
    void CCmdEditTrack::loadTrackInformation(irr::io::IAttributes &a_cAttr) {
      if (a_cAttr.existsAttribute("ThumbnailPosition")) m_cThumbnailPos = a_cAttr.getAttributeAsVector3d("ThumbnailPosition");
      if (a_cAttr.existsAttribute("ThumbnailTarget"  )) m_cThumbnailTgt = a_cAttr.getAttributeAsVector3d("ThumbnailTarget"  );

      if (a_cAttr.existsAttribute("TrackName")) m_sTrackName = a_cAttr.getAttributeAsString("TrackName").c_str();
      if (a_cAttr.existsAttribute("Uuid"     )) m_sTrackUUID = a_cAttr.getAttributeAsString("Uuid"     ).c_str();

      if (a_cAttr.existsAttribute("decoration")) {
        irr::scene::ISceneNode *l_pDecoration = helpers::getDecorationRootNode(m_pSmgr->getRootSceneNode());

        std::string l_sDecoration64 = a_cAttr.getAttributeAsString("decoration").c_str();

        size_t l_iSize = 0;

        unsigned char *l_pDataU = messages::base64_decode(l_sDecoration64.c_str(), l_sDecoration64.size(), &l_iSize);

        if (l_pDataU != nullptr) {
          std::vector<unsigned char> l_vData;
          
          for (size_t i = 0; i < l_iSize; i++) {
            l_vData.push_back(l_pDataU[i]);
          }

          irr::scene::ISceneNode *l_pDecoration = helpers::getDecorationRootNode(m_pSmgr->getRootSceneNode(), true);

          data::CMemoryReadFile *l_pFile = new data::CMemoryReadFile("___DecorationLoader", l_vData);

          if (l_pFile != nullptr) {
            m_pSmgr->loadScene(l_pFile, nullptr, l_pDecoration);
            l_pFile->drop();
          }
        }
      }

      ensureUUID();
    }

    /**
    * Make sure the track has an UUID assigned
    */
    void CCmdEditTrack::ensureUUID() {
      if (m_sTrackUUID == "") {
        UUIDv4::UUIDGenerator<std::mt19937_64> l_cGenerator;

        UUIDv4::UUID l_cUuid = l_cGenerator.getUUID();
        char c[64];
        memset(c, 0, 64);

        l_cUuid.str(c);

        m_sTrackUUID = c;
      }
    }

    /**
    * Change the name of the track
    * @param a_sName the new name of the track
    */
    void CCmdEditTrack::setTrackName(const std::string &a_sName) {
      m_sTrackName = a_sName;
    }

    /**
    * Get the name of the track
    * @return the name of the track
    */
    const std::string &CCmdEditTrack::getTrackName() {
      return m_sTrackName;
    }

    /**
    * Get the track's UUID used as name for the export folder
    * @return the track's UUID
    */
    const std::string &CCmdEditTrack::getUUID() {
      return m_sTrackUUID;
    }

    /**
    * Get the main menu
    * @return the main menu
    */
    CCmdMainMenu *CCmdEditTrack::getMainMenu() {
      return m_pMainMenu;
    }
  }
}