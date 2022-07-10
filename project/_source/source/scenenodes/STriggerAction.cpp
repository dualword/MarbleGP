/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */

#include <scenenodes/STriggerAction.h>
#include <string>

namespace dustbin {
  namespace scenenodes {

    STriggerAction::STriggerAction() : 
      m_eAction   (enAction::None), 
      m_iTimer    (-1), 
      m_iNodeId   (-1), 
      m_iStep     (-1),
      m_fVelocity (0.0f), 
      m_fForce    (0.0f), 
      m_bVisible  (true), 
      m_eCondition(enMarbleCountCondition::Equal),
      m_eType     (enTriggerType::Timer)
    {
    }

    STriggerAction::STriggerAction(const STriggerAction& a_cOther) :
      m_eAction        (a_cOther.m_eAction),
      m_iTimer         (a_cOther.m_iTimer),
      m_iNodeId        (a_cOther.m_iNodeId),
      m_iStep          (a_cOther.m_iStep),
      m_fVelocity      (a_cOther.m_fVelocity),
      m_fForce         (a_cOther.m_fForce),
      m_bVisible       (a_cOther.m_bVisible),
      m_eCondition     (a_cOther.m_eCondition),
      m_eType          (a_cOther.m_eType),
      m_vTarget        (a_cOther.m_vTarget)
    {
    }

    STriggerAction::STriggerAction(irr::io::IAttributes* a_pIn, int a_iIndex, enTriggerType a_eType) : 
      m_eAction   (enAction::None), 
      m_iTimer    (a_eType == enTriggerType::Timer ? -1 : 0), 
      m_iNodeId   (-1), 
      m_iStep     (-1),
      m_fVelocity (0.0f), 
      m_fForce    (0.0f), 
      m_bVisible  (true), 
      m_eCondition(enMarbleCountCondition::Equal),
      m_eType     (a_eType)
    {
      m_eAction = (enAction)a_pIn->getAttributeAsEnumeration((std::string("Action_") + std::to_string(a_iIndex)).c_str(), g_ActionTypes, 0);

      if (m_eType == enTriggerType::Timer)
        m_iTimer = a_pIn->getAttributeAsInt((std::string("Time_") + std::to_string(a_iIndex)).c_str());
      else if (a_eType == enTriggerType::MarbleCount) {
        m_iTimer     =                         a_pIn->getAttributeAsInt        ((std::string("Count_")     + std::to_string(a_iIndex)).c_str());
        m_eCondition = (enMarbleCountCondition)a_pIn->getAttributeAsEnumeration((std::string("Condition_") + std::to_string(a_iIndex)).c_str(), g_Conditions, 0);
      }

      if (m_eAction != enAction::None)
        m_iNodeId = a_pIn->getAttributeAsInt((std::string("NodeId_") + std::to_string(a_iIndex)).c_str());

      switch (m_eAction) {
        case enAction::None:
        case enAction::InvertMotor:
        case enAction::StartMotor:
        case enAction::StopMotor:
          break;

        case enAction::SetMotorParams:
          m_fForce    =  a_pIn->getAttributeAsFloat((std::string("Force_"   ) + std::to_string(a_iIndex)).c_str());
          m_fVelocity =  a_pIn->getAttributeAsFloat((std::string("Velocity_") + std::to_string(a_iIndex)).c_str());
          break;

        case enAction::RotateSceneNode:

          m_fVelocity = a_pIn->getAttributeAsFloat   ((std::string("Velocity_") + std::to_string(a_iIndex)).c_str());
          m_vTarget   = a_pIn->getAttributeAsVector3d((std::string("Target_"  ) + std::to_string(a_iIndex)).c_str());
          break;

        case enAction::SceneNodeVisibility:
          m_bVisible = a_pIn->getAttributeAsBool((std::string("Visible_") + std::to_string(a_iIndex)).c_str());
          break;

        case enAction::CameraUpVector:
          m_vTarget = a_pIn->getAttributeAsVector3d((std::string("UpVector") + std::to_string(a_iIndex)).c_str());
      }
    }

    void STriggerAction::serialize(irr::io::IAttributes* a_pOut, int a_iIndex, enTriggerType a_eType) const {
      a_pOut->addEnum((std::string("Action_") + std::to_string(a_iIndex)).c_str(), (irr::s32)m_eAction, g_ActionTypes);

      if (m_eAction != enAction::None) {
        if (a_eType == enTriggerType::Timer)
          a_pOut->addInt ((std::string("Time_") + std::to_string(a_iIndex)).c_str(), m_iTimer);
        else if (a_eType == enTriggerType::MarbleCount) {
          a_pOut->addInt ((std::string("Count_"    ) + std::to_string(a_iIndex)).c_str(), m_iTimer);
          a_pOut->addEnum((std::string("Condition_") + std::to_string(a_iIndex)).c_str(), (irr::s32)m_eCondition, g_Conditions);
        }

        a_pOut->addInt ((std::string("NodeId_") + std::to_string(a_iIndex)).c_str(), m_iNodeId);
      }

      switch (m_eAction) {
        case enAction::None:
        case enAction::InvertMotor:
        case enAction::StartMotor:
        case enAction::StopMotor:
          break;

        case enAction::SetMotorParams:
          a_pOut->addFloat((std::string("Force_"   ) + std::to_string(a_iIndex)).c_str(), m_fForce   );
          a_pOut->addFloat((std::string("Velocity_") + std::to_string(a_iIndex)).c_str(), m_fVelocity);
          break;

        case enAction::RotateSceneNode:
          a_pOut->addFloat   ((std::string("Velocity_") + std::to_string(a_iIndex)).c_str(), m_fVelocity);
          a_pOut->addVector3d((std::string("Target_"  ) + std::to_string(a_iIndex)).c_str(), m_vTarget);
          break;

        case enAction::SceneNodeVisibility:
          a_pOut->addBool ((std::string("Visible_" ) + std::to_string(a_iIndex)).c_str(), m_bVisible );
          break;

        case enAction::CameraUpVector:
          a_pOut->addVector3d((std::string("UpVector") + std::to_string(a_iIndex)).c_str(), m_vTarget);
      }
    }
  }
}
