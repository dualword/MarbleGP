/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace scenenodes {
    const irr::c8* const g_ActionTypes[] = {
      "None",
      "InvertMotor",
      "StartMotor",
      "StopMotor",
      "SetMotorParameters",
      "RotateSceneNode",
      "SceneNodeVisibility",
      "CameraUpVector",
      0
    };

    const irr::c8* const g_Conditions[] = {
      "Equal",
      "Less",
      "LessOrEqual",
      "Greater",
      "GreaterOrEqual",
      0
    };

    enum class enTriggerType {
      Timer       = 0,
      MarbleCount = 1,
      MarbleTouch = 2
    };

    enum class enAction {
      None                = 0,
      InvertMotor         = 1,
      StartMotor          = 2,
      StopMotor           = 3,
      SetMotorParams      = 4,
      RotateSceneNode     = 5,
      SceneNodeVisibility = 6,
      CameraUpVector      = 7
    };

    enum class enMarbleCountCondition {
      Equal           = 0,
      Less            = 1,
      LessOrEqual     = 2,
      Greater         = 3,
      GreaterOrEqual  = 4
    };

    struct STriggerAction {
      enAction m_eAction;
      irr::s32 m_iTimer;
      irr::s32 m_iNodeId;
      irr::s32 m_iStep;

      irr::f32 m_fVelocity;
      irr::f32 m_fForce;

      bool m_bVisible;

      enMarbleCountCondition m_eCondition;

      enTriggerType m_eType;

      irr::core::vector3df m_vTarget;

      STriggerAction();

      STriggerAction(const STriggerAction& a_cOther);

      STriggerAction(irr::io::IAttributes* a_pIn, int a_iIndex, enTriggerType a_eType);

      void serialize(irr::io::IAttributes *a_pOut, int a_iIndex, enTriggerType a_eType) const;
    };
  }
}
