// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/ICustomEventReceiver.h>
#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <CGlobal.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
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

      std::map<std::string, std::string> m_mSettings,   /**< All the settings of the program */
                                         m_mGlobals;    /**< The global variables */

      int m_iRasterSize;    /**< The raster size for UI layout */

      std::vector<int> m_vFontSizes;  /**< The supported font sizes */

      std::map<int, irr::gui::IGUIFont*> m_mFonts;  /**< A map for the fonts */

      data::SSettings m_cSettings;  /**< The game settings */

      controller::ICustomEventReceiver *m_pCtrlMenu,    /**< The custom event receiver for the menu */
                                       *m_pCtrlGame;    /**< The custom event receiver for the game */

      state::IState *m_pActiveState;   /**< The active state */

      sound::ISoundInterface *m_pSoundInterface;  /**< The audio interface */

      std::map<state::enState, state::IState *> m_mStates;

      network::CGameServer *m_pServer;    /**< The game server */
      network::CGameClient *m_pClient;    /**< The net game client */

      irr::video::ITexture    *m_pNextRaceScreen;     /**< The render target texture used for the "next race" screen */
      irr::video::ITexture    *m_pLogo;               /**< The "Dustbin::Game MarbleGP" Logo */
      irr::core::dimension2du  m_cLogo;               /**< The size of the rendered logo in the "next race" screen */

      std::string m_sDeviceName;    /**< The device name, used for the Android port */

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
      std::string getSettings();

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
      virtual state::IState *getActiveState();

      /**
      * Get the a state by it's id
      * @param a_eState the id of the requested state
      * @return the state with the id or nullptr if no state with the id exists
      */
      virtual state::IState *getState(state::enState a_eState);

      /**
      * Get the name of a track
      * @param a_sTrack the track identifier
      * @return the track name, "Unknown Track" if no track data was found
      */
      virtual std::string getTrackName(const std::string &a_sTrack);

      /**
      * Init the next game screen. Must be called when the race data is defined
      */
      virtual void initNextRaceScreen();

      /**
      * Draw the next race screen
      * @param a_fAlpha the transparency of the next race screen [0..1]
      */
      virtual void drawNextRaceScreen(irr::f32 a_fAlpha);

      /**
      * Get the device name
      * @return the device name
      */
      virtual const std::string &getDeviceName();

#ifdef _ANDROID
      /**
      * Get a pointer to the Android app
      * @return the Android app
      */
      virtual android_app *getAndroidApp() override;
#endif
  };
}


