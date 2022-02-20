// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#define GLS_VERSION "#version 130\n"

namespace dustbin {
  namespace shader {

    class CShaderHandlerBase {
      protected:
        irr::IrrlichtDevice       *m_pDevice;
        irr::scene::ISceneManager *m_pSmgr;
        irr::video::IVideoDriver  *m_pDrv; 
        irr::core::vector3df       m_cLightPos,
                                   m_cLightTgt;
        irr::f32                   m_fLightFOV;

        bool findLightCamera(irr::scene::ISceneNode *a_pNode);

      public:
        CShaderHandlerBase(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_vViewportSize);
        virtual ~CShaderHandlerBase();

        virtual void initialize();
        virtual void beginScene();
        virtual void renderScene(const irr::core::recti &a_cViewPort) = 0;
        virtual void removeNode(irr::scene::ISceneNode *a_pNode) = 0;
        virtual void addNode(irr::scene::ISceneNode *a_pNode) = 0;
        virtual irr::video::E_MATERIAL_TYPE getMaterialType() = 0;

        void getLightData(irr::core::vector3df &a_cLightPos, irr::core::vector3df &a_cLightTarget, irr::f32 &a_fLightFOV);
        virtual void setLightData(const irr::core::vector3df &a_cLightPos, const irr::core::vector3df &a_cLightTarget, irr::f32 a_fLightFOV);
    };

  }
}
