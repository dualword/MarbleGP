// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerAI.h>
#include <string>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerAi_V2
    * @author Christian Keimel
    * This is a new AI controller for the marbles. In order to be able
    * to compare the performance this new controller was added as a 
    * class of it's own
    */
    class CControllerAi_V2 : public IControllerAI {
      private:
        /**
        * @class SAiPathSection
        * @author Christian Keimel
        * This struct is used to store the AI path data
        * read from the scene
        */
        typedef struct SAiPathSection {
          int m_iIndex;         /**< Index for debugging */
          int m_iCheckpoint;    /**< The checkpoint index this section belongs to */
          bool m_bStartup;      /**< Is this a startup section? */

          irr::core::line3df m_cLine3d;     /**< The 3d line of this section, necessary to find the correct section on race startup, respawn and stun */
          irr::core::line3df m_cEdges[2];   /**< The side edges of the section */

          irr::core::vector3df m_cNormal;   /**< The normal of the section */

          std::vector<SAiPathSection *> m_vNext;          /**< The next options after this section */
          std::vector<int             > m_vCheckpoints;   /**< Vector with the next indices of the checkpoint */

          std::vector<irr::core::line3df> m_vLinesCentral;    /**< The central lines defining the path */
          std::vector<irr::core::line3df> m_vLinesBorder[2];  /**< The border lines defining the edge of the track */

          /**
          * Prepare the 3d line data for storing them in the next and border line vectors. To do
          * so we need to transform the nodes to lie in a plane defined by the position and normal
          * of this section. Starting from the end (when a distance of 500 meters is exceeded) we
          * need to traverse backwards and transform the points of the next line to lie in the plane
          * of the previous section. Complicated but it somehow works
          * @param a_fLength the length that has alreaddy been exceeded
          * @param a_iLineIdx the index of the line to process (0 == m_cLine3d, 1 == m_cEdges[0], 2 == m_cEdges[1])
          * @param a_vOutput [out] the vector that will be filled with all the 3d points lying in the plane
          */
          bool prepareTransformedData(irr::f32 a_fLength, int a_iLineIdx, std::vector<irr::core::line3df> &a_vOutput, std::vector<int> &a_vStack);

          /**
          * Fill the vectors of the points for the next 500+ meters
          * with Irrlicht vectors transformed to the section plane
          */
          void fillLineVectors();

          /**
          * Check whether or not the two passed lines (for edges) may overlap
          * @param a_cLine1 the first line
          * @param a_cLine2 the second line
          * @return true if the lines may overlap, false otherwise
          */
          bool doLinesOverlap(const irr::core::line3df &a_cLine1, const irr::core::line3df &a_cLine2);
        }
        SAiPathSection;

        int m_iMarbleId;          /**< ID of the marble this instance controls */
        int m_iLastCheckpoint;    /**< The last passed checkpoint */

        SAiPathSection *m_pCurrent;   /**< The currently closest section of the marble */

        irr::core::vector3df m_cPosition;   /**< The marble's position */
        irr::core::vector3df m_cVelocity;   /**< The marble's velocity */
        irr::core::vector3df m_cContact;    /**< The contact point of the marble */
        irr::core::vector3df m_cDirection;  /**< The camera direction */
        irr::core::vector3df m_cCameraUp;   /**< The camera up vector */

        static std::vector<SAiPathSection *> m_vAiPath;     /**< A list of all ai path sections */
        static int                           m_iInstances;  /**< Instance counter. If the counter is zero the constrcutor will create the AI data, if it reaches zero in the destructor the AI data will be deleted */

        std::vector<irr::core::line2df> m_v2dLinesCentral;    /**< The central lines defining the path in 2d */
        std::vector<irr::core::line2df> m_v2dLinesBorder[2];  /**< The border lines defining the edge of the track in 2d */

        /**
        * Select the closest AI path section to the position. Will be called
        * when the race is started, after respawn and stun
        * @param a_cPosition the position of the marble
        * @param a_bSelectStartupPath select a path marked as "startup"
        * @return the closest matching AI path section
        */
        SAiPathSection *selectClosest(const irr::core::vector3df &a_cPosition, std::vector<SAiPathSection *> &a_vOptions, bool a_bSelectStartupPath);

      public:
        /**
        * The constructor
        * @param a_iMarbleId the marble ID for this controller
        * @param a_sControls details about the skills of the controller
        */
        CControllerAi_V2(int a_iMarbleId, const std::string &a_sControls);

        virtual ~CControllerAi_V2();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) override;

        /**
        * Notify the controller about marble movement
        * @param a_iMarbleId ID of the moving marble
        * @param a_cNewPos the new position of the marble
        * @param a_cVelocity the velocity of the marble
        * @param a_cCameraPos the position of the camera
        * @param a_cCameraUp the up-vector of the camera
        */
        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp) override;

        /**
        * Notify the controller about a marble respawn
        * @param a_iMarbleId the respawning marble
        */
        virtual void onMarbleRespawn(int a_iMarbleId) override;

        /**
        * Notify the controller about a passed checkpoint
        * @param a_iMarbleId the marble that passed the checkpoint
        * @param a_iCheckpoint the passed checkpoint
        */
        virtual void onCheckpoint(int a_iMarbleId, int a_iCheckpoint);

        /**
        * Get the control values for the marble
        * @param a_iMarbleId [out] ID of the marble this controller controls
        * @param a_iCtrlX [out] the steering value 
        * @param a_iCtrlY [out] the throttle value
        * @param a_bBrake [out] is the brake active?
        * @param a_bRearView [out] does the marble look to the back?
        * @param a_bRespawn [out] does the marble want a manual respawn?
        */
        virtual bool getControlMessage(irr::s32 &a_iMarbleId, irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRearView, bool &a_bRespawn) override;

        /**
        * Set the controller to debug mode
        * @param a_bDebug the new debug flag
        */
        virtual void setDebug(bool a_bDebug) override;

        /**
        * For debuggin purposes: Draw the data used to control the marble (2d)
        * @param a_pDrv the video driver
        */
        virtual void drawDebugData2d(irr::video::IVideoDriver *a_pDrv) override;
    };
  }
}
