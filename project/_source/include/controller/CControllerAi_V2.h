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
        struct SAiPathSection;

        /**
        * @class SPathLine2d
        * @autor Christian Keimel
        * This struct holds 3 2d lines for the path calculation: the central line and two borders
        */
        typedef struct SPathLine2d {
          irr::core::line2df m_cLines[3];   /**< The path lines (index 0 == central, 1 == border, 2 == border) */

          SPathLine2d();
          SPathLine2d(const SPathLine2d &a_cOther);
          SPathLine2d(irr::core::line2df &a_cLine1, irr::core::line2df &a_cLine2, irr::core::line2df &a_cLine3);

          ~SPathLine2d();

          std::vector<SPathLine2d *> m_vNext;   /**< The next lines */

          SPathLine2d *m_pPrevious;   /**< The previous path line */

          /**
          * Debug draw this 2d line instance
          * @param a_pDrv the Irrlicht video driver
          * @param a_cOffset the offset to draw
          * @param a_fScale the scale to use for drawing
          */
          void debugDraw(irr::video::IVideoDriver *a_pDrv, const irr::core::vector2di &a_cOffset, irr::f32 a_fScale);
        }
        SPathLine2d;

        /**
        * @class SPathLine3d
        * @author Christian Keimel
        * This struct holds 3 3d lines for the path calculation: the central line and two borders
        */
        typedef struct SPathLine3d {
          irr::core::line3df m_cLines[3];   /**< The path lines (index 0 == central, 1 == border, 2 == border) */

          SPathLine3d();
          SPathLine3d(const SPathLine3d &a_cOther);
          SPathLine3d(irr::core::line3df &a_cLine1, irr::core::line3df &a_cLine2, irr::core::line3df &a_cLine3);

          ~SPathLine3d();

          std::vector<SPathLine3d *> m_vNext;   /**< The next lines*/

          int m_iSectionIndex;        /**< Index of the section this path line belongs to */

          SPathLine3d *m_pPrevious;   /**< The previous path line */

          /**
          * Create 2d path lines out of the list of 3d path lines
          * @param a_cMatrix the camera matrix to use for the transformation
          */
          SPathLine2d *transformTo2d(const irr::core::matrix4 &a_cMatrix);

          /**
          * Transform the lines to lie in the given plane
          * @param a_cPlane the plane to place the points on
          * @param a_cNormal the normal of the path section
          */
          void transformLinesToPlane(const irr::core::plane3df &a_cPlane, const irr::core::vector3df &a_cNormal);
        }
        SPathLine3d;

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

          SPathLine3d *m_pAiPath;

          SAiPathSection();
          ~SAiPathSection();

          /**
          * Prepare the 3d line data for storing them in the next and border line vectors. To do
          * so we need to transform the nodes to lie in a plane defined by the position and normal
          * of this section. Starting from the end (when a distance of 500 meters is exceeded) we
          * need to traverse backwards and transform the points of the next line to lie in the plane
          * of the previous section. Complicated but it somehow works
          * @param a_fLength the length that has alreaddy been exceeded
          * @param a_vStack stack of indices to prevent cyclic processing
          * @param a_pPrevious the previous line item
          */
          SPathLine3d *prepareTransformedData(irr::f32 a_fLength, std::vector<int> &a_vStack, SPathLine3d *a_pPrevious);

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

        SPathLine2d *m_p2dPath;   /**< The 2d path for the control calculation */

        void findEnds(std::vector<SPathLine2d *> &a_vEnds, SPathLine2d *a_pLine);

        /**
        * Select the closest AI path section to the position. Will be called
        * when the race is started, after respawn and stun
        * @param a_bSelectStartupPath select a path marked as "startup"
        * @return the closest matching AI path section
        */
        SAiPathSection *selectClosest(const irr::core::vector3df &a_cPosition, std::vector<SAiPathSection *> &a_vOptions, bool a_bSelectStartupPath);

        /**
        * Draw a debug line
        * @param a_pDrv the Irrlicht video driver
        * @param a_cLine the line to draw
        * @param a_cColor the color of the line
        */
        void draw2dDebugLine(irr::video::IVideoDriver *a_pDrv, const irr::core::line2di &a_cLine, const irr::video::SColor &a_cColor);

        /**
        * Draw a debug line with factor
        * @param a_pDrv the Irrlicht video driver
        * @param a_cLine the line to draw
        * @param a_fFactor the factor to scale the line
        * @param a_cColor the color of the line
        * @param a_cOffset offset of the line
        */
        void draw2dDebugLineFloat(irr::video::IVideoDriver *a_pDrv, const irr::core::line2df &a_cLine, irr::f32 a_fFactor, const irr::video::SColor &a_cColor, const irr::core::vector2di &a_cOffset);

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
