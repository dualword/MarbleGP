// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CStartingGridSceneNode_base.h>

namespace dustbin {
  namespace scenenodes {
    class CStartingGridSceneNode_Editor : public CStartingGridSceneNode_Base {
      private:

      public:
        CStartingGridSceneNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CStartingGridSceneNode_Editor();

        irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager);

        virtual void render();
    };
  }
}
