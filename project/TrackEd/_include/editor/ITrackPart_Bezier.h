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
    class ITrackPart_Bezier : public ITrackPart {
      protected:
        int m_iRoadBufferIdx;   /**< The index of the mesh buffer for the road */

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

        /**
        * Get the (normalized) normal vector of a point
        * @param a_iIndex the point index
        * @return the (normalized) normal vector of a point
        */
        virtual irr::core::vector3df getNormalOfPoint(int a_iIndex) const = 0;

        /**
        * Get the (normalized) side vector of a point
        * @param a_iIndex the point index
        * @return the (normalized) side vector of a point
        */
        virtual irr::core::vector3df getSideOfPoint(int a_iIndex) const = 0;

        std::vector<irr::core::vector3df> m_vPoints;    /**< The reference points used for the track calculation */
        std::vector<irr::core::vector3df> m_vTrack;     /**< The calculated center of the track */

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
        virtual irr::core::vector3df getBezier(std::vector<irr::core::vector3df> &a_vPoints, irr::f32 a_fStep);

        /**
        * Calculate the points of the curve, road center
        * @param a_vPoints optional vector with the reference points used for calculation. If empty "m_vPoints" will be used
        * @param a_pTrack optional pointer to a vector which will be filled with the track (if not nullptr, otherwise m_vTrack is filled)
        * @param a_bFillAll boolean parameter to override skipping the first bezier item
        * @return the length of the curve
        */
        irr::f32 calculateCurve(const std::vector<irr::core::vector3df> &a_vPoints = std::vector<irr::core::vector3df>(), std::vector<irr::core::vector3df> *a_pTrack = nullptr, bool a_bFillAll = false);

      public:
        ITrackPart_Bezier(const std::string &a_sIdentifier, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~ITrackPart_Bezier();

        /**
        * Create the track part
        * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
        */
        virtual void createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) override;

        /**
        * Get the information on the route for the marbles
        * @return a vector with positions to pass for the marbles
        */
        virtual std::vector<SAiPath> getAiPathData() override;

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

        virtual void drawReferencePoint(irr::video::IVideoDriver *a_pDrv) override;
 
        /**
        * Get the number of sections in this track part
        * @return the number of sections in this track part
        */
        virtual int getCount() override;
   };
  }
}