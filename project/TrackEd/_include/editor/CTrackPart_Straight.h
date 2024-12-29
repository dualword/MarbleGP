// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/CTrackPart_FlatSegment.h>

namespace dustbin {
  namespace tracked {
    /**
    * @class CTrackPart_Straight
    * @author Christian Keimel
    */
    class CTrackPart_Straight : public CTrackPart_FlatSegment {
      public:
        CTrackPart_Straight(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_Straight();

        /**
        * Does this reference point allow addition of another reference point?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool refPointAdditionAllowed(int a_iIndex) override;
    };
  }
}
