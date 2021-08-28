// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CStartingGridSceneNode_Editor.h>

namespace dustbin {
  namespace scenenodes {
    CStartingGridSceneNode_Editor::CStartingGridSceneNode_Editor(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pMgr, irr::s32 a_iId) :
      ISceneNode(a_pParent, a_pMgr, a_iId),
      m_iMarblesPerRow(2),
      m_fRowLength(50.0f),
      m_fWidth(10.0f),
      m_fOffset(0.2f),
      m_fAngle(0.0f)
    {
      m_cMaterial.Wireframe = false;
      m_cMaterial.Lighting  = false;

      m_aVertices[0] = irr::video::S3DVertex(0.0f, 5.0f, 0.0f, 0.0f, 1.0f, 0.0f, irr::video::SColor(255, 255, 255, 0), 0, 0);

      for (int i = 1; i <= 18; i++) {
        irr::core::vector2df v = irr::core::vector2df(1.0f, 0.0f);
        v.rotateBy(20.0 * (irr::f64)(i));

        m_aVertices[i] = irr::video::S3DVertex(3.0f * v.X, 5.0f, 3.0f * v.Y, 0.0f, 1.0f, 0.0f, irr::video::SColor(255, 255, 255, 0), 0, 0);
      }
      m_aVertices[19] = irr::video::S3DVertex(0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, irr::video::SColor(255, 255, 255, 0), 0, 0);

      irr::scene::IAnimatedMesh* l_pMesh = a_pMgr->getMesh("data/objects/sphere.obj");

      if (l_pMesh != nullptr)
        for (int i = 0; i < 16; i++) {
          m_pMarbles[i] = a_pMgr->addMeshSceneNode(l_pMesh, this);
          m_pMarbles[i]->setIsDebugObject(true);
          m_pMarbles[i]->setPosition(irr::core::vector3df(0.0f, 2.25f, 0.0f));
          m_pMarbles[i]->getMaterial(0).setTexture(0, i == 0 ? a_pMgr->getVideoDriver()->getTexture("data/textures/spheretexture_blue.png") : a_pMgr->getVideoDriver()->getTexture("data/textures/spheretexture.png"));
          m_pMarbles[i]->getMaterial(0).Lighting = true;
        }

      updateGridPositions();
    }

    CStartingGridSceneNode_Editor::~CStartingGridSceneNode_Editor() {

    }

    //*** Virtual method inherited from irr::scene::ISceneNode
    irr::u32 CStartingGridSceneNode_Editor::getMaterialCount() {
      return 0;
    }

    void CStartingGridSceneNode_Editor::OnRegisterSceneNode() {
      if (IsVisible)
        SceneManager->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    irr::scene::ESCENE_NODE_TYPE CStartingGridSceneNode_Editor::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_StartingGridScenenodeId;
    }

    void CStartingGridSceneNode_Editor::render() {
      if (IsVisible) {

        irr::u16 l_aIndices[108];

        for (int i = 0; i < 18; i++) {
          l_aIndices[     (3 * i)    ] = 0;
          l_aIndices[     (3 * i) + 1] = i != 17 ? i + 2 : 1;
          l_aIndices[     (3 * i) + 2] = i + 1;
          l_aIndices[54 + (3 * i)    ] = 19;
          l_aIndices[54 + (3 * i) + 1] = i + 1;
          l_aIndices[54 + (3 * i) + 2] = i != 17 ? i + 2 : 1;
        }


        irr::video::IVideoDriver* l_pDrv = SceneManager->getVideoDriver();

        l_pDrv->setMaterial(m_cMaterial);
        l_pDrv->setTransform(irr::video::ETS_WORLD, getAbsoluteTransformation());
        l_pDrv->drawVertexPrimitiveList(m_aVertices, 20, &l_aIndices[0], 36, irr::video::EVT_STANDARD, irr::scene::EPT_TRIANGLES, irr::video::EIT_16BIT);
      }
    }

    const irr::core::aabbox3d<irr::f32>& CStartingGridSceneNode_Editor::getBoundingBox() const {
      return m_cBox;
    }

    void CStartingGridSceneNode_Editor::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addInt  ("MarblesPerRow", m_iMarblesPerRow);
      a_pOut->addFloat("Width"        , m_fWidth);
      a_pOut->addFloat("RowLength"    , m_fRowLength);
      a_pOut->addFloat("Angle"        , m_fAngle);
      a_pOut->addFloat("Offset"       , m_fOffset);
    }

    void CStartingGridSceneNode_Editor::updateGridPositions() {
      m_cBox.reset(getPosition());

      for (int i = 0; i < 20; i++)
        m_cBox.addInternalPoint(m_aVertices[i].Pos);

      irr::f32 l_fOffset = m_fOffset / m_iMarblesPerRow;

      for (int i = 0; i < 16; i++) {
        int l_iRow = i / m_iMarblesPerRow,
            l_iCol = i % m_iMarblesPerRow;


        irr::f32 l_fX = m_iMarblesPerRow == 1 ? 0.0f : (l_iCol * m_fWidth / (m_iMarblesPerRow - 1.0f)) - (m_fWidth / 2.0f),
                 l_fY = (l_iRow * m_fRowLength) + (l_iCol * l_fOffset * m_fRowLength);

        irr::core::vector2df v = irr::core::vector2df(l_fX, l_fY);

        v.rotateBy(m_fAngle);

        m_pMarbles[i]->setPosition(irr::core::vector3df(v.X, 2.5f, v.Y));
        m_cBox.addInternalPoint(irr::core::vector3df(v.X, 2.5f, v.Y));
      }
    }

    void CStartingGridSceneNode_Editor::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      // No scaling is possible
      setScale(irr::core::vector3df(1.0f));

      m_iMarblesPerRow = a_pIn->getAttributeAsInt  ("MarblesPerRow");
      m_fWidth         = a_pIn->getAttributeAsFloat("Width");
      m_fRowLength     = a_pIn->getAttributeAsFloat("RowLength");
      m_fAngle         = a_pIn->getAttributeAsFloat("Angle");
      m_fOffset        = a_pIn->getAttributeAsFloat("Offset");

      if (m_fOffset < 0.0f)
        m_fOffset = 0.0f;
      else if (m_fOffset > 0.75f)
        m_fOffset = 0.75f;

      if (m_iMarblesPerRow < 1)
        m_iMarblesPerRow = 1;

      if (m_iMarblesPerRow > 16)
        m_iMarblesPerRow = 16;

      updateGridPositions();
    }

    irr::scene::ISceneNode* CStartingGridSceneNode_Editor::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewManager) {
      if (a_pNewParent  == nullptr) a_pNewParent  = Parent;
      if (a_pNewManager == nullptr) a_pNewManager = SceneManager;

      CStartingGridSceneNode_Editor* l_pRet = new CStartingGridSceneNode_Editor(a_pNewParent, a_pNewManager, ID);

      l_pRet->m_iMarblesPerRow = m_iMarblesPerRow;
      l_pRet->m_fWidth         = m_fWidth;
      l_pRet->m_fRowLength     = m_fRowLength;
      l_pRet->m_fOffset        = m_fOffset;
      l_pRet->m_fAngle         = m_fAngle;

      return l_pRet;
    }
  }
}