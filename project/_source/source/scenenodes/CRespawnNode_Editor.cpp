// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CRespawnNode_Editor.h>
#include <scenenodes/CDustbinId.h>

namespace dustbin {
  namespace scenenodes {
    CRespawnNode_Editor::CRespawnNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) : CRespawnNode(a_pParent, a_pMgr, a_iId) {
      irr::scene::IMesh* l_pMesh = a_pMgr->getMeshCache()->getMeshByFilename("data/objects/respawn_editor.3ds");

      if (l_pMesh == nullptr) {
        l_pMesh = a_pMgr->getMesh("data/objects/respawn_editor.3ds");

        if (l_pMesh != nullptr) {
          for (irr::u32 i = 0; i < l_pMesh->getMeshBufferCount(); i++) {
            irr::scene::IMeshBuffer* l_pBuffer = l_pMesh->getMeshBuffer(i);
            irr::video::S3DVertex* l_pVertices = (irr::video::S3DVertex*)l_pBuffer->getVertices();

            for (irr::u32 j = 0; j < l_pBuffer->getVertexCount(); j++) {
              l_pVertices[j].Color = irr::video::SColor(0xFF, 0, 0, 0xFF);
            }
          }
        }
      }

      if (l_pMesh != nullptr) {
        irr::scene::ISceneNode* l_pNode = a_pMgr->addMeshSceneNode(l_pMesh, this);
        l_pNode->setIsDebugObject(true);
        l_pNode->setScale(irr::core::vector3df(5.0f));

        for (unsigned i = 0; i < l_pNode->getMaterialCount(); i++) {
          l_pNode->getMaterial(i).Lighting = false;
        }
      }
    }

    CRespawnNode_Editor::~CRespawnNode_Editor() {

    }

    void CRespawnNode_Editor::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    irr::scene::ISceneNode* CRespawnNode_Editor::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent == nullptr) a_pNewParent = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CRespawnNode* l_pNew = new CRespawnNode_Editor(a_pNewParent, a_pNewManager, getNextSceneNodeId());

      l_pNew->setPosition(getPosition());
      l_pNew->setRotation(getRotation());
      l_pNew->setScale(getScale());

      l_pNew->drop();
      return l_pNew;
    }
  }
}