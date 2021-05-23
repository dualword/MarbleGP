// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <lua.hpp>
#include <lua/ILuaObject.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    class CGui3dItem;
  }

  namespace lua {
    /**
    * @class CLua3dGuiItem
    * @author Christian Keimel
    * This is the lua interface for the 3d gui item
    * @see CLuaSceneNode
    */
    class CLua3dGuiItem : public ILuaObject {
      private:
        scenenodes::CGui3dItem *m_pItem;

      public:
        CLua3dGuiItem(scenenodes::CGui3dItem *a_pItem);
        CLua3dGuiItem(const CLua3dGuiItem &a_cOther);

        virtual ~CLua3dGuiItem();

        void setText(const std::string &a_sText);
        std::string getText();
        int setBackgroundColor(lua_State *a_pState);

        float getValue();
        void setValue(float a_fValue);

        static void registerClass(lua_State *a_pState);
    };
  }
}