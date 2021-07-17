// (w) 2021 by Dustbin::Games / Christian Keimel
#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>

#include <lua.hpp>
#include <LuaBridge/LuaBridge.h>

#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <messages/CMessageHelpers.h>
#include <lua/CLuaSingleton_system.h>
#include <sound/CSoundInterface.h>
#include <gui/CGuiItemFactory.h>
#include <platform/CPlatform.h>
#include <platform/CPlatform.h>
#include <state/CErrorState.h>
#include <gui_freetype_font.h>
#include <gui/CMenuButton.h>
#include <state/CLuaState.h>
#include <CMainClass.h>
#include <algorithm>

namespace dustbin {
  CMainClass::CMainClass() : m_pActiveState(nullptr), m_pFs(nullptr), m_pSmgr(nullptr), m_pGui(nullptr), m_pDrv(nullptr), m_pDevice(nullptr), m_eStateChange(state::enState::None), m_pSoundInterface(nullptr), m_iRasterSize(-1), m_eState(enAppState::Continue) {
    CGlobal::m_pInstance = this;

#ifdef _OPENGL_ES
    irr::video::E_DRIVER_TYPE l_eDriver = irr::video::EDT_OGLES2;
#else
    irr::video::E_DRIVER_TYPE l_eDriver = irr::video::EDT_OPENGL;
#endif

    irr::IrrlichtDevice *l_pDevice = irr::createDevice(irr::video::EDT_NULL);

    std::wstring l_sPath = platform::portableGetDataPath() + L"/setup.xml";
    irr::io::IXMLReader *l_pXml = l_pDevice->getFileSystem()->createXMLReader(l_sPath.c_str());

    if (l_pXml) {
      while (l_pXml->read()) {
        irr::core::stringw l_sNode = l_pXml->getNodeName();
        if (irr::core::stringw(L"setting") == l_sNode) {
          std::string l_sName  = "", 
            l_sValue = "";

          for (irr::u32 i = 0; i < l_pXml->getAttributeCount(); i++) {
            if (irr::core::stringw(l_pXml->getAttributeName(i)) == L"name" ) l_sName  = irr::core::stringc(l_pXml->getAttributeValue(i)).c_str();
            if (irr::core::stringw(l_pXml->getAttributeName(i)) == L"value") l_sValue = messages::urlDecode(irr::core::stringc(l_pXml->getAttributeValue(i)).c_str());
          }

          if (l_sName != "" && l_sValue != "")
            m_mSettings[l_sName] = l_sValue;
        }
      }
      l_pXml->drop();
    }

    irr::video::IVideoModeList* l_pList = l_pDevice->getVideoModeList();

    irr::core::dimension2du l_cScreenSize = irr::core::dimension2du(0, 0);

    for (int i = 0; i < l_pList->getVideoModeCount(); i++) {
      irr::core::dimension2du l_cDim = l_pList->getDesktopResolution();
      if (l_cDim.Width > l_cScreenSize.Width && l_cDim.Height > l_cScreenSize.Height)
        l_cScreenSize = l_cDim;
    }

    l_pDevice->closeDevice();
    l_pDevice->run();
    l_pDevice->drop();

    std::string l_sSettings = getSetting("settings");
    bool l_bSettingsLoaded = false;

    if (l_sSettings != "") {
      lua_State* l_pState = luaL_newstate();
      luaL_openlibs(l_pState);
      luabridge::enableExceptions(l_pState);
      
      std::string l_sScript = std::string("function getSettings()\n  return ") + l_sSettings + "\nend\n";

      printf("%s\n", l_sScript.c_str());

      luaL_dostring(l_pState, l_sScript.c_str());

      try {
        lua_getglobal(l_pState, "getSettings");
        if (!lua_isnil(l_pState, -1)) {
          if (lua_pcall(l_pState, 0, 1, 0) == 0) {
            m_cSettings.loadFromStack(l_pState);
            l_bSettingsLoaded = true;
          }
          else {
            std::string l_sMessage = std::string("ERROR: function \"scrollbar_changed\" failed: \"") + std::string(lua_tostring(l_pState, -1)) + std::string("\"");
            printf("Error: \"%s\"\n", l_sMessage.c_str());
          }
        }
      }
      catch (luabridge::LuaException e) {
        printf("*** Settings not loaded.\n");
      }

      lua_close(l_pState);
    }

    if (!l_bSettingsLoaded) {
      m_cSettings.m_gfx_resolution_w = l_cScreenSize.Width;
      m_cSettings.m_gfx_resolution_h = l_cScreenSize.Height;
      m_cSettings.m_gfx_fullscreen = true;
      m_cSettings.m_gfx_shadows = 1;
      m_cSettings.m_gfx_ambientlight = 2;
      m_cSettings.m_sfx_master = 1000;
      m_cSettings.m_sfx_soundtrack = 750;
      m_cSettings.m_sfx_menu = 500;
      m_cSettings.m_sfx_game = 500;
      m_cSettings.m_ingame_racetime = true;
      m_cSettings.m_ingame_laptimes = false;
      m_cSettings.m_ingame_ranking = true;
      m_cSettings.m_ingame_rearview = false;
      m_cSettings.m_misc_menuctrl = "";
      m_cSettings.m_misc_usemenuctrl = false;
    }

    m_pDevice = irr::createDevice(l_eDriver, irr::core::dimension2du(m_cSettings.m_gfx_resolution_w, m_cSettings.m_gfx_resolution_h), 32, m_cSettings.m_gfx_fullscreen, false, false, this);

    if (m_pDevice != nullptr) {
      irr::core::array<irr::SJoystickInfo> l_cList;
      m_pDevice->activateJoysticks(l_cList);

      m_pDevice->setResizable(true);

      scenenodes::CMarbleGPSceneNodeFactory *l_pMGPFactory = new scenenodes::CMarbleGPSceneNodeFactory(m_pDevice->getSceneManager());
      m_pDevice->getSceneManager()->registerSceneNodeFactory(l_pMGPFactory);
      l_pMGPFactory->drop();

      m_pDevice->getTimer()->start();

      m_pSmgr = m_pDevice->getSceneManager();
      m_pGui  = m_pDevice->getGUIEnvironment();
      m_pDrv  = m_pDevice->getVideoDriver();
      m_pFs   = m_pDevice->getFileSystem();

      gui::CGuiItemFactory* l_pGuiFactory = new gui::CGuiItemFactory(m_pGui);
      m_pGui->registerGUIElementFactory(l_pGuiFactory);
      l_pGuiFactory->drop();

      for (unsigned i = 0; i < m_pGui->getRegisteredGUIElementFactoryCount(); i++) {
        irr::gui::IGUIElementFactory* l_pFactory = m_pGui->getGUIElementFactory(i);

        for (int j = 0; j < l_pFactory->getCreatableGUIElementTypeCount(); j++) {
          printf("Factory %i element %-2i: \"%s\"\n", i, j, l_pFactory->getCreateableGUIElementTypeName(j));
        }
      }

      std::string l_sFontPath = platform::portableGetFontPath();

      if (l_sFontPath != "") {
        printf("Font path: \"%s\"\n", l_sFontPath.c_str());

        std::vector<std::string> l_vSearchFonts = {
          "Arial.ttf",
          "Arialbd.ttf"
        };

        for (std::vector<std::string>::iterator it = l_vSearchFonts.begin(); it != l_vSearchFonts.end(); it++) {
          if (m_pFs->existFile(std::string(l_sFontPath + "/" + *it).c_str())) {
            l_sFontPath += "/" + *it;
            break;
          }
        }
      }

      m_pFontFace = new CGUITTFace();

      if (m_pFs->existFile(l_sFontPath.c_str()))
        m_pFontFace->load(l_sFontPath.c_str());
      else
        m_pFontFace->load("data/fonts/adventpro-regular.ttf");

      m_pGui->getSkin()->setFont(getFont(enFont::Regular, m_pDrv->getScreenSize()));

      irr::video::SColor l_cColor = m_pGui->getSkin()->getColor(irr::gui::EGDC_HIGH_LIGHT);
      l_cColor.setAlpha(232);
      m_pGui->getSkin()->setColor(irr::gui::EGDC_HIGH_LIGHT, l_cColor);

      l_cColor = m_pGui->getSkin()->getColor(irr::gui::EGDC_3D_HIGH_LIGHT);
      l_cColor.setAlpha(232);
      m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_HIGH_LIGHT, l_cColor);

      m_pGui->getSkin()->setColor(irr::gui::EGDC_HIGH_LIGHT, irr::video::SColor(0xFF, 0, 0, 0));
      m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_FACE, irr::video::SColor(0xFF, 232, 232, 232));
      m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_HIGH_LIGHT, irr::video::SColor(0xFF, 232, 232, 232));
      m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_LIGHT, irr::video::SColor(0xFF, 232, 232, 232));
      m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_SHADOW, irr::video::SColor(0xFF, 128, 128, 128));

      m_mStates[state::enState::LuaState] = new state::CLuaState();
      m_mStates[state::enState::ErrorState] = new state::CErrorState();

      m_cScreenSize = m_pDrv->getScreenSize();
    }
  }

  CMainClass::~CMainClass() {
    std::wstring l_sPath = platform::portableGetDataPath() + L"/setup.xml";

    irr::io::IXMLWriter *l_pXml = m_pFs->createXMLWriter(l_sPath.c_str());

    if (l_pXml) {
      l_pXml->writeXMLHeader();
      l_pXml->writeElement(L"marbles3", false);
      l_pXml->writeLineBreak();

      for (std::map<std::string, std::string>::iterator it = m_mSettings.begin(); it != m_mSettings.end(); it++) {
        irr::core::array<irr::core::stringw> l_aNames,
          l_aValues;

        l_aNames.push_back("name" ); l_aValues.push_back(irr::core::stringw(it->first .c_str()));
        l_aNames.push_back("value"); l_aValues.push_back(irr::core::stringw(messages::urlEncode(it->second.c_str()).c_str()));

        l_pXml->writeElement(L"setting", true, l_aNames, l_aValues);
        l_pXml->writeLineBreak();
      }


      l_pXml->writeClosingTag(L"marbles3");

      l_pXml->drop();
    }

    if (m_pActiveState)
      m_pActiveState->deactivate();

    for (std::map<dustbin::state::enState, dustbin::state::IState *>::iterator it = m_mStates.begin(); it != m_mStates.end(); it++)
      delete it->second;

    m_mStates.clear();

    while (m_mFonts.size() > 0) {
      m_mFonts.begin()->second->drop();
      m_mFonts.erase(m_mFonts.begin());
    }

    m_mFonts.clear();

    delete m_pSoundInterface;
    m_pFontFace->drop();

    m_pGui->clear();
    m_pDrv->removeAllTextures();

    CGlobal::m_pInstance = nullptr;

    m_pDevice->closeDevice();
    m_pDevice->run();
    m_pDevice->drop();
  }

  /**
  * This method does everything. On return the application ends
  */
  enAppState CMainClass::run() {
    if (m_pDevice != nullptr) {
      if (m_pDevice->run()) {
        try {
          int l_iFps = m_pDrv->getFPS();
          m_pDevice->setWindowCaption(std::wstring(L"Dustbin::Games - MarbleGP [" + std::to_wstring(l_iFps) + L" FPS]").c_str());

          m_pDrv->beginScene();
          m_pSmgr->drawAll();

          for (std::vector<irr::scene::ISceneManager*>::iterator it = m_vBeforeGui.begin(); it != m_vBeforeGui.end(); it++) {
            m_pDrv->clearZBuffer();
            (*it)->drawAll();
          }

          m_pGui->drawAll();

          for (std::vector<irr::scene::ISceneManager*>::iterator it = m_vAfterGui.begin(); it != m_vAfterGui.end(); it++) {
            m_pDrv->clearZBuffer();
            (*it)->drawAll();
          }

          m_pDrv->endScene();

          state::enState l_eState = state::enState::None;

          if (m_pActiveState != nullptr)
            l_eState = m_pActiveState->run();
          else
            l_eState = state::enState::LuaState;

          if (l_eState != state::enState::None) {
            if (m_pActiveState != nullptr)
              m_pActiveState->deactivate();

            m_pActiveState = m_mStates.find(l_eState) != m_mStates.end() ? m_mStates[l_eState] : nullptr;

            if (m_pActiveState != nullptr) {
              m_pActiveState->activate();
            }
          }

          irr::core::dimension2du l_cThisSize = m_pDrv->getScreenSize();
          if (l_cThisSize != m_cScreenSize) {
            m_pActiveState->beforeResize();

            for (std::vector<irr::video::ITexture*>::iterator it = m_vRemoveOnResize.begin(); it != m_vRemoveOnResize.end(); it++) {
              m_pDrv->removeTexture(*it);
            }

            m_pGui->clear();

            while (m_mFonts.size() > 0) {
              m_mFonts.begin()->second->drop();
              m_mFonts.erase(m_mFonts.begin());
            }

            m_cScreenSize = l_cThisSize;
            m_iRasterSize = -1;

            m_cSettings.m_gfx_resolution_w = m_cScreenSize.Width;
            m_cSettings.m_gfx_resolution_h = m_cScreenSize.Height;

            m_pGui->getSkin()->setFont(getFont(enFont::Regular, m_pDrv->getScreenSize()));
            m_pActiveState->onResize(m_cScreenSize);
          }
        }
        catch (std::exception e) {
          m_pActiveState = m_mStates[state::enState::ErrorState];
          m_pActiveState->activate();
        }
      }
      else m_eState = enAppState::Quit;
    }

    return m_eState;
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
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          switch (a_cEvent.GUIEvent.Caller->getType()) {
            case gui::g_MenuButtonId:
            case irr::gui::EGUIET_BUTTON:
            case irr::gui::EGUIET_CHECK_BOX:
            case irr::gui::EGUIET_COMBO_BOX:
            case irr::gui::EGUIET_SPIN_BOX:
            case irr::gui::EGUIET_SCROLL_BAR:
              getSoundInterface()->play2d(L"data/sounds/button_hover.ogg", 1.0f, 0.0f);
              break;

            default:
              // Play no sound
              break;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED ||
          a_cEvent.GUIEvent.EventType == irr::gui::EGET_COMBO_BOX_CHANGED ||
          a_cEvent.GUIEvent.EventType == irr::gui::EGET_CHECKBOX_CHANGED) {
          getSoundInterface()->play2d(L"data/sounds/button_press.ogg", 1.0f, 0.0f);
        }
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
    // else return "data/lua/menu_main.lua";
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


  /**
  * Register a new scene manager for drawing
  * @param a_pSmgr the new scene manager
  * @param a_iRenderPosition the position to render the new scene manager. "0" is the render index of the GUI environment, negative position renders the new scene manager before the gui
  */
  void CMainClass::registerSceneManager(irr::scene::ISceneManager* a_pSmgr, int a_iRenderPosition) {
    if (a_iRenderPosition < 0)
      m_vBeforeGui.push_back(a_pSmgr);
    else
      m_vAfterGui.push_back(a_pSmgr);
  }

  /**
  * Remove a scene manager from the rendering pipeline
  * @param a_pSmgr the scene manager to remove
  */
  void CMainClass::removeSceneManager(irr::scene::ISceneManager* a_pSmgr) {
    std::vector<irr::scene::ISceneManager *>::iterator it = std::find(m_vBeforeGui.begin(), m_vBeforeGui.end(), a_pSmgr);
    if (it != m_vBeforeGui.end())
      m_vBeforeGui.erase(it);

    it = std::find(m_vAfterGui.begin(), m_vAfterGui.end(), a_pSmgr);
    if (it != m_vAfterGui.end())
      m_vAfterGui.erase(it);
  }

  /**
  * This function requests a state change. The change will be applied with the next "IState::run" call
  * @param a_eState the new state
  */
  void CMainClass::stateChange(dustbin::state::enState a_eNewState) {
    m_eStateChange = a_eNewState;
    if (m_eStateChange == state::enState::Quit)
      m_eState = enAppState::Quit;
  }

  /**
   * Get the raster size for the UI layout
   * @return the raster size
   */
  int CMainClass::getRasterSize() {
    if (m_iRasterSize == -1) {
      irr::core::dimension2du l_cDim = m_pDrv->getScreenSize();

      int l_iWidth  = l_cDim.Width  / 80,
          l_iHeight = l_cDim.Height / 55;

      m_iRasterSize = l_iWidth < l_iHeight ? l_iWidth : l_iHeight;
    }

    return m_iRasterSize;
  }

  /**
   * Get a rectangle on the screen for GUI layout
   * @param a_iLeft Left position
   * @param a_iTop Top position
   * @param a_iRight Right position
   * @param a_iBottom bottom position
   * @param a_ePosition the layout position of the rectangle
   * @param a_pParent an optional parent element
   */
  irr::core::recti CMainClass::getRect(int a_iLeft, int a_iTop, int a_iRight, int a_iBottom, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent) {
    irr::core::position2di  l_cAnchor;
    int                     l_iRaster = getRasterSize();
    irr::core::dimension2du l_cSize   = m_pDrv->getScreenSize();

    switch (a_ePosition) {
      case enLayout::UpperLeft:
      case enLayout::Relative: {
        l_cAnchor = irr::core::position2di(0, 0);
        break;
      }

      case enLayout::UpperMiddle: {
        l_cAnchor = irr::core::position2di((l_cSize.Width / 2) - (l_iRaster / 2), 0);
        break;
      }

      case enLayout::UpperRight: {
        l_cAnchor = irr::core::position2di(l_cSize.Width - l_iRaster, 0);
        break;
      }

      case enLayout::Left: {
        l_cAnchor = irr::core::position2di(0, l_cSize.Height / 2);
        break;
      }

      case enLayout::Center: {
        l_cAnchor = irr::core::position2di((l_cSize.Width / 2) - (l_iRaster / 2), (l_cSize.Height / 2) - (l_iRaster / 2));
        break;
      }

      case enLayout::Right: {
        l_cAnchor = irr::core::position2di(l_cSize.Width - l_iRaster, l_cSize.Height / 2);
        break;
      }

      case enLayout::LowerLeft: {
        l_cAnchor = irr::core::position2di(0, l_cSize.Height - l_iRaster);
        break;
      }

      case enLayout::LowerMiddle: {
        l_cAnchor = irr::core::position2di((l_cSize.Width / 2) - (l_iRaster / 2), l_cSize.Height - l_iRaster);
        break;
      }

      case enLayout::LowerRight: {
        l_cAnchor = irr::core::position2di(l_cSize.Width - l_iRaster, l_cSize.Height - l_iRaster);
        break;
      }
    }

    irr::core::position2di l_cPos = l_cAnchor + irr::core::position2di(a_iLeft * l_iRaster, a_iTop * l_iRaster);

    irr::core::recti l_cRet = irr::core::recti(l_cPos, irr::core::dimension2du((a_iRight - a_iLeft) * l_iRaster, (a_iBottom - a_iTop) * l_iRaster));

    if (a_pParent != nullptr && a_ePosition != enLayout::Relative) {
      l_cRet.UpperLeftCorner  -= a_pParent->getAbsoluteClippingRect().UpperLeftCorner;
      l_cRet.LowerRightCorner -= a_pParent->getAbsoluteClippingRect().UpperLeftCorner;
    }

    return l_cRet;
  }

  /**
   * Get a rectangle on the screen for GUI layout
   * @param a_cRect Irrlicht rectangle with the coordinates to be converted
   * @param a_ePosition the layout position of the rectangle
   * @param a_pParent an optional paraent element
   */
  irr::core::recti CMainClass::getRect(const irr::core::recti& a_cRect, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent) {
    return getRect(a_cRect.UpperLeftCorner.X, a_cRect.UpperLeftCorner.Y, a_cRect.LowerRightCorner.X, a_cRect.LowerRightCorner.Y, a_ePosition, a_pParent);
  }

  /**
  * Get a requested state change. This function also sets the corresponding member back to "None"
  * @return the state change
  */
  dustbin::state::enState CMainClass::getStateChange() {
    state::enState l_eRet = m_eStateChange;
    m_eStateChange = state::enState::None;
    return l_eRet;
  }

  state::IState* CMainClass::getActiveState() {
    return m_pActiveState;
  }

  dustbin::audio::CSoundInterface* CMainClass::getSoundInterface() {
    if (m_pSoundInterface == nullptr) {
      m_pSoundInterface = new audio::CSoundInterface(m_pFs);

      m_pSoundInterface->preloadSound(L"data/sounds/theme_menu.ogg");
      m_pSoundInterface->preloadSound(L"data/sounds/theme_race.ogg");
      m_pSoundInterface->preloadSound(L"data/sounds/theme_result.ogg");
      m_pSoundInterface->preloadSound(L"data/sounds/button_hover.ogg");
      m_pSoundInterface->preloadSound(L"data/sounds/button_press.ogg");

      m_pSoundInterface->assignSoundtracks(
        {
          { enSoundTrack::enStMenu  , L"data/sounds/theme_menu.ogg" },
          { enSoundTrack::enStRace  , L"data/sounds/theme_race.ogg" },
          { enSoundTrack::enStFinish, L"data/sounds/theme_result.ogg" }
        }
      );
    }

    return m_pSoundInterface;
  }

  /**
  * Get a font for a specific viewport size
  * @param a_eFont the type of font
  * @param a_cViewport the viewport size
  * @see dustbin::enFont
  */
  irr::gui::IGUIFont* CMainClass::getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport) {
    // Tiny: 0.5 * RasterSize, Small: 0.75 * RasterSize, Regular: RasterSize, Big: 1.5 * RasterSize, Huge: 2 * RasterSize
    int l_iSize = getRasterSize();

    switch (a_eFont) {
      case enFont::Tiny   : l_iSize = 3 * l_iSize / 4; break;
      case enFont::Small  : l_iSize =     l_iSize    ; break;
      case enFont::Regular: l_iSize = 3 * l_iSize / 2; break;
      case enFont::Big    : l_iSize = 2 * l_iSize    ; break;
      case enFont::Huge   : l_iSize = 5 * l_iSize / 2; break;
    }

    if (m_mFonts.find(l_iSize) == m_mFonts.end()) {
      CGUIFreetypeFont* l_pFont = new CGUIFreetypeFont(m_pDrv);
      l_pFont->AntiAlias = true;
      l_pFont->attach(m_pFontFace, l_iSize);
      m_mFonts[l_iSize] = l_pFont;
    }

    return m_mFonts[l_iSize];
  }

  /**
  * Get the settings struct
  * @return the settings struct
  */
  const SSettings& CMainClass::getSettings() {
    return m_cSettings;
  }

  /**
  * Update the game settings
  * @param a_cSettings the settings
  */
  void CMainClass::setSettings(const SSettings& a_cSettings) {
    int l_iOldWidth  = m_cSettings.m_gfx_resolution_w,
        l_iOldHeight = m_cSettings.m_gfx_resolution_h;

    bool l_bOldFullscreen = m_cSettings.m_gfx_fullscreen;

    m_cSettings.copyFrom(a_cSettings);

    if (m_cSettings.m_gfx_resolution_w != l_iOldWidth || m_cSettings.m_gfx_resolution_h != l_iOldHeight || m_cSettings.m_gfx_fullscreen != l_bOldFullscreen) {
      m_eState = enAppState::Restart;
    }

    lua_State* l_pState = luaL_newstate();
    luaL_openlibs(l_pState);
    luabridge::enableExceptions(l_pState);

    lua::CLuaSingleton_system *l_pSystem = new lua::CLuaSingleton_system(l_pState);

    std::string l_sScript = "system:executeluascript(\"data/lua/serializer.lua\")\n";
    if (luaL_dostring(l_pState, l_sScript.c_str()) == LUA_OK) {
      try {
        try {
          lua_getglobal(l_pState, "serializeTable");
          if (!lua_isnil(l_pState, -1)) {
            m_cSettings.pushToStack(l_pState);
            lua_pushinteger(l_pState, 2);
            if (lua_pcall(l_pState, 2, 1, 0) != 0) {
              std::string l_sMessage = std::string("ERROR: function \"serializeTable\" failed: \"") + std::string(lua_tostring(l_pState, -1)) + std::string("\"");
              printf("Error: \"%s\"\n", l_sMessage.c_str());
            }
            else {
              std::string s = lua_tostring(l_pState, lua_gettop(l_pState));
              setSetting("settings", s);
            }
          }
        }
        catch (luabridge::LuaException e) {
          printf("*** Settings serialization failed: \"%s\".\n", lua_tostring(l_pState, -1));
        }
      }
      catch (luabridge::LuaException e) {
        printf("*** Settings not loaded.\n");
      }
    }
    else printf("Error while saving settings: \"%s\".\n", lua_tostring(l_pState, -1));

    delete l_pSystem;

    lua_close(l_pState);
  }

  /**
  * Get an image from a string. The following prefixes are possible:
  * - file://: load a file from a subfolder
  * - generate://: generate a marble texture
  * @param a_sInput the URI of the file
  * @return an Irrlicht texture object with the requested image or nullptr
  */
  irr::video::ITexture* CMainClass::createTexture(const std::string& a_sUri) {
    irr::video::ITexture* l_pRet = nullptr;

    size_t l_iPos = a_sUri.find("://");

    if (l_iPos != std::string::npos) {
      std::string l_sPrefix  = a_sUri.substr(0, l_iPos ),
                  l_sPostFix = a_sUri.substr(l_iPos + 3);

      if (l_sPrefix == "file") {
        l_pRet = m_pDrv->getTexture(l_sPostFix.c_str());  
      }
    }

    return l_pRet;
  }
}

