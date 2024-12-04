// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace tracked {
    const irr::u32 c_iBufferIdxRoad   = 0;    /**< Index of the road mesh buffer */
    const irr::u32 c_iBufferIdxArrow  = 1;    /**< Index of the arrow road texture */
    const irr::u32 c_iBufferIdxJoin   = 2;    /**< Index of the join road texture */
    const irr::u32 c_iBufferIdxCross  = 3;    /**< Index of the red cross road texture */
    const irr::u32 c_iBufferIdxPlane  = 4;    /**< Index of the plane road texture */
    const irr::u32 c_iBufferIdxBottom = 5;    /**< Index of the bottom mesh buffer */
    const irr::u32 c_iBufferIdxYellow = 6;    /**< Index of the yellow side mesh buffer */
    const irr::u32 c_iBufferIdxBlack  = 7;    /**< Index of the black side mesh buffer */
    
    const irr::u32 c_iBufferCount = 8;  /**< Total number of buffers for the road */

    class ITrackPart;

    /**
    * @class ITrackPart
    * @author Christian Keimel
    * The interface for all track parts
    */
    class ITrackPart {
      private:
        bool m_bSelected;  /**< Is this track part selected in the editor? */

      protected:
        std::vector<ITrackPart *> m_vNextParts;

      public:
        /**
        * The constructor
        * @param a_sType the type identifier
        * @param a_cPosition the starting position of the part
        */
        ITrackPart();

        /**
        * The destructor
        */
        virtual ~ITrackPart();

        /**
        * Create the track part
        * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
        */
        virtual void createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers) = 0;

        /**
        * Serialize the track part
        * @param a_cAttr the attributes to serialize the data to
        */
        virtual void serialize(irr::io::IAttributes &a_cAttr) = 0;

        /**
        * De-serialize the track part
        * @param a_cAttr the attributes to read the data from
        */
        virtual void deserialize(const irr::io::IAttributes &a_cAttr) = 0;

        /**
        * Get the end position of this track part
        * @return the end position of this track part
        */
        virtual const irr::core::vector3df &getEndPosition() = 0;

        /**
        * Get the direction of the end of this track part
        */
        virtual const irr::core::vector3df getEndDirection() = 0;

        /**
        * Set the selection flag of this road part
        * @param a_bSelected the new value
        */
        virtual void setSelected(bool a_bSelected);

        /**
        * Is this track part selected?
        * @return true if the track part is selected, false otherwise
        */
        bool isSelected();

        /**
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        /**
        * Get the next line defining the road to go (one side of the road to the other)
        * @param a_iIndex the index of the lines
        * @param a_cPos1 [out] the first position of the line
        * @param a_cPos2 [out] the second position of the line
        * @return true if a section line was found, false otherwise
        */
        virtual bool getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) = 0;

        /**
        * Set the next track part
        * @param a_pNext the next track part
        */
        virtual void setNext(ITrackPart *a_pNext) = 0;

        /**
        * Get the next track part
        * @return the next track part
        */
        virtual ITrackPart *getNext() = 0;

        /**
        * Draw a preview if necessary
        * @param a_pDrv the Irrlicht video driver
        */
        virtual void drawPreview(irr::video::IVideoDriver *a_pDrv) {
        }
    };
  }
}
