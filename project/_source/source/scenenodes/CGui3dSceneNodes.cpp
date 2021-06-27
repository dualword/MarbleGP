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

    /**
     * The text values of the enGui3dType enumeration
     * @see enGui3dType
     */
    const irr::c8* const g_aUiElementNames[] = {
      "None",
      "Decoration",
      "Label",
      "Button",
      "IconButton",
      "Image",
      "Slider",
      "TextInput",
      0
    };

    /**
     * The text values of the enGui3dAlign enumeration
     * @see enGui3dAlign
     */
    const irr::c8* const g_aTextAlignment[] = {
      "Left",
      "Center",
      "Right",
      0
    };

    /**
     * The text values for the enGui3dVerticalAlign enumeration
     * @see enGui3dVerticalAlign
     */
    const irr::c8* const g_aVerticalAlignment[] = {
      "Top",
      "Middle",
      "Bottom",
      0
    };

    /**
    * The text values for axis (e.g. slider direction and normal
    */
    const irr::c8* const g_aAxisNames[] = {
      "X",
      "Y",
      "Z",
      0
    };

    static std::map<std::string, CGui3dItemBase::enGui3dType> g_mTypeMap = {
      { g_aUiElementNames[0], CGui3dItemBase::enGui3dType::None },
      { g_aUiElementNames[1], CGui3dItemBase::enGui3dType::Decoration },
      { g_aUiElementNames[2], CGui3dItemBase::enGui3dType::Label },
      { g_aUiElementNames[3], CGui3dItemBase::enGui3dType::Button },
      { g_aUiElementNames[4], CGui3dItemBase::enGui3dType::IconButton },
      { g_aUiElementNames[5], CGui3dItemBase::enGui3dType::Image },
      { g_aUiElementNames[6], CGui3dItemBase::enGui3dType::Slider },
      { g_aUiElementNames[7], CGui3dItemBase::enGui3dType::TextInput }
    };

    CGui3dItemBase::CGui3dItemBase(irr::scene::ISceneNode* a_pParent, irr::scene::ISceneManager* a_pSmgr, irr::s32 a_iId) : irr::scene::ISceneNode(a_pParent != nullptr ? a_pParent : a_pSmgr->getRootSceneNode(), a_pSmgr, a_iId), 
      m_pSmgr         (a_pSmgr), 
      m_eType         (enGui3dType ::Decoration),
      m_eAlign        (enGui3dAlign::Left      ),
      m_eDirection    (enGui3dAxis ::AxisX     ),
      m_eNormal       (enGui3dAxis ::AxisZ     ),
      m_pRttTexture   (nullptr),
      m_pFont         (nullptr),
      m_cBackground   (irr::video::SColor(0xFF, 255, 255, 255)),
      m_cHoverColor   (irr::video::SColor(0xFF, 255, 128, 128)),
      m_cClickColor   (irr::video::SColor(0xFF, 255,   0,   0)),
      m_cTextColor    (irr::video::SColor(0xFF, 0, 0, 0)),
      m_cTextBackColor(irr::video::SColor(0xFF, 192, 192, 192)),
      m_bMultiLine    (false),
      m_eVerticalAlign(enGui3dVerticalAlign::Middle),
      m_sImage        (""),
      m_bShowText     (false),
      m_iTextBackAlpha(128),
      m_fMinValue     (0.0f),
      m_fMaxValue     (100.0f),
      m_fValue        (50.0f)
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
            g_sBackgroundName,
            g_sMultilineName,
            g_sVertAlignName
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
            g_sHoverColorName,
            g_sClickColorName,
            g_sMultilineName,
            g_sVertAlignName
          }
        },
        {
          enGui3dType::IconButton, {
            g_sTextureName,
            g_sBackgroundName,
            g_sHoverColorName,
            g_sClickColorName
          }
        },
        {
          enGui3dType::Image, {
            g_sTextureName,
            g_sTextName,
            g_sTextureWidthName,
            g_sTextureHeightName,
            g_sAlignmentName,
            g_sTextColorName,
            g_sBackgroundName,
            g_sMultilineName,
            g_sVertAlignName,
            g_sImageFileName,
            g_sOverlayTextName,
            g_sTextBackgroundName,
            g_sTextBackAlphaName
          }
        },
        {
          enGui3dType::Slider, {
            g_sBackgroundName,
            g_sTextureName,
            g_sHoverColorName,
            g_sClickColorName,
            g_sValueName,
            g_sMinValueName,
            g_sMaxValueName,
            g_sSliderDirectionName,
            g_sSliderNormalName,
            g_sSliderMinOffsetName,
            g_sSliderMaxOffsetName
          }
        },
        {
          enGui3dType::TextInput, {
            g_sTextureName,
            g_sTextName,
            g_sTextureWidthName,
            g_sTextureHeightName,
            g_sAlignmentName,
            g_sTextColorName,
            g_sBackgroundName
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

    CGui3dItemBase::enGui3dVerticalAlign CGui3dItemBase::stringToVerticalAlign(const std::string& a_sAlign) {
      for (int i = 0; g_aVerticalAlignment[i] != 0; i++) {
        if (a_sAlign == g_aVerticalAlignment[i])
          return (enGui3dVerticalAlign)i;
      }
      return (enGui3dVerticalAlign)0;
    }

    CGui3dItemBase::enGui3dAxis CGui3dItemBase::stringToAxis(const std::string &a_sAxis) {
      for (int i = 0; g_aAxisNames[i] != 0; i++) {
        if (a_sAxis == g_aAxisNames[i])
          return (enGui3dAxis)i;
      }
      return (enGui3dAxis)0;
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
      l_pRet->m_cClickColor = m_cClickColor;
      l_pRet->m_eDirection  = m_eDirection;
      l_pRet->m_eNormal     = m_eNormal;
      l_pRet->m_fMinOffset  = m_fMinOffset;
      l_pRet->m_fMaxOffset  = m_fMaxOffset;
      l_pRet->m_fMinValue   = m_fMinValue;
      l_pRet->m_fMaxValue   = m_fMaxValue;
      l_pRet->m_fValue      = m_fValue;

      return l_pRet;
    }

    void CGui3dItemBase::updateRttText(const irr::video::SColor &a_cBackgroundColor, const irr::video::SColor &a_cTextColor) {
      if (m_pRttTexture == nullptr)
        updateRttTexture();

      if (m_pRttTexture == nullptr)
        return;

      irr::video::IVideoDriver *l_pDrv = m_pSmgr->getVideoDriver();
      l_pDrv->setRenderTarget(m_pRttTexture);
      l_pDrv->draw2DRectangle(a_cBackgroundColor, irr::core::rect<irr::s32>(irr::core::position2di(0,0), m_pRttTexture->getSize()));

      if (m_eType == enGui3dType::Image) {
        if (m_pSmgr->getGUIEnvironment()->getFileSystem()->existFile(m_sImage.c_str())) {
          irr::video::ITexture *l_pTexture = l_pDrv->getTexture(m_sImage.c_str());
          if (l_pTexture != nullptr) {
            l_pDrv->draw2DImage(l_pTexture, irr::core::rect<irr::s32>(irr::core::position2di(0, 0), m_cRttSize), irr::core::rect<irr::s32>(irr::core::position2di(0, 0), l_pTexture->getSize()));
          }
        }
        
        if (m_iTextBackAlpha > 0 && m_bShowText) {
          l_pDrv->draw2DRectangle(irr::video::SColor(m_iTextBackAlpha, m_cTextBackColor.getRed(), m_cTextBackColor.getGreen(), m_cTextBackColor.getBlue()), irr::core::rect<irr::s32>(irr::core::position2di(0, 0), m_pRttTexture->getSize()));
        }
      }

      // Only render text for the items that display text. For the image it's optional
      if (m_eType == enGui3dType::Button || m_eType == enGui3dType::Label || (m_eType == enGui3dType::Image && m_bShowText) || m_eType == enGui3dType::TextInput) {
        irr::core::rect<irr::s32> l_cRect = m_cRect;
        
        std::wstring l_sLine = L"";
        irr::core::position2di l_cPos = irr::core::position2di(0, 0);
#ifdef _IRREDIT_PLUGIN
        irr::core::dimension2di l_cTotalDim = m_pFont->getDimension(m_sText.c_str()),
                                l_cSpaceDim = m_pFont->getDimension(L" ");
#else
        irr::core::dimension2du l_cTotalDim = m_pFont->getDimension(m_sText.c_str()),
                                l_cSpaceDim = m_pFont->getDimension(L" ");
#endif

        std::vector<std::wstring> l_vWords;

        size_t l_cSpace = m_sText.find(L" "),
               l_cInit  = 0;

        // First off we split the text into words
        while (l_cSpace != std::wstring::npos) {
          l_vWords.push_back(m_sText.substr(l_cInit, l_cSpace - l_cInit));
          l_cInit  = l_cSpace + 1;
          l_cSpace = m_sText.find(L" ", l_cInit);
        }

        l_vWords.push_back(m_sText.substr(l_cInit));

        /**
         * @struct SWordRect
         * @author Christian Keimel
         * A little helper data structure for the layout of text
         */
        struct SWordRect {
          std::wstring m_sText;
          irr::core::rect<irr::s32> m_cRect;
          irr::gui::IGUIFont *m_pFont;
#ifdef _IRREDIT_PLUGIN
          irr::core::dimension2di m_cDim;
#else
          irr::core::dimension2du m_cDim;
#endif
          SWordRect(const std::wstring& a_sText, irr::gui::IGUIFont* a_pFont, irr::core::position2di a_cPos) {
            m_sText = a_sText;
            m_cDim  = a_pFont->getDimension(m_sText.c_str());
            m_cRect = irr::core::rect<irr::s32>(a_cPos, m_cDim);
            m_pFont = a_pFont;
          }

          void draw(const irr::video::SColor &a_cColor, const irr::core::rect<irr::s32> &a_cClip) {
            m_pFont->draw(m_sText.c_str(), m_cRect, a_cColor, true, true, &a_cClip);
          }
        };

        std::vector<SWordRect> l_vLayout;
        l_cPos = m_cRect.UpperLeftCorner;

        // Then we distribute the words in the texture. If the text is too long we cut it and end the text with "..."
        for (std::vector<std::wstring>::iterator it = l_vWords.begin(); it != l_vWords.end(); it++) {
#ifdef _IRREDIT_PLUGIN
          irr::core::dimension2di l_cDim = m_pFont->getDimension((*it).c_str());
#else
          irr::core::dimension2du l_cDim = m_pFont->getDimension((*it).c_str());
#endif
          if ((irr::s32)(l_cPos.X + l_cDim.Width) > (irr::s32)m_cRect.LowerRightCorner.X) {
            irr::core::position2di l_cOld = l_cPos;

            l_cPos.X = m_cRect.UpperLeftCorner.X;
            l_cPos.Y += l_cDim.Height;

            if (l_cPos.Y > m_cRect.LowerRightCorner.Y) {
#ifdef _IRREDIT_PLUGIN
              irr::core::dimension2di l_cDots = m_pFont->getDimension(L"...");
#else
              irr::core::dimension2du l_cDots = m_pFont->getDimension(L"...");
#endif
              l_vLayout.push_back(SWordRect(L"...", m_pFont, l_cOld));
              break;
            }
          }

          l_vLayout.push_back(SWordRect((*it), m_pFont, l_cPos));
          l_cPos.X += l_cDim.Width + l_cSpaceDim.Width;
        }

        if (l_vLayout.size() > 0) {
          std::vector<std::vector<SWordRect> > l_vLines;
          irr::s32 l_iPosY = (*l_vLayout.begin()).m_cRect.UpperLeftCorner.Y;

          std::vector<SWordRect> l_cLine;

          irr::core::rect<irr::s32> l_cTotalRect = irr::core::rect<irr::s32>((*l_vLayout.begin()).m_cRect);

          for (std::vector<SWordRect>::iterator it = l_vLayout.begin(); it != l_vLayout.end(); it++) {
            l_cTotalRect.addInternalPoint((*it).m_cRect.UpperLeftCorner );
            l_cTotalRect.addInternalPoint((*it).m_cRect.LowerRightCorner);

            if ((*it).m_cRect.UpperLeftCorner.Y != l_iPosY) {
              l_vLines.push_back(l_cLine);
              l_cLine = std::vector<SWordRect>();
              l_iPosY = (*it).m_cRect.UpperLeftCorner.Y;
            }
            l_cLine.push_back(*it);
          }

          if (l_cLine.size() > 0)
            l_vLines.push_back(l_cLine);

          int l_iOffsetY = 0;

          if (m_eVerticalAlign == enGui3dVerticalAlign::Middle) {
            l_iOffsetY = m_cRect.getCenter().Y - (l_cTotalRect.getHeight() / 2);
          }
          else if (m_eVerticalAlign == enGui3dVerticalAlign::Bottom) {
            l_iOffsetY = m_cRect.getHeight() - (l_cTotalRect.getHeight());
          }

          for (std::vector<std::vector<SWordRect> >::iterator it = l_vLines.begin(); it != l_vLines.end(); it++) {
            if ((*it).size() > 0) {
              irr::core::rect<irr::s32> l_cTotal = irr::core::rect<irr::s32>((*(*it).begin()).m_cRect.UpperLeftCorner, (*(*it).begin()).m_cRect.LowerRightCorner);

              for (std::vector<SWordRect>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
                l_cTotal.addInternalPoint((*it2).m_cRect.UpperLeftCorner );
                l_cTotal.addInternalPoint((*it2).m_cRect.LowerRightCorner);
              }

              int l_iOffsetX = 0;

              if (m_eAlign == enGui3dAlign::Right) {
                l_iOffsetX = m_cRect.LowerRightCorner.X - l_cTotal.getWidth();
              }
              else if (m_eAlign == enGui3dAlign::Center) {
                l_iOffsetX = m_cRect.getCenter().X - (l_cTotal.getWidth() / 2);
              }

              for (std::vector<SWordRect>::iterator it2 = (*it).begin(); it2 != (*it).end(); it2++) {
                (*it2).m_cRect.UpperLeftCorner .X += l_iOffsetX;
                (*it2).m_cRect.LowerRightCorner.X += l_iOffsetX;
                (*it2).m_cRect.UpperLeftCorner .Y += l_iOffsetY;
                (*it2).m_cRect.LowerRightCorner.Y += l_iOffsetY;

                (*it2).draw(m_cTextColor, m_cRect);
              }
            }
          }
        }
      }

      l_pDrv->setRenderTarget(nullptr);
    }

    void CGui3dItemBase::updateRttTexture() {
      if (getParent() != nullptr && getParent()->getType() == irr::scene::ESNT_MESH) {
        std::string l_sTextureName = std::string("Rtt_") + getParent()->getName() + "_" + std::to_string(getID()) + ".png";

        if (m_eType == enGui3dType::IconButton) {
          // Use small texture for the icon buttons which do not render anything
          m_cRttSize.Width  = 16;
          m_cRttSize.Height = 16;
        }

        m_pRttTexture = m_pSmgr->getVideoDriver()->addRenderTargetTexture(m_cRttSize, l_sTextureName.c_str());

        switch (m_eType) {
          case enGui3dType::Label:
          case enGui3dType::Button:
          case enGui3dType::Image: 
          case enGui3dType::TextInput: {
            m_cRect = irr::core::rect<irr::s32>(0, 0, m_cRttSize.Width, m_cRttSize.Height);
            break;
          }

          case enGui3dType::IconButton:
            m_cRect = irr::core::rect<irr::s32>(0, 0, 16, 16);
            break;

          default:
            // No element necessary
            break;
        }

        irr::scene::IMeshSceneNode *l_pNode = reinterpret_cast<irr::scene::IMeshSceneNode *>(getParent());

        if (l_pNode->getMaterialCount() >= (irr::u32)m_iRttTexture) {
          l_pNode->getMaterial(m_iRttTexture).setTexture(0, m_pRttTexture);
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
               if ((*it) == g_sTextureName        ) a_pOut->addInt       ((*it).c_str(), m_iRttTexture);
          else if ((*it) == g_sTextName           ) a_pOut->addString    ((*it).c_str(), m_sText.c_str());
          else if ((*it) == g_sAlignmentName      ) a_pOut->addEnum      ((*it).c_str(), (int)m_eAlign        , g_aTextAlignment);
          else if ((*it) == g_sVertAlignName      ) a_pOut->addEnum      ((*it).c_str(), (int)m_eVerticalAlign, g_aVerticalAlignment);
          else if ((*it) == g_sTextColorName      ) a_pOut->addColor     ((*it).c_str(), m_cTextColor);
          else if ((*it) == g_sBackgroundName     ) a_pOut->addColor     ((*it).c_str(), m_cBackground);
          else if ((*it) == g_sHoverColorName     ) a_pOut->addColor     ((*it).c_str(), m_cHoverColor);
          else if ((*it) == g_sTextureWidthName   ) a_pOut->addInt       ((*it).c_str(), m_cRttSize.Width);
          else if ((*it) == g_sTextureHeightName  ) a_pOut->addInt       ((*it).c_str(), m_cRttSize.Height);
          else if ((*it) == g_sClickColorName     ) a_pOut->addColor     ((*it).c_str(), m_cClickColor);
          else if ((*it) == g_sMultilineName      ) a_pOut->addBool      ((*it).c_str(), m_bMultiLine);
          else if ((*it) == g_sImageFileName      ) a_pOut->addString    ((*it).c_str(), m_sImage.c_str());
          else if ((*it) == g_sOverlayTextName    ) a_pOut->addBool      ((*it).c_str(), m_bShowText);
          else if ((*it) == g_sTextBackgroundName ) a_pOut->addColor     ((*it).c_str(), m_cTextBackColor);
          else if ((*it) == g_sTextBackAlphaName  ) a_pOut->addInt       ((*it).c_str(), m_iTextBackAlpha);
          else if ((*it) == g_sMinValueName       ) a_pOut->addFloat     ((*it).c_str(), m_fMinValue);
          else if ((*it) == g_sMaxValueName       ) a_pOut->addFloat     ((*it).c_str(), m_fMaxValue);
          else if ((*it) == g_sValueName          ) a_pOut->addFloat     ((*it).c_str(), m_fValue);
          else if ((*it) == g_sSliderDirectionName) a_pOut->addEnum      ((*it).c_str(), (int)m_eDirection, g_aAxisNames);
          else if ((*it) == g_sSliderNormalName   ) a_pOut->addEnum      ((*it).c_str(), (int)m_eNormal   , g_aAxisNames);
          else if ((*it) == g_sSliderMinOffsetName) a_pOut->addFloat     ((*it).c_str(), m_fMinOffset);
          else if ((*it) == g_sSliderMaxOffsetName) a_pOut->addFloat     ((*it).c_str(), m_fMaxOffset);
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

      if (a_pIn->existsAttribute(g_sSliderDirectionName.c_str())) {
        m_eDirection = stringToAxis(a_pIn->getAttributeAsEnumeration(g_sSliderDirectionName.c_str()));
      }

      if (a_pIn->existsAttribute(g_sSliderNormalName.c_str())) {
        m_eNormal = stringToAxis(a_pIn->getAttributeAsEnumeration(g_sSliderNormalName.c_str()));
      }

      if (a_pIn->existsAttribute(g_sVertAlignName.c_str())) {
        std::string l_sAlign = a_pIn->getAttributeAsEnumeration(g_sVertAlignName.c_str());
        enGui3dVerticalAlign l_eAlign = stringToVerticalAlign(l_sAlign);

        if (l_eAlign != m_eVerticalAlign) {
          m_eVerticalAlign = l_eAlign;
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

      if (a_pIn->existsAttribute(g_sMultilineName.c_str())) {
        bool l_bMultiline = a_pIn->getAttributeAsBool(g_sMultilineName.c_str());
        if (l_bMultiline != m_bMultiLine) {
          m_bMultiLine = l_bMultiline;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sImageFileName.c_str())) {
        std::string l_sImage = a_pIn->getAttributeAsString(g_sImageFileName.c_str()).c_str();
        if (l_sImage != m_sImage) {
          m_sImage = l_sImage;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sOverlayTextName.c_str())) {
        bool l_bShowText = a_pIn->getAttributeAsBool(g_sOverlayTextName.c_str());
        if (l_bShowText != m_bShowText) {
          m_bShowText = l_bShowText;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sTextBackgroundName.c_str())) {
        irr::video::SColor l_cColor = a_pIn->getAttributeAsColor(g_sTextBackgroundName.c_str());
        if (l_cColor != m_cTextBackColor) {
          m_cTextBackColor = l_cColor;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sTextBackAlphaName.c_str())) {
        irr::s32 l_iAlpha32 = a_pIn->getAttributeAsInt(g_sTextBackAlphaName.c_str());
        if (l_iAlpha32 < 0  ) l_iAlpha32 = 0;
        if (l_iAlpha32 > 255) l_iAlpha32 = 255;
        irr::u8 l_iAlpha = (irr::u8)l_iAlpha32;

        if (l_iAlpha != m_iTextBackAlpha) {
          m_iTextBackAlpha = l_iAlpha;
          l_bUpdateRttText = true;
        }
      }

      if (a_pIn->existsAttribute(g_sMinValueName.c_str())) {
        m_fMinValue = a_pIn->getAttributeAsFloat(g_sMinValueName.c_str());
      }

      if (a_pIn->existsAttribute(g_sMaxValueName.c_str())) {
        m_fMaxValue = a_pIn->getAttributeAsFloat(g_sMaxValueName.c_str());
      }

      if (a_pIn->existsAttribute(g_sValueName.c_str())) {
        m_fValue = a_pIn->getAttributeAsFloat(g_sValueName.c_str());
      }

      if (a_pIn->existsAttribute(g_sHoverColorName.c_str())) {
        m_cHoverColor = a_pIn->getAttributeAsColor(g_sHoverColorName.c_str());
      }

      if (a_pIn->existsAttribute(g_sSliderMinOffsetName.c_str())) {
        m_fMinOffset = a_pIn->getAttributeAsFloat(g_sSliderMinOffsetName.c_str());
      }

      if (a_pIn->existsAttribute(g_sSliderMaxOffsetName.c_str())) {
        m_fMaxOffset = a_pIn->getAttributeAsFloat(g_sSliderMaxOffsetName.c_str());
      }

      if (a_pIn->existsAttribute(g_sClickColorName.c_str())) {
        m_cClickColor = a_pIn->getAttributeAsColor(g_sClickColorName.c_str());
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
      if (IsVisible) {
        m_pSmgr->registerNodeForRendering(this);

        for (irr::core::list<irr::scene::ISceneNode *>::Iterator it = Children.begin(); it != Children.end(); it++)
          (*it)->OnRegisterSceneNode();
      }

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

    std::string CGui3dItemBase::g_sTextureName         = "Texture";
    std::string CGui3dItemBase::g_sTextName            = "Text";
    std::string CGui3dItemBase::g_sAlignmentName       = "Alignment";
    std::string CGui3dItemBase::g_sTextColorName       = "Textcolor";
    std::string CGui3dItemBase::g_sBackgroundName      = "Backgroundcolor";
    std::string CGui3dItemBase::g_sHoverColorName      = "HoverColor";
    std::string CGui3dItemBase::g_sTextureWidthName    = "TextureWidth";
    std::string CGui3dItemBase::g_sTextureHeightName   = "TextureHeight";
    std::string CGui3dItemBase::g_sClickColorName      = "ClickedColor";
    std::string CGui3dItemBase::g_sMultilineName       = "Multiline";
    std::string CGui3dItemBase::g_sVertAlignName       = "VerticalAlign";
    std::string CGui3dItemBase::g_sImageFileName       = "Image";
    std::string CGui3dItemBase::g_sOverlayTextName     = "ShowText";
    std::string CGui3dItemBase::g_sTextBackgroundName  = "OverlayBackground";
    std::string CGui3dItemBase::g_sTextBackAlphaName   = "OverlayAlpha";
    std::string CGui3dItemBase::g_sMinValueName        = "MinValue";
    std::string CGui3dItemBase::g_sMaxValueName        = "MaxValue";
    std::string CGui3dItemBase::g_sValueName           = "Value";
    std::string CGui3dItemBase::g_sSliderDirectionName = "Direction";
    std::string CGui3dItemBase::g_sSliderNormalName    = "Normal";
    std::string CGui3dItemBase::g_sSliderMinOffsetName = "MinOffset";
    std::string CGui3dItemBase::g_sSliderMaxOffsetName = "MaxOffset";
  }
}
