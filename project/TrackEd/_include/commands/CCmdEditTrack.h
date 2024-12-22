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

    class CCmdMainMenu;   /**< Forward declaration of the main menu */

    /**
    * @class CCmdEditTrack
    * @author Christian Keimel
    * This is the actual track editor class
    */
    class CCmdEditTrack: public ICommand {
      private:
        enum class enEditMode {
          Default,
          AddReferencePoint,
          MovePointOnLine,
          MovePointInPlane
        };

        enEditMode m_eEditMode;   /**< The current edit mode */

        irr::scene::ISceneManager  *m_pSmgr;        /**< The Irrlicht scene manager */
        irr::gui::IGUIEnvironment  *m_pGui;         /**< The Irrlicht GUI environment */
        irr::video::IVideoDriver   *m_pDrv;         /**< The Irrlicht video driver */
        irr::scene::ISceneNode     *m_pWorld;       /**< The world scene node */
        irr::scene::ISceneNode     *m_pHover;       /**< The hovered editor scene node */
        irr::scene::ISceneNode     *m_pSelected;    /**< The hovered editor scene node */
        irr::scene::ISceneNode     *m_pPickPoint;   /**< The scene node to add a point to a track part */
        irr::scene::IMeshSceneNode *m_pTrack;       /**< The track's scene node */
        irr::scene::IMeshSceneNode *m_pZeroPlane;   /**< The plane indicating zero Y */
        irr::scene::SMesh          *m_pMesh;        /**< The track's mesh structure */
        irr::core::line3df          m_cMoveLine;    /**< The line to move points along */
        irr::core::plane3df         m_cMovePlane;   /**< The plane to move points in */
        irr::core::plane3df         m_cZeroPlane;   /**< The plane at Y zero to prevent reference points being set below 0 */
        irr::core::vector3df        m_cMovePoint;   /**< The point which was clicked for moving */

        irr::gui::IGUIContextMenu *m_pMenu;       /**< The context menu */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< Our scene collision manager */

        std::vector<irr::scene::SMeshBuffer *> m_vBuffers;  /**< The mesh buffers of the track */

        irr::core::vector2di m_cDownPoint[3];     /**< The point on the screen where a mouse button was pressed (0 == left, 1 == right, 2 == middle) */

        irr::scene::ISceneNode *m_pClick;   /**< The clicked selector */

        CCmdMainMenu *m_pMainMenu;    /**< The main menu */

        bool m_bFinishTrack;    /**< A flag to indicate whether or not the start of the track is hovered */
        bool m_bFinished;       /**< Is the track finished? */

        irr::f32 m_fBaseOffset;   /**< Offset of the base to the road */
        irr::f32 m_fBaseHeight;   /**< Height of the side walls of the base */

        std::vector<tracked::ITrackPart *> m_vParts;    /**< List of all track parts */

        std::map<irr::scene::ISceneNode *, std::tuple<tracked::ITrackPart *, int>> m_mReferencePoints;    /**< A map mapping the reference points to the track part (0) and it's point index (1) */

        std::map<irr::u32, irr::u32> m_mBufferMap;  /**< A map mapping the defined buffer IDs to the actual IDs of the track mesh */

        /**
        * Build the basement of the track (only done if m_fBaseOffset and m_fBaseHeight are greater than zero)
        */
        void createBasement();

        /**
        * Add the editable reference points of a track part
        * @param a_pPart the track part
        */
        void addReferencePoints(tracked::ITrackPart *a_pPart);

        /**
        * Set the edit mode and update the info text
        * @param a_eEditMode the new edit mode
        */
        void setEditMode(enEditMode a_eMode);

        /**
        * Get the correct collision point considering the move and the zero plane
        * @param a_cMousePos the mouse position to calculate the ray from
        * @return the ray collision point
        * @see m_cMovePlane
        * @see m_cZeroPlane
        */
        irr::core::vector3df getRayPlaneCollision(const irr::core::vector2di &a_cMousePos);

      public:
        CCmdEditTrack(irr::scene::ISceneManager *a_pSmgr, irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui, CCmdMainMenu *a_pMainMenu);
        virtual ~CCmdEditTrack();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
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

        /**
        * Set the parameters for the track base
        * @param a_fOffset the offset of the walls
        * @param a_fHeight the height of the walls
        */
        void setBaseParameters(irr::f32 a_fOffset, irr::f32 a_fHeight);

        /**
        * Update the reference points after the track was modified
        */
        void updateReferencePoints();

        /**
        * Clear the reference points before loading a track
        */
        void clearReferencePoints();

        /**
        * Get the track defining vector
        * @return the track defining vector
        */
        const std::vector<tracked::ITrackPart *> &getTrack();

        /**
        * (Re) create the track
        */
        void createTrack();

        /**
        * Clear the track
        */
        void clearTrack();

        /**
        * Add a track part
        * @param a_pPart the new track part
        */
        void addTrackPart(tracked::ITrackPart *a_pPart);

        /**
        * Get the track mesh
        * @return the track mesh
        */
        irr::scene::IMeshSceneNode *getTrackMesh();

        /**
        * Get the index of a mesh buffer (mesh buffer indices found in "tracked::c_iBufferIdx..."
        * @param a_iBuffer the buffer index
        * @return the index of the buffer in the track mesh, 0xFFFF if the buffer was not added
        */
        irr::u32 getBufferIndex(irr::u32 a_iBuffer);
    };
  }
}
