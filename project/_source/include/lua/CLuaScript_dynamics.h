// (w) 2021 by Dustbin::Games / Christian Keimel
#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <lua.hpp>

#include <lua/CLuaPhysicsEntity.h>
#include <LuaBridge/LuaBridge.h>
#include <string>
#include <vector>

namespace dustbin {
  namespace gameclasses {
    class CWorld;
    class CObject;
  }

  namespace lua {
    class CLuaSingleton_system;

    /**
    * @class CLuaScript_dynamics
    * @author Christian Keimel
    * This is a script that is executed in the dynamics thread. It
    * can trigger things in the scene, e.g. start motors and so
    */
    class CLuaScript_dynamics {
      private:

        lua_State* m_pState;

        CLuaSingleton_system* m_pSystem;

        std::vector<CLuaPhysicsEntity> m_vEntities;

      public:
        CLuaScript_dynamics(gameclasses::CWorld *a_pWorld, const std::string &a_sScript);
        ~CLuaScript_dynamics();

        /**
        * LUA callback if a trigger was triggered
        * @param a_iMarble the triggering marble
        * @param a_iTrigger the ID of the trigger
        */
        void onTrigger(int a_iMarble, int a_iTrigger);

        /**
        * LUA callback for passes of a checkpoint by a marble
        * @param a_iMarble the marble
        * @param a_iCheckpoint the checkpoint
        */
        void onCheckpoint(int a_iMarble, int a_iCheckpoint);

        /**
        * LUA callback if a marble respawns. Only called once when the respawn starts
        * @param a_iMarble the respawning marble
        */
        void onRespawn(int a_iMarble);

        /**
        * LUA callback for every simulation step
        * @param a_iStep the current simulation step
        */
        void onStep(int a_iStep);

        /**
        * LUA callback for marble movement. This is called every simulation step
        * for every marble so be sure not to make this too complicated
        */
        void onMarbleMoved(int a_iMarble, const irr::core::vector3df& a_vPosition);

        /**
        * LUA callback for changes in the player stunned state
        * @param a_iMarble the player's marble
        * @param a_iState the new state (1 == stunned, 0 == back to normal)
        */
        void onPlayerStunned(int a_iMarble, int a_iState);
    };
  }
}