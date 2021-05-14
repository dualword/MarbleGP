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
    class CLua3dGuiItem;
    class CLuaSceneNode;

    /**
     * @class CLuaSceneManager
     * @author Christian Keimel
     * This class exposes a Scene Manager to LUA
     */
    class CLuaSceneManager : public ILuaObject {
      private:
        irr::scene::ISceneManager *m_pSmgr;

        irr::scene::ISceneNode *getNodeFromName(const std::string &a_sName, irr::scene::ISceneNode *a_pNode);

      public:
        CLuaSceneManager(irr::scene::ISceneManager *a_pSmgr);
        CLuaSceneManager(const CLuaSceneManager &a_cOther);

        virtual ~CLuaSceneManager();

        CLuaCamera addCamera();
        CLuaSceneNode getSceneNodeFromName(const std::string &a_sName);
        CLua3dGuiItem getGuiItemFromName(const std::string &a_sName);

        void loadScene(const std::string &a_sFile);
        void clear();
        void drop();

        CLuaSceneManager createNewSceneManager(int a_iRenderPosition);

        static void registerClass(lua_State *a_pState);
    };
  }
}
