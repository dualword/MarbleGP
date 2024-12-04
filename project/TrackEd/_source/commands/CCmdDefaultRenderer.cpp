// (w) 2024 by Dustbin::Games / Christian Keimel
// This file is licensed under the terms of the ZLib license
// https://www.zlib.net/zlib_license.html
#include <commands/CCmdDefaultRenderer.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
        CCmdDefaultRenderer::CCmdDefaultRenderer(irr::IrrlichtDevice* a_pDevice) :
          ICommand("SceneRenderer"),
          m_pDrv  (a_pDevice->getVideoDriver   ()),
          m_pSmgr (a_pDevice->getSceneManager  ()),
          m_pGui  (a_pDevice->getGUIEnvironment())
        {
        }

        CCmdDefaultRenderer::~CCmdDefaultRenderer() {
        }

        /**
        * If this method returns "true" the command is finished and
        * will be removed from the vector of active commands
        * return true if the command is finished and can be removed
        */
        bool CCmdDefaultRenderer::isFinished() {
          return false;
        }

        /**
        * This method is called on every frame.
        */
        void CCmdDefaultRenderer::run() {
          m_pDrv->beginScene(true, true);
          m_pSmgr->drawAll();
          m_pGui->drawAll();

          tracked::IGlobal::getInstance()->beforeEndScene();

          m_pDrv->endScene();
        }
  }
}