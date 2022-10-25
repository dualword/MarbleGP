// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <messages/IMessage.h>
#include <threads/IQueue.h>
#include <irrlicht.h>

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
        * it posts a control message to the queue.
        */
        virtual void postControlMessage() = 0;

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) = 0;

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp) = 0;

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
        * Draw 3d AI debug data (if wanted and necessary)
        * @param a_pDrv the video driver
        */
        virtual void draw3dDebugData(irr::video::IVideoDriver *a_pDrv) = 0;

        /**
        * Get the AI controller (if any)
        * @return the AI controller
        */
        virtual IControllerAI *getAiController() = 0;
    };
  }
}
