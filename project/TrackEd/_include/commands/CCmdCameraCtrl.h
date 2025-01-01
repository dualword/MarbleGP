// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    /**
    * @class CCmdCameraCtrl
    * @author Christian Keimel
    * This command controls the camera
    */
    class CCmdCameraCtrl : public ICommand {
      private:
        irr::scene::ICameraSceneNode *m_pCamera;  /**< The camera */
        irr::scene::ISceneManager    *m_pSmgr;    /**< The Irrlicht scene manager */
        irr::gui::IGUIEnvironment    *m_pGui;     /**< The Irrlicht GUI environment */
        
        bool m_bMouseDown[3];      /**< Is a mouse button pressed down (0 == left, 1 == right, 2 == middle) */

        irr::core::position2di m_cPosition;     /**< The mouse position */
        irr::core::vector3df   m_cCamPos;       /**< The camera position */

        irr::f64 m_fAngleH;     /**< The horizontal angle */
        irr::f64 m_fAngleV;     /**< The vertical angle */

        void updateCameraTarget();

      public:
        CCmdCameraCtrl(irr::scene::ISceneManager* a_pSmgr, irr::gui::IGUIEnvironment *a_pGui);
        virtual ~CCmdCameraCtrl();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * The Irrlicht event handler method. Implement it if the command
        * needs to react to user input
        */
        virtual bool OnEvent(const irr::SEvent &a_cEvent) override;
    };
  }
}
