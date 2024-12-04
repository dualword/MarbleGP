// (w) 2024 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

namespace dustbin {
  namespace animators {
    const int g_PlaneAnimatorId = MAKE_IRR_ID('p', 'l', 'n', 'e');

    /**
    * @class CPlaneAnimator
    * @author Christian Keimel
    * This animator moves the object it is attached to so it stays centered below the camera
    */
    class CPlaneAnimator : public irr::scene::ISceneNodeAnimator {
      public:
        CPlaneAnimator(irr::scene::ISceneNode *a_pNode);
        virtual ~CPlaneAnimator();

        virtual irr::scene::ISceneNodeAnimator *createClone(irr::scene::ISceneNode *a_pNode, irr::scene::ISceneManager *a_pMgr) override;

        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getType() const override;

        virtual void animateNode(irr::scene::ISceneNode *a_pNode, irr::u32 a_iTimeMs) override;
    };
  }
}
