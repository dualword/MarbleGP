// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CWorldNode.h>
#include <scenenodes/CDustbinId.h>

namespace dustbin {
  namespace scenenodes {
    CWorldNode::CWorldNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      irr::scene::ISceneNode(a_pParent, a_pMgr, a_iId),
      m_iChildren(0)
    {
      m_cBox.reset(getPosition());

      setScale(irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      sceneNodeIdUsed(a_iId);
    }

    CWorldNode::~CWorldNode() {
    }

    void CWorldNode::render() {
      if (m_iChildren != Children.getSize()) {
        m_cBox.reset(getPosition());

        for (irr::core::list<irr::scene::ISceneNode*>::Iterator it = Children.begin(); it != Children.end(); it++)
          m_cBox.addInternalBox((*it)->getBoundingBox());
      }
    }

    const irr::core::aabbox3d<irr::f32>& CWorldNode::getBoundingBox() const {
      return m_cBox;
    }

    irr::scene::ESCENE_NODE_TYPE CWorldNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_WorldNodeId;
    }

    void CWorldNode::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CWorldNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);
      sceneNodeIdUsed(getID());
    }

    void CWorldNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      setScale   (irr::core::vector3df(1.0f));
      setPosition(irr::core::vector3df(0.0f));
      setRotation(irr::core::vector3df(0.0f));

      sceneNodeIdUsed(getID());
    }

    irr::scene::ISceneNode* CWorldNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent  == nullptr) a_pNewParent  = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CWorldNode* l_pNew = new CWorldNode(a_pNewParent, a_pNewManager, getNextSceneNodeId());
      return l_pNew;
    }
  }
}