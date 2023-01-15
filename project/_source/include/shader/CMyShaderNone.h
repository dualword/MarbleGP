// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <shader/CShaderHandlerBase.h>

namespace dustbin {
  namespace shader {

    class CShaderHandlerNone : public CShaderHandlerBase {
      private:
        irr::core::recti m_cMainViewport;

      public:
        CShaderHandlerNone(irr::IrrlichtDevice* a_pDevice, irr::core::dimension2du& a_cViewportSize);
        virtual ~CShaderHandlerNone();

        virtual void beginScene();
        virtual void renderScene(const irr::core::recti& a_cViewPort);
        virtual void removeNode(irr::scene::ISceneNode* a_pNode);
        virtual void addNode(irr::scene::ISceneNode* a_pNode);
        virtual irr::video::E_MATERIAL_TYPE getMaterialType();
    };

  }
}