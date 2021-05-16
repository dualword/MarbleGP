// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaCamera.h>
#include <_generated/lua/lua_tables.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaHelpers.h>

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

    int CLuaCamera::setPosition(lua_State *a_pState) {
      int l_iArgC = lua_gettop(a_pState);
      if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setposition\". 1 argument required."); return 0; }

      SVector3d l_position;
      l_position.loadFromStack(a_pState); lua_pop(a_pState, 1);

      m_pCamera->setPosition(convertVectorToIrr(l_position));
      return 0;
    }

    int CLuaCamera::setUpVector(lua_State *a_pState) {
      int l_iArgC = lua_gettop(a_pState);
      if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setupvector\". 1 argument required."); return 0; }

      SVector3d l_up;
      l_up.loadFromStack(a_pState); lua_pop(a_pState, 1);

      m_pCamera->setUpVector(convertVectorToIrr(l_up));
      return 0;
    }

    int CLuaCamera::setTarget(lua_State *a_pState) {
      int l_iArgC = lua_gettop(a_pState);
      if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"settarget\". 1 argument required."); return 0; }

      SVector3d l_target;
      l_target.loadFromStack(a_pState); lua_pop(a_pState, 1);

      m_pCamera->setTarget(convertVectorToIrr(l_target));
      return 0;
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