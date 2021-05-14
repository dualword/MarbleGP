// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <vector>

namespace dustbin {
  namespace lua {
    /**
     * @class ILuaObject
     * @authod Christian Keimel
     * This is the base interface for all LUA objects.
     */
    class ILuaObject {
      public:
        ILuaObject() { }
        virtual ~ILuaObject() { }
    };
  }
}
