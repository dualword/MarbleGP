// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/ITrackPart.h>
#include <commands/ICommand.h>
#include <map>

namespace dustbin {
  namespace tracked {
    class ITrackPart;           /**< Forward declaration of the track part interface */
  }

  namespace commands {
    typedef struct STrackEdit STrackEdit;   /**< Forward declaration of the track editor structure */

    /**
    * @class CCmdEditTrack
    * @author Christian Keimel
    * This is the actual track editor class
    */
    class CCmdEditTrack: public ICommand {
      private:
        irr::scene::ISceneManager  *m_pSmgr;    /**< The Irrlicht scene manager */
        irr::gui::IGUIEnvironment  *m_pGui;     /**< The Irrlicht GUI environment */
        irr::video::IVideoDriver   *m_pDrv;     /**< The Irrlicht video driver */
        irr::scene::ISceneNode     *m_pWorld;   /**< The world scene node */
        irr::scene::IMeshSceneNode *m_pTrack;   /**< The track's scene node */
        irr::scene::SMesh          *m_pMesh;    /**< The track's mesh structure */
        irr::scene::ISceneNode     *m_pPick;    /**< The object used for the next track point */
        irr::scene::IMeshSceneNode *m_pStart;   /**< The start point of the track */
        irr::core::plane3df         m_cPlane;   /**< The plane to pick the next point from */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< Our scene collision manager */

        std::vector<irr::scene::SMeshBuffer *> m_vBuffers;  /**< The mesh buffers of the track */

        irr::core::vector2di m_cDownPoint[3];     /**< The point on the screen where a mouse button was pressed (0 == left, 1 == right, 2 == middle) */

        irr::scene::ISceneNode *m_pHover;   /**< The hovered selector */
        irr::scene::ISceneNode *m_pClick;   /**< The clicked selector */

        bool m_bFinishTrack;    /**< A flag to indicate whether or not the start of the track is hovered */
        bool m_bFinished;       /**< Is the track finished? */

        std::vector<tracked::ITrackPart *> m_vParts;

        std::vector<irr::scene::ISceneNode *> m_vNewPoints;

        /**
        * (Re) create the track
        */
        void createTrack();

        /**
        * Add a new track part
        * @param a_bFinishLap set to "true" if the next part is the start
        */
        void addNewTrackPart(bool a_bFinishLap);

      public:
        CCmdEditTrack(irr::scene::ISceneManager *a_pSmgr, irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CCmdEditTrack();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Let this command draw something (if necessary)
        * @param a_pDrv the video driver
        */
        virtual void draw(irr::video::IVideoDriver *a_pDrv) override;
    };
  }
}
