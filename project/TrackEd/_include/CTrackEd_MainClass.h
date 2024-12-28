// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <commands/CCommandHandler.h>
#include <font/font_manager.h>
#include <IGlobal.h>

#include <irrlicht.h>
#include <string>
#include <vector>
#include <map>

namespace dustbin {
  namespace tracked {
    class CTrackPartFactory;    /**< Forward declaration of the track part factory */

    /**
    * @class TrackEd_MainClass
    * @author Christian Keimel
    * The main class of the MarbleGP track editor
    */
    class CTrackEd_MainClass : public IGlobal, public commands::CCommandHandler {
      private:
        std::map<std::string, std::string> m_mSettings;   /**< The settings map */
        std::map<std::string, std::string> m_mGlobals;    /**< The map with the global values, not stored when the program ends */

        irr::IrrlichtDevice       *m_pDevice;     /**< The Irrlicht device */
        irr::scene::ISceneManager *m_pSmgr;       /**< The Irrlicht scene manager */
        irr::video::IVideoDriver  *m_pDrv;        /**< The Irrlicht video driver */
        irr::gui::IGUIEnvironment *m_pGui;        /**< The Irrlicht GUI environment */
        irr::io::IFileSystem      *m_pFs;         /**< The Irrlicht file system */
        irr::video::SMaterial      m_cMaterial;   /**< The material for the "before end scene" callback */

        FontManager *m_pFontMgr;    /**< The TTF font manager */

        std::wstring m_sPathMarbleGP;   /**< The data path of MarbleGP */
        std::wstring m_sPathTrackEd ;   /**< The data path of the track editor */

        commands::CCmdEditTrack *m_pEditCommand;    /**< The "Edit Track" command */

        irr::u32 m_iWidth;    /**< Width of the window */
        irr::u32 m_iHeight;   /**< Height of the window */

        /**
        * Load the settings XML
        */
        void loadSettings();

        /**
        * Save the settings XML
        */
        void saveSettings();


      public:
        CTrackEd_MainClass();
        virtual ~CTrackEd_MainClass();

        /**
        * Run the editor
        * @return false to stop the application
        */
        bool run();

        /**
        * Load a scene. Use this method instead of directly calling
        * "SceneManager->loadscene" as an user event is fired
        * @param a_sFile the scene file
        */
        virtual void loadScene(const std::string &a_sFile) override;

        /**
        * Clear the scene. Use this method as an user event is fired
        */
        virtual void clearScene() override;

        /**
        * Get a setting
        * @param a_sKey the setting key
        * @return the value for the key, empty string if key was not found
        */
        virtual std::string getSetting(const std::string& a_sKey) override;

        /**
        * Set a setting
        * @param a_sKey the key of the setting
        * @param a_sValue the value of the setting
        */
        virtual void setSetting(const std::string& a_sKey, const std::string& a_sValue) override;

        /**
        * Get a global variable not stored when the program quits
        * @param a_sKey the name of the variable
        * @return the value of the variable, en empty string if the name was not found
        */
        virtual std::string getGlobal(const std::string &a_sKey) override;

        /**
        * Set a global variable not stored when the program quits
        * @param a_sKey the name of the variable
        * @param a_sValue the value of the variable
        */
        virtual void setGlobal(const std::string &a_sKey, const std::string &a_sValue) override;

        /**
        * A callback called before "endscene" is called. In this
        * place the commands can draw something (if necessary)
        */
        virtual void beforeEndScene() override;

        /**
        * Get the Irrlicht Device
        * @return the Irrlicht Device
        */
        virtual irr::IrrlichtDevice *getDevice() override;

        /**
        * Get the scene manager
        * @return the scene manager
        */
        virtual irr::scene::ISceneManager *getSceneManager() override;

        /**
        * Get the video driver
        * @return the video driver
        */
        virtual irr::video::IVideoDriver *getVideoDriver() override;

        /**
        * Get the GUI environment
        * @return the GUI environment
        */
        virtual irr::gui::IGUIEnvironment *getGuiEnvironment() override;

        /**
        * Get the file system
        * @return the file system
        */
        virtual irr::io::IFileSystem *getFileSystem() override;

        /**
        * Get the active "Edit Track" command
        * @return the active "Edit Track" command
        */
        virtual commands::CCmdEditTrack *getEditTrackCommand() override;
    };
  }
}


