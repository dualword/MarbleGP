// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/CControllerBase.h>
#include <controller/IControllerGame.h>
#include <vector>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerGame
    * @author Christian Keimel
    * This class handles handles controls for the actual game
    */
    class CControllerGame : public CControllerBase, public IControllerGame {
      private:
        bool m_bWithdrawDown;   /**< Cancel button pressed? */
        bool m_bPauseDown;      /**< Pause button pressed? */

        void setupControls();

        /**
        * Create the control string for the hints for a single control
        * @param a_cCtrl the control
        * @return a string with the description of the control
        */
        std::wstring createSingleControlString(SCtrlInput &a_cCtrl);

        /**
        * Create the control string for the hints for two linked controls
        * @param a_cCtrl1 the first control
        * @param a_cCtrl2 the second control
        * @return a string with the description of the controls
        */
        std::wstring createDoubleControlString(SCtrlInput &a_cCtrl1, SCtrlInput &a_cCtrl2);

      public:
        CControllerGame();
        virtual ~CControllerGame();

        /**
        * Returns the throttle control (-1 .. 1): forward - backward
        * @return the throttle state
        */
        virtual irr::f32 getThrottle() override;

        /**
        * Returns the steer (-1 .. 1): right - left
        * @return the steer state
        */
        virtual irr::f32 getSteer() override;

        /**
        * Returns the state of the brake control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getBrake() override;

        /**
         * Return the rearview control
         * @return true if control >= 0.5, false otherwise
         */
        virtual bool getRearView() override;

        /**
        * Get the respawn control state
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRespawn() override;

        /**
        * Get the "withdraw from race" button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool withdrawFromRace() override;

        /**
        * Get the pause button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool pause() override;

        /**
        * Process Irrlicht evnts to update the controls
        * @param a_cEvent event to process
        */
        virtual void updateControls(const irr::SEvent &a_cEvent) override;

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() override;

        /**
         * If this controller has an UI this method will move it to the front.
         * The Android touch and gyroscope controllers have an UI
         */
        virtual void moveGuiToFront() override;

        /**
        * Get the text shown in the tutorial
        * @param a_bFirst true if this is the first help point (controls), false if it's the fourth (respawn)
        * @return the text shown in the tutorial
        */
        virtual std::wstring getTutorialText(bool a_bFirst) override;
    };

  } // namespace controller
} // namespace dustbin