// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/CTrackPart_FlatSegment.h>

namespace dustbin {
  namespace tracked {
    /**
    * @class CTrackPart_Turn
    * @author Christian Keimel
    * This track segment is a turn with a (mostly) fixed angle
    */
    class CTrackPart_Turn : public CTrackPart_FlatSegment {
      public:
        enum class enTurnAngle {
          FourtyFive,
          Ninety,
          OneHundred35,
          OneHundred80
        };

      protected:
        enTurnAngle m_eAngle;   /**< The preset turn angle, will further limit the movement of the reference points */

        bool m_bLeft;   /**< Is this a left turn? */

        irr::f32 m_aDistance[3];    /**< The distances between the reference points */

        /**
        * Calculate the reference points from the distance values
        * @return the new reference points
        */
        std::vector<irr::core::vector3df> calculateReferencePoints();

      public:
        CTrackPart_Turn(CTrackPart_Turn::enTurnAngle a_eAngle, bool a_bLeft, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_Turn();

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
        * This method is called when the reference points of the previous track part have changed
        */
        virtual void updateReferencePoints() override;

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
        * Set the trak part's reference points
        * @param a_cPoints a vector with the reference points
        */
        virtual void setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) override;

        /**
        * Does this reference point allow addition of another reference point?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool refPointAdditionAllowed(int a_iIndex) override;
    };
  }
}
