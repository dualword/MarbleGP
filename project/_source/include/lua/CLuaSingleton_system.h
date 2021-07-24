// (w) 2021 by Dustbin::Games / Christian Keimel
// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua/ILuaObject.h>
#include <lua.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  class CGlobal;

  namespace lua {
    class CLuaSceneManager;

    /**
    * @class CLuaResolutionList
    * @author Christian Keimel
    * This class stores the list of possible resolutions for the GFX settings menu
    */
    class CLuaResolutionList : public ILuaObject {
      private:
        std::vector<irr::core::dimension2du> m_vList;  /**< The list of resolutions (0 == width, 1 == height, 2 == pixel depth */

      public:
        CLuaResolutionList(lua_State *a_pState);
        virtual ~CLuaResolutionList();

        /**
        * Returns the number of resolutions in the vector
        * @return the number of resolutions in the vector
        */
        int getCount();

        /**
        * Get a string representation of the resolution
        * @param a_iIndex the index of the resolution
        * @return a string representation of the resolution
        */
        std::string getResolutionString(int a_iIndex);

        /**
        * Get the resolution data
        * @param a_iIndex the index of the resolution
        * @param a_cResolution [out] the resolution
        * @return true if the resolution was found, false otherwise
        */
        bool getResolution(int a_iIndex, irr::core::dimension2du &a_cResolution);

        /**
        * Refresh the list. If the window was resized manually the list might not be up to date
        */
        void refresh();
    };

    /**
    * @class CLuaSingleton_system
    * @author Christian Keimel
    * This is the singleton for the global LUA "system" object
    */
    class CLuaSingleton_system : public ILuaObject {
      private:
        CLuaResolutionList *m_pResolutionList;
        CGlobal *m_pGlobal;
        lua_State *m_pState;

      public:
        CLuaSingleton_system(lua_State *a_pState);
        virtual ~CLuaSingleton_system();

        /**
        * Get the global scene manager
        * @return the global scene manager
        */
        CLuaSceneManager getSceneManager();

        /**
        * Set a setting
        * @param a_sKey the key of the setting
        * @param a_sValue the value of the setting
        */
        void setSetting(const std::string &a_sKey, const std::string &a_sValue);

        /**
        * Get a setting
        * @param a_sKey the key of the setting
        * @return the setting, empty string if the setting was not found
        */
        std::string getSetting(const std::string &a_sKey);

        /**
        * Set a global variable
        * @param a_sKey the key of the global variable
        * @param a_sValue the value of the global variable
        */
        void setGlobal(const std::string &a_sKey, const std::string &a_sValue);

        /**
        * Get a global variable
        * @param a_sKey the key of the global variable
        * @return the global variable, empty string if the global was not found
        */
        std::string getGlobal(const std::string &a_sKey);

        /**
        * Push a script to the lua script stack
        * @param a_sScript the script to push
        */
        void pushScript(const std::string &a_sScript);

        /**
        * Request a state change
        * @param a_iNewState integer version of the new state
        */
        void stateChange(int a_iNewState);

        /**
        * Get a list with the supported screen resolutions
        */
        CLuaResolutionList *getResolutionList();

        /**
        * Execute a lua script from a file
        * @param a_sFile filename of the script
        */
        void executeLuaScript(const std::string &a_sFile);

        /**
        * Execute a lua script from a string
        * @param a_sScript script as std::string
        */
        void executeLuaString(const std::string &a_sScript);

        /**
        * Get the application settings from a LUA script
        */
        int getSettings(lua_State* a_pState);

        /**
        * Update the settings from a LUA script
        */
        int setSettings(lua_State* a_pState);

        /**
        * Create a XML string with the default configuration for the menu controller
        * @return a XML string
        */
        std::string getControllerXml_Menu();

        /**
        * Create a XML string with the default configuration for the game controller
        * @return a XML string
        */
        std::string getControllerXml_Game();

        /**
        * Get the first controller from the configuration
        * @param a_sConfig the configuration as XML-string
        * @return the first controller
        */
        std::string getFirstController(const std::string& a_sConfig);

        /**
        * Url-Encode a string
        * @param a_sInput the un-encoded string
        * @return the encoded string
        */
        std::string urlEncode(const std::string a_sInput);

        /**
        * Url-Decode a string
        * @param a_sInput the encoded string
        * @return the decoded string
        */
        std::string urlDecode(const std::string a_sInput);

        /**
        * Get a list of the available texture patterns
        * @param a_pState the LUA state
        * @return "1", as only the list of patterns is returned
        */
        int getTexturePatterns(lua_State* a_pState);

        /**
        * Change the Z-Layer for the menu controller
        * @param m_iZLayer the new Z-Layer
        */
        void setZLayer(int a_iZLayer);
    };
  }
}
