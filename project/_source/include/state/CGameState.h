// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <_generated/messages/IGameState.h>
#include <_generated/lua/lua_tables.h>
#include <threads/CMessageQueue.h>
#include <gameclasses/SPlayer.h>
#include <gfx/SViewPort.h>
#include <state/IState.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    class CControllerMenu;
  }

  namespace gameclasses {
    class CDynamicThread;
  }

  namespace shader {
    class CShaderHandlerBase;
  }

  namespace scenenodes {
    class CCheckpointNode;
  }

  class CGlobal;

  namespace state {
    /**
    * @class CErrorState
    * @author Christian Keimel
    * This is the state that show the error messages
    */
    class CGameState : public IState, public messages::IGameState {
      private:
        enum class enGameState {
          Countdown,
          Racing,
          Finished
        };

        irr::scene::ISceneManager* m_pSgmr;
        irr::video::IVideoDriver * m_pDrv;
        irr::gui::IGUIEnvironment* m_pGui;
        irr::io::IFileSystem     * m_pFs;

        threads::CInputQueue * m_pInputQueue;
        threads::COutputQueue* m_pOutputQueue;

        int m_iStep;

        enGameState m_eState;

        CGlobal* m_pGlobal;
        
        gameclasses::CDynamicThread* m_pDynamics;

        SChampionShip m_cChampionship;  /**< This record contains information about the game setup, e.g. players and viewports */

        std::vector<gameclasses::SPlayer*> m_vPlayers; /**< The players of the game*/

        irr::video::ITexture* m_pCheckpointTextures[3];  /**< The checkpoint textures (0 == white, 1 == Flash 1, 2 == Flash 2) */

        std::vector<messages::IMessage*> m_vMoveMessages; /**< Move messages are not applied before the step has finished */
        std::map<int, gfx::SViewPort> m_mViewports;  /**< The viewports of the game */

        std::map<irr::s32, scenenodes::CCheckpointNode*> m_mCheckpoints;  /**< All checkpoints on this lap */

        irr::core::recti m_cScreen; /**< The viewport covering the while screen */

        gameclasses::SMarbleNodes *m_aMarbles[16]; /**< For direct access to the marbles */

        shader::CShaderHandlerBase* m_pShader;  /**< The shader to use */

        /**
        * Find a single scene node by it's type
        * @param a_eType the type to search for
        * @param a_pParent the parent node to search
        */
        irr::scene::ISceneNode* findSceneNodeByType(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pParent);

        /**
        * Fill the "Checkpoint" list
        * @param a_pParent the parent node to search
        */
        void fillCheckpointList(irr::scene::ISceneNode* a_pParent);

        /**
        * This method prepares the scene before a viewport is rendered
        * @param a_pViewport the viewport that will be rendered
        */
        void beforeDrawScene(gfx::SViewPort* a_pViewport);

        /**
        * This method resets the scene after a viewport was rendered
        * @param a_pViewport the viewport that was rendered
        */
        void afterDrawScene(gfx::SViewPort* a_pViewPort);

      protected:
        /**
         * This function receives messages of type "StepMsg"
         * @param a_StepNo The current step number
         */
        virtual void onStepmsg(irr::u32 a_StepNo);

        /**
         * This function receives messages of type "Countdown"
         * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
         */
        virtual void onCountdown(irr::u8 a_Tick);

        /**
         * This function receives messages of type "ObjectMoved"
         * @param a_ObjectId The ID of the object
         * @param a_Position The current position
         * @param a_Rotation The current rotation (Euler angles)
         * @param a_LinearVelocity The linear velocity
         * @param a_AngularVelocity The angualar (rotation) velocity
         */
        virtual void onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity);
     
        /**
         * This function receives messages of type "MarbleMoved"
         * @param a_ObjectId The ID of the object
         * @param a_Position The current position
         * @param a_Rotation The current rotation (Euler angles)
         * @param a_LinearVelocity The linear velocity
         * @param a_AngularVelocity The angualar (rotation) velocity
         * @param a_CameraPosition The position of the camera
         * @param a_CameraUp The Up-Vector of the camera
         * @param a_ControlX The marble's current controller state in X-Direction
         * @param a_ControlY The marble's current controller state in Y-Direction
         * @param a_Contact A Flag indicating whether or not the marble is in contact with another object
         * @param a_ControlBrake Flag indicating whether or not the marble's brake is active
         * @param a_ControlRearView Flag indicating whether or not the marble's player looks behind
         * @param a_ControlRespawn Flag indicating whether or not the manual respawn button is pressed
         */
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn);
        
        /**
         * This function receives messages of type "Trigger"
         * @param a_TriggerId ID of the trigger
         * @param a_ObjectId ID of the marble that caused the trigger
         */
        virtual void onTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId);

        /**
         * This function receives messages of type "PlayerRespawn"
         * @param a_MarbleId ID of the marble
         * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
         */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State);

        /**
         * This function receives messages of type "CameraRespawn"
         * @param a_MarbleId The ID of the marble which is respawning
         * @param a_Position The new position of the camera
         * @param a_Target The new target of the camera, i.e. the future position of the marble
         */
        virtual void onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target);

        /**
         * This function receives messages of type "PlayerStunned"
         * @param a_MarbleId ID of the marble
         * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
         */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State);

        /**
         * This function receives messages of type "PlayerFinished"
         * @param a_MarbleId ID of the finished marble
         * @param a_RaceTime Racetime of the finished player in simulation steps
         * @param a_Laps The number of laps the player has done
         */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps);

        /**
         * This function receives messages of type "RaceFinished"
         * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
         */
        virtual void onRacefinished(irr::u8 a_Cancelled);

        /**
         * This function receives messages of type "Checkpoint"
         * @param a_MarbleId ID of the marble
         * @param a_Checkpoint The checkpoint ID the player has passed
         */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint);

        /**
         * This function receives messages of type "LapStart"
         * @param a_MarbleId ID of the marble
         * @param a_LapNo Number of the started lap
         */
        virtual void onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo);

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