/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#pragma once

#include <string>

#ifdef _WINDOWS
extern "C" {
#endif
#include <lua.h>
#ifdef _WINDOWS
}
#endif

namespace dustbin {
  namespace lua {
    class ILuaClass {
      protected:
        lua_State *m_pState;
        std::string m_sError;

      public:
        ILuaClass();

        void setLuaState(lua_State *a_pState);

        lua_State *getLuaState();

        const std::string &getError();
    };
  }
}