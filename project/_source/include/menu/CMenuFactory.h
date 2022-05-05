// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <string>

namespace irr {
  class IrrlichtDevice;   /**< Forward declaration of the Irrlicht device */
}

namespace dustbin {
  namespace state {
    class IState;   /**< Forward declaration of the state interface */
  }

  namespace menu {
    class IMenuHandler;   /**< Forward declaration of the menu handler */
    class IMenuManager;   /**< Forward declaration of the menu manager */

    IMenuHandler *createMenuMain       (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState);
    IMenuHandler *createMenuSettings   (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState);
    IMenuHandler *createMenuProfiles   (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState);
    IMenuHandler *createMenuSetupGame  (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState *a_pState);
    IMenuHandler *createMenuSelectTrack(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
    IMenuHandler *createMenuRaceResult (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
    IMenuHandler *createMenuStandings  (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
    IMenuHandler *createMenuStartServer(irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
    IMenuHandler *createMenuJoinServer (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
    IMenuHandler *createMenuMessage    (irr::IrrlichtDevice* a_pDevice, IMenuManager* a_pManager, state::IState* a_pState);
  }
}

