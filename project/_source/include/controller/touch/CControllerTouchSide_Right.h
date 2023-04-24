#pragma once

#include <controller/touch/CControllerTouchSide.h>

namespace dustbin {
  namespace controller {
    /**
    * @class CControllerTouchSide_Left
    * @author Christian Keimel
    * This is the implementation of the controller with the steering on the right and throttle on the left side
    */
    class CControllerTouchSide_Right : public CControllerTouchSide {
      protected:
        /**
        * Get the control that is touched
        * @param a_cTouch position of the touch event
        * @return the enum of the touched element, enControl::Count if no element was touched
        */
        virtual enControl getTouchControl(const irr::core::vector2di &a_cTouch) override;

      public:
        CControllerTouchSide_Right(irr::video::IVideoDriver *a_pDrv, const irr::core::recti &a_cViewport);
        virtual ~CControllerTouchSide_Right();
    };
  }
}
