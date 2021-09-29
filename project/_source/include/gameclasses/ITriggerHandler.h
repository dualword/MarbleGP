// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

namespace dustbin {
  namespace gameclasses {
    /**
    * @class ITriggerHandler
    * @author Christian Keimel
    * This interface needs to be implemented by a class
    * handling triggers from the game
    * @see CDynamicThread
    */
    class ITriggerHandler {
      public:
        /**
        * Callback to handle a trigger
        * @param a_iTrigger trigger id
        * @param a_iMarble Id of the marble 
        * @param a_vPosition position of the marble that triggered
        */
        virtual void handleTrigger(int a_iTrigger, int a_iMarble, const irr::core::vector3df& a_vPosition) = 0;

        /**
        * Callback to start respawn of a marble
        * @param a_iMarble Id of the marble to respawn
        */
        virtual void handleRespawn(int a_iMarble) = 0;
    };
  }
}