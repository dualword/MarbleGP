// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <shader/CShaderHandlerBase.h>
#include <EffectHandler.h>

namespace dustbin {
  namespace shader {

    class CShaderHandlerXEffect : public CShaderHandlerBase {
      protected:
        EffectHandler* m_pEffect;
        irr::u32       m_iShadowSize,
                       m_iAmbient;

        void addToShadow(irr::scene::ISceneNode* a_pNode);
        void removeChildren(irr::scene::ISceneNode* a_pNode);

      public:
        CShaderHandlerXEffect(irr::IrrlichtDevice* a_pDevice, irr::core::dimension2du& a_cViewportSize, irr::u32 a_iShadowSize, irr::u32 a_iAmbient);
        virtual ~CShaderHandlerXEffect();

        virtual void initialize();
        virtual void renderScene(const irr::core::recti& a_cViewPort);
        virtual void removeNode(irr::scene::ISceneNode* a_pNode);
        virtual void addNode(irr::scene::ISceneNode* a_pNode);
        virtual irr::video::E_MATERIAL_TYPE getMaterialType();

        virtual void setLightData(const irr::core::vector3df& a_cLightPos, const irr::core::vector3df& a_cLightTarget, irr::f32 a_fLightFOV);
    };

  }
}