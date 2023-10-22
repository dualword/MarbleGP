// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CSpeedNode.h>
#include <scenenodes/CDustbinId.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CSpeedNode::CSpeedNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      ISceneNode(a_pParent, a_pMgr, a_iId),
      m_fMinSpeed (0.0f),
      m_fMaxSpeed (200.0f),
      m_fBestSpeed(150.0f)
    {
      m_cBox.reset(getPosition());
      sceneNodeIdUsed(a_iId);

      for (int i = 0; i < 4; i++)
        m_aArrow[i] = nullptr;

      for (int i = 0; i < 3; i++)
        m_aWarn[i] = nullptr;

      for (int i = 0; i < 4; i++)
        m_aSpeed[i] = nullptr;
    }

    CSpeedNode::~CSpeedNode() {
    }

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CSpeedNode::getMaterialCount() {
      return 0;
    }

    irr::scene::ESCENE_NODE_TYPE CSpeedNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_SpeedNodeId;
    }

    void CSpeedNode::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CSpeedNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CSpeedNode::getBoundingBox() const {
      return m_cBox;
    }

    void CSpeedNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addFloat("minspeed" , m_fMinSpeed );
      a_pOut->addFloat("maxspeed" , m_fMaxSpeed );
      a_pOut->addFloat("bestspeed", m_fBestSpeed);

      sceneNodeIdUsed(getID());
    }

    void CSpeedNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      m_fMinSpeed  = a_pIn->getAttributeAsFloat("minspeed");
      m_fMaxSpeed  = a_pIn->getAttributeAsFloat("maxspeed");
      m_fBestSpeed = a_pIn->getAttributeAsFloat("bestspeed");

      sceneNodeIdUsed(getID());
    }

    irr::scene::ISceneNode* CSpeedNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent  == nullptr) a_pNewParent  = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CSpeedNode* l_pNew = new CSpeedNode(a_pNewParent, a_pNewManager, getNextSceneNodeId());

      l_pNew->m_fMinSpeed  = m_fMinSpeed;
      l_pNew->m_fMaxSpeed  = m_fMaxSpeed;
      l_pNew->m_fBestSpeed = m_fBestSpeed;

      return l_pNew;
    }

    /**
    * Get the minimal speed
    * @return the minimal speed
    */
    irr::f32 CSpeedNode::getMinSpeed() {
      return m_fMinSpeed;
    }

    /**
    * Get the maximal speed
    * @return the minimal speed
    */
    irr::f32 CSpeedNode::getMaxSpeed() {
      return m_fMaxSpeed;
    }

    /**
    * Get the bestspeed
    * @return the minimal speed
    */
    irr::f32 CSpeedNode::getBestSpeed() {
      return m_fBestSpeed;
    }

    /**
    * Set the speed of the marble of the current viewport
    */
    void CSpeedNode::setMarbleSpeed(irr::f32 a_fSpeed, int a_iStep) {
      if (IsVisible) {
        irr::video::ITexture *l_pTexture = m_aSpeed[3];

        if (a_fSpeed > 0.0f) {
          if (a_fSpeed <= m_fMinSpeed || a_fSpeed >= m_fMaxSpeed) {
            // The speed is totally wrong
            int l_iTexture = (a_iStep / 120) % 3;

            if (a_fSpeed < m_fBestSpeed) {
              // The player is way too slow
              l_pTexture = l_iTexture == 0 ? m_aArrow[0] : l_iTexture == 1 ? m_aWarn[0] : m_aSpeed[0];
            }
            else {
              // The player is way too fast
              l_pTexture = l_iTexture == 0 ? m_aArrow[1] : l_iTexture == 1 ? m_aWarn[1] : m_aSpeed[1];
            }
          }
          else {
            // the velocity is in the correct range
            if (a_fSpeed >= m_fBestSpeed - 2.5f && a_fSpeed <= m_fBestSpeed + 2.5f) {
              // The velocity is close to perfect
              l_pTexture = m_aSpeed[2];
            }
            else {
              bool l_bArrow = (a_iStep / 120) % 2 == 0;

              if (a_fSpeed < m_fBestSpeed) {
                // The player is a little too slow
                l_pTexture = l_bArrow ? m_aArrow[2] : m_aSpeed[2];
              }
              else {
                // the player is a little too fast
                l_pTexture = l_bArrow ? m_aArrow[3] : m_aSpeed[2];
              }
            }
          }
        }

        updateTexture(l_pTexture);
      }
    }

    /**
    * Fill the texture maps
    */
    void CSpeedNode::fillTextureMaps() {
      CGlobal *l_pGlobal = CGlobal::getInstance();
      irr::gui::IGUIFont *l_pFont = l_pGlobal->getFont(enFont::Huge, irr::core::dimension2du(1920, 1080));

      irr::video::IVideoDriver *l_pDrv = l_pGlobal->getVideoDriver();
        
      irr::core::dimension2du l_cDim = l_pFont->getDimension(L"666");

      l_cDim.Width  = l_cDim.Width > l_cDim.Height ? 5 * l_cDim.Width / 4 : 5 * l_cDim.Height / 4;
      l_cDim.Height = l_cDim.Width > l_cDim.Height ? 5 * l_cDim.Width / 4 : 5 * l_cDim.Height / 4;

      irr::core::recti l_cRect = irr::core::recti(0, 0, l_cDim.Width, l_cDim.Height);

      for (int i = 0; i < 4; i++) {
        std::string l_sName = "speedtexture_" + std::to_string((int)m_fBestSpeed) + (i == 0 ? "_orange" : i == 1 ? "_red" : i == 2 ? "_green" : "_gray");

        m_aSpeed[i] = l_pDrv->getTexture(l_sName.c_str());

        if (m_aSpeed[i] == nullptr && l_pFont != nullptr) {
          m_aSpeed[i] = l_pDrv->addRenderTargetTexture(l_cDim, l_sName.c_str(), irr::video::ECF_A8R8G8B8);
          l_pDrv->setRenderTarget(m_aSpeed[i]);

          l_pDrv->beginScene(true, true, 
            i == 0 ? irr::video::SColor(0xFF, 0xFF, 0x80, 0) : 
            i == 1 ? irr::video::SColor(0xFF, 0xFF, 0x20, 0) :
            i == 2 ? irr::video::SColor(0xFF,    0, 0x80, 0) : irr::video::SColor(0xFF, 0xF5, 0xF5, 0xF5));

          l_pFont->draw(std::to_wstring((int)m_fBestSpeed).c_str(), l_cRect, i == 3 ? irr::video::SColor(0xFF, 0, 0, 0) : irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), true, true);
          l_pDrv->setRenderTarget(nullptr);
        }
      }

      for (int i = 0; i < 4; i++) {
        m_aArrow[i] = l_pDrv->getTexture(i == 0 ? "data/textures/speed_orange_up.png" : i == 1 ? "data/textures/speed_red_down.png" : i == 2 ? "data/textures/speed_green_up.png" : "speed_green_down.png");

        if (i < 3)
          m_aWarn[i] = l_pDrv->getTexture(i == 0 ? "data/textures/speed_orange_warn.png" : i == 1 ? "data/textures/speed_red_warn.png" : "data/textures/speed_green_warn.png");
      }

      updateTexture(m_aSpeed[3]);
    }

    /**
    * Update the textures of all "IMeshSceneNode" children
    * @param a_pTexture the new texture
    */
    void CSpeedNode::updateTexture(irr::video::ITexture* a_pTexture) {
      if (a_pTexture != nullptr) {
        for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator l_itChild = Children.begin(); l_itChild != Children.end(); l_itChild++) {
          if ((*l_itChild)->getType() == irr::scene::ESNT_MESH) {
            irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(*l_itChild);
            l_pNode->getMaterial(0).setTexture(0, a_pTexture);
          }
        }
      }
    }
  }
}