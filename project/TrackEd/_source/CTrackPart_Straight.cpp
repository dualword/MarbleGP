// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel

#include <editor/CTrackPart_Straight.h>
#include <CDefines.h>

namespace dustbin {
  namespace tracked {
    CTrackPart_Straight::CTrackPart_Straight(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal) : CTrackPart_FlatSegment(a_pPrevious, a_cNormal, c_sTypeStraight) {
    }

    CTrackPart_Straight::~CTrackPart_Straight() {
    }

    /**
    * Does this reference point allow addition of another reference point?
    * @param a_iIndex index of the reference point
    * @return true if so, false otherwise
    */
    bool CTrackPart_Straight::refPointAdditionAllowed(int a_iIndex) {
      // A straight is and remains a straight, so no further reference points are allowed
      return false;
    }
  }
}