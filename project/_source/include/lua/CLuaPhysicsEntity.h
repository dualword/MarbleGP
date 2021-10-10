// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <lua.hpp>
#include <string>

namespace dustbin {
  namespace gameclasses {
    class CObject;
  }

  namespace lua {
    /**
    * @class CLuaPhysicsEntity
    * @author Christian Keimel
    * An internal class for accessing the physical objects in
    * the scene from the LUA script
    */
    class CLuaPhysicsEntity {
      public:
        gameclasses::CObject* m_pObject;

        CLuaPhysicsEntity();
        ~CLuaPhysicsEntity();

        void startMotor(float a_fVelocity, float a_fForce);
        void stopMotor();

        int getId();
        std::string getName();

        static void registerClass(lua_State* a_pState);
    };
  }
}