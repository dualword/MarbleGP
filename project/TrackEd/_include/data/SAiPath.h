// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace tracked {
    class ITrackPart;   /**< Forward declaration of the track part class */

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

    /**
    * @class SAiPath
    * @author Christian Keimel
    * Data for the AI path
    */
    typedef struct SAiPath {
      enPathType m_eType;       /**< The type of this path segment */

      bool m_bCalMinVel;    /**< Does this segment require minimum speed calculation? */
    
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
        m_bCalMinVel  (false),
        m_fWidth      (0.0f),
        m_fJumpMinVel (0.0f),
        m_fJumpMaxVel (0.0f),
        m_fJumpBestVel(0.0f),
        m_pPart       (nullptr)
      {
      }

      SAiPath(enPathType a_eType, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cNormal, const irr::core::vector3df &a_cSide, irr::f32 a_fWidth, tracked::ITrackPart *a_pPart) :
        m_eType       (a_eType    ),
        m_bCalMinVel  (false      ),
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
        m_bCalMinVel  (a_cOther.m_bCalMinVel  ),
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
  }
}