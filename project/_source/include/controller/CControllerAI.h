// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CMarbleController.h>
#include <scenenodes/CAiNode.h>
#include <vector>

namespace dustbin {
  namespace messages {
    class CMarbleControl;
  }

  namespace controller {
    /**
    * @class CControllerAI
    * @author Christian Keimel
    * This is the class that actually controls the AI marbles
    */
    class CControllerAI {
      private:
        irr::core::vector3df m_cPos;
        irr::core::vector3df m_cVel;
        irr::core::vector3df m_cCamPos;
        irr::core::vector3df m_cCamUp;

        irr::s32 m_iMarbleId;
        irr::f32 m_fVel;
        irr::s32 m_iLastChange;
        irr::f32 m_fLastOffset;

        scenenodes::CAiNode *m_pNode;

        scenenodes::CAiNode::SAiLink *m_pCurrent;

        irr::f32 m_fThrottle;

        irr::s8 m_iCtrlX;
        irr::s8 m_iCtrlY;

        bool m_bBrake;
        bool m_bDebug;

        std::map<int, int> m_mChoices;

        std::vector<scenenodes::CAiNode::SAiPathNode *> m_vPath;

        void selectClosestLink();
        irr::core::vector3df getLookAhead(irr::f32 a_fDistance);

      public:
        CControllerAI(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue, scenenodes::CAiNode *a_pNode);
        virtual ~CControllerAI();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) { }

        virtual void onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cNewPos);

        virtual void onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp);

        virtual void onMarbleRespawn(int a_iMarbleId);

        bool getControlMessage(irr::s32 &a_iMarbleId, irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRearView, bool &a_bRespawn);

        void setDebug(bool a_bDebug);
    };
  }
}
