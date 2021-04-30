// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

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
    class CMarbleGPSceneNodeFactory : public irr::scene::ISceneNodeFactory {
      private:
        irr::scene::ESCENE_NODE_TYPE getTypeFromName(const irr::c8 *a_sName);
        irr::scene::ISceneManager *m_pSmgr;

    public:
        CMarbleGPSceneNodeFactory(irr::scene::ISceneManager *a_pSmgr);
        virtual ~CMarbleGPSceneNodeFactory();

        virtual irr::scene::ISceneNode* addSceneNode(irr::scene::ESCENE_NODE_TYPE a_eType, irr::scene::ISceneNode *a_pParent = 0);
        virtual irr::scene::ISceneNode* addSceneNode(const irr::c8 *a_sTypeName, irr::scene::ISceneNode *a_pParent = 0);
        virtual irr::u32 getCreatableSceneNodeTypeCount() const;
        virtual const irr::c8 *getCreateableSceneNodeTypeName(irr::u32 a_iIdx) const;
        virtual irr::scene::ESCENE_NODE_TYPE getCreateableSceneNodeType(irr::u32 a_iIdx) const;
        virtual const irr::c8 *getCreateableSceneNodeTypeName(irr::scene::ESCENE_NODE_TYPE a_eType) const;
    };
  }
}
