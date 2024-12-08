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
      protected:
        enum class enType {
          Bezier1,
          Bezier2,
          Bezier3
        };

        std::vector<irr::scene::ISceneNode *> m_vEdits;
        std::vector<irr::scene::ISceneNode *> m_vLinks;
        std::vector<irr::core::vector3df    > m_vPoints;

        enType m_eType;   /**< The type of this track part */

        std::vector<irr::core::vector3df> m_vTrack;

        std::vector<double> fillIndexVector(int a_iSteps);

        /**
        * Calculate the points of the curve, road center
        * @param a_cOut the vector to be filled
        * @return the length of the curve
        */
        double calculateCurve(std::vector<irr::core::vector3df> &a_cOut);

      public:
        CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr);
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
        virtual void serialize(irr::io::IAttributes &a_cAttr) override;

        /**
        * De-serialize the track part
        * @param a_cAttr the attributes to read the data from
        */
        virtual void deserialize(const irr::io::IAttributes &a_cAttr) override;

        /**
        * Draw a preview if necessary
        * @param a_pDrv the Irrlicht video driver
        */
        virtual void drawPreview(irr::video::IVideoDriver *a_pDrv);

        /**
        * Set the selection flag of this road part
        * @param a_bSelected the new value
        */
        virtual void setSelected(bool a_bSelected) override;

        /**
        * Get the direction of the end of this track part
        */
        virtual const irr::core::vector3df getEndDirection() override;

        /**
        * Get the end position of this track part
        * @return the end position of this track part
        */
        virtual const irr::core::vector3df &getEndPosition() override;

        /**
        * Get the next line defining the road to go (one side of the road to the other)
        * @param a_iIndex the index of the lines
        * @param a_cPos1 [out] the first position of the line
        * @param a_cPos2 [out] the second position of the line
        * @return true if a section line was found, false otherwise
        */
        virtual bool getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) override;

        /**
        * Get the normal at the end point
        * @return the normal at the end point
        */
        virtual const irr::core::vector3df &getNormal() override;

        /**
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };

    /**
    * @class CTrackPart_Checkpoint
    * @author Christian Keimel
    * The track part for checkpoints
    */
    class CTrackPart_Checkpoint : public ITrackPart {
      private:
        irr::core::vector3df m_cStart;
        irr::core::vector3df m_cEnd;

        irr::scene::ISceneNode *m_pCheckpoint;

        bool m_bRespawnLeft;    /**< Is the respawn on the left side? */

      public:
        CTrackPart_Checkpoint(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal, irr::scene::ISceneCollisionManager *a_pColMgr);
        virtual ~CTrackPart_Checkpoint();

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
        virtual void serialize(irr::io::IAttributes &a_cAttr) override;

        /**
        * De-serialize the track part
        * @param a_cAttr the attributes to read the data from
        */
        virtual void deserialize(const irr::io::IAttributes &a_cAttr) override;

        /**
        * Get the end position of this track part
        * @return the end position of this track part
        */
        virtual const irr::core::vector3df &getEndPosition() override;

        /**
        * Get the direction of the end of this track part
        */
        virtual const irr::core::vector3df getEndDirection() override;

        /**
        * Get the next line defining the road to go (one side of the road to the other)
        * @param a_iIndex the index of the lines
        * @param a_cPos1 [out] the first position of the line
        * @param a_cPos2 [out] the second position of the line
        * @return true if a section line was found, false otherwise
        */
        virtual bool getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) override;

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
        * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
        * @return true if the reference point can move freely, false otherwise
        */
        virtual bool canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) override;

        /**
        * Get the normal at the end point
        * @return the normal at the end point
        */
        virtual const irr::core::vector3df &getNormal() override;

        /**
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
