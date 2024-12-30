// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace tracked {
    /**
    * @class CPointMoveManager
    * @author Christian Keimel
    * This class moves a point according to user mouse input
    */
    class CPointMoveManager {
      public:
        enum class enMoveMode {
          None,     /**< No movement active */
          Plane,    /**< Point is moved on plane */
          Line      /**< Point is moved on line */
        };

      private:
        enMoveMode m_eMode;   /**< The move mode */

        irr::core::plane3df m_cPlane;   /**< The plane to move the point on (if "m_eMode" is enMoveMode::Plane) */
        irr::core::plane3df m_cZero;    /**< The zero plane, no point may be moved below Y == 0! */
        irr::core::line3df  m_cLine;    /**< The line to move the point on (if "m_eMode" is enMoveMode::Line) */

        irr::core::vector3df m_cPlanePoint;     /**< The point of the plane */
        irr::core::vector3df m_cPlaneNormal;    /**< the normal of the plane  */

        irr::core::vector3df m_cPointToMove;    /**< The point to move */
        irr::core::vector3df m_cPointClicked;   /**< The point in the plane or on the line which was clicked */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< The Irrlicht scene collision manager */

      public:
        CPointMoveManager(irr::scene::ISceneCollisionManager *a_pColMgr);
        virtual ~CPointMoveManager();

        /**
        * Get the current move mode
        * @return the current move mode
        */
        enMoveMode getMode() const;

        /**
        * End the movement, set m_eMode to enMoveMode::None
        */
        void endMovement();

        /**
        * Update the point to move
        * @param a_cMouse the mouse position
        * @return the new position of the point to move
        */
        const irr::core::vector3df updateMovement(const irr::core::position2di &a_cMouse);

        /**
        * Get the line the point is moved on
        * @return the line the point is moved on
        */
        const irr::core::line3df &getMoveLine() const;

        /**
        * Get the position and normal of the plane the point is moved on
        * @param a_cPoint [out] the point of the plane
        * @param a_cNormal [out] the normal of the plane
        */
        void getMovePlane(irr::core::vector3df &a_cPoint, irr::core::vector3df &a_cNormal);

        /**
        * Set to "move in plane" mode and initialze the variables
        * @param a_cPoint the point to move
        * @param a_cMouse the position of the mouse
        * @param a_cPlanePoint the point of the plane to move the point on
        * @param a_cPlaneNormal the normal of the plane to move the point on
        */
        void setMoveInPlane(const irr::core::vector3df &a_cPoint, const irr::core::position2di &a_cMouse, const irr::core::vector3df &a_cPlanePoint, const irr::core::vector3df &a_cPlaneNormal);

        /**
        * Set to "move in plane" mode and initialze the variables for adding a reference point
        * @param a_cMouse the position of the mouse
        * @param a_cPlanePoint the point of the plane to move the point on
        * @param a_cPlaneNormal the normal of the plane to move the point on
        */
        void setMoveInPlane(const irr::core::position2di &a_cMouse, const irr::core::vector3df &a_cPlanePoint, const irr::core::vector3df &a_cPlaneNormal);

        /**
        * Set to "move on line" mode and initialize the variables
        * @param a_cPoint the point to move
        * @param a_cMouse the position of the mouse
        * @param a_cLine the line to move the point on
        * @param a_cPlanePoint the point of the plane necessary for the calculation
        * @param a_cPlaneNormal the normal of the plane necessary for the calculation
        */
        void setMoveOnLine(const irr::core::vector3df &a_cPoint, const irr::core::position2di &a_cMouse, const irr::core::line3df &a_cLine, const irr::core::vector3df &a_cPlanePoint, const irr::core::vector3df &a_cPlaneNormal);
    };
  }
}
