// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <algorithm>
#include <vector>

namespace dustbin {
  namespace state {
    /**
     * Enumeration for the states. If new states are added this enumeration must be extended
     */
    enum class enState {
      None = 0,       /**< No state, if the method IState::run returns this no state change is performed */
      LuaState = 1,   /**< The LUA state implementing all menues */
      GameState = 2,  /**< The center of all. The actual game */
      ErrorState = 3, /**< The error state */
      Quit = 255      /**< Application exit */
    };

    /**
    * @class IJoystickEventHandler
    * @author Christian Keimel
    * This class is used to pass joystick events to CControllerUi instances as Irrlicht
    * does not pass joystick events to UI elements
    */
    class IJoystickEventHandler {
      public:
        virtual void OnJoystickEvent(const irr::SEvent& a_cEvent) = 0;
    };

    /**
     * @class IState
     * @author Christian Keimel
     * @brief The interface for all states
     */
    class IState {
      private:
        bool m_bDefaultEnabled;

      protected:
        std::vector<IJoystickEventHandler*> m_vJoystickHandlers;

      public:
        enum enMouseButton {
          Left,
          Middle,
          Right
        };

        IState() : m_bDefaultEnabled(true) { }

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
         * This is a callback method that gets invoked when the window is resized
         * @param a_cDim the new dimension of the window
         */
        virtual void onResize(const irr::core::dimension2du &a_cDim) = 0;

        /**
        * Change the Z-Layer for the Menu Controller
        * @param a_iZLayer the new Z-Layer
        */
        virtual void setZLayer(int a_iZLayer) = 0;

        /**
        * This method is called before the UI is cleared on window resize. It can be
        * used to save all necessary data to re-build the UI
        */
        virtual void beforeResize() = 0;

        /**
         * Return the state's ID
         */
        virtual enState getId() = 0;

        /**
         * Get the state of the mouse buttons. As the cursor control Irrlicht Object does not
         * report the state of the button I decided to hack it this way
         * @param a_iButton The mouse button
         */
        virtual bool isMouseDown(enMouseButton a_iButton) = 0;

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
        * With this method you can block or enable handling "ok" and "cancel" by "Enter" or "Esc" key
        * @param a_bEnable enable the default handling
        */
        void enableDefault(bool a_bEnabled) {
          m_bDefaultEnabled = a_bEnabled;
        }

        /**
        * Is the default handling of "enter" and "cancel" enabled?
        * @return true or false
        */
        bool isDefaultEnabled() {
          return m_bDefaultEnabled;
        }

        /**
        * Register a joystick handler
        * @param a_pHandler the handler to register
        */
        void registerJoystickHandler(IJoystickEventHandler* a_pHandler) {
          if (std::find(m_vJoystickHandlers.begin(), m_vJoystickHandlers.end(), a_pHandler) == m_vJoystickHandlers.end()) {
            m_vJoystickHandlers.push_back(a_pHandler);
          }
        }

        /**
        * Unregister a joystick handler
        * @param a_pHandler the handler to remove from the list
        */
        void unregisterJoystickHandler(IJoystickEventHandler* a_pHandler) {
          if (std::find(m_vJoystickHandlers.begin(), m_vJoystickHandlers.end(), a_pHandler) != m_vJoystickHandlers.end()) {
            m_vJoystickHandlers.erase(std::find(m_vJoystickHandlers.begin(), m_vJoystickHandlers.end(), a_pHandler));
          }
        }
    };
  }
}
