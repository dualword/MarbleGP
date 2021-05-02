// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <state/IState.h>

class CLuaScript_dialog;

namespace dustbin {
  class CGlobal;

  namespace scenenodes {
    class CGui3dRoot;
  }

  namespace state {
    /**
    * @class CLuaState
    * @author Christian Keimel
    * @brief This state is responsible for all menues
    */
    class CLuaState : public IState {
      private:
        irr::IrrlichtDevice       *m_pDevice;
        irr::video::IVideoDriver  *m_pDrv;
        irr::scene::ISceneManager *m_pSmgr;
        irr::gui::IGUIEnvironment *m_pGui;

        CLuaScript_dialog      *m_pScript;
        CGlobal                *m_pGlobal;
        scenenodes::CGui3dRoot *m_pGuiRoot;

        bool m_bButtons[3];   /**< The mouse button states */

        /**
         * Initialize the 3d GUI Root node
         * @param a_pParent the children of this scenenode will be iterated to find the root of the 3d GUI
         */
        void initGuiRoot(irr::scene::ISceneNode *a_pParent);

      protected:
        virtual void onUievent(const std::string &a_type, irr::s32 a_id, const std::string &a_name, const std::string &a_data);

      public:
        CLuaState();
        
        virtual ~CLuaState();

        /**
        * This method is called when the state is activated
        */
        virtual void activate();

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate();

        /**
        * Return the state's ID
        */
        virtual enState getId();

        /**
        * This is a callback method that gets invoked when the window is resized
        * @param a_cDim the new dimension of the window
        */
        virtual void onResize(const irr::core::dimension2du &a_cDim);

        /**
         * Get the state of the mouse buttons. As the cursor control Irrlicht Object does not
         * report the state of the button I decided to hack it this way
         * @param a_iButton The mouse button
         */
        virtual bool isMouseDown(enMouseButton a_eButton);

        /**
        * Event handling method. The main class passes all Irrlicht events to this method
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent);

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run();
    };
  }
}
