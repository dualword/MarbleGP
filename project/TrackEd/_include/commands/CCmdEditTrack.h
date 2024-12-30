// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/ITrackPart.h>
#include <commands/ICommand.h>
#include <map>

namespace dustbin {
  namespace tracked {
    class ITrackPart;           /**< Forward declaration of the track part interface */
    class CAiPath;              /**< Forward declaration of the AI path object */
    class CPointMoveManager;    /**< Forward declaration of the point move manager */
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
        irr::core::vector3df        m_cMovePoint;   /**< The point which was clicked for moving */

        tracked::CPointMoveManager *m_pMoveMgr;     /**< The point move manager */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< Our scene collision manager */

        std::vector<irr::scene::SMeshBuffer *> m_vBuffers;  /**< The mesh buffers of the track */

        irr::core::vector2di m_cDownPoint[3];     /**< The point on the screen where a mouse button was pressed (0 == left, 1 == right, 2 == middle) */

        irr::scene::ISceneNode *m_pClick;   /**< The clicked selector */

        CCmdMainMenu *m_pMainMenu;    /**< The main menu */

        bool m_bFinishTrack;    /**< A flag to indicate whether or not the start of the track is hovered */
        bool m_bFinished;       /**< Is the track finished? */

        irr::f32 m_fBaseOffset;   /**< Offset of the base to the road */
        irr::f32 m_fBaseHeight;   /**< Height of the side walls of the base */

        std::string m_sTrackName;     /**< The name of the track */
        std::string m_sTrackUUID;     /**< The UUID of the track */

        std::vector<tracked::ITrackPart *> m_vParts;    /**< List of all track parts */

        std::map<irr::scene::ISceneNode *, std::tuple<tracked::ITrackPart *, int>> m_mReferencePoints;    /**< A map mapping the reference points to the track part (0) and it's point index (1) */

        std::map<irr::u32, irr::u32> m_mBufferMap;  /**< A map mapping the defined buffer IDs to the actual IDs of the track mesh */

        irr::core::vector3df m_cThumbnailPos;   /**< Position of the thumbnail camera */
        irr::core::vector3df m_cThumbnailTgt;   /**< Target of the thumbnail camera */

        tracked::CAiPath *m_pAiPath;

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
        * Make sure the track has an UUID assigned
        */
        void ensureUUID();

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

        /**
        * Set the data of the thumbnail camera
        * @param a_cPos the new position of the thumbnail camera
        * @param a_cTgt the new target of the thumbnail camera
        */
        void setThumbnailCamera(const irr::core::vector3df &a_cPos, const irr::core::vector3df &a_cTgt);

        /**
        * Get the thumbnail camera parameters
        * @param a_cPos [out] position of the thumbnail camera
        * @param a_cTgt [out] target of the thumbnail camera
        */
        void getThumbnailCamera(irr::core::vector3df &a_cPos, irr::core::vector3df &a_cTgt) const;

        /**
        * Save the track information
        * @param a_cAttr the IAttributes to save the information to
        */
        void saveTrackInformation(irr::io::IAttributes &a_cAttr);

        /**
        * Load the track information
        * @param a_cAttr the IAttributes to load the information from
        */
        void loadTrackInformation(irr::io::IAttributes &a_cAttr);

        /**
        * Change the name of the track
        * @param a_sName the new name of the track
        */
        void setTrackName(const std::string &a_sName);

        /**
        * Get the name of the track
        * @return the name of the track
        */
        const std::string &getTrackName();

        /**
        * Get the track's UUID used as name for the export folder
        * @return the track's UUID
        */
        const std::string &getUUID();

        /**
        * Get the main menu
        * @return the main menu
        */
        CCmdMainMenu *getMainMenu();
    };
  }
}
