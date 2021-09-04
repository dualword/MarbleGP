// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <shader/CShaderHandlerXEffect.h>

namespace dustbin {
  namespace shader {

    class CShaderHandleXEffectSplitscreen : public CShaderHandlerXEffect {
      private:
        irr::video::ITexture *m_pRtt;
        bool m_bRenderToTexture;

      public:
        CShaderHandleXEffectSplitscreen(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient);
        CShaderHandleXEffectSplitscreen(irr::IrrlichtDevice *a_pDevice, irr::core::dimension2du &a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient, irr::video::ITexture *a_pRtt);
        virtual ~CShaderHandleXEffectSplitscreen();

        virtual void renderScene(const irr::core::recti &a_cViewPort);
    };

  }
}