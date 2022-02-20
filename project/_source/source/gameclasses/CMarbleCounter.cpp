// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gameclasses/CMarbleCounter.h>
#include <gameclasses/COdeNodes.h>

namespace dustbin {
  namespace gameclasses {
    CMarbleCounter::CMarbleCounter(int a_iTriggerPlus, int a_iTriggerMinus, const std::vector<scenenodes::STriggerAction> &a_vActions) :
      m_iTriggerPlus (a_iTriggerPlus),
      m_iTriggerMinus(a_iTriggerMinus),
      m_iMarbleCount (0),
      m_pWorld       (nullptr)
    {
      for (int i = 0; i < 16; i++)
        m_aMarblesIn[i] = false;

      for (std::vector<scenenodes::STriggerAction>::const_iterator it = a_vActions.begin(); it != a_vActions.end(); it++)
        m_vActions.push_back(scenenodes::STriggerAction(*it));
    }

    CMarbleCounter::CMarbleCounter(const CMarbleCounter &a_cOther, CWorld *a_pWorld) :
      m_iTriggerPlus (a_cOther.m_iTriggerPlus),
      m_iTriggerMinus(a_cOther.m_iTriggerMinus),
      m_iMarbleCount (a_cOther.m_iMarbleCount),
      m_pWorld       (a_pWorld)
    {
      for (int i = 0; i < 16; i++)
        m_aMarblesIn[i] = false;

      for (std::vector<scenenodes::STriggerAction>::const_iterator it = a_cOther.m_vActions.begin(); it != a_cOther.m_vActions.end(); it++)
        m_vActions.push_back(scenenodes::STriggerAction(*it));
    }

    CMarbleCounter::CMarbleCounter(const CMarbleCounter &a_cOther) :
      m_iTriggerPlus (a_cOther.m_iTriggerPlus),
      m_iTriggerMinus(a_cOther.m_iTriggerMinus),
      m_iMarbleCount (a_cOther.m_iMarbleCount),
      m_pWorld       (a_cOther.m_pWorld      )
    {
      for (int i = 0; i < 16; i++)
        m_aMarblesIn[i] = false;

      for (std::vector<scenenodes::STriggerAction>::const_iterator it = a_cOther.m_vActions.begin(); it != a_cOther.m_vActions.end(); it++)
        m_vActions.push_back(scenenodes::STriggerAction(*it));
    }

    CMarbleCounter::~CMarbleCounter() {
    }

    void CMarbleCounter::checkAction(int a_iMarbleCountNew) {
      printf("Marble Counter: %i --> %i\n", m_iMarbleCount, a_iMarbleCountNew);

      for (std::vector<scenenodes::STriggerAction>::iterator it = m_vActions.begin(); it != m_vActions.end(); it++) {
        switch ((*it).m_eCondition) {
          case scenenodes::enMarbleCountCondition::Equal: {
            if (m_iMarbleCount != (*it).m_iTimer && a_iMarbleCountNew == (*it).m_iTimer) {
              executeAction(*it);
            }
            break;
          }

          case scenenodes::enMarbleCountCondition::Greater: {
            if (m_iMarbleCount <= (*it).m_iTimer && a_iMarbleCountNew > (*it).m_iTimer) {
              executeAction(*it);
            }
            break;
          }

          case scenenodes::enMarbleCountCondition::GreaterOrEqual: {
            if (m_iMarbleCount < (*it).m_iTimer && a_iMarbleCountNew >= (*it).m_iTimer) {
              executeAction(*it);
            }
            break;
          }

          case scenenodes::enMarbleCountCondition::Less: {
            if (m_iMarbleCount >= (*it).m_iTimer && a_iMarbleCountNew < (*it).m_iTimer) {
              executeAction(*it);
            }
            break;
          }

          case scenenodes::enMarbleCountCondition::LessOrEqual: {
            if (m_iMarbleCount > (*it).m_iTimer && a_iMarbleCountNew <= (*it).m_iTimer) {
              executeAction(*it);
            }
            break;
          }
        }
      }

      m_iMarbleCount = a_iMarbleCountNew;
    }

    void CMarbleCounter::marbleRespawn(int a_iMarbleId) {
      int  l_iId = a_iMarbleId - 10000;

      if (l_iId >= 0 && l_iId < 16) {
        if (m_aMarblesIn[l_iId]) {
          m_aMarblesIn[l_iId] = false;
          checkAction(m_iMarbleCount - 1);
        }
      }
    }

    void CMarbleCounter::handleTrigger(int a_iTriggerId, int a_iMarbleId) {
      bool l_bCheck = false;
      int  l_iDelta = 0;
      int  l_iId    = a_iMarbleId - 10000;

      if (l_iId >= 0 && l_iId < 16) {
        if (a_iTriggerId == m_iTriggerPlus) {
          l_iDelta = (!m_aMarblesIn[l_iId]) ? 1 : 0;
          l_bCheck = true;
          m_aMarblesIn[l_iId] = true;
        }
        else if (a_iTriggerId == m_iTriggerMinus) {
          l_iDelta = m_aMarblesIn[l_iId] ? -1 : 0;
          l_bCheck = true;
          m_aMarblesIn[l_iId] = false;
        }
      }

      if (l_bCheck) {
        checkAction(m_iMarbleCount + l_iDelta);
      }
    }

    void CMarbleCounter::setWorld(CWorld* a_pWorld) {
      m_pWorld = a_pWorld;
    }

    void CMarbleCounter::executeAction(const scenenodes::STriggerAction& a_cAction) {
      if (m_pWorld == nullptr)
        return;

      switch (a_cAction.m_eAction) {
        case scenenodes::enAction::InvertMotor: {
          for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
            if ((*it2)->m_iId == a_cAction.m_iNodeId) {
              if ((*it2)->m_cJoint != nullptr) {
                if ((*it2)->m_bSliderJoint) {
                  dReal v = dJointGetSliderParam((*it2)->m_cJoint, dParamVel);
                  dReal f = dJointGetSliderParam((*it2)->m_cJoint, dParamFMax);
                  dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, 0.0);
                  dJointSetSliderParam((*it2)->m_cJoint, dParamVel, -v);
                  dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, f);
                }
                else {
                  dReal v = dJointGetHingeParam((*it2)->m_cJoint, dParamVel);
                  dReal f = dJointGetHingeParam((*it2)->m_cJoint, dParamFMax);
                  dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, 0.0);
                  dJointSetHingeParam((*it2)->m_cJoint, dParamVel, -v);
                  dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, f);
                }
              }
            }
          }
          break;
        }

        case scenenodes::enAction::SetMotorParams: {
          for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
            if ((*it2)->m_iId == a_cAction.m_iNodeId) {
              if ((*it2)->m_cJoint != nullptr) {
                if ((*it2)->m_bSliderJoint) {
                  dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, a_cAction.m_fForce   );
                  dJointSetSliderParam((*it2)->m_cJoint, dParamVel , a_cAction.m_fVelocity);
                }
                else {
                  dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, a_cAction.m_fForce   );
                  dJointSetHingeParam((*it2)->m_cJoint, dParamVel , a_cAction.m_fVelocity);
                }
              }
            }
          }
          break;
        }

        case scenenodes::enAction::StopMotor:
          for (std::vector<gameclasses::CObject*>::iterator it2 = m_pWorld->m_vMoving.begin(); it2 != m_pWorld->m_vMoving.end(); it2++) {
            if ((*it2)->m_iId == a_cAction.m_iNodeId) {
              if ((*it2)->m_cJoint != nullptr) {
                if ((*it2)->m_bSliderJoint) {
                  dJointSetSliderParam((*it2)->m_cJoint, dParamFMax, 0.0);
                  dJointSetSliderParam((*it2)->m_cJoint, dParamVel , 0.0);
                }
                else {
                  dJointSetHingeParam((*it2)->m_cJoint, dParamFMax, 0.0);
                  dJointSetHingeParam((*it2)->m_cJoint, dParamVel , 0.0);
                }
              }
            }
          }
          break;

        default:
          break;
      }
    }
  }
}

