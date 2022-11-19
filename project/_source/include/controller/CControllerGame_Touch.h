// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerGame.h>
#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    const int g_TouchControlId = MAKE_IRR_ID('t', 'o', 'c', 'h');
    const irr::c8 g_TouchControlName[] = "TouchControl";

    /**
    * @class CControllerGame_Touch
    * @author Christian Keimel
    * This controller class handles Android touch
    * events and turns them into marble controls.
    * It provides both the controller and the GUi
    * element.
    */
    class CControllerGame_Touch : public IControllerGame, public irr::gui::IGUIElement {
      private:
        irr::f32 m_fSteer;    /**< The steering control */
        irr::f32 m_fThrottle; /**< The throttle control */

        bool m_bBrake;    /**< The brake control */
        bool m_bRear;     /**< The rearview control */
        bool m_bRespawn;  /**< The respawn control */
        bool m_bWithdraw; /**< The actual withdraw from race control. "withdrawFromRace()" just returns "true" once when the control is released. */
        bool m_bPause;    /**< The actual pause control. "pause()" just returns "true" once when the control is released. */

        bool m_bWithdrawPressed;  /**< Is the "withdraw from race" button pressed? */
        bool m_bPausePressed;     /**< Is the "pause" button pressed? */

      public:
        CControllerGame_Touch(IControllerGame::enType a_eType);
        virtual ~CControllerGame_Touch();

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
        * Implementation of the serialization method which does nothing in this case
        */
        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const override;

        /**
        * Implementation of the deserialization method which does nothing in this case
        */
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) override;

        /**
        * handle Irrlicht events
        * @param a_cEvent the Irrlicht event to handle
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;

        virtual void draw() override;
    };
  }
}
