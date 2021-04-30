// (w) 2021 by Dustbin::Games / Christian Keimel
#include <_generated/lua/CLuaScript_dialog.h>
#include <scenenodes/CGui3dSceneNodes.h>
#include <lua/CLuaTypeHelpers.h>
#include <state/CLuaState.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace state {
    CLuaState::CLuaState() : 
      m_pDevice (CGlobal::getInstance()->getIrrlichtDevice()), 
      m_pSmgr   (CGlobal::getInstance()->getSceneManager  ()), 
      m_pDrv    (CGlobal::getInstance()->getVideoDriver   ()), 
      m_pGui    (CGlobal::getInstance()->getGuiEnvironment()),
      m_pGlobal (CGlobal::getInstance()),
      m_pScript (nullptr),
      m_pGuiRoot(nullptr) {

    }

    CLuaState::~CLuaState() {
      deactivate();
    }

    void CLuaState::initGuiRoot(irr::scene::ISceneNode* a_pParent) {
      if (a_pParent->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_i3dGuiRootID)
        m_pGuiRoot = reinterpret_cast<scenenodes::CGui3dRoot *>(a_pParent);
      else
        for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++)
          initGuiRoot(*it);
    }

    /**
    * This method is called when the state is activated
    */
    void CLuaState::activate() {
      std::string l_sScriptName = m_pGlobal->popScript(),
                  l_sScript = lua::loadLuaScript(l_sScriptName);

      m_pScript = new CLuaScript_dialog(l_sScript);

      if (m_pScript != nullptr) {
        m_pScript->initialize();
        initGuiRoot(m_pSmgr->getRootSceneNode());
      }
    }

    /**
    * This method is called when the state is deactivated
    */
    void CLuaState::deactivate() {
      if (m_pScript != nullptr) {
        delete m_pScript;
        m_pScript = nullptr;
      }

      m_pGuiRoot = nullptr;
    }

    /**
    * Return the state's ID
    */
    enState CLuaState::getId() {
      return enState::LuaState;
    }

    /**
    * This is a callback method that gets invoked when the window is resized
    * @param a_cDim the new dimension of the window
    */
    void CLuaState::onResize(const irr::core::dimension2du& a_cDim) {
      irr::scene::ICameraSceneNode *l_pCam = m_pSmgr->getActiveCamera();
      if (l_pCam != nullptr) {
        irr::f32 l_fRatio = (irr::f32)a_cDim.Width / (irr::f32)a_cDim.Height;
        l_pCam->setAspectRatio(l_fRatio);
      }

      if (m_pScript != nullptr)
        m_pScript->windowresized();
    }

    /**
    * Event handling method. The main class passes all Irrlicht events to this method
    */
    bool CLuaState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      return l_bRet;
    }

    /**
    * This method is always called. Here the state has to perform it's actual work
    * @return enState::None for running without state change, any other value will switch to the state
    */
    enState CLuaState::run() {
      enState l_eRet = enState::None;

      int l_iFps = m_pDrv->getFPS();
      m_pDevice->setWindowCaption(std::wstring(L"Dustbin::Games - MarbleGP [" + std::to_wstring(l_iFps) + L" FPS]").c_str());

      m_pDrv->beginScene();
      m_pSmgr->drawAll();
      m_pGui->drawAll();
      m_pDrv->endScene();

      if (m_pScript != nullptr)
        m_pScript->step();

      if (m_pGuiRoot != nullptr)
        m_pGuiRoot->step();

      return l_eRet;
    }
  }
}