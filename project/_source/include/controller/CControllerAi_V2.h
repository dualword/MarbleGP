// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerAI.h>
#include <data/CDataStructs.h>
#include <string>
#include <vector>
#include <tuple>
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
        struct SPathLine3d;

        /**
        * @class SPathLine2d
        * @autor Christian Keimel
        * This struct holds 3 2d lines for the path calculation: the central line and two borders
        */
        typedef struct SPathLine2d {
          irr::core::line2df m_cLines[3];   /**< The path lines (index 0 == central, 1 == border, 2 == border) */
          irr::core::line3df m_cOriginal;   /**< The original 3d line (central) */
          irr::f32           m_fWidth;      /**< Width of the path */
          irr::s32           m_iMarbleId;   /**< ID of the AI controlled marble */
          irr::core::matrix4 m_cMatrix;

          irr::core::vector3df m_cNormal;

          irr::core::plane3df m_cPlane;

          data::SMarblePosition *m_pMarbles;    /**< The marble data (for debugging) */

          SPathLine3d *m_pParent;   /**< The 3d path line this 2d path line belongs to */

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

          /**
          * Check if a given line intersects with on of the borders (m_cLine indices 1 and 2)
          * @param a_cLine the line to test
          * @return true if the line intersects with a border, false otherwise
          */
          bool doesLineIntersectBorder(const irr::core::line2df &a_cLine);
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
          SPathLine2d  m_cPathLine;   /**< The 2d path line */

          SAiPathSection *m_pParent;  /**< The AI path section this path line belongs to */

          irr::core::aabbox3df m_cBBox;   /**< The 3d axis aligned bounding box of this line */

          /**
          * Create 2d path lines out of the list of 3d path lines
          * @param a_cMatrix the camera matrix to use for the transformation
          * @param a_mSplitSelections a map with all the already selected directions on road splits
          * @param a_mLastStepSelections the selection map used in the last step
          * @param a_vMarbles the current positions of the marbles
          * @param a_vMarblePosVel [out] transformed positions (tuple index 0) and velocities (tuple index 1) of the marbles
          */
          SPathLine2d *transformTo2d(
            const irr::core::matrix4 &a_cMatrix, 
            std::map<irr::core::vector3df, int> &a_mSplitSelections, 
            std::map<irr::core::vector3df, int> &a_mLastStepSelections, 
            std::vector<const data::SMarblePosition *> &a_vMarbles,
            std::vector<std::tuple<irr::core::vector3df, irr::core::vector3df>> &a_vMarblePosVel
          );

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

          irr::core::line3df m_cRealLine;   /**< The real (not adjusted) 3d line of the section */
          irr::core::line3df m_cLine3d;     /**< The 3d line of this section, necessary to find the correct section on race startup, respawn and stun */
          irr::core::line3df m_cEdges[2];   /**< The side edges of the section */

          irr::core::plane3df m_cPlane;     /**< This section's 3d plane */

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
          * @param a_pPrevious the previous line item
          * @param a_pMarbles the marble data (for debugging)
          * @param a_iMarbleId ID of the AI controlled marble (for debugging)
          */
          SPathLine3d *prepareTransformedData(irr::f32 a_fLength, SPathLine3d *a_pPrevious, data::SMarblePosition *a_pMarbles, int a_iMarbleId);

          /**
          * Fill the vectors of the points for the next 500+ meters
          * with Irrlicht vectors transformed to the section plane
          * @param a_pMarbles the marble data (for debugging)
          * @param a_iMarbleId ID of the AI controlled marble (for debugging)
          */
          void fillLineVectors(data::SMarblePosition *a_pMarbles, int a_iMarbleId);

          /**
          * Check whether or not the two passed lines (for edges) may overlap
          * @param a_cLine1 the first line
          * @param a_cLine2 the second line
          * @return true if the lines may overlap, false otherwise
          */
          bool doLinesOverlap(const irr::core::line3df &a_cLine1, const irr::core::line3df &a_cLine2);
        }
        SAiPathSection;

        /**
        * @class SRacePosition
        * @author Christian Keimel
        * A structure to hold information about the race position of the marbles
        */
        typedef struct SRacePosition {
          int m_iMarble;
          int m_iPosition;
          int m_iDeficitAhead;
          int m_iDeficitLeader;

          SRacePosition();
        }
        SRacePosition;

        enum class enMarbleMode {
          OffTrack,
          Default,
          Cruise,
          TimeAttack
        };

        int m_iMarbleId;          /**< ID of the marble this instance controls */
        int m_iIndex;             /**< Index of the marble in the marble data array */
        int m_iLastCheckpoint;    /**< The last passed checkpoint */
        int m_iMyPosition;        /**< My position in the race */
        int m_iPathSelection;     /**< The index of the path selection maps */

        irr::f32 m_fVCalc;    /**< The calculated speed */
        irr::f32 m_fScale;    /**< The scaling factor for the debug image */

        SAiPathSection *m_pCurrent;   /**< The currently closest section of the marble */
        gui::CGameHUD  *m_pHUD;       /**< The game HUD which uses this controller to give hints to the player */

        irr::f64 m_fOldAngle;

        enMarbleMode m_eMode;   /**< The marble's current mode */

        irr::video::IVideoDriver *m_pDrv;       /**< The Irrlicht video driver for debug rendering */
        irr::video::ITexture     *m_pDebugRTT;  /**< Render target texture for debugging */
        irr::core::dimension2du   m_cRttSize;   /**< Size of the debug RTT */
        irr::core::vector2di      m_cOffset;    /**< Offset for debug rendering */

        irr::core::vector2df m_cVelocity2d; /**< The transformed velocity of the marble */

        std::map<irr::core::vector3df, int> m_mSplitSelections[2];    /**< Selections of split roads */

        SRacePosition          m_aRacePositions[16];    /**< The positions in the race */
        const data::SMarblePosition *m_aMarbles      ;    /**< Data of the marbles */

        static std::vector<SAiPathSection *> m_vAiPath;     /**< A list of all ai path sections */
        static int                           m_iInstances;  /**< Instance counter. If the counter is zero the constrcutor will create the AI data, if it reaches zero in the destructor the AI data will be deleted */

        SPathLine2d *m_p2dPath;   /**< The 2d path for the control calculation */

        /**
        * Switch this AI marble to another mode
        * @param a_eMode the new mode
        * @param a_iCall ID of the calling code portion
        */
        void switchMarbleMode(enMarbleMode a_eMode, int a_iCall);

        /**
        * Find all ends of a path and store them in the given vector. If a path splits this will produce two
        * ends, not matter if the two paths are re-united within the look-ahead distance
        * @param a_vEnds [out] the vector to store the ends in
        * @param a_pLine the line to check
        * @param a_fLength the length that has already been processed
        * @param a_fFactor factor to multiply the length of the line (a_pLine) with. Necessary for the first call as a portion of the line 
        * is already done, all later calls just get "1.0"
        */
        void findEnds(std::vector<SPathLine2d *> &a_vEnds, SPathLine2d *a_pLine, irr::f32 a_fLength, irr::f32 a_fFactor);

        /**
        * Select the closest AI path section to the position. Will be called
        * when the race is started, after respawn and stun
        * @param a_bSelectStartupPath select a path marked as "startup"
        * @return the closest matching AI path section
        */
        SAiPathSection *selectClosest(const irr::core::vector3df &a_cPosition, std::vector<SAiPathSection *> &a_vOptions, bool a_bSelectStartupPath);

        /**
        * Select the new current AI path section
        * @param a_cPosition the marble's position
        * @param a_pCurrent the current section
        * @return the new current AI path section
        */
        SAiPathSection *selectCurrentSection(const irr::core::vector3df &a_cPosition, SAiPathSection *a_pCurrent);

        /**
        * Get the best, i.e. not colliding with a border line, line in the path starting
        * from the startpoint of the "a_cLine" parameter
        * @param a_cLine [out] the best line, start is used as input
        * @param a_pEnd the 2d path lines to search
        * @param a_pCollide [out] the 2d line path where the collision was detected (can be nullptr)
        */
        bool getBestLine(irr::core::line2df &a_cLine, SPathLine2d *a_pEnd, SPathLine2d **a_pCollide);

        /**
        * Get the 2d lines for calculating the marble controls for Default mode
        * @param a_cLineOne [out] the first control line
        * @param a_cLineTwo [out] the second control line
        * @param a_pPath the AI path data for calculating the two lines
        * @return the number of calculated lines (1 or 2)
        */
        int getControlLines_Default(irr::core::line2df &a_cLineOne, irr::core::line2df &a_cLineTwo, SPathLine2d *a_pPath);

        /**
        * Get the 2d lines for calculating the marble controls for Cruise mode
        * @param a_cLineOne [out] the first control line
        * @param a_cLineTwo [out] the second control line
        * @param a_pPath the AI path data for calculating the two lines
        * @return the number of calculated lines (1 or 2)
        */
        int getControlLines_Cruise(irr::core::line2df &a_cLineOne, irr::core::line2df &a_cLineTwo, SPathLine2d *a_pPath);

        /**
        * Get the 2d lines for calculating the marble controls for Time Attack mode
        * @param a_cLineOne [out] the first control line
        * @param a_cLineTwo [out] the second control line
        * @param a_pPath the AI path data for calculating the two lines
        * @return the number of calculated lines (1 or 2)
        */
        int getControlLines_TimeAttack(irr::core::line2df &a_cLineOne, irr::core::line2df &a_cLineTwo, SPathLine2d *a_pPath);

        /**
        * Get the 2d lines for calculating the marble controls after the marble has respawned
        * @param a_cLineOne [out] the first control line
        * @param a_cLineTwo [out] the second control line
        * @param a_pPath the AI path data for calculating the two lines
        * @return the number of calculated lines (1 or 2)
        */
        int getControlLines_Offtrack(irr::core::line2df &a_cLineOne, irr::core::line2df &a_cLineTwo, SPathLine2d *a_pPath);

      public:
        /**
        * The constructor
        * @param a_iMarbleId the marble ID for this controller
        * @param a_sControls details about the skills of the controller
        */
        CControllerAi_V2(int a_iMarbleId, const std::string &a_sControls, data::SMarblePosition *a_pMarbles);

        virtual ~CControllerAi_V2();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) override;

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
        virtual void onCheckpoint(int a_iMarbleId, int a_iCheckpoint) override;

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
        * @param a_DeficitLeader Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) override;

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
        * Tell the controller about it's HUD
        * @param a_pHUD the HUD
        */
        virtual void setHUD(gui::CGameHUD *a_pHUD) override;

        /**
        * Get the render target texture for debugging
        * @return the render target texture for debugging
        */
        virtual irr::video::ITexture *getDebugTexture() override;
    };
  }
}
