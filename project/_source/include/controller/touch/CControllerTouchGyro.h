// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <controller/touch/CControllerTouchSteerOnly.h>
#include <irrlicht.h>
#include <string>
#include <map>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchGyro
    * @author Christian Keimel
    * The controller using the gyroscope for steering
    */
    class CControllerTouchGyro : public CControllerTouchSteerOnly {
      private:
        irr::f32 m_fGyroX;

        irr::video::ITexture *m_pGyroOne;       /**< The circle shown in the center */
        irr::video::ITexture *m_pGyroTwo;       /**< The filled circle showing the current controls */

        irr::core::recti m_cIconSrc;        /**< The source rectangle for the icons */
        irr::core::recti m_cGyroOne;        /**< The draw rectangle for the gyro circle (the second icon is calculated) */

      public:
        CControllerTouchGyro(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~CControllerTouchGyro();

        /**
        * Returns the steer (-1 .. 1): left - right
        * @return the steer state
        */
        virtual irr::f32 getSteer() override;

        /**
        * Handle an event
        * @param a_cEvent the event to handle
        * @return true if the event was handled, false otherwise
        */
        virtual bool handleEvent(const irr::SEvent &a_cEvent) override;

        /**
        * Draw the controller
        */
        virtual void draw() override;
    };
  }
}
