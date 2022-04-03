// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CCameraTouchAnimator.h>
#include <messages/CSerializer64.h>

#define _USE_MATH_DEFINES

#include <math.h>

namespace dustbin {
  namespace scenenodes {

    CMyCameraAnimator::CMyCameraAnimator(irr::IrrlichtDevice *a_pDevice) :
      m_fAngleV   (0.0f),
      m_fAngleH   (0.0f),
      m_iTouchCnt (0),
      m_bReady    (false),
      m_pCamera   (nullptr)
    {
      m_aTouchID[0] = -1;
      m_aTouchID[1] = -1;
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
    }

    void CMyCameraAnimator::animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 a_iTimeMs) {
      if (a_pNode->getType() == irr::scene::ESNT_CAMERA) {
        irr::scene::ICameraSceneNode *l_pCam = reinterpret_cast<irr::scene::ICameraSceneNode *>(a_pNode);

        if (l_pCam != m_pCamera)
          m_pCamera = l_pCam;

        if (!m_bReady) {
          l_pCam->updateAbsolutePosition();

          m_vCamLookAt = l_pCam->getRotation().rotationToDirection();
          m_vCamPos    = l_pCam->getAbsolutePosition();
          m_bReady = true;
        }

        l_pCam->setUpVector(irr::core::vector3df(0.0f, 1.0f, 0.0f));
        l_pCam->setTarget  (m_vCamPos + m_vCamLookAt);
        l_pCam->setPosition(m_vCamPos);
      }
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
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        switch (a_cEvent.TouchInput.Event) {
          case irr::ETIE_PRESSED_DOWN:
            switch (m_iTouchCnt) {
              case 0:
                m_aTouchID [0]   = a_cEvent.TouchInput.ID;
                m_vPosition[0].X = a_cEvent.TouchInput.X;
                m_vPosition[0].Y = a_cEvent.TouchInput.Y;
                m_iTouchCnt = 1;
                l_bRet = true;
                break;

              case 1:
                m_aTouchID [1]   = a_cEvent.TouchInput.ID;
                m_vPosition[1].X = a_cEvent.TouchInput.X;
                m_vPosition[1].Y = a_cEvent.TouchInput.Y;
                m_iTouchCnt = 2;

                m_cLine = irr::core::line2di(m_vPosition[0], m_vPosition[1]);

                l_bRet = true;
                break;

              default:
                // Nothing to do with more than two fingers
                break;
            }
            break;

          case irr::ETIE_MOVED: {
            irr::core::position2di l_aOld[2];
            irr::core::line2di     l_cOld = m_cLine;

            for (int i = 0; i < m_iTouchCnt; i++) {
              l_aOld[i] = m_vPosition[i];
              if (a_cEvent.TouchInput.ID == m_aTouchID[i]) {
                m_vPosition[i] = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

                if (i == 1) {
                  m_cLine = irr::core::line2di(m_vPosition[0], m_vPosition[1]);
                }
              }
            }

            if (m_iTouchCnt == 1) {
              // Rotate Camera
              m_fAngleH -= 0.25f * ((irr::f32)l_aOld[0].X - m_vPosition[0].X);
              m_fAngleV += 0.25f * ((irr::f32)l_aOld[0].Y - m_vPosition[0].Y);

              while (m_fAngleV >  80.0f) m_fAngleV =  80.0f;
              while (m_fAngleV < -80.0f) m_fAngleV = -80.0f;

              irr::f32 l_fCamX = (irr::f32)(1.0f * sin(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f));
              irr::f32 l_fCamY = (irr::f32)(1.0f * sin(m_fAngleV * M_PI / 180.0f));
              irr::f32 l_fCamZ = (irr::f32)(1.0f * cos(m_fAngleH * M_PI / 180.0f) * cos(m_fAngleV * M_PI / 180.0f));

              m_vCamLookAt = irr::core::vector3df(l_fCamX, l_fCamY, l_fCamZ);
            }
            else if (m_iTouchCnt == 2) {
              // Move Camera
              irr::core::vector3df l_cForeward = m_vCamLookAt.normalize() * -0.25f * (irr::f32)(l_cOld.getLength() - m_cLine.getLength());
              irr::core::vector3df l_cSideward = ((irr::f32)(l_cOld.getMiddle().X - m_cLine.getMiddle().X)) * m_vCamLookAt.crossProduct(irr::core::vector3df(0, 1, 0));
              irr::core::vector3df l_cUpward   = ((irr::f32)(l_cOld.getMiddle().Y - m_cLine.getMiddle().Y)) * m_vCamLookAt.crossProduct(irr::core::vector3df(1, 0, 0));

              m_vCamPos += l_cForeward + l_cSideward - l_cUpward;
            }
            break;
          }

          case irr::ETIE_LEFT_UP:
            for (int i = 0; i < 2; i++) {
              if (m_aTouchID[i] == a_cEvent.TouchInput.ID) {
                if (i == 0) {
                  m_aTouchID[0] = m_aTouchID[1];
                  m_aTouchID[1] = -1;

                  m_vPosition[0] = m_vPosition[1];
                }
                else {
                  m_aTouchID[1] = -1;
                }
                m_iTouchCnt--;
              }
            }
            break;

          default:
            break;
        }
      }

      return l_bRet;
    }

    void CMyCameraAnimator::setData(const irr::core::vector3df& a_cPos, irr::f32 a_fAngleV, irr::f32 a_fAngleH) {
      m_vCamPos = a_cPos;
      m_fAngleV = a_fAngleV;
      m_fAngleH = a_fAngleH;

      if (m_pCamera != nullptr) {
        m_pCamera->setPosition(m_vCamPos);
        animateNode(m_pCamera, 0);
      }
    }
  }
}

