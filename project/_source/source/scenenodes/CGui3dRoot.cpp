// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CGui3dRoot.h>

namespace dustbin {
  namespace scenenodes {
    CGui3dRoot::CGui3dRoot(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : CGui3dRootBase(a_pParent, a_pSmgr, a_iId),
      m_pClick (nullptr),
      m_pHover (nullptr),
      m_pSelect(nullptr),
      m_pCursor(nullptr)
    {
    }

    CGui3dRoot::~CGui3dRoot() {
      if (m_pSelector != nullptr)
        m_pSelector->drop();
    }

    /**
    * Initialize the 3d GUI members for handling events
    * @param a_pCursor the Irrlicht cursor control
    */
    void CGui3dRoot::initGui3d() {
      std::vector<dustbin::scenenodes::CGui3dItem *> l_vItems;
      getGuiItems(this, l_vItems);

      m_pColMgr   = m_pSmgr->getSceneCollisionManager();
      m_pSelector = m_pSmgr->createMetaTriangleSelector();

      for (std::vector<dustbin::scenenodes::CGui3dItem*>::iterator it = l_vItems.begin(); it != l_vItems.end(); it++) {
        irr::scene::ISceneNode *l_pNode = (*it)->getParent();
        if (l_pNode->getType() == irr::scene::ESNT_MESH) {
          irr::scene::IMeshSceneNode *l_pMesh = reinterpret_cast<irr::scene::IMeshSceneNode *>(l_pNode);
          irr::scene::ITriangleSelector *l_pSelector = m_pSmgr->createTriangleSelector(l_pMesh->getMesh(), l_pMesh);
          l_pMesh->setTriangleSelector(l_pSelector);
          m_pSelector->addTriangleSelector(l_pSelector);

          m_mItemScenenodeMap[(*it)->getParent()] = *it;
        }
      }

      m_pSmgr->drawAll();
    }

    /**
    * Call this function to react to user input
    */
    void CGui3dRoot::step() {
      if (m_pCursor != nullptr && m_pColMgr != nullptr) {
        irr::core::line3df l_cLine = m_pColMgr->getRayFromScreenCoordinates(m_pCursor->getPosition(), m_pSmgr->getActiveCamera());

        irr::core::vector3df    l_cPoint;
        irr::core::triangle3df  l_cTriangle;
        irr::scene::ISceneNode *l_pNode = m_pColMgr->getSceneNodeAndCollisionPointFromRay(l_cLine, l_cPoint, l_cTriangle);

        CGui3dItem *l_pHover = nullptr;

        if (l_pNode != nullptr) {

          if (m_mItemScenenodeMap.find(l_pNode) != m_mItemScenenodeMap.end()) {
            l_pHover = reinterpret_cast<CGui3dItem *>(m_mItemScenenodeMap[l_pNode]);

          }
        }

        if (l_pHover != m_pHover) {
          if (m_pHover != nullptr)
            m_pHover->itemLeft();

          m_pHover = l_pHover;

          if (m_pHover != nullptr)
            m_pHover->itemEntered();
        }
      }
    }

    /**
    * Set the Irrlicht Cursor Control instance
    * @param a_pCursor the Irrlicht Cursor Control instance
    */
    void CGui3dRoot::setCursorControl(irr::gui::ICursorControl* a_pCursor) {
      m_pCursor = a_pCursor;
    }

    /**
     * Get all children which are CGui3dItem instances
     * @param a_pParent this node is checked for the type and it's children are iterated to find more
     * @param a_vItems this vectr is filled with the children found
     * @see CGui3dItem
     */
    void CGui3dRoot::getGuiItems(irr::scene::ISceneNode* a_pParent, std::vector<dustbin::scenenodes::CGui3dItem *>& a_vItems) {
      if (a_pParent->getType() == (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiItemID)
        a_vItems.push_back(reinterpret_cast<dustbin::scenenodes::CGui3dItem *>(a_pParent));

      for (irr::core::list<irr::scene::ISceneNode*>::ConstIterator it = a_pParent->getChildren().begin(); it != a_pParent->getChildren().end(); it++) {
        getGuiItems(*it, a_vItems);
      }
    }
  }
}