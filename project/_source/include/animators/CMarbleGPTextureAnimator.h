#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace animators {
    const int g_AnimatorId = MAKE_IRR_ID('m', 'g', 'p', 't');

    /**
    * @class CMarbleGPTextureAnimator
    * @author Christian Keimel
    * This is an animator for the in-game displays showing the race status.
    * For the editor it's just an empty implementation
    */
    class CMarbleGPTextureAnimator : public irr::scene::ISceneNodeAnimator {
      private:
        enum class enAnimatorType {
          Lapcount,
          Standings
        };

        enAnimatorType m_eType;   /**< Type of texture */

        irr::u32 m_iTexture;    /**< Index of the texture material */

        irr::scene::ISceneNode *m_pNode;    /**< The attached node */

      public:
        CMarbleGPTextureAnimator(irr::scene::ISceneNode *a_pNode);
        virtual ~CMarbleGPTextureAnimator();

        virtual irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pMgr) override;

        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getType() const override;

        virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timeMs) override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions=0) const override;

        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions=0) override;
    };
  }
}
