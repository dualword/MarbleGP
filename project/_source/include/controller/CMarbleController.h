// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <controller/CControllerGame.h>
#include <controller/IController.h>
#include <data/CDataStructs.h>
#include <threads/IQueue.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    class CAiNode;
  }
  namespace controller {
    class IControllerAI;

    /**
    * @class CMarbleController
    * @author Christian Keimel
    * This class links the control input to the controller interface
    */
    class CMarbleController : public IController {
      protected:
        CControllerGame *m_pController;
        IControllerAI   *m_pAiControls;
        int              m_iMarbleId;
        
        data::SPlayerData::enAiHelp  m_eAiHelp;
        scenenodes::CAiNode         *m_pAiNode;

      public:
        CMarbleController(int a_iMarbleId, const std::string& a_sControls, scenenodes::CAiNode *a_pAiNode, data::SPlayerData::enAiHelp a_eAiHelp, threads::IQueue* a_pQueue);

        virtual ~CMarbleController();

        /**
        * This message must be implemented by all descendants. If called
        * it posts a control message to the queue.
        */
        virtual void postControlMessage();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent);

        virtual void onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cNewPos);

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp);

        virtual void onMarbleRespawn(int a_iMarbleId);

        /**
        * Notify the controller about a passed checkpoint
        * @param a_iMarbleId the marble that passed the checkpoint
        * @param a_iCheckpoint the passed checkpoint
        */
        virtual void onCheckpoint(int a_iMarbleId, int a_iCheckpoint);

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
        * Get the AI controller (if any)
        * @return the AI controller
        */
        virtual IControllerAI *getAiController();
    };
  }
}
