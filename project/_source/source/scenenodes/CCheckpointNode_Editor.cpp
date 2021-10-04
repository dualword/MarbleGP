// (w) 2021 by Dustbin::Games / Christian Keimel

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CCheckpointNode_Editor.h>
#include <scenenodes/CRespawnNode.h>
#include <algorithm>

namespace dustbin {
  namespace scenenodes {


    CCheckpointNode_Editor::CCheckpointNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CCheckpointNode(a_pParent, a_pMgr, a_iId),
      m_bShowLinks(false)
    {
    }

    CCheckpointNode_Editor::~CCheckpointNode_Editor() {
    }

    void CCheckpointNode_Editor::render() {
    }

    void CCheckpointNode_Editor::OnRegisterSceneNode() {
      if (IsVisible && m_bShowLinks)
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);

      ISceneNode::OnRegisterSceneNode();
    }

    void CCheckpointNode_Editor::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      CCheckpointNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addBool("ShowLinks", m_bShowLinks);
    }

    void CCheckpointNode_Editor::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      CCheckpointNode::deserializeAttributes(a_pIn, a_pOptions);

      if (a_pIn->existsAttribute("ShowLinks")) {
        bool l_bShowLinks = a_pIn->getAttributeAsBool("ShowLinks");

        if (l_bShowLinks != m_bShowLinks) {

          m_bShowLinks = l_bShowLinks;
        }
      }
      else m_bShowLinks = false;

      updateAbsolutePosition();
      getParent()->updateAbsolutePosition();

      m_cBox.reset(getAbsolutePosition());
      m_cBox.addInternalPoint(getParent()->getTransformedBoundingBox().getCenter());

      if (m_bShowLinks) {
        irr::core::vector3df l_cPos   = getParent()->getTransformedBoundingBox().getCenter(),
                             l_cScale = getParent()->getScale();

        // first of all we create a vector with all link mesh IDs
        std::vector<irr::s32> l_vLinkMeshes;

        for (std::map<irr::s32, irr::scene::ISceneNode*>::iterator it = m_mLinkMeshes.begin(); it != m_mLinkMeshes.end(); it++)
          l_vLinkMeshes.push_back(it->first);

        // Next we iterate all the links of the checkpoint
        for (std::vector<irr::s32>::iterator it = m_vLinks.begin(); it != m_vLinks.end(); it++) {
          if (m_mLinkMeshes.find(*it) == m_mLinkMeshes.end()) {
            irr::scene::ISceneNode* l_pLinked = SceneManager->getSceneNodeFromId(*it);
            if (l_pLinked != nullptr) {
              irr::scene::IMesh* l_pMesh = SceneManager->getMesh("data/objects/Link.3ds");
              if (l_pMesh != nullptr) {
                irr::scene::ISceneNode* l_pNode = SceneManager->addMeshSceneNode(l_pMesh, this);
                l_pNode->setIsDebugObject(true);
                m_mLinkMeshes[*it] = l_pNode;
                m_cBox.reset(getPosition());
                m_cBox.addInternalBox(l_pNode->getBoundingBox());
              }
            }
          }
          else {
            // Remove the ID from the local vector of all the link IDs
            for (std::vector<irr::s32>::iterator it2 = l_vLinkMeshes.begin(); it2 != l_vLinkMeshes.end(); it2++) {
              if (*it2 == *it) {
                l_vLinkMeshes.erase(it2);
                break;
              }
            }
          }
        }

        irr::scene::ISceneNode* l_pRespawn = SceneManager->getSceneNodeFromId(m_iRespawn);

        if (l_pRespawn != nullptr) {
          for (std::vector<irr::s32>::iterator it = l_vLinkMeshes.begin(); it != l_vLinkMeshes.end(); it++) {
            if (*it == m_iRespawn) {
              l_vLinkMeshes.erase(it);
              break;
            }
          }

          if (m_mLinkMeshes.find(m_iRespawn) == m_mLinkMeshes.end()) {
            irr::scene::IMesh* l_pMesh = SceneManager->getMesh("data/objects/Link.3ds");
            if (l_pMesh != nullptr) {
              irr::scene::ISceneNode* l_pNode = SceneManager->addMeshSceneNode(l_pMesh, this);
              l_pNode->setIsDebugObject(true);
              m_mLinkMeshes[m_iRespawn] = l_pNode;
            }
          }
        }

        // l_vLinkMeshes does now contain link nodes that are no longer used so we clean up m_mLinkMeshes
        for (std::vector<irr::s32>::iterator it = l_vLinkMeshes.begin(); it != l_vLinkMeshes.end(); it++) {
          if (m_mLinkMeshes.find(*it) != m_mLinkMeshes.end()) {
            m_mLinkMeshes[*it]->setVisible(false);
            SceneManager->addToDeletionQueue(m_mLinkMeshes[*it]);
            m_mLinkMeshes.erase(*it);
          }
        }

        // We need to adjust rotation and scaling of the link meshes
        for (std::map<irr::s32, irr::scene::ISceneNode*>::iterator it = m_mLinkMeshes.begin(); it != m_mLinkMeshes.end(); it++) {
          irr::scene::ISceneNode* l_pLinked = SceneManager->getSceneNodeFromId(it->first);
          if (l_pLinked != nullptr) {
            irr::core::vector3df l_cLinkPos  = l_pLinked->getType() == (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId ? l_pLinked->getParent()->getTransformedBoundingBox().getCenter() : l_pLinked->getAbsolutePosition() + irr::core::vector3df(0.0f, 10.0f, 0.0f),
                                 l_cDiff     = l_cPos - l_cLinkPos,
                                 l_cRotation = l_cDiff.getHorizontalAngle();

            irr::f32 l_fLength = l_cDiff.getLength();

            it->second->setRotation(l_cRotation - getParent()->getRotation());
            it->second->setPosition(irr::core::vector3df(0.0f, 2.5f, 0.0f) / getParent()->getScale());
            it->second->setScale(irr::core::vector3df(3.0f, 3.0f, l_fLength) / l_cScale);
            it->second->setVisible(m_bShowLinks);
          }
        }
      }
      else {
        for (std::map<irr::s32, irr::scene::ISceneNode*>::iterator it = m_mLinkMeshes.begin(); it != m_mLinkMeshes.end(); it++) {
          it->second->setVisible(false);
        }
      }
    }
  }
}