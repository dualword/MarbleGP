// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CJointNode.h>
#include <scenenodes/CDustbinId.h>
#include <string>

namespace dustbin {
  namespace scenenodes {

    CJointNode::CJointNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId != -1 ? a_iId : getNextSceneNodeId()),
      m_vAxis               (irr::core::vector3df(0.0f, 1.0f, 0.0f)),
      m_iType               (0),
      m_fHiStop             (0.0f),
      m_fLoStop             (0.0f),
      m_fForce              (0.0f),
      m_fVelocity           (0.0f),
      m_bUseHiStop          (false),
      m_bUseLoStop          (false),
      m_bEnableMotor        (false)
    {
    }

    CJointNode::~CJointNode() {
    }

    void CJointNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CJointNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CJointNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_JointNodeId;
    }

    void CJointNode::OnRegisterSceneNode() {
    }

    void CJointNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::scene::ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addEnum("Type", m_iType < 0 ? 0 : m_iType > 1 ? 1 : m_iType, g_JointTypes);
      a_pOut->addVector3d("Axis", m_vAxis);
      a_pOut->addBool("EnableMotor", m_bEnableMotor);

      if (m_bEnableMotor) {
        a_pOut->addFloat("Velocity", m_fVelocity);
        a_pOut->addFloat("Force"   , m_fForce);
      }

      a_pOut->addBool("UseHiStop", m_bUseHiStop);

      if (m_bUseHiStop)
        a_pOut->addFloat("HiStop", m_fHiStop);

      a_pOut->addBool("UseLoStop", m_bUseLoStop);

      if (m_bUseLoStop)
        a_pOut->addFloat("LoStop", m_fLoStop);
      
    }

    void CJointNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::scene::ISceneNode::deserializeAttributes(a_pIn);

      if (a_pIn->existsAttribute("Type")) {
        const irr::c8* l_sType = a_pIn->getAttributeAsEnumeration("Type");
        if (std::string(l_sType) == g_JointTypes[1])
          m_iType = 1;
        else
          m_iType = 0;
      }
      
      if (a_pIn->existsAttribute("Axis"       )) m_vAxis        = a_pIn->getAttributeAsVector3d("Axis"       );
      if (a_pIn->existsAttribute("EnableMotor")) m_bEnableMotor = a_pIn->getAttributeAsBool    ("EnableMotor");
      if (a_pIn->existsAttribute("Velocity"   )) m_fVelocity    = a_pIn->getAttributeAsFloat   ("Velocity"   );
      if (a_pIn->existsAttribute("Force"      )) m_fForce       = a_pIn->getAttributeAsFloat   ("Force"      );
      if (a_pIn->existsAttribute("UseHiStop"  )) m_bUseHiStop   = a_pIn->getAttributeAsBool    ("UseHiStop"  );
      if (a_pIn->existsAttribute("HiStop"     )) m_fHiStop      = a_pIn->getAttributeAsFloat   ("HiStop"     );
      if (a_pIn->existsAttribute("UseLoStop"  )) m_bUseLoStop   = a_pIn->getAttributeAsBool    ("UseLoStop"  );
      if (a_pIn->existsAttribute("LoStop"     )) m_fLoStop      = a_pIn->getAttributeAsFloat   ("LoStop"     );
    }

    irr::scene::ISceneNode* CJointNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      CJointNode* l_pRet = new CJointNode(a_pNewParent != nullptr ? a_pNewParent : getParent(), a_pNewManager != nullptr ? a_pNewManager : SceneManager, getNextSceneNodeId());

      l_pRet->m_iType        = m_iType;
      l_pRet->m_vAxis        = m_vAxis;
      l_pRet->m_bEnableMotor = m_bEnableMotor;
      l_pRet->m_fVelocity    = m_fVelocity;
      l_pRet->m_fForce       = m_fForce;
      l_pRet->m_bUseHiStop   = m_bUseHiStop;
      l_pRet->m_fHiStop      = m_fHiStop;
      l_pRet->m_bUseLoStop   = m_bUseLoStop;
      l_pRet->m_fLoStop      = m_fLoStop;

      return l_pRet;
    }
  }
}