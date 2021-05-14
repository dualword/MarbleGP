// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaCamera.h>
#include <LuaBridge/LuaBridge.h>

namespace dustbin {
  namespace lua {
    CLuaCamera::CLuaCamera(irr::scene::ICameraSceneNode* a_pCamera) : m_pCamera(a_pCamera) {
    }

    CLuaCamera::CLuaCamera(const CLuaCamera& a_cOther) : m_pCamera(a_cOther.m_pCamera) {
    }

    CLuaCamera::~CLuaCamera() {
    }

    void CLuaCamera::activate() {
      m_pCamera->getSceneManager()->setActiveCamera(m_pCamera);
    }

    void CLuaCamera::setPosition(float a_fX, float a_fY, float a_fZ) {
      m_pCamera->setPosition(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }

    void CLuaCamera::setUpVector(float a_fX, float a_fY, float a_fZ) {
      m_pCamera->setUpVector(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }
    void CLuaCamera::setTarget(float a_fX, float a_fY, float a_fZ) {
      m_pCamera->setTarget(irr::core::vector3df(a_fX, a_fY, a_fZ));
    }

    void CLuaCamera::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaCamera>("camera")
          .addFunction("activate"   , &CLuaCamera::activate)
          .addFunction("setposition", &CLuaCamera::setPosition)
          .addFunction("setupvector", &CLuaCamera::setUpVector)
          .addFunction("settarget"  , &CLuaCamera::setTarget)
        .endClass();
    }
  }
}