// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#ifdef _WINDOWS

#include <scenenodes/CMyCameraAnimator.h>
#include <messages/CSerializer64.h>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace scenenodes {

    CMyCameraAnimator::CMyCameraAnimator(irr::IrrlichtDevice *a_pDevice) {
      m_pDevice       = a_pDevice;
      m_bLeftDown     = false;
      m_bRightDown    = false;
      m_bLoaded       = false;
      m_bActive       = true;
      m_fAngleH       = 0.0f;
      m_fAngleV       = 0.0f;
      m_pCamera       = nullptr;
    }

    void CMyCameraAnimator::initPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam) {
      m_fAngleH -= 0.5f * ((irr::f32)m_vPosition.X - m_pDevice->getCursorControl()->getPosition().X);
      m_fAngleV += 0.5f * ((irr::f32)m_vPosition.Y - m_pDevice->getCursorControl()->getPosition().Y);

      while (m_fAngleV >  80.0f) m_fAngleV =  80.0f;
      while (m_fAngleV < -80.0f) m_fAngleV = -80.0f;

      irr::f32 l_fCamX = (irr::f32)(1.0f * sin(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f)),
               l_fCamY = (irr::f32)(1.0f * sin(m_fAngleV * M_PI / 180.0f)),
               l_fCamZ = (irr::f32)(1.0f * cos(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f));

      m_vCamLookAt = irr::core::vector3df(l_fCamX, l_fCamY, l_fCamZ);
      a_pCam->setTarget(a_pCam->getPosition() + m_vCamLookAt);
    }

    void CMyCameraAnimator::animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 m_iTimeMs) {
      if (a_pNode->getType() == irr::scene::ESNT_CAMERA && m_bActive) {
        irr::scene::ICameraSceneNode *l_pCam = reinterpret_cast<irr::scene::ICameraSceneNode *>(a_pNode);

        if (l_pCam != m_pCamera)
          m_pCamera = l_pCam;

        if (m_bLoaded) {
          l_pCam->setPosition(m_vCamPos);
          l_pCam->setTarget(m_vCamLookAt);
          m_bLoaded = false;
        }

        m_vCamPos = a_pNode->getPosition();

        l_pCam->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));

        if (m_bLeftDown) {
          initPositionAndLookAt(l_pCam);
        }
        else if (m_bRightDown) {
          irr::core::vector3df vStrafe = m_vCamLookAt.crossProduct(irr::core::vector3df(0,1,0)),
                               v       = l_pCam->getPosition() + ((irr::f32)(m_vPosition.Y - m_pDevice->getCursorControl()->getPosition().Y)) * m_vCamLookAt + ((irr::f32)(m_vPosition.X - m_pDevice->getCursorControl()->getPosition().X)) * vStrafe;

          l_pCam->setPosition(v);
          l_pCam->setTarget  (v + m_vCamLookAt);
        }
      }

      m_vPosition = m_pDevice->getCursorControl()->getPosition();
    }

    irr::scene::ISceneNodeAnimator *CMyCameraAnimator::createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pNewManager) {
      return nullptr;
    }

    irr::scene::ESCENE_NODE_ANIMATOR_TYPE CMyCameraAnimator::getType() const {
      return irr::scene::ESNAT_UNKNOWN;
    }

    bool CMyCameraAnimator::hasFinished(void) const {
      return false;
    }

    bool CMyCameraAnimator::isEventReceiverEnabled() const {
      return true;
    }

    bool CMyCameraAnimator::OnEvent(const irr::SEvent &a_cEvent) {
      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        m_bLeftDown  = a_cEvent.MouseInput.isLeftPressed ();
        m_bRightDown = a_cEvent.MouseInput.isRightPressed();
      }
      return false;
    }

    void CMyCameraAnimator::setData(const irr::core::vector3df& a_cPos, irr::f32 a_fAngleV, irr::f32 a_fAngleH) {
      m_vCamPos = a_cPos;
      m_fAngleV = a_fAngleV;
      m_fAngleH = a_fAngleH;

      if (m_pCamera != nullptr) {
        m_bLeftDown  = true;
        m_bRightDown = true;

        m_pCamera->setPosition(m_vCamPos);
        animateNode(m_pCamera, 0);

        m_bLeftDown  = false;
        m_bRightDown = false;
      }
    }
  }
}

#endif