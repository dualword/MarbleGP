// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <messages/IMessage.h>
#include <threads/IQueue.h>
#include <irrlicht.h>
#include <string>

namespace dustbin {
  namespace controller {
    class IControllerAI;

    /**
    * @class IController
    * @author Christian Keimel
    * This is the interface for all game controllers. Unlike
    * "CControllerBase" and it's descandents it just encapsulates
    * the controller input and makes it available to the game
    */
    class IController {
      protected:
        threads::IQueue* m_pQueue;

      public:
        IController(threads::IQueue* a_pQueue) {
          m_pQueue = a_pQueue;
        }

        virtual ~IController() { }

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
        virtual void postControlMessage(bool &a_bLeft, bool &a_bRight, bool &a_bForward, bool &a_bBackward, bool &a_bBrake, bool &a_bRespawn, bool &a_bAutomatic) = 0;

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) = 0;

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp, bool a_bContact) = 0;

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
        * Callback called for every simulation step
        * @param a_iStep the current simulation step
        */
        virtual void onStep(int a_iStep) = 0;

        /**
        * This function receives messages of type "Trigger"
        * @param a_TriggerId ID of the trigger
        * @param a_ObjectId ID of the marble that caused the trigger
        */
        virtual void onTrigger(irr::s32 a_iTriggerId, irr::s32 a_iObjectId) = 0;

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() = 0;

        /**
        * Get the AI controller (if any)
        * @return the AI controller
        */
        virtual IControllerAI *getAiController() = 0;

        /**
         * If this controller has an UI this method will move it to the front.
         * The Android touch and gyroscope controllers have an UI
         */
        virtual void moveGuiToFront() = 0;

        /**
         * Get the text shown in the tutorial
         * @param a_bFirst true if this is the first help point (controls), false if it's the fourth (respawn)
         * @return the text shown in the tutorial
         */
        virtual std::wstring getTutorialText(bool a_bFirst) = 0;
    };
  }
}
