// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaScript_dialog.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaScript_dialog::CLuaScript_dialog(const std::string& a_sScript) : m_pState(nullptr), m_pLuaSmgr(nullptr) {
      m_pState = luaL_newstate();


      luaL_openlibs(m_pState);

      CLuaSceneManager::registerClass(m_pState);

      luabridge::getGlobalNamespace(m_pState)
        .beginClass<CLuaScript_dialog>("LuaDialog")
          .addFunction("getscenemanager", &CLuaScript_dialog::getSceneManager)
        .endClass();

      std::error_code l_cError;
      luabridge::push(m_pState, this, l_cError);
      lua_setglobal(m_pState, "dialog");

      luaL_dostring(m_pState, a_sScript.c_str());

      m_pLuaSmgr = new CLuaSceneManager(CGlobal::getInstance()->getSceneManager());
    }

    CLuaScript_dialog::~CLuaScript_dialog() {
      if (m_pState != nullptr) {
        lua_close(m_pState);
      }

      if (m_pLuaSmgr != nullptr)
        delete m_pLuaSmgr;
    }

    void CLuaScript_dialog::initialize() {
      luabridge::LuaRef l_cInitialize = luabridge::getGlobal(m_pState, "initialize");
      if (l_cInitialize.isCallable())
        l_cInitialize();
    }

    void CLuaScript_dialog::step(int a_iTime) {
      luabridge::LuaRef l_cStep = luabridge::getGlobal(m_pState, "step");
      if (l_cStep.isCallable())
        l_cStep(a_iTime);
    }

    CLuaSceneManager* CLuaScript_dialog::getSceneManager() {
      return m_pLuaSmgr;
    }
  }
}
