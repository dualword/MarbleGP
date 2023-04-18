// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerGame.h>
#include <irrlicht.h>
#include <string>
#include <tuple>
#include <map>

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
        size_t m_iTouchThrottle;
        size_t m_iTouchSteer;
        size_t m_iTouchBrake;
        size_t m_iTouchRearview;
        size_t m_iTouchRespawn;

        irr::s32 m_iThrottleY1;   /**< The upper vertical limit of the neutral throttle */
        irr::s32 m_iThrottleY2;   /**< The lower vertical limit of the neutral throttle */
        irr::s32 m_iThrottleY3;   /**< If the throttle touch is below this Y value it also triggers the brake */
        irr::s32 m_iSteerX1;      /**< The left limit of the neutral steering */
        irr::s32 m_iSteerX2;      /**< The right limit of the neutral steering */

        irr::s32 m_iThrottleHeight;    /**< Height of the actual throttle from 0 to max */
        irr::s32 m_iSteeringWidth;     /**< Width of the actual steering from 0 to max */

        irr::f32 m_fThrottle;   /**< The throttle value */
        irr::f32 m_fSteering;   /**< The steering value */

        bool m_bBrkThrottle;    /**< Is the throttle touch in the braking area? */

        irr::video::IVideoDriver *m_pDrv;

        irr::video::ITexture *m_pSpeedNeutral[2];     /**< Texture for the neutral speed icon */
        irr::video::ITexture *m_pSpeedForward;        /**< Texture for the forward icon */
        irr::video::ITexture *m_pSpeedBackward;       /**< Texture for the backward icon */
        irr::video::ITexture *m_pSteerNeutral[2];     /**< Texture for the neutral steering icon */
        irr::video::ITexture *m_pSteerLeft;           /**< Texture for the steer left icon */
        irr::video::ITexture *m_pSteerRight;          /**< Texture for the steer right icon */
        irr::video::ITexture *m_pBrake[2];            /**< Texture for the brake icon */
        irr::video::ITexture *m_pRespawn[2];          /**< Texture for the respawn icon */
        irr::video::ITexture *m_pRearview[2];         /**< Texture for the rearview icon */

        irr::core::recti m_cViewport;
        irr::core::recti m_cSpeedNeutral[2];  /**< The output rectangles for the speed control "zero" position */
        irr::core::recti m_cBrake[2];         /**< The output rectangles for the brake control */
        irr::core::recti m_cRearView;         /**< The output rectangle for the rearview button */
        irr::core::recti m_cRespawn;          /**< The output rectangle for the respawn button */
        irr::core::recti m_cSteerNeutral;     /**< The output rectangle for the steering "zero" position */
        irr::core::recti m_cImage;            /**< The source rectangle for the images */
        irr::core::recti m_cTouchSpeed[2];    /**< The two speed touch rectangles */
        irr::core::recti m_cTouchSteer;       /**< The steering touch rectangle */
      public:
        CControllerGame_Touch(IControllerGame::enType a_eType, const irr::core::recti &a_cViewport);
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

        /**
        * The player has finished, hide the UI elements if necessary
        */
        virtual void playerFinished() override;

        virtual void draw() override;
    };
  }
}
