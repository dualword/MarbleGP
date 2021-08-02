// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CSkyBoxFix.h>
#include <lua/CLuaSceneManager.h>
#include <LuaBridge/LuaBridge.h>
#include <lua/CLuaSceneNode.h>
#include <lua/CLuaGuiItem.h>
#include <lua/CLuaCamera.h>
#include <CGlobal.h>

namespace dustbin {
  namespace lua {
    CLuaSceneManager::CLuaSceneManager(irr::scene::ISceneManager* a_pSmgr) : m_pSmgr(a_pSmgr), m_pRtt(nullptr) {
    }

    CLuaSceneManager::CLuaSceneManager(const CLuaSceneManager& a_cOther) : m_pSmgr(a_cOther.m_pSmgr), m_pRtt(a_cOther.m_pRtt) {
      if (m_pRtt != nullptr) 
        m_pRtt->grab();

      if (m_pSmgr != CGlobal::getInstance()->getSceneManager())
        m_pSmgr->grab();
    }

    CLuaSceneManager::~CLuaSceneManager() {
      if (m_pRtt != nullptr)
        m_pRtt->drop();

      if (m_pSmgr != CGlobal::getInstance()->getSceneManager())
        m_pSmgr->drop();
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
      scenenodes::CSkyBoxFix *l_pFix = new scenenodes::CSkyBoxFix(CGlobal::getInstance()->getVideoDriver(), m_pSmgr, CGlobal::getInstance()->getFileSystem(), a_sFile);
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

    void CLuaSceneManager::render() {
      if (m_pRtt != nullptr) {
        m_pSmgr->getVideoDriver()->setRenderTarget(m_pRtt, true, true);
        m_pSmgr->drawAll();
        m_pSmgr->getVideoDriver()->setRenderTarget(nullptr, false, false);
      }
      else m_pSmgr->drawAll();
    }

    void CLuaSceneManager::setRenderTarget(CLuaGuiItem* a_pRenderTarget, const std::string& a_sRttName) {
      if (a_pRenderTarget != nullptr) {
        irr::gui::IGUIElement* l_pRtt = a_pRenderTarget->getElement();
        if (l_pRtt != nullptr && l_pRtt->getType() == irr::gui::EGUIET_IMAGE) {
          irr::gui::IGUIImage* l_pImage = reinterpret_cast<irr::gui::IGUIImage*>(l_pRtt);
          irr::core::dimension2du l_cDim = irr::core::dimension2du(l_pImage->getAbsoluteClippingRect().getWidth(), l_pImage->getAbsoluteClippingRect().getHeight());

          m_pRtt = m_pSmgr->getVideoDriver()->addRenderTargetTexture(l_cDim, a_sRttName.c_str());
          m_pRtt->grab();
          l_pImage->setImage(m_pRtt);

          irr::scene::ICameraSceneNode* l_pCam = m_pSmgr->getActiveCamera();

          if (l_pCam != nullptr)
            l_pCam->setAspectRatio((irr::f32)l_pImage->getAbsoluteClippingRect().getWidth() / (irr::f32)l_pImage->getAbsoluteClippingRect().getHeight());
        }
      }
    }

    void CLuaSceneManager::registerClass(lua_State *a_pState) {
      CLuaCamera   ::registerClass(a_pState);
      CLuaSceneNode::registerClass(a_pState);

      luabridge::getGlobalNamespace(a_pState)
        .beginClass<CLuaSceneManager>("LuaSceneManager")
          .addFunction("addcamera"            , &CLuaSceneManager::addCamera)
          .addFunction("loadscene"            , &CLuaSceneManager::loadScene)
          .addFunction("render"               , &CLuaSceneManager::render)
          .addFunction("clear"                , &CLuaSceneManager::clear)
          .addFunction("drop"                 , &CLuaSceneManager::drop)
          .addFunction("getscenenodefromname" , &CLuaSceneManager::getSceneNodeFromName)
          .addFunction("createnewscenemanager", &CLuaSceneManager::createNewSceneManager)
          .addFunction("setrendertarget"      , &CLuaSceneManager::setRenderTarget)
        .endClass();
    }
  }
}