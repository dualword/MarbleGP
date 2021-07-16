// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <state/IState.h>

namespace dustbin {
  namespace state {
    /**
    * @class CErrorState
    * @author Christian Keimel
    * This is the state that show the error messages
    */
    class CErrorState : public IState {
      private:
        bool m_bBackToLua;

        /**
        * This function creates the UI, it is called from "activate" and "onResize"
        */
        void createUi();

      public:
        CErrorState();
        ~CErrorState();

        /**
         * This method is called when the state is activated
         */
        virtual void activate();

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate();

        /**
         * This is a callback method that gets invoked when the window is resized
         * @param a_cDim the new dimension of the window
         */
        virtual void onResize(const irr::core::dimension2du& a_cDim);

        /**
        * Change the Z-Layer for the Menu Controller
        * @param a_iZLayer the new Z-Layer
        */
        virtual void setZLayer(int a_iZLayer) override;

        /**
        * This method is called before the UI is cleared on window resize. It can be
        * used to save all necessary data to re-build the UI
        */
        virtual void beforeResize();

        /**
         * Return the state's ID
         */
        virtual enState getId();

        /**
         * Get the state of the mouse buttons. As the cursor control Irrlicht Object does not
         * report the state of the button I decided to hack it this way
         * @param a_iButton The mouse button
         */
        virtual bool isMouseDown(enMouseButton a_iButton);

        /**
         * Event handling method. The main class passes all Irrlicht events to this method
         */
        virtual bool OnEvent(const irr::SEvent& a_cEvent);

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run();
    };
  } // namespace state
} // namespace dustbin