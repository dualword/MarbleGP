// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua/ILuaObject.h>
#include <lua.hpp>
#include <vector>
#include <string>

namespace dustbin {
  namespace lua {
    class CLuaCamera;

    /**
     * @class CLuaSceneManager
     * @author Christian Keimel
     * This class exposes a Scene Manager to LUA
     */
    class CLuaSceneManager : public ILuaObject {
      private:
        irr::scene::ISceneManager *m_pSmgr;

      public:
        CLuaSceneManager(irr::scene::ISceneManager *a_pSmgr);
        virtual ~CLuaSceneManager();

        CLuaCamera *addCamera();
        void loadScene(const std::string &a_sFile);
        void clear();

        static void registerClass(lua_State *a_pState);
    };
  }
}
