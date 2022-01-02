// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <state/IState.h>
#include <CGlobal.h>
#include <string>
#include <vector>
#include <map>

class CGUITTFace;

namespace dustbin {

  enum class enAppState {
    Continue,
    Restart,
    Quit
  };

  /**
   * @class CMainClass
   * @author Christian Keimel
   * @brief This is the main class. It forwards events to all states and it takes care of handling settings and globals
   */
  class CMainClass : public CGlobal {
    private:
      int m_iRasterSize;

      irr::IrrlichtDevice       *m_pDevice; /**< The Irrlicht Device Instance */
      irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht Video Driver Instance */
      irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht Gui Environment Instance */
      irr::scene::ISceneManager *m_pSmgr;   /**< The Irrlicht Scene Manager Instance */
      irr::io::IFileSystem      *m_pFs;     /**< The Irrlicht File System Instance */

      enAppState m_eState;  /**< The current state that is returned by the "run" method */

      irr::core::dimension2du m_cScreenSize;  /**< The current size of the window (or screen if fullscreen) */

      SSettings m_cSettings;  /**< The game settings */

      irr::gui::IGUIFont* m_pFallbackFont;  /**< Fallback font for OpenGL ES */

      state::IState *m_pActiveState;  /**< The currently active state */

      std::map<std::string, std::string> m_mSettings,   /**< The settings table. Will be stored when the instance is destroyed */
                                         m_mGlobals;    /**< The globals table */

      std::vector<std::string> m_vScriptStack;  /**< The stack of LUA scripts */

      std::vector<irr::scene::ISceneManager *> m_vBeforeGui,
                                               m_vAfterGui;

      std::vector<irr::video::ITexture*> m_vRemoveOnResize; /**< A vector that stores the textures that will be removed on resize (type "button://") */

      std::map<state::enState, state::IState *> m_mStates; /**< The game states */

      state::enState m_eStateChange;  /**< Global for requested state change */

      dustbin::audio::CSoundInterface *m_pSoundInterface; /**< The sound interface */

      std::map<int, irr::gui::IGUIFont*> m_mFonts;  /**< A map for the fonts */

      std::vector<irr::video::ITexture*> m_pRtts; /**< Render target textures */

      std::vector<int> m_vFontSizes;  /**< The supported font sizes */

      /**
      * This function loads the pattern texture and adjusts the marble
      * @param a_sFile the filename of the pattern texture
      * @param a_cColor the color for the pattern
      */
      irr::video::ITexture* adjustTextureForMarble(const std::string& a_sFile, const irr::video::SColor& a_cColor);

      /**
      * Convert a string to an Irrlicht color
      * @param a_cColor the color that will be changed
      * @param a_sColor the input string
      */
      void fillColorFromString(irr::video::SColor& a_cColor, const std::string& a_sColor);

      /**
      * Find a parameter in the map of texture parameters
      * @param a_mParameters the parameter map
      * @param a_sKey the key of the parameter
      * @return the parameter, empty string if the key was not found
      */
      std::string findTextureParameter(std::map<std::string, std::string>& a_mParameters, const std::string a_sKey);

      /**
      * Parse texture parameters
      * @param a_sInput the input as string
      * @return a string/string map with all parameters
      */
      std::map<std::string, std::string> parseParameters(const std::string& a_sInput);

      /**
      * Add a fading border to the starting numbers
      * @param a_sNumber the number
      * @param a_cNumberColor the color of the number
      * @param a_cBorderColor the color of the frame
      * @param a_cBackgroundColor the background color of the number portion of the texture
      * @return a texture with the fading border
      */
      irr::video::ITexture *createFadingBorder(const std::string a_sNumber, const irr::video::SColor &a_cNumberColor, const irr::video::SColor &a_cBorderColor, const irr::video::SColor &a_cBackgroundColor);

    public:
      CMainClass();
      virtual ~CMainClass();

      /**
       * This method does everything. On return the application ends
       */
      enAppState run();

      // Methods inherited from CGlobal

      /**
      * Determine whether or not a setting value exists for the given key
      * @param a_sKey the key to search for
      * @return true if the key exists, false otherwise
      */
      virtual bool hasSetting(const std::string &a_sKey);

      /**
      * Method to get a setting. Settings are stored and loaded when the application ends or starts.
      * @param a_sKey key of the setting
      * @return value assigned to the key, empty string if no value was found
      */
      virtual std::string getSetting(const std::string &a_sKey);

      /**
      * Method to modify or set a setting. Settings are stored and loaded when the application ends or starts.
      * @param a_sKey key of the setting
      * @param a_sValue value of the setting. An empty string will erase the setting
      */
      virtual void setSetting(const std::string &a_sKey, const std::string &a_sValue);


      /**
      * Modify or set a global variable. Globals are not loaded or saved so they live only while the application is running
      * @param a_sKey the key of the global
      * @param a_sValue the value of the global. An empty string will erase the global
      */
      virtual void setGlobal(const std::string &a_sKey, const std::string &a_sValue);

      /**
      * Retrieve a global. Globals are not loaded or saved so they live only while the application is running
      * @param a_sKey the key of the global
      * @return the global assigned to the key, an empty string if the global does not exist
      */
      virtual std::string getGlobal(const std::string &a_sKey);


      /**
      * Determine whether or not a global exists
      * @param a_sKey the key to search for
      * @return true if the global exists, false otherwise
      */
      virtual bool hasGlobal(const std::string &a_sKey);

      /**
      * Get the active Irrlicht device
      * @return the active Irrlicht device
      */
      virtual irr::IrrlichtDevice *getIrrlichtDevice();

      /**
      * Get the Irrlicht File System Instance
      * @return the Irrlicht File System Instance
      */
      virtual irr::io::IFileSystem *getFileSystem();

      /**
      * Get the Irrlicht Scene Manager Instance
      * @return 
      */
      virtual irr::scene::ISceneManager *getSceneManager();

      /**
      * Get the Irrlicht gui environment instance
      * @return the Irrlicht gui environment instance
      */
      virtual irr::gui::IGUIEnvironment *getGuiEnvironment();

      /**
      * Get the Irrlicht video driver instance
      * @return the Irrlicht video driver instance
      */
      virtual irr::video::IVideoDriver *getVideoDriver();

      /**
      * Open a file for reading. The folder is defined by the file type, and the function searches
      * the data folder first, thereafter it searches the MarbleGP.dat file
      * @param a_eType type of file
      * @param a_sName name of file (without path)
      * @return an Irrlicht read file object instance
      */
      virtual irr::io::IReadFile *openFileForReading(enFileType a_eType, const std::string &a_sName);

      /**
      * Opens a file for writing. This function will always return a file in the "data" folder of the user's data directory
      * @param a_sName name of the file
      */
      virtual irr::io::IWriteFile *openFileForWriting(const std::string &a_sName);

      /**
      * Search for a file. The place to look for the file is either the user's data folder or both
      * @param a_eSearch the seach location (data folder, MarbleGP data archive, both)
      * @param a_eType the type of file to search. This defines the folder to search in
      * @param a_sName the name of the file (without path information)
      * @return true if the file exists, false otherwise
      */
      virtual bool existsFile(enFileSearch a_eSeach, enFileType a_eType, const std::string &a_sName);

      /**
      * Push a script to the stack of lua scripts
      * @param a_sScript the script to push
      */
      virtual void pushScript(const std::string &a_sScript);

      /**
      * Remove and return the topmost script from the stack of lua scripts
      * @return the topmost script from the lus script stack
      */
      virtual std::string popScript();

      /**
      * Get the topmost script from the lua script stack without removing it
      */
      virtual std::string peekScript();

      /**
      * Clear the script stack
      */
      virtual void clearScriptStack();

      /**
       * Get the currently active state
       * @ return the currently active state
       */
      virtual state::IState *getActiveState();


      /**
      * Register a new scene manager for drawing
      * @param a_pSmgr the new scene manager
      * @param a_iRenderPosition the position to render the new scene manager. "0" is the render index of the GUI environment, negative position renders the new scene manager before the gui
      */
      virtual void registerSceneManager(irr::scene::ISceneManager *a_pSmgr, int a_iRenderPosition);

      /**
      * Remove a scene manager from the rendering pipeline
      * @param a_pSmgr the scene manager to remove
      */
      virtual void removeSceneManager(irr::scene::ISceneManager *a_pSmgr);

      /**
      * This function requests a state change. The change will be applied with the next "IState::run" call
      * @param a_eState the new state
      */
      virtual void stateChange(dustbin::state::enState a_eNewState);

      /**
       * Get the raster size for the UI layout
       * @return the raster size
       */
      virtual int getRasterSize();

      /**
       * Get a rectangle on the screen for GUI layout
       * @param a_iLeft Left position
       * @param a_iTop Top position
       * @param a_iRight Right position
       * @param a_iBottom bottom position
       * @param a_ePosition the layout position of the rectangle
       * @param a_pParent an optional parent element
       */
      virtual irr::core::recti getRect(int a_iLeft, int a_iTop, int a_iRight, int a_iBottom, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr);

      /**
       * Get a rectangle on the screen for GUI layout
       * @param a_cRect Irrlicht rectangle with the coordinates to be converted
       * @param a_ePosition the layout position of the rectangle
       * @param a_pParent an optional paraent element
       */
      virtual irr::core::recti getRect(const irr::core::recti& a_cRect, enLayout a_ePosition, irr::gui::IGUIElement* a_pParent = nullptr);

      /**
      * Get a requested state change. This function also sets the corresponding member back to "None"
      * @return the state change
      */
      virtual dustbin::state::enState getStateChange();

      /**
      * Get access to the sound interface singleton
      * @return the sound interface singleton
      */
      virtual audio::CSoundInterface *getSoundInterface();

      /**
      * Get a font for a specific viewport size
      * @param a_eFont the type of font
      * @param a_cViewport the viewport size
      * @see dustbin::enFont
      */
      virtual irr::gui::IGUIFont* getFont(enFont a_eFont, const irr::core::dimension2du a_cViewport);

      /**
      * Get an image from a string. The following prefixes are possible:
      * - file://: load a file from a subfolder
      * - generate://: generate a marble texture
      * @param a_sInput the URI of the file
      * @return an Irrlicht texture object with the requested image or nullptr
      */
      virtual irr::video::ITexture* createTexture(const std::string& a_sUri);

      /**
      * Get the settings struct
      * @return the settings struct
      */
      virtual const SSettings& getSettings();

      /**
      * Update the game settings
      * @param a_cSettings the settings
      */
      virtual void setSettings(const SSettings& a_cSettings);

      // Method inherited from irr::IEventReceiver
      virtual bool OnEvent(const irr::SEvent &a_cEvent);
  };
}