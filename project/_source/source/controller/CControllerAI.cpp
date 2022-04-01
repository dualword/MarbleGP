// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/CMessages.h>
#include <controller/CControllerAI.h>
#include <gui/CGuiAiDebug.h>
#include <math.h>
#include <cmath>

namespace dustbin {
  namespace controller {
    CControllerAI::CControllerAI(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue, scenenodes::CAiNode *a_pNode) : 
      CMarbleController(a_iMarbleId, a_sControls, a_pQueue), 
      m_fVel    (0.0f),
      m_pNode   (a_pNode),  
      m_pCurrent(nullptr),
      m_pAiDebug(nullptr)
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
            printf("%i: %.2f | %.2f\n", (*it2)->m_pThis->m_iIndex, l_fDummy, l_fDist);
            m_pCurrent = *it2;
            l_fDist = l_fDummy;
          }
        }
      }

      printf("Closest: %i\n", m_pCurrent != nullptr ? m_pCurrent->m_pThis->m_iIndex : -1);
    }

    irr::core::vector3df CControllerAI::getLookAhead(irr::f32 a_fDistance) {
      if (m_pCurrent != nullptr) {
        scenenodes::CAiNode::SAiLink *p = m_pCurrent;

        irr::core::vector3df l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_cPos);

        while (l_cPoint == m_pCurrent->m_cLinkLine.end) {
          m_pCurrent = *m_pCurrent->m_pNext->m_vNext.begin();
          irr::core::vector3df l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_cPos);
        }

        if (a_fDistance == 0.0f)
          return l_cPoint;

        irr::core::line3df l_cLine = irr::core::line3df(l_cPoint, m_pCurrent->m_cLinkLine.end);

        irr::f32 l_fLength = l_cLine.getLength();

        while (a_fDistance > l_fLength) {
          a_fDistance -= l_fLength;
          p = *p->m_pNext->m_vNext.begin();

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
      if (m_pCurrent != nullptr) {
        irr::core::vector3df l_cPoint = m_cPos - getLookAhead(0.0f);
        
        irr::f32 l_fOffset = 1.0f - std::fmin(1.0f, l_cPoint.getLengthSQ() / 9.0f);

        irr::f32 l_fLookAhead = 1.5f * l_fOffset * m_fVel;
        
        l_fLookAhead = std::fmax(l_fLookAhead, 15.0f);

        irr::core::vector3df l_cAhead1 = getLookAhead(       l_fLookAhead);
        irr::core::vector3df l_cAhead2 = getLookAhead(2.0f * l_fLookAhead);

        m_pNode->setClosest  (m_iMarbleId, irr::core::line3df(m_cPos, l_cAhead1));
        m_pNode->setLookAhead(m_iMarbleId, irr::core::line3df(m_cPos, l_cAhead2));

        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixLH(m_cCamPos, m_cPos + 1.5f * m_cCamUp, m_cCamUp);

        l_cMatrix.transformVect(l_cAhead1);
        l_cMatrix.transformVect(l_cAhead2);

        irr::core::vector2df l_cPoint1 = irr::core::vector2df(l_cAhead1.X, l_cAhead1.Z);
        irr::core::vector2df l_cPoint2 = irr::core::vector2df(l_cAhead2.X, l_cAhead2.Z);

        l_cPoint1.normalize();
        l_cPoint2.normalize();

        m_pAiDebug->clearLines();
        m_pAiDebug->addLine(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(0.0f, 1.0f)));
        m_pAiDebug->addLine(irr::core::line2df(irr::core::vector2df(), l_cPoint1));
        m_pAiDebug->addLine(irr::core::line2df(irr::core::vector2df(), l_cPoint2));

        irr::f32 l_fAngle1 = (l_cPoint1.X < 0.0f ? -1.0f : 1.0f) * (irr::f32)(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(0.0f, 1.0f))).getAngleWith(irr::core::line2df(irr::core::vector2df(), l_cPoint1));
        irr::f32 l_fAngle2 = (l_cPoint2.X < 0.0f ? -1.0f : 1.0f) * (irr::f32)(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(0.0f, 1.0f))).getAngleWith(irr::core::line2df(irr::core::vector2df(), l_cPoint2));

        irr::f32 l_fSteer = l_fAngle1 / 5.0f;

        if (l_fSteer >  1.0f) l_fSteer =  1.0f;
        if (l_fSteer < -1.0f) l_fSteer = -1.0f;

        irr::f32 l_fThrottle = m_fVel < 1.25f * l_cAhead1.Z ? 1.0f : 0.0f;

        irr::s8 l_iCtrlX = (irr::s8)(127.0f * l_fSteer   );
        irr::s8 l_iCtrlY = (irr::s8)(127.0f * l_fThrottle);

        bool l_bBrake = m_fVel > l_cAhead1.Z;

        messages::CMarbleControl l_cMarble = messages::CMarbleControl(m_iMarbleId, l_iCtrlX, l_iCtrlY, l_bBrake, false, false);
        m_pQueue->postMessage(&l_cMarble);
      }
    }

    void CControllerAI::setDebugGui(gui::CGuiAiDebug* a_pDebug) {
      m_pAiDebug = a_pDebug;
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
        }
      }
    }

    void CControllerAI::onMarbleRespawn(int a_iMarbleId) {
      if (a_iMarbleId == m_iMarbleId) {
        m_pCurrent = nullptr;
        printf("I have respawned.\n");
      }
    }
  }
}
