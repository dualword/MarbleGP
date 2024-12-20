// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once
#include <editor/CTrackPart_Bezier.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace tracked {
    /**
    * @class CTrackPart_Incline
    * @author Christian Keimel
    * A track part to go up
    */
    class CTrackPart_StartRamp : public CTrackPart_Bezier {
      public:
        CTrackPart_StartRamp(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_StartRamp();

        /**
        * Can a reference point be moved freely in the 2d plane
        * @param a_iIndex index of the reference point
        * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
        * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
        * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in. Only used if "true" is returned.
        * @return true if the reference point can move freely, false otherwise
        */
        virtual bool canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) override;

        /**
        * Serialize the track part
        * @param a_cAttr the attributes to serialize the data to
        */
        virtual void serialize(irr::io::IAttributes &a_cAttr) const override;

        /**
        * De-serialize the track part
        * @param a_cAttr the attributes to read the data from
        */
        virtual void deserialize(const irr::io::IAttributes &a_cAttr) override;

        /**
        * This method is called when the reference points of the previous track part have changed
        */
        virtual void updateReferencePoints() override;

        /**
        * Does this reference point allow addition of new track parts?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool partAdditionAllowed(int a_iIndex) override;
    };
  }
}