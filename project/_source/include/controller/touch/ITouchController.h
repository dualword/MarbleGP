// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace controller {
    /**
    * @class ITouchController
    * @author Christian Keimel
    * This is the base class for all touch controllers. 
    * @see CControllerGame_Touch
    */
    class ITouchController {
      protected:
        typedef struct SControl {
          irr::core::recti      m_cDestination;     /**< The rectangle on the screen to render the item to */
          irr::core::recti      m_cSource;          /**< The source rectangle */
          irr::video::SColor    m_cBackground;      /**< The background color */
          irr::video::ITexture *m_pTextureOff;      /**< The image of the button drawn when the control is inactive */
          irr::video::ITexture *m_pTextureOn;       /**< The image of the button drawn when the control is active */
          bool                  m_bTouched;         /**< Is the control touched? */
          bool                  m_bActive;          /**< Is this item active? */
          size_t                m_iTouchID;         /**< ID which is touching this control element, -1 if not touched */

          SControl();
          SControl(const irr::core::recti &a_cDestination, const irr::core::recti &a_cSource, const irr::video::SColor &a_cBackground, irr::video::ITexture *a_pOff, irr::video::ITexture *a_pOn);
        }
        SControl;

        irr::video::IVideoDriver *m_pDrv;       /**< The Irrlicht video driver */
        irr::core::recti          m_cViewport;  /**< The viewport to render to */

      public:
        ITouchController(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~ITouchController();

        /**
        * Returns the throttle control (-1 .. 1): forward - backward
        * @return the throttle state
        */
        virtual irr::f32 getThrottle() = 0;

        /**
        * Returns the steer (-1 .. 1): left - right
        * @return the steer state
        */
        virtual irr::f32 getSteer() = 0;

        /**
        * Returns the state of the brake control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getBrake() = 0;

        /**
        * Return the rearview control
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRearView() = 0;

        /**
        * Get the respawn control state
        * @return true if control >= 0.5, false otherwise
        */
        virtual bool getRespawn() = 0;

        /**
        * Get the cancel button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool withdrawFromRace() = 0;

        /**
        * Get the pause button state
        * @return true if the button was pressed since the last query, false otherwise
        */
        virtual bool pause() = 0;

        /**
        * Handle an event
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleEvent(const irr::SEvent &a_cEvent) = 0;
        /**
        * Draw the controller
        */
        virtual void draw() = 0;
    };
  }
}
