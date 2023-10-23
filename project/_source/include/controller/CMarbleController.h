// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/IController.h>
#include <data/CDataStructs.h>
#include <threads/IQueue.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    class CAiNode;
  }

  namespace lua {
    class CLuaScript_ai;
  }

  namespace controller {
    class IControllerGame;
    class IControllerAI;

    /**
    * @class CMarbleController
    * @author Christian Keimel
    * This class links the control input to the controller interface
    */
    class CMarbleController : public IController {
      protected:
        IControllerGame *m_pController;
        IControllerAI   *m_pAiControls;
        int              m_iMarbleId;
        bool             m_bOwnsCtrl;     /**< Does this class need to delete the controller or is this done by the GUI */
        
        data::SMarblePosition        m_aMarbles[16];
        data::SPlayerData::enAiHelp  m_eAiHelp;
        scenenodes::CAiNode         *m_pAiNode;       
        lua::CLuaScript_ai          *m_pLuaScript;    /**< AI LUA script for decisions */

      public:
        CMarbleController(
          int                          a_iMarbleId, 
          const std::string           &a_sControls,
          scenenodes::CAiNode         *a_pAiNode, 
          data::SPlayerData::enAiHelp  a_eAiHelp, 
          threads::IQueue             *a_pQueue, 
          const std::string            &a_sAiScript,
          const irr::core::recti       &a_cViewport
        );

        virtual ~CMarbleController();

        /**
        * This message must be implemented by all descendants. If called
        * it posts a control message to the queue. The parameters are used
        * to update the HUD if AI help is in use
        * @param a_bLeft [out] does the marble steer left?
        * @param a_bRight [out] does the marble steer right?
        * @param a_bForward [out] does the marble accelerate?
        * @param a_bBackward [out] does the marble decelerate?
        * @param a_bBrake [out] is the marble braking?
        * @param a_bRespawn [out] does the marble request manual respawn?
        * @param a_bAutomatic [out] is the automatic control active?
        */
        virtual void postControlMessage(bool &a_bLeft, bool &a_bRight, bool &a_bForward, bool &a_bBackward, bool &a_bBrake, bool &a_bRespawn, bool &a_bAutomatic) override;

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) override;

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp, bool a_bHasContact) override;

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
        * Callback for object moved messages
        * @param a_iObjectId ID of the object
        * @param a_cPosition new position of the object
        * @param a_cRotation new rotation of the object
        * @param a_cVelLin linear velocity of the object
        * @param a_fVelAng angular velocity of the object
        */
        virtual void onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cRotation, const irr::core::vector3df &a_cVelLin, float a_fVelAng) override;

        /**
        * Get the AI controller (if any)
        * @return the AI controller
        */
        virtual IControllerAI *getAiController() override;

        /**
        * Callback called for every simulation step
        * @param a_iStep the current simulation step
        */
        virtual void onStep(int a_iStep) override;

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() override;

        /**
        * This function receives messages of type "Trigger"
        * @param a_TriggerId ID of the trigger
        * @param a_ObjectId ID of the marble that caused the trigger
        */
        virtual void onTrigger(irr::s32 a_iTriggerId, irr::s32 a_iObjectId) override;

        /**
         * If this controller has an UI this method will move it to the front.
         * The Android touch and gyroscope controllers have an UI
         */
        virtual void moveGuiToFront() override;

        /**
        * Get the text shown in the tutorial
        * @param a_bFirst true if this is the first help point (controls), false if it's the fourth (respawn)
        * @return the text shown in the tutorial
        */
        virtual std::wstring getTutorialText(bool a_bFirst) override;
    };
  }
}
