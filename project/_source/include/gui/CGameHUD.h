// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/IGameHUD.h>
#include <gui/CGameHUDHelpers.h>
#include <irrlicht.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace gameclasses {
    struct SPlayer;
  }

  namespace controller {
    class IControllerAI;
  }

  namespace scenenodes {
    class CAiPathSceneNode;
  }

  namespace gui {
    class CRankingElement;  /**< Forward declaration of the ranking element */
    class CHudAiHelp;       /**< Forward declaration of the AI help icon display */
    class CHudSpeedBar;     /**< Forward declaration of the speed bar item */
    class CHudSteering;     /**< Forward declaration of the steering display item */
    class CHudBanner;       /**< Forward declaration of the banner renderer */
    class CHudLapTimes;     /**< Forward declaration of the lap time renderer */
    class CHudFade;         /**< Forward declaration of the HUD fade renderer */

    /**
    * @class CGameHUD
    * @author Christian Keimel
    * This is the UI element for the in-game HUD rendering
    * all necessary information for the player
    */
    class CGameHUD : public irr::gui::IGUIElement, public messages::IGameHUD {
      private:
        int                        m_iMarble;       /**< The marble ID of the player */
        int                        m_iLapCnt;       /**< The number of laps */
        int                        m_iPlayers;      /**< The number of players for the ranking */
        int                        m_iCtrlHeight;   /**< Height of the control display */
        irr::f32                   m_fVel;          /**< Speed of the marble of this HUD */
        irr::f32                   m_fThrottle;     /**< The throttle setting of the marble */
        irr::f32                   m_fSteer;        /**< The steer setting of the marble */
        bool                       m_bShowSpeed;    /**< Is the speed meter visible? */
        bool                       m_bBrake;        /**< Is the brake of the marble active? */
        bool                       m_bManRsp;       /**< Does the user request manual respawn? */
        bool                       m_bHightlight;   /**< From the settings: highlight leader and marble ahead */
        bool                       m_bShowCtrl;     /**< From the settings: show marble controls */
        bool                       m_aRostrum[16];  /**< Flags for all player that are on the rostrum */
        bool                       m_bShowRanking;  /**< From the settings: show ranking */
        bool                       m_bFadeStart;    /**< Are we currently fading the start screen? */
        bool                       m_aFinished[16]; /**< Finished players */
        bool                       m_bPaused;       /**< Is the game paused? */
        bool                       m_bCountdown;    /**< Is the countdown banner still visible? */
        irr::core::recti           m_cRect;         /**< The total rect of the viewport */
        irr::gui::IGUIEnvironment *m_pGui;
        gameclasses::SPlayer      *m_pPlayer;       /**< This is the data of the player this HUD belongs to */
        irr::core::dimension2du    m_cDefSize;      /**< Size of the deficit display */
        irr::gui::IGUIFont        *m_pDefFont;      /**< Font of the deficit display */
        irr::gui::IGUIFont        *m_pTimeFont;     /**< Font for laptimes and ranking */
        irr::video::IVideoDriver  *m_pDrv;          /**< The Irrlicht video driver */
        irr::core::dimension2du    m_cScreen;       /**< The screen size */
        irr::core::vector3df       m_cUpVector;     /**< The marble's up vector */
        irr::core::dimension2du    m_cLabelSize;    /**< Dimension of the speed bar, used for calculating some sizes */
        irr::core::vector3df       m_cPosition;     /**< The marble's position */
        CRankingElement           *m_aRanking[16];  /**< The GUI elements for the ranking */
        int                        m_iGoStep;       /**< The step when the countdown reached 0 */
        int                        m_iStep;         /**< The current simulation step */
        bool                       m_bRespawn;      /**< Is the marble this HUD belongs to currently respawning? */
        bool                       m_bStunned;      /**< Is the marble this HUD belongs to currently stunned? */
        bool                       m_bFinished;     /**< Has the marble this HUD belongs to finished the race ? */
        bool                       m_bRanking;      /**< Show the ranking list */
        int                        m_iFadeStart;    /**< The step when countdown 1 was received (and fading the grid starts) */
        int                        m_iFinished;     /**< Finished position of the player */
        irr::gui::IGUIFont        *m_pPosFont;      /**< Font for showing the finish position */
        int                        m_iWithdraw;     /**< Withdraw from race step */
        irr::video::SColor         m_cRankBack;     /**< The background color for the ranking */
        SHighLight                 m_aHiLight[3];   /**< Data for the highlighted marbles */
        irr::scene::ISceneManager *m_pSmgr;         /**< The Irrlicht scene manager */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< The Irrlicht scene collision manager */

        std::map<enTextElements, STextElement> m_mTextElements;   /**< The text elements of the HUD */

        std::vector<gameclasses::SPlayer *> *m_vRanking;

        irr::core::dimension2du getDimension(const std::wstring &s, irr::gui::IGUIFont *a_pFont);

        std::map<int, irr::core::vector3df> m_mMarblePositions;

        irr::core::dimension2du        m_cLapNoDim;       /**< Size of the lap number of the lap times display */
        irr::core::dimension2du        m_cPosNameDim;     /**< Size of the position display (upper left) */
        irr::core::dimension2du        m_cStartNr;        /**< Size of the starting number */
        irr::core::dimension2du        m_cCheckered;      /**< Size of the checkered flag texture */
        irr::s32                       m_iLapTimeOffset;  /**< Vertical offset between the lap time labels */
        bool                           m_bShowLapTimes;   /**< Draw lap times? */
        irr::video::ITexture          *m_pCheckered;      /**< The checkered flag texture */
        controller::IControllerAI     *m_pAiController;   /**< AI controller to show the user control hints */
        CHudAiHelp                    *m_pAiHelp;         /**< The AI help display */
        CHudSpeedBar                  *m_pSpeedBar;       /**< The speed bar */
        CHudSteering                  *m_pSteering;       /**< The steering display */
        CHudBanner                    *m_pBanner;         /**< The banner renderer */
        CHudLapTimes                  *m_pLapTimes;       /**< The lap times renderer */
        CHudFade                      *m_pFade;           /**< The HUD fade renderer */
        scenenodes::CAiPathSceneNode  *m_pAiNode;         /**< The AI path node */

        std::wstring getDeficitString(int a_iDeficit);

      protected:
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
        virtual void onCamerarespawn(irr::s32 a_MarbleId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Target) override;

        /**
        * This function receives messages of type "PlayerFinished"
        * @param a_MarbleId ID of the finished marble
        * @param a_RaceTime Racetime of the finished player in simulation steps
        * @param a_Laps The number of laps the player has done
        */
        virtual void onPlayerfinished(irr::s32 a_MarbleId, irr::u32 a_RaceTime, irr::s32 a_Laps) override;

        /**
        * This function receives messages of type "PlayerStunned"
        * @param a_MarbleId ID of the marble
        * @param a_State New stunned state (1 == Player stunned, 2 == Player recovered)
        */
        virtual void onPlayerstunned(irr::s32 a_MarbleId, irr::u8 a_State) override;

        /**
        * This function receives messages of type "RaceFinished"
        * @param a_Cancelled A flag indicating whether or not the race was cancelled by a player
        */
        virtual void onRacefinished(irr::u8 a_Cancelled) override;

        /**
        * This function receives messages of type "RacePosition"
        * @param a_MarbleId ID of the marble
        * @param a_Position Position of the marble
        * @param a_Laps The current lap of the marble
        * @param a_Deficit Deficit of the marble on the leader in steps
        */
        virtual void onRaceposition(irr::s32 a_MarbleId, irr::s32 a_Position, irr::s32 a_Laps, irr::s32 a_DeficitAhead, irr::s32 a_DeficitLeader) override;

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
        virtual void onMarblemoved(irr::s32 a_ObjectId, const irr::core::vector3df &a_Position, const irr::core::vector3df &a_Rotation, const irr::core::vector3df &a_LinearVelocity, irr::f32 a_AngularVelocity, const irr::core::vector3df &a_CameraPosition, const irr::core::vector3df &a_CameraUp, irr::s8 a_ControlX, irr::s8 a_ControlY, bool a_Contact, bool a_ControlBrake, bool a_ControlRearView, bool a_ControlRespawn) override;

        /**
        * This function receives messages of type "LapStart"
        * @param a_MarbleId ID of the marble
        * @param a_LapNo Number of the started lap
        */
        virtual void onLapstart(irr::s32 a_MarbleId, irr::s32 a_LapNo) override;

        /**
        * This function receives messages of type "PlayerRostrum"
        * @param a_MarbleId ID of the marble sent to the rostrum
        */
        virtual void onPlayerrostrum(irr::s32 a_MarbleId) override;

        /**
        * This function receives messages of type "Countdown"
        * @param a_Tick The countdown tick (4 == Ready, 3, 2, 1, 0 == Go)
        */
        virtual void onCountdown(irr::u8 a_Tick) override;

        /**
        * This function receives messages of type "StepMsg"
        * @param a_StepNo The current step number
        */
        virtual void onStepmsg(irr::u32 a_StepNo) override;

        /**
        * This function receives messages of type "ConfirmWithdraw"
        * @param a_MarbleId ID of the marble
        * @param a_Timeout The number of steps defining the timeout for the withdrawal
        */
        virtual void onConfirmwithdraw(irr::s32 a_MarbleId, irr::s32 a_Timeout) override;

        /**
        * This function receives messages of type "PauseChanged"
        * @param a_Paused The current paused state
        */
        virtual void onPausechanged(bool a_Paused) override;

        /**
        * This function receives messages of type "Checkpoint"
        * @param a_MarbleId ID of the marble
        * @param a_Checkpoint The checkpoint ID the player has passed
        */
        virtual void onCheckpoint(irr::s32 a_MarbleId, irr::s32 a_Checkpoint) override;

      private:
        /**
        * Render the ranking element below the speed bar
        * @param a_cPos the position to render to
        */
        void renderNearbyRanking(const irr::core::position2di &a_cPos, const irr::core::recti &a_cTotal);

        /**
        * Render the ranking shown in the upper left corner of the screen
        */
        void renderRanking();

      public:
        CGameHUD(gameclasses::SPlayer *a_pPlayer, const irr::core::recti &a_cRect, int a_iLapCnt, irr::gui::IGUIEnvironment *a_pGui, std::vector<gameclasses::SPlayer *> *a_vRanking);
        virtual ~CGameHUD();

        virtual void draw() override;

        void updateRanking();

        void setSettings(bool a_bHightlight, bool a_bShowCtrl, bool a_bShowRanking, bool a_bShowLapTimes);

        bool isResultParentVisible();
        void showResultParent();

        /**
        * This method is called to prepare for scene drawing,
        * i.e. show the necessary highlight nodes
        */
        void beforeDrawScene();

        /**
        * This method is called when the scene was drawn to
        * hide all highlight nodes of this HUD
        */
        void afterDrawScene();

        /**
        * This method is only used to tell the HUD that it shall display AI help
        * @param a_pController the controller
        */
        void setAiController(controller::IControllerAI *a_pController);

        /**
        * Update the AI help display of the HUD
        * @param a_bLeft does the marble steer left?
        * @param a_bRight does the marble steer right?
        * @param a_bForward does the marble accelerate?
        * @param a_bBackward does the marble decelerate?
        * @param a_bBrake is the marble braking?
        * @param a_bRespawn does the marble request manual respawn?
        * @param a_bAutomatic is the automatic control active?
        * @param a_cPoint1 the first point calculated by the AI
        * @param a_cPoint2 the second point calculated by the AI
        */
        void updateAiHelp(bool a_bLeft, bool a_bRight, bool a_bForward, bool a_bBackward, bool a_bBrake, bool a_bRespawn, bool a_bAutomatic, const irr::core::vector3df &a_cPoint1, const irr::core::vector3df &a_cPoint2);

        /**
        * Start the fade-out at the end of the race
        */
        void startFinalFadeOut();
    };
  }
}
