// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCmdCameraCtrl.h>
#include <CDefines.h>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace commands {
      CCmdCameraCtrl::CCmdCameraCtrl(irr::scene::ISceneManager* a_pSmgr, irr::gui::IGUIEnvironment *a_pGui) : 
        ICommand ("CameraCtrl"), 
        m_pSmgr  (a_pSmgr),
        m_pGui   (a_pGui), 
        m_pCamera(nullptr), 
        m_fAngleH(0.0), 
        m_fAngleV(0.0) 
      {
        for (int i = 0; i < 3; i++)
          m_bMouseDown[i] = false;

        m_cCamPos = irr::core::vector3df(50.0f, 25.0f, 0.0f);

        m_fAngleH = 270.0f;
        m_fAngleV = -25.0f;
      }

      CCmdCameraCtrl::~CCmdCameraCtrl() {
      }

      /**
      * If this method returns "true" the command is finished and
      * will be removed from the vector of active commands
      * @return true if the command is finished and can be removed
      */
      bool CCmdCameraCtrl::isFinished() {
        return false;
      }

      void CCmdCameraCtrl::updateCameraTarget() {
        irr::f32 l_fCamX = (irr::f32)(1.0f * sin(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f));
        irr::f32 l_fCamY = (irr::f32)(1.0f * sin(m_fAngleV * M_PI / 180.0f));
        irr::f32 l_fCamZ = (irr::f32)(1.0f * cos(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f));

        m_pCamera->setTarget(m_pCamera->getPosition() + irr::core::vector3df(l_fCamX, l_fCamY, l_fCamZ));
      }

      /**
      * The Irrlicht event handler method. Implement it if the command
      * needs to react to user input
      */
      bool CCmdCameraCtrl::OnEvent(const irr::SEvent &a_cEvent) {
        bool l_bRet = false;

        if (a_cEvent.EventType == irr::EET_USER_EVENT) {
          if (a_cEvent.UserEvent.UserData1 == c_iSceneWillBeCleared) {
            m_cCamPos = m_pCamera->getPosition();

            m_pCamera = nullptr;
          }
          else if (a_cEvent.UserEvent.UserData1 == c_iSceneLoaded) {
            m_pCamera = m_pSmgr->addCameraSceneNode();
            m_pCamera->setIsDebugObject(true);
            m_pCamera->setPosition(m_cCamPos);
            updateCameraTarget();

            m_pSmgr->setActiveCamera(m_pCamera);
          }
        }
        else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
          if (m_pCamera != nullptr) {
            irr::core::vector2di l_cPos = irr::core::vector2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            // We only consider the mouse down events if no UI element is hovered
            bool l_bUiHover = m_pGui->getRootGUIElement()->getElementFromPoint(l_cPos) != m_pGui->getRootGUIElement();

            switch (a_cEvent.MouseInput.Event) {
              case irr::EMIE_LMOUSE_PRESSED_DOWN: if (!l_bUiHover) { m_bMouseDown[0] = true; l_bRet = true; m_cPosition = l_cPos; } break;
              case irr::EMIE_RMOUSE_PRESSED_DOWN: if (!l_bUiHover) { m_bMouseDown[1] = true; l_bRet = true; m_cPosition = l_cPos; } break;
              case irr::EMIE_MMOUSE_PRESSED_DOWN: if (!l_bUiHover) { m_bMouseDown[2] = true; l_bRet = true; m_cPosition = l_cPos; } break;

              case irr::EMIE_LMOUSE_LEFT_UP: if (m_bMouseDown[0]) { m_bMouseDown[0] = false; l_bRet = true; } break;
              case irr::EMIE_RMOUSE_LEFT_UP: if (m_bMouseDown[1]) { m_bMouseDown[1] = false; l_bRet = true; } break;
              case irr::EMIE_MMOUSE_LEFT_UP: if (m_bMouseDown[2]) { m_bMouseDown[2] = false; l_bRet = true; } break;

              case irr::EMIE_MOUSE_MOVED: {
                if (m_bMouseDown[0]) {
                  m_fAngleH += (irr::f64)(l_cPos.X - m_cPosition.X);
                  m_fAngleV += (irr::f64)(m_cPosition.Y - l_cPos.Y);

                  if (m_fAngleV > 80.0) m_fAngleV =  80.0;
                  if (m_fAngleV < -80.0) m_fAngleV = -80.0;

                  m_cPosition = l_cPos;

                  updateCameraTarget();
                  l_bRet = true;
                }
                else if (m_bMouseDown[1]) {
                  irr::core::vector3df l_cLookAt = (m_pCamera->getTarget() - m_pCamera->getPosition()).normalize();
                  irr::core::vector3df l_cSide   = l_cLookAt.crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f));

                  m_pCamera->setPosition(m_pCamera->getPosition() + l_cLookAt * (irr::f32)(m_cPosition.Y - l_cPos.Y) + l_cSide * (irr::f32)(m_cPosition.X - l_cPos.X));
                  updateCameraTarget();

                  m_cPosition = l_cPos;

                  l_bRet = true;
                }
                else if (m_bMouseDown[2]) {
                  irr::core::vector3df l_cLookAt = (m_pCamera->getTarget() - m_pCamera->getPosition()).normalize();
                  irr::core::vector3df l_cSide   = l_cLookAt.crossProduct(irr::core::vector3df(0.0f, 1.0f, 0.0f));
                  irr::core::vector3df l_cUp     = l_cLookAt.crossProduct(l_cSide);

                  m_pCamera->setPosition(m_pCamera->getPosition() + l_cSide * (irr::f32)(m_cPosition.X - l_cPos.X) + l_cUp * (irr::f32)(l_cPos.Y - m_cPosition.Y));
                  updateCameraTarget();

                  m_cPosition = l_cPos;

                  l_bRet = true;
                }
                break;
              }

              case irr::EMIE_MOUSE_WHEEL: {
                irr::core::vector3df l_cLookAt = (m_pCamera->getTarget() - m_pCamera->getPosition()).normalize();
                m_pCamera->setPosition(m_pCamera->getPosition() + l_cLookAt * 2.5f * a_cEvent.MouseInput.Wheel);
                updateCameraTarget();
                l_bRet = true;
                break;
              }

              default:
                break;
            }
          }
        }

        return l_bRet;
      }
  }
}