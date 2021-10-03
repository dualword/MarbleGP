// (w) 2021 by Dustbin::Games / Christian Keimel

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CCheckpointNode_Editor.h>
#include <scenenodes/CRespawnNode.h>

namespace dustbin {
  namespace scenenodes {


    CCheckpointNode_Editor::CCheckpointNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CCheckpointNode(a_pParent, a_pMgr, a_iId),
      m_iShowLinks(-1)
    {
    }

    CCheckpointNode_Editor::~CCheckpointNode_Editor() {
    }

    void CCheckpointNode_Editor::render() {
    }

    void CCheckpointNode_Editor::OnRegisterSceneNode() {
      if (IsVisible && m_mLinks.find(m_iShowLinks) != m_mLinks.end())
        SceneManager->registerNodeForRendering(this, irr::scene::ESNRP_SOLID);

      ISceneNode::OnRegisterSceneNode();
    }

    void CCheckpointNode_Editor::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      CCheckpointNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addInt("ShowLinks", m_iShowLinks);
    }

    void CCheckpointNode_Editor::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      CCheckpointNode::deserializeAttributes(a_pIn, a_pOptions);

      if (a_pIn->existsAttribute("ShowLinks")) {
        int l_iShowLinks = a_pIn->getAttributeAsInt("ShowLinks");

        if (l_iShowLinks != m_iShowLinks) {
          for (std::map<irr::s32, std::vector<irr::scene::ISceneNode*>>::iterator it = m_mLinkMeshes.begin(); it != m_mLinkMeshes.end(); it++) {
            for (std::vector<irr::scene::ISceneNode*>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
              (*it2)->setVisible(it->first == l_iShowLinks);
          }
          m_iShowLinks = l_iShowLinks;
        }
      }
      else m_iShowLinks = -1;

      updateAbsolutePosition();
      getParent()->updateAbsolutePosition();

      m_cBox.reset(getAbsolutePosition());
      m_cBox.addInternalPoint(getParent()->getTransformedBoundingBox().getCenter());

      irr::core::vector3df l_cPos   = getParent()->getTransformedBoundingBox().getCenter(),
                           l_cScale = getParent()->getScale();

      for (std::map<irr::s32, std::vector<irr::s32> >::iterator it = m_mLinks.begin(); it != m_mLinks.end(); it++) {
        for (std::vector<int>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
          irr::scene::ISceneNode* l_pNext = SceneManager->getSceneNodeFromId(*it2);
          if (l_pNext != nullptr && l_pNext->getType() == (irr::scene::ESCENE_NODE_TYPE)g_CheckpointNodeId) {
            CCheckpointNode* p = reinterpret_cast<CCheckpointNode*>(l_pNext);
            p->addPreviousNode(this->getID());

            if (m_mLinkMeshes.find(*it2) == m_mLinkMeshes.end()) {
              irr::scene::IMesh* l_pMesh = SceneManager->getMesh("data/objects/Link.3ds");
              if (l_pMesh != nullptr) {
                irr::scene::ISceneNode* l_pNode = SceneManager->addMeshSceneNode(l_pMesh, this);

                irr::core::vector3df l_cNextPos = l_pNext->getParent()->getTransformedBoundingBox().getCenter();

                irr::core::vector3df l_cDiff = l_cPos - l_cNextPos,
                                     l_cRot  = l_cDiff.getHorizontalAngle();

                irr::f32 l_fLength = l_cDiff.getLength();

                l_pNode->setRotation(l_cRot - getParent()->getRotation());
                l_pNode->setPosition((l_cPos - getAbsolutePosition()) / getParent()->getScale());
                l_pNode->setScale(irr::core::vector3df(3.0f, 3.0f, l_fLength) / l_cScale);
                l_pNode->setIsDebugObject(true);
                l_pNode->setVisible(m_iShowLinks == it->first);

                if (m_mLinkMeshes.find(it->first) == m_mLinkMeshes.end())
                  m_mLinkMeshes[it->first] = std::vector<irr::scene::ISceneNode*>();

                m_mLinkMeshes[it->first].push_back(l_pNode);
              }
            }
          }
        }

        for (std::map<irr::s32, irr::s32>::iterator it = m_mRespawn.begin(); it != m_mRespawn.end(); it++) {
          irr::scene::ISceneNode* l_pRespawn = SceneManager->getSceneNodeFromId(it->second);
          if (l_pRespawn != nullptr && l_pRespawn->getType() == (irr::scene::ESCENE_NODE_TYPE)scenenodes::g_RespawnNodeId && m_mLinkMeshes.find(it->second) == m_mLinkMeshes.end()) {
            irr::scene::IMesh* l_pMesh = SceneManager->getMesh("data/objects/Link.3ds");
            if (l_pMesh != nullptr) {
              irr::scene::ISceneNode* l_pNode = SceneManager->addMeshSceneNode(l_pMesh, this);

              irr::core::vector3df l_cRespawnPos = l_pRespawn->getTransformedBoundingBox().getCenter(),
                                   l_cDiff       = l_cPos - l_cRespawnPos,
                                   l_cRot        = l_cDiff.getHorizontalAngle();

              irr::f32 l_fLength = l_cDiff.getLength();

              l_pNode->setRotation(l_cRot - getParent()->getRotation());
              l_pNode->setPosition((l_cPos - getAbsolutePosition()) / getParent()->getScale());
              l_pNode->setScale(irr::core::vector3df(3.0f, 3.0f, l_fLength) / l_cScale);
              l_pNode->setIsDebugObject(true);
              l_pNode->setVisible(m_iShowLinks == it->first);

              if (m_mLinkMeshes.find(it->second) == m_mLinkMeshes.end())
                m_mLinkMeshes[it->second] = std::vector<irr::scene::ISceneNode*>();

              m_mLinkMeshes[it->second].push_back(l_pNode);
            }
          }
        }
      }
    }

    /**
    * Remove a previous node
    * @param a_iId ID of the previous node
    */
    void CCheckpointNode_Editor::removePreviousNode(int a_iId) {
      if (m_mLinkMeshes.find(a_iId) != m_mLinkMeshes.end()) {
        for (std::vector<irr::scene::ISceneNode*>::iterator it = m_mLinkMeshes[a_iId].begin(); it != m_mLinkMeshes[a_iId].end(); it++) {
          (*it)->setVisible(false);
          SceneManager->addToDeletionQueue(*it);
        }
      }
    }
  }
}