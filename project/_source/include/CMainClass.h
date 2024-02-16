// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/ICustomEventReceiver.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <string>
#include <tuple>
#include <map>

namespace dustbin {
  namespace gui {
    class CVirtualKeyboard;
  }

  namespace shaders {
    class CDustbinShaders;
  }

  /**
  * @class CMainClass
  * @author Christian Keimel
  * This is the main class where all the magic happens
  */
  class CMainClass : public CGlobal {
    private:
      irr::IrrlichtDevice       *m_pDevice;     /**< The Irrlicht device */
      irr::scene::ISceneManager *m_pSmgr;       /**< The main scene manager */
      irr::video::IVideoDriver  *m_pDrv;        /**< The video driver in use */
      irr::gui::IGUIEnvironment *m_pGui;        /**< The GUI environment */
      irr::io::IFileSystem      *m_pFs;         /**< The Irrlicht file system */

      bool m_bFocused;        /**< Is the window focused? */
      bool m_bFirstStart;     /**< Is this the first start of the game? */

      std::map<std::string, std::string> m_mSettings;   /**< All the settings of the program */
      std::map<std::string, std::string> m_mGlobals;    /**< The global variables */

      int m_iRasterSize;    /**< The raster size for UI layout */

      irr::u32 m_iRenderFlags;    /**< Flags for shadow rendering */

      std::vector<int> m_vFontSizes;  /**< The supported font sizes */

      std::map<int, irr::gui::IGUIFont*> m_mFonts;  /**< A map for the fonts */

      data::SSettings m_cSettings;  /**< The game settings */

      controller::ICustomEventReceiver *m_pCtrlMenu;    /**< The custom event receiver for the menu */
      controller::ICustomEventReceiver *m_pCtrlGame;    /**< The custom event receiver for the game */

      shaders::CDustbinShaders *m_pShader;    /**< The shaders */

      state::IState *m_pActiveState;   /**< The active state */

      sound::ISoundInterface *m_pSoundInterface;  /**< The audio interface */

      std::map<state::enState, state::IState *> m_mStates;

      network::CGameServer *m_pServer;    /**< The game server */
      network::CGameClient *m_pClient;    /**< The net game client */

      irr::video::ITexture    *m_pNextRaceScreen;     /**< The render target texture used for the "next race" screen */
      irr::video::ITexture    *m_pLogo;               /**< The "Dustbin::Game MarbleGP" Logo */
      irr::core::dimension2du  m_cLogo;               /**< The size of the rendered logo in the "next race" screen */

      std::string m_sDeviceName;    /**< The device name, used for the Android port */

      gui::CVirtualKeyboard *m_pKeyBoard;

      std::vector<std::tuple<irr::ELOG_LEVEL, std::string>> m_vLogMessages;   /**< The Irrlicht log messages */

      std::map<int, int> m_mBrake;    /**< The brake efficiency map */

      irr::core::array<irr::SJoystickInfo> m_aJoysticks;    /**< The Joysticks */

      gameclasses::STournament *m_pTournament;    /**< The current tournament */

#ifdef _ANDROID
      android_app *m_pAndroidApp;  /**< The Android App */
#endif

    public:
      /**
      * The constructor
      * @param a_sSettings The settings XML as a string
      */
      CMainClass(const std::string &a_sSettings
#ifdef _ANDROID
        , android_app *a_pApp
#endif
      );
      virtual ~CMainClass();

      /**
      * Set the Irrlicht device
      * @param a_pDevice the Irrlicht device to use
      */
      void setIrrlichtDevice(irr::IrrlichtDevice *a_pDevice);

      /**
      * Set the custom event receivers
      * @param a_pMenu the custom event receiver for the menu
      * @param a_pGame the custom event receiver for the game
      */
      void setCustomEventReceivers(controller::ICustomEventReceiver *a_pMenu, controller::ICustomEventReceiver *a_pGame);

      /**
      * This method is called for every frame.
      * @return the state
      * @see CMainClass::enMainState
      * @see CMainClass::setIrrlichtDevice
      */
      state::enState run();

      /**
      * Get the settings string that will be stored by the main
      * function wherever appropriate for the current system
      * @return a string with the settings
      */
      virtual std::string getSettings() override;

      virtual irr::IrrlichtDevice       *getIrrlichtDevice() override;   /**< Get the Irrlicht device */
      virtual irr::gui::IGUIEnvironment *getGuiEnvironment() override;   /**< Get the Irrlicht GUI environment */
      virtual irr::video::IVideoDriver  *getVideoDriver   () override;   /**< Get the used video driver */
      virtual irr::scene::ISceneManager *getSceneManager  () override;   /**< Get the main scene manager */
      virtual irr::io::IFileSystem      *getFileSystem    () override;   /**< Get the Irrlicht file system instance */

      /**
      * Set the device name, important for the Android port
      * @param a_sName the name of the device
      */
      void setDeviceName(const std::string &a_sName);

      /**
      * Set a setting (settings are stored when the program quits and restored on startup)
      * @param a_sKey the key of the setting
      * @param a_sValue the value of the setting
      */
      virtual void setSetting(const std::string &a_sKey, const std::string &a_sValue) override;

      /**
      * Get a setting (settings are stored when the program quits and restored on startup)
      * @param a_sKey the key of the setting
      * @return a_sValue the value of the setting
      */
      virtual std::string getSetting(const std::string &a_sKey) override;

      /**
      * Set a global (globals are not stored when the program quits and restored on startup)
      * @param a_sKey the key of the global
      * @param a_sValue the value of the global
      */
      virtual void setGlobal(const std::string &a_sKey, const std::string &a_sValue) override;

      /**
      * Get a global (globals are not stored when the program quits and restored on startup)
      * @param a_sKey the key of the global
      * @return a_sValue the value of the global
      */
      virtual std::string getGlobal(const std::string &a_sKey) override;

      /**
      * Get a font for a specific viewport size
      * @param a_eFont the type of font
      * @param a_cViewport the viewport size
      * @see dustbin::enFont
      */
      virtual irr::gui::IGUIFont* getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport) override;

      /**
      * Get a font with a specific size. If no font with the size was
      * found the biggest font with smaller size will be returned
      * @param a_iSize the pixel size of the font
      * @return the font
      */
      virtual irr::gui::IGUIFont *getFontBySize(irr::u32 a_iSize) override;

      /**
      * Get the raster size for the UI layout
      * @return the raster size
      */
      virtual int getRasterSize(const irr::core::dimension2du *l_pViewport = 0) override;

      /**
      * Get an image from a string. The following prefixes are possible:
      * - file://: load a file from a subfolder
      * - generate://: generate a marble texture
      * @param a_sInput the URI of the file
      * @return an Irrlicht texture object with the requested image or nullptr
      */
      virtual irr::video::ITexture* createTexture(const std::string& a_sUri) override;

      /**
      * Get a rectangle on the screen for GUI layout
      * @param a_iLeft Left position
      * @param a_iTop Top position
      * @param a_iRight Right position
      * @param a_iBottom bottom position
      * @param a_ePosition the layout position of the rectangle
      * @param a_pParent an optional parent element
      */
      virtual irr::core::recti getRect(int a_iLeft, int a_iTop, int a_iRight, int a_iBottom, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr) override;

      /**
      * Get a rectangle from the mouse position
      * @param a_cPosition the mouse position
      * @returns the rectangle on the grid hovered by the mouse
      */
      virtual const irr::core::recti getRectFromMouse(const irr::core::position2di &a_cPosition) override;

      /**
      * Get a rectangle on the screen for GUI layout
      * @param a_cRect Irrlicht rectangle with the coordinates to be converted
      * @param a_ePosition the layout position of the rectangle
      * @param a_pParent an optional paraent element
      */
      virtual irr::core::recti getRect(const irr::core::recti& a_cRect, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr) override;

      /**
      * Get the game settings
      * @return the game settings
      */
      virtual data::SSettings &getSettingData() override;

      /**
      * Get the sound interface
      * @return the sound interface
      */
      virtual sound::ISoundInterface *getSoundInterface() override;

      /**
      * Handle an Irrlicht event
      * @param a_cEvent the event to handle
      * @return true if the event was handled
      */
      virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

      /**
      * Get the running network server instance
      * @return the running network server instance or nullptr if none is running
      */
      virtual network::CGameServer *getGameServer() override;

      /**
      * Start a game server
      * @param a_vAvailableSlots a list of the available player IDs for the game
      */
      virtual void startGameServer(const std::vector<int> &a_vAvailableSlots) override;

      /**
      * Stop a game server. Nothing happens if no server is running
      */
      virtual void stopGameServer() override;

      /**
      * Get the running network client
      * @return the running network client, nullptr if no client is running
      */
      virtual network::CGameClient *getGameClient() override;

      /**
      * Start and connect a net client
      * @param a_iHostIP the server to connect to
      * @param a_iPort the port the server is running
      * @param a_pQueue the queue that will receive the output from this client
      */
      virtual void startGameClient(irr::u32 a_iHostIP, int a_iPort, threads::CInputQueue *a_pQueue) override;

      /**
      * Stop a game client. Nothing happens if no client is running
      */
      virtual void stopGameClient() override;

      /**
      * Get the currently active state
      * @return the currently active state
      */
      virtual state::IState *getActiveState() override;

      /**
      * Get the a state by it's id
      * @param a_eState the id of the requested state
      * @return the state with the id or nullptr if no state with the id exists
      */
      virtual state::IState *getState(state::enState a_eState) override;

      /**
      * Get the name of a track
      * @param a_sTrack the track identifier
      * @return the track name, "Unknown Track" if no track data was found
      */
      virtual std::string getTrackName(const std::string &a_sTrack) override;

      /**
      * Init the next game screen. Must be called when the race data is defined
      */
      virtual void initNextRaceScreen() override;

      /**
      * Draw the next race screen
      * @param a_fAlpha the transparency of the next race screen [0..1]
      */
      virtual void drawNextRaceScreen(irr::f32 a_fAlpha) override;

      /**
      * Get the device name
      * @return the device name
      */
      virtual const std::string &getDeviceName() override;

      /**
      * Clear the GUI environment
      */
      virtual void clearGui() override;

      /**
      * Callback before the UI is cleared
      */
      virtual void beforeMenuLoad() override;

      /**
      * Callback when a new menu has been loaded
      */
      virtual void menuLoaded() override;

      /**
      * Callback when the virtual keyboard is destroyed
      */
      virtual void virtualKeyboardDestroyed() override;

      /**
      * Get the recorded log messages
      * @return the recorded log messages
      */
      virtual const std::vector<std::tuple<irr::ELOG_LEVEL, std::string>> &getLogMessages() override;

      /**
      * Get the brake map (key = velocity, value = number of steps to reach). For usage:
      * find the correct entry in the map for the current and the desired velocity and
      * use the difference of the values to know the number of steps to reach the speed
      * @return map of the measured braking efficiency
      */
      virtual const std::map<int, int> &getBrakeEfficiency() override;

      /**
      * Get the Irrlicht joystick information
      * @return the Irrlicht joystick information
      */
      virtual const irr::core::array<irr::SJoystickInfo> &getJoystickInfo() const override;

      /**
      * Get the shader instance
      */
      virtual shaders::CDustbinShaders *getShader() const override;

      /**
      * Get the render flags for the shadow rendering
      * @return the render flags for the shadow rendering
      */
      virtual irr::u32 getRenderFlags() override;

      /**
      * Start a new tournament
      * @return the new tournament
      */
      virtual gameclasses::STournament *startTournament() override;

      /**
      * Get the current tournament
      * @return the current tournament
      */
      virtual gameclasses::STournament *getTournament() override;

#ifdef _ANDROID
      /**
      * Get a pointer to the Android app
      * @return the Android app
      */
      virtual android_app *getAndroidApp() override;
#endif
  };
}


