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
      m_eEditMode   (enEditMode::Default),
      m_pSmgr       (a_pSmgr),
      m_pGui        (a_pGui),
      m_pDrv        (a_pDrv),
      m_pWorld      (nullptr),
      m_pHover      (nullptr),
      m_pSelected   (nullptr),
      m_pPickPoint  (nullptr),
      m_pTrack      (nullptr),
      m_pMesh       (nullptr),
      m_pMenu       (nullptr),
      m_pColMgr     (nullptr),
      m_bFinishTrack(false),
      m_bFinished   (false)
    {
      m_pColMgr  = m_pSmgr->getSceneCollisionManager();

      tracked::CTrackPart_Bezier *l_pStart = new tracked::CTrackPart_Bezier(nullptr, irr::core::vector3df(0.0f, 1.0f, 0.0f), m_pColMgr);
      l_pStart->setReferencePoints({ irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 3.5f) });

      m_vParts.push_back(l_pStart);

      addReferencePoints(l_pStart);

      createTrack();
    }
        
    CCmdEditTrack::~CCmdEditTrack() {
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
            irr::scene::IMeshSceneNode *l_pNode = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"), m_pSmgr->getRootSceneNode(), -1, 50.0f * (l_cPoint + 0.05f * a_pPart->getNormal()));
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

      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/road_back.png"));
      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(1, m_pSmgr->getVideoDriver()->getTexture("data/textures/hexagon_256.png"));
      m_vBuffers[tracked::c_iBufferIdxJoin]->Material.setTexture(2, m_pSmgr->getVideoDriver()->getTexture("data/textures/join_trans.png"));

      m_vBuffers[tracked::c_iBufferIdxBottom]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/warning.png"));

      m_vBuffers[tracked::c_iBufferIdxYellow]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/yellow.png"));
      m_vBuffers[tracked::c_iBufferIdxBlack]->Material.setTexture(0, m_pSmgr->getVideoDriver()->getTexture("data/textures/black.png"));

      if (m_pMesh == nullptr) {
        m_pMesh = new irr::scene::SMesh();
      }

      tracked::ITrackPart *l_pLast = nullptr;

      m_pMesh->MeshBuffers.clear();

      bool l_bBlack = true;

      for (auto l_pPart: m_vParts) {
        printf("Black: %s", l_bBlack ? "true" : "false");
        l_pPart->createTrackPart(m_vBuffers, l_bBlack);
        printf(", %s\n", l_bBlack ? "true" : "false");
      }

      int l_iCount = 0;
      for (auto l_pBuffer : m_vBuffers) {
        if (l_pBuffer->Vertices.size() > 0 && l_pBuffer->Indices.size() > 0) {
          l_pBuffer->recalculateBoundingBox();
          m_pMesh->addMeshBuffer(l_pBuffer);

          irr::video::ITexture *t = l_pBuffer->getMaterial().getTexture(2);

          printf("Add mesh buffer %i: %i, %i (%s)\n", l_iCount, l_pBuffer->Vertices.size(), l_pBuffer->Indices.size(), t != nullptr ? t->getName().getPath().c_str() : "-");
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
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdEditTrack::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (!l_bRet) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          irr::gui::IGUIElement *l_pHover = m_pGui->getRootGUIElement()->getElementFromPoint(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

          irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y), m_pSmgr->getActiveCamera());
          irr::core::vector2di l_cMouse = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

          irr::core::vector3df v;
          irr::core::triangle3df t;

          irr::scene::ISceneNode *l_pObject = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cRay, v, t, 0);

          irr::core::vector3df l_cRayCollision;

          m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cRayCollision);

          if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            m_pHover = nullptr;

            for (auto &l_tPoints: m_mReferencePoints) {
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

            irr::core::vector3df l_cDir = m_vParts.back()->getEndDirection();
            irr::core::vector3df l_cEnd = m_vParts.back()->getEndPosition () * 50.0f;

            if (m_eEditMode == enEditMode::AddReferencePoint) {
              m_pPickPoint->setPosition(l_cRayCollision);
            }

            // if (m_vNewPoints.size() > 0) {
            //   if (l_pObject == m_pStart) {
            //     l_cRayCollision = m_pStart->getAbsolutePosition();
            //     m_bFinishTrack = true;
            //   }
            //   else {
            //     irr::core::vector2df l_cDir2d = irr::core::vector2df(l_cDir.X, l_cDir.Z);
            //     irr::core::vector2df l_cEnd2d = irr::core::vector2df(l_cEnd.X, l_cEnd.Z);
            //     irr::core::vector2df l_cPnt2d = irr::core::vector2df(l_cRayCollision.X, l_cRayCollision.Z);
            // 
            //     irr::core::line2df l_cLine = irr::core::line2df(l_cEnd2d, l_cEnd2d + 1000.0f * l_cDir2d.normalize());
            // 
            //     irr::core::vector2df l_cClose = l_cLine.getClosestPoint(l_cPnt2d);
            // 
            //     if ((l_cClose - l_cPnt2d).getLength() < 10.0f) {
            //       l_cRayCollision.X = l_cClose.X;
            //       l_cRayCollision.Z = l_cClose.Y;
            //     }
            //     else if (std::abs(l_cPnt2d.X) < 2.0f) {
            //       l_cRayCollision.X = 0.0f;
            //     }
            //   }
            // 
            //   m_pPick->setPosition(l_cRayCollision);
            // }
            // else {
            //   irr::core::line3df l_cLine = irr::core::line3df(l_cEnd, l_cEnd + 1000.0f * l_cDir);
            //   m_pPick->setPosition(l_cLine.getClosestPoint(l_cRayCollision));
            // }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_cDownPoint[0] = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
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

                std::vector<irr::core::vector3df> l_vRefPoints = l_pPart->getReferencePoints();
                l_vRefPoints.push_back(l_cRayCollision / 50.0f);
                l_pPart->setReferencePoints(l_vRefPoints);
                addReferencePoints(l_pPart);
                m_pPickPoint->setVisible(false);

                createTrack();

                m_eEditMode = enEditMode::Default;
              }
              else printf("Reference point not found!\n");
            }

            //   m_pPick->updateAbsolutePosition();
            //   irr::scene::ISceneNode *l_pNode = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"), m_pSmgr->getRootSceneNode(), -1, m_pPick->getAbsolutePosition(), irr::core::vector3df(), irr::core::vector3df(0.5f));
            //   l_pNode->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/red.png"));
            //   m_vNewPoints.push_back(l_pNode);
            // 
            //   if (m_vNewPoints.size() >= 3) {
            //     addNewTrackPart(m_bFinishTrack);
            //   }
            // }
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

                    l_vItems.push_back(std::make_tuple(c_sAddSegment, true));
                    l_vItems.push_back(std::make_tuple(c_sAddCheckpoint, true));


                    // l_vItems.push_back(std::make_tuple(c_sAddIncline   , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddDecline   , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddJump      , true));
                    // l_vItems.push_back(std::make_tuple(c_sAddGap       , true));
                    // l_vItems.push_back(std::make_tuple(c_sFinishTrack  , true));

                    tracked::ITrackPart *l_pPart = std::get<0>(m_mReferencePoints[m_pHover]);
                    tracked::CTrackPart_Bezier *l_pPartBezier = dynamic_cast<tracked::CTrackPart_Bezier *>(l_pPart);


                    if (l_pPartBezier != nullptr) {
                      if (l_pPartBezier->getReferencePoints().size() < 4 && l_pPartBezier->getPrevious() != nullptr) {
                        l_vItems.push_back(std::make_tuple(L"----", false));
                        l_vItems.push_back(std::make_tuple(c_sAddReference, true));
                      }
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
                tracked::CTrackPart_Bezier *l_pNew = new tracked::CTrackPart_Bezier(m_vParts.back(), m_vParts.back()->getNormal(), m_pColMgr);

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
                tracked::CTrackPart_Checkpoint *l_pNew = new tracked::CTrackPart_Checkpoint(m_vParts.back(), m_vParts.back()->getNormal(), m_pColMgr);
                
                irr::core::vector3df l_cDirection = m_vParts.back()->getEndDirection();
                
                std::vector<irr::core::vector3df> l_vRefPoints = {
                  m_vParts.back()->getReferencePoints().back(),
                  m_vParts.back()->getReferencePoints().back() + 0.5f * l_cDirection.normalize()
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
                      m_eEditMode = enEditMode::AddReferencePoint;

                      for (auto &l_tPoint: m_mReferencePoints) {
                        l_tPoint.first->setVisible(m_pSelected == l_tPoint.first);
                      }

                      m_pPickPoint->setVisible(true);
                    }
                  }
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

            m_pPickPoint = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh("data/objects/sphere.obj"));
            m_pPickPoint->getMaterial(0).setTexture(0, m_pDrv->getTexture("data/textures/yellow.png"));
            m_pPickPoint->setVisible(false);

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