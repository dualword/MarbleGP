// (w) 2021 by Dustbin::Games / Christian Keimel

#include <lua/CLuaPhysicsEntity.h>
#include <gameclasses/COdeNodes.h>
#include <LuaBridge/LuaBridge.h>
#include <ode/ode.h>

namespace dustbin {
  namespace lua {
    CLuaPhysicsEntity::CLuaPhysicsEntity() : m_pObject(nullptr) { }
    CLuaPhysicsEntity::~CLuaPhysicsEntity() { }

    void CLuaPhysicsEntity::startMotor(float a_fVelocity, float a_fForce) {
      if (m_pObject != nullptr && m_pObject->m_cJoint != 0) {
        dJointSetSliderParam(m_pObject->m_cJoint, dParamVel, a_fVelocity);
        dJointSetSliderParam(m_pObject->m_cJoint, dParamFMax, a_fForce);
      }
    }

    void CLuaPhysicsEntity::stopMotor() {
      if (m_pObject != nullptr && m_pObject->m_cJoint != 0) {
        dJointSetSliderParam(m_pObject->m_cJoint, dParamFMax, 0.0);
      }
    }

    int CLuaPhysicsEntity::getId() {
      if (m_pObject != nullptr)
        return m_pObject->m_iId;
      else
        return -1;
    }

    std::string CLuaPhysicsEntity::getName() {
      if (m_pObject != nullptr)
        return m_pObject->m_sName;
      else
        return "";
    }

    void CLuaPhysicsEntity::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaPhysicsEntity>("LuaPhysicsEntity")
          .addFunction("getid"     , &CLuaPhysicsEntity::getId     )
          .addFunction("getname"   , &CLuaPhysicsEntity::getName   )
          .addFunction("startmotor", &CLuaPhysicsEntity::startMotor)
          .addFunction("stopmotor" , &CLuaPhysicsEntity::stopMotor )
        .endClass();
    }
  }
}