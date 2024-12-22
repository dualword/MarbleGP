// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CAiPathNode.h>

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {

  namespace tracked {
    class ITrackPart;

    const irr::u32 c_iBufferIdxRoad      =  0;      /**< Index of the road mesh buffer */
    const irr::u32 c_iBufferIdxArrow     =  1;      /**< Index of the arrow road texture */
    const irr::u32 c_iBufferIdxJoin      =  2;      /**< Index of the join road texture */
    const irr::u32 c_iBufferIdxCross     =  3;      /**< Index of the red cross road texture */
    const irr::u32 c_iBufferIdxPlane     =  4;      /**< Index of the plane road texture */
    const irr::u32 c_iBufferIdxBottom    =  5;      /**< Index of the bottom mesh buffer */
    const irr::u32 c_iBufferIdxYellow    =  6;      /**< Index of the yellow side mesh buffer */
    const irr::u32 c_iBufferIdxBlack     =  7;      /**< Index of the black side mesh buffer */
    const irr::u32 c_iBufferIdxGround    =  8;      /**< Index of the base ground plane buffer */
    const irr::u32 c_iBufferIdxWallOuter =  9;      /**< Index of the outer walls of the base buffer */
    const irr::u32 c_iBufferIdxWallInner = 10;      /**< Index of the inner walls of the base buffer */
    
    const irr::u32 c_iBufferCount = 11;  /**< Total number of buffers for the road */

    /**
    * @class ITrackPart
    * @author Christian Keimel
    * The interface for all track parts
    */
    class ITrackPart {
      private:
        std::string m_sType;    /**< The type ID of the track part */

        bool m_bStart;     /**<* Is this the start segment? */

        int m_iInternalId;    /**< The Internal ID */
        int m_iPrevious;      /**< ID of the previous part */
        int m_iNext;          /**< ID of the next part */

        ITrackPart *m_pPrevious;  /**< The previous track part (always nullptr for the start part) */
        ITrackPart *m_pNext;      /**< The next track part */

        irr::core::vector3df m_cNormalStart;    /**< The normal at the start point */
        irr::core::vector3df m_cNormalEnd;      /**< The normal at the end point */

        irr::core::line3df m_cStart;    /**< The start of the part */
        irr::core::line3df m_cEnd;      /**< The end of the part */

      protected:
        irr::f32 m_fWidth;    /**< The width of this part */

        /**
        * Set the end normal
        * @param a_cNormal the new end normal
        */
        void setEndNormal(const irr::core::vector3df &a_cNormal);


        /**
        * Get the index of a vertex in a given mesh buffer
        * @param a_cVertex the vertex to find
        * @param a_pBuffer the mesh buffer to search in
        * @return the index of the vertex, -1 if the vertex was not found
        */
        irr::s32 getIndexOfVertex(const irr::core::vector3df &a_cVertex, irr::scene::SMeshBuffer *a_pBuffer);

        /**
        * Add a quad to a mesh buffer
        * @param a_aVertices the four vertices of the quad
        * @param a_aTexturePos the four texture positions of the vertices
        * @param a_pBuffer the buffer to be filled
        * @param a_bForceAdd add the first vertices regardless of whether or not the position is already in 
        * @param a_bForceAdd2nd add the second vertices to the mesh buffer regardless of whether or not the position is already in 
        * @param a_aNormals [out] the normals of the points, must be an array with size 4 or nullptr
        */
        void addQuadToMeshBuffer(
          irr::core::vector3df a_aVertices[4],
          irr::core::position2df a_aTexturePos[4],
          irr::scene::SMeshBuffer *a_pBuffer,
          bool a_bForceAdd,
          bool a_bForceAdd2nd,
          irr::core::vector3df *a_aNormals
        );

        /**
        * Set the start of the part
        * @param a_cStart the start of the part
        */
        void setStart(const irr::core::line3df &a_cStart);

        /**
        * Set the end of the part
        * @param a_cEnd the end of the part
        */
        void setEnd(const irr::core::line3df &a_cEnd);

      public:
        /**
        * The constructor
        * @param a_sType the type ID of this track part
        * @param a_pPrevious the previous track part (nullptr for the starting part)
        * @param a_cNormal the normal at the starting point
        */
        ITrackPart(const std::string a_sType, ITrackPart *a_pPrevious, const irr::core::vector3df &a_cNormal);

        /**
        * The destructor
        */
        virtual ~ITrackPart();

        /**
        * Create the track part
        * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
        * @param a_bBlack [in] [out] the next color of the side quad
        */
        virtual void createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers, bool &a_bBlack) = 0;

        /**
        * Set the trak part's reference points
        * @param a_cPoints a vector with the reference points
        */
        virtual void setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints) = 0;

        /**
        * Serialize the track part
        * @param a_cAttr the attributes to serialize the data to
        */
        virtual void serialize(irr::io::IAttributes &a_cAttr) const;

        /**
        * De-serialize the track part
        * @param a_cAttr the attributes to read the data from
        */
        virtual void deserialize(const irr::io::IAttributes &a_cAttr);

        /**
        * Get the direction of the end of this track part
        * @return the direction of the end of this track part
        */
        virtual const irr::core::vector3df getEndDirection() = 0;

        /**
        * Get the side vector of the end of this track part
        * @return the side vector of the end of this track part
        */
        virtual irr::core::vector3df getEndSide() const = 0;

        /**
        * Can a reference point be moved freely in the 2d plane
        * @param a_iIndex index of the reference point
        * @param a_cLine [out] the line in which the point can be moved if false is returned. A line with length of 0 is returned if the point can't move at all. Only used if "false" is returned
        * @param a_cPlanePoint [out] The point of the plane the reference point is moved in. Only used if "true" is returned.
        * @param a_cPlaneNormal [out] The normal of the plane the reference point is moved in.
        * @return true if the reference point can move freely, false otherwise
        */
        virtual bool canPointMoveFreely(int a_iIndex, irr::core::line3df &a_cLine, irr::core::vector3df &a_cPlanePoint, irr::core::vector3df &a_cPlaneNormal) = 0;

        /**
        * Get the part's reference points
        * @return a vector with the part's reference points
        */
        virtual const std::vector<irr::core::vector3df> getReferencePoints() = 0;

        /**
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        /**
        * Set the next track part
        * @param a_pNext the next track part
        */
        virtual void setNext(ITrackPart *a_pPrevious);

        /**
        * Set the previous track part
        * @param a_pPreviouse the previous track part
        */
        virtual void setPrevious(ITrackPart *a_pNext);

        /**
        * Get the next track part
        * @return the next track part
        */
        virtual ITrackPart *getNext();

        /**
        * Get the previous part
        * @return the previous part
        */
        virtual ITrackPart *getPrevious();

        /**
        * Get the normal at the end point
        * @return the normal at the end point
        */
        const irr::core::vector3df &getEndNormal() const;

        /**
        * Get the normal at the start point
        * @return the normal at the start point
        */
        const irr::core::vector3df &getStartNormal() const;

        /**
        * This method is called when the reference points of the previous track part have changed
        */
        virtual void updateReferencePoints() = 0;

        /**
        * Does this reference point allow addition of new track parts?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool partAdditionAllowed(int a_iIndex) = 0;

        /**
        * Add this track part to the AI path node
        * @param a_pAi the AI path node
        * @param a_pLast the last AI path section
        */
        virtual void addToAiNode(scenenodes::CAiPathNode *a_pAi, scenenodes::CAiPathNode::SAiPathSection *a_pLast) = 0;

        /**
        * Does this reference point allow addition of another reference point?
        * @param a_iIndex index of the reference point
        * @return true if so, false otherwise
        */
        virtual bool refPointAdditionAllowed(int a_iIndex) = 0;

        /**
        * Get the start of the part
        * @return the start of the part
        */
        const irr::core::line3df &getStart() const;

        /**
        * Get the end of the part
        * @return the end of the part
        */
        const irr::core::line3df &getEnd() const;

        /**
        * Get the ID of the track part
        * @return the ID of the track part
        */
        int getId();

        /**
        * Is this the start segment?
        * @return true if so, false otherwise
        */
        bool isStartSegment();

        /**
        * Get the width of this track part
        * @return the width of this track part
        */
        irr::f32 getWidth();
        

        /**
        * Get the id of the previous track part, needed for deserialization
        * @return the ID of the previous part
        */
        int getPreviousID();

        /**
        * Get the id of the next track part, needed for deserialization
        * @return the ID of the next part
        */
        int getNextID();

        /**
        * Get the type ID of this track part
        * @return the type ID of this track part
        */
        const std::string &getType();

        /**
        * Add custom entries to the right-click context menu
        * @param a_iRefPoint the hovered reference point
        */
        virtual std::vector<std::wstring> contextMenuEntries(int a_iRefPoint);

        virtual void drawReferencePoint(irr::video::IVideoDriver *a_pDrv);

        /**
        * Does this part flip vertices? Necessary for downward ramps
        * @return true if vertices are flipped
        */
        virtual bool doesFlipVertices();
    };
  }
}
