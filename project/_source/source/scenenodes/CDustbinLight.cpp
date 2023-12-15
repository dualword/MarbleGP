#include <scenenodes/CDustbinLight.h>

namespace dustbin {
  namespace scenenodes {
    CDustbinLight::CDustbinLight(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId),
      m_fNear  (5.0f),
      m_fFar   (2000.0f),
      m_fFov   (2000.0f),
      m_cTarget(irr::core::vector3df(0.0f, 0.0f, 0.0f))
    {
    }

    CDustbinLight::~CDustbinLight() {
    }

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CDustbinLight::getMaterialCount() {
      return 0;
    }

    irr::scene::ESCENE_NODE_TYPE CDustbinLight::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE )g_DustbinLightId;
    }

    void CDustbinLight::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CDustbinLight::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CDustbinLight::getBoundingBox() const {
      return m_cBox;
    }

    void CDustbinLight::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addFloat   ("nearvalue"  , m_fNear   );
      a_pOut->addFloat   ("farvalue"   , m_fFar     );
      a_pOut->addFloat   ("fieldofview", m_fFov     );
      a_pOut->addVector3d("lighttarget", m_cTarget  );
    }

    void CDustbinLight::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      m_fNear     = a_pIn->getAttributeAsFloat   ("nearvalue"  );
      m_fFar      = a_pIn->getAttributeAsFloat   ("farvalue"   );
      m_fFov      = a_pIn->getAttributeAsFloat   ("fieldofview");
      m_cTarget   = a_pIn->getAttributeAsVector3d("lighttarget");

      printf("Target: %.2f, %.2f, %.2f\n", m_cTarget.X, m_cTarget.Y, m_cTarget.Z);
    }

    irr::scene::ISceneNode* CDustbinLight::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      CDustbinLight *l_pNew = new CDustbinLight(a_pNewParent != nullptr ? a_pNewParent : Parent, a_pNewManager != nullptr ? a_pNewManager : SceneManager, ID);

      l_pNew->setPosition   (getPosition());
      l_pNew->setNearValue  (m_fNear);
      l_pNew->setFarValue   (m_fFar);
      l_pNew->setFieldOfView(m_fFov);

      l_pNew->setLightTarget(m_cTarget);

      return l_pNew;
    }

    /**
    * Set the light target
    * @param a_cTarget the light target
    */
    void CDustbinLight::setLightTarget(const irr::core::vector3df& a_cTarget) {
      m_cTarget = a_cTarget;
    }

    /**
    * Get the light target
    * @return the light target
    */
    const irr::core::vector3df &CDustbinLight::getLightTarget() const {
      return m_cTarget;
    }

    /**
    * Set the field of view
    * @param a_fFov the field of view
    */
    void CDustbinLight::setFieldOfView(irr::f32 a_fFov) {
      m_fFov = a_fFov;
    }

    /**
    * Get the field of view
    * @return the field of view
    */
    irr::f32 CDustbinLight::getFieldOfView() const {
      return m_fFov;
    }

    /**
    * Set the light's near value
    * @param a_fNear the light's near value
    */
    void CDustbinLight::setNearValue(irr::f32 a_fNear) {
      m_fNear = a_fNear;
    }

    /**
    * Get the light's near value
    * @return the light's near value
    */
    irr::f32 CDustbinLight::getNearValue() const {
      return m_fNear;
    }

    /**
    * Set the light's far value
    * @param a_fFar the light's far value
    */
    void CDustbinLight::setFarValue(irr::f32 a_fFar) {
      m_fFar = a_fFar;
    }

    /**
    * Get the light's far value
    * @return the light's far value
    */
    irr::f32 CDustbinLight::getFarValue() const {
      return m_fFar;
    }
}
}