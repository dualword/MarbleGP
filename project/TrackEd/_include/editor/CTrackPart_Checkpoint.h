// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <editor/ITrackPart.h>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace tracked {
    

    /**
    * @class CTrackPart_Checkpoint
    * @author Christian Keimel
    * The track part for checkpoints
    */
    class CTrackPart_Checkpoint : public ITrackPart {
      private:
        irr::core::vector3df m_cStart;
        irr::core::vector3df m_cEnd;

        irr::scene::ISceneNode *m_pCheckpoint;    /**< The checkpoint mesh */
        irr::scene::ISceneNode *m_pRespawn;       /**< A mesh indicating the respawn position and direction */

        bool m_bRespawnLeft;    /**< Is the respawn on the left side? */

      public:
        CTrackPart_Checkpoint(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);
        virtual ~CTrackPart_Checkpoint();

        /**
        * Get the checkpoint scene node
        * @return the checkpoint scene node
        */
        irr::scene::ISceneNode *getCheckpointNode();

        /**
        * Get the respawn scene node
        * @return the respawn scene node
        */
        irr::scene::ISceneNode *getRespawnNode();

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
        * Get the information on the route for the marbles
        * @return a vector with positions to pass for the marbles
        */
        virtual std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> getRoute() override;

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
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Get the side vector of the end of this track part
        * @return the side vector of the end of this track part
        */
        virtual irr::core::vector3df getEndSide() const override;

        /**
        * Get the number of sections in this track part
        * @return the number of sections in this track part
        */
        virtual int getCount() override;

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
    };
  }
}
