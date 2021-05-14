// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua.hpp>

#include <scenenodes/CGui3dItem.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLua3dGuiItem.h>

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

    irr::video::SColor CLua3dGuiItem::fixColor(int &a_iAlpha, int &a_iRed, int &a_iGreen, int &a_iBlue) {
      if (a_iAlpha < 0) a_iAlpha = 0; if (a_iAlpha > 255) a_iAlpha = 255;
      if (a_iRed   < 0) a_iRed   = 0; if (a_iRed   > 255) a_iRed   = 255;
      if (a_iGreen < 0) a_iGreen = 0; if (a_iGreen > 255) a_iGreen = 255;
      if (a_iBlue  < 0) a_iBlue  = 0; if (a_iBlue  > 255) a_iBlue  = 255;

      return irr::video::SColor(a_iAlpha, a_iRed, a_iGreen, a_iBlue);
    }

    void CLua3dGuiItem::setBackgroundColor(int a_iAlpha, int a_iRed, int a_iGreen, int a_iBlue) {
      if (m_pItem != nullptr)
        m_pItem->setBackgroundColor(fixColor(a_iAlpha, a_iRed, a_iGreen, a_iBlue));
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