// (w) 2021 by Dustbin::Games / Christian Keimel
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaCamera.h>

namespace dustbin {
  namespace lua {
    CLuaSceneManager::CLuaSceneManager(irr::scene::ISceneManager* a_pSmgr) : m_pSmgr(a_pSmgr) {
    }

    CLuaSceneManager::~CLuaSceneManager() {
    }

    CLuaCamera* CLuaSceneManager::addCamera() {
      CLuaCamera *l_pRet = new CLuaCamera(m_pSmgr->addCameraSceneNode());
      m_vObjects.push_back(l_pRet);
      return l_pRet;
    }

    void CLuaSceneManager::loadScene(const std::string& a_sFile) {
      m_pSmgr->loadScene(a_sFile.c_str());
    }

    void CLuaSceneManager::clear() {
      m_pSmgr->clear();
    }

    void CLuaSceneManager::registerClass(lua_State *a_pState) {
      CLuaCamera::registerClass(a_pState);

      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSceneManager>("LuaSceneManager")
          .addFunction("addcamera", &CLuaSceneManager::addCamera)
          .addFunction("loadscene", &CLuaSceneManager::loadScene)
          .addFunction("clear"    , &CLuaSceneManager::clear)
        .endClass();
    }
  }
}