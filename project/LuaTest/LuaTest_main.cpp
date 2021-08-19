// (w) 2021 by Dustbin::Games / Christian Keimel
#include <streambuf>
#include <stdio.h>
#include <fstream>
#include <lua.hpp>
#include <string>

#include <LuaBridge/LuaBridge.h>


class CLuaSingleton_system {
  private:
    lua_State* m_pState;

  public:
    void executeLuaScript(const std::string& a_sScript) {
      std::ifstream t(a_sScript);
      std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

      luaL_dostring(m_pState, str.c_str());
    }

    CLuaSingleton_system(lua_State* a_pState) : m_pState(a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSingleton_system>("LuaSystem")
          .addFunction("executeluascript", &CLuaSingleton_system::executeLuaScript)
        .endClass();

      std::error_code l_cError;
      luabridge::push(m_pState, this, l_cError);
      lua_setglobal(m_pState, "system");
    }
};

int main(int argc, char* argv[]) {
  printf("Command Line Arguments:\n\n");

  for (int i = 0; i < argc; i++)
    printf("%i: \"%s\"\n", i, argv[i]);

  printf("\n");

  if (argc < 2) {
    printf("Please specify a LUA script to run as commandline argument.\n");
    return 0;
  }

  std::string l_sScript = argv[1];

  printf("Running \"%s\"\n\n", l_sScript.c_str());

  lua_State* l_pState = luaL_newstate();

  luaL_openlibs(l_pState);

  CLuaSingleton_system l_cSingleton = CLuaSingleton_system(l_pState);

  std::ifstream t(l_sScript);
  std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

  luaL_dostring(l_pState, str.c_str());

  lua_close(l_pState);

  return 0;
}