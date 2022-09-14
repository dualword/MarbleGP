// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaSingleton_system.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <CGlobal.h>

#ifdef _WINDOWS
extern "C" {
#endif
#include <lua.h>
#ifdef _WINDOWS
}
#endif

namespace dustbin {
  namespace lua {
    /**
    * Executes a LUA script from the data folder
    * @param Filename of the LUA script. Must be located in the application's data folder
    */
    std::string CLuaSingleton_system::executeluascript(const std::string& a_script) {
      initialize();

      std::string s = doloadtextfile("data/" + a_script);

      if (s != "") {
        return executeluastring(s);
      }
      
      return "Script file not found.";
    }

    /**
    * Executes a LUA string
    * @param The LUA script to execute as string
    */
    std::string CLuaSingleton_system::executeluastring(const std::string& a_script) {
      initialize();

      int l_iResult = luaL_dostring(m_pState, a_script.c_str());

      if (l_iResult != 0) {
        if (!lua_isnil(m_pState, -1)) {
          std::string l_sMsg = "LUA Error: ";
          l_sMsg += lua_tostring(m_pState, -1);
          return l_sMsg;
        }
        else {
          lua_close(m_pState);
          m_pState = nullptr;
          std::string l_sMsg = "Error ";
          l_sMsg += std::to_string(l_iResult);
          l_sMsg += " on \"CLuaScript_scene\" execution.";
          return l_sMsg;
        }
      }

      return "";
    }

    /**
    * Loads a text file from the application's roaming folder
    * @param Name of the text file
    */
    std::string CLuaSingleton_system::loadtextfile(const std::string& a_filename) {
      return doloadtextfile(helpers::ws2s(platform::portableGetDataPath()) + a_filename);
    }
    /**
    * The actual code that loads text files, not path adjustment done here
    * @param Name of the text file
    */
    std::string CLuaSingleton_system::doloadtextfile(const std::string& a_filename) {
      initialize();

      std::string l_sRet = "";
      

      if (m_fs->existFile(a_filename.c_str())) {
        irr::io::IReadFile *l_pFile = m_fs->createAndOpenFile(a_filename.c_str());

        if (l_pFile != nullptr) {
          char *s = new char[l_pFile->getSize() + 1];
          memset(s, 0, l_pFile->getSize() + 1);
          l_pFile->read(s, l_pFile->getSize());
          l_sRet = s;
          l_pFile->drop();
          delete []s;
        }
      }

      return l_sRet;
    }

    /**
    * Inialializes the members. Must not be called from extarnal as all methods of the singleton use this method if the members are not yet initialized
    */
    void CLuaSingleton_system::initialize() {
      if (m_device == nullptr)
        m_device = CGlobal::getInstance()->getIrrlichtDevice();

      if (m_fs == nullptr)
        m_fs = CGlobal::getInstance()->getFileSystem();
    }
  }
}

