#include <lua/ILuaClass.h>

ILuaClass::ILuaClass() {
  m_pState = nullptr;
}

void ILuaClass::setLuaState(lua_State *a_pState) {
  m_pState = a_pState;
}

lua_State *ILuaClass::getLuaState() {
  return m_pState;
}

