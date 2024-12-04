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

        irr::scene::ISceneCollisionManager *m_pColMgr;

        std::vector<irr::scene::ISceneNode *> m_vEdits;
        std::vector<irr::scene::ISceneNode *> m_vLinks;
        std::vector<irr::core::vector3df    > m_vPoints;

        enType m_eType;   /**< The type of this track part */

        ITrackPart *m_pPrevious;
        ITrackPart *m_pNext;

        std::vector<irr::core::vector3df> m_vTrack;

        std::vector<double> fillIndexVector(int a_iSteps);

        /**
        * Calculate the points of the curve, road center
        * @param a_cOut the vector to be filled
        * @return the length of the curve
        */
        double calculateCurve(std::vector<irr::core::vector3df> &a_cOut);

      public:
        CTrackPart_Bezier(ITrackPart *a_pPrevious, const irr::core::vector3df &a_cPosition, irr::scene::ISceneCollisionManager *a_pColMgr);
        virtual ~CTrackPart_Bezier();

        /**
        * Create the track part
        * @param a_vBuffers the vector of irr::scene::SMeshBuffer instances to be filled
        */
        virtual void createTrackPart(std::vector<irr::scene::SMeshBuffer *> &a_vBuffers) override;

        void setReferencePoints(const std::vector<irr::core::vector3df> &a_cPoints);

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
        * Set the next track part
        * @param a_pNext the next track part
        */
        virtual void setNext(ITrackPart *a_pNext) override;

        /**
        * Get the next track part
        * @return the next track part
        */
        virtual ITrackPart *getNext() override;

        /**
        * Get the next line defining the road to go (one side of the road to the other)
        * @param a_iIndex the index of the lines
        * @param a_cPos1 [out] the first position of the line
        * @param a_cPos2 [out] the second position of the line
        * @return true if a section line was found, false otherwise
        */
        virtual bool getSectionLine(irr::u32 a_iIndex, irr::core::vector3df &a_cPos1, irr::core::vector3df &a_cPos2) override;

        /**
        * Handle an event if this part is selected
        * @param a_cEvent the event to handle
        * @return true if the event was handled
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
