// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <controller/CControllerAI.h>
#include <math.h>
#include <cmath>

namespace dustbin {
  namespace controller {
    CControllerAI::CControllerAI(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue, scenenodes::CAiNode *a_pNode) : 
      CMarbleController(a_iMarbleId, a_sControls, a_pQueue), 
      m_fVel       (0.0f),
      m_iLastChange(0),
      m_pNode      (a_pNode),  
      m_pCurrent   (nullptr),
      m_bBrake     (false),
      m_bDebug     (false)
    {
      if (m_pNode != nullptr)
        m_vPath = a_pNode->getPath();
    }

    CControllerAI::~CControllerAI() {
    }

    void CControllerAI::selectClosestLink() {
      irr::f32 l_fDist = 0.0f;

      for (std::vector<scenenodes::CAiNode::SAiPathNode*>::iterator it = m_vPath.begin(); it != m_vPath.end(); it++) {
        for (std::vector<scenenodes::CAiNode::SAiLink*>::iterator it2 = (*it)->m_vNext.begin(); it2 != (*it)->m_vNext.end(); it2++) {
          irr::core::vector3df l_cPoint = (*it2)->m_cLinkLine.getClosestPoint(m_cPos);
          irr::f32 l_fDummy = m_cPos.getDistanceFrom(l_cPoint);

          if ((it == m_vPath.begin() && it2 == (*it)->m_vNext.begin()) || l_fDummy < l_fDist) {
            m_pCurrent = *it2;
            l_fDist = l_fDummy;
          }
        }
      }

      m_iLastChange = 0;
    }

    irr::core::vector3df CControllerAI::getLookAhead(irr::f32 a_fDistance) {
      if (m_pCurrent != nullptr) {
        scenenodes::CAiNode::SAiLink *p = m_pCurrent;

        irr::core::vector3df l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_cPos);

        m_pCurrent->m_pThis->m_iIndex;

        while (l_cPoint == m_pCurrent->m_cLinkLine.end) {
          if (m_mChoices.find(m_pCurrent->m_pThis->m_iIndex) == m_mChoices.end()) {
            m_mChoices[m_pCurrent->m_pThis->m_iIndex] = std::rand() % m_pCurrent->m_pNext->m_vNext.size();
          }

          m_pCurrent = m_pCurrent->m_pNext->m_vNext[m_mChoices[m_pCurrent->m_pThis->m_iIndex]];
          l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_cPos);
          m_iLastChange = 0;
        }

        if (a_fDistance == 0.0f)
          return l_cPoint;

        irr::core::line3df l_cLine = irr::core::line3df(l_cPoint, m_pCurrent->m_cLinkLine.end);

        irr::f32 l_fLength = l_cLine.getLength();

        while (a_fDistance > l_fLength) {
          a_fDistance -= l_fLength;

          if (m_mChoices.find(p->m_pThis->m_iIndex) == m_mChoices.end()) {
            m_mChoices[p->m_pThis->m_iIndex] = std::rand() % p->m_pNext->m_vNext.size();
          }

          p = p->m_pNext->m_vNext[m_mChoices[p->m_pThis->m_iIndex]];

          l_cLine   = p->m_cLinkLine;
          l_fLength = p->m_fLinkLength;
          l_cPoint  = l_cLine.start;
        }

        return l_cPoint + a_fDistance * (l_cLine.end - l_cLine.start).normalize();
      }
      else printf("Error: no path!\n");

      return irr::core::vector3df(0.0f);
    }

    /**
    * This message must be implemented by all descendants. If called
    * it posts a control message to the queue.
    */
    void CControllerAI::postControlMessage() {
      if (m_pQueue != nullptr) {
        messages::CMarbleControl *p = getControlMessage();
        m_pQueue->postMessage(p);
        delete p;
      }
    }

    void CControllerAI::onObjectMoved(int a_iObjectId, const irr::core::vector3df& a_cNewPos) {
    }

    void CControllerAI::onMarbleMoved(int a_iMarbleId, const irr::core::vector3df& a_cNewPos, const irr::core::vector3df& a_cVelocity, const irr::core::vector3df &a_cCameraPos, const irr::core::vector3df &a_cCameraUp) {
      if (a_iMarbleId == m_iMarbleId) {
        m_cPos    = a_cNewPos;
        m_cVel    = a_cVelocity;
        m_cCamPos = a_cCameraPos;
        m_cCamUp  = a_cCameraUp;
        
        m_fVel = a_cVelocity.getLength();

        if (m_pCurrent == nullptr) {
          selectClosestLink();
          m_iLastChange = 0;
        }
      }
    }

    void CControllerAI::onMarbleRespawn(int a_iMarbleId) {
      if (a_iMarbleId == m_iMarbleId) {
        m_pCurrent = nullptr;
      }
    }

    messages::CMarbleControl *CControllerAI::getControlMessage() { 
      if (m_iLastChange > 1200) {
        m_pCurrent = nullptr;
        printf("Search for new AI path!\n");
      }
      else if (m_pCurrent != nullptr) {
        std::vector<irr::core::line3df> l_vDebug;

        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixLH(m_cCamPos, m_cPos + 1.5f * m_cCamUp, m_cCamUp);

        irr::core::vector3df l_cPoint = getLookAhead(0.0f);
        l_cMatrix.transformVect(l_cPoint);

        irr::f32 l_fOffset = 1.0f - std::fmin(1.0f, std::abs(l_cPoint.X / 2.0f));

        irr::f32 l_fLookAhead1 = 1.5f * l_fOffset * m_fVel;
        irr::f32 l_fLookAhead2 = 2.5f * l_fOffset * m_fVel;

        l_fLookAhead1 = std::fmax(l_fLookAhead1, 15.0f);
        l_fLookAhead2 = std::fmax(l_fLookAhead2, 25.0f);

        irr::core::vector3df l_cAhead1 = getLookAhead(l_fLookAhead1);
        irr::core::vector3df l_cAhead2 = getLookAhead(l_fLookAhead2);

        if (m_bDebug) {
          l_vDebug.push_back(irr::core::line3df(m_cPos   , l_cAhead1));
          l_vDebug.push_back(irr::core::line3df(l_cAhead1, l_cAhead2));
        }

        l_cMatrix.transformVect(l_cAhead1);
        l_cMatrix.transformVect(l_cAhead2);

        irr::core::vector2df l_cPoint1 = irr::core::vector2df(l_cAhead1.X, l_cAhead1.Z);
        irr::core::vector2df l_cPoint2 = irr::core::vector2df(l_cAhead2.X, l_cAhead2.Z);

        irr::f32 l_fAngleNew = (irr::f32)irr::core::line2df(irr::core::vector2df(0.0f), l_cPoint1).getAngleWith(irr::core::line2df(l_cPoint1, l_cPoint2));

        irr::f32 l_fFactor = 2.0f - (irr::f32)(std::fmin(90.0, l_fAngleNew) / 45.0);
        l_fFactor = std::fmax(0.5f, l_fFactor);

        irr::core::vector3df l_cNewPoint = getLookAhead(std::fmax(15.0f, l_fFactor * l_fOffset * m_fVel));

        if (m_bDebug) {
          l_vDebug.push_back(irr::core::line3df(m_cPos, l_cNewPoint));
        }

        l_cMatrix.transformVect(l_cNewPoint);

        l_cPoint1.normalize();

        irr::f32 l_fAngle = (l_cPoint1.X < 0.0f ? -1.0f : 1.0f) * (irr::f32)(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(0.0f, 1.0f))).getAngleWith(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(l_cNewPoint.X, l_cNewPoint.Z)));// l_cPoint1));

        irr::f32 l_fSteer = l_fAngle / 5.0f;

        if (l_fSteer >  0.75f) l_fSteer =  1.0f;
        if (l_fSteer < -0.75f) l_fSteer = -1.0f;

        if (l_fSteer >  1.0f) l_fSteer =  1.0f;
        if (l_fSteer < -1.0f) l_fSteer = -1.0f;

        irr::f32 l_fSpeedFactor = 3.0f - std::min(2.0f, std::abs(l_fAngle / 3.5f));

        irr::f32 l_fVel = 1.1f * l_fSpeedFactor * l_cNewPoint.Z;  // 1.1f needs to be verified

        if (m_pCurrent->m_pNext->m_fMaxVel > 0.0f) {
          if (l_fVel > m_pCurrent->m_pNext->m_fMaxVel)
            l_fVel = m_pCurrent->m_pNext->m_fMaxVel;
        }
        else if (m_pCurrent->m_pNext->m_fMinVel > 0.0f) {
          if (l_fVel < m_pCurrent->m_pNext->m_fMinVel)
            l_fVel = m_pCurrent->m_pNext->m_fMinVel;
        }

        m_fThrottle = m_fVel < l_fVel ? 1.0f : -1.0f;

        m_iCtrlX = (irr::s8)(127.0f * l_fSteer * l_fSteer * (l_fSteer < 0.0f ? -1.0f : 1.0f));
        m_iCtrlY = (irr::s8)(127.0f * m_fThrottle);

        m_bBrake = m_fVel > l_fVel;

        if (m_bDebug) m_pNode->setDebugLines(m_iMarbleId, l_vDebug);
        m_iLastChange++;

        return new messages::CMarbleControl(m_iMarbleId, m_iCtrlX, m_iCtrlY, m_bBrake, false, false);
      }

      return nullptr;
    }

    void CControllerAI::setDebug(bool a_bDebug) {
      m_bDebug = a_bDebug;
    }
  }
}
