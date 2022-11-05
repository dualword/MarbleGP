// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <_generated/messages/CMessages.h>
#include <controller/CControllerAI.h>
#include <math.h>
#include <cmath>

namespace dustbin {
  namespace controller {
    CControllerAI::CControllerAI(int a_iMarbleId, const std::string& a_sControls, threads::IQueue* a_pQueue, scenenodes::CAiNode *a_pNode, data::SMarblePosition *a_pMarbles) : 
      m_iMarbleId  (a_iMarbleId),
      m_iIndex     (a_iMarbleId - 10000),
      m_iLastChange(0),
      m_fLastOffset(0.0f),
      m_pNode      (a_pNode),
      m_pMarbles   (a_pMarbles),
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
          irr::core::vector3df l_cPoint = (*it2)->m_cLinkLine.getClosestPoint(m_pMarbles[m_iIndex].m_cPosition);
          irr::f32 l_fDummy = m_pMarbles[m_iIndex].m_cPosition.getDistanceFrom(l_cPoint);

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

        irr::core::vector3df l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_pMarbles[m_iIndex].m_cPosition);

        while (l_cPoint == m_pCurrent->m_cLinkLine.end) {
          if (m_mChoices.find(m_pCurrent->m_pThis->m_iIndex) == m_mChoices.end()) {
            m_mChoices[m_pCurrent->m_pThis->m_iIndex] = std::rand() % m_pCurrent->m_pNext->m_vNext.size();
          }

          m_pCurrent = m_pCurrent->m_pNext->m_vNext[m_mChoices[m_pCurrent->m_pThis->m_iIndex]];
          l_cPoint = m_pCurrent->m_cLinkLine.getClosestPoint(m_pMarbles[m_iIndex].m_cPosition);
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

    void CControllerAI::onMarbleRespawn(int a_iMarbleId) {
      if (a_iMarbleId == m_iMarbleId) {
        m_pCurrent = nullptr;
      }
    }

    /**
    * Notify the controller about a passed checkpoint
    * @param a_iMarbleId the marble that passed the checkpoint
    * @param a_iCheckpoint the passed checkpoint
    */
    void CControllerAI::onCheckpoint(int a_iMarbleId, int a_iCheckpoint) {
    }

    /**
    * This function receives messages of type "RacePosition"
    * @param a_MarbleId ID of the marble
    * @param a_Position Position of the marble
    * @param a_Laps The current lap of the marble
    * @param a_DeficitAhead Deficit of the marble on the marble ahead in steps
    * @param a_DeficitLeader Deficit of the marble on the leader in steps
    */
    void CControllerAI::onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) {
    }

    /**
    * Get the control values for the marble
    * @param a_iMarbleId [out] ID of the marble this controller controls
    * @param a_iCtrlX [out] the steering value 
    * @param a_iCtrlY [out] the throttle value
    * @param a_bBrake [out] is the brake active?
    * @param a_bRearView [out] does the marble look to the back?
    * @param a_bRespawn [out] does the marble want a manual respawn?
    * @param a_eMode [out] the AI mode the marble is currently in
    * @param a_cPoint1 [out] the first point for the AI calculation
    * @param a_cPoint2 [out] the second point for the AI calculation
    */
    bool CControllerAI::getControlMessage(
      irr::s32 &a_iMarbleId, 
      irr::s8 &a_iCtrlX, 
      irr::s8 &a_iCtrlY, 
      bool &a_bBrake, 
      bool &a_bRearView, 
      bool &a_bRespawn, 
      enMarbleMode &a_eMode, 
      irr::core::vector3df &a_cPoint1, 
      irr::core::vector3df &a_cPoint2
    ) { 
      bool l_bRespawn = false;

      if (m_pCurrent == nullptr) {
        selectClosestLink();
        m_iLastChange = 0;
      }

      if (m_iLastChange > 1200) {
        m_pCurrent = nullptr;
        // printf("Search for new AI path!\n");
        selectClosestLink();
      }

      if (m_pCurrent == nullptr)
        return false;

      if (m_pCurrent != nullptr) {
        irr::f32 l_fVelocity = m_pMarbles[m_iIndex].m_cVelocity.getLength();

        std::vector<irr::core::line3df> l_vDebug;

        irr::core::matrix4 l_cMatrix;
        l_cMatrix = l_cMatrix.buildCameraLookAtMatrixLH(
          m_pMarbles[m_iIndex].m_cCamera + m_pMarbles[m_iIndex].m_cPosition + 3.0f * m_pMarbles[m_iIndex].m_cCameraUp,
          m_pMarbles[m_iIndex].m_cPosition + 1.5f * m_pMarbles[m_iIndex].m_cCameraUp, 
          m_pMarbles[m_iIndex].m_cCameraUp
        );

        irr::core::vector3df l_cPoint = getLookAhead(0.0f);
        l_bRespawn = l_cPoint.getDistanceFromSQ(m_pMarbles[m_iIndex].m_cPosition) > 10000;
        l_cMatrix.transformVect(l_cPoint);

        irr::core::vector3df l_cOffset = getLookAhead(l_fVelocity);
        l_cMatrix.transformVect(l_cOffset);

        irr::f32 l_fOffset = std::abs(l_cPoint.X / 2.0f);

        bool l_bRoll = false;

        if ((l_cPoint.X < 0.0f && l_cOffset.X > 0.0f) || (l_cPoint.X > 0.0f && l_cOffset.X < 0.0f)) {
          l_fOffset /= 3.0f;

          if (l_fOffset < m_fLastOffset)
            l_bRoll = true;
        }

        l_fOffset = 1.0f - std::fmin(1.0f, l_fOffset);


        irr::f32 l_fLookAhead1 = 1.5f * l_fOffset * l_fVelocity;
        irr::f32 l_fLookAhead2 = 2.5f * l_fOffset * l_fVelocity;

        l_fLookAhead1 = std::fmax(l_fLookAhead1, 15.0f);
        l_fLookAhead2 = std::fmax(l_fLookAhead2, 25.0f);

        irr::core::vector3df l_cAhead1 = getLookAhead(l_fLookAhead1);
        irr::core::vector3df l_cAhead2 = getLookAhead(l_fLookAhead2);

        if (m_bDebug) {
          l_vDebug.push_back(irr::core::line3df(m_pMarbles[m_iIndex].m_cPosition, l_cAhead1));
          l_vDebug.push_back(irr::core::line3df(l_cAhead1, l_cAhead2));
        }

        l_cMatrix.transformVect(l_cAhead1);
        l_cMatrix.transformVect(l_cAhead2);

        irr::core::vector2df l_cPoint1 = irr::core::vector2df(l_cAhead1.X, l_cAhead1.Z);
        irr::core::vector2df l_cPoint2 = irr::core::vector2df(l_cAhead2.X, l_cAhead2.Z);

        irr::f32 l_fAngleNew = (irr::f32)irr::core::line2df(irr::core::vector2df(0.0f), l_cPoint1).getAngleWith(irr::core::line2df(l_cPoint1, l_cPoint2));

        irr::f32 l_fFactor = 2.0f - (irr::f32)(std::fmin(90.0, l_fAngleNew) / 45.0);
        l_fFactor = std::fmax(0.5f, l_fFactor);

        irr::core::vector3df l_cNewPoint = getLookAhead(std::fmax(15.0f, l_fFactor * l_fOffset * l_fVelocity));

        if (m_bDebug) {
          l_vDebug.push_back(irr::core::line3df(m_pMarbles[m_iIndex].m_cPosition, l_cNewPoint));
        }

        l_cMatrix.transformVect(l_cNewPoint);

        l_cPoint1.normalize();

        irr::f32 l_fAngle = (l_cNewPoint.X < 0.0f ? -1.0f : 1.0f) * (irr::f32)(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(0.0f, 1.0f))).getAngleWith(irr::core::line2df(irr::core::vector2df(), irr::core::vector2df(l_cNewPoint.X, l_cNewPoint.Z)));// l_cPoint1));

        irr::f32 l_fSteer = l_fAngle / 5.0f;

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

        m_fThrottle = l_fVelocity < l_fVel ? 1.0f : -1.0f;

        m_iCtrlX = (irr::s8)(127.0f * l_fSteer * l_fSteer * (l_fSteer < 0.0f ? -1.0f : 1.0f));
        m_iCtrlY = (irr::s8)(127.0f * m_fThrottle);

        m_bBrake = l_fVelocity > 1.1f * l_fVel && (!l_bRoll || l_fVelocity > 1.15f * l_fVel);

        if (m_bDebug) m_pNode->setDebugLines(m_iMarbleId, l_vDebug);
        m_iLastChange++;

        // printf("%5i%5i %s %5.2f\n", m_iCtrlX, m_iCtrlY, m_bBrake ? "B" : " ", m_fLastOffset - l_fOffset);
        m_fLastOffset = l_fOffset;
      }

      a_iMarbleId = m_iMarbleId;
      a_iCtrlX    = m_iCtrlX;
      a_iCtrlY    = m_iCtrlY;
      a_bBrake    = m_bBrake;
      a_bRearView = false;
      a_bRespawn  = l_bRespawn;

      return true;
    }

    void CControllerAI::setDebug(bool a_bDebug) {
      m_bDebug = a_bDebug;
    }
  }
}
