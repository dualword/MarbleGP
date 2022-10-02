// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerAI.h>
#include <string>
#include <vector>

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
          irr::core::line3df m_cLine3d;     /**< The 3d line of this section, necessary to find the correct section on race startup, respawn and stun */
          irr::core::line2df m_cLine2d;     /**< This line is the 2d representation of m_cLine3d */
          irr::core::line2df m_cEdges[2];   /**< The 2d lines that define the borders of this section */

          std::vector<SAiPathSection *> m_vNext;    /**< The next options after this section */
        }
        SAiPathSection;

        int m_iMarbleId;

        std::vector<SAiPathSection *> m_vAiPath;  /**< A list of all ai path sections */

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
