// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <lua.hpp>
#include <string>

namespace dustbin {
  namespace lua {
    class CLuaSceneManager;

    /**
     * @class CLuaScript_dialog
     * @author Christian Keimel
     * This class handles the LUA scripts for dialogs
     */
    class CLuaScript_dialog {
      protected:
        CLuaSceneManager *m_pLuaSmgr;

        lua_State *m_pState;

      public:
        CLuaScript_dialog(const std::string &a_sScript);
        virtual ~CLuaScript_dialog();

        void initialize();
        void step(int a_iTime);

        CLuaSceneManager *getSceneManager();
    };
  }
}