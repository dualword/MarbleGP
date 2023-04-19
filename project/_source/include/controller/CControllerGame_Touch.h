// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/IControllerGame.h>
#include <irrlicht.h>
#include <string>
#include <vector>
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
        typedef struct SControl {
          irr::core::recti      m_cDestination;     /**< The rectangle on the screen to render the item to */
          irr::core::recti      m_cSource;          /**< The source rectangle */
          irr::video::SColor    m_cBackground;      /**< The background color */
          irr::video::ITexture *m_pTextureOff;      /**< The image of the button drawn when the control is inactive */
          irr::video::ITexture *m_pTextureOn;       /**< The image of the button drawn when the control is active */
          bool                  m_bTouched;         /**< Is the control touched? */
          size_t                m_iTouchID;         /**< ID which is touching this control element, -1 if not touched */

          SControl();
          SControl(const irr::core::recti &a_cDestination, const irr::core::recti &a_cSource, const irr::video::SColor &a_cBackground, irr::video::ITexture *a_pOff, irr::video::ITexture *a_pOn);
        }
        SControl;

        enum class enControl {
          Neutral,
          Forward,
          Backward,
          Left,
          Right,
          Rearview,
          Respawn,

          Count
        };

        size_t m_iTouchSteer;

        irr::s32 m_iThrottleHeight;    /**< Height of the actual throttle from 0 to max */

        irr::core::recti m_cSteerRect;  /**< The rectangle with the steering controls */

        irr::core::vector2di m_cSteerPos;   /**< The position of the steering touch */

        irr::video::IVideoDriver *m_pDrv;

        irr::core::recti m_cViewport;

        std::map<enControl, SControl> m_mControls;

        void addToControlMap(enControl a_eControl, const irr::core::recti &a_cDestination, const irr::video::SColor &a_cColor, const std::string &a_sOff, const std::string &a_sOn);

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
