// (w) 2021 by Dustbin::Games / Christian Keimel
#include <shader/CShaderHandleXEffectSplitscreen.h>


namespace dustbin {
  namespace shader {

    CShaderHandleXEffectSplitscreen::CShaderHandleXEffectSplitscreen(irr::IrrlichtDevice* a_pDevice, irr::core::dimension2du& a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient) : CShaderHandlerXEffect(a_pDevice, a_cViewportSize, a_iShadowSize, a_iAmbient) {
      m_pRtt = m_pDrv->addRenderTargetTexture(a_cViewportSize, "XEffectRenderTarget");
      m_bRenderToTexture = false;
    }

    CShaderHandleXEffectSplitscreen::CShaderHandleXEffectSplitscreen(irr::IrrlichtDevice* a_pDevice, irr::core::dimension2du& a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient, irr::video::ITexture* a_pRtt) : CShaderHandlerXEffect(a_pDevice, a_cViewportSize, a_iShadowSize, a_iAmbient) {
      m_pRtt = a_pRtt;
      m_bRenderToTexture = true;
    }

    CShaderHandleXEffectSplitscreen::~CShaderHandleXEffectSplitscreen() {

    }

    void CShaderHandleXEffectSplitscreen::renderScene(const irr::core::recti& a_cViewPort) {
      m_pEffect->update(m_pRtt);

      if (!m_bRenderToTexture) {
        m_pDrv->setRenderTarget(0, false, false);
        m_pDrv->draw2DImage(m_pRtt, a_cViewPort.UpperLeftCorner);
      }
    }

  }
}