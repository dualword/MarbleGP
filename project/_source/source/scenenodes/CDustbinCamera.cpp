// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CDustbinCamera.h>

namespace dustbin {
  namespace scenenodes {
    CDustbinCamera::CDustbinCamera(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : 
      ISceneNode(a_pParent != nullptr ? a_pParent : a_pMgr->getRootSceneNode(), a_pMgr, a_iId),
      m_fAngleV(0.0f),
      m_fAngleH(0.0f)
    {

    }

    CDustbinCamera::~CDustbinCamera() {
    }

    void CDustbinCamera::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CDustbinCamera::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CDustbinCamera::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_DustbinCameraId;
    }

    void CDustbinCamera::OnRegisterSceneNode() {
      SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_AUTOMATIC);
    }

    void CDustbinCamera::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addFloat("AngleV", m_fAngleV);
      a_pOut->addFloat("AngleH", m_fAngleH);
    }

    void CDustbinCamera::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      m_fAngleV = a_pIn->getAttributeAsFloat("AngleV");
      m_fAngleH = a_pIn->getAttributeAsFloat("AngleH");
    }

    irr::scene::ISceneNode* CDustbinCamera::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      scenenodes::CDustbinCamera *p = new CDustbinCamera(a_pNewParent != nullptr ? a_pNewParent : Parent, a_pNewManager != nullptr ? a_pNewManager : SceneManager, -1);
      p->m_fAngleH = m_fAngleH;
      p->m_fAngleV = m_fAngleV;
      return p;
    }

    void CDustbinCamera::setValues(const irr::core::vector3df& a_cPosition, irr::f32 a_fAngleV, irr::f32 a_fAngleH) {
      setPosition(a_cPosition);
      m_fAngleV = a_fAngleV;
      m_fAngleH = a_fAngleH;
    }

    void CDustbinCamera::getValues(irr::core::vector3df& a_cPosition, irr::f32& a_fAngleV, irr::f32& a_fAngleH) {
      updateAbsolutePosition();
      a_cPosition = getAbsolutePosition();
      a_fAngleV = m_fAngleV;
      a_fAngleH = m_fAngleH;
    }
  }
}