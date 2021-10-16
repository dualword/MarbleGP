// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CStartingGridSceneNode_Base.h>

namespace dustbin {
  namespace scenenodes {
    /**
    * @class CStartingGridSceneNode
    * @author Christian Keimel
    * This is the starting grid node for the game which does not
    * render a cone and creates phyiscs objects
    */
    class CStartingGridSceneNode : public CStartingGridSceneNode_Base {
      private:
        int m_iNextMarble;

      public:
        CStartingGridSceneNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId);
        virtual ~CStartingGridSceneNode();

        irr::scene::ISceneNode* clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager);

        gameclasses::SMarbleNodes *getNextMarble();
        void removeUnusedMarbles();

        irr::f32 getAngle();

        virtual void render();
    };
  }
}
