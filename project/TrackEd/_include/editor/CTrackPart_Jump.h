// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/ITrackPart_Bezier.h>
#include <vector>

namespace dustbin {
  namespace tracked {
    /**
    * @class CTrackPart_Jump
    * @author Christian Keimel
    * This class allows adding jumps to a track
    */
    class CTrackPart_Jump : public ITrackPart_Bezier {
      private:
        std::vector<irr::core::vector3df> m_vTrackPoints[2];  /**< The calculated (center) points of the track */

        std::vector<irr::core::vector3df> m_vNormals;   /**< The normals of the track points */

        irr::core::vector3df m_cSide;   /**< The part's side vector */

      protected:
        /**
        * Get the (normalized) normal vector of a point
        * @param a_iIndex the point index
        * @return the (normalized) normal vector of a point
        */
        virtual irr::core::vector3df getNormalOfPoint(int a_iIndex) const override;

        /**
        * Get the (normalized) side vector of a point
        * @param a_iIndex the point index
        * @return the (normalized) side vector of a point
        */
        virtual irr::core::vector3df getSideOfPoint(int a_iIndex) const override;

      public:
        CTrackPart_Jump(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_Jump();

        /**
        * Create the track part
        * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
        */
        virtual void createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) override;

        /**
        * Set the trak part's reference points
        * @param a_cPoints a vector with the reference points
        */
        virtual void setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) override;

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
        * Does this reference point allow addition of another reference point?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool refPointAdditionAllowed(int a_iIndex) override;

        /**
        * Get the side vector at a specific index of the road
        * @param a_iIndex the index
        * @return the side vector at the index
        */
        virtual irr::core::vector3df getSideAtIndex(int a_iIndex) override;

        /**
        * Get the normal at a specific index of the road
        * @param a_iIndex the index
        * @return the normal at the index
        */
        virtual irr::core::vector3df getNormalAtIndex(int a_iIndex) override;

        /**
        * Get the side vector of the end of this track part
        * @return the side vector of the end of this track part
        */
        virtual irr::core::vector3df getEndSide() const override;

        /**
        * Can a reference point be moved freely in the 2d plane
        * @param a_iIndex index of the reference point
        * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
        * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
        * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
        * @return true if the reference point can move freely, false otherwise
        */
        virtual bool canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) override;

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

        /**
        * Get the information on the route for the marbles
        * @return a vector with positions to pass for the marbles
        */
        virtual std::vector<SAiPath> getAiPathData() override;
    };
  }
}
