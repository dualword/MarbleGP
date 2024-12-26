// (w) 2024 by Dustbin::Games / Christian Keimel
#include <shaders/CDustbinShaderDefines.h>
#include <commands/CCmdDustbinRenderer.h>
#include <scenenodes/CDustbinLight.h>
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
      m_pShader->setRenderOptions(shaders::enShadowMode::SolidTrans, shaders::enShadowQuality::High);
    }

    CCmdDustbinRenderer::~CCmdDustbinRenderer() {
      delete m_pShader;
      m_pShader = nullptr;
    }

    irr::scene::ISceneNode *CCmdDustbinRenderer::findNodeByType(irr::scene::ISceneNode *a_pParent, irr::scene::ESCENE_NODE_TYPE a_eType) {
      if (a_pParent->getType() == a_eType)
        return a_pParent;

      for (auto l_pChild: a_pParent->getChildren()) {
        irr::scene::ISceneNode *l_pRet = findNodeByType(l_pChild, a_eType);
        if (l_pRet != nullptr)
          return l_pRet;
      }

      return nullptr;
    }

    irr::scene::ISceneNode *CCmdDustbinRenderer::findNodeByName(irr::scene::ISceneNode *a_pParent, const std::string &a_sName) {
      if (a_sName == a_pParent->getName())
        return a_pParent;

      for (auto l_pChild: a_pParent->getChildren()) {
        irr::scene::ISceneNode *l_pRet = findNodeByName(l_pChild, a_sName);
        if (l_pRet != nullptr)
          return l_pRet;
      }

      return nullptr;
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
            m_pShader->clear();
            helpers::addNodeToShader(m_pShader, m_pSmgr->getRootSceneNode(), nullptr, helpers::countMeshSceneNodes(m_pSmgr->getRootSceneNode()));
            m_pShader->setRenderOptions(shaders::enShadowMode::TransColor, shaders::enShadowQuality::High);
            m_pShader->addLightCamera();
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iUpdateShader) {
          if (m_pShader != nullptr && m_pSmgr != nullptr) {
            scenenodes::CDustbinLight *l_pLight = reinterpret_cast<scenenodes::CDustbinLight *>(findNodeByType(m_pSmgr->getRootSceneNode(), (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_DustbinLightId));

            if (l_pLight != nullptr) {
              irr::scene::IMeshSceneNode *l_pTrack = reinterpret_cast<irr::scene::IMeshSceneNode *>(findNodeByName(m_pSmgr->getRootSceneNode(), "GeneratedTrack"));

              if (l_pTrack != nullptr) {
                irr::core::aabbox3df l_cBox = l_pTrack->getMesh()->getBoundingBox();
                irr::core::vector3df l_cTgt = l_cBox.getCenter();

                l_cTgt.Y = 0.0f;

                l_pLight->setLightTarget(l_cTgt);
              }
            }

            m_pShader->clear();
            helpers::addNodeToShader(m_pShader, m_pSmgr->getRootSceneNode(), nullptr, helpers::countMeshSceneNodes(m_pSmgr->getRootSceneNode()));
            m_pShader->setRenderOptions(shaders::enShadowMode::TransColor, shaders::enShadowQuality::High);
            m_pShader->addLightCamera();
          }
        }
        else if (a_cEvent.UserEvent.UserData1 == c_iRenderScene) {
          m_pDrv->beginScene(true, true);

          if (m_pShader != nullptr) {
            m_pShader->renderScene();
          }
          else {
            m_pSmgr->drawAll();
          }

          m_pDrv->endScene();
        }
      }

      return l_bRet;
    }
  }
}