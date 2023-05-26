// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <_generated/messages/IGameState.h>
#include <threads/CMessageQueue.h>
#include <gameclasses/SPlayer.h>
#include <data/CDataStructs.h>
#include <gfx/SViewPort.h>
#include <state/IState.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    class ICustomEventReceiver;
    class CAiControlThread;
  }

  namespace gameclasses {
    class CDynamicThread;
  }

  namespace shader {
    class CShaderHandlerBase;
  }

  namespace scenenodes {
    class CCheckpointNode;
    class CMyCameraAnimator;
    class CDustbinCamera;
    class CRostrumNode;
    class CStartingGridSceneNode;
  }

  namespace sound {
    class ISoundInterface;
  }

  namespace network {
    class CGameClient;
    class CGameServer;
  }

  namespace lua {
    class CLuaScript_scene;
  }

  namespace gui {
#ifdef _TOUCH_CONTROL
    class IGuiMarbleControl;
#endif
  }

  class CGlobal;

  namespace state {
    /**
    * @class SGameViewports
    * @author Christian Keimel
    * This struct maps the number of local players
    * to the viewport distribution
    */
    struct SGameViewports {
      /**
      * Enumeration for the type of viewport
      */
      enum class enType {
        Player,   /**< A viewport for a player */
        Racedata, /**< A viewport for the race data, i.e. ranking, racetime, leader... */
        Logo      /**< A viewport for a logo, just to fill the empty space */
      };

      /**
      * @class SViewportDef
      * @author Christian Keimel
      * This struct holds information about how a viewport looks like
      */
      struct SViewportDef {
        int    m_iColumn;   /**< The column of the viewport*/
        int    m_iRow;      /**< The row of the viewport*/
        enType m_eType;     /**< The type of viewport */

        SViewportDef() : m_iColumn(0), m_iRow(0), m_eType(enType::Player) {
        }

        SViewportDef(int a_iColumn, int a_iRow, enType a_eType) : m_iColumn(a_iColumn), m_iRow(a_iRow), m_eType(a_eType) {
        }
      };

      /**
      * @class SViewportDistribution
      * @author Christian Keimel
      * This struct is responsible for distributing the viewports
      * across the window depending on the number of players
      */
      struct SViewportDistribution {
        int m_iColumns;   /**< The number of columns for this distribution */
        int m_iRows;      /**< The number of rows for this distribution */

        std::vector<SViewportDef> m_vViewports;  /**< The actual viewports for the players */

        SViewportDistribution() : m_iColumns(0), m_iRows(0) {
        }

        SViewportDistribution(int a_iColumns, int a_iRows) : m_iColumns(a_iColumns), m_iRows(a_iRows) {
        }
      };

      /**
      * This map hods the data for the distribution per number of local players
      * key == number of local players, value == actual distribution
      */
      std::map<int, SViewportDistribution> m_mDistribution;
    };

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

        enGameState m_eState;

        threads::CInputQueue * m_pInputQueue;
        threads::COutputQueue* m_pOutputQueue;

        int m_iStep,      /**< The current world step. 120 steps / second */
            m_iFinished,  /**< The game step when the race was finishes */
            m_iFadeOut;   /**< The game step when fading out starts */

        irr::f32 m_fSfxVolume;  /**< The volume of the in-game sound effects */

        bool m_bEnded;  /**< A message notifying that the race has ended was received (dynamics thread terminated) */

        sound::ISoundInterface *m_pSoundIntf;

        gameclasses::CDynamicThread *m_pDynamics;

        scenenodes::CRostrumNode *m_pRostrum;

        std::vector<gameclasses::SPlayer*> m_vPlayers;    /**< The players of the game */
        std::vector<gameclasses::SPlayer*> m_vPosition;   /**< The players' positions in the race */

        irr::video::ITexture* m_pCheckpointTextures[3];  /**< The checkpoint textures (0 == white, 1 == Flash 1, 2 == Flash 2) */

        std::vector<messages::IMessage*> m_vMoveMessages; /**< Move messages are not applied before the step has finished */
        std::map<int, gfx::SViewPort> m_mViewports;  /**< The viewports of the game */

        std::map<irr::s32, scenenodes::CCheckpointNode*> m_mCheckpoints;  /**< All checkpoints on this lap */

        irr::core::recti m_cScreen; /**< The viewport covering the while screen */

        gameclasses::SMarbleNodes *m_aMarbles[16]; /**< For direct access to the marbles */

        shader::CShaderHandlerBase* m_pShader;  /**< The shader to use */

        std::map<irr::s32, irr::scene::ISceneNode*> m_mMoving;  /**< All moving non-marble objects of the track */

        scenenodes::CMyCameraAnimator *m_pCamAnimator;   /**< Camera animator for the "view track" mode */

        irr::scene::ICameraSceneNode* m_pCamera;     /**< The camera for the "view track" mode */

        std::vector<scenenodes::CDustbinCamera *> m_vCameras;   /**< Static cameras for the "replay" and "view track" modes */

        controller::CAiControlThread *m_pAiThread;

        data::SChampionshipRace *m_pRace;

        network::CGameClient *m_pClient;    /**< The game client (if a network game is running) */
        network::CGameServer *m_pServer;    /**< The game server (if a network game is running) */

        SGameViewports m_cViewports;  /**< Viewport distribution */

        int m_iNumOfViewports;    /**< The number of necessary viewports, aka local players */

        data::SRacePlayers m_cPlayers;  /**< The player of the race */

        data::SGameData m_cGameData;  /**< The data of the current game */
        data::SSettings m_cSettings;  /**< The global settings */

        scenenodes::CStartingGridSceneNode *m_pGridNode;  /**< The starting grid scene node that holds all the available marbles */
        
        irr::f32 m_fGridAngle;    /**< The starting grid angle */

        irr::scene::ISceneNode *m_pAiNode;    /**< The scene node with the AI data */

        lua::CLuaScript_scene *m_pLuaScript;

#ifdef _TOUCH_CONTROL
        gui::IGuiMarbleControl *m_pTouchControl;
#endif

        /**
        * Fill the map of the moving non-marble objects
        * @param a_pNode the node to check
        * @see CGameState::m_mMoving
        */
        void fillMovingMap(irr::scene::ISceneNode* a_pNode);

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

        /**
        * Some error has happened, show to the user
        * @param a_sHeadline the headline of the error
        * @param a_sMessage the message of the error
        */
        void handleError(const std::string &a_sHeadline, const std::string &a_sMessage);

      protected:
        /**
         * This function receives messages of type "StepMsg"
         * @param a_StepNo The current step number
         */
        virtual void onStepmsg(irr::u32 a_StepNo) override;

        /**
         * This function receives messages of type "Countdown"
         * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
         */
        virtual void onCountdown(irr::u8 a_Tick) override;

        /**
         * This function receives messages of type "ObjectMoved"
         * @param a_ObjectId The ID of the object
         * @param a_Position The current position
         * @param a_Rotation The current rotation (Euler angles)
         * @param a_LinearVelocity The linear velocity
         * @param a_AngularVelocity The angualar (rotation) velocity
         */
        virtual void onObjectmoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity) override;
     
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
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Rotation, const irr::core::vector3df& a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df& a_CameraPosition, const irr::core::vector3df& a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) override;
        
        /**
         * This function receives messages of type "Trigger"
         * @param a_TriggerId ID of the trigger
         * @param a_ObjectId ID of the marble that caused the trigger
         */
        virtual void onTrigger(irr::s32 a_TriggerId, irr::s32 a_ObjectId) override;

        /**
        * This function receives messages of type "PlayerAssignMarble"
        * @param a_playerid The ID of the player
        * @param a_marbleid The ID of the marble for the player
        */
        virtual void onPlayerassignmarble(irr::s32 a_playerid, irr::s32 a_marbleid) override;

        /**
        * This function receives messages of type "RaceSetupDone"
        */
        virtual void onRacesetupdone() override;

        /**
         * This function receives messages of type "PlayerRespawn"
         * @param a_MarbleId ID of the marble
         * @param a_State New respawn state (1 == Respawn Start, 2 == Respawn Done). Between State 1 and 2 a CameraRespawn is sent
         */
        virtual void onPlayerrespawn(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
         * This function receives messages of type "CameraRespawn"
         * @param a_MarbleId The ID of the marble which is respawning
         * @param a_Position The new position of the camera
         * @param a_Target The new target of the camera, i.e. the future position of the marble
         */
        virtual void onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df& a_Position, const irr::core::vector3df& a_Target) override;

        /**
         * This function receives messages of type "PlayerStunned"
         * @param a_MarbleId ID of the marble
         * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
         */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
         * This function receives messages of type "PlayerFinished"
         * @param a_MarbleId ID of the finished marble
         * @param a_RaceTime Racetime of the finished player in simulation steps
         * @param a_Laps The number of laps the player has done
         */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) override;

        /**
         * This function receives messages of type "RaceFinished"
         * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
         */
        virtual void onRacefinished(irr::u8 a_Cancelled) override;

        /**
         * This function receives messages of type "Checkpoint"
         * @param a_MarbleId ID of the marble
         * @param a_Checkpoint The checkpoint ID the player has passed
         */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) override;

        /**
         * This function receives messages of type "LapStart"
         * @param a_MarbleId ID of the marble
         * @param a_LapNo Number of the started lap
         */
        virtual void onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) override;

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_Deficit Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) override;

        /**
        * This function receives messages of type "PlayerWithdrawn"
        * @param a_MarbleId ID of the marble
        */
        virtual void onPlayerwithdrawn(irr::s32 a_MarbleId) override;

        /**
        * This function receives messages of type "ServerDisconnect"
        */
        virtual void onServerdisconnect() override;

        /**
        * This function receives messages of type "RaceResult"
        * @param a_data Encoded SRacePlayer structure
        */
        virtual void onRaceresult(const std::string &a_data) override;

        /**
        * This function receives messages of type "EndRaceState"
        */
        virtual void onEndracestate() override;

        /**
        * This function receives messages of type "PlayerRemoved"
        * @param a_playerid ID of the removed player
        */
        virtual void onPlayerremoved(irr::s32 a_playerid) override;

        /**
        * This function receives messages of type "PauseChanged"
        * @param a_Paused The current paused state
        */
        virtual void onPausechanged(bool a_Paused) override;

        /**
        * This function receives messages of type "LuaMessage"
        * @param a_NumberOne First number for any information
        * @param a_NumberTwo Other number for any information
        * @param a_Data String for any further information
        */
        virtual void onLuamessage(irr::s32 a_NumberOne, irr::s32 a_NumberTwo, const std::string &a_Data) override;

        void addStaticCameras(irr::scene::ISceneNode *a_pNode);

        /**
        * Assign a viewport to a player
        * @param a_fAngle the grid angle read from the grid scene node
        * @param a_pPlayer the player to assign the viewport to
        */
        void assignViewport(irr::f32 a_fAngle, gameclasses::SPlayer *a_pPlayer);

        /**
        * Prepare the shader for the game
        */
        void prepareShader();

        /**
        * Hide the AI node unless defined as visible in the settings
        */
        void hideAiNode();

      public:
        CGameState(irr::IrrlichtDevice *a_pDevice, CGlobal *a_pGlobal);
        virtual ~CGameState();

        /**
         * This method is called when the state is activated
         */
        virtual void activate() override;

        /**
        * This method is called when the state is deactivated
        */
        virtual void deactivate() override;

        /**
         * Return the state's ID
         */
        virtual enState getId() override;

        /**
         * Event handling method. The main class passes all Irrlicht events to this method
         */
        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;

        /**
        * This method is always called. Here the state has to perform it's actual work
        * @return enState::None for running without state change, any other value will switch to the state
        */
        virtual enState run() override;
    };
  } // namespace state
} // namespace dustbin