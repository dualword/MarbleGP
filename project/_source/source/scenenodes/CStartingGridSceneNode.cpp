// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode.h>
#include <shaders/CDustbinShaders.h>
#include <scenenodes/CDustbinId.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace scenenodes {
    CStartingGridSceneNode::CStartingGridSceneNode(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      ISceneNode      (a_pParent, a_pMgr, a_iId),
      m_iNextMarble   (0),
      m_iMarblesPerRow(2),
      m_fWidth        (10.0f),
      m_fRowLength    (50.0f),
      m_fOffset       (0.2f),
      m_fAngle        (0.0f)
    {
      m_cMaterial.Wireframe = false;
      m_cMaterial.Lighting = false;

      irr::scene::IAnimatedMesh* l_pMesh = a_pMgr->getMesh("data/objects/sphere.obj");

      if (l_pMesh != nullptr)
        for (int i = 0; i < 16; i++) {
          m_pMarbles[i] = new gameclasses::SMarbleNodes();

          m_pMarbles[i]->m_pPositional = a_pMgr->addEmptySceneNode(this, 10000 + i);
          m_pMarbles[i]->m_pPositional->setIsDebugObject(true);
          m_pMarbles[i]->m_pPositional->setPosition(irr::core::vector3df(0.0f, 2.25f, 0.0f));

          m_pMarbles[i]->m_pRotational = a_pMgr->addMeshSceneNode(l_pMesh, m_pMarbles[i]->m_pPositional);
          m_pMarbles[i]->m_pRotational->getMaterial(0).setTexture(0, i == 0 ? a_pMgr->getVideoDriver()->getTexture("data/textures/spheretexture_blue.png") : a_pMgr->getVideoDriver()->getTexture("data/textures/spheretexture.png"));
          // m_pMarbles[i]->m_pRotational->getMaterial(0).Lighting = true;
          // m_pMarbles[i]->m_pRotational->getMaterial(0).AmbientColor = irr::video::SColor(255, 16, 16, 16);
          // m_pMarbles[i]->m_pRotational->getMaterial(0).EmissiveColor = irr::video::SColor(255, 32, 32, 32);
          // m_pMarbles[i]->m_pRotational->getMaterial(0).Shininess = 50;
          m_pMarbles[i]->m_pRotational->setName(std::string("Marble_" + std::to_string(i + 1)).c_str());

          m_pMarbles[i]->m_pRotational->getMesh()->getMeshBuffer(0)->setDirty();
        }

      updateGridPositions();
    }

    CStartingGridSceneNode::~CStartingGridSceneNode() {
      for (int i = 0; i < 16; i++)
        if (m_pMarbles[i] != nullptr) {
          delete m_pMarbles[i];
          m_pMarbles[i] = nullptr;
        }
    }

    void CStartingGridSceneNode::render() {
    }

    /**
    * Get a marble scene node without removing it from the list
    * @param a_iMarbleID the ID of the marble
    * @return the marble's positional scene node
    */
    irr::scene::ISceneNode *CStartingGridSceneNode::getMarbleById(int a_iMarbleID) {
      irr::scene::ISceneNode *l_pRet = nullptr;

      for (int i = 0; i < 16; i++) {
        if (m_pMarbles[i] != nullptr && m_pMarbles[i]->m_pPositional != nullptr && m_pMarbles[i]->m_pPositional->getID() == a_iMarbleID) {
          l_pRet = m_pMarbles[i]->m_pPositional;
          m_pMarbles[i] = nullptr;
        }
      }

      return l_pRet;
    }

    gameclasses::SMarbleNodes *CStartingGridSceneNode::getMarble(int a_iMarbleID) {
      for (int i = 0; i < 16; i++) {
        if (m_pMarbles[i] != nullptr && m_pMarbles[i]->m_pPositional != nullptr && m_pMarbles[i]->m_pPositional->getID() == a_iMarbleID) {
          gameclasses::SMarbleNodes *l_pRet = m_pMarbles[i];

          // Move the marble to our parent
          l_pRet->m_pPositional->updateAbsolutePosition();
          irr::core::vector3df l_cPos = l_pRet->m_pPositional->getAbsolutePosition();

          l_pRet->m_pPositional->setParent(getParent());
          l_pRet->m_pPositional->setPosition(l_cPos);

          m_pMarbles[i] = nullptr;

          return l_pRet;
        }
      }

      return nullptr;
    }

    void CStartingGridSceneNode::removeUnusedMarbles() {
      for (int i = 0; i < 16; i++) {
        if (m_pMarbles[i] != nullptr) {
          m_pMarbles[i]->m_pPositional->setVisible(false);
          m_pMarbles[i]->m_pPositional->getSceneManager()->addToDeletionQueue(m_pMarbles[i]->m_pPositional);
          delete m_pMarbles[i];
          m_pMarbles[i] = nullptr;
        }
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

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CStartingGridSceneNode::getMaterialCount() const {
      return 0;
    }

    irr::scene::ESCENE_NODE_TYPE CStartingGridSceneNode::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
    }

    const irr::core::aabbox3d<irr::f32>& CStartingGridSceneNode::getBoundingBox() const {
      return m_cBox;
    }

    void CStartingGridSceneNode::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addInt("MarblesPerRow", m_iMarblesPerRow);
      a_pOut->addFloat("Width", m_fWidth);
      a_pOut->addFloat("RowLength", m_fRowLength);
      a_pOut->addFloat("Angle", m_fAngle);
      a_pOut->addFloat("Offset", m_fOffset);

      sceneNodeIdUsed(getID());
    }

    void CStartingGridSceneNode::updateGridPositions() {
      m_cBox.reset(getPosition());

      irr::f32 l_fOffset = m_fOffset / m_iMarblesPerRow;

      for (int i = 0; i < 16; i++) {
        int l_iRow = i / m_iMarblesPerRow,
          l_iCol = i % m_iMarblesPerRow;


        irr::f32 l_fX = m_iMarblesPerRow == 1 ? 0.0f : (l_iCol * m_fWidth / (m_iMarblesPerRow - 1.0f)) - (m_fWidth / 2.0f),
          l_fY = (l_iRow * m_fRowLength) + (l_iCol * l_fOffset * m_fRowLength);

        irr::core::vector2df v = irr::core::vector2df(l_fX, l_fY);

        v.rotateBy(m_fAngle);

        // Position and rotation of the positional node are updated. The rotation of the
        // m_pRotational member will be changed with the data from the physics engine.
        m_pMarbles[i]->m_pPositional->setPosition(irr::core::vector3df(v.X, 2.5f, v.Y));
        m_pMarbles[i]->m_pPositional->setRotation(irr::core::vector3df(0.0f, -m_fAngle, 0.0f));

        m_cBox.addInternalPoint(irr::core::vector3df(v.X, 2.5f, v.Y));
      }
    }

    void CStartingGridSceneNode::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      // No scaling is possible
      setScale(irr::core::vector3df(1.0f));

      m_iMarblesPerRow = a_pIn->getAttributeAsInt("MarblesPerRow");
      m_fWidth = a_pIn->getAttributeAsFloat("Width");
      m_fRowLength = a_pIn->getAttributeAsFloat("RowLength");
      m_fAngle = a_pIn->getAttributeAsFloat("Angle");
      m_fOffset = a_pIn->getAttributeAsFloat("Offset");

      if (m_fOffset < 0.0f)
        m_fOffset = 0.0f;
      else if (m_fOffset > 0.75f)
        m_fOffset = 0.75f;

      if (m_iMarblesPerRow < 1)
        m_iMarblesPerRow = 1;

      if (m_iMarblesPerRow > 16)
        m_iMarblesPerRow = 16;

      updateGridPositions();

      sceneNodeIdUsed(getID());
    }
  }
}