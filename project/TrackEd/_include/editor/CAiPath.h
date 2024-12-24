// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <vector>

namespace dustbin {
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

          SAiPath() : 
            m_eType       (enPathType::None),
            m_fWidth      (0.0f),
            m_fJumpMinVel (0.0f),
            m_fJumpMaxVel (0.0f),
            m_fJumpBestVel(0.0f)
          {
          }

          SAiPath(enPathType a_eType, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cNormal, const irr::core::vector3df &a_cSide, irr::f32 a_fWidth) :
            m_eType       (a_eType    ),
            m_cPosition   (a_cPosition),
            m_cNormal     (a_cNormal  ),
            m_cSide       (a_cSide    ),
            m_fWidth      (a_fWidth   ),
            m_fJumpMinVel (0.0f),
            m_fJumpMaxVel (0.0f),
            m_fJumpBestVel(0.0f)
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
            m_fJumpBestVel(a_cOther.m_fJumpBestVel)
          {
          }
        }
        SAiPath;

        std::vector<SAiPath> m_vPath;    /**< The AI path */

        std::vector<std::vector<irr::core::vector3df>> m_vJumpData;

      public:
        CAiPath(tracked::ITrackPart *a_pTrack);
        virtual ~CAiPath();

        /**
        * Get the number of AI path segments
        * @return the number of AI path segments
        */
        size_t getCount();

        /**
        * Get the type of a path segment
        * @param a_iIndex the index
        * @return the type of a path segment
        */
        enPathType getTypeOfSegment(size_t a_iIndex);

        /**
        * Get the end position of a segment
        * @param a_iIndex the index
        * @return the end position of a segment
        */
        const irr::core::vector3df getSegmentPosition(size_t a_iIndex);

        /**
        * Get the normal of a segment
        * @param a_iIndex the index
        * @return the normal of a segment
        */
        const irr::core::vector3df getSegmentNormal(size_t a_iIndex);

        /**
        * Get the side vector of a segment
        * @param a_iIndex the index
        * @return the side vector of a segment
        */
        const irr::core::vector3df getSegmentSideVector(size_t a_iIndex);

        /**
        * Get the width of a segment
        * @param a_iIndex the index
        * @return the width of a segment
        */
        irr::f32 getSegmentWidth(size_t a_iIndex);

        /**
        * Get the jump parameters of a segment
        * @param a_iIndex the index
        * @param a_fMinVel [out] the minimum velocity for the jump
        * @param a_fMaxVel [out] the maximum velocity for the jump
        * @param a_fBestVel [out] the best velocity for the jump
        * @return true if the segment is a jump, false otherwise
        */
        bool getJumpParameters(size_t a_iIndex, irr::f32 &a_fMinVel, irr::f32 &a_fMaxVel, irr::f32 &a_fBestVel);

        /**
        * Draw debug data
        * @param a_pDrv the video driver
        * @param a_pGui the GUI environment
        */
        void drawDebugData(irr::video::IVideoDriver *a_pDrv, irr::gui::IGUIEnvironment *a_pGui);
    };
  }
}
