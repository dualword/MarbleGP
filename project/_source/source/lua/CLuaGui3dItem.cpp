// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua.hpp>

#include <scenenodes/CGui3dItem.h>
#include <_generated/lua/lua_tables.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLua3dGuiItem.h>
#include <lua/CLuaHelpers.h>
#include <codecvt>
#include <locale>

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

    std::string CLua3dGuiItem::getText() {
      if (m_pItem != nullptr) {
        std::wstring l_sText = m_pItem->getText();
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> l_cConverter;
        std::string l_sRet = l_cConverter.to_bytes(l_sText);
        return l_sRet;
      }
      else return "";
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

    float CLua3dGuiItem::getValue() {
      if (m_pItem != nullptr)
        return m_pItem->getValue();
      else
        return 0.0f;
    }

    void CLua3dGuiItem::setValue(float a_fValue) {
      if (m_pItem != nullptr)
        m_pItem->setValue(a_fValue);
    }

    void CLua3dGuiItem::registerClass(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLua3dGuiItem>("GuiItem3d")
          .addFunction("settext"           , &CLua3dGuiItem::setText)
          .addFunction("gettext"           , &CLua3dGuiItem::getText)
          .addFunction("setbackgroundcolor", &CLua3dGuiItem::setBackgroundColor)
          .addFunction("getvalue"          , &CLua3dGuiItem::getValue)
          .addFunction("setvalue"          , &CLua3dGuiItem::setValue)
        .endClass();
    }
  }
}