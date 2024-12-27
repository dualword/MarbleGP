// (w) 2024 by Dustbin::Games / Christian Keimel
#include <CTrackEd_MainClass.h>

#include <scenenodes/CMarbleGPSceneNodeFactory.h>
#include <animators/CAnimatorFactory.h>
#include <helpers/CStringHelpers.h>
#include <platform/CPlatform.h>
#include <font/font_manager.h>
#include <CDefines.h>

#include <commands/CCmdDustbinRenderer.h>
#include <commands/CCmdDefaultRenderer.h>
#include <commands/CCmdCameraCtrl.h>
#include <commands/CCmdEditTrack.h>
#include <commands/CCmdSaveTrack.h>
#include <commands/CCmdMainMenu.h>

#include <filesystem>

namespace dustbin {
  namespace tracked {
    CTrackEd_MainClass::CTrackEd_MainClass() :
      IGlobal(),
      m_pDevice (nullptr),
      m_pSmgr   (nullptr),
      m_pDrv    (nullptr),
      m_pGui    (nullptr),
      m_pFs     (nullptr),
      m_pFontMgr(nullptr),
      m_iWidth  (0),
      m_iHeight (0)
    {
      std::wstring l_sPath = dustbin::platform::portableGetDataPath();

      m_sPathMarbleGP = l_sPath + L"MarbleGP\\";
      m_sPathTrackEd  = l_sPath + L"TrackEd\\";

      if (!std::filesystem::exists(m_sPathMarbleGP)) std::filesystem::create_directories(m_sPathMarbleGP);
      if (!std::filesystem::exists(m_sPathTrackEd )) std::filesystem::create_directories(m_sPathTrackEd );

      irr::IrrlichtDevice* l_pNull = irr::createDevice(irr::video::EDT_NULL);
      irr::video::IVideoModeList* l_pList = l_pNull->getVideoModeList();

      m_iWidth  = 90 * l_pList->getDesktopResolution().Width  / 100;
      m_iHeight = 85 * l_pList->getDesktopResolution().Height / 100;

      m_pFs = l_pNull->getFileSystem();
      loadSettings();
      m_pFs = nullptr;

      l_pNull->drop();

      if (getSetting("width") != "" && getSetting("height") != "") {
        m_iWidth  = std::atoi(getSetting("width" ).c_str());
        m_iHeight = std::atoi(getSetting("height").c_str());
      }
      else {
        setSetting("width" , std::to_string(m_iWidth ));
        setSetting("height", std::to_string(m_iHeight));
      }

      m_pDevice = irr::createDevice(irr::video::EDT_OPENGL, irr::core::dimension2du(m_iWidth, m_iHeight), 32U, false, false, false);
      m_pDevice->setWindowCaption(L"Dustbin::Games MarbleGP Track Editor");
      m_pDevice->setEventReceiver(this);

      m_pSmgr = m_pDevice->getSceneManager  ();
      m_pDrv  = m_pDevice->getVideoDriver   ();
      m_pGui  = m_pDevice->getGUIEnvironment();
      m_pFs   = m_pDevice->getFileSystem    ();

      printf("Working Directory: \"%s\"\n", m_pFs->getWorkingDirectory().c_str());

      std::string l_sTrackEdPath = helpers::ws2s(platform::portableGetTrackEdPath());

      irr::io::IFileArchive *l_pArchive = nullptr;
      if (m_pFs->addFileArchive(l_sTrackEdPath.c_str(), true, false, irr::io::EFAT_FOLDER, "", &l_pArchive)) {
        printf("TrackEd path added.\n");
      }

      m_pFontMgr = new FontManager();

      unsigned l_iFontSize = 18;
      std::string l_sFontSize = getSetting("fontsize");
      if (l_sFontSize != "")
        l_iFontSize = std::atoi(l_sFontSize.c_str());

      irr::gui::IGUIFont *l_pFont = m_pFontMgr->GetTtFont(m_pDrv, m_pFs, "data/fonts/arialmt.ttf", l_iFontSize);
      m_pGui->getSkin()->setFont(l_pFont);

      scenenodes::CMarbleGPSceneNodeFactory* l_pFactory = new scenenodes::CMarbleGPSceneNodeFactory(m_pSmgr);
      m_pSmgr->registerSceneNodeFactory(l_pFactory);
      l_pFactory->drop();

      animators::CAnimatorFactory* l_pFactory2 = new animators::CAnimatorFactory(m_pDevice);
      m_pSmgr->registerSceneNodeAnimatorFactory(l_pFactory2);
      l_pFactory2->drop();

      commands::CCmdMainMenu  *l_pMain = new commands::CCmdMainMenu(this, m_pGui);
      commands::CCmdEditTrack *l_pEdit = new commands::CCmdEditTrack      (m_pSmgr, m_pDrv, m_pGui, l_pMain);

      l_pMain->setEditTrackCommand(l_pEdit);

      addCommand(new commands::CCmdDefaultRenderer(m_pDevice      ));
      addCommand(new commands::CCmdDustbinRenderer(m_pDevice      ));
      addCommand(new commands::CCmdCameraCtrl     (m_pSmgr, m_pGui));
      addCommand(l_pMain);
      addCommand(l_pEdit);

      CCommandHandler::run();

      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iStartupFinished;
      l_cEvent.UserEvent.UserData2 = c_iStartupFinished;
      m_pDevice->postEventFromUser(l_cEvent);

      m_cMaterial.Lighting      = false;
      m_cMaterial.DiffuseColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.EmissiveColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.AmbientColor  = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.SpecularColor = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.Wireframe     = true;
    }

    CTrackEd_MainClass::~CTrackEd_MainClass() {
      for (auto l_pCmd: m_vCommands) {
        delete l_pCmd;
      }

      m_vCommands.clear();

      setSetting("width" , std::to_string(m_pDrv->getScreenSize().Width ));
      setSetting("height", std::to_string(m_pDrv->getScreenSize().Height));

      saveSettings();

      m_pSmgr = nullptr;
      m_pDrv  = nullptr;
      m_pGui  = nullptr;
      m_pFs   = nullptr;

      m_pDevice->drop();
    }

    /**
    * Run the editor
    * @return false to stop the application
    */
    bool CTrackEd_MainClass::run() {
      bool l_bRet = true;

      if (m_pDevice->run()) {
        CCommandHandler::run();
      }
      else l_bRet = false;

      return l_bRet;
    }

    /**
    * Get a setting
    * @param a_sKey the setting key
    * @return the value for the key, empty string if key was not found
    */
    std::string CTrackEd_MainClass::getSetting(const std::string& a_sKey) {
      if (m_mSettings.find(a_sKey) != m_mSettings.end())
        return m_mSettings[a_sKey];
      else
        return "";
    }

    /**
    * Set a setting
    * @param a_sKey the key of the setting
    * @param a_sValue the value of the setting
    */
    void CTrackEd_MainClass::setSetting(const std::string& a_sKey, const std::string& a_sValue) {
      m_mSettings[a_sKey] = a_sValue;
    }

    /**
    * Get a global variable not stored when the program quits
    * @param a_sKey the name of the variable
    * @return the value of the variable, en empty string if the name was not found
    */
    std::string CTrackEd_MainClass::getGlobal(const std::string &a_sKey) {
      if (m_mGlobals.find(a_sKey) != m_mGlobals.end())
        return m_mGlobals[a_sKey];
      else
        return "";
    }

    /**
    * Set a global variable not stored when the program quits
    * @param a_sKey the name of the variable
    * @param a_sValue the value of the variable
    */
    void CTrackEd_MainClass::setGlobal(const std::string &a_sKey, const std::string &a_sValue) {
      m_mGlobals[a_sKey] = a_sValue;
    }

    /**
    * A callback called before "endscene" is called. In this
    * place the commands can draw something (if necessary)
    */
    void CTrackEd_MainClass::beforeEndScene() {
      m_pDrv->setTransform(irr::video::ETS_WORLD, irr::core::IdentityMatrix);
      m_pDrv->setMaterial(m_cMaterial);

      for (auto l_pCmd : m_vCommands) {
        l_pCmd->draw(m_pDrv);
      }
    }

    /**
    * Get the Irrlicht Device
    * @return the Irrlicht Device
    */
    irr::IrrlichtDevice *CTrackEd_MainClass::getDevice() {
      return m_pDevice;
    }

    /**
    * Get the scene manager
    * @return the scene manager
    */
    irr::scene::ISceneManager *CTrackEd_MainClass::getSceneManager() {
      return m_pSmgr;
    }

    /**
    * Get the video driver
    * @return the video driver
    */
    irr::video::IVideoDriver *CTrackEd_MainClass::getVideoDriver() {
      return m_pDrv;
    }

    /**
    * Get the GUI environment
    * @return the GUI environment
    */
    irr::gui::IGUIEnvironment *CTrackEd_MainClass::getGuiEnvironment() {
      return m_pGui;
    }

    /**
    * Get the file system
    * @return the file system
    */
    irr::io::IFileSystem *CTrackEd_MainClass::getFileSystem() {
      return m_pFs;
    }

    /**
    * Load the settings XML
    */
    void CTrackEd_MainClass::loadSettings() {
      std::string l_sFile = helpers::ws2s(m_sPathTrackEd) + "settings.xml";

      if (m_pFs->existFile(l_sFile.c_str())) {
        irr::io::IXMLReader *l_pXml = m_pFs->createXMLReader(l_sFile.c_str());

        if (l_pXml) {
          while (l_pXml->read()) {
            if (l_pXml->getNodeType() == irr::io::EXN_ELEMENT) {
              if (std::wstring(L"setting") == l_pXml->getNodeName() && l_pXml->getAttributeValue(L"key") != nullptr && l_pXml->getAttributeValue(L"value") != nullptr) {
                std::wstring l_sKey   = l_pXml->getAttributeValue(L"key");
                std::wstring l_sValue = l_pXml->getAttributeValue(L"value");

                m_mSettings[helpers::ws2s(l_sKey)] = helpers::ws2s(l_sValue);
              }
            }
          }

          l_pXml->drop();
        }
      }
    }

    /**
    * Save the settings XML
    */
    void CTrackEd_MainClass::saveSettings() {
      std::string l_sFile = helpers::ws2s(m_sPathTrackEd) + "settings.xml";

      irr::io::IXMLWriter *l_pXml = m_pFs->createXMLWriter(l_sFile.c_str());

      if (l_pXml) {
        l_pXml->writeXMLHeader();
        l_pXml->writeElement(L"TrackEd_Settings", false);
        l_pXml->writeLineBreak();

        for (auto l_tSetting : m_mSettings) {
          l_pXml->writeElement(L"setting", true, L"key", helpers::s2ws(l_tSetting.first).c_str(), L"value", helpers::s2ws(l_tSetting.second).c_str());
          l_pXml->writeLineBreak();
        }

        l_pXml->writeClosingTag(L"TrackEd_Settings");
        l_pXml->writeLineBreak();

        l_pXml->drop();
      }
    }

    /**
    * Load a scene. Use this method instead of directly calling
    * "SceneManager->loadscene" as an user event is fired
    * @param a_sFile the scene file
    */
    void CTrackEd_MainClass::loadScene(const std::string &a_sFile) {
      m_pSmgr->loadScene(a_sFile.c_str());

      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iSceneLoaded;
      l_cEvent.UserEvent.UserData2 = c_iSceneLoaded;
      m_pDevice->postEventFromUser(l_cEvent);
    }

    /**
    * Clear the scene. Use this method as an user event is fired
    */
    void CTrackEd_MainClass::clearScene() {
      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iSceneWillBeCleared;
      l_cEvent.UserEvent.UserData2 = c_iSceneWillBeCleared;
      m_pDevice->postEventFromUser(l_cEvent);

      m_pSmgr->clear();
    }
  }
}