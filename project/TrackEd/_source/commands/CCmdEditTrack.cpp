// (w) 2024 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>
#include <animators/CPlaneAnimator.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdMainMenu.h>
#include <editor/CTrackParts.h>
#include <editor/ITrackPart.h>
#include <CDefines.h>
#include <IGlobal.h>
#include <cmath>

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
      m_pMenu       (nullptr),
      m_pColMgr     (nullptr),
      m_pClick      (nullptr),
      m_pMainMenu   (a_pMainMenu),
      m_bFinishTrack(false),
      m_bFinished   (false),
      m_fBaseOffset (-1.0f),
      m_fBaseHeight (-1.0f)
    {
      m_pColMgr = m_pSmgr->getSceneCollisionManager();
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
      for (auto &l_tPoint: m_mReferencePoints) {
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

      m_cPlane = irr::core::plane3df(irr::core::vector3df(0.0f), irr::core::vector3df(0.0f, 1.0f, 0.0f));
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

      m_vBuffers[tracked::c_iBufferIdxBottom]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

      m_vBuffers[tracked::c_iBufferIdxYellow]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/yellow.png"));
      m_vBuffers[tracked::c_iBufferIdxBlack ]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/black.png"));

      m_vBuffers[tracked::c_iBufferIdxGround]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/ground_back.png"));
      m_vBuffers[tracked::c_iBufferIdxGround]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_ground.png"));

      m_vBuffers[tracked::c_iBufferIdxWallInner]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/wall_outer.png"));

      m_vBuffers[tracked::c_iBufferIdxWallOuter]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/wall.png"));

      if (m_pMesh == nullptr) {
        m_pMesh = new irr::scene::SMesh();
      }

      m_pMesh->MeshBuffers.clear();

      bool l_bBlack = true;

      for (auto l_pPart: m_vParts) {
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

      irr::SEvent l_cEvent;
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
      // if (m_vParts.size() > 0) {
      //   irr::core::vector3df l_cOld1;
      //   irr::core::vector3df l_cOld2;
      // 
      //   m_vParts.front()->getSectionLine(0, l_cOld1, l_cOld2);
      // 
      //   bool l_bColor = true;
      // 
      //   for (auto l_pPart : m_vParts) {
      //     irr::u32 l_iIndex = 0;
      // 
      //     irr::core::vector3df l_cNew1;
      //     irr::core::vector3df l_cNew2;
      // 
      //     while (l_pPart->getSectionLine(l_iIndex, l_cNew1, l_cNew2)) {
      //       if (l_cOld1 != l_cNew1 && l_cOld2 != l_cNew2) {
      //         irr::core::triangle3df l_cTriangle = irr::core::triangle3df(50.0f * l_cOld1, 50.0f * l_cOld2, 50.0f * l_cNew1);
      //         m_pDrv->draw3DTriangle(l_cTriangle, l_pPart == m_vParts.front() ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : l_bColor ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF));
      // 
      //         l_cTriangle = irr::core::triangle3df(50.0f * l_cOld2, 50.0f * l_cNew2, 50.0f * l_cNew1);
      //         m_pDrv->draw3DTriangle(l_cTriangle, l_pPart == m_vParts.front() ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : l_bColor ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF));
      //       }
      // 
      //       l_cOld1 = l_cNew1;
      //       l_cOld2 = l_cNew2;
      // 
      //       l_iIndex++;
      // 
      //     }
      //     l_bColor = !l_bColor;
      //   }
      // }
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

      if (!l_bRet) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          irr::core::vector2di   l_cMouse = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
          irr::gui::IGUIElement *l_pHover = m_pGui->getRootGUIElement()->getElementFromPoint(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

          if (m_eEditMode == enEditMode::Default || m_eEditMode == enEditMode::AddReferencePoint) {
            irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());

            irr::core::vector3df v;
            irr::core::triangle3df t;

            irr::scene::ISceneNode *l_pObject = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cRay, v, t, 0);

            irr::core::vector3df l_cRayCollision;

            m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

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
                irr::core::vector3df l_cEnd = m_vParts.back()->getEndPosition () * 50.0f;

                if (m_eEditMode == enEditMode::AddReferencePoint) {
                  m_pPickPoint->setPosition(l_cRayCollision);
                }
              }
            }
          }
          else if (m_eEditMode == enEditMode::MovePointOnLine) {
            irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
            irr::core::vector3df l_cRayCollision;
            m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

            m_pSelected->setPosition(m_cMoveLine.getClosestPoint(l_cRayCollision));

            l_bRet = true;
          }
          else if (m_eEditMode == enEditMode::MovePointInPlane) {
            irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
            irr::core::vector3df l_cRayCollision;
            m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

            m_pSelected->setPosition(l_cRayCollision + 2.5f * std::get<0>(m_mReferencePoints[m_pSelected])->getNormal());

            l_bRet = true;
          }
          
          if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_cDownPoint[0] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            if (m_eEditMode == enEditMode::Default) {
              if (l_pHover != nullptr && m_mReferencePoints.find(m_pHover) != m_mReferencePoints.end()) {
                irr::core::vector3df l_cPlanePoint;
                irr::core::vector3df l_cPlaneNormal;

                bool l_bCanMove = std::get<0>(m_mReferencePoints[m_pHover])->canPointMoveFreely(std::get<1>(m_mReferencePoints[m_pHover]), m_cMoveLine, l_cPlanePoint, l_cPlaneNormal);

                if (l_bCanMove) {
                  setEditMode(enEditMode::MovePointInPlane);
                  m_pSelected  = m_pHover;

                  m_cMovePlane = irr::core::plane3df(50.0f * l_cPlanePoint, l_cPlaneNormal);

                  irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
                  irr::core::vector3df l_cRayCollision;
                  m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, m_cMovePoint);
                }
                else {
                  if (m_cMoveLine.getLength() > 0.0f) {
                    setEditMode(enEditMode::MovePointOnLine);
                    m_pSelected = m_pHover;

                    m_cMoveLine.start = 50.0f * m_cMoveLine.start;
                    m_cMoveLine.end   = 50.0f * m_cMoveLine.end;

                    m_cMovePlane = irr::core::plane3df(50.0f * l_cPlanePoint, l_cPlaneNormal);

                    irr::core::vector3df l_cRayCollision;

                    irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
                    m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

                    m_cMovePoint = m_cMoveLine.getClosestPoint(l_cRayCollision);
                  }
                  else {
                    if (m_pMainMenu != nullptr) {
                      m_pMainMenu->setWarningText(L"This point cannot be moved!");
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

                if (m_pMenu != nullptr) {
                  irr::gui::IGUIElement *l_pElement = m_pGui->getRootGUIElement()->getElementFromPoint(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

                  if (l_pElement != m_pMenu) {
                    m_pMenu->setVisible(false);
                    m_pGui->addToDeletionQueue(m_pMenu);
                    m_pMenu = nullptr;
                  }
                }
              }
            }
            else if (m_eEditMode == enEditMode::AddReferencePoint) {
              if (m_mReferencePoints.find(m_pSelected) != m_mReferencePoints.end()) {
                tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pSelected]);

                irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
                irr::core::vector3df l_cRayCollision;
                m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

                std::vector<irr::core::vector3df> l_vRefPoints = l_pPart->getReferencePoints();
                l_vRefPoints.push_back(l_cRayCollision / 50.0f);
                l_pPart->setReferencePoints(l_vRefPoints);
                addReferencePoints(l_pPart);
                m_pPickPoint->setVisible(false);

                createTrack();

                setEditMode(enEditMode::Default);
              }
              else printf("Reference point not found!\n");
            }
            else if (m_eEditMode == enEditMode::MovePointInPlane) {
              irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
              irr::core::vector3df l_cRayCollision;
              m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

              irr::core::vector3df l_cMove = l_cRayCollision - m_cMovePoint;

              std::vector<irr::core::vector3df> l_vRefPoints = std::get<0>(m_mReferencePoints[m_pSelected])->getReferencePoints();
              l_vRefPoints[std::get<1>(m_mReferencePoints[m_pSelected])] += l_cMove / 50.0f;

              std::get<0>(m_mReferencePoints[m_pSelected])->setReferencePoints(l_vRefPoints);

              createTrack();
              setEditMode(enEditMode::Default);
              updateReferencePoints();
              m_pSelected = nullptr;

              l_bRet = true;
            }
            else if (m_eEditMode == enEditMode::MovePointOnLine) {
              irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
              irr::core::vector3df l_cRayCollision;
              m_cMovePlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

              std::vector<irr::core::vector3df> l_vRefPoints = std::get<0>(m_mReferencePoints[m_pSelected])->getReferencePoints();

              irr::core::vector3df l_cClosest = m_cMoveLine.getClosestPoint(l_cRayCollision);

              irr::core::vector3df l_cMove = l_cClosest - m_cMovePoint;

              l_vRefPoints[std::get<1>(m_mReferencePoints[m_pSelected])] += l_cMove / 50.0f;

              m_cMovePoint = l_cClosest;

              std::get<0>(m_mReferencePoints[m_pSelected])->setReferencePoints(l_vRefPoints);

              createTrack();
              setEditMode(enEditMode::Default);
              updateReferencePoints();
              m_pSelected = nullptr;
              l_bRet = true;
            }

            if (m_pMainMenu != nullptr)
              m_pMainMenu->hideWarning();
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN) {
            m_cDownPoint[1] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP) {
            if (m_cDownPoint[1].getDistanceFrom(l_cMouse) < 2) {
              if (m_pMenu != nullptr) {
                irr::gui::IGUIElement *l_pElement = m_pGui->getRootGUIElement()->getElementFromPoint(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

                if (l_pElement != m_pMenu) {
                  m_pMenu->setVisible(false);
                  m_pGui->addToDeletionQueue(m_pMenu);
                  m_pMenu = nullptr;
                }
              }
              else {
                if (m_pHover != nullptr) {
                  if (m_eEditMode == enEditMode::Default) {
                    std::vector<std::tuple<std::wstring, bool>> l_vItems;

                    tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pHover]);

                    if (l_pPart->partAdditionAllowed(std::get<1>(m_mReferencePoints[m_pHover]))) {
                      l_vItems.push_back(std::make_tuple(c_sAddSegment, true));
                      l_vItems.push_back(std::make_tuple(c_sAddCheckpoint, true));
                    }

                    if (l_pPart->getPrevious() == nullptr) {
                      l_vItems.push_back(std::make_tuple(c_sFinishTrack, true));
                    }


                    // l_vItems.push_back(std::make_tuple(c_sAddIncline   , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddDecline   , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddJump      , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddGap       , true));

                    if (l_pPart->refPointAdditionAllowed(std::get<1>(m_mReferencePoints[m_pHover]))) {
                      l_vItems.push_back(std::make_tuple(L"----", false));
                      l_vItems.push_back(std::make_tuple(c_sAddReference, true));
                    }

                    irr::core::dimension2du l_cSize = irr::core::dimension2du(0, 0);

                    irr::core::dimension2du l_cScreen = m_pDrv->getScreenSize();

                    for (auto l_sItem : l_vItems) {
                      irr::core::dimension2du l_cDim = m_pGui->getSkin()->getFont()->getDimension(std::get<0>(l_sItem).c_str());

                      l_cSize.Height += l_cDim.Height;
                      if (l_cDim.Width < l_cSize.Width)
                        l_cDim.Width = l_cSize.Width;

                      l_cSize.Width  = 6 * l_cSize.Width / 5;
                      l_cSize.Height = 6 * l_cSize.Height / 5;

                      m_pMenu = m_pGui->addContextMenu(
                        irr::core::recti(
                          (irr::u32)a_cEvent.MouseInput.X < l_cScreen.Width - l_cSize.Width ? a_cEvent.MouseInput.X : l_cScreen.Width - l_cSize.Width,
                          (irr::u32)a_cEvent.MouseInput.Y < l_cScreen.Height - l_cSize.Height ? a_cEvent.MouseInput.Y : l_cScreen.Height - l_cSize.Height,
                          l_cSize.Width,
                          l_cSize.Height
                        )
                      );

                      m_pSelected = m_pHover;

                      for (auto l_sItem : l_vItems) {
                        m_pMenu->addItem(std::get<0>(l_sItem).c_str());
                      }
                    }
                  }
                }
              }
            }
          }
        }
        else if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
          if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_MENU_ITEM_SELECTED) {
            if (a_cEvent.GUIEvent.Caller == m_pMenu) {
              std::wstring l_sItem = m_pMenu->getItemText(m_pMenu->getSelectedItem());

              if (l_sItem == c_sAddSegment) {
                tracked::CTrackPart_Bezier *l_pNew = new tracked::CTrackPart_Bezier(m_vParts.back(), m_vParts.back()->getNormal());

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
                tracked::CTrackPart_Checkpoint *l_pNew = new tracked::CTrackPart_Checkpoint(m_vParts.back(), m_vParts.back()->getNormal());
                
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
              else if (l_sItem == c_sAddReference) {
                if (m_pSelected != nullptr) {
                  if (m_mReferencePoints.find(m_pSelected) != m_mReferencePoints.end()) {
                    tracked::CTrackPart_Bezier *l_pPart = dynamic_cast<tracked::CTrackPart_Bezier *>(std::get<0>(m_mReferencePoints[m_pSelected]));
                    if (l_pPart != nullptr && l_pPart->getReferencePoints().size() < 4) {
                      setEditMode(enEditMode::AddReferencePoint);

                      m_cMovePlane = irr::core::plane3df(l_pPart->getReferencePoints().front(), l_pPart->getNormal());

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

                  tracked::CTrackPart_Bezier *l_pNew = new tracked::CTrackPart_Bezier(l_pLast, l_pLast->getNormal());

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
              else printf("Item Selected: %i (%ls)\n", m_pMenu->getSelectedItem(), l_sItem.c_str());


              m_pMenu = nullptr;
            }
          }
        }
        else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
          if (a_cEvent.UserEvent.UserData1 == c_iStartupFinished) {
            m_pSmgr->loadScene("data/scenes/skybox.xml");

            m_pWorld = m_pSmgr->addSceneNode("WorldNode", m_pSmgr->getRootSceneNode());
            dustbin::scenenodes::CStartingGridSceneNode *l_pGrid = reinterpret_cast<dustbin::scenenodes::CStartingGridSceneNode *>(m_pSmgr->addSceneNode("StartingGridScenenode", m_pWorld));
            l_pGrid->setPosition(irr::core::vector3df(0.0f, 2.5f, 150.0f));
            l_pGrid->setAngle(180.0f);

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

            m_pPickPoint = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/box.3ds"));
            m_pPickPoint->setScale(irr::core::vector3df(1.0f, 5.0f, 1.0f));
            m_pPickPoint->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/yellow.png"));
            m_pPickPoint->setVisible(false);

            tracked::CTrackPart_Bezier *l_pStart = new tracked::CTrackPart_Bezier(nullptr, irr::core::vector3df(0.0f, 1.0f, 0.0f));
            l_pStart->setReferencePoints({ irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 3.5f) });

            m_vParts.push_back(l_pStart);
            addReferencePoints(l_pStart);
            createTrack();

            irr::SEvent l_cEvent;
            l_cEvent.EventType = irr::EET_USER_EVENT;
            l_cEvent.UserEvent.UserData1 = c_iSceneLoaded;
            l_cEvent.UserEvent.UserData2 = c_iSceneLoaded;
            tracked::IGlobal::getInstance()->getDevice()->postEventFromUser(l_cEvent);
          }
        }
      }

      return l_bRet;
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
  }
}