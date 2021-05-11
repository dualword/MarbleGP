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
      protected:
        std::vector<ILuaObject *> m_vObjects;

      public:
        ILuaObject() { }
        virtual ~ILuaObject() { 
          for (std::vector<ILuaObject *>::iterator it = m_vObjects.begin(); it != m_vObjects.end(); it++)
            delete *it;

          m_vObjects.clear();
        }
    };
  }
}
