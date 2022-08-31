// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <_generated/messages/IGameHUD.h>
#include <irrlicht.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace gameclasses {
    struct SPlayer;
  }

  namespace scenenodes {
    class CMyBillboard;       /**< Forward declaration of the modified billboard */
    class CMyBillboardText;   /**< Forward declaration of the modified text billboard */
  }

  namespace gui {
    class CRankingElement;  /**< Forward declaration of the ranking element */

    /**
    * @class CGameHUD
    * @author Christian Keimel
    * This is the UI element for the in-game HUD rendering
    * all necessary information for the player
    */
    class CGameHUD : public irr::gui::IGUIElement, public messages::IGameHUD {
      private:
        /**
        * @class STextElement
        * @author Christian Keimel
        * This structure collects all information
        * about the textual elements of the HUD
        */
        struct STextElement {
          irr::core::recti          m_cThisRect;    /**< Screen rect of the element */
          std::wstring              m_sText;        /**< Text of the element */
          irr::gui::IGUIFont       *m_pFont;        /**< Font of the text element */
          irr::video::SColor        m_cBackground;  /**< The background color*/
          irr::video::SColor        m_cTextColor;   /**< The text color */
          irr::video::IVideoDriver *m_pDrv;         /**< The video driver */
          irr::gui::EGUI_ALIGNMENT  m_eAlignV;      /**< Vertical alignment*/
          irr::gui::EGUI_ALIGNMENT  m_eAlignH;      /**< Horizontal alignment */
          bool                      m_bVisible;     /**< Is this text element visible? */

          STextElement(const irr::core::recti &a_cRect, const std::wstring &a_sText, irr::gui::IGUIFont *a_pFont, const irr::video::SColor &a_cBackground, const irr::video::SColor &a_cTextColor, irr::video::IVideoDriver *a_pDrv);

          STextElement();

          void render();
          void setPosition(const irr::core::position2di &a_cPos);
        };

        /**
        * @class SHighLight
        * @author Christian Keimel
        * This data struct holds all data
        * necessary for highlighting the
        * players that are also shown in
        * the ranking display
        */
        struct SHighLight {
          int  m_iMarbleId;     /**< ID of the highlighted marble */
          int  m_iPosition;     /**< Position of the marble */
          bool m_bVisible;      /**< Visibility flag */
          bool m_bFinished;     /**< Has this player finished? */
          bool m_bViewport;     /**< Visible in the current viewport? */

          scenenodes::CMyBillboard     *m_pArrow;    /**< The arrow billboard node */
          scenenodes::CMyBillboardText *m_pPosition; /**< The position text node */

          SHighLight();
        };

        /**
        * An enum with all text elements used
        */
        enum class enTextElements {
          Name,       /**< The player's name */
          LapHead,    /**< Header of the lap distplay */
          Lap,        /**< The lap display */
          PosHead,    /**< Header of the position display */
          Pos,        /**< The position display */
          Ahead,      /**< The player ahead */
          Behind,     /**< The player behind */
          TimeAhead,  /**< The time difference to the marble in from */
          TimeBehind  /**< The time difference to the marble behind */
        };

        /**
        * Struct for the laptimes
        */
        typedef struct SLapTime {
          int m_iStart;   /**< Start time of the lap */
          int m_iEnd;     /**< End time of the lap */
          int m_iLapNo;   /**< The lap */
          int m_iLapTime; /**< The lap time */

          std::vector<int> m_vSplitTimes;   /**< The split times of the lap */

          SLapTime();
        } SLapTime;

        /**
        * Struct for a player's race time
        */
        typedef struct SPlayerRacetime {
          int m_iFastest;     /**< The player's fastest lap */
          int m_iLastSplit;   /**< Time of the last split */

          std::vector<SLapTime> m_vLapTimes;

          std::vector<int> m_vPlayerBestSplit;    /**< The best split times of the player */

          SPlayerRacetime();
        } SPlayerRacetime;

        int                        m_iMarble;       /**< The marble ID of the player */
        int                        m_iLapCnt;       /**< The number of laps */
        int                        m_iPlayers;      /**< The number of players for the ranking */
        int                        m_iCtrlHeight;   /**< Height of the control display */
        irr::f32                   m_fVel;          /**< Speed of the marble of this HUD */
        irr::f32                   m_fThrottle;     /**< The throttle setting of the marble */
        irr::f32                   m_fSteer;        /**< The steer setting of the marble */
        bool                       m_bShowSpeed;    /**< Is the speed meter visible? */
        bool                       m_bBrake;        /**< Is the brake of the marble active? */
        bool                       m_bHightlight;   /**< From the settings: highlight leader and marble ahead */
        bool                       m_bShowCtrl;     /**< From the settings: show marble controls */
        bool                       m_aRostrum[16];  /**< Flags for all player that are on the rostrum */
        bool                       m_bShowRanking;  /**< From the settings: show ranking */
        bool                       m_bFadeStart;    /**< Are we currently fading the start screen? */
        bool                       m_aFinished[16]; /**< Finished players */
        bool                       m_bPaused;       /**< Is the game paused? */
        irr::core::recti           m_cRect;         /**< The total rect of the viewport */
        irr::gui::IGUIEnvironment *m_pGui;
        gameclasses::SPlayer      *m_pPlayer;       /**< This is the data of the player this HUD belongs to */
        irr::core::dimension2du    m_cDefSize;      /**< Size of the deficit display */
        irr::gui::IGUIFont        *m_pDefFont;      /**< Font of the deficit display */
        irr::gui::IGUIFont        *m_pTimeFont;     /**< Font for laptimes and ranking */
        irr::video::IVideoDriver  *m_pDrv;          /**< The Irrlicht video driver */
        irr::core::dimension2du    m_cScreen;       /**< The screen size */
        irr::gui::IGUIFont        *m_pSpeedFont;    /**< Font for the speed text */
        irr::core::dimension2du    m_cSpeedTotal;   /**< Total size of the speed meter */
        irr::core::dimension2du    m_cSpeedText;    /**< Size of the speed text */
        irr::core::position2di     m_cSpeedOffset;  /**< Offset of the speed bar */
        irr::core::dimension2du    m_cSpeedBar;     /**< Size of the speed bar */
        irr::core::vector3df       m_cUpVector;     /**< The marble's up vector */
        CRankingElement            *m_aRanking[16];  /**< The GUI elements for the ranking */
        irr::gui::IGUITab          *m_pRankParent;   /**< The parent for the ranking display */
        irr::video::ITexture       *m_pCountDown[5]; /**< The countdown textures */
        irr::video::ITexture       *m_pPaused;       /**< The paused texture */
        int                        m_iCountDown;    /**< The current countdown state (4..0) */
        irr::f32                   m_fCdAlpha;      /**< Alpha channel of the countdown */
        int                        m_iGoStep;       /**< The step when the countdown reached 0 */
        int                        m_iFinishStep;   /**< The simulation step when the player has finished */
        int                        m_iStep;         /**< The current simulation step */
        irr::core::recti           m_cCountDown;    /**< The rect to draw the countdown */
        irr::core::recti           m_cCntSource;    /**< The source rect for drawning the countdow */
        irr::video::ITexture      *m_pStunned;      /**< The "Stunned" image */
        irr::video::ITexture      *m_pRespawn;      /**< The "Respawn" image */
        irr::video::ITexture      *m_pFinished;     /**< The "Finished" image */
        bool                       m_bRespawn;      /**< Is the marble this HUD belongs to currently respawning? */
        bool                       m_bStunned;      /**< Is the marble this HUD belongs to currently stunned? */
        bool                       m_bFinished;     /**< Has the marble this HUD belongs to finished the race ? */
        int                        m_iFadeStart;    /**< The step when countdown 1 was received (and fading the grid starts) */
        int                        m_iFinished;     /**< Finished position of the player */
        irr::core::recti           m_cLaurelLft;    /**< Recti for the left laurel wreath */
        irr::core::recti           m_cLaurelRgt;    /**< Recti for the right laurel wreath */
        irr::core::recti           m_cLaurelSrc;    /**< Source rect for rendering the laurel wreaths */
        irr::video::ITexture      *m_pLaurel[4];    /**< The laurel wreath images */
        irr::gui::IGUIFont        *m_pPosFont;      /**< Font for showing the finish position */
        irr::gui::IGUIStaticText  *m_pWithdraw;     /**< The "Confirm Withdraw" static text */
        int                        m_iWithdraw;     /**< The step when the "confirm withdraw" will be hidden again */
        irr::video::SColor         m_cRankBack;     /**< The background color for the ranking */
        SHighLight                 m_aHiLight[3];   /**< Data for the highlighted marbles */
        irr::scene::ISceneManager *m_pSmgr;         /**< The Irrlicht scene manager */

        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< The Irrlicht scene collision manager */

        std::map<enTextElements, STextElement> m_mTextElements;   /**< The text elements of the HUD */

        std::vector<gameclasses::SPlayer *> *m_vRanking;

        irr::core::dimension2du getDimension(const std::wstring &s, irr::gui::IGUIFont *a_pFont);

        std::map<int, irr::core::vector3df> m_mMarblePositions;

        std::map<int, SPlayerRacetime> m_mLapTimes;       /**< The lap times of the marbles (key == marble id, value == lap time structure vector) */
        std::vector<int>               m_vBestSplits;     /**< The best split times */
        int                            m_iBestLapTime;    /**< The best lap time */
        irr::core::position2di         m_cLapTimePos;     /**< Position of the lap times */
        irr::core::dimension2du        m_cLapTotalDim;    /**< Size of the lap times labels */
        irr::core::dimension2du        m_cLapNoDim;       /**< Size of the lap number of the lap times display */
        irr::core::dimension2du        m_cPosNameDim;     /**< Size of the position display (upper left) */
        irr::core::dimension2du        m_cStartNr;        /**< Size of the starting number */
        irr::s32                       m_iLapTimeOffset;  /**< Vertical offset between the lap time labels */
        bool                           m_bShowLapTimes;   /**< Draw lap times? */
        irr::video::ITexture          *m_pCheckered;      /**< The checkered flag texture */

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
    };
  }
}
