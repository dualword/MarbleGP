// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua.hpp>
#include <string>

namespace dustbin {
  namespace gui {
    class CDialog;
  }

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

        gui::CDialog *m_pDialog;

        lua_State *m_pState;

      public:
        CLuaScript_dialog(const std::string &a_sScript);
        virtual ~CLuaScript_dialog();

        void initialize();
        void cleanup();
        void step(int a_iTime);

        void windowresized();

        void uiElementHovered(int a_iId, const std::string &a_sName);
        void uiElementLeft(int a_iId, const std::string &a_sName);
        void uiButtonClicked(int a_iId, const std::string &a_sName);
        void uiValueChanged(int a_iId, const std::string &a_sName, float a_fValue);
        void uiTextChanged(int a_iId, const std::string& a_sName, const std::string& a_sNewValue);
        void uiCheckboxChanged(int a_iId, const std::string& a_sName, bool a_bChecked);
        void beforeResize();

        /**
        * Query one of two elements that might be defined as default (Default OK and Cancel)
        * @param a_bCancel do you want the "Cancel" default? Pass false to get the "OK" default
        * @return a pointer to the element, "nullptr" if the element was not defined
        */
        irr::gui::IGUIElement* getDefaultElement(bool a_bCancel);
    };
  }
}