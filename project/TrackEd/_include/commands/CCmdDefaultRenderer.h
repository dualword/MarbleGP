// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#pragma once

#include <commands/ICommand.h>

namespace dustbin {
  namespace commands {
    /**
    * @class CCmdDefaultRenderer
    * @author Christian Keimel
    * This command renders the scene using the Irrlicht default renderer
    */
    class CCmdDefaultRenderer : public ICommand {
      private:
        irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht video driver */
        irr::scene::ISceneManager *m_pSmgr;   /**< The Irrlicht scene manager */
        irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht GUI environment */

      public:
        CCmdDefaultRenderer(irr::IrrlichtDevice* a_pDevice);
        virtual ~CCmdDefaultRenderer();

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * @return true if the command is finished and can be removed
        */
        virtual bool isFinished() override;

        /**
        * This method is called on every frame.
        */
        virtual void run() override;
    };
  }
}
