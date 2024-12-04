// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <map>

namespace dustbin {
  namespace tracked {
    /**
    * @class IGlobal
    * @author Christian Keimel
    * The global interface which gives access to all important object instances
    */
    class IGlobal {
      protected:
        static IGlobal* m_pInstance;

      public:
        IGlobal();
        virtual ~IGlobal();

        /**
        * Get the TrackEd_MainClass singleton instance
        * @return the TrackEd_MainClass singleton instance
        */
        static IGlobal* getInstance();

        /**
        * Load a scene. Use this method instead of directly calling
        * "SceneManager->loadscene" as an user event is fired
        * @param a_sFile the scene file
        */
        virtual void loadScene(const std::string &a_sFile) = 0;

        /**
        * Clear the scene. Use this method as an user event is fired
        */
        virtual void clearScene() = 0;

        /**
        * Get a setting
        * @param a_sKey the setting key
        * @return the value for the key, empty string if key was not found
        */
        virtual std::string getSetting(const std::string& a_sKey) = 0;

        /**
        * Set a setting
        * @param a_sKey the key of the setting
        * @param a_sValue the value of the setting
        */
        virtual void setSetting(const std::string& a_sKey, const std::string& a_sValue) = 0;

        /**
        * A callback called before "endscene" is called. In this
        * place the commands can draw something (if necessary)
        */
        virtual void beforeEndScene() = 0;

        /**
        * Get the Irrlicht Device
        * @return the Irrlicht Device
        */
        virtual irr::IrrlichtDevice* getDevice() = 0;

        /**
        * Get the scene manager
        * @return the scene manager
        */
        virtual irr::scene::ISceneManager* getSceneManager() = 0;

        /**
        * Get the video driver
        * @return the video driver
        */
        virtual irr::video::IVideoDriver* getVideoDriver() = 0;

        /**
        * Get the GUI environment
        * @return the GUI environment
        */
        virtual irr::gui::IGUIEnvironment* getGuiEnvironment() = 0;

        /**
        * Get the file system
        * @return the file system
        */
        virtual irr::io::IFileSystem* getFileSystem() = 0;
    };
  }
}
