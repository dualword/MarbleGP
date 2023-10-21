#pragma once

#include <irrlicht.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace animators {
    /**
    * @class CAnimatorFactory
    * @author Christian Keimel
    * Irrlicht animator factory
    */
    class CAnimatorFactory : public irr::scene::ISceneNodeAnimatorFactory {
      private:
        std::map<int        , std::string> m_mTypeName;
        std::map<std::string, int        > m_mNameType;

      public:
        CAnimatorFactory();
        virtual ~CAnimatorFactory();

        virtual irr::scene::ISceneNodeAnimator *createSceneNodeAnimator(irr::scene::ESCENE_NODE_ANIMATOR_TYPE a_eType, irr::scene::ISceneNode *a_pTarget) override;
        virtual irr::scene::ISceneNodeAnimator *createSceneNodeAnimator(const irr::c8 *a_sTypeName, irr::scene::ISceneNode *a_pTarget) override;
        virtual irr::u32 getCreatableSceneNodeAnimatorTypeCount() const override;
        virtual irr::scene::ESCENE_NODE_ANIMATOR_TYPE getCreateableSceneNodeAnimatorType(irr::u32 a_iIdx) const override;
        virtual const irr::c8 *getCreateableSceneNodeAnimatorTypeName(irr::u32 a_iIdx) const override;
        virtual const irr::c8 *getCreateableSceneNodeAnimatorTypeName(irr::scene::ESCENE_NODE_ANIMATOR_TYPE a_eType) const override;
     };
  }
}
