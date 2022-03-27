/* (w) 2021 - 22 by Dustbin::Games (Christian Keimel) - This file is licensed under the terms of the zlib license */
#pragma once

#include <scenenodes/CAiNode.h>
#include <irrlicht.h>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace gameclasses {
    /**
    * @class CAiGameData
    * @author Christian Keimel
    * This class calculates and manages all data necessary for the MarbleGP AI
    */
    class CAiGameData {
      private:
        enum class enPathDirection {
          UseStart,
          UseEnd,
          UseBoth
        };

        irr::video::IVideoDriver  *m_pDrv;
        irr::scene::ISceneManager *m_pSmgr;

      public:
        CAiGameData(irr::video::IVideoDriver *a_pDrv, irr::scene::ISceneManager *a_pSmgr);
        ~CAiGameData();

        void draw();
    };
  }
}
