// (w) 2021 by Dustbin::Games / Christian Keimel

#include <scenenodes/CRostrumNode.h>
#include <scenenodes/CPhysicsNode.h>

namespace dustbin {
  namespace scenenodes {
    CRostrumNode::CRostrumNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : ISceneNode(a_pParent != nullptr ? a_pParent : a_pMgr->getRootSceneNode(), a_pMgr, a_iId) {
      setScale(irr::core::vector3df(15.0f));

      irr::scene::IAnimatedMesh *l_pMesh = a_pMgr->getMesh("data/objects/rostrum.3ds");

      if (l_pMesh) {
        irr::scene::IMeshSceneNode *l_pNode = a_pMgr->addMeshSceneNode(l_pMesh->getMesh(0), this, -1);
        l_pNode->setIsDebugObject(true);

        for (int i = 0; i < (int)l_pNode->getMaterialCount(); i++) {
          l_pNode->getMaterial(i).EmissiveColor = irr::video::SColor(0xFF, 0xA0, 0xA0, 0xA0);
          l_pNode->getMaterial(i).MaterialType  = irr::video::EMT_SOLID;

          if (i == 0)
            l_pNode->getMaterial(i).setTexture(0, a_pMgr->getVideoDriver()->getTexture("data/textures/warning.png"));
          else if (i == 2)
            l_pNode->getMaterial(i).setTexture(0, a_pMgr->getVideoDriver()->getTexture("data/textures/red_cross.png"));
          else if (i == 3)
            l_pNode->getMaterial(i).setTexture(0, a_pMgr->getVideoDriver()->getTexture("data/textures/rostrum.png"));
        }

        irr::scene::ISceneNode *l_pPhysics = a_pMgr->addSceneNode(scenenodes::g_PhysicsNodeName, l_pNode);

        if (l_pPhysics) {
          scenenodes::CPhysicsNode *l_pNode = reinterpret_cast<scenenodes::CPhysicsNode *>(l_pPhysics);
          irr::io::IAttributes     *l_pAttr = SceneManager->getFileSystem()->createEmptyAttributes();

          l_pNode->serializeAttributes(l_pAttr);

          l_pAttr->setAttribute("Type"    , "Trimesh");
          l_pAttr->setAttribute("Static"  , true);
          l_pAttr->setAttribute("Collides", true);

          l_pNode->deserializeAttributes(l_pAttr);

          l_pAttr->drop();
        }
      }
    }

    CRostrumNode::~CRostrumNode() {
    }

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CRostrumNode::getMaterialCount() {
      return 0;
    }

    irr::scene::ESCENE_NODE_TYPE CRostrumNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_RostrumNodeId;
    }

    void CRostrumNode::OnRegisterSceneNode() {
      SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);
    }

    void CRostrumNode::render() {
      for (irr::core::list<irr::scene::ISceneNode *>::ConstIterator it = Children.begin(); it != Children.end(); it++)
        (*it)->render();
    }

    const irr::core::aabbox3d<irr::f32> &CRostrumNode::getBoundingBox() const {
      return m_cBox;
    }

    void CRostrumNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);
    }

    void CRostrumNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);
    }

    irr::scene::ISceneNode *CRostrumNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      return new CRostrumNode(a_pNewParent != nullptr ? a_pNewParent : Parent, a_pNewManager != nullptr ? a_pNewManager : SceneManager, -1);
    }

    irr::core::vector3df CRostrumNode::getRostrumPosition(int a_iPosition) {
      int l_iRow = a_iPosition / 4;
      int l_iCol = a_iPosition % 4;

      irr::core::vector3df l_cRet = irr::core::vector3df(-0.75f, 0.5f, 0.75f);


      l_cRet.X += l_iCol * 0.5f;
      l_cRet.Y -= 0.5f + a_iPosition * 0.1f;
      l_cRet.Z -= l_iRow * 0.5f;

      AbsoluteTransformation.transformVect(l_cRet);

      return l_cRet;
    }

    irr::core::vector3df CRostrumNode::getCameraPosition() {
      irr::core::vector3df l_cPos = irr::core::vector3df(0.0f, 0.75f, -2.25f);
      AbsoluteTransformation.transformVect(l_cPos);
      return l_cPos;
    }
  }
}
