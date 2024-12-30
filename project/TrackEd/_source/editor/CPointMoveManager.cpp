// (w) 2024 by Dustbin::Games / Christian Keimel
#include <editor/CPointMoveManager.h>

namespace dustbin {
  namespace tracked {
    CPointMoveManager::CPointMoveManager(irr::scene::ISceneCollisionManager *a_pColMgr) : m_eMode(enMoveMode::None), m_pColMgr(a_pColMgr) {
      m_cZero = irr::core::plane3df(irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 1.0f, 0.0f));
    }

    CPointMoveManager::~CPointMoveManager() {
    }

    /**
    * Get the current move mode
    * @return the current move mode
    */
    CPointMoveManager::enMoveMode CPointMoveManager::getMode() {
      return m_eMode;
    }

    /**
    * End the movement, set m_eMode to enMoveMode::None
    */
    void CPointMoveManager::endMovement() {
      m_eMode = enMoveMode::None;
    }

    /**
    * Get the line the point is moved on
    * @return the line the point is moved on
    */
    const irr::core::line3df &CPointMoveManager::getMoveLine() {
      return m_cLine;
    }

    /**
    * Get the position and normal of the plane the point is moved on
    * @param a_cPoint [out] the point of the plane
    * @param a_cNormal [out] the normal of the plane
    */
    void CPointMoveManager::getMovePlane(irr::core::vector3df &a_cPoint, irr::core::vector3df &a_cNormal) {
      a_cPoint  = m_cPlanePoint;
      a_cNormal = m_cPlaneNormal;
    }

    /**
    * Update the point to move
    * @param a_cMouse the mouse position
    * @return the new position of the point to move
    */
    const irr::core::vector3df CPointMoveManager::updateMovement(const irr::core::position2di &a_cMouse) {
      irr::core::vector3df l_cNewPoint;
      
      irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(a_cMouse);

      m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cNewPoint);

      if (m_eMode == enMoveMode::Plane) {
        irr::core::vector3df l_cRet = m_cPointToMove + l_cNewPoint - m_cPointClicked;

        if (l_cRet.Y < 0.0f) {
          l_cRet.Y = 0.0f;
        }

        return l_cRet;
      }
      else if (m_eMode == enMoveMode::Line) {
        irr::core::vector3df l_cRet = m_cPointToMove + (m_cLine.getClosestPoint(l_cNewPoint) - m_cPointClicked);

        if (l_cRet.Y < 0.0f) {
          irr::core::vector3df l_cNew = l_cRet;

          if (m_cZero.getIntersectionWithLimitedLine(m_cLine.start, m_cLine.end, l_cNew)) {
            l_cRet = l_cNew;
          }
        }

        printf("MoveOnLine: %.2f, %.2f, %.2f\n", l_cRet.X, l_cRet.Y, l_cRet.Z);

        return l_cRet;
      }
      else return m_cPointToMove;
    }

    /**
    * Set to "move in plane" mode and initialze the variables
    * @param a_cPoint the point to move
    * @param a_cMouse the position of the mouse
    * @param a_cPlanePoint the point of the plane to move the point on
    * @param a_cPlaneNormal the normal of the plane to move the point on
    */
    void CPointMoveManager::setMoveInPlane(const irr::core::vector3df &a_cPoint, const irr::core::position2di &a_cMouse, const irr::core::vector3df &a_cPlanePoint, const irr::core::vector3df &a_cPlaneNormal) {
      m_cPlane = irr::core::plane3df(a_cPlanePoint, a_cPlaneNormal);

      m_cPointToMove = a_cPoint;

      irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(a_cMouse);
      m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, m_cPointClicked);

      m_cPlanePoint  = a_cPlanePoint;
      m_cPlaneNormal = a_cPlaneNormal;

      m_eMode = enMoveMode::Plane;
    }

    /**
    * Set to "move on line" mode and initialize the variables
    * @param a_cPoint the point to move
    * @param a_cMouse the position of the mouse
    * @param a_cLine the line to move the point on
    * @param a_cPlanePoint the point of the plane necessary for the calculation
    * @param a_cPlaneNormal the normal of the plane necessary for the calculation
    */
    void CPointMoveManager::setMoveOnLine(const irr::core::vector3df &a_cPoint, const irr::core::position2di &a_cMouse, const irr::core::line3df &a_cLine, const irr::core::vector3df &a_cPlanePoint, const irr::core::vector3df &a_cPlaneNormal) {
      m_cPlane = irr::core::plane3df(a_cPlanePoint, a_cPlaneNormal);
      m_cLine  = a_cLine;

      m_cPointToMove = a_cPoint;
      printf("Point: %.2f, %.2f, %.2f\n", m_cPointToMove.X, m_cPointToMove.Y, m_cPointToMove.Z);

      irr::core::vector3df l_cTemp;

      irr::core::line3df l_cRay = m_pColMgr->getRayFromScreenCoordinates(a_cMouse);
      m_cPlane.getIntersectionWithLimitedLine(l_cRay.start, l_cRay.end, l_cTemp);

      m_cPointClicked = m_cLine.getClosestPoint(l_cTemp);

      m_cPlanePoint  = a_cPlanePoint;
      m_cPlaneNormal = a_cPlaneNormal;

      m_eMode = enMoveMode::Line;
    }
  }
}