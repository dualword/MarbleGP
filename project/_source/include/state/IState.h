// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <menu/IMenuManager.h>
#include <algorithm>
#include <vector>

namespace dustbin {
  class CGlobal;  /**< Forward declration of the global singleton class */

  namespace state {
    /**
     * Enumeration for the states. If new states are added this enumeration must be extended
     */
    enum class enState {
      None,         /**< No state, if the method IState::run returns this no state change is performed */
      Menu,         /**< The LUA state implementing all menues */
      Game,         /**< The center of all. The actual game */
      Restart,      /**< Restart the application */
      Quit          /**< Application exit */
    };

    /**
     * @class IState
     * @author Christian Keimel
     * @brief The interface for all states
     */
    class IState {
      protected:
        irr::IrrlichtDevice       *m_pDevice;
        irr::io::IFileSystem      *m_pFs;
        irr::scene::ISceneManager *m_pSmgr;
        irr::gui::IGUIEnvironment *m_pGui;
        irr::video::IVideoDriver  *m_pDrv;

        CGlobal *m_pGlobal;

        enState m_eState;

        bool m_bWillBeDeleted;    /**< A flag to distinguish between the normal deactivation and deletion on program end */

      public:
        enum enMouseButton {
          Left,
          Middle,
          Right
        };

        IState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal) : m_pDevice(a_pDevice), m_pFs(nullptr), m_pSmgr(nullptr), m_pGui(nullptr), m_pDrv(nullptr), m_pGlobal(a_pGlobal), m_eState(enState::None) { 
          m_pSmgr = m_pDevice->getSceneManager();
          m_pGui  = m_pDevice->getGUIEnvironment();
          m_pFs   = m_pDevice->getFileSystem();
          m_pDrv  = m_pDevice->getVideoDriver();
          
          m_bWillBeDeleted = false;
        }

        virtual ~IState() { }

        /**
         * This method is called when the state is activated
         */
        virtual void activate() = 0;

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate() = 0;

        /**
         * Return the state's ID
         */
        virtual enState getId() = 0;

        /**
         * Event handling method. The main class passes all Irrlicht events to this method
         */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) = 0;

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run() = 0;

        /**
        * Get the global instance
        * @return the global instance
        */
        CGlobal* getGlobal() {
          return m_pGlobal;
        }

        /**
        * Restart the application, e.g. because the gfx settings have changed
        */
        void setState(enState a_eState) {
          m_eState = a_eState;
        }

        /**
        * Does this state want the virtual keyboard?
        */
        virtual bool showVirtualKeyboard() {
          return false;
        }

        /**
        * A callback for the menu state to get informed about a menu change
        * @param a_sMenu the loaded menu
        */
        virtual void menuChanged(const std::string &a_sMenu) { }

        void willBeDeleted() {
          m_bWillBeDeleted = true;
        }

        /**
        * Turn the menu controller on or off
        * @param a_bFlag on / off flag
        */
        virtual void enableMenuController(bool a_bFlag) {
        }
    };
  }
}
