// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace gui {
    class IProgressCallback;
  }

  namespace gameclasses {
    /**
    * @class COfftrackDetector
    * @author Christian Keimel
    * This class is used to detect out-of-track marbles. This is done using two methods:
    * 1. If the Y-Coordinate is too low it's off track
    * 2. A raster of on-track sectors is defined and used (raster size: 10.0 x 10.0)
    */
    class COfftrackDetector {
      private:
        irr::core::vector2df m_cMin;    /**< The minimum X and Z coordinates */
        irr::core::vector2df m_cMax;    /**< The maximum X and Z coordinates */
        irr::f32             m_fMinY;   /**< The mimimum Y coordinate */

        irr::core::vector2di m_cSectors;  /**< The number of sectors for X and Z */

        irr::scene::ISceneManager *m_pSmgr;

        std::vector<std::vector<bool>> m_vSectors;

        /**
        * Iterate the scene nodes to get the bounding box
        * @param a_cNode the node to iterate
        * @param a_pSelector the triangle selector to be filled
        * @return the bounding box
        */
        void getBoundingBox(irr::scene::ISceneNode* a_pNode, irr::core::aabbox3df *a_pBox, irr::scene::IMetaTriangleSelector *a_pSelector, int &a_iSelectorCount);

        /**
        * Calculate the respawn areas for the track
        */
        void calculateRespawnAreas(gui::IProgressCallback *a_pProgress);

    public:
        COfftrackDetector(irr::scene::ISceneManager *a_pSmgr, gui::IProgressCallback *a_pProgress);
        ~COfftrackDetector();

        /**
        * Check whether or not a point is on the track
        * @param a_cPos the point to verify
        * @return true if the point is on the track, false otherwise
        */
        bool isOnTrack(const irr::core::vector3df &a_cPos);
    };
  }
}
