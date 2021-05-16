// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua.hpp>

#include <scenenodes/CGui3dItem.h>
#include <_generated/lua/lua_tables.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLua3dGuiItem.h>
#include <lua/CLuaHelpers.h>

namespace dustbin {
  namespace lua {
    CLua3dGuiItem::CLua3dGuiItem(scenenodes::CGui3dItem* a_pItem) : m_pItem(a_pItem) {
    }

    CLua3dGuiItem::CLua3dGuiItem(const CLua3dGuiItem& a_cOther) : m_pItem(a_cOther.m_pItem) {
    }

    CLua3dGuiItem::~CLua3dGuiItem() {
    }

    void CLua3dGuiItem::setText(const std::string& a_sText) {
      if (m_pItem != nullptr)
        m_pItem->setText(std::wstring(a_sText.begin(), a_sText.end()));
    }

    int CLua3dGuiItem::setBackgroundColor(lua_State *a_pState) {
      if (m_pItem != nullptr) {
        int l_iArgC = lua_gettop(a_pState);
        if (l_iArgC < 2) { luaL_error(a_pState, "Not enough arguments for function \"setbackrgoundcolor\". 1 argument required."); return 0; }

        SColor l_cColor;
        l_cColor.loadFromStack(a_pState); lua_pop(a_pState, 1);

        m_pItem->setBackgroundColor(convertColorToIrr(l_cColor));
      }
      return 0;
    }

    void CLua3dGuiItem::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLua3dGuiItem>("GuiItem3d")
          .addFunction("settext"           , &CLua3dGuiItem::setText)
          .addFunction("setbackgroundcolor", &CLua3dGuiItem::setBackgroundColor)
        .endClass();
    }
  }
}