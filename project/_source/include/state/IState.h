// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

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
     * @class IState
     * @author Christian Keimel
     * @brief The interface for all states
     */
    class IState {
      public:
        enum enMouseButton {
          Left,
          Middle,
          RIght
        };

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
    };
  }
}
