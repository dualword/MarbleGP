// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CPhysicsNode.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    const irr::c8* const g_aOdeEntityTypes[] = {
      "Trimesh",
      "Sphere",
      "Box",
      0
    };

    CPhysicsNode::CPhysicsNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      ISceneNode(a_pParent, a_pMgr, a_iId),
      m_eType(enNodeType::Trimesh),
      m_bCollides(true),
      m_bStatic(true),
      m_fMass(1.0)
    {
      m_cBox.reset(getPosition());
    }

    CPhysicsNode::~CPhysicsNode() {
    }

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CPhysicsNode::getMaterialCount() {
      return 0;
    }

    irr::scene::ESCENE_NODE_TYPE CPhysicsNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_PhysicsNodeId;
    }

    void CPhysicsNode::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CPhysicsNode::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CPhysicsNode::getBoundingBox() const {
      return m_cBox;
    }

    void CPhysicsNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addEnum("Type", (irr::s32)m_eType, g_aOdeEntityTypes);

      a_pOut->addBool ("static"  , m_bStatic  );
      a_pOut->addBool ("collides", m_bCollides);

      if (!m_bStatic)
        a_pOut->addFloat("mass", m_fMass);

      for (size_t i = 0; i < m_vTrigger.size() && i < m_vRespawn.size(); i++) {
        std::string l_sNameFlag = "DoesTrigger_" + std::to_string(i + 1),
                    l_sNameTrgr = "Trigger_" + std::to_string(i + 1),
                    l_sNameRspn = "Respawn_" + std::to_string(i + 1);

        a_pOut->addBool(l_sNameRspn.c_str(), m_vRespawn[i]);
        a_pOut->addBool(l_sNameFlag.c_str(), std::get<0>(m_vTrigger[i]));

        if (std::get<0>(m_vTrigger[i]))
          a_pOut->addInt(l_sNameTrgr.c_str(), std::get<1>(m_vTrigger[i]));
      }
    }

    void CPhysicsNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      if (Parent != nullptr && Parent->getType() == irr::scene::ESNT_MESH) {
        irr::scene::IMeshSceneNode* l_pParent = reinterpret_cast<irr::scene::IMeshSceneNode*>(Parent);

        if (a_pIn->existsAttribute("static"))
          m_bStatic = a_pIn->getAttributeAsBool("static");

        if (a_pIn->existsAttribute("collides"))
          m_bCollides = a_pIn->getAttributeAsBool("collides");

        if (a_pIn->existsAttribute("mass"))
          m_fMass = a_pIn->getAttributeAsFloat("mass");

        m_vTrigger.clear();
        m_vRespawn.clear();

        for (unsigned i = 0; i < l_pParent->getMaterialCount(); i++) {
          std::string l_sNameFlag = "DoesTrigger_" + std::to_string(i + 1),
                      l_sNameTrgr = "Trigger_"     + std::to_string(i + 1),
                      l_sNameRspn = "Respawn_"     + std::to_string(i + 1);

          bool     b = false,
                   r = false;
          irr::s32 t = 0;

          if (a_pIn->existsAttribute(l_sNameFlag.c_str()))
            b = a_pIn->getAttributeAsBool(l_sNameFlag.c_str());

          if (a_pIn->existsAttribute(l_sNameTrgr.c_str()))
            t = a_pIn->getAttributeAsInt(l_sNameTrgr.c_str());

          if (a_pIn->existsAttribute(l_sNameRspn.c_str()))
            r = a_pIn->getAttributeAsBool(l_sNameRspn.c_str());

          irr::u8 t8 = (t > 255 ? 255 : t < 0 ? 0 : (irr::u8)t);
          m_vTrigger.push_back(std::make_tuple(b, t8));
          m_vRespawn.push_back(r);
        }

        if (a_pIn->existsAttribute("Type")) {
          std::string l_sType = a_pIn->getAttributeAsEnumeration("Type");

          for (int i = 0; g_aOdeEntityTypes[i] != 0; i++) {
            if (l_sType == g_aOdeEntityTypes[i])
              m_eType = (enNodeType)i;
          }
        }
      }
    }

    irr::scene::ISceneNode* CPhysicsNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent  == nullptr) a_pNewParent  = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CPhysicsNode* l_pNew = new CPhysicsNode(a_pNewParent, a_pNewManager, ID);

      l_pNew->m_bCollides = m_bCollides;
      l_pNew->m_bStatic   = m_bStatic;
      l_pNew->m_fMass     = m_fMass;

      for (std::vector<std::tuple<bool, irr::u8> >::iterator it = m_vTrigger.begin(); it != m_vTrigger.end(); it++) {
        l_pNew->m_vTrigger.push_back(std::make_tuple(std::get<0>(*it), std::get<1>(*it)));
      }

      for (std::vector<bool>::iterator it = m_vRespawn.begin(); it != m_vRespawn.end(); it++) {
        l_pNew->m_vRespawn.push_back(*it);
      }

      return l_pNew;
    }

    CPhysicsNode::enNodeType CPhysicsNode::getNodeType() {
      return m_eType;
    }
  }
}