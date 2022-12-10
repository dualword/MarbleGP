// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace gui {
    class CGameHUD;
  }

  namespace controller {
    /**
    * @class IControllerAI
    * @author Christian Keimel
    * This interface is the base class for the AI controllers
    */
    class IControllerAI {
      public:
        /**
        * An enumeration for the marble modes
        */
        enum class enMarbleMode {
          OffTrack,     /**< The marble is off-track, hard steering necessary to get back on */
          Default,      /**< The default mode, good balance between safety and lap time */
          Cruise,       /**< Cruise mode, more safety, lap time not as good */
          TimeAttack,   /**< Time Attack mode, sacrifice some safety for better lap time */
          Jump,         /**< We are approaching a jump so we need to handle things differently */
          Respawn       /**< The marble wants to respawn */
        };

        IControllerAI() { }
        virtual ~IControllerAI() { }

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) = 0;

        /**
        * Notify the controller about a marble respawn
        * @param a_iMarbleId the respawning marble
        */
        virtual void onMarbleRespawn(int a_iMarbleId) = 0;

        /**
        * Notify the controller about a passed checkpoint
        * @param a_iMarbleId the marble that passed the checkpoint
        * @param a_iCheckpoint the passed checkpoint
        */
        virtual void onCheckpoint(int a_iMarbleId, int a_iCheckpoint) = 0;

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
        * @param a_DeficitLeader Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) = 0;

        /**
        * Get the control values for the marble
        * @param a_iMarbleId [out] ID of the marble this controller controls
        * @param a_iCtrlX [out] the steering value 
        * @param a_iCtrlY [out] the throttle value
        * @param a_bBrake [out] is the brake active?
        * @param a_bRearView [out] does the marble look to the back?
        * @param a_bRespawn [out] does the marble want a manual respawn?
        * @param a_eMode [out] the AI mode the marble is currently in
        * @param a_cPoint1 [out] the first point for the AI calculation
        * @param a_cPoint2 [out] the second point for the AI calculation
        */
        virtual bool getControlMessage(irr::s32 &a_iMarbleId, irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRearView, bool &a_bRespawn, enMarbleMode &a_eMode, irr::core::vector3df &a_cPoint1, irr::core::vector3df &a_cPoint2) = 0;

        /**
        * Set the controller to debug mode
        * @param a_bDebug the new debug flag
        */
        virtual void setDebug(bool a_bDebug) = 0;

        /**
        * Tell the controller about it's HUD
        * @param a_pHUD the HUD
        */
        virtual void setHUD(gui::CGameHUD *a_pHUD) = 0;

        /**
        * Get the render target texture for debugging
        * @return the render target texture for debugging
        */
        virtual irr::video::ITexture *getDebugTexture() = 0;
    };
  }
}

