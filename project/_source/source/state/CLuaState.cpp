// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CGui3dRoot.h>
#include <lua/CLuaScript_dialog.h>
#include <lua/CLuaHelpers.h>
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
      m_pGuiRoot(nullptr),
      m_pTimer  (nullptr)
    {
      for (int i = 0; i < 3; i++)
        m_bButtons[i] = false;

      m_pTimer = m_pDevice->getTimer();
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

      m_pScript = new lua::CLuaScript_dialog(l_sScript);
      m_pScript->initialize();
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

      // if (m_pScript != nullptr)
      //   m_pScript->windowresized();
    }

    /**
    * Event handling method. The main class passes all Irrlicht events to this method
    */
    bool CLuaState::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        m_bButtons[0] = a_cEvent.MouseInput.isLeftPressed  ();
        m_bButtons[1] = a_cEvent.MouseInput.isMiddlePressed();
        m_bButtons[2] = a_cEvent.MouseInput.isRightPressed ();
      }

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
        m_pScript->step(m_pTimer->getTime());

      if (m_pGuiRoot != nullptr)
        m_pGuiRoot->step();

      return l_eRet;
    }

    void CLuaState::onUievent(const std::string& a_type, irr::s32 a_id, const std::string& a_name, const std::string& a_data) {
      /*if (m_pScript != nullptr) {
        if (a_type == "uielementhovered")
          m_pScript->uielementhovered(a_id, a_name);
        else if (a_type == "uielementleft")
          m_pScript->uielementleft(a_id, a_name);
        else if (a_type == "uibuttonclicked")
          m_pScript->uibuttonclicked(a_id, a_name);
      }*/
    }

    bool CLuaState::isMouseDown(enMouseButton a_eButton) {
      switch (a_eButton) {
        case enMouseButton::Left  : return m_bButtons[0];
        case enMouseButton::Middle: return m_bButtons[1];
        case enMouseButton::RIght : return m_bButtons[2];
        default: return false;
      }
    }
  }
}