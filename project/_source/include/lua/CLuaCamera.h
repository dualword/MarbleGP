// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua/ILuaObject.h>
#include <lua.hpp>

namespace dustbin {
  namespace lua {
    /**
     * @class CLuaCamera
     * @author Christian Keimel
     * This class represents an Irrlicht camera scenenode
     */
    class CLuaCamera : public ILuaObject {
      private:
        irr::scene::ICameraSceneNode *m_pCamera;

      public:
        CLuaCamera(irr::scene::ICameraSceneNode *a_pCamera);
        CLuaCamera(const CLuaCamera &a_cOther);

        virtual ~CLuaCamera();

        void activate();
        int setPosition(lua_State *a_pState);
        int setUpVector(lua_State *a_pState);
        int setTarget  (lua_State *a_pState);

        static void registerClass(lua_State *a_pState);
    };
  }
}