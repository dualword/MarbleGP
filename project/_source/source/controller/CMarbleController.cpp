// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <controller/CControllerGame_Touch.h>
#include <controller/CControllerAiHelp_V2.h>
#include <controller/CControllerGame_Gyro.h>
#include <_generated/messages/CMessages.h>
#include <controller/CMarbleController.h>
#include <_generated/lua/CLuaScript_ai.h>
#include <controller/CControllerAi_V2.h>
#include <controller/IControllerGame.h>
#include <controller/CControllerGame.h>
#include <helpers/CStringHelpers.h>
#include <messages/CSerializer64.h>
#include <CGlobal.h>

namespace dustbin {
  namespace controller {
    CMarbleController::CMarbleController(
      int                          a_iMarbleId, 
      const std::string           &a_sControls, 
      scenenodes::CAiNode         *a_pAiNode, 
      data::SPlayerData::enAiHelp  a_eAiHelp, 
      threads::IQueue             *a_pQueue, 
      const std::string           &a_sAiScript,
      const irr::core::recti      &a_cViewport
    ) : 
      IController  (a_pQueue), 
      m_pController(nullptr), 
      m_pAiControls(nullptr), 
      m_iMarbleId  (a_iMarbleId),
      m_bOwnsCtrl  (true),
      m_eAiHelp    (a_eAiHelp),
      m_pAiNode    (a_pAiNode),
      m_pLuaScript (nullptr)
    {
      controller::IControllerGame *l_pController = nullptr;

      if (a_sControls.substr(0, std::string("DustbinTouchSteerRight").size()) == "DustbinTouchSteerRight") {
        l_pController = new controller::CControllerGame_Touch(controller::CControllerGame::enType::TouchRight, a_cViewport);
        m_bOwnsCtrl   = false;
      }
      else if (a_sControls.substr(0, std::string("DustbinTouchSteerLeft").size()) == "DustbinTouchSteerLeft") {
        l_pController = new controller::CControllerGame_Touch(controller::CControllerGame::enType::TouchLeft, a_cViewport);
        m_bOwnsCtrl   = false;
      }
      else if (a_sControls.substr(0, std::string("DustbinTouchSteerOnly").size()) == "DustbinTouchSteerOnly") {
        l_pController = new controller::CControllerGame_Touch(controller::CControllerGame::enType::TouchSteer, a_cViewport);
        m_bOwnsCtrl   = false;
      }
      else if (a_sControls.substr(0, std::string("DustbinGyroscope").size()) == "DustbinGyroscope") {
        l_pController = new controller::CControllerGame_Gyro(controller::CControllerGame::enType::Gyroscope, a_cViewport, a_eAiHelp == data::SPlayerData::enAiHelp::High);
        m_bOwnsCtrl = false;
      }
      else {
        controller::CControllerGame *p = new controller::CControllerGame();
        p->deserialize(a_sControls);
        l_pController = p;
      }

      m_pController = l_pController;

      if (m_eAiHelp != data::SPlayerData::enAiHelp::Off) {
        irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

        data::SMarbleAiData l_cAiData = data::SMarbleAiData(m_eAiHelp);

        if (m_eAiHelp == data::SPlayerData::enAiHelp::BotMb2 || m_eAiHelp == data::SPlayerData::enAiHelp::BotMb3 || m_eAiHelp == data::SPlayerData::enAiHelp::BotMgp) {
          std::string l_sScriptFile = a_sAiScript + "/ai.lua";

          if (l_pFs->existFile(l_sScriptFile.c_str())) {
            std::string l_sScript = helpers::loadTextFile(l_sScriptFile);
            m_pLuaScript = new lua::CLuaScript_ai(l_sScript);

            if (m_pLuaScript->getError() == "") {
            }
            else {
              printf("LUA error: %s\n", m_pLuaScript->getError().c_str());
              delete m_pLuaScript;
              m_pLuaScript = nullptr;
            }
          }

          m_pAiControls = new CControllerAi_V2(a_iMarbleId, l_cAiData.serialize(), m_aMarbles, m_pLuaScript, a_cViewport);
        }
        else {
          std::string l_aScript[] = {
            a_sAiScript + "/aihelp.lua",
            a_sAiScript + "/ai.lua",
            ""
          };

          m_pLuaScript = nullptr;

          for (int i = 0; l_aScript[i] != "" && m_pLuaScript == nullptr; i++) {
            if (l_pFs->existFile(l_aScript[i].c_str())) {
              std::string l_sScript = helpers::loadTextFile(l_aScript[i]);
              m_pLuaScript = new lua::CLuaScript_ai(l_sScript);

              if (m_pLuaScript->getError() == "") {
                printf("Loaded lua script \"%s\"\".\n", l_aScript[i].c_str());
              }
              else {
                printf("LUA error: %s\n", m_pLuaScript->getError().c_str());
                delete m_pLuaScript;
                m_pLuaScript = nullptr;
              }
            }
          }


          m_pAiControls = new CControllerAiHelp_V2(m_iMarbleId, l_cAiData.serialize(), m_aMarbles, m_pLuaScript, a_cViewport);
        }

        if (m_pAiControls != nullptr && CGlobal::getInstance()->getSettingData().m_bDebugAI) {
          m_pAiControls->setDebug(true);

          irr::video::ITexture *l_pTexture = m_pAiControls->getDebugTexture();

          if (l_pTexture != nullptr) {
            irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cViewport.getWidth(), a_cViewport.getHeight());
            irr::core::dimension2du l_cSize   = l_cScreen;

            l_cSize.Width  /= 3;
            l_cSize.Height /= 3;

            CGlobal::getInstance()->getGuiEnvironment()->addImage(
              l_pTexture,
              irr::core::vector2di(a_cViewport.UpperLeftCorner.X, a_cViewport.LowerRightCorner.Y - l_cSize.Height)
            );
          }
        }
      }
    }

    CMarbleController::~CMarbleController() {
      if (m_pController != nullptr && m_bOwnsCtrl)
        delete m_pController;

      if (m_pAiControls != nullptr)
        delete m_pAiControls;
    }

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
    void CMarbleController::postControlMessage(bool &a_bLeft, bool &a_bRight, bool &a_bForward, bool &a_bBackward, bool &a_bBrake, bool &a_bRespawn, bool &a_bAutomatic) {
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

          IControllerAI::enMarbleMode l_eMode = IControllerAI::enMarbleMode::Default;

          m_pAiControls->getControlMessage(i, l_iBotX, l_iBotY, l_bBrakeBot, l_bRearBot, l_bRspnBot, l_eMode);

          switch (m_eAiHelp) {
            // Bot: marble is always controlled by AI
            case data::SPlayerData::enAiHelp::BotMgp:
            case data::SPlayerData::enAiHelp::BotMb2:
            case data::SPlayerData::enAiHelp::BotMb3:
              l_iCtrlX     = l_iBotX;
              l_iCtrlY     = l_iBotY;
              l_bBrake     = l_bBrakeBot;
              l_bRespawn  |= l_bRspnBot;
              a_bAutomatic = true;
              break;

            // High: Player needs to steer the marble, only in modes "jump" and "off-track"
            //       does the AI take complete control
            case data::SPlayerData::enAiHelp::High:
              if (l_eMode == IControllerAI::enMarbleMode::Jump || l_eMode == IControllerAI::enMarbleMode::OffTrack) {
                l_iCtrlX     = l_iBotX;
                a_bAutomatic = true;
              }
              else {
                if (l_iCtrlX > 0 && l_iBotX > 0) l_iCtrlX = l_iBotX;
                if (l_iCtrlX < 0 && l_iBotX < 0) l_iCtrlX = l_iBotX;
              }
              l_iCtrlY     = l_iBotY;
              l_bBrake     = l_bBrakeBot;
              l_bRespawn  |= l_bRspnBot;
              break;

            // Medium: The marble is controlled by the player unless in the modes "Jump" and "Off-Track"
            case data::SPlayerData::enAiHelp::Medium:
              l_iCtrlY     = l_iBotY;
              l_bBrake     = l_bBrakeBot;
              l_bRespawn  |= l_bRspnBot;
              break;

            // Low: The marbles is controlled by the player, in mode "Jump" the velocity is adjusted by the AI,
            //      and in mode "Off-Track" control is completely taken over by AI
            case data::SPlayerData::enAiHelp::Low:
              if (l_eMode == IControllerAI::enMarbleMode::Jump) {
                l_iCtrlY     = l_iBotY;
                l_bBrake     = l_bBrakeBot;
                a_bAutomatic = true;
              }
              else if (l_eMode == IControllerAI::enMarbleMode::OffTrack) {
                l_iCtrlX     = l_iBotX;
                l_iCtrlY     = l_iBotY;
                l_bBrake     = l_bBrakeBot;
                a_bAutomatic = true;
              }
              l_bRespawn  |= l_bRspnBot;
              break;

            case data::SPlayerData::enAiHelp::Display:
            case data::SPlayerData::enAiHelp::Off:
              break;
          }

          a_bLeft      = l_iBotX < -32;
          a_bRight     = l_iBotX >  32;
          a_bForward   = l_iBotY > 0;
          a_bBackward  = l_iBotY < 0;
          a_bBrake     = l_bBrakeBot;
          a_bRespawn   = l_bRspnBot;
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
        m_pController->updateControls(a_cEvent);
    }

    void CMarbleController::onMarbleMoved(int a_iMarbleId, const irr::core::vector3df &a_cNewPos, const irr::core::vector3df &a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp, bool a_bHasContact) { 
      int l_iIndex = a_iMarbleId - 10000;

      if (l_iIndex >= 0 && l_iIndex < 16) {
        m_aMarbles[l_iIndex].m_iMarbleId  = a_iMarbleId;
        m_aMarbles[l_iIndex].m_cPosition  = a_cNewPos;
        m_aMarbles[l_iIndex].m_cVelocity  = a_cVelocity;
        m_aMarbles[l_iIndex].m_cDirection = a_cCameraPos - a_cNewPos;
        m_aMarbles[l_iIndex].m_cCamera    = a_cCameraPos;
        m_aMarbles[l_iIndex].m_cCameraUp  = a_cCameraUp;
        m_aMarbles[l_iIndex].m_bContact   = a_bHasContact;
      }
    }

    void CMarbleController::onMarbleRespawn(int a_iMarbleId) { 
      if (m_pAiControls != nullptr)
        m_pAiControls->onMarbleRespawn(a_iMarbleId);

      if (m_pLuaScript != nullptr)
        m_pLuaScript->onplayerrespawn(a_iMarbleId, 3);
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CMarbleController::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
      if (m_pAiControls != nullptr)
        m_pAiControls->onCheckpoint(a_iMarbleId, a_iCheckpoint);

      if (m_pLuaScript != nullptr)
        m_pLuaScript->oncheckpoint(a_iMarbleId, a_iCheckpoint);
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

      if (m_pLuaScript != nullptr)
        m_pLuaScript->onraceposition(a_MarbleId, a_Position, a_Laps, a_DeficitAhead, a_DeficitLeader);
    }

    /**
    * Get the AI controller (if any)
    * @return the AI controller
    */
    IControllerAI* CMarbleController::getAiController() {
      return m_pAiControls;
    }


    /**
    * Callback called for every simulation step
    * @param a_iStep the current simulation step
    */
    void CMarbleController::onStep(int a_iStep) {
      if (m_pLuaScript != nullptr)
        m_pLuaScript->onstep(a_iStep);
    }

    /**
    * This function receives messages of type "Trigger"
    * @param a_TriggerId ID of the trigger
    * @param a_ObjectId ID of the marble that caused the trigger
    */
    void CMarbleController::onTrigger(irr::s32 a_iTriggerId, irr::s32 a_iObjectId) {
      if (m_pLuaScript != nullptr)
        m_pLuaScript->ontrigger(a_iObjectId, a_iTriggerId);
    }

    /**
    * The player has finished, hide the UI elements if necessary
    */
    void CMarbleController::playerFinished() {
      if (m_pController != nullptr)
        m_pController->playerFinished();
    }
 }
}