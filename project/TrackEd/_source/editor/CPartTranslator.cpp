// (w) 2025 by Dustbin::Games / Christian Keimel
#include <editor/CPointMoveManager.h>
#include <helpers/CTrackEdHelpers.h>
#include <editor/CPartTranslator.h>

#include <string>
#include <tuple>

namespace dustbin {
  namespace tracked {
    CPartTranslator::CPartTranslator(irr::scene::ISceneManager *a_pSmgr) : 
      m_pSmgr     (nullptr),
      m_pNode     (nullptr),
      m_pSelector (nullptr),
      m_pHover    (nullptr),
      m_pMove     (nullptr),
      m_pOriginal (nullptr),
      m_pDragging (nullptr),
      m_pMoveMgr  (nullptr),
      m_iMove     (-1),
      m_bMouseDown(false),
      m_bVisible  (false)
    {
      m_pSmgr = a_pSmgr->createNewSceneManager(false);
      m_pSmgr->addCameraSceneNode();

      m_pColMgr = m_pSmgr->getSceneCollisionManager();

      m_pMoveMgr = new CPointMoveManager(m_pColMgr);
      m_pMoveMgr->restrictMovement(false);

      m_pDragging = m_pSmgr->getVideoDriver()->getTexture("data/textures/editor_yellow.png");

      // Add the scene nodes for the part translation
      std::tuple<std::string, std::string, irr::core::vector3df> l_aArrowData[3];

      l_aArrowData[0] = std::make_tuple("data/objects/editor_arrow.3ds", "data/textures/editor_red.png"  , irr::core::vector3df(1.0f,  0.0f, 0.0f));
      l_aArrowData[1] = std::make_tuple("data/objects/editor_arrow.3ds", "data/textures/editor_blue.png" , irr::core::vector3df(0.0f, -1.0f, 0.0f));
      l_aArrowData[2] = std::make_tuple("data/objects/editor_arrow.3ds", "data/textures/editor_green.png", irr::core::vector3df(0.0f,  0.0f, 1.0f));

      m_pSelector = m_pSmgr->createMetaTriangleSelector();

      for (int i = 0; i < 3; i++) {
        irr::scene::IMeshSceneNode *l_pNode = m_pSmgr->addMeshSceneNode(m_pSmgr->getMesh(std::get<0>(l_aArrowData[i]).c_str()));
        m_aArrows[i] = l_pNode;
        m_aArrows[i]->setScale(irr::core::vector3df(10.0f));
        m_aArrows[i]->getMaterial(0).Lighting = false;
        m_aArrows[i]->getMaterial(0).setTexture(0, m_pSmgr->getVideoDriver()->getTexture(std::get<1>(l_aArrowData[i]).c_str()));
        m_aArrows[i]->getMaterial(0).MaterialType = irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;

        irr::scene::ITriangleSelector *l_pSelector = m_pSmgr->createTriangleSelector(l_pNode->getMesh(), l_pNode);
        m_aArrows[i]->setTriangleSelector(l_pSelector);
        m_pSelector->addTriangleSelector(l_pSelector);

        irr::core::vector3df r = std::get<2>(l_aArrowData[i]).getHorizontalAngle();
        m_aArrows[i]->setRotation(r);
      }
    }

    CPartTranslator::~CPartTranslator() {
      m_pSmgr->drop();

      delete m_pMoveMgr;
    }

    /**
    * Set the node to edit
    * @param a_pNode the node to edit
    */
    void CPartTranslator::setEditNode(irr::scene::ISceneNode *a_pNode) {
      // We don't edit the root scene node of the scene
      if (a_pNode != nullptr && a_pNode != a_pNode->getSceneManager()->getRootSceneNode()) {
        m_pNode = a_pNode;
        m_bVisible = true;

        m_cParentMtx = m_pNode->getParent()->getAbsoluteTransformation(); 
        m_cInveseMtx = m_pNode->getParent()->getAbsoluteTransformation();

        m_cInveseMtx.makeInverse();

        m_pNode->updateAbsolutePosition();
        updateMovePositions();
      }
      else {
        m_pNode = nullptr;
        m_bVisible = false;
      }
    }

    /**
    * Update the position of the move elements
    */
    void CPartTranslator::updateMovePositions() {
      irr::core::vector3df l_cDirections[] = {
        irr::core::vector3df(-1.0f,  0.0f,  0.0f),
        irr::core::vector3df( 0.0f, -1.0f,  0.0f),
        irr::core::vector3df( 0.0f,  0.0f, -1.0f)
      };

      for (int i = 0; i < 3; i++) {
        m_cParentMtx.rotateVect(l_cDirections[i]);
        m_aArrows[i]->setRotation(l_cDirections[i].getHorizontalAngle());
        m_aArrows[i]->setPosition(m_pNode->getAbsolutePosition());
      }
    }

    bool CPartTranslator::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      // Only do something if a node is set, i.e. we are visible
      if (m_bVisible) {
        if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_bMouseDown = true;

            m_iMove = -1;
            if (m_pHover != nullptr) {
              for (int i = 0; i < 3; i++) {
                if (m_aArrows[i] == m_pHover) {
                  m_iMove = i;
                  break;
                }
              }

              if (m_iMove != -1) {
                m_pMove = m_aArrows[m_iMove];

                m_pOriginal = m_aArrows[m_iMove]->getMaterial(0).getTexture(0);
                m_aArrows[m_iMove]->getMaterial(0).setTexture(0, m_pDragging);

                irr::core::vector3df l_aDirections[] = {
                  irr::core::vector3df(-1.0f,  0.0f,  0.0f),
                  irr::core::vector3df( 0.0f, -1.0f,  0.0f),
                  irr::core::vector3df( 0.0f,  0.0f, -1.0f)
                };

                irr::core::vector3df l_aNormals[] = {
                  irr::core::vector3df(0.0f, 1.0f, 0.0f),
                  irr::core::vector3df(0.0f, 0.0f, 1.0f),
                  irr::core::vector3df(1.0f, 0.0f, 0.0f)
                };

                m_cParentMtx.rotateVect(l_aDirections[m_iMove]);
                m_cParentMtx.rotateVect(l_aNormals   [m_iMove]);

                irr::core::vector3df l_cPos = m_pHover->getAbsolutePosition();

                m_pMoveMgr->setMoveOnLine(
                  l_cPos,
                  irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y),
                  irr::core::line3df(l_cPos - 1000.0f * l_aDirections[m_iMove], l_cPos + 1000.0f * l_aDirections[m_iMove]),
                  l_cPos,
                  l_aNormals[m_iMove]
                );
                l_bRet = true;
              }
              else m_pMove = nullptr;
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            m_bMouseDown = false;

            if (m_pMoveMgr->getMode() != CPointMoveManager::enMoveMode::None) {
              m_pMoveMgr->endMovement();
              l_bRet = true;
            }

            if (m_pMove != nullptr && m_pOriginal != nullptr) {
              m_pMove->getMaterial(0).setTexture(0, m_pOriginal);
              m_pMove = nullptr;
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            if (!m_bMouseDown) {
              irr::core::vector2di l_cMouse = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
              irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(l_cMouse, m_pSmgr->getActiveCamera());
              irr::core::triangle3df l_cTriangle;

              m_pHover = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cRay, m_cPoint, l_cTriangle);

              for (int i = 0; i < 3; i++) {
                m_aArrows[i]->getMaterial(0).MaterialType = m_pHover == m_aArrows[i] ? irr::video::EMT_SOLID : irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL;
              }
            }
            else {
              if (m_pMoveMgr->getMode() == CPointMoveManager::enMoveMode::Line) {
                irr::core::vector3df l_cOldPos = m_pNode->getPosition();
                irr::core::vector3df l_cNewPos = m_pMoveMgr->updateMovement(irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y));

                m_cInveseMtx.transformVect(l_cNewPos);
                m_pNode->setPosition(irr::core::vector3df(
                  m_iMove == 0 ? l_cNewPos.X : l_cOldPos.X,
                  m_iMove == 1 ? l_cNewPos.Y : l_cOldPos.Y,
                  m_iMove == 2 ? l_cNewPos.Z : l_cOldPos.Z
                ));
                m_pNode->updateAbsolutePosition();
                updateMovePositions();

                l_bRet = true;
              }
            }
          }
        }
      }

      return l_bRet;
    }

    void CPartTranslator::draw(irr::video::IVideoDriver *a_pDrv, irr::scene::ICameraSceneNode *a_pCamera) {
      if (m_bVisible) {
        if (m_pMoveMgr->getMode() == CPointMoveManager::enMoveMode::Line) {
          irr::core::vector3df l_cPoint;
          irr::core::vector3df l_cNormal;

          m_pMoveMgr->getMovePlane(l_cPoint, l_cNormal);
          helpers::drawPlane3df(a_pDrv, l_cPoint, l_cNormal);

          a_pDrv->draw3DLine(m_pMoveMgr->getMoveLine().start, m_pMoveMgr->getMoveLine().end);
        }

        m_pSmgr->getActiveCamera()->setPosition(a_pCamera->getAbsolutePosition());
        m_pSmgr->getActiveCamera()->setTarget  (a_pCamera->getTarget          ());

        a_pDrv->clearZBuffer();
        m_pSmgr->drawAll();
      }
    }
  }
}
