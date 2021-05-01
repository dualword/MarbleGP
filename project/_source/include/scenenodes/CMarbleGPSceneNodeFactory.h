// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#include <scenenodes/CMarbleGPSceneNodeFactoryEditor.h>

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace scenenodes {
    /**
     * @class CMarbleGPSceneNodeFactory
     * @author Christian Keimel
     * @brief this is the implementation for the factory of all custom scene nodes for MarbleGP
     */
    class CMarbleGPSceneNodeFactory : public CMarbleGPSceneNodeFactoryEditor {
      public:
        CMarbleGPSceneNodeFactory(irr::scene::ISceneManager *a_pSmgr);
        virtual ~CMarbleGPSceneNodeFactory();

        virtual irr::scene::ISceneNode* addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pParent = 0);
    };
  }
}
