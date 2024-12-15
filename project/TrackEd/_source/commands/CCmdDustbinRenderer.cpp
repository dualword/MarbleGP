// (w) 2024 by Dustbin::Games / Christian Keimel
#include <shaders/CDustbinShaderDefines.h>
#include <commands/CCmdDustbinRenderer.h>
#include <shaders/CDustbinShaders.h>
#include <helpers/CTextureHelpers.h>
#include <CDefines.h>
#include <IGlobal.h>

namespace dustbin {
  namespace commands {
    CCmdDustbinRenderer::CCmdDustbinRenderer(irr::IrrlichtDevice *a_pDevice) : 
      ICommand ("SceneRenderer"),
      m_pShader(nullptr), 
      m_pDevice(a_pDevice),
      m_pDrv   (a_pDevice->getVideoDriver()),
      m_pSmgr  (a_pDevice->getSceneManager()),
      m_pGui   (a_pDevice->getGUIEnvironment())
    {
      m_pShader = new shaders::CDustbinShaders(m_pDevice);
      m_pShader->setRenderOptions(shaders::enShadowMode::Off, shaders::enShadowQuality::High);
    }

    CCmdDustbinRenderer::~CCmdDustbinRenderer() {
      delete m_pShader;
      m_pShader = nullptr;
    }

    /**
    * If this method returns "true" the command is finished and
    * will be removed from the vector of active commands
    * @return true if the command is finished and can be removed
    */
    bool CCmdDustbinRenderer::isFinished() {
      return false;
    }

    /**
    * This method is called on every frame.
    */
    void CCmdDustbinRenderer::run() {
      if (m_pShader != nullptr) {
        m_pShader->startShadowMaps();
        m_pShader->renderShadowMap(
          (irr::u32)shaders::enShadowMap::Solid | 
          (irr::u32)shaders::enShadowMap::Marbles |
          (irr::u32)shaders::enShadowMap::Moving |
          (irr::u32)shaders::enShadowMap::Transparent | 
          (irr::u32)shaders::enShadowMap::TranspColor
        );
        m_pShader->endShadowMaps();

        m_pDrv->beginScene(true, true);
        m_pShader->renderScene();
      }
      else {
        m_pDrv->beginScene(true, true);
        m_pSmgr->drawAll();
      }


      tracked::IGlobal::getInstance()->beforeEndScene();

      m_pGui->drawAll();
      m_pDrv->endScene();
    }

    /**
    * The Irrlicht event handler method. Implement it if the command
    * needs to react to user input
    */
    bool CCmdDustbinRenderer::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iSceneWillBeCleared) {
          if (m_pShader != nullptr)
            m_pShader->clear();
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iSceneLoaded) {
          if (m_pShader != nullptr && m_pSmgr != nullptr) {
            helpers::addNodeToShader(m_pShader, m_pSmgr->getRootSceneNode(), nullptr, helpers::countMeshSceneNodes(m_pSmgr->getRootSceneNode()));
            m_pShader->setRenderOptions(shaders::enShadowMode::Off, shaders::enShadowQuality::High);
            m_pShader->addLightCamera();
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iUpdateShader) {
          if (m_pShader != nullptr && m_pSmgr != nullptr) {
            m_pShader->clear();
            helpers::addNodeToShader(m_pShader, m_pSmgr->getRootSceneNode(), nullptr, helpers::countMeshSceneNodes(m_pSmgr->getRootSceneNode()));
            m_pShader->setRenderOptions(shaders::enShadowMode::Off, shaders::enShadowQuality::High);
            m_pShader->addLightCamera();
          }
        }
      }

      return l_bRet;
    }
  }
}