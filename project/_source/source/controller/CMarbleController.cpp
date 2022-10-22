// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <controller/CMarbleController.h>
#include <controller/CControllerAi_V2.h>
#include <messages/CSerializer64.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CMarbleController::CMarbleController(int a_iMarbleId, const std::string& a_sControls, scenenodes::CAiNode *a_pAiNode, data::SPlayerData::enAiHelp a_eAiHelp, threads::IQueue* a_pQueue) : 
      IController  (a_pQueue), 
      m_pController(nullptr), 
      m_pAiControls(nullptr), 
      m_iMarbleId  (a_iMarbleId),
      m_eAiHelp    (a_eAiHelp),
      m_pAiNode    (a_pAiNode)
    {
      messages::CSerializer64 l_cSerializer = messages::CSerializer64(a_sControls.c_str());

      std::string l_sHead = l_cSerializer.getString();

      if (l_sHead == "DustbinController") {
        m_pController = new controller::CControllerGame();
        m_pController->deserialize(a_sControls);
      }

      if (m_eAiHelp != data::SPlayerData::enAiHelp::Off) {
        m_pAiControls = new CControllerAi_V2(a_iMarbleId, "");
        
        if (CGlobal::getInstance()->getSettingData().m_bDebugAI) {
          m_pAiControls->setDebug(true);

          irr::video::ITexture *l_pTexture = m_pAiControls->getDebugTexture();

          if (l_pTexture != nullptr) {
            irr::core::dimension2du l_cScreen = CGlobal::getInstance()->getVideoDriver()->getScreenSize();
            irr::core::dimension2du l_cSize   = l_cScreen;

            l_cSize.Width  /= 3;
            l_cSize.Height /= 3;

            CGlobal::getInstance()->getGuiEnvironment()->addImage(
              l_pTexture,
              irr::core::vector2di(0, l_cScreen.Height - l_cSize.Height)
            );
          }
        }
      }
    }

    CMarbleController::~CMarbleController() {
      if (m_pController != nullptr)
        delete m_pController;

      if (m_pAiControls != nullptr)
        delete m_pAiControls;
    }

    /**
    * This message must be implemented by all descendants. If called
    * it posts a control message to the queue.
    */
    void CMarbleController::postControlMessage() {
      if (m_pQueue != nullptr && m_pController != nullptr) {
        irr::f32 l_fCtrlX = m_pController->getSteer();
        irr::f32 l_fCtrlY = m_pController->getThrottle();

        irr::s8 l_iCtrlX = (irr::s8)(127.0f * (l_fCtrlX > 1.0f ? 1.0f : l_fCtrlX < -1.0f ? -1.0f : l_fCtrlX));
        irr::s8 l_iCtrlY = (irr::s8)(127.0f * (l_fCtrlY > 1.0f ? 1.0f : l_fCtrlY < -1.0f ? -1.0f : l_fCtrlY));


        bool l_bBrake    = m_pController->getBrake();
        bool l_bRearView = m_pController->getRearView();
        bool l_bRespawn  = m_pController->getRespawn();

        if (m_pAiControls != nullptr) {
          irr::s32 i = 0;
          irr::s8  l_iBotX = 0;
          irr::s8  l_iBotY = 0;
          bool     l_bBrakeBot = false;
          bool     l_bRearBot  = false;
          bool     l_bRspnBot  = false;

          m_pAiControls->getControlMessage(i, l_iBotX, l_iBotY, l_bBrakeBot, l_bRearBot, l_bRspnBot);

          if (m_eAiHelp == data::SPlayerData::enAiHelp::Bot) {
            l_iCtrlX    = l_iBotX;
            l_iCtrlY    = l_iBotY;
            l_bBrake    = l_bBrakeBot;
            l_bRearView = l_bRearBot;
            l_bRespawn  = l_bRspnBot;
          }
        }

        messages::CMarbleControl l_cMessage = messages::CMarbleControl(m_iMarbleId, l_iCtrlX, l_iCtrlY, l_bBrake, l_bRearView, l_bRespawn);
        m_pQueue->postMessage(&l_cMessage);

        if (m_pController->pause()) {
          messages::CTogglePause l_cMsg = messages::CTogglePause(m_iMarbleId);
          m_pQueue->postMessage(&l_cMsg);
        }

        if (m_pController->withdrawFromRace()) {
          messages::CPlayerWithdraw l_cMsg = messages::CPlayerWithdraw(m_iMarbleId);
          m_pQueue->postMessage(&l_cMsg);
        }
      }
    }

    /**
    * Update the controller with the Irrlicht event
    * @param a_cEvent the Irrlicht event
    */
    void CMarbleController::update(const irr::SEvent& a_cEvent) {
      if (m_pController != nullptr)
        m_pController->update(a_cEvent);
    }

    void CMarbleController::onObjectMoved(int a_iObjectId, const irr::core::vector3df &a_cNewPos) { 
    }

    void CMarbleController::onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp) { 
      if (m_pAiControls != nullptr)
        m_pAiControls->onMarbleMoved(a_iMarbleId, a_cNewPos, a_cVelocity, a_cCameraPos, a_cCameraUp);
    }

    void CMarbleController::onMarbleRespawn(int a_iMarbleId) { 
      if (m_pAiControls != nullptr)
        m_pAiControls->onMarbleRespawn(a_iMarbleId);
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CMarbleController::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (m_pAiControls != nullptr)
        m_pAiControls->onCheckpoint(a_iMarbleId, a_iCheckpoint);
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
    * @param a_DeficitLeader Deficit of the marble on the leader in steps
    */
    void CMarbleController::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
      if (m_pAiControls != nullptr)
        m_pAiControls->onRaceposition(a_MarbleId, a_Position, a_Laps, a_DeficitAhead, a_DeficitLeader);
    }

    /**
    * For debuggin purposes: Draw the data used to control the marble (2d)
    * @param a_pDrv the video driver
    */
    void CMarbleController::drawDebugData2d(irr::video::IVideoDriver* a_pDrv) {
      if (m_pAiControls != nullptr)
        m_pAiControls->drawDebugData2d(a_pDrv);
    }

    /**
    * Get the AI controller (if any)
    * @return the AI controller
    */
    IControllerAI* CMarbleController::getAiController() {
      return m_pAiControls;
    }
  }
}