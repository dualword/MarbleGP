// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerGame.h>
#include <irrlicht.h>
#include <string>
#include <tuple>
#include <map>

namespace dustbin {
  namespace controller {
    const int g_GyroControlId = MAKE_IRR_ID('g', 'y', 'c', 't');
    const irr::c8 g_GyroControlName[] = "GyroControl";

    /**
    * @class CControllerGame_Gyro
    * @author Christian Keimel
    * This controller class handles Android gyro
    * events and turns them into marble controls.
    * It provides both the controller and the GUi
    * element.
    */
    class CControllerGame_Gyro : public IControllerGame, public irr::gui::IGUIElement {
      private:
        irr::video::IVideoDriver *m_pDrv;   /**< The Irrlicht video driver */

        irr::video::ITexture *m_pGyroOne;       /**< The circle shown in the center */
        irr::video::ITexture *m_pGyroTwo;       /**< The filled circle showing the current controls */
        irr::video::ITexture *m_pReset;         /**< The "reset gyro" button icon */
        irr::video::ITexture *m_pRespawn [2];   /**< The "respawn" button icon */
        irr::video::ITexture *m_pRearview[2];   /**< The "rearview" button icon */

        irr::core::recti m_cIconSrc;        /**< The source rectangle for the icons */
        irr::core::recti m_cGyroOne;        /**< The draw rectangle for the gyro circle (the second icon is calculated) */
        irr::core::recti m_cReset[2];       /**< The draw rectangles for the "reset gyro" buttons */
        irr::core::recti m_cRespawn[2];     /**< The draw rectangles for the "respawn" buttons */
        irr::core::recti m_cRearview[2];    /**< The draw rectangles for the "rearview" buttons */
        irr::core::recti m_cViewport;       /**< The viewport to draw to */

        irr::f32 m_fGyroX;    /**< The X coordinate calculated from the gyro for steering */
        irr::f32 m_fGyroY;    /**< The X coordinate calculated from the gyro for throttle */

        irr::s32 m_iRespawn;    /**< Touch ID for the respawn button */
        irr::s32 m_iRearview;   /**< Touch ID for the rearview button */

        bool m_bSteerOnly;  /**< Only steering supported due to high AI help? */

      public:
        CControllerGame_Gyro(IControllerGame::enType a_eType, const irr::core::recti &a_cViewport, bool a_bSteerOnly);
        virtual ~CControllerGame_Gyro();

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

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() override;

        virtual void draw() override;
    };
  }
}
