// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht/irrlicht.h>
#include <state/IState.h>
#include <CGlobal.h>
#include <string>
#include <vector>
#include <map>

namespace dustbin {

  /**
   * @class CMainClass
   * @author Christian Keimel
   * @brief This is the main class. It forwards events to all states and it takes care of handling settings and globals
   */
  class CMainClass : public irr::IEventReceiver, public CGlobal {
    private:
      irr::IrrlichtDevice       *m_pDevice; /**< The Irrlicht Device Instance */
      irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht Video Driver Instance */
      irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht Gui Environment Instance */
      irr::scene::ISceneManager *m_pSmgr;   /**< The Irrlicht Scene Manager Instance */
      irr::io::IFileSystem      *m_pFs;     /**< The Irrlicht File System Instance */

      state::IState *m_pActiveState;  /**< The currently active state */

      std::map<std::string, std::string> m_mSettings,   /**< The settings table. Will be stored when the instance is destroyed */
                                         m_mGlobals;    /**< The globals table */

      std::vector<std::string> m_vScriptStack;  /**< The stack of LUA scripts */

      std::map<state::enState, state::IState *> m_mStates; /**< The game states */

    public:
      CMainClass();
      virtual ~CMainClass();

      /**
       * This method does everything. On return the application ends
       */
      void run();

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

      // Method inherited from irr::IEventReceiver
      virtual bool OnEvent(const irr::SEvent &a_cEvent);
  };
}