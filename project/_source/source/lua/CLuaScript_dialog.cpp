// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaSingleton_system.h>
#include <lua/CLuaSingleton_audio.h>
#include <lua/CLuaScript_dialog.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaGuiItem.h>
#include <gui/CDialog.h>
#include <exception>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaScript_dialog::CLuaScript_dialog(const std::string& a_sScript) : m_pState(nullptr), m_pSystem(nullptr) {
      m_pState = luaL_newstate();

      luaL_openlibs(m_pState);

      CLuaSceneManager::registerClass(m_pState);
      CLuaGuiItem     ::registerClass(m_pState);

      luabridge::enableExceptions(m_pState);

      m_pSystem = new CLuaSingleton_system(m_pState);
      m_pAudio  = new CLuaSingleton_audio (m_pState);

      m_pDialog = new gui::CDialog(m_pState);

      try {
        if (luaL_dostring(m_pState, a_sScript.c_str()) != LUA_OK) {
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA script");
          throw std::exception();
        }
      }
      catch (luabridge::LuaException e) {
        printf("Exception: %s\n", e.what());
      }
    }

    CLuaScript_dialog::~CLuaScript_dialog() {
      if (m_pState != nullptr) {
        lua_close(m_pState);
      }

      delete m_pSystem;
      delete m_pAudio;

      if (m_pDialog != nullptr)
        delete m_pDialog;
    }

    void CLuaScript_dialog::initialize() {
      try {
        luabridge::LuaRef l_cInitialize = luabridge::getGlobal(m_pState, "initialize");
        if (l_cInitialize.isCallable()) {
          l_cInitialize();
        }
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"initialize\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::cleanup() {
      try {
        luabridge::LuaRef l_cCleanup = luabridge::getGlobal(m_pState, "cleanup");
        if (l_cCleanup.isCallable())
          l_cCleanup();
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"cleanup\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::step(int a_iTime) {
      try {
        luabridge::LuaRef l_cStep = luabridge::getGlobal(m_pState, "step");
        if (l_cStep.isCallable())
          l_cStep(a_iTime);
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"step\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::windowresized() {
      if (m_pDialog != nullptr) {
        m_pDialog->clear();
        m_pDialog->createUi();

        try {
          luabridge::LuaRef l_cCallback = luabridge::getGlobal(m_pState, "windowresized");
          if (l_cCallback.isCallable())
            l_cCallback();
        }
        catch (luabridge::LuaException e) {
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"windowresized\"");
          throw std::exception();
        }
      }
    }

    void CLuaScript_dialog::uiElementHovered(int a_iId, const std::string& a_sName) {
      try {
        luabridge::LuaRef l_cCallback = luabridge::getGlobal(m_pState, "uielementhovered");
        if (l_cCallback.isCallable())
          l_cCallback(a_iId, a_sName);
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uielementhovered\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::uiElementLeft(int a_iId, const std::string& a_sName) {
      try {
        luabridge::LuaRef l_cCallback = luabridge::getGlobal(m_pState, "uielementleft");
        if (l_cCallback.isCallable())
          l_cCallback(a_iId, a_sName);
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uielementleft\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::uiButtonClicked(int a_iId, const std::string& a_sName) {
      try {
        luabridge::LuaRef l_cCallback = luabridge::getGlobal(m_pState, "uibuttonclicked");
        if (l_cCallback.isCallable())
          l_cCallback(a_iId, a_sName);
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uibuttonclicked\"");
        throw std::exception();
      }
    }

    void CLuaScript_dialog::uiValueChanged(int a_iId, const std::string& a_sName, float a_fValue) {
      try {
        luabridge::LuaRef l_cCallback = luabridge::getGlobal(m_pState, "uivaluechanged");
        if (l_cCallback.isCallable())
          l_cCallback(a_iId, a_sName, a_fValue);
      }
      catch (luabridge::LuaException e) {
        CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
        CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running LUA function \"uivaluechanged\"");
        throw std::exception();
      }
    }

    /**
    * Query one of two elements that might be defined as default (Default OK and Cancel)
    * @param a_bCancel do you want the "Cancel" default? Pass false to get the "OK" default
    * @return a pointer to the element, "nullptr" if the element was not defined
    */
    irr::gui::IGUIElement* CLuaScript_dialog::getDefaultElement(bool a_bCancel) {
      if (m_pDialog != nullptr) {
        if (a_bCancel)
          return m_pDialog->cancelClicked();
        else
          return m_pDialog->defaultClicked();
      }

      return nullptr;
    }
  }
}
