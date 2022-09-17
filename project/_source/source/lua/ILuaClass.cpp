#include <helpers/CStringHelpers.h>
#include <lua/ILuaClass.h>

namespace dustbin {
  namespace lua {

    ILuaClass::ILuaClass() {
      m_pState = nullptr;
    }

    void ILuaClass::setLuaState(lua_State *a_pState) {
      m_pState = a_pState;
    }

    const std::string& ILuaClass::getError() {
      helpers::addToDebugLog("    ILuaClass::getError");
      return m_sError;
    }

    lua_State *ILuaClass::getLuaState() {
      return m_pState;
    }
  }
}