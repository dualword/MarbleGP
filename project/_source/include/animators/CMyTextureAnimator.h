/// (w) 2024 by Dustbin::Games - This file is licensed under the terms of the ZLib License (like Irrlicht)
#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <tuple>

namespace dustbin {
  namespace animators {
    const int g_TextureAnimatorId = MAKE_IRR_ID('t', 'x', 'a', 'n');

    /**
    * @class CMyTextureAnimator
    * @author Christian Keimel
    * This animator animates a texture using a list of textures
    */
    class CMyTextureAnimator : public irr::scene::ISceneNodeAnimator {
      private:
        std::vector<std::tuple<std::string, irr::video::ITexture *, irr::s32>>           m_vTextures;    /**< The list of textures (0 == filename, 1 == texture, 2 == time to display) */
        std::vector<std::tuple<std::string, irr::video::ITexture *, irr::s32>>::iterator m_itTexture;    /**< The current texture */

        int m_iTexture;   /**< Index of the texture to modify */
        int m_iLastTick;  /**< The last animation time */

        irr::scene::ISceneNode *m_pNode;  /**< The animated node */

        irr::video::IVideoDriver *m_pDrv;   /**< The video driver */

      public:
        CMyTextureAnimator(irr::scene::ISceneNode *a_pNode, irr::video::IVideoDriver *a_pDrv);
        virtual ~CMyTextureAnimator();

        virtual void animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 a_iTimeMs) override;

        virtual irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pNewManager = nullptr) override;

        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getType() const override;

        virtual bool hasFinished() const override;

        virtual bool isEventReceiverEnabled() const override;

        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions = nullptr) override;
        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions = nullptr) const override;
    };
  }
}
