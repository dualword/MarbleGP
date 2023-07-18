/// (w) 2023 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#include <_generated/luamesh/CLuaSingleton_system.h>
#include <lua/luawrap.hpp>
#include <lauxlib.h>

namespace dustbin {
  namespace luamesh {
    /**
    * Load another LUA script
    * @param The script file to load
    */
    void CLuaSingleton_system::loadscript(const std::string& a_script) {
      irr::io::IReadFile *l_pFile = m_device->getFileSystem()->createAndOpenFile(a_script.c_str());

      if (l_pFile != nullptr) {
        char *l_pBuffer = new char[l_pFile->getSize() + 1];
        memset(l_pBuffer, 0, l_pFile->getSize() + 1);
        l_pFile->read(l_pBuffer, l_pFile->getSize());
        std::string l_sScript = l_pBuffer;
        delete []l_pBuffer;

        int l_iResult = luaL_dostring(m_pState, l_sScript.c_str());

        if (l_iResult != 0) {
          if (!lua_isnil(m_pState, -1)) {
            std::string l_sMsg = "LUA Error: ";
            l_sMsg += lua_tostring(m_pState, -1);
            m_sError = l_sMsg;
            printf("LUA Error: \"%s\"\n", l_sMsg.c_str());
          }
          else {
            lua_close(m_pState);
            m_pState = nullptr;
            std::string l_sMsg = "Error ";
            l_sMsg += std::to_string(l_iResult);
            l_sMsg += " on \"CLuaScript_meshbuilder\" execution.";
            m_sError = l_sMsg;
            printf("LUA Error: \"%s\"\n", l_sMsg.c_str());
          }
        }
      }
    }
  }
}