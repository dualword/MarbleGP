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
        void setPosition(float a_fX, float a_fY, float a_fZ);
        void setUpVector(float a_fX, float a_fY, float a_fZ);
        void setTarget(float a_fX, float a_fY, float a_fZ);

        static void registerClass(lua_State *a_pState);
    };
  }
}