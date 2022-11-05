// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <controller/CAiControlThread.h>
#include <controller/CControllerAI.h>

namespace dustbin {
  namespace controller {
    /**
    * This function receives messages of type "StepMsg"
    * @param a_StepNo The current step number
    */
    void CAiControlThread::onStepmsg(irr::u32 a_StepNo) {
      for (int i = 0; m_aControllers[i] != nullptr; i++) {
        irr::s32 l_iMarbleId = -1;
        irr::s8  l_iCtrlX    = 0;
        irr::s8  l_iCtrlY    = 0;
        bool     l_bBrake    = false;
        bool     l_bRearView = false;
        bool     l_bRespawn  = false;

        IControllerAI::enMarbleMode l_eMode;

        if (m_aControllers[i]->getControlMessage(l_iMarbleId, l_iCtrlX, l_iCtrlY, l_bBrake, l_bRearView, l_bRespawn, l_eMode)) {
          messages::CMarbleControl p = messages::CMarbleControl(l_iMarbleId, l_iCtrlX, l_iCtrlY, l_bBrake, l_bRearView, l_bRespawn);
          m_pDynamicsIn->postMessage(&p);
        }
      }
    }

    /**
    * This function receives messages of type "ObjectMoved"
    * @param a_ObjectId The ID of the object
    * @param a_Position The current position
    * @param a_Rotation The current rotation (Euler angles)
    * @param a_LinearVelocity The linear velocity
    * @param a_AngularVelocity The angualar (rotation) velocity
    */
    void CAiControlThread::onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity) {
    }

    /**
    * This function receives messages of type "MarbleMoved"
    * @param a_ObjectId The ID of the object
    * @param a_Position The current position
    * @param a_Rotation The current rotation (Euler angles)
    * @param a_LinearVelocity The linear velocity
    * @param a_AngularVelocity The angualar (rotation) velocity
    * @param a_CameraPosition The position of the camera
    * @param a_CameraUp The Up-Vector of the camera
    * @param a_ControlX The marble's current controller state in X-Direction
    * @param a_ControlY The marble's current controller state in Y-Direction
    * @param a_Contact A Flag indicating whether or not the marble is in contact with another object
    * @param a_ControlBrake Flag indicating whether or not the marble's brake is active
    * @param a_ControlRearView Flag indicating whether or not the marble's player looks behind
    * @param a_ControlRespawn Flag indicating whether or not the manual respawn button is pressed 
    */
    void CAiControlThread::onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) {
      int l_iIndex = a_ObjectId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16) {
        m_aMarbles[l_iIndex].m_iMarbleId  = a_ObjectId;
        m_aMarbles[l_iIndex].m_cPosition  = a_Position;
        m_aMarbles[l_iIndex].m_cVelocity  = a_LinearVelocity;
        m_aMarbles[l_iIndex].m_cDirection = a_CameraPosition - a_Position;
        m_aMarbles[l_iIndex].m_cCamera    = a_CameraPosition;
        m_aMarbles[l_iIndex].m_cCameraUp  = a_CameraUp;
      }
    }

    /**
    * This function receives messages of type "PlayerRespawn"
    * @param a_MarbleId ID of the marble
    * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
    */
    void CAiControlThread::onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) {
      if (a_State == 2) {
        for (int i = 0; m_aControllers[i] != nullptr; i++)
          m_aControllers[i]->onMarbleRespawn(a_MarbleId);
      }
    }

    /**
    * This function receives messages of type "Checkpoint"
    * @param a_MarbleId ID of the marble
    * @param a_Checkpoint The checkpoint ID the player has passed
    */
    void CAiControlThread::onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) {
      for (int i = 0; m_aControllers[i] != nullptr; i++)
        m_aControllers[i]->onCheckpoint(a_MarbleId, a_Checkpoint);
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
    * @param a_DeficitLeader Deficit of the marble on the leader in steps
    */
    void CAiControlThread::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
      for (int i = 0; m_aControllers[i] != nullptr; i++)
        m_aControllers[i]->onRaceposition(a_MarbleId, a_Position, a_Laps, a_DeficitAhead, a_DeficitLeader);
    }

    /**
    * The implementing object must implement this method. It is called when the thread start working
    */
    void CAiControlThread::execute() {
      while (!m_bStopThread) {
        messages::IMessage *l_pMsg = m_pInputQueue->waitMesssage();

        if (l_pMsg != nullptr) {
          handleMessage(l_pMsg, false);
          delete l_pMsg;
        }
      }

      printf("AI Control Thread ends.\n");
    }

    CAiControlThread::CAiControlThread(threads::COutputQueue *a_pDynamicsOut, threads::CInputQueue *a_pDynamicsIn, scenenodes::CAiNode* a_pAiNode) : 
      m_pAiNode      (a_pAiNode), 
      m_pDynamicsOut (a_pDynamicsOut),
      m_pDynamicsIn  (a_pDynamicsIn), 
      m_iNumberOfBots(0)
    {
      for (int i = 0; i < 17; i++)
        m_aControllers[i] = nullptr;

      m_pDynamicsOut->addListener(m_pInputQueue);
    }

    CAiControlThread::~CAiControlThread() {
      for (int i = 0; m_aControllers[i] != nullptr; i++) {
        delete m_aControllers[i];
      }

      m_pDynamicsOut->removeListener(m_pInputQueue);
    }

    void CAiControlThread::addAiMarble(int a_iMarbleId, const std::string& a_sControls) {
      if (m_iNumberOfBots < 16) {
        m_aControllers[m_iNumberOfBots] = new controller::CControllerAI(a_iMarbleId, a_sControls, nullptr, m_pAiNode, m_aMarbles);
        m_iNumberOfBots++;
      }
      else printf("Too many bots!\n");
    }
  }
}