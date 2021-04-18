// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht/irrlicht.h>
#include <string>

namespace dustbin {

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

  /**
   * @class CGlobal
   * @author Christian Keimel
   * @brief This interface is used to store data that should be available globally
   */
  class CGlobal {
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
  };
}