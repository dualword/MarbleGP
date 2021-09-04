// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <_generated/lua/lua_tables.h>
#include <gameclasses/SPlayer.h>
#include <gfx/SViewPort.h>
#include <state/IState.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    class CControllerMenu;
  }

  class CGlobal;

  namespace state {
    /**
    * @class CErrorState
    * @author Christian Keimel
    * This is the state that show the error messages
    */
    class CGameState : public IState {
      private:
        irr::scene::ISceneManager* m_pSgmr;
        irr::video::IVideoDriver * m_pDrv;
        irr::gui::IGUIEnvironment* m_pGui;
        irr::io::IFileSystem     * m_pFs;

        CGlobal* m_pGlobal;

        SChampionShip m_cChampionship;  /**< This record contains information about the game setup, e.g. players and viewports */

        std::vector<gameclasses::SPlayer> m_vPlayers; /**< The players of the game*/

        std::map<int, gfx::SViewPort> m_mViewports;  /**< The viewports of the game */

        irr::core::recti m_cScreen; /**< The viewport covering the while screen */

        /**
        * Find a single scene node by it's type
        * @param a_eType the type to search for
        * @param a_pParent the parent node to search
        */
        irr::scene::ISceneNode* findSceneNodeByType(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pParent);

      public:
        CGameState();
        virtual ~CGameState();

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