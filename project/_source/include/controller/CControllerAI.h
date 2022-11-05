// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerAI.h>
#include <scenenodes/CAiNode.h>
#include <data/CDataStructs.h>
#include <vector>

namespace dustbin {
  namespace messages {
    class CMarbleControl;
  }

  namespace threads {
    class IQueue;
  }

  namespace controller {
    /**
    * @class CControllerAI
    * @author Christian Keimel
    * This is the class that actually controls the AI marbles
    */
    class CControllerAI : public IControllerAI {
      private:
        irr::s32 m_iMarbleId;
        irr::s32 m_iIndex;
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

        data::SMarblePosition *m_pMarbles;

        void selectClosestLink();
        irr::core::vector3df getLookAhead(irr::f32 a_fDistance);

      public:
        CControllerAI(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue, scenenodes::CAiNode *a_pNode, data::SMarblePosition *a_pMarbles);
        virtual ~CControllerAI();

        /**
        * Update the controller with the Irrlicht event
        * @param a_cEvent the Irrlicht event
        */
        virtual void update(const irr::SEvent& a_cEvent) override { }

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
        * @param a_eMode [out] the AI mode the marble is currently in
        */
        virtual bool getControlMessage(irr::s32 &a_iMarbleId, irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRearView, bool &a_bRespawn, enMarbleMode &a_eMode) override;

        /**
        * Set the controller to debug mode
        * @param a_bDebug the new debug flag
        */
        virtual void setDebug(bool a_bDebug) override;

        /**
        * Tell the controller about it's HUD
        * @param a_pHUD the HUD
        */
        virtual void setHUD(gui::CGameHUD* a_pHUD) override {
        }

        /**
        * Get the render target texture for debugging
        * @return the render target texture for debugging
        */
        virtual irr::video::ITexture* getDebugTexture() override {
          return nullptr;
        }

        /**
        * Draw 3d AI debug data (if wanted and necessary)
        * @param a_pDrv the video driver
        */
        virtual void draw3dDebugData(irr::video::IVideoDriver* a_pDrv) {
        }
    };
  }
}
