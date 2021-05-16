#include <lua/CLuaSingleton_system.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaHelpers.h>
#include <algorithm>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaSingleton_system::CLuaSingleton_system(lua_State* a_pState) : m_pGlobal(CGlobal::getInstance()), m_pResolutionList(nullptr), m_pState(a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSingleton_system>("LuaDialog")
          .addFunction("getscenemanager"  , &CLuaSingleton_system::getSceneManager)
          .addFunction("setsetting"       , &CLuaSingleton_system::setSetting)
          .addFunction("getsetting"       , &CLuaSingleton_system::getSetting)
          .addFunction("setglobal"        , &CLuaSingleton_system::setGlobal)
          .addFunction("getglobal"        , &CLuaSingleton_system::getGlobal)
          .addFunction("pushscript"       , &CLuaSingleton_system::pushScript)
          .addFunction("statechange"      , &CLuaSingleton_system::stateChange)
          .addFunction("getresolutionlist", &CLuaSingleton_system::getResolutionList)
          .addFunction("executeluascript" , &CLuaSingleton_system::executeLuaScript)
          .addFunction("executeluastring" , &CLuaSingleton_system::executeLuaString)
        .endClass();

      std::error_code l_cError;
      luabridge::push(a_pState, this, l_cError);
      lua_setglobal(a_pState, "system");

      m_pResolutionList = new CLuaResolutionList(a_pState);
    }

    CLuaSingleton_system::~CLuaSingleton_system() {
      if (m_pResolutionList != nullptr)
        delete m_pResolutionList;
    }

    /**
    * Get the global scene manager
    * @return the global scene manager
    */
    CLuaSceneManager CLuaSingleton_system::getSceneManager() {
      return CLuaSceneManager(m_pGlobal->getSceneManager());
    }

    /**
    * Set a setting
    * @param a_sKey the key of the setting
    * @param a_sValue the value of the setting
    */
    void CLuaSingleton_system::setSetting(const std::string& a_sKey, const std::string& a_sValue) {
      m_pGlobal->setSetting(a_sKey, a_sValue);
    }

    /**
    * Get a setting
    * @param a_sKey the key of the setting
    * @return the setting, empty string if the setting was not found
    */
    std::string CLuaSingleton_system::getSetting(const std::string& a_sKey) {
      if (m_pGlobal->hasSetting(a_sKey))
        return m_pGlobal->getSetting(a_sKey);
      else
        return "";
    }

    /**
    * Set a global variable
    * @param a_sKey the key of the global variable
    * @param a_sValue the value of the global variable
    */
    void CLuaSingleton_system::setGlobal(const std::string& a_sKey, const std::string& a_sValue) {
      m_pGlobal->setGlobal(a_sKey, a_sValue);
    }

    /**
    * Get a global variable
    * @param a_sKey the key of the global variable
    * @return the global variable, empty string if the global was not found
    */
    std::string CLuaSingleton_system::getGlobal(const std::string& a_sKey) {
      if (m_pGlobal->hasGlobal(a_sKey))
        return m_pGlobal->getGlobal(a_sKey);
      else
        return "";
    }

    /**
    * Push a script to the lua script stack
    * @param a_sScript the script to push
    */
    void CLuaSingleton_system::pushScript(const std::string& a_sScript) {
      m_pGlobal->pushScript(a_sScript);
    }

    /**
    * Request a state change
    * @param a_iNewState integer version of the new state
    */
    void CLuaSingleton_system::stateChange(int a_iNewState) {
      m_pGlobal->stateChange((state::enState)a_iNewState);
    }
    
    /**
    * Get a list with the supported screen resolutions
    */
    CLuaResolutionList *CLuaSingleton_system::getResolutionList() {
      return m_pResolutionList;
    }

    /**
    * Execute a lua script from a file
    */
    void CLuaSingleton_system::executeLuaScript(const std::string& a_sFile) {
      irr::io::IFileSystem *l_pFs = CGlobal::getInstance()->getFileSystem();

      if (l_pFs->existFile(a_sFile.c_str())) {
        std::string l_sScript = loadLuaScript(a_sFile);
      }
    }

    /**
    * Execute a lua script from a string
    * @param a_sScript script as std::string
    */
    void CLuaSingleton_system::executeLuaString(const std::string& a_sScript) {
      if (a_sScript != "")
        luaL_dostring(m_pState, a_sScript.c_str());
    }



    CLuaResolutionList::CLuaResolutionList(lua_State *a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaResolutionList>("LuaResolutionList")
          .addFunction("count", &CLuaResolutionList::getCount)
          .addFunction("get"  , &CLuaResolutionList::getResolutionString)
        .endClass();

      irr::video::IVideoModeList *l_pList = CGlobal::getInstance()->getIrrlichtDevice()->getVideoModeList();

      for (irr::s32 i = 0; i < l_pList->getVideoModeCount(); i++) {
        irr::core::dimension2du l_cDim = l_pList->getVideoModeResolution(i);

        bool l_bAdd = true;

        for (std::vector<irr::core::dimension2du>::iterator it = m_vList.begin(); it != m_vList.end(); it++) {
          if ((*it).Width == l_cDim.Width && (*it).Height == l_cDim.Height)
            l_bAdd = false;
        }

        if (l_bAdd) {
          m_vList.push_back(irr::core::dimension2du(l_cDim));
        }
      }

      std::sort(m_vList.begin(), m_vList.end(), [](const irr::core::dimension2du &a_cDim1, const irr::core::dimension2du &a_cDim2) {
          if (a_cDim1.Width != a_cDim2.Height) {
            if (a_cDim1.Width > a_cDim2.Width)
              return true;
            else
              return false;
          }
          else {
            if (a_cDim1.Height > a_cDim2.Height)
              return true;
            else
              return false;
          }

          return false;
        });

      printf("Ready.\n");
    }

    CLuaResolutionList::~CLuaResolutionList() {
    }

    /**
    * Returns the number of resolutions in the vector
    * @return the number of resolutions in the vector
    */
    int CLuaResolutionList::getCount() {
      return (int)m_vList.size();
    }

    /**
    * Get a string representation of the resolution
    * @param a_iIndex the index of the resolution
    * @return a string representation of the resolution
    */
    std::string CLuaResolutionList::getResolutionString(int a_iIndex) {
      if (a_iIndex >= 0 && a_iIndex < m_vList.size()) {
        return std::to_string(m_vList[a_iIndex].Width) + "x" + std::to_string(m_vList[a_iIndex].Height);
      }
      else return "";
    }

    /**
    * Get the resolution data
    * @param a_iIndex the index of the resolution
    * @param a_cResolution [out] the resolution
    * @return true if the resolution was found, false otherwise
    */
    bool CLuaResolutionList::getResolution(int a_iIndex, irr::core::dimension2du& a_cResolution) {
      if (a_iIndex >= 0 && a_iIndex < m_vList.size()) {
        a_cResolution = m_vList[a_iIndex];

        return true;
      }
      else return false;
    }
  }
}