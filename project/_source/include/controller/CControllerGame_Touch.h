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
        enum class enControls {
          Forward    = 0,
          Backward   = 1,
          Left       = 2,
          Right      = 3,
          Brake      = 4,
          Rearview   = 5,
          Respawn    = 6,
          NoThrottle = 7,
          NoSteer    = 8,

          Count      = 9
        };

        typedef struct STouchInput {
          irr::video::IVideoDriver *m_pDrv;   /**< The Irrlicht video driver */

          irr::video::ITexture *m_pOff;       /**< The image shown if the control is not active */
          irr::video::ITexture *m_pOn;        /**< The image shown if the control is active */

          irr::core::recti m_cScreen;         /**< The rectangle on the screen to draw the texture */
          irr::core::recti m_cTouch;          /**< The rectangle that can be touched for the control */
          irr::core::recti m_cSourceOff;      /**< The source rectangle for the in-active image */
          irr::core::recti m_cSourceOn;       /**< The source rectangle for the active image */

          bool m_bActive;                     /**< Flag to indicate whether the control is active or not */

          STouchInput();
          STouchInput(const std::string &a_sPathOff, const std::string &a_sPathOn, const irr::core::recti &a_cScreen, const irr::core::recti &a_cTouch, irr::video::IVideoDriver *a_pDrv);

          void draw(const irr::core::recti &a_cClip);
        }
        STouchInput;

        struct STouchState {
          int  m_iIndex;      /**< The touch index */

          enControls m_eControl;  /**< Which control is it activating? */

          STouchState();
        };

        irr::video::IVideoDriver *m_pDrv;       /**< The Irrlicht video driver */
        irr::core::recti          m_cViewport;  /**< The viewport for clipping*/

        STouchInput *m_aControls[(int)enControls::Count];   /**< Array with the controls */

        STouchState m_aTouch[5];    /**< The touch states */

        /**
        * Create the throttle controls
        * @param a_bLeft throttle controls on the left?
        */
        void createThrottleControls(bool a_bLeft);

        /**
        * Create the steering controls
        * @param a_bLeft throttle controls on the left?
        */
        void createSteeringControls(bool a_bLeft);

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

        virtual void draw() override;
    };
  }
}
