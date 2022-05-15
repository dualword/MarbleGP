// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#define ENET_IMPLEMENTATION
#include <enet.h>

#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <controller/CControllerBase.h>
#include <helpers/CTextureHelpers.h>
#include <helpers/CStringHelpers.h>
#include <sound/ISoundInterface.h>
#include <network/CGameServer.h>
#include <network/CGameClient.h>
#include <helpers/CMenuLoader.h>
#include <gui/CGuiItemFactory.h>
#include <menu/IMenuHandler.h>
#include <state/CMenuState.h>
#include <state/CGameState.h>
#include <state/IState.h>
#include <CMainClass.h>
#include <cstdlib>
#include <ctime>

namespace dustbin {
  /**
  * The constructor
  * @param a_sSettings The settings XML as a string
  */
  CMainClass::CMainClass(const std::string& a_sSettings
#ifdef _ANDROID
    , android_app *a_pApp
#endif
  ) :
    m_pDevice         (nullptr),
    m_pSmgr           (nullptr),
    m_pDrv            (nullptr),
    m_pGui            (nullptr),
    m_pFs             (nullptr),
    m_iRasterSize     (-1),
    m_pCtrlMenu       (nullptr),
    m_pCtrlGame       (nullptr),
    m_pActiveState    (nullptr),
    m_pSoundInterface (nullptr),
    m_pServer         (nullptr),
    m_pClient         (nullptr),
    m_pNextRaceScreen (nullptr),
    m_pLogo           (nullptr)
#ifdef _ANDROID
    ,m_pAndroidApp     (a_pApp)
#endif
  {
    m_pInstance = this;
    std::srand((unsigned int)std::time(nullptr));

    /**
    * @class CMemoryReadFile
    * @author Christian Keimel
    * This little class is necessary to have an Irrlicht read file
    * even though the Irrlicht File system is not yet created.
    */
    class CMemoryReadFile : public irr::io::IFileReadCallBack {
      private:
        size_t m_iSize,
               m_iPos;

        const char *m_pData;

        irr::io::path m_sName;

      public:
        CMemoryReadFile(const char* a_pData, size_t a_iSize) : m_iSize(a_iSize), m_iPos(0), m_pData(a_pData), m_sName("MemoryReadFile_Settings") {
        }

        virtual long getSize() const {
          return (long)m_iSize;
        }

        virtual int read(void* a_pBuffer, int a_iReadSize) {
          irr::u32 l_iRet = 0;

          if (m_iPos + a_iReadSize < m_iSize) {
            memcpy(a_pBuffer, &m_pData[m_iPos], a_iReadSize);
            l_iRet = (irr::u32)a_iReadSize;
          }
          else {
            memcpy(a_pBuffer, &m_pData[m_iPos], m_iSize - m_iPos);
            l_iRet = (irr::u32)(m_iSize - m_iPos);
          }

          return l_iRet;
        }
    };

    CMemoryReadFile *l_pFile = new CMemoryReadFile(a_sSettings.data(), a_sSettings.size());
    if (l_pFile) {
      irr::io::IrrXMLReader *l_pXml = irr::io::createIrrXMLReader(l_pFile);
      if (l_pXml) {
        while (l_pXml->read()) {
          std::string l_sNode = l_pXml->getNodeName();
          if (l_sNode == "setting" && l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
            std::string l_sName  = l_pXml->getAttributeValueSafe("name" ),
                        l_sValue = l_pXml->getAttributeValueSafe("value");

            printf("\t%s = %s\n", l_sName.c_str(), l_sValue.c_str());

            if (l_sName != "" && l_sValue != "")
              m_mSettings[l_sName] = l_sValue;
          }
        }

        delete l_pXml;
      }

      delete l_pFile;
    }
    m_cSettings.loadSettings(m_mSettings);
  }

  CMainClass::~CMainClass() {
    if (m_pActiveState != nullptr) {
      m_pActiveState->willBeDeleted();
      m_pActiveState->deactivate();
    }

    if (m_pCtrlGame != nullptr) {
      delete m_pCtrlGame;
      m_pCtrlGame = nullptr;
    }

    if (m_pCtrlMenu != nullptr) {
      delete m_pCtrlMenu;
      m_pCtrlMenu = nullptr;
    }

    for (std::map<state::enState, state::IState*>::iterator it = m_mStates.begin(); it != m_mStates.end(); it++) {
      delete it->second;
    }

    if (m_pSoundInterface != nullptr) {
      delete m_pSoundInterface;
      m_pSoundInterface = nullptr;
    }

    if (m_pServer != nullptr) {
      m_pServer->stopThread();
      m_pServer->join();
      delete m_pServer;
      m_pServer = nullptr;
    }

    if (m_pClient != nullptr) {
      m_pClient->stopThread();
      m_pClient->join();
      delete m_pClient;
      m_pClient = nullptr;
    }

    m_mStates.clear();
  }

  /**
  * Set the Irrlicht device
  * @param a_pDevice the Irrlicht device to use
  */
  void CMainClass::setIrrlichtDevice(irr::IrrlichtDevice* a_pDevice) {
    m_pDevice = a_pDevice;
    m_pSmgr   = m_pDevice->getSceneManager  ();
    m_pDrv    = m_pDevice->getVideoDriver   ();
    m_pGui    = m_pDevice->getGUIEnvironment();
    m_pFs     = m_pDevice->getFileSystem    ();

    if (m_pFs->existFile("marblegp.dat")) {
      bool b = m_pFs->addFileArchive("marblegp.dat", true, false, irr::io::EFAT_ZIP);

      printf("Marblegp.dat added: %s\n", b ? "success" : "failed");
    }

    irr::video::SColor l_cColor = m_pGui->getSkin()->getColor(irr::gui::EGDC_HIGH_LIGHT);
    l_cColor.setAlpha(232);
    m_pGui->getSkin()->setColor(irr::gui::EGDC_HIGH_LIGHT, l_cColor);

    l_cColor = m_pGui->getSkin()->getColor(irr::gui::EGDC_3D_HIGH_LIGHT);
    l_cColor.setAlpha(232);
    m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_HIGH_LIGHT, l_cColor);

    m_pGui->getSkin()->setColor(irr::gui::EGDC_HIGH_LIGHT   , irr::video::SColor(0xFF,   0,   0,   0));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_FACE      , irr::video::SColor(0xFF, 232, 232, 232));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_HIGH_LIGHT, irr::video::SColor(0xFF, 232, 232, 232));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_LIGHT     , irr::video::SColor(0xFF, 232, 232, 232));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_3D_SHADOW    , irr::video::SColor(0xFF, 128, 128, 128));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_GRAY_EDITABLE, irr::video::SColor(0xFF, 230, 230, 230));
    m_pGui->getSkin()->setColor(irr::gui::EGDC_GRAY_TEXT    , irr::video::SColor(0xFF,   0,   0,   0));

    m_vFontSizes.clear();

    irr::io::IXMLReader *l_pXml = m_pFs->createXMLReader("data/fonts/fontsizes.xml");

    if (l_pXml != nullptr) {
      while (l_pXml->read()) {
        switch (l_pXml->getNodeType()) {
          case irr::io::EXN_ELEMENT: {
            std::wstring l_sName = l_pXml->getNodeName();
            if (l_sName == L"size") {
              m_vFontSizes.push_back(l_pXml->getAttributeValueAsInt(L"value"));
            }
          }

          default:
            break;
        }
      }
      l_pXml->drop();
    }

    // The tiny original Irrlicht font 
    // is stored as minimal sized font
    m_mFonts[0] = m_pGui->getSkin()->getFont();

    // Set the "regular" font size for the resolution to be the default font size
    m_pGui->getSkin()->setFont(getFont(enFont::Regular, m_pDrv->getScreenSize()));

    gui::CGuiItemFactory* l_pGuiFactory = new gui::CGuiItemFactory(m_pGui);
    m_pGui->registerGUIElementFactory(l_pGuiFactory);
    l_pGuiFactory->drop();

    scenenodes::CMarbleGPSceneNodeFactory *l_pNodeFactory = new scenenodes::CMarbleGPSceneNodeFactory(m_pSmgr);
    m_pSmgr->registerSceneNodeFactory(l_pNodeFactory);
    l_pNodeFactory->drop();

    m_pSoundInterface = sound::createSoundInterface(m_pDevice);

    m_pSoundInterface->setMasterVolume(0.0f);

    m_pSoundInterface->preloadSound(L"data/sounds/theme_menu.ogg"   , true );
    m_pSoundInterface->preloadSound(L"data/sounds/theme_race.ogg"   , false);
    m_pSoundInterface->preloadSound(L"data/sounds/theme_result.ogg" , false);
    m_pSoundInterface->preloadSound(L"data/sounds/button_hover.ogg" , true );
    m_pSoundInterface->preloadSound(L"data/sounds/button_press.ogg" , true );
    m_pSoundInterface->preloadSound(L"data/sounds/countdown.ogg"    , false);
    m_pSoundInterface->preloadSound(L"data/sounds/countdown_go.ogg" , false);
    m_pSoundInterface->preloadSound(L"data/sounds/checkpoint.ogg"   , false);
    m_pSoundInterface->preloadSound(L"data/sounds/gameover.ogg"     , false);
    m_pSoundInterface->preloadSound(L"data/sounds/lap.ogg"          , false);
    m_pSoundInterface->preloadSound(L"data/sounds/respawn_start.ogg", false);
    m_pSoundInterface->preloadSound(L"data/sounds/respawn.ogg"      , false);
    m_pSoundInterface->preloadSound(L"data/sounds/stunned.ogg"      , false);

    m_pSoundInterface->assignSoundtracks(
      {
        { enSoundTrack::enStMenu  , L"data/sounds/theme_menu.ogg"   },
        { enSoundTrack::enStRace  , L"data/sounds/theme_race.ogg"   },
        { enSoundTrack::enStFinish, L"data/sounds/theme_result.ogg" }
      }
    );

    m_pSoundInterface->setSoundtrackVolume(m_cSettings.m_fSoundTrack);
    m_pSoundInterface->setSfxVolumeMenu   (m_cSettings.m_fSfxMenu   );
    m_pSoundInterface->setSfxVolumeGame   (m_cSettings.m_fSfxGame   );
    m_pSoundInterface->setMasterVolume    (m_cSettings.m_fSfxMaster );

    m_mStates[state::enState::Menu] = new state::CMenuState(m_pDevice, this);
    m_mStates[state::enState::Game] = new state::CGameState(m_pDevice, this);
    m_pActiveState = m_mStates[state::enState::Menu];

    m_pActiveState->activate();
  }

  /**
  * Set the custom event receivers
  * @param a_pMenu the custom event receiver for the menu
  * @param a_pGame the custom event receiver for the game
  */
  void CMainClass::setCustomEventReceivers(controller::ICustomEventReceiver* a_pMenu, controller::ICustomEventReceiver* a_pGame) {
    m_pCtrlMenu = a_pMenu;
    m_pCtrlGame = a_pGame;

    if (m_mStates.find(state::enState::Menu) != m_mStates.end())
      reinterpret_cast<state::CMenuState *>(m_mStates[state::enState::Menu])->setTouchController(a_pMenu);
  }

  /**
  * Get the settings string that will be stored by the main
  * function wherever appropriate for the current system
  * @return a string with the settings
  */
  std::string CMainClass::getSettings() {
    m_cSettings.saveSettings(m_mSettings);

    std::string s = "<?xml version=\"1.0\"?>\n";

    s += "<marblegp>\n";

    for (std::map<std::string, std::string>::iterator it = m_mSettings.begin(); it != m_mSettings.end(); it++) {
      s += "  <setting name=\"" + it->first + "\" value=\"" + it->second + "\" />\n";
    }

    s += "</marblegp>\n";

    return s;
  }

  irr::IrrlichtDevice* CMainClass::getIrrlichtDevice() {
    return m_pDevice;
  }

  irr::gui::IGUIEnvironment* CMainClass::getGuiEnvironment() {
    return m_pGui;
  }

  irr::video::IVideoDriver* CMainClass::getVideoDriver() {
    return m_pDrv;
  }

  irr::scene::ISceneManager* CMainClass::getSceneManager() {
    return m_pSmgr;
  }

  irr::io::IFileSystem* CMainClass::getFileSystem() {
    return m_pFs;
  }

  /**
  * Set a setting (settings are stored when the program quits and restored on startup)
  * @param a_sKey the key of the setting
  * @param a_sValue the value of the setting
  */
  void CMainClass::setSetting(const std::string& a_sKey, const std::string& a_sValue) {
    m_mSettings[a_sKey] = a_sValue;
  }

  /**
  * Get a setting (settings are stored when the program quits and restored on startup)
  * @param a_sKey the key of the setting
  * @return a_sValue the value of the setting
  */
  std::string CMainClass::getSetting(const std::string& a_sKey) {
    if (m_mSettings.find(a_sKey) != m_mSettings.end())
      return m_mSettings[a_sKey];
    else
      return "";
  }

  /**
  * Set a global (globals are not stored when the program quits and restored on startup)
  * @param a_sKey the key of the global
  * @param a_sValue the value of the global
  */
  void CMainClass::setGlobal(const std::string& a_sKey, const std::string& a_sValue) {
    m_mGlobals[a_sKey] = a_sValue;
  }

  /**
  * Get a global (globals are not stored when the program quits and restored on startup)
  * @param a_sKey the key of the global
  * @return a_sValue the value of the global
  */
  std::string CMainClass::getGlobal(const std::string& a_sKey) {
    if (m_mGlobals.find(a_sKey) != m_mGlobals.end())
      return m_mGlobals[a_sKey];
    else
      return "";
  }

  /**
  * Get a font for a specific viewport size
  * @param a_eFont the type of font
  * @param a_cViewport the viewport size
  * @see dustbin::enFont
  */
  irr::gui::IGUIFont* CMainClass::getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport) {
    // Tiny: 0.5 * RasterSize, Small: 0.75 * RasterSize, Regular: RasterSize, Big: 1.5 * RasterSize, Huge: 2 * RasterSize
    int l_iSize = getRasterSize(&a_cViewport);

    switch (a_eFont) {
      case enFont::Tiny   : l_iSize =     l_iSize / 2; break;
      case enFont::Small  : l_iSize = 3 * l_iSize / 4; break;
      case enFont::Regular:                            break;
      case enFont::Big    : l_iSize = 3 * l_iSize / 2; break;
      case enFont::Huge   : l_iSize = 2 * l_iSize    ; break;
    }

    int l_iFontFile = 0;

    for (std::vector<int>::iterator it = m_vFontSizes.begin(); it != m_vFontSizes.end(); it++) {
      if (*it >= l_iSize)
        break;

      l_iFontFile = *it;
    }

    if (m_mFonts.find(l_iFontFile) == m_mFonts.end()) {
      printf("Loading font with size %i\n", l_iSize);
      std::string l_sFile = "data/fonts/font_" + std::to_string(l_iFontFile) + ".xml";
      m_mFonts[l_iFontFile] = m_pGui->getFont(l_sFile.c_str());
    }

    return m_mFonts[l_iFontFile];
  }

  /**
  * Get the raster size for the UI layout
  * @return the raster size
  */
  int CMainClass::getRasterSize(const irr::core::dimension2du *l_pViewport) {
    if (m_iRasterSize == -1 || l_pViewport != nullptr) {
      int l_iColumns = 80,
          l_iRows    = 55;

      if (m_pFs->existFile("data/grid_layout.xml")) {
        irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8("data/grid_layout.xml");
        if (l_pXml) {
          while (l_pXml->read()) {
            std::string l_sNode = l_pXml->getNodeName();
            if (l_sNode == "grid" && l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
              std::string l_sColumns = l_pXml->getAttributeValueSafe("columns"),
                          l_sRows    = l_pXml->getAttributeValueSafe("rows"   );

              if (l_sColumns != "")
                l_iColumns = std::atoi(l_sColumns.c_str());

              if (l_sRows != "")
                l_iRows = std::atoi(l_sRows.c_str());
            }
          }
          l_pXml->drop();
        }
      }

      irr::core::dimension2du l_cDim = l_pViewport == nullptr ? m_pDrv->getScreenSize() : *l_pViewport;

      int l_iWidth  = l_cDim.Width  / l_iColumns,
          l_iHeight = l_cDim.Height / l_iRows;

      if (l_pViewport != nullptr)
        return l_iWidth < l_iHeight ? l_iWidth : l_iHeight;

      m_iRasterSize = l_iWidth < l_iHeight ? l_iWidth : l_iHeight;
    }

    return m_iRasterSize;
  }

  /**
  * Get an image from a string. The following prefixes are possible:
  * - file://: load a file from a subfolder
  * - generate://: generate a marble texture
  * @param a_sInput the URI of the file
  * @return an Irrlicht texture object with the requested image or nullptr
  */
  irr::video::ITexture* CMainClass::createTexture(const std::string& a_sUri) {
    return helpers::createTexture(a_sUri, m_pDrv, m_pFs);
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

      case enLayout::FillWindow: {
        return irr::core::recti(0, 0, l_cSize.Width, l_cSize.Height);
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
  * Get the game settings
  * @return the game settings
  */
  data::SSettings& CMainClass::getSettingData() {
    return m_cSettings;
  }

  /**
  * Handle an Irrlicht event
  * @param a_cEvent the event to handle
  * @return true if the event was handled
  */
  bool CMainClass::OnEvent(const irr::SEvent& a_cEvent) {
    bool l_bRet = false;

    // if (m_pCtrlMenu != nullptr)
    //   l_bRet = m_pCtrlMenu->handleEvent(a_cEvent);

    // if (!l_bRet && m_pCtrlGame != nullptr)
    //  l_bRet = m_pCtrlGame->handleEvent(a_cEvent);

    if (!l_bRet && m_pActiveState != nullptr)
      l_bRet = m_pActiveState->OnEvent(a_cEvent);

    return l_bRet;
  }

  /**
  * Get the running network server instance
  * @return the running network server instance or nullptr if none is running
  */
  network::CGameServer *CMainClass::getGameServer() {
    return m_pServer;
  }

  /**
  * Start a game server. Calling this function while a server is running will stop and destroy it
  * @param a_vAvailableSlots a list of the available player IDs for the game  
  */
  void CMainClass::startGameServer(const std::vector<int> &a_vAvailableSlots) {
    if (m_pServer != nullptr) {
      m_pServer->stopThread();
      m_pServer->join();
      delete m_pServer;
    }

    m_pServer = new network::CGameServer(a_vAvailableSlots, this);
    m_pServer->startThread();
  }

  /**
  * Stop a game server. Nothing happens if no server is running
  */
  void CMainClass::stopGameServer() {
    if (m_pServer != nullptr) {
      m_pServer->stopThread();
      m_pServer->join();
      delete m_pServer;
      m_pServer = nullptr;
    }
  }

  /**
  * Get the running network client
  * @return the running network client, nullptr if no client is running
  */
  network::CGameClient *CMainClass::getGameClient() {
    return m_pClient;
  }

  /**
  * Start and connect a net client
  * @param a_sHost the server to connect to
  * @param a_iPort the port the server is running
  */
  void CMainClass::startGameClient(const std::string& a_sHost, int a_iPort, threads::CInputQueue *a_pQueue) {
    if (m_pClient != nullptr) {
      m_pClient->stopThread();
      m_pClient->join();
      delete m_pClient;
    }

    m_pClient = new network::CGameClient(a_sHost, a_iPort, this);
    m_pClient->getOutputQueue()->addListener(a_pQueue);
    m_pClient->startThread();
  }

  /**
  * Stop a game client. Nothing happens if no client is running
  */
  void CMainClass::stopGameClient() {
    if (m_pClient != nullptr) {
      m_pClient->stopThread();
      m_pClient->join();
      delete m_pClient;
      m_pClient = nullptr;
    }
  }

  /**
  * Get the currently active state
  * @return the currently active state
  */
  state::IState *CMainClass::getActiveState() {
    return m_pActiveState;
  }

  /**
  * Get the a state by it's id
  * @param a_eState the id of the requested state
  * @return the state with the id or nullptr if no state with the id exists
  */
  state::IState *CMainClass::getState(state::enState a_eState) {
    if (m_mStates.find(a_eState) != m_mStates.end())
      return m_mStates[a_eState];
    else
      return nullptr;
  }

  /**
  * Get the name of a track
  * @param a_sTrack the track identifier
  * @return the track name, "Unknown Track" if no track data was found
  */
  std::string CMainClass::getTrackName(const std::string& a_sTrack) {
    std::string l_sFile = "data/levels/" + a_sTrack + "/track.xml";

    if (m_pFs->existFile(l_sFile.c_str())) {
      std::string l_sXml = "data/levels/" + a_sTrack + "/info.xml", l_sName = a_sTrack;
      int l_iPos = 9999;

      if (m_pFs->existFile(l_sXml.c_str())) {
        irr::io::IReadFile *l_pFile = m_pFs->createAndOpenFile(l_sXml.c_str());

        if (l_pFile) {
          irr::io::IXMLReaderUTF8 *l_pXml = m_pFs->createXMLReaderUTF8(l_pFile);
          if (l_pXml) {
            bool l_bName = false;

            while (l_pXml->read()) {
              std::string l_sNode = l_pXml->getNodeName();

              if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
                if (l_sNode == "name")
                  l_bName = true;
              }
              else if (l_pXml->getNodeType() == irr::io::EXN_TEXT) {
                if (l_bName)
                  return l_pXml->getNodeData();
              }
              else if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT_END) {
                if (l_sNode == "name")
                  l_bName = false;
              }
            }
            l_pXml->drop();
          }
          l_pFile->drop();
        }
      }
    }

    return "Unknown Track";
  }

  /**
  * Init the next game screen. Must be called when the race data is defined
  */
  void CMainClass::initNextRaceScreen() {
    irr::gui::IGUIFont *l_pFont = getFont(enFont::Big, m_pDrv->getScreenSize());
    irr::core::dimension2du l_cDim = l_pFont->getDimension(L"ThisTextShouldBeLongEnoughForThisScreenAtLeastIHopeSo");
    irr::core::dimension2du l_cImg = irr::core::dimension2du(l_cDim.Width, 4 * l_cDim.Height);

    if (m_pNextRaceScreen == nullptr) {
      m_pNextRaceScreen = m_pDrv->addRenderTargetTexture(irr::core::dimension2du(l_cDim.Width, 4 * l_cDim.Height), "__NextRaceScreen");
    }

    m_pDrv->setRenderTarget(m_pNextRaceScreen, true, true, irr::video::SColor(128, 192, 192, 192));

    data::SGameData l_cData = data::SGameData(getGlobal("gamedata"));

    std::string l_sThumbnail = "data/levels/" + l_cData.m_sTrack + "/thumbnail.png";

    irr::core::position2di l_cPos = irr::core::position2di(0, 0);

    if (m_pFs->existFile(l_sThumbnail.c_str())) {
      irr::video::ITexture *l_pThumbnail = m_pDrv->getTexture(l_sThumbnail.c_str());
      if (l_pThumbnail != nullptr) {
        m_pDrv->draw2DImage(l_pThumbnail, irr::core::recti(0, 0, l_cImg.Height, l_cImg.Height), irr::core::recti(irr::core::position2di(0, 0), l_pThumbnail->getOriginalSize()));
        l_cPos.X += 5 * l_cImg.Height / 4;
        l_cDim.Width -= l_cPos.X;
      }
    }

    l_cPos.Y += l_cDim.Height / 2;
    irr::core::recti l_cRect = irr::core::recti(l_cPos, l_cDim);

    l_pFont->draw(helpers::s2ws(getTrackName(l_cData.m_sTrack)).c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), false, false);

    l_cRect.UpperLeftCorner .Y += 2 * l_cDim.Height;
    l_cRect.LowerRightCorner.Y += 2 * l_cDim.Height;

    l_pFont->draw((std::to_wstring(l_cData.m_iLaps) + L" Lap" + (l_cData.m_iLaps == 1 ? L"" : L"s")).c_str(), l_cRect, irr::video::SColor(0xFF, 0, 0, 0), false, false);

    m_pDrv->setRenderTarget(0, false, false);

    if (m_pLogo == nullptr) {
      m_pLogo = m_pDrv->getTexture("data/images/logo.png");
      m_cLogo = irr::core::dimension2du(m_pDrv->getScreenSize().Height / 5, m_pDrv->getScreenSize().Height / 5);
    }
  }

  /**
  * Draw the next race screen
  * @param a_fAlpha the transparency of the next race screen [0..1]
  */
  void CMainClass::drawNextRaceScreen(irr::f32 a_fAlpha) {
    m_pDrv->draw2DRectangle(irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 0, 0, 0), irr::core::recti(irr::core::position2di(0, 0), m_pDrv->getScreenSize()));

    if (m_pNextRaceScreen != nullptr) {
      irr::core::vector2di l_cPos = irr::core::vector2di(m_pDrv->getScreenSize().Width / 2 - m_pNextRaceScreen->getOriginalSize().Width / 2, m_pDrv->getScreenSize().Height / 3 - m_pNextRaceScreen->getOriginalSize().Height / 2);
      irr::video::SColor l_cColor = irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 255, 255, 255);
      m_pDrv->draw2DImage(m_pNextRaceScreen, l_cPos, irr::core::recti(irr::core::position2di(0, 0), m_pNextRaceScreen->getOriginalSize()), (const irr::core::recti *)nullptr, l_cColor, true);
    }

    if (m_pLogo != nullptr) {
      irr::core::vector2di l_cPos = irr::core::vector2di(m_pDrv->getScreenSize().Width / 2 - m_cLogo.Width / 2, (2 * m_pDrv->getScreenSize().Height / 3) - m_cLogo.Height / 2);
      irr::video::SColor l_aColor[] = {
        irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 255, 255, 255),
        irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 255, 255, 255),
        irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 255, 255, 255),
        irr::video::SColor((irr::u32)(255.0f * a_fAlpha), 255, 255, 255)
      };

      m_pDrv->draw2DImage(m_pLogo, irr::core::recti(l_cPos, m_cLogo), irr::core::recti(irr::core::vector2di(0, 0), m_pLogo->getOriginalSize()), nullptr, l_aColor, true);
    }
  }

  /**
  * Get the sound interface
  * @return the sound interface
  */
  sound::ISoundInterface *CMainClass::getSoundInterface() {
    return m_pSoundInterface;
  }

#ifdef _ANDROID
  /**
  * Get a pointer to the Android app
  * @return the Android app
  */
  android_app *CMainClass::getAndroidApp() {
    return m_pAndroidApp;
  }
#endif

  /**
  * This method is called to run the game. If a graphics change
  * is requested ("true" returned) it's necessary to create a new
  * Irrlicht device and pass it to the class instance
  * @return "true" if gfx changes are needed, "false" if the application was stopped
  * @see CMainClass::setIrrlichtDevice
  */
  state::enState CMainClass::run() {
    state::enState l_eState = state::enState::None;

    if (m_pActiveState != nullptr) {
      l_eState = m_pActiveState->run();

      std::wstring l_sCaption = std::wstring(L"Dustbin::Games - MarbleGP [") + std::to_wstring(m_pDrv->getFPS()) + L" Fps]";
      m_pDevice->setWindowCaption(l_sCaption.c_str());

      if (l_eState != m_pActiveState->getId() && m_mStates.find(l_eState) != m_mStates.end()) {
        m_pActiveState->deactivate();
        m_pActiveState = m_mStates[l_eState];
        m_pActiveState->activate();
      }
    }

    return l_eState;
  }
}

