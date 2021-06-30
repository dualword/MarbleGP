// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CSkyBoxFix.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaSceneNode.h>
#include <lua/CLuaCamera.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaSceneManager::CLuaSceneManager(irr::scene::ISceneManager* a_pSmgr) : m_pSmgr(a_pSmgr) {
    }

    CLuaSceneManager::CLuaSceneManager(const CLuaSceneManager& a_cOther) : m_pSmgr(a_cOther.m_pSmgr) {
    }

    CLuaSceneManager::~CLuaSceneManager() {
    }

    irr::scene::ISceneNode* CLuaSceneManager::getNodeFromName(const std::string& a_sName, irr::scene::ISceneNode* a_pNode) {
      if (a_sName == a_pNode->getName())
        return a_pNode;

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pNode->getChildren().begin(); it != a_pNode->getChildren().end(); it++) {
        irr::scene::ISceneNode *l_pRet = getNodeFromName(a_sName, *it);
        if (l_pRet != nullptr)
          return l_pRet;
      }

      return nullptr;
    }

    CLuaCamera CLuaSceneManager::addCamera() {
      return CLuaCamera(m_pSmgr->addCameraSceneNode());
    }

    CLuaSceneNode CLuaSceneManager::getSceneNodeFromName(const std::string& a_sName) {
      irr::scene::ISceneNode *l_pNode = getNodeFromName(a_sName, m_pSmgr->getRootSceneNode());

      return CLuaSceneNode(l_pNode);
    }



    void CLuaSceneManager::loadScene(const std::string& a_sFile) {
      scenenodes::CSkyBoxFix *l_pFix = new scenenodes::CSkyBoxFix(m_pSmgr->getVideoDriver(), m_pSmgr, m_pSmgr->getFileSystem(), a_sFile);
      if (!m_pSmgr->loadScene(a_sFile.c_str()))
        printf("**** Load scene \"%s\" failed.\n", a_sFile.c_str());
      l_pFix->hideOriginalSkybox(m_pSmgr->getRootSceneNode());
      delete l_pFix;
    }

    void CLuaSceneManager::clear() {
      m_pSmgr->clear();
    }

    void CLuaSceneManager::drop() {
      if (m_pSmgr != nullptr && m_pSmgr != CGlobal::getInstance()->getSceneManager()) {
        CGlobal::getInstance()->removeSceneManager(m_pSmgr);
        m_pSmgr->drop();
      }
    }

    CLuaSceneManager CLuaSceneManager::createNewSceneManager(int a_iRenderPosition) {
      irr::scene::ISceneManager *l_pNew = CGlobal::getInstance()->getSceneManager()->createNewSceneManager();
      CGlobal::getInstance()->registerSceneManager(l_pNew, a_iRenderPosition);
      return CLuaSceneManager(l_pNew);
    }

    void CLuaSceneManager::registerClass(lua_State *a_pState) {
      CLuaCamera   ::registerClass(a_pState);
      CLuaSceneNode::registerClass(a_pState);

      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSceneManager>("LuaSceneManager")
          .addFunction("addcamera"           , &CLuaSceneManager::addCamera)
          .addFunction("loadscene"           , &CLuaSceneManager::loadScene)
          .addFunction("clear"               , &CLuaSceneManager::clear)
          .addFunction("drop"                , &CLuaSceneManager::drop)
          .addFunction("getscenenodefromname", &CLuaSceneManager::getSceneNodeFromName)
        .endClass();
    }
  }
}