// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <data/CDataStructs.h>
#include <irrlicht.h>
#include <string>

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
      * Get the raster size for the UI layout
      * @return the raster size
      */
      virtual int getRasterSize() = 0;

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

