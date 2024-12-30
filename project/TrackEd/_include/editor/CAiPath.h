// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/SAiPath.h>
#include <irrlicht.h>
#include <vector>

namespace dustbin {
  namespace scenenodes {
    class CAiPathNode;  /**< Foward declaration of the AI path scene node */
  }

  namespace tracked {
    class ITrackPart;   /**< Forward declaration of the track part */

    /**
    * @class CAiPath
    * @author Christian Keimel
    * This class is used to store and analyse the AI path
    */
    class CAiPath {
      private:

        std::vector<SAiPath> m_vPath;    /**< The AI path */

        std::vector<std::vector<irr::core::vector3df>> m_vJumpData;   /**< Debug data for the jumps */

        std::vector<irr::scene::ISceneNode *> m_vSpeedIndicators;   /**< The list of speed indicators */

        bool m_bLapFinished;    /**< Is the lap finished or does it have open ends? */

      public:
        CAiPath(tracked::ITrackPart *a_pTrack);
        virtual ~CAiPath();

        /**
        * Draw debug data
        * @param a_pDrv the video driver
        * @param a_pGui the GUI environment
        */
        void drawDebugData(irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui);

        /**
        * Add the AI data to the scene node storing the data
        * @param a_pAi the scene node to add the data to
        */
        void addAiToNode(scenenodes::CAiPathNode *a_pAi);

        /**
        * Add and initialize the speed indicators before jumps
        * @param a_pWorld the parent node to add the indicators to
        * @param a_pSmgr the Irrlicht scene manager
        */
        void addSpeedIndicators(irr::scene::ISceneNode *a_pWorld, irr::scene::ISceneManager *a_pSmgr);
    };
  }
}
