// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <state/IState.h>
#include <irrlicht.h>
#include <string>
#include <map>

#ifdef _ANDROID
#include <android_native_app_glue.h>
#endif

namespace dustbin {
  namespace controller {
    class CControllerBase;  /**< Forward declaration of the joystick event handler */
  }

  namespace sound {
    class ISoundInterface;  /**< Forward declaration for the audio interface */
  }

  namespace network {
    class CGameServer;  /**< Forward declaration of the network game server */
    class CGameClient;  /**< Forward declaration of the network game client */
  }

  namespace threads {
    class CInputQueue;  /**< Forward declaration of the thread input queue */
  }

  namespace state {
    class IState;   /**< Forward declaration of the state interface */
  }

  namespace menu {
    class IMenuHandler; /**< Forward declaration of the menu handler interface */
  }

  // Enum for the font size. The actual size
  // depends on the screen resolution
  enum class enFont {
    Tiny,     /**< Very small (tiny) font */
    Small,    /**< Small font*/
    Regular,  /**< Regular font */
    Big,      /**< Big font */
    Huge      /**< Very big (huge) font */
  };

  enum class enLayout {
    UpperLeft,
    UpperMiddle,
    UpperRight,
    Left,
    Center,
    Right,
    LowerLeft,
    LowerMiddle,
    LowerRight,
    Relative,
    FillWindow
  };

  /**
  * @class CGlobal
  * @author Christian Keimel
  * This is the global base class that provides
  * access to some important data through it's singleton
  */
  class CGlobal : public irr::IEventReceiver {
    protected:
      static CGlobal *m_pInstance;

    public:
      CGlobal();
      virtual ~CGlobal();

      virtual irr::IrrlichtDevice       *getIrrlichtDevice() = 0;   /**< Get the Irrlicht device */
      virtual irr::gui::IGUIEnvironment *getGuiEnvironment() = 0;   /**< Get the Irrlicht GUI environment */
      virtual irr::video::IVideoDriver  *getVideoDriver   () = 0;   /**< Get the used video driver */
      virtual irr::scene::ISceneManager *getSceneManager  () = 0;   /**< Get the main scene manager */
      virtual irr::io::IFileSystem      *getFileSystem    () = 0;   /**< Get the Irrlicht file system instance */

      /**
      * Set a setting (settings are stored when the program quits and restored on startup)
      * @param a_sKey the key of the setting
      * @param a_sValue the value of the setting
      */
      virtual void setSetting(const std::string &a_sKey, const std::string &a_sValue) = 0;

      /**
      * Get a setting (settings are stored when the program quits and restored on startup)
      * @param a_sKey the key of the setting
      * @return a_sValue the value of the setting
      */
      virtual std::string getSetting(const std::string &a_sKey) = 0;

      /**
      * Set a global (globals are not stored when the program quits and restored on startup)
      * @param a_sKey the key of the global
      * @param a_sValue the value of the global
      */
      virtual void setGlobal(const std::string &a_sKey, const std::string &a_sValue) = 0;

      /**
      * Get a global (globals are not stored when the program quits and restored on startup)
      * @param a_sKey the key of the global
      * @return a_sValue the value of the global
      */
      virtual std::string getGlobal(const std::string &a_sKey) = 0;

      /**
      * Get a font for a specific viewport size
      * @param a_eFont the type of font
      * @param a_cViewport the viewport size
      * @see dustbin::enFont
      */
      virtual irr::gui::IGUIFont* getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport) = 0;

      /**
      * Get a font with a specific size. If no font with the size was
      * found the biggest font with smaller size will be returned
      * @param a_iSize the pixel size of the font
      * @return the font
      */
      virtual irr::gui::IGUIFont *getFontBySize(irr::u32 a_iSize) = 0;

      /**
      * Get the raster size for the UI layout
      * @return the raster size
      */
      virtual int getRasterSize(const irr::core::dimension2du *l_pViewport = nullptr) = 0;

      /**
      * Get an image from a string. The following prefixes are possible:
      * - file://: load a file from a subfolder
      * - generate://: generate a marble texture
      * @param a_sInput the URI of the file
      * @return an Irrlicht texture object with the requested image or nullptr
      */
      virtual irr::video::ITexture* createTexture(const std::string& a_sUri) = 0;

      /**
      * Handle an Irrlicht event
      * @param a_cEvent the event to handle
      * @return true if the event was handled
      */
      virtual bool OnEvent(const irr::SEvent &a_cEvent) = 0;

      /**
      * Get a rectangle on the screen for GUI layout
      * @param a_iLeft Left position
      * @param a_iTop Top position
      * @param a_iRight Right position
      * @param a_iBottom bottom position
      * @param a_ePosition the layout position of the rectangle
      * @param a_pParent an optional parent element
      */
      virtual irr::core::recti getRect(int a_iLeft, int a_iTop, int a_iRight, int a_iBottom, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr) = 0;

      /**
      * Get a rectangle from the mouse position
      * @param a_cPosition the mouse position
      * @returns the rectangle on the grid hovered by the mouse
      */
      virtual const irr::core::recti getRectFromMouse(const irr::core::position2di &a_cPosition) = 0;

      /**
      * Get a rectangle on the screen for GUI layout
      * @param a_cRect Irrlicht rectangle with the coordinates to be converted
      * @param a_ePosition the layout position of the rectangle
      * @param a_pParent an optional paraent element
      */
      virtual irr::core::recti getRect(const irr::core::recti& a_cRect, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr) = 0;

      /**
      * Get the game settings
      * @return the game settings
      */
      virtual data::SSettings &getSettingData() = 0;

      /**
      * Get the sound interface
      * @return the sound interface
      */
      virtual sound::ISoundInterface *getSoundInterface() = 0;

      /**
      * Get the running network server instance
      * @return the running network server instance or nullptr if none is running
      */
      virtual network::CGameServer *getGameServer() = 0;

      /**
      * Start a game server
      * @param a_vAvailableSlots a list of the available player IDs for the game
      */
      virtual void startGameServer(const std::vector<int> &a_vAvailableSlots) = 0;

      /**
      * Stop a game server. Nothing happens if no server is running
      */
      virtual void stopGameServer() = 0;

      /**
      * Get the running network client
      * @return the running network client, nullptr if no client is running
      */
      virtual network::CGameClient *getGameClient() = 0;

      /**
      * Start and connect a net client
      * @param a_iHostIP the server to connect to
      * @param a_iPort the port the server is running
      * @param a_pQueue the queue that will receive the output from this client
      */
      virtual void startGameClient(irr::u32 a_iHostIP, int a_iPort, threads::CInputQueue *a_pQueue) = 0;

      /**
      * Stop a game client. Nothing happens if no client is running
      */
      virtual void stopGameClient() = 0;

      /**
      * Get the currently active state
      * @return the currently active state
      */
      virtual state::IState *getActiveState() = 0;

      /**
      * Get the a state by it's id
      * @param a_eState the id of the requested state
      * @return the state with the id or nullptr if no state with the id exists
      */
      virtual state::IState *getState(state::enState a_eState) = 0;

      /**
      * Get the name of a track
      * @param a_sTrack the track identifier
      * @return the track name, "Unknown Track" if no track data was found
      */
      virtual std::string getTrackName(const std::string &a_sTrack) = 0;

      /**
      * Init the next game screen. Must be called when the race data is defined
      */
      virtual void initNextRaceScreen() = 0;

      /**
      * Draw the next race screen
      * @param a_fAlpha the transparency of the next race screen [0..1]
      */
      virtual void drawNextRaceScreen(irr::f32 a_fAlpha) = 0;

      /**
      * Get the device name
      * @return the device name
      */
      virtual const std::string &getDeviceName() = 0;

      /**
      * Clear the GUI environment
      */
      virtual void clearGui() = 0;

      /**
      * Callback before the UI is cleared
      */
      virtual void beforeMenuLoad() = 0;

      /**
      * Callback when a new menu has been loaded
      */
      virtual void menuLoaded() = 0;

      /**
      * Callback when the virtual keyboard is destroyed
      */
      virtual void virtualKeyboardDestroyed() = 0;

      /**
      * Get the recorded log messages
      * @return the recorded log messages
      */
      virtual const std::vector<std::tuple<irr::ELOG_LEVEL, std::string>> &getLogMessages() = 0;

      /**
      * Get the brake map (key = velocity, value = number of steps to reach). For usage:
      * find the correct entry in the map for the current and the desired velocity and
      * use the difference of the values to know the number of steps to reach the speed
      * @return map of the measured braking efficiency
      */
      virtual const std::map<int, int> &getBrakeEfficiency() = 0;

      /**
      * Get the Irrlicht joystick information
      * @return the Irrlicht joystick information
      */
      virtual const irr::core::array<irr::SJoystickInfo> &getJoystickInfo() const = 0;

#ifdef _ANDROID
      /**
      * Get a pointer to the Android app
      * @return the Android app
      */
      virtual android_app *getAndroidApp() = 0;
#endif

      /**
      * Get the singleton instance of CGlobal
      * @return the singleton instance of CGlobal
      */
      static CGlobal *getInstance();
  };
}

