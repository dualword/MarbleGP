// (w) 2021 by Dustbin::Games / Christian Keimel
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
    }

    void CMyCameraAnimator::setPositionLookAt(irr::scene::ICameraSceneNode *a_pCam, const irr::core::vector3df &a_cPosition, const irr::core::vector3df &a_cLookAt) {
      a_pCam->setPosition(a_cPosition);
      a_pCam->setTarget(a_cLookAt);

      irr::core::vector3df l_vLook = (a_pCam->getTarget() - a_pCam->getPosition()).normalize();

      m_fAngleV = (irr::f32)(asin(l_vLook.Y) * 180.0f / M_PI);
      m_fAngleH = (irr::f32)((acos(l_vLook.Z) * 180.0 / M_PI) / cos(m_fAngleV * M_PI / 180));
    }

    void CMyCameraAnimator::copyPositionAndLookAt(irr::scene::ICameraSceneNode *a_pCam, irr::scene::ICameraSceneNode *a_pOther) {
      a_pCam->setPosition(a_pOther->getPosition());
      a_pCam->setTarget(a_pOther->getTarget());

      irr::core::vector3df l_vLook = (a_pCam->getTarget() - a_pCam->getPosition()).normalize();

      m_fAngleV = (irr::f32)(asin(l_vLook.Y) * 180.0f / M_PI);
      m_fAngleH = (irr::f32)(irr::core::vector2df(l_vLook.X, l_vLook.Z).getAngle()) + 90.0f;

      printf("%.2f, %.2f\n", m_fAngleV, m_fAngleH);
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

    void CMyCameraAnimator::setIsActive(bool a_bIsActive) {
      m_bActive = a_bIsActive;
    }

    bool CMyCameraAnimator::isActive() {
      return m_bActive;
    }

    std::string CMyCameraAnimator::saveData() {
      messages::CSerializer64 *l_pData = new messages::CSerializer64();

      l_pData->addVector3df(m_vCamLookAt);
      l_pData->addVector3df(m_vCamPos   );
      l_pData->addF32(m_fAngleH);
      l_pData->addF32(m_fAngleV);

      std::string l_sRet = (const char *)l_pData->getBuffer();
      delete l_pData;
      return l_sRet;
    }

    void CMyCameraAnimator::loadData(const std::string& a_sData, irr::core::vector3df &a_cPos, irr::core::vector3df &a_cTgt) {
      messages::CSerializer64 *l_pData = new messages::CSerializer64(a_sData.c_str());

      m_vCamLookAt = l_pData->getVector3df();
      m_vCamPos    = l_pData->getVector3df();
      m_fAngleH    = l_pData->getF32();
      m_fAngleV    = l_pData->getF32();

      m_bLoaded = true;

      delete l_pData;

      a_cPos = m_vCamPos;
      a_cTgt = m_vCamLookAt;
    }
  }
}

