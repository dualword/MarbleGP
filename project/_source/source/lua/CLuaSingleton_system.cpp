#include <controller/CControllerMenu.h>
#include <controller/CControllerGame.h>
#include <_generated/lua/lua_tables.h>
#include <lua/CLuaSingleton_system.h>
#include <messages/CMessageHelpers.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <platform/CPlatform.h>
#include <lua/CLuaHelpers.h>
#include <algorithm>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaSingleton_system::CLuaSingleton_system(lua_State* a_pState) : m_pGlobal(CGlobal::getInstance()), m_pResolutionList(nullptr), m_pState(a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSingleton_system>("LuaSystem")
          .addFunction("getscenemanager"       , &CLuaSingleton_system::getSceneManager)
          .addFunction("setsetting"            , &CLuaSingleton_system::setSetting)
          .addFunction("getsetting"            , &CLuaSingleton_system::getSetting)
          .addFunction("hassetting"            , &CLuaSingleton_system::hasSetting)
          .addFunction("setglobal"             , &CLuaSingleton_system::setGlobal)
          .addFunction("getglobal"             , &CLuaSingleton_system::getGlobal)
          .addFunction("pushscript"            , &CLuaSingleton_system::pushScript)
          .addFunction("popscript"             , &CLuaSingleton_system::popScript)
          .addFunction("peekscript"            , &CLuaSingleton_system::peekScript)
          .addFunction("clearscriptstack"      , &CLuaSingleton_system::clearScriptStack)
          .addFunction("statechange"           , &CLuaSingleton_system::stateChange)
          .addFunction("getresolutionlist"     , &CLuaSingleton_system::getResolutionList)
          .addFunction("executeluascript"      , &CLuaSingleton_system::executeLuaScript)
          .addFunction("executeluastring"      , &CLuaSingleton_system::executeLuaString)
          .addFunction("getsettings"           , &CLuaSingleton_system::getSettings)
          .addFunction("setsettings"           , &CLuaSingleton_system::setSettings)
          .addFunction("getcontrollerdata_menu", &CLuaSingleton_system::getControllerData_Menu)
          .addFunction("getcontrollerdata_game", &CLuaSingleton_system::getControllerXml_Game)
          .addFunction("urlencode"             , &CLuaSingleton_system::urlEncode)
          .addFunction("urldecode"             , &CLuaSingleton_system::urlDecode)
          .addFunction("setzlayer"             , &CLuaSingleton_system::setZLayer)
          .addFunction("getfirstcontroller"    , &CLuaSingleton_system::getFirstController)
          .addFunction("gettexturepatterns"    , &CLuaSingleton_system::getTexturePatterns)
          .addFunction("getimportedtextures"   , &CLuaSingleton_system::getImportedTextures)
          .addFunction("removetexture"         , &CLuaSingleton_system::removeTexture)
          .addFunction("gettracklist"          , &CLuaSingleton_system::getTrackList)
          .addFunction("fileexists"            , &CLuaSingleton_system::fileExists)
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
    * Does a setting exist?
    * @param a_sKey the setting key
    * @return true if the setting exists
    */
    bool CLuaSingleton_system::hasSetting(const std::string& a_sKey) {
      return m_pGlobal->hasSetting(a_sKey);
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
    * Remove the uppermost lua script from the script stack
    * @return the uppermost script from the stack
    */
    std::string CLuaSingleton_system::popScript() {
      return m_pGlobal->popScript();
    }

    /**
    * Get the uppermost script from the script stack without removing it
    * @return the uppermost script from the stack
    */
    std::string CLuaSingleton_system::peekScript() {
      return m_pGlobal->peekScript();
    }

    /**
    * Clear the LUA script stack
    */
    void CLuaSingleton_system::clearScriptStack() {
      m_pGlobal->clearScriptStack();
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
      m_pResolutionList->refresh();
      return m_pResolutionList;
    }

    /**
    * Execute a lua script from a file
    */
    void CLuaSingleton_system::executeLuaScript(const std::string& a_sFile) {
      irr::io::IFileSystem *l_pFs = m_pGlobal->getFileSystem();

      if (l_pFs->existFile(a_sFile.c_str())) {
        std::string l_sScript = loadLuaScript(a_sFile);
        executeLuaString(l_sScript);
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

    /**
    * Get the application settings from a LUA script
    */
    int CLuaSingleton_system::getSettings(lua_State* a_pState) {
      m_pGlobal->getSettings().pushToStack(a_pState);
      return 1;
    }

    /**
    * Update the settings from a LUA script
    */
    int CLuaSingleton_system::setSettings(lua_State* a_pState) {
      SSettings l_cSettings;
      l_cSettings.loadFromStack(a_pState);
      m_pGlobal->setSettings(l_cSettings);
      return 0;
    }

    /**
    * Url-Encode a string
    * @param a_sInput the un-encoded string
    * @return the encoded string
    */
    std::string CLuaSingleton_system::urlEncode(const std::string a_sInput) {
      return messages::urlEncode(a_sInput);
    }

    /**
    * Url-Decode a string
    * @param a_sInput the encoded string
    * @return the decoded string
    */
    std::string CLuaSingleton_system::urlDecode(const std::string a_sInput) {
      return messages::urlDecode(a_sInput);
    }

    /**
    * Get a list of the imported textures
    * @param a_pState the LUA state
    * @return "1", as only the list of patterns is returned
    */
    int CLuaSingleton_system::getImportedTextures(lua_State* a_pState) {
      std::vector<std::string> l_vTextures;

      irr::io::IFileSystem* l_pFs = m_pGlobal->getFileSystem();

      std::string l_sFolder = l_pFs->getWorkingDirectory().c_str();

      l_pFs->changeWorkingDirectoryTo(platform::ws2s(platform::portableGetTexturePath()).c_str());

      irr::io::IFileList* l_pList = l_pFs->createFileList();

      if (l_pList != nullptr) {
        for (unsigned i = 0; i < l_pList->getFileCount(); i++) {
          std::string l_sName = l_pList->getFileName(i).c_str();
          if (l_sName.find(".png") != std::string::npos) {
            l_vTextures.push_back(l_sName);
          }
        }
        l_pList->drop();
      }

      l_pFs->changeWorkingDirectoryTo(l_sFolder.c_str());

      pushToStack(l_vTextures, a_pState);
      return 1;
    }

    /**
    * Get a list of the available texture patterns
    * @param a_pState the LUA state
    * @return "1", as only the list of patterns is returned
    */
    int CLuaSingleton_system::getTexturePatterns(lua_State* a_pState) {
      std::vector<std::string> l_cList;

      irr::io::IReadFile* l_pFile = m_pGlobal->getFileSystem()->createAndOpenFile("data/texture_patterns.xml");
      if (l_pFile) {
        irr::io::IXMLReaderUTF8* l_pXml = m_pGlobal->getFileSystem()->createXMLReaderUTF8(l_pFile);
        if (l_pXml) {
          while (l_pXml->read()) {
            std::string l_sNode = l_pXml->getNodeName();

            if (l_sNode == "pattern" && l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
              std::string l_sPattern = l_pXml->getAttributeValueSafe("file");
              if (l_sPattern != "")
                l_cList.push_back(l_sPattern);
            }
          }

          l_pXml->drop();
        }
        l_pFile->drop();
      }

      pushToStack(l_cList, a_pState);
      return 1;
    }

    /**
    * Create a XML string with the default configuration for the game controller
    * @return a XML string
    */
    std::string CLuaSingleton_system::getControllerXml_Game() {
      controller::CControllerGame* p = new controller::CControllerGame();
      std::string l_sRet = p->serialize();
      delete p;
      return l_sRet;
    }

    /**
    * Get the first controller from the configuration
    * @param a_sConfig the configuration as XML-string
    * @return the first controller
    */
    std::string CLuaSingleton_system::getFirstController(const std::string& a_sConfig) {
      std::string l_sRet = "Unkown";

      controller::CControllerBase* p = new controller::CControllerBase();
      p->deserialize(a_sConfig);

      std::vector<controller::CControllerBase::SCtrlInput> l_vInputs = p->getInputs();

      if (l_vInputs.size() > 0) {
        if (l_vInputs[0].m_eType == controller::CControllerBase::enInputType::Key) {
          l_sRet = "Keyboard";
        }
        else {
          l_sRet = l_vInputs[0].m_sJoystick;
        }
      }
      delete p;

      return l_sRet;
    }

    /**
    * Create a XML string with the default configuration for the menu controller
    * @return a XML string
    */
    std::string CLuaSingleton_system::getControllerData_Menu() {
      controller::CControllerMenu* p = new controller::CControllerMenu(-1);
      std::string l_sRet = p->serialize();
      delete p;
      return l_sRet;
    }


    /**
    * Change the Z-Layer for the menu controller
    * @param m_iZLayer the new Z-Layer
    */
    void CLuaSingleton_system::setZLayer(int a_iZLayer) {
      m_pGlobal->getActiveState()->setZLayer(a_iZLayer);
    }

    /**
    * Remove a texture from the Irrlicht texture cache. Use this function with caution as it will
    * most likely cause crashes if you remove a still needed texture!
    * @param a_sTexture (file)name of the texture to remove
    */
    void CLuaSingleton_system::removeTexture(const std::string& a_sTexture) {
      std::string l_sTexture = a_sTexture;
      size_t l_iPos = l_sTexture.find("://");

      if (l_iPos != std::string::npos)
        l_sTexture = l_sTexture.substr(l_iPos + 3);

      irr::video::ITexture* p = m_pGlobal->getVideoDriver()->findTexture(l_sTexture.c_str());
      if (p != nullptr) {
        m_pGlobal->getVideoDriver()->removeTexture(p);
      }
    }

    /**
    * Check whether or not a file exits
    * @param a_sFile the file to check
    * @return true if the file exists, false otherwise
    */
    bool CLuaSingleton_system::fileExists(const std::string& a_sFile) {
      return m_pGlobal->getFileSystem()->existFile(a_sFile.c_str());
    }

    /**
    * Get the list of tracks
    * @param a_pState the LUA state
    * @return the number of result values, 1 in this case
    */
    int CLuaSingleton_system::getTrackList(lua_State* a_pState) {
      irr::io::IFileSystem* l_pFs = m_pGlobal->getFileSystem();

      std::string l_sFolder = l_pFs->getWorkingDirectory().c_str();

      l_pFs->changeWorkingDirectoryTo("data/levels");

      irr::io::IFileList* l_pFolders = l_pFs->createFileList();

      int l_iFound = 1;

      lua_newtable(a_pState);

      if (l_pFolders) {
        for (unsigned i = 0; i < l_pFolders->getFileCount(); i++) {
          if (l_pFolders->isDirectory(i)) {
            std::string l_sFolder = l_pFolders->getFileName(i).c_str();

            if (l_sFolder != "." && l_sFolder != "..") {
              std::string l_sName = l_sFolder + "/track.xml",
                          l_sInfo = l_sFolder + "/info.xml";

              if (l_pFs->existFile(l_sName.c_str())) {
                std::string l_sName = l_sFolder;
                int         l_iPos  = -1;

                printf("%2i: ==> %s\n", l_iFound, l_sName.c_str());
                
                if (l_pFs->existFile(l_sInfo.c_str())) {
                  irr::io::IXMLReaderUTF8* l_pXml = l_pFs->createXMLReaderUTF8(l_sInfo.c_str());
                  int l_iState = 0;

                  if (l_pXml) {
                    while (l_pXml->read()) {
                      std::string l_sNode = l_pXml->getNodeName();

                      switch (l_iState) {
                        case 0: {
                          if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                            if (l_sNode == "name")
                              l_iState = 1;
                            else if (l_sNode == "position")
                              l_iState = 2;
                          }
                          break;
                        }

                        case 1: {
                          if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                            l_sName = l_pXml->getNodeData();
                            l_iState = 0;
                          }
                          break;
                        }

                        case 2: {
                          if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                            l_iPos = std::atoi(l_pXml->getNodeData());
                            l_iState = 0;
                          }
                          break;
                        }
                      }
                    }
                    l_pXml->drop();
                  }
                }

                STrack l_cTrack;
                l_cTrack.m_folder   = l_sFolder;
                l_cTrack.m_name     = l_sName;
                l_cTrack.m_position = l_iPos;

                lua_pushinteger(a_pState, l_iFound);
                l_cTrack.pushToStack(a_pState);
                lua_settable(a_pState, -3);

                l_iFound++;
              }
            }
          }
        }

        l_pFolders->drop();
      }

      l_pFs->changeWorkingDirectoryTo(l_sFolder.c_str());

      return 1;
    }


    CLuaResolutionList::CLuaResolutionList(lua_State* a_pState) {
      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaResolutionList>("LuaResolutionList")
          .addFunction("count", &CLuaResolutionList::getCount)
          .addFunction("get", &CLuaResolutionList::getResolutionString)
        .endClass();

      refresh();
    }

    void CLuaResolutionList::refresh() {
      m_vList.clear();

      irr::video::IVideoModeList *l_pList = CGlobal::getInstance()->getIrrlichtDevice()->getVideoModeList();

      irr::core::dimension2du l_cSize = CGlobal::getInstance()->getVideoDriver()->getScreenSize();
      bool l_bFound = false;

      for (irr::s32 i = 0; i < l_pList->getVideoModeCount(); i++) {
        irr::core::dimension2du l_cDim = l_pList->getVideoModeResolution(i);

        bool l_bAdd = true;

        for (std::vector<irr::core::dimension2du>::iterator it = m_vList.begin(); it != m_vList.end(); it++) {
          if ((*it).Width == l_cDim.Width && (*it).Height == l_cDim.Height)
            l_bAdd = false;
        }

        if (l_bAdd) {
          m_vList.push_back(irr::core::dimension2du(l_cDim));

          if (l_cDim == l_cSize)
            l_bFound = true;
        }
      }

      if (!l_bFound)
        m_vList.push_back(l_cSize);

      std::sort(m_vList.begin(), m_vList.end(), [](const irr::core::dimension2du &a_cDim1, const irr::core::dimension2du &a_cDim2) {
          if (a_cDim1.Width != a_cDim2.Height) {
            if (a_cDim1.Width < a_cDim2.Width)
              return true;
            else
              return false;
          }
          else {
            if (a_cDim1.Height < a_cDim2.Height)
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