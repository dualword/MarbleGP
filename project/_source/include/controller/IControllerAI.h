// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <irrlicht.h>
#include <string>

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
          Loop,         /**< We are in a loop, so let's try to keep the speed as high as possible */
          Respawn,      /**< The marble wants to respawn */
          Respawn2
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
        * Callback for object moved messages
        * @param a_iObjectId ID of the object
        * @param a_cPosition new position of the object
        * @param a_cRotation new rotation of the object
        * @param a_cVelLin linear velocity of the object
        * @param a_fVelAng angular velocity of the object
        */
        virtual void onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cRotation, const irr::core::vector3df &a_cVelLin, float a_fVelAng) = 0;

        /**
        * A marble has finished
        * @param a_iMarbleId ID of the finished marble
        */
        virtual void onPlayerFinished(irr::s32 a_iMarbleId) = 0;

        /**
        * Get the control values for the marble
        * @param a_iMarbleId [out] ID of the marble this controller controls
        * @param a_iCtrlX [out] the steering value 
        * @param a_iCtrlY [out] the throttle value
        * @param a_bBrake [out] is the brake active?
        * @param a_bRearView [out] does the marble look to the back?
        * @param a_bRespawn [out] does the marble want a manual respawn?
        * @param a_eMode [out] the AI mode the marble is currently in
        */
        virtual bool getControlMessage(irr::s32 &a_iMarbleId, irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRearView, bool &a_bRespawn, enMarbleMode &a_eMode) = 0;

        /**
        * Set the controller to debug mode
        * @param a_bDebugPath the new debug flag for path debugging
        * @param a_bDebugDice the new debug flat for dice debugging
        */
        virtual void setDebug(bool a_bDebugPath, bool a_bDebugDice) = 0;

        /**
        * Tell the controller about it's HUD
        * @param a_pHUD the HUD
        */
        virtual void setHUD(gui::CGameHUD *a_pHUD) = 0;

        /**
        * Get the render target texture for path debugging
        * @return the render target texture for debugging
        */
        virtual irr::video::ITexture *getDebugPathTexture() = 0;

        /**
        * Get the render target texture for dice debugging
        * @return the render target texture for dice debugging
        */
        virtual irr::video::ITexture *getDebugDiceTexture() = 0;

        /**
        * This function receives messages of type "LuaMessage"
        * @param a_NumberOne First number for any information
        * @param a_NumberTwo Other number for any information
        * @param a_Data String for any further information
        */
        virtual void onLuamessage(irr::s32 a_NumberOne, irr::s32 a_NumberTwo, const std::string &a_Data) = 0;

        /**
        * Get a tutorial string
        * @return a tutorial string
        */
        virtual std::wstring getTutorialText(data::SPlayerData::enAiHelp a_eAiHelp) = 0;
    };
  }
}

