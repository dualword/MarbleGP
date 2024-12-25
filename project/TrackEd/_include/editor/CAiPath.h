// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

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
      public:
        /**
        * The type of the segments
        */
        enum class enPathType {
          None,                 /**< Not a segment (e.g. index out of range) */
          Standard,             /**< Just a normal segment */
          Incline,              /**< An upward segment */
          Decline,              /**< A downward segment */
          Jump,                 /**< A jump */
          JumpArea              /**< Markes how far a marble can jump safely after a jump was detected */
        };

      private:
        /**
        * @class SAiPath
        * @author Christian Keimel
        * Data for the AI path
        */
        typedef struct SAiPath {
          enPathType m_eType;

          irr::core::vector3df m_cPosition;   /**< The (end) position of the segment */
          irr::core::vector3df m_cNormal;     /**< The normal of the segment */
          irr::core::vector3df m_cSide;       /**< The side vector of the segment */

          irr::f32 m_fWidth;        /**< The width of the segment */
          irr::f32 m_fJumpMinVel;   /**< The maximal velocity if this is a jump */
          irr::f32 m_fJumpMaxVel;   /**< The maximal velocity if this is a jump */
          irr::f32 m_fJumpBestVel;  /**< The best velocity if this is a jump */

          ITrackPart *m_pPart;    /**< The track part this path section comes from */

          SAiPath() : 
            m_eType       (enPathType::None),
            m_fWidth      (0.0f),
            m_fJumpMinVel (0.0f),
            m_fJumpMaxVel (0.0f),
            m_fJumpBestVel(0.0f),
            m_pPart       (nullptr)
          {
          }

          SAiPath(enPathType a_eType, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cNormal, const irr::core::vector3df &a_cSide, irr::f32 a_fWidth, tracked::ITrackPart *a_pPart) :
            m_eType       (a_eType    ),
            m_cPosition   (a_cPosition),
            m_cNormal     (a_cNormal  ),
            m_cSide       (a_cSide    ),
            m_fWidth      (a_fWidth   ),
            m_fJumpMinVel (0.0f),
            m_fJumpMaxVel (0.0f),
            m_fJumpBestVel(0.0f),
            m_pPart       (a_pPart)
          {
          }
          
          SAiPath(const SAiPath &a_cOther) : 
            m_eType       (a_cOther.m_eType       ),
            m_cPosition   (a_cOther.m_cPosition   ),
            m_cNormal     (a_cOther.m_cNormal     ),
            m_cSide       (a_cOther.m_cSide       ),
            m_fWidth      (a_cOther.m_fWidth      ),
            m_fJumpMinVel (a_cOther.m_fJumpMinVel ),
            m_fJumpMaxVel (a_cOther.m_fJumpMaxVel ),
            m_fJumpBestVel(a_cOther.m_fJumpBestVel),
            m_pPart       (a_cOther.m_pPart       )
          {
          }
        }
        SAiPath;

        std::vector<SAiPath> m_vPath;    /**< The AI path */

        std::vector<std::vector<irr::core::vector3df>> m_vJumpData;   /**< Debug data for the jumps */

        std::vector<irr::scene::ISceneNode *> m_vSpeedIndicators;   /**< The list of speed indicators */

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
