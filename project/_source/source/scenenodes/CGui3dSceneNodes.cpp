// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CGui3dSceneNodes.h>
#include <vector>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

namespace dustbin {
  namespace scenenodes {

    const irr::c8* const g_UiElementNames[] = {
      "Decoration",
      "Label",
      "Button",
      0
    };

    CGui3dRoot::CGui3dRoot(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent != nullptr ? a_pParent : a_pSmgr->getRootSceneNode(), a_pSmgr, a_iId),
      m_pSmgr   (a_pSmgr), 
      m_pFont   (nullptr), 
      m_pRtt    (nullptr), 
      m_pGui    (nullptr),
      m_pDrv    (nullptr),
      m_cDimension(5.0f, 1.25f, 0.5f) {

      if (a_iId == -1)
        setID(getNextSceneNodeId());

      m_pGui     = m_pSmgr->getGUIEnvironment();
      m_pFont    = m_pGui->getFont("data/fonts/big.xml");
      m_cRttDim  = irr::core::dimension2di(128, 20);
      m_pRtt     = m_pSmgr->getVideoDriver()->addRenderTargetTexture(m_cRttDim, std::to_string(getID()).c_str());
      m_pDrv     = m_pSmgr->getVideoDriver();

      m_cMaterial[0].MaterialType    = irr::video::EMT_SOLID;
      m_cMaterial[0].Wireframe       = false;
      m_cMaterial[0].AmbientColor    = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial[0].DiffuseColor    = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial[0].BackfaceCulling = true;
      m_cMaterial[0].Lighting        = false;
      m_cMaterial[0].Thickness       = 2.0f;

      // renderRTT();

      setPosition(irr::core::vector3df(0.0f, 0.0f, 0.0f));
    }

    CGui3dRoot::~CGui3dRoot() {

    }

    void CGui3dRoot::OnRegisterSceneNode() {
      if (IsVisible)
        m_pSmgr->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CGui3dRoot::render() {
      irr::core::matrix4 l_cMat;

      l_cMat.setTranslation(getPosition());
      l_cMat.setRotationDegrees(getRotation());
      l_cMat.setScale(getScale());

      m_pDrv->setTransform(irr::video::ETS_WORLD, l_cMat);
      if (m_pBuffers[0] != nullptr) {
        m_pDrv->setMaterial(m_cMaterial[0]);
        m_pDrv->drawMeshBuffer(m_pBuffers[0]);
      }
    }

    const irr::core::aabbox3d<irr::f32>& CGui3dRoot::getBoundingBox() const {
      return m_cBox;
    }

    irr::u32 CGui3dRoot::getMaterialCount() {
      return 0;
    }

    irr::video::SMaterial& CGui3dRoot::getMaterial(irr::u32 i) {
      return m_cMaterial[0];
    }

    irr::scene::ESCENE_NODE_TYPE CGui3dRoot::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_s3dGuiButtonID;
    }

    irr::scene::ISceneNode* CGui3dRoot::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewSmgr) {
      return nullptr;
    }

    int CGui3dRoot::getNextId() {
      int l_iRet = 1;

      for (std::map<int, CGui3dElement*>::const_iterator it = m_mChildElements.begin(); it != m_mChildElements.end(); it++) {
        if (it->first >= l_iRet)
          l_iRet = it->first + 1;
      }

      return l_iRet;
    }

    CGui3dElement* CGui3dRoot::getElement(irr::scene::ISceneNode* a_pNode) const {
      for (std::map<int, CGui3dElement*>::const_iterator it = m_mChildElements.begin(); it != m_mChildElements.end(); it++) {
        if (it->second->getNode() == a_pNode)
          return it->second;
      }
      return nullptr;
    }

    void CGui3dRoot::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);
      
      for (std::map<int, CGui3dElement*>::const_iterator it = m_mChildElements.begin(); it != m_mChildElements.end(); it++) {
        if (it->second->getNode() != nullptr) {
          std::string l_sSuffix = "_" + std::to_string(it->first);
          a_pOut->addString(std::string("name" + l_sSuffix).c_str(), it->second->getNode()->getName());

          a_pOut->addEnum(std::string("type" + l_sSuffix).c_str(), it->second->getTypeName().c_str(), g_UiElementNames);
          it->second->serialize(a_pOut, l_sSuffix);
        }
      }
    }

    void CGui3dRoot::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      for (std::map<int, CGui3dElement*>::iterator it = m_mChildElements.begin(); it != m_mChildElements.end(); it++) {
        std::string l_sSuffix = "_" + std::to_string(it->first);
        it->second->deserialize(a_pIn, l_sSuffix);
      }
    }

    const std::string &CGui3dRoot::getNodeTypeName() {
      return m_sNodeTypeName;
    }

    const irr::scene::ESCENE_NODE_TYPE CGui3dRoot::getNodeType() {
      return (irr::scene::ESCENE_NODE_TYPE)g_s3dGuiButtonID;
    }

    void CGui3dRoot::renderRTT() {
    }

    void CGui3dRoot::addChild(ISceneNode* a_pChild) {
      ISceneNode::addChild(a_pChild);
      m_mChildElements[getNextId()] = new CGui3dDecoration(a_pChild);
    }

    std::string CGui3dRoot::m_sNodeTypeName = "Gui3dRoot";

    CGui3dElement::CGui3dElement(enType a_eType, irr::scene::ISceneNode *a_pNode) : m_pNode(a_pNode), m_eType(a_eType) {
    }

    CGui3dElement::enType CGui3dElement::getType() {
      return m_eType;
    }

    irr::scene::ISceneNode* CGui3dElement::getNode() {
      return m_pNode;
    }

    CGui3dDecoration::CGui3dDecoration(irr::scene::ISceneNode* a_pNode) : CGui3dElement(enType::enDecoration, a_pNode) {
    }

    void CGui3dDecoration::serialize(irr::io::IAttributes* a_pOut, const std::string& a_sSuffix) const {
    }

    void CGui3dDecoration::deserialize(irr::io::IAttributes* a_pIn, const std::string& a_sSuffix) {
    }

    std::string CGui3dDecoration::getTypeName() {
      return "Decoration";
    }
  }
}
