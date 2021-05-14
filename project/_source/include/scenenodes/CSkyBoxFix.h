// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <string>

namespace dustbin {
  namespace scenenodes {
    /**
     * @class CSkyBoxFix
     * @author Christian Keimel
     * @brief This class fixes an issue with the skyboxes (border lines visible) and must be created before the scene is loaded. After loading the scene call "hideOriginalSkybox"
     */
    class CSkyBoxFix {
      private:
        enum enSkyboxState {
          enSbNoSkyBox,
          enSbSkyBox,
          enSbMaterial,
          enSbAttributes,
          enSbTexture1,
          enSbDone
        };

        bool m_bSkyBoxCreated;

        irr::scene::ISceneNode *m_pSkyBox;

      public:
        /**
         * Constructor
         * @param a_pDevice the active Irrlicht device
         * @param a_sIrrFile the irrlicht scene file to load
         */
        CSkyBoxFix(irr::video::IVideoDriver *a_pDrv, irr::scene::ISceneManager *a_pSmgr, irr::io::IFileSystem *a_pFs, const std::string &l_sIrrFile);

        /**
         * Hide the skybox loaded by the Irrlicht scene manager
         * @param a_pInput call this with the root scene node
         */
        void hideOriginalSkybox(irr::scene::ISceneNode *a_pInput);
    };
  }
}
