// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaScript_dynamics.h>
#include <lua/CLuaSingleton_system.h>
#include <_generated/lua/lua_tables.h>
#include <gameclasses/COdeNodes.h>
#include <LuaBridge/LuaBridge.h>
#include <LuaBridge/Vector.h>
#include <lua/CLuaHelpers.h>
#include <CGlobal.h>
#include <exception>

namespace dustbin {
  namespace lua {
    CLuaScript_dynamics::CLuaScript_dynamics(gameclasses::CWorld* a_pWorld, const std::string& a_sScript) : 
      m_pSystem(nullptr),
      m_pState(nullptr)
    {
      if (a_sScript != "") {
        m_pState = luaL_newstate();
        luaL_openlibs(m_pState);

        CLuaPhysicsEntity::registerClass(m_pState);
        m_pSystem = new CLuaSingleton_system(m_pState);

        if (luaL_dostring(m_pState, a_sScript.c_str()) != LUA_OK) {
          CGlobal::getInstance()->setGlobal("ERROR_MESSAGE", lua_tostring(m_pState, -1));
          CGlobal::getInstance()->setGlobal("ERROR_HEAD", "Error while running game logic LUA script");
          throw std::exception();
        }

        for (std::vector<gameclasses::CObject*>::iterator it = a_pWorld->m_vObjects.begin(); it != a_pWorld->m_vObjects.end(); it++) {
          CLuaPhysicsEntity l_cEntity = CLuaPhysicsEntity();
          l_cEntity.m_pObject = *it;
          m_vEntities.push_back(l_cEntity);
        }

        luabridge::LuaRef l_cInit = luabridge::getGlobal(m_pState, "initialize");
        if (l_cInit.isCallable())
          l_cInit(m_vEntities);
      }
    }

    CLuaScript_dynamics::~CLuaScript_dynamics() {
      if (m_pState != nullptr)
        lua_close(m_pState);

      if (m_pSystem != nullptr)
        delete m_pSystem;
    }

    /**
    * LUA callback if a trigger was triggered
    * @param a_iMarble the triggering marble
    * @param a_iTrigger the ID of the trigger
    */
    void CLuaScript_dynamics::onTrigger(int a_iMarble, int a_iTrigger) {
      if (m_pState != nullptr) {
        luabridge::LuaRef l_cTrigger = luabridge::getGlobal(m_pState, "onTrigger");
        if (l_cTrigger.isCallable())
          l_cTrigger(a_iMarble, a_iTrigger);
      }
    }

    /**
    * LUA callback for passes of a checkpoint by a marble
    * @param a_iMarble the marble
    * @param a_iCheckpoint the checkpoint
    */
    void CLuaScript_dynamics::onCheckpoint(int a_iMarble, int a_iCheckpoint) {
      if (m_pState != nullptr) {
        luabridge::LuaRef l_cCheckpoint = luabridge::getGlobal(m_pState, "onCheckpoint");
        if (l_cCheckpoint.isCallable())
          l_cCheckpoint(a_iMarble, a_iCheckpoint);
      }
    }

    /**
    * LUA callback if a marble respawns. Only called once when the respawn starts
    * @param a_iMarble the respawning marble
    */
    void CLuaScript_dynamics::onRespawn(int a_iMarble) {
      if (m_pState != nullptr) {
        luabridge::LuaRef l_cRespawn = luabridge::getGlobal(m_pState, "onRespawn");
        if (l_cRespawn.isCallable())
          l_cRespawn(a_iMarble);
      }
    }

    /**
    * LUA callback for every simulation step
    * @param a_iStep the current simulation step
    */
    void CLuaScript_dynamics::onStep(int a_iStep) {
      if (m_pState != nullptr) {
        luabridge::LuaRef l_cStep = luabridge::getGlobal(m_pState, "onStep");
        if (l_cStep.isCallable())
          l_cStep(a_iStep);
      }
    }

    /**
    * LUA callback for marble movement. This is called every simulation step
    * for every marble so be sure not to make this too complicated
    * @param a_iMarble the marble that is moving
    * @param a_vPosition the new position of the marble
    */
    void CLuaScript_dynamics::onMarbleMoved(int a_iMarble, const irr::core::vector3df& a_vPosition) {
      if (m_pState != nullptr) {
        lua_getglobal(m_pState, "onMarbleMoved");
        if (!lua_isnil(m_pState, -1)) {
          lua_pushinteger(m_pState, a_iMarble);
          SVector3d l_vPosition = convertVectorToLua(a_vPosition);
          l_vPosition.pushToStack(m_pState);
          lua_pcall(m_pState, 2, 0, 0);
        }
      }
    }
  }
}
