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
    * @class CLuaSceneNode
    * @author Christian Keimel
    * This class allows control of a scene node from a LUA script
    */
    class CLuaSceneNode : public ILuaObject {
      protected:
        irr::scene::ISceneNode *m_pNode;

      public:
        CLuaSceneNode(irr::scene::ISceneNode *a_pNode);
        CLuaSceneNode(const CLuaSceneNode &a_cOther);
        virtual ~CLuaSceneNode();

        int setPosition(lua_State *a_pState);
        int setRotation(lua_State *a_pState);
        int setScale(lua_State *a_pState);

        void setVisible(bool a_bVisible);

        irr::scene::ISceneNode *getNode();

        static void registerClass(lua_State *a_pState);
    };
  }
}