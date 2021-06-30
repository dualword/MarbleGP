// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <state/IState.h>
#include <string>

namespace dustbin {

  namespace state {
    class IState;
  }

  namespace audio {
    class CSoundInterface;
  }

  enum class enFileType {
    ImageFile,
    LuaFile,
    SceneFile,
    DataFile
  };

  enum class enFileSearch {
    DataFolder,
    MarbleGPData,
    Both
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
    LowerRight
  };

  enum class enFont {
    Tiny,
    Small,
    Regular,
    Big,
    Huge
  };

  /**
   * @class CGlobal
   * @author Christian Keimel
   * @brief This interface is used to store data that should be available globally
   */
  class CGlobal : public irr::IEventReceiver {
    protected:
      static CGlobal *m_pInstance;

    public:
      virtual ~CGlobal() { }


      /**
       * Method to get a setting. Settings are stored and loaded when the application ends or starts.
       * @param a_sKey key of the setting
       * @return value assigned to the key, empty string if no value was found
       */
      virtual std::string getSetting(const std::string &a_sKey) = 0;

      /**
       * Method to modify or set a setting. Settings are stored and loaded when the application ends or starts.
       * @param a_sKey key of the setting
       * @param a_sValue value of the setting. An empty string will erase the setting
       */
      virtual void setSetting(const std::string &a_sKey, const std::string &a_sValue) = 0;

      /**
       * Determine whether or not a setting value exists for the given key
       * @param a_sKey the key to search for
       * @return true if the key exists, false otherwise
       */
      virtual bool hasSetting(const std::string &a_sKey) = 0;

      /**
       * Retrieve a global. Globals are not loaded or saved so they live only while the application is running
       * @param a_sKey the key of the global
       * @return the global assigned to the key, an empty string if the global does not exist
       */
      virtual std::string getGlobal(const std::string &a_sKey) = 0;

      /**
       * Modify or set a global variable. Globals are not loaded or saved so they live only while the application is running
       * @param a_sKey the key of the global
       * @param a_sValue the value of the global. An empty string will erase the global
       */
      virtual void setGlobal(const std::string &a_sKey, const std::string &a_sValue) = 0;

      /**
       * Determine whether or not a global exists
       * @param a_sKey the key to search for
       * @return true if the global exists, false otherwise
       */
      virtual bool hasGlobal(const std::string &a_sKey) = 0;

      /**
       * Get the active Irrlicht device
       * @return the active Irrlicht device
       */
      virtual irr::IrrlichtDevice *getIrrlichtDevice() = 0;

      /**
       * Get the Irrlicht File System Instance
       * @return the Irrlicht File System Instance
       */
      virtual irr::io::IFileSystem *getFileSystem() = 0;

      /**
       * Get the Irrlicht Scene Manager Instance
       * @return the Irrlicht Scene Manager Instance
       */
      virtual irr::scene::ISceneManager *getSceneManager() = 0;

      /**
       * Get the Irrlicht gui environment instance
       * @return the Irrlicht gui environment instance
       */
      virtual irr::gui::IGUIEnvironment *getGuiEnvironment() = 0;

      /**
       * Get the Irrlicht video driver instance
       * @return the Irrlicht video driver instance
       */
      virtual irr::video::IVideoDriver *getVideoDriver() = 0;

      /**
       * Push a script to the stack of lua scripts
       * @param a_sScript the script to push
       */
      virtual void pushScript(const std::string &a_sScript) = 0;

      /**
       * Remove and return the topmost script from the stack of lua scripts
       * @return the topmost script from the lus script stack
       */
      virtual std::string popScript() = 0;

      /**
       * Get the topmost script from the lua script stack without removing it
       */
      virtual std::string peekScript() = 0;

      /**
       * Open a file for reading. The folder is defined by the file type, and the function searches
       * the data folder first, thereafter it searches the MarbleGP.dat file
       * @param a_eType type of file
       * @param a_sName name of file (without path)
       * @return an Irrlicht read file object instance
       */
      virtual irr::io::IReadFile *openFileForReading(enFileType a_eType, const std::string &a_sName) = 0;

      /**
       * Opens a file for writing. This function will always return a file in the "data" folder of the user's data directory
       * @param a_sName name of the file
       */
      virtual irr::io::IWriteFile *openFileForWriting(const std::string &a_sName) = 0;

      /**
       * Search for a file. The place to look for the file is either the user's data folder or both
       * @param a_eSearch the seach location (data folder, MarbleGP data archive, both)
       * @param a_eType the type of file to search. This defines the folder to search in
       * @param a_sName the name of the file (without path information)
       * @return true if the file exists, false otherwise
       */
      virtual bool existsFile(enFileSearch a_eSeach, enFileType a_eType, const std::string &a_sName) = 0;

      /**
       * Get the currently active state
       * @ return the currently active state
       */
      virtual state::IState *getActiveState() = 0;

      /**
       * Register a new scene manager for drawing
       * @param a_pSmgr the new scene manager
       * @param a_iRenderPosition the position to render the new scene manager. "0" is the render index of the GUI environment, negative position renders the new scene manager before the gui
       */
      virtual void registerSceneManager(irr::scene::ISceneManager *a_pSmgr, int a_iRenderPosition) = 0;

      /**
      * Remove a scene manager from the rendering pipeline
      * @param a_pSmgr the scene manager to remove
      */
      virtual void removeSceneManager(irr::scene::ISceneManager *a_pSmgr) = 0;

      /**
       * Get a setting as boolean
       * @param a_sKey the key of the setting
       * @return the value, false if the key is not stored
       */
      bool getSettingBool(const std::string &a_sKey);

      /**
       * Get a setting as integer
       * @param a_sKey the key of the setting
       * @return the value, 0 if the key is not stored
       */
      int getSettingInt(const std::string &a_sKey);

      /**
       * Get a setting as double
       * @param a_sKey the key of the setting
       * @return the value, 0.0 if the key is not stored
       */
      double getSettingDouble(const std::string &a_sKey);

      /**
       * Set a boolean setting
       * @param a_sKey the key of the setting
       * @param a_bValue the value
       */
      void setSettingBool(const std::string &a_sKey, bool a_bValue);

      /**
       * Set an integer setting
       * @param a_sKey the key of the setting
       * @param a_iValue the value
       */
      void setSettingInt(const std::string &a_sKey, int a_iValue);

      /**
       * Set a double setting
       * @param a_sKey the key of the setting
       * @param a_fValue the value
       */
      void setSettingDouble(const std::string &a_sKey, double a_fValue);

      /**
       * Set a boolean global
       * @param a_sKey the key of the setting
       * @param a_bValue the value
       */
      void setGlobalBoolean(const std::string &a_sKey, bool a_bValue);

      /**
       * Set an integer global
       * @param a_sKey the key of the setting
       * @param a_bValue the value
       */
      void setGlobalInt(const std::string &a_sKey, int a_iValue);

      /**
       * Set a double global
       * @param a_sKey the key of the setting
       * @param a_bValue the value
       */
      void setGlobalDouble(const std::string &a_sKey, double a_fValue);

      /**
       * Get a global as boolean
       * @param a_sKey the key of the setting
       * @return the value, 0 if the key is not stored
       */
      bool getGlobalBool(const std::string &a_sKey);

      /**
       * Get a global as integer
       * @param a_sKey the key of the setting
       * @return the value, 0 if the key is not stored
       */
      int getGlobalInt(const std::string &a_sKey);

      /**
       * Get a global as double
       * @param a_sKey the key of the setting
       * @return the value, 0 if the key is not stored
       */
      double getGlobalDouble(const std::string &a_sKey);

      /**
       * Get the singleton instance of CGlobal
       * @return the singleton instance of CGlobal
       */
      static CGlobal *getInstance();

      /**
      * This function requests a state change. The change will be applied with the next "IState::run" call
      * @param a_eState the new state
      */
      virtual void stateChange(dustbin::state::enState a_eNewState) = 0;

      /**
       * Get the raster size for the UI layout
       * @return the raster size
       */
      virtual int getRasterSize() = 0;

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
      * Get a requested state change. This function also sets the corresponding member back to "None"
      * @return the state change
      */
      virtual state::enState getStateChange() = 0;

      /**
      * Get access to the sound interface singleton
      * @return the sound interface singleton
      */
      virtual audio::CSoundInterface *getSoundInterface() = 0;

      /**
      * Get a font for a specific viewport size
      * @param a_eFont the type of font
      * @param a_cViewport the viewport size
      * @see dustbin::enFont
      */
      virtual irr::gui::IGUIFont* getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport) = 0;

      /**
      * Get an image from a string. The following prefixes are possible:
      * - file://: load a file from a subfolder
      * - generate://: generate a marble texture
      * @param a_sInput the URI of the file
      * @return an Irrlicht texture object with the requested image or nullptr
      */
      virtual irr::video::ITexture* createTexture(const std::string& a_sUri) = 0;

      virtual bool OnEvent(const irr::SEvent& a_cEvent) = 0;
  };
}