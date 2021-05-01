// (w) 2021 by Dustbin::Games / Christian Keimel
#include <scenenodes/CGui3dSceneNodes.h>
#include <algorithm>
#include <vector>
#include <map>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef _IRREDIT_PLUGIN
#include <Windows.h>
#endif

namespace dustbin {
  namespace scenenodes {

    const irr::c8* const g_aUiElementNames[] = {
      "None",
      "Decoration",
      "Label",
      "Button",
      "IconButton",
      0
    };

    const irr::c8* const g_aTextAlignment[] = {
      "Left",
      "Center",
      "Right",
      0
    };

    static std::map<std::string, CGui3dItemBase::enGui3dType> g_mTypeMap = {
      { g_aUiElementNames[0], CGui3dItemBase::enGui3dType::None },
      { g_aUiElementNames[1], CGui3dItemBase::enGui3dType::Decoration },
      { g_aUiElementNames[2], CGui3dItemBase::enGui3dType::Label },
      { g_aUiElementNames[3], CGui3dItemBase::enGui3dType::Button },
      { g_aUiElementNames[4], CGui3dItemBase::enGui3dType::IconButton }
    };

    CGui3dItemBase::CGui3dItemBase(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent != nullptr ? a_pParent : a_pSmgr->getRootSceneNode(), a_pSmgr, a_iId), 
      m_pSmgr      (a_pSmgr), 
      m_eType      (enGui3dType ::Decoration),
      m_eAlign     (enGui3dAlign::Left      ),
      m_pRttTexture(nullptr),
      m_pFont      (nullptr),
      m_cBackground(irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF)),
      m_cHoverColor(irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF)),
      m_cTextColor (irr::video::SColor(0xFF, 0, 0, 0))
    {
      if (getID() == -1)
        setID(getNextSceneNodeId());

      m_cRttSize = irr::core::dimension2di(512, 32);

      m_sText = L"3d UI Element";
      m_iRttTexture = -1;

      m_pFont = m_pSmgr->getGUIEnvironment()->getFont("data/fonts/big.xml");

      m_mSerializerMap = {
        { enGui3dType::Label, {
            g_sTextureName,
            g_sTextName,
            g_sTextureWidthName,
            g_sTextureHeightName,
            g_sAlignmentName,
            g_sTextColorName,
            g_sBackgroundName
          }
        },
        {
          enGui3dType::Button, {
            g_sTextureName,
            g_sTextName,
            g_sTextureWidthName,
            g_sTextureHeightName,
            g_sAlignmentName,
            g_sTextColorName,
            g_sBackgroundName,
            g_sHoverColorName
          }
        },
        {
          enGui3dType::IconButton, {
            g_sTextureName,
            g_sBackgroundName,
            g_sHoverColorName
          }
        }
      };
    }

    CGui3dItemBase::~CGui3dItemBase() {
    }

    std::string CGui3dItemBase::typeToString(enGui3dType a_eType) {
      return g_aUiElementNames[(int)a_eType];
    }

    std::string CGui3dItemBase::alignToString(enGui3dAlign a_eAlign) {
      return g_aUiElementNames[(int)a_eAlign];
    }

    CGui3dItemBase::enGui3dAlign CGui3dItemBase::stringToAlign(const std::string& a_sAlign) {
      for (int i = 0; g_aTextAlignment[i] != 0; i++) {
        if (a_sAlign == g_aTextAlignment[i])
          return (enGui3dAlign)i;
      }
      return (enGui3dAlign)0;
    }

    void CGui3dItemBase::OnRegisterSceneNode() {

    }

    void CGui3dItemBase::render() {

    }

    const irr::core::aabbox3d<irr::f32>& CGui3dItemBase::getBoundingBox() const {
      return m_cBox;
    }

    irr::u32 CGui3dItemBase::getMaterialCount() {
      return 0;
    }

    irr::video::SMaterial& CGui3dItemBase::getMaterial(irr::u32 i) {
      return m_cMaterial;
    }

    irr::scene::ESCENE_NODE_TYPE CGui3dItemBase::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiItemID;
    }

    irr::scene::ISceneNode* CGui3dItemBase::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewSmgr) {
      CGui3dItemBase *l_pRet = new CGui3dItemBase(a_pNewParent != nullptr ? a_pNewParent : getParent(), a_pNewSmgr != nullptr ? a_pNewSmgr : m_pSmgr, -1);

      l_pRet->m_cBackground = m_cBackground;
      l_pRet->m_cHoverColor = m_cHoverColor;
      l_pRet->m_cRttSize    = m_cRttSize;
      l_pRet->m_cTextColor  = m_cTextColor;
      l_pRet->m_eAlign      = m_eAlign;
      l_pRet->m_eType       = m_eType;
      l_pRet->m_iRttTexture = m_iRttTexture;
      l_pRet->m_sText       = m_sText;

      return l_pRet;
    }

    void CGui3dItemBase::updateRttText(const irr::video::SColor &a_cBackgroundColor, const irr::video::SColor &a_cTextColor) {
      if (m_pRttTexture == nullptr)
        updateRttTexture();

      irr::video::IVideoDriver *l_pDrv = m_pSmgr->getVideoDriver();
      l_pDrv->setRenderTarget(m_pRttTexture);
      l_pDrv->draw2DRectangle(a_cBackgroundColor, irr::core::rect<irr::s32>(irr::core::position2di(0,0), m_pRttTexture->getSize()));

      irr::core::rect<irr::s32> l_cRect = m_cRect;
      bool l_bCenterH = false;

      switch (m_eAlign) {
      case enGui3dAlign::Left:
        break;

      case enGui3dAlign::Center:
        l_bCenterH = true;
        break;

      case enGui3dAlign::Right: {
#ifdef _IRREDIT_PLUGIN
        irr::core::dimension2di l_cText = m_pFont->getDimension(m_sText.c_str());
        l_cRect.UpperLeftCorner.X = l_cRect.LowerRightCorner.X - l_cText.Width;
#else
        irr::core::dimension2du l_cText = m_pFont->getDimension(m_sText.c_str());
        l_cRect.UpperLeftCorner.X = l_cRect.LowerRightCorner.X - l_cText.Width;
#endif
        break;
      }
      }

      m_pFont->draw(m_sText.c_str(), l_cRect, a_cTextColor, l_bCenterH, true);

      l_pDrv->setRenderTarget(nullptr);
    }

    void CGui3dItemBase::updateRttTexture() {
      if (getParent() != nullptr && getParent()->getType() == irr::scene::ESNT_MESH) {
        std::string l_sTextureName = std::string("Rtt_") + getParent()->getName() + "_" + std::to_string(getParent()->getID()) + ".png";
#ifdef _IRREDIT_PLUGIN
        irr::core::dimension2d<irr::s32> l_cRttSize;
#else
        irr::core::dimension2du l_cRttSize;
#endif

        l_cRttSize.Width  = 5 * m_cRttSize.Width  / 4;
        l_cRttSize.Height = 5 * m_cRttSize.Height / 4;

        m_pRttTexture = m_pSmgr->getVideoDriver()->addRenderTargetTexture(l_cRttSize, l_sTextureName.c_str());

        switch (m_eType) {
        case enGui3dType::Label:
        case enGui3dType::Button: {
          irr::s32 l_iLeft = (l_cRttSize.Width  - m_cRttSize.Width ) / 2,
            l_iTop  = (l_cRttSize.Height - m_cRttSize.Height) / 2;

          m_cRect = irr::core::rect<irr::s32>(l_iLeft, l_iTop, l_iLeft + m_cRttSize.Width, l_iTop + m_cRttSize.Height);
          break;
        }

        default:
          // No element necessary
          break;
        }

        irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(getParent());

        if (l_pNode->getMaterialCount() >= (irr::u32)m_iRttTexture) {
          l_pNode->getMaterial(m_iRttTexture).setTexture(m_iRttTexture, m_pRttTexture);
        }

        if (m_pRttTexture != nullptr)
          updateRttText(m_cBackground, m_cTextColor);
      }
    }

    void CGui3dItemBase::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      irr::scene::ISceneNode::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addEnum("Type", (irr::s32)m_eType, g_aUiElementNames);

      std::map<enGui3dType, std::vector<std::string> >::const_iterator l_itMap = m_mSerializerMap.find(m_eType);

      if (l_itMap != m_mSerializerMap.end()) {
        for (std::vector<std::string>::const_iterator it = l_itMap->second.begin(); it != l_itMap->second.end(); it++) {
               if ((*it) == g_sTextureName      ) a_pOut->addInt       ((*it).c_str(), m_iRttTexture);
          else if ((*it) == g_sTextName         ) a_pOut->addString    ((*it).c_str(), m_sText.c_str());
          else if ((*it) == g_sAlignmentName    ) a_pOut->addEnum      ((*it).c_str(), (int)m_eAlign, g_aTextAlignment);
          else if ((*it) == g_sTextColorName    ) a_pOut->addColor     ((*it).c_str(), m_cTextColor);
          else if ((*it) == g_sBackgroundName   ) a_pOut->addColor     ((*it).c_str(), m_cBackground);
          else if ((*it) == g_sHoverColorName   ) a_pOut->addColor     ((*it).c_str(), m_cHoverColor);
          else if ((*it) == g_sTextureWidthName ) a_pOut->addInt       ((*it).c_str(), m_cRttSize.Width);
          else if ((*it) == g_sTextureHeightName) a_pOut->addInt       ((*it).c_str(), m_cRttSize.Height);
        }
      }
    }

    void CGui3dItemBase::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      irr::scene::ISceneNode::deserializeAttributes(a_pIn, a_pOptions);

      enGui3dType l_eType = enGui3dType::None;
      
      bool l_bUpdateRttText    = false,
           l_bUpdateRttTexture = false;

      if (a_pIn->existsAttribute("Type")) {
        std::string l_sType = a_pIn->getAttributeAsEnumeration("Type");

        if (g_mTypeMap.find(l_sType) != g_mTypeMap.end())
          l_eType = g_mTypeMap[l_sType];

        if (m_eType != l_eType) 
          l_bUpdateRttText = true;
      }

      m_eType = l_eType;

      if (a_pIn->existsAttribute(g_sTextureName.c_str())) {
        int l_iTexture = a_pIn->getAttributeAsInt(g_sTextureName.c_str());
        if (l_iTexture != m_iRttTexture) {
          l_bUpdateRttTexture = true;
          m_iRttTexture = l_iTexture;
        }
      }

      if (a_pIn->existsAttribute(g_sTextName.c_str())) {
        std::wstring l_sText = a_pIn->getAttributeAsStringW(g_sTextName.c_str()).c_str();
        if (l_sText != m_sText)
          l_bUpdateRttText = true;
        m_sText = l_sText;
      }

      if (a_pIn->existsAttribute(g_sTextureWidthName.c_str())) {
        irr::s32 l_iWidth = a_pIn->getAttributeAsInt(g_sTextureWidthName.c_str());
        if (l_iWidth != m_cRttSize.Width) {
          l_bUpdateRttTexture = true;
          m_cRttSize.Width = l_iWidth;
        }
      }

      if (a_pIn->existsAttribute(g_sTextureHeightName.c_str())) {
        irr::s32 l_iHeight = a_pIn->getAttributeAsInt(g_sTextureHeightName.c_str());
        if (l_iHeight != m_cRttSize.Height) {
          l_bUpdateRttTexture = true;
          m_cRttSize.Height = l_iHeight;
        }
      }

      if (a_pIn->existsAttribute(g_sAlignmentName.c_str())) {
        std::string l_sAlign = a_pIn->getAttributeAsEnumeration(g_sAlignmentName.c_str());
        enGui3dAlign l_eAlign = stringToAlign(l_sAlign);

        if (l_eAlign != m_eAlign) {
          m_eAlign = l_eAlign;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sTextColorName.c_str())) {
        irr::video::SColor l_cColor = a_pIn->getAttributeAsColor(g_sTextColorName.c_str());
        if (l_cColor != m_cTextColor) {
          m_cTextColor = l_cColor;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sBackgroundName.c_str())) {
        irr::video::SColor l_cColor = a_pIn->getAttributeAsColor(g_sBackgroundName.c_str());
        if (l_cColor != m_cBackground) {
          m_cBackground = l_cColor;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sHoverColorName.c_str())) {
        irr::video::SColor l_cColor = a_pIn->getAttributeAsColor(g_sHoverColorName.c_str());
        if (l_cColor != m_cHoverColor) {
          m_cHoverColor = l_cColor;
          l_bUpdateRttText = true;
        }
      }

      // If the texture is updated the text will also be updated
      if (l_bUpdateRttTexture) {
        updateRttTexture();
      }
      else if (l_bUpdateRttText) {
        updateRttText(m_cBackground, m_cTextColor);
      }
    }

    const std::string& CGui3dItemBase::getNodeTypeName() {
      return m_sNodeTypeName;
    }

    const irr::scene::ESCENE_NODE_TYPE CGui3dItemBase::getNodeType() {
      return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiItemID;
    }

    CGui3dRootBase::CGui3dRootBase(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent != nullptr ? a_pParent : a_pSmgr->getRootSceneNode(), a_pSmgr, a_iId), 
      m_pSmgr    (a_pSmgr), 
      m_pColMgr  (nullptr)
    {
      if (a_iId == -1)
        setID(getNextSceneNodeId());

      m_cMaterial.MaterialType    = irr::video::EMT_SOLID;
      m_cMaterial.Wireframe       = false;
      m_cMaterial.AmbientColor    = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.DiffuseColor    = irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF);
      m_cMaterial.BackfaceCulling = true;
      m_cMaterial.Lighting        = false;
      m_cMaterial.Thickness       = 2.0f;

      setPosition(irr::core::vector3df(0.0f, 0.0f, 0.0f));
    }

    CGui3dRootBase::~CGui3dRootBase() {
    }

    void CGui3dRootBase::OnRegisterSceneNode() {
      if (IsVisible)
        m_pSmgr->registerNodeForRendering(this);

      ISceneNode::OnRegisterSceneNode();
    }

    void CGui3dRootBase::render() {
    }

    const irr::core::aabbox3d<irr::f32>& CGui3dRootBase::getBoundingBox() const {
      return m_cBox;
    }

    irr::u32 CGui3dRootBase::getMaterialCount() {
      return 0;
    }

    irr::video::SMaterial& CGui3dRootBase::getMaterial(irr::u32 i) {
      return m_cMaterial;
    }

    irr::scene::ESCENE_NODE_TYPE CGui3dRootBase::getType() const {
      return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiRootID;
    }

    irr::scene::ISceneNode* CGui3dRootBase::clone(irr::scene::ISceneNode* a_pNewParent, irr::scene::ISceneManager* a_pNewSmgr) {
      return nullptr;
    }

    void CGui3dRootBase::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      ISceneNode::serializeAttributes(a_pOut, a_pOptions);
    }

    void CGui3dRootBase::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      ISceneNode::deserializeAttributes(a_pIn, a_pOptions);
    }

    const std::string &CGui3dRootBase::getNodeTypeName() {
      return m_sNodeTypeName;
    }

    const irr::scene::ESCENE_NODE_TYPE CGui3dRootBase::getNodeType() {
      return (irr::scene::ESCENE_NODE_TYPE)g_i3dGuiRootID;
    }

    std::string CGui3dRootBase::m_sNodeTypeName = "Gui3dRoot";
    std::string CGui3dItemBase::m_sNodeTypeName = "Gui3dItem";

    std::string CGui3dItemBase::g_sTextureName       = "Texture";
    std::string CGui3dItemBase::g_sTextName          = "Text";
    std::string CGui3dItemBase::g_sAlignmentName     = "Alignment";
    std::string CGui3dItemBase::g_sTextColorName     = "Textcolor";
    std::string CGui3dItemBase::g_sBackgroundName    = "Backgroundcolor";
    std::string CGui3dItemBase::g_sHoverColorName    = "HoverColor";
    std::string CGui3dItemBase::g_sTextureWidthName  = "TextureWidth";
    std::string CGui3dItemBase::g_sTextureHeightName = "TextureHeight";
  }
}
