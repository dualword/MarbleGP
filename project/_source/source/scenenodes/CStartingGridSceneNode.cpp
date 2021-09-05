// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>

namespace dustbin {
  namespace scenenodes {
    CStartingGridSceneNode::CStartingGridSceneNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      CStartingGridSceneNode_Base(a_pParent, a_pMgr, a_iId),
      m_iNextMarble(0)
    {
    }

    CStartingGridSceneNode::~CStartingGridSceneNode() {
    }

    void CStartingGridSceneNode::render() {
    }

    irr::scene::ISceneNode* CStartingGridSceneNode::getNextMarble() {
      irr::scene::ISceneNode* l_pRet = nullptr;

      if (m_iNextMarble < 16) {
        l_pRet = m_pMarbles[m_iNextMarble];
        if (l_pRet != nullptr) {
          // Move the marble to our parent
          l_pRet->updateAbsolutePosition();
          irr::core::vector3df l_cPos = l_pRet->getAbsolutePosition();

          l_pRet->setParent(getParent());
          l_pRet->setPosition(l_cPos);
        }
        m_iNextMarble++;
      }

      return l_pRet;
    }

    void CStartingGridSceneNode::removeUnusedMarbles() {
      while (m_iNextMarble < 16) {
        if (m_pMarbles[m_iNextMarble] != nullptr) {
          m_pMarbles[m_iNextMarble]->setVisible(false);
          m_pMarbles[m_iNextMarble]->getSceneManager()->addToDeletionQueue(m_pMarbles[m_iNextMarble]);
        }

        m_iNextMarble++;
      }
    }

    irr::scene::ISceneNode* CStartingGridSceneNode::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent == nullptr) a_pNewParent = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CStartingGridSceneNode* l_pRet = new CStartingGridSceneNode(a_pNewParent, a_pNewManager, ID);

      l_pRet->m_iMarblesPerRow = m_iMarblesPerRow;
      l_pRet->m_fWidth = m_fWidth;
      l_pRet->m_fRowLength = m_fRowLength;
      l_pRet->m_fOffset = m_fOffset;
      l_pRet->m_fAngle = m_fAngle;

      return l_pRet;
    }

    irr::f32 CStartingGridSceneNode::getAngle() {
      return m_fAngle;
    }
  }
}