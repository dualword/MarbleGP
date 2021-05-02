// (w) 2021 by Dustbin::Games / Christian Keimel
#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <state/CLuaState.h>
#include <CMainClass.h>
#include <chrono>
#include <thread>

namespace dustbin {
  CMainClass::CMainClass() : m_pActiveState(nullptr), m_pFs(nullptr), m_pSmgr(nullptr), m_pGui(nullptr), m_pDrv(nullptr), m_pDevice(nullptr) {
    CGlobal::m_pInstance = this;

#ifdef _OPENGL_ES
    irr::video::E_DRIVER_TYPE l_eDriver = irr::video::EDT_OGLES2;
    bool l_bFullScreen = true;
#else
    irr::video::E_DRIVER_TYPE l_eDriver = irr::video::EDT_OPENGL;
    bool l_bFullScreen = false;
#endif


    m_pDevice = irr::createDevice(l_eDriver, irr::core::dimension2du(1028, 768), 32, l_bFullScreen, false, false, this);

    if (m_pDevice != nullptr) {
      m_pDevice->setResizable(true);

      scenenodes::CMarbleGPSceneNodeFactory *l_pMGPFactory = new scenenodes::CMarbleGPSceneNodeFactory(m_pDevice->getSceneManager());
      m_pDevice->getSceneManager()->registerSceneNodeFactory(l_pMGPFactory);
      l_pMGPFactory->drop();
    }
  }

  CMainClass::~CMainClass() {
    CGlobal::m_pInstance = nullptr;
  }

  /**
  * This method does everything. On return the application ends
  */
  void CMainClass::run() {

    if (m_pDevice != nullptr) {
      m_pSmgr = m_pDevice->getSceneManager();
      m_pGui  = m_pDevice->getGUIEnvironment();
      m_pDrv  = m_pDevice->getVideoDriver();
      m_pFs   = m_pDevice->getFileSystem();

      m_mStates[state::enState::LuaState] = new state::CLuaState();
      m_pActiveState = m_mStates.begin()->second;
      
      if (m_pActiveState != nullptr) {
        m_pActiveState->activate();

        std::chrono::high_resolution_clock::time_point l_cNextStep = std::chrono::high_resolution_clock::now();

        irr::core::dimension2du l_cSize = m_pDrv->getScreenSize();

        m_pDevice->setWindowCaption(L"MarbleGP - Dustbin::Games");

        while (m_pDevice->run() && m_pActiveState != nullptr) {
          state::enState l_eState = m_pActiveState->run();

          irr::core::dimension2du l_cThisSize = m_pDrv->getScreenSize();
          if (l_cThisSize != l_cSize) {
            l_cSize = l_cThisSize;
            printf("Rescaled: %i, %i\n", l_cSize.Width, l_cSize.Height);
            m_pActiveState->onResize(l_cSize);
          }

          if (l_eState != state::enState::None) {
            m_pActiveState->deactivate();
            m_pActiveState = m_mStates.find(l_eState) != m_mStates.end() ? m_mStates[l_eState] : nullptr;

            if (m_pActiveState != nullptr) {
              m_pActiveState->activate();
            }
          }

          l_cNextStep = l_cNextStep + std::chrono::duration<int, std::ratio<1, 1000>>(10);
          std::this_thread::sleep_until(l_cNextStep);
        }
      }

      m_pDevice->drop();
    }
  }

  // Methods inherited from CGlobal

  /**
  * Determine whether or not a setting value exists for the given key
  * @param a_sKey the key to search for
  * @return true if the key exists, false otherwise
  */
  bool CMainClass::hasSetting(const std::string& a_sKey) {
    return m_mSettings.find(a_sKey) != m_mSettings.end();
  }

  /**
  * Method to get a setting. Settings are stored and loaded when the application ends or starts.
  * @param a_sKey key of the setting
  * @return value assigned to the key, empty string if no value was found
  */
  std::string CMainClass::getSetting(const std::string& a_sKey) {
    if (hasSetting(a_sKey)) return std::string(m_mSettings[a_sKey]); else return "";
  }

  /**
  * Method to modify or set a setting. Settings are stored and loaded when the application ends or starts.
  * @param a_sKey key of the setting
  * @param a_sValue value of the setting. An empty string will erase the setting
  */
  void CMainClass::setSetting(const std::string& a_sKey, const std::string& a_sValue) {
    if (a_sValue != "")
      m_mSettings[a_sKey] = a_sValue;
    else
      if (hasSetting(a_sKey)) m_mSettings.erase(a_sKey);
  }


  /**
  * Modify or set a global variable. Globals are not loaded or saved so they live only while the application is running
  * @param a_sKey the key of the global
  * @param a_sValue the value of the global. An empty string will erase the global
  */
  void CMainClass::setGlobal(const std::string& a_sKey, const std::string& a_sValue) {
    if (a_sValue != "")
      m_mGlobals[a_sKey] = a_sValue;
    else
      if (hasGlobal(a_sKey)) m_mGlobals.erase(a_sKey);
  }

  /**
  * Retrieve a global. Globals are not loaded or saved so they live only while the application is running
  * @param a_sKey the key of the global
  * @return the global assigned to the key, an empty string if the global does not exist
  */
  std::string CMainClass::getGlobal(const std::string& a_sKey) {
    if (hasGlobal(a_sKey)) return std::string(m_mGlobals[a_sKey]); else return "";
  }


  /**
  * Determine whether or not a global exists
  * @param a_sKey the key to search for
  * @return true if the global exists, false otherwise
  */
  bool CMainClass::hasGlobal(const std::string& a_sKey) {
    return m_mGlobals.find(a_sKey) != m_mGlobals.end();
  }

  /**
  * Get the active Irrlicht device
  * @return the active Irrlicht device
  */
  irr::IrrlichtDevice* CMainClass::getIrrlichtDevice() {
    return m_pDevice;
  }

  /**
  * Get the Irrlicht File System Instance
  * @return the Irrlicht File System Instance
  */
  irr::io::IFileSystem* CMainClass::getFileSystem() {
    return m_pFs;
  }

  /**
  * Get the Irrlicht Scene Manager Instance
  * @return 
  */
  irr::scene::ISceneManager* CMainClass::getSceneManager() {
    return m_pSmgr;
  }

  /**
  * Get the Irrlicht gui environment instance
  * @return the Irrlicht gui environment instance
  */
  irr::gui::IGUIEnvironment* CMainClass::getGuiEnvironment() {
    return m_pGui;
  }

  /**
  * Get the Irrlicht video driver instance
  * @return the Irrlicht video driver instance
  */
  irr::video::IVideoDriver* CMainClass::getVideoDriver() {
    return m_pDrv;
  }

  /**
  * Open a file for reading. The folder is defined by the file type, and the function searches
  * the data folder first, thereafter it searches the MarbleGP.dat file
  * @param a_eType type of file
  * @param a_sName name of file (without path)
  * @return an Irrlicht read file object instance
  */
  irr::io::IReadFile* CMainClass::openFileForReading(enFileType a_eType, const std::string& a_sName) {
    return nullptr;
  }

  /**
  * Opens a file for writing. This function will always return a file in the "data" folder of the user's data directory
  * @param a_sName name of the file
  */
  irr::io::IWriteFile* CMainClass::openFileForWriting(const std::string& a_sName) {
    return nullptr;
  }

  /**
  * Search for a file. The place to look for the file is either the user's data folder or both
  * @param a_eSearch the seach location (data folder, MarbleGP data archive, both)
  * @param a_eType the type of file to search. This defines the folder to search in
  * @param a_sName the name of the file (without path information)
  * @return true if the file exists, false otherwise
  */
  bool CMainClass::existsFile(enFileSearch a_eSeach, enFileType a_eType, const std::string& a_sName) {
    return false;
  }



  bool CMainClass::OnEvent(const irr::SEvent& a_cEvent) {
    bool l_bRet = m_pActiveState != nullptr ? m_pActiveState->OnEvent(a_cEvent) : false;

    if (!l_bRet) {
      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
      }
    }

    return l_bRet;
  }


  /**
  * Push a script to the stack of lua scripts
  * @param a_sScript the script to push
  */
  void CMainClass::pushScript(const std::string& a_sScript) {
    m_vScriptStack.push_back(a_sScript);
  }

  /**
  * Remove and return the topmost script from the stack of lua scripts
  * @return the topmost script from the lus script stack
  */
  std::string CMainClass::popScript() {
    if (m_vScriptStack.size() > 0) {
      std::string m_sTemp = m_vScriptStack.back();
      m_vScriptStack.pop_back();
      printf("Pop LUA script: \"%s\"\n", m_sTemp.c_str());
      return m_sTemp;
    }
    else return "data/lua/menu_main.lua";
  }

  /**
  * Get the topmost script from the lua script stack without removing it
  */
  std::string CMainClass::peekScript() {
    if (m_vScriptStack.size() > 0) {
      std::string m_sTemp = m_vScriptStack.back();
      printf("Peek LUA script: \"%s\"\n", m_sTemp.c_str());
      return m_sTemp;
    }
    else return "";
  }

  state::IState* CMainClass::getActiveState() {
    return m_pActiveState;
  }
}