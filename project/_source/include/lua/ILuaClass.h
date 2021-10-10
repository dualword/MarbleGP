/*
(w) 2016 - 2021 by Christian Keimel / https://www.dustbin-online.de
This file is licensed under the terms of the ZLib license - https://www.zlib.net/zlib_license.html
*/
#pragma once

#include <luawrap.hpp>

class ILuaClass {
  protected:
    lua_State *m_pState;

  public:
    ILuaClass();

    void setLuaState(lua_State *a_pState);

    lua_State *getLuaState();
};
