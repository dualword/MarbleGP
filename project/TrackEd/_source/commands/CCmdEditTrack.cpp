// (w) 2024 by Dustbin::Games / Christian Keimel
#include <animators/CPlaneAnimator.h>
#include <helpers/CStringHelpers.h>
#include <commands/CCmdEditTrack.h>
#include <editor/CTrackParts.h>
#include <editor/ITrackPart.h>
#include <CDefines.h>
#include <IGlobal.h>

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

    CCmdEditTrack::CCmdEditTrack(irr::scene::ISceneManager *a_pSmgr, irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui) :
      ICommand      ("CmdEditTrack"),
      m_pSmgr       (a_pSmgr),
      m_pGui        (a_pGui),
      m_pDrv        (a_pDrv),
      m_pWorld      (nullptr),
      m_pTrack      (nullptr),
      m_pMesh       (nullptr),
      m_pPick       (nullptr),
      m_pStart      (nullptr),
      m_pColMgr     (nullptr),
      m_bFinishTrack(false),
      m_bFinished   (false)
    {
      m_pColMgr  = m_pSmgr->getSceneCollisionManager();

      tracked::CTrackPart_Bezier *l_pStart = new tracked::CTrackPart_Bezier(nullptr, irr::core::vector3df(0.0f), m_pColMgr);
      l_pStart->setReferencePoints({ irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 3.5f) });

      m_vParts.push_back(l_pStart);

      m_cPlane = irr::core::plane3df(irr::core::vector3df(0.0f), irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }
        
    CCmdEditTrack::~CCmdEditTrack() {
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * return true if the command is finished and can be removed
    */
    bool CCmdEditTrack::isFinished() {
      return false;
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

      m_vBuffers[tracked::c_iBufferIdxBottom]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

      m_vBuffers[tracked::c_iBufferIdxYellow]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/yellow.png"));
      m_vBuffers[tracked::c_iBufferIdxBlack]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/black.png"));

      if (m_pMesh == nullptr) {
        m_pMesh = new irr::scene::SMesh();
      }

      tracked::ITrackPart *l_pLast = nullptr;

      m_pMesh->MeshBuffers.clear();

      int l_iCount = 0;
      for (auto l_pBuffer : m_vBuffers) {
        if (l_pBuffer->Vertices.size() > 0 && l_pBuffer->Indices.size() > 0) {
          l_pBuffer->recalculateBoundingBox();
          m_pMesh->addMeshBuffer(l_pBuffer);
          printf("Add mesh buffer %i: %i, %i\n", l_iCount, l_pBuffer->Vertices.size(), l_pBuffer->Indices.size());
        }
        l_iCount++;
      }

      m_pMesh->recalculateBoundingBox();
      m_pMesh->setDirty();

      if (m_pTrack == nullptr) {
        m_pTrack = m_pSmgr->addMeshSceneNode(m_pMesh, m_pWorld, -1, irr::core::vector3df(0.0f), irr::core::vector3df(0.0f), irr::core::vector3df(50.0f));
        m_pTrack->setName("GeneratedTrack");
      }
    }

    /**
    * Let this command draw something (if necessary)
    * @param a_pDrv the video driver
    */
    void CCmdEditTrack::draw(irr::video::IVideoDriver *a_pDrv) {
      if (m_vParts.size() > 0) {
        irr::core::vector3df l_cOld1;
        irr::core::vector3df l_cOld2;

        m_vParts.front()->getSectionLine(0, l_cOld1, l_cOld2);

        bool l_bColor = true;

        for (auto l_pPart : m_vParts) {
          irr::u32 l_iIndex = 0;

          irr::core::vector3df l_cNew1;
          irr::core::vector3df l_cNew2;

          while (l_pPart->getSectionLine(l_iIndex, l_cNew1, l_cNew2)) {
            if (l_cOld1 != l_cNew1 && l_cOld2 != l_cNew2) {
              irr::core::triangle3df l_cTriangle = irr::core::triangle3df(50.0f * l_cOld1, 50.0f * l_cOld2, 50.0f * l_cNew1);
              m_pDrv->draw3DTriangle(l_cTriangle, l_pPart == m_vParts.front() ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : l_bColor ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF));

              l_cTriangle = irr::core::triangle3df(50.0f * l_cOld2, 50.0f * l_cNew2, 50.0f * l_cNew1);
              m_pDrv->draw3DTriangle(l_cTriangle, l_pPart == m_vParts.front() ? irr::video::SColor(0xFF, 0xFF, 0x80, 0x80) : l_bColor ? irr::video::SColor(0xFF, 0xFF, 0xFF, 0) : irr::video::SColor(0xFF, 0, 0, 0xFF));
            }

            l_cOld1 = l_cNew1;
            l_cOld2 = l_cNew2;

            l_iIndex++;

          }
          l_bColor = !l_bColor;
        }
      }
    }

    /**
    * Add a new track part
    * @param a_bFinishLap set to "true" if the next part is the start
    */
    void CCmdEditTrack::addNewTrackPart(bool a_bFinishLap) {
      if (a_bFinishLap) {
        m_bFinished = true;

        if (m_pStart != nullptr) m_pStart->setVisible(false);
        if (m_pPick  != nullptr) m_pPick ->setVisible(false);

        for (auto l_pPoint : m_vNewPoints) {
          l_pPoint->setVisible(false);
        }

        printf("Track finished.\n");
      }

      tracked::CTrackPart_Bezier *l_pNew = new tracked::CTrackPart_Bezier(m_vParts.back(), m_vParts.back()->getEndPosition(), m_pColMgr);
      std::vector<irr::core::vector3df> l_cReference;

      l_cReference.push_back(m_vParts.back()->getEndPosition());

      for (auto l_pMesh : m_vNewPoints) {
        l_pMesh->updateAbsolutePosition();
        l_cReference.push_back(l_pMesh->getAbsolutePosition() / 50.0f);
      }

      l_pNew->setReferencePoints(l_cReference);
      l_pNew->setNext(a_bFinishLap ? m_vParts.front() : m_vParts.back());
      m_vParts.push_back(l_pNew);

      for (auto l_pNode: m_vNewPoints) {
        l_pNode->setVisible(false);
        m_pSmgr->addToDeletionQueue(l_pNode);
      }

      m_vNewPoints.clear();
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdEditTrack::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (!l_bRet) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          if (m_pPick != nullptr && m_pPick->isVisible()) {
            irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
            irr::core::vector2di l_cMouse = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            irr::core::vector3df v;
            irr::core::triangle3df t;

            irr::scene::ISceneNode *l_pObject = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cRay, v, t, 0);

            irr::core::vector3df l_cRayCollision;

            m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

            if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
              m_bFinishTrack = false;

              irr::core::vector3df l_cDir = m_vParts.back()->getEndDirection();
              irr::core::vector3df l_cEnd = m_vParts.back()->getEndPosition () * 50.0f;

              if (m_vNewPoints.size() > 0) {
                if (l_pObject == m_pStart) {
                  l_cRayCollision = m_pStart->getAbsolutePosition();
                  m_bFinishTrack = true;
                }
                else {
                  irr::core::vector2df l_cDir2d = irr::core::vector2df(l_cDir.X, l_cDir.Z);
                  irr::core::vector2df l_cEnd2d = irr::core::vector2df(l_cEnd.X, l_cEnd.Z);
                  irr::core::vector2df l_cPnt2d = irr::core::vector2df(l_cRayCollision.X, l_cRayCollision.Z);

                  irr::core::line2df l_cLine = irr::core::line2df(l_cEnd2d, l_cEnd2d + 1000.0f * l_cDir2d.normalize());

                  irr::core::vector2df l_cClose = l_cLine.getClosestPoint(l_cPnt2d);

                  if ((l_cClose - l_cPnt2d).getLength() < 10.0f) {
                    l_cRayCollision.X = l_cClose.X;
                    l_cRayCollision.Z = l_cClose.Y;
                  }
                  else if (std::abs(l_cPnt2d.X) < 2.0f) {
                    l_cRayCollision.X = 0.0f;
                  }
                }

                m_pPick->setPosition(l_cRayCollision);
              }
              else {
                irr::core::line3df l_cLine = irr::core::line3df(l_cEnd, l_cEnd + 1000.0f * l_cDir);
                m_pPick->setPosition(l_cLine.getClosestPoint(l_cRayCollision));
              }
            }
            else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
              m_cDownPoint[0] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            }
            else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
              if (m_cDownPoint[0].getDistanceFrom(l_cMouse) < 2) {
                m_pPick->updateAbsolutePosition();
                irr::scene::ISceneNode *l_pNode = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"), m_pSmgr->getRootSceneNode(), -1, m_pPick->getAbsolutePosition(), irr::core::vector3df(), irr::core::vector3df(0.5f));
                l_pNode->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/red.png"));
                m_vNewPoints.push_back(l_pNode);

                if (m_vNewPoints.size() >= 3) {
                  addNewTrackPart(m_bFinishTrack);
                }
              }
            }
            else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_PRESSED_DOWN) {
              m_cDownPoint[1] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            }
            else if (a_cEvent.MouseInput.Event == irr::EMIE_RMOUSE_LEFT_UP) {
              if (m_cDownPoint[1].getDistanceFrom(l_cMouse) < 2 && m_vNewPoints.size() > 0) {
                addNewTrackPart(m_bFinishTrack);
              }
            }
          }
        }
        else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
          if (a_cEvent.UserEvent.UserData1 == c_iStartupFinished) {
            m_pSmgr->loadScene("data/scenes/skybox.xml");

            m_pWorld = m_pSmgr->addSceneNode("WorldNode", m_pSmgr->getRootSceneNode());
            irr::scene::ISceneNode *l_pGrid = m_pSmgr->addSceneNode("StartingGridScenenode", m_pWorld);
            l_pGrid->setPosition(irr::core::vector3df(0.0f, 2.5f, 150.0f));
            l_pGrid->setRotation(irr::core::vector3df(0.0f, 180.0f, 0.0f));

            irr::scene::IMesh *l_pPlane = m_pSmgr->getMesh("data/objects/plane.3ds");

            irr::scene::IMeshSceneNode *l_pNode = m_pSmgr->addMeshSceneNode(
              l_pPlane, 
              m_pSmgr->getRootSceneNode(), 
              -1, 
              irr::core::vector3df(0.0f),
              irr::core::vector3df(0.0f),
              irr::core::vector3df(150.0f, 5.0f, 150.0f)
            );
          
            l_pNode->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/editor_cross.png"));
            l_pNode->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
            l_pNode->getMaterial(0).Lighting = false;
          
            l_pNode->addAnimator(new animators::CPlaneAnimator(l_pNode));

            m_pPick = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"));
            m_pPick->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/yellow.png"));

            createTrack();

            m_pStart = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"), m_pSmgr->getRootSceneNode(), -1);
            m_pStart->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/wall_outer_back.png"));

            irr::scene::ITriangleSelector *l_pSelector = m_pSmgr->createTriangleSelector(m_pStart->getMesh(), m_pStart);
            m_pStart->setTriangleSelector(l_pSelector);
            l_pSelector->drop();

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
  }
}