// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <lua.hpp>
#include <string>

namespace dustbin {
  namespace lua {
    class CLuaSingleton_system;
    class CLuaSingleton_audio;

    /**
     * @class CLuaScript_dialog
     * @author Christian Keimel
     * This class handles the LUA scripts for dialogs
     */
    class CLuaScript_dialog {
      protected:
        CLuaSingleton_system *m_pSystem;
        CLuaSingleton_audio  *m_pAudio;

        lua_State *m_pState;

      public:
        CLuaScript_dialog(const std::string &a_sScript);
        virtual ~CLuaScript_dialog();

        void initialize();
        void cleanup();
        void step(int a_iTime);

        void uiElementHovered(int a_iId, const std::string &a_sName);
        void uiElementLeft(int a_iId, const std::string &a_sName);
        void uiButtonClicked(int a_iId, const std::string &a_sName);
        void uiValueChanged(int a_iId, const std::string &a_sName, float a_fValue);
    };
  }
}