// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once
#include <editor/ITrackPart.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace tracked {
    /**
    * @class CTrackPart_Bezier
    * @author Christian Keimel
    * The interface for using a Bezier curve for the actual track
    */
    class CTrackPart_Bezier : public ITrackPart {
      private:
        /**
         * Get the next point using the Bezier1 algorithm
         * @param p array with the two points for the algorithm
         * @param fStep step number (0..1)
         * @return the next point
         */
        irr::core::vector3df getBezier1(irr::core::vector3df a_aPoints[], irr::f32 a_fStep);

        /**
         * Get the next point using the Bezier2 algorithm
         * @param p array with the three points (start, help, end) for the algorithm
         * @param fStep step number (0..1)
         * @return the next point
         */
        irr::core::vector3df getBezier2(irr::core::vector3df a_aPoints[], irr::f32 a_fStep);

        /**
         * Get the next point using the Bezier3 algorithm
         * @param p array with the four points (start, help1, help2, end) for the algorithm
         * @param fStep step number (0..1)
         * @return the next point
         */
        irr::core::vector3df getBezier3(irr::core::vector3df a_aPoints[], irr::f32 a_fStep);

      protected:
        std::vector<irr::core::vector3df> m_vPoints;
        std::vector<irr::core::vector3df> m_vTrack;

        /**
        * A little helper functions to fill the indices for the Bezier calculation
        * @param a_iSteps the number of steps
        * @param a_pPrevious the previous track part
        * @return a vector with the indices ranging from 0 to 1
        */
        std::vector<irr::f32> fillIndexVector(int a_iSteps, ITrackPart *a_pPrevious);

        /**
        * Get the point of the bezier curve
        * @param a_vPoints vector with the reference points
        * @param a_fStep step number (0..1)
        * @return the next point
        */
        irr::core::vector3df getBezier(std::vector<irr::core::vector3df> &a_vPoints, irr::f32 a_fStep);

        /**
        * Calculate the points of the curve, road center
        * @return the length of the curve
        */
        irr::f32 calculateCurve();

      public:
        CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_Bezier();

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
        * Get the part's reference points
        * @return a vector with the part's reference points
        */
        virtual const std::vector<irr::core::vector3df> getReferencePoints() override;

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

        /**
        * Does this reference point allow addition of another reference point?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool refPointAdditionAllowed(int a_iIndex) override;

        /**
        * Get the direction of the end of this track part
        */
        virtual const irr::core::vector3df getEndDirection() override;

        /**
        * Get the normal at the end point
        * @return the normal at the end point
        */
        virtual const irr::core::vector3df &getNormal() override;

        /**
        * Add this track part to the AI path node
        * @param a_pAi the AI path node
        * @param a_pLast the last AI path section
        */
        virtual void addToAiNode(scenenodes::CAiPathNode *a_pAi, scenenodes::CAiPathNode::SAiPathSection *a_pLast) override;
    };
  }
}