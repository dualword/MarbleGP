// (w) 2021 by Dustbin::Games / Christian Keimel
#pragma once

#ifdef _LINUX_INCLUDE_PATH
#include <irrlicht.h>
#else
#include <irrlicht/irrlicht.h>
#endif

#include <scenenodes/CMarbleGPSceneNodes.h>
#include <vector>
#include <string>
#include <map>

namespace dustbin {
  namespace scenenodes {
    const int g_i3dGuiRootID    = MAKE_IRR_ID('3', 'g', 'u', 'i');  /**< The type id of the 3d UI root node */
    const int g_i3dGuiItemID = MAKE_IRR_ID('3', 'e', 'l', 'm');  /**< The type id of the 3d UI element node */

    /**
     * @class CGui3dItemBase
     * @author Christian Keimel
     * @brief This is the base node for turning the parent (of type IMeshSceneNode) into a 3d GUI item. The actual functionality is added in the derived class used in the application
     * @see CGui3dItem
     */
    class CGui3dItemBase : public irr::scene::ISceneNode, public CSceneNodeBase {
      public:
        /**
         * This enumeration defines the type of 3d GUI item
         */
        enum class enGui3dType {
          None = 0,       /**< No item at all */
          Decoration = 1, /**< Decoration item */
          Label = 2,      /**< A text label */
          Button = 3,     /**< A text button */
          IconButton = 4, /**< An icon button, i.e. the 3d model of the mesh represents the icon */
          Image = 5       /**< An image, can also have text overlaying */
        };

        /**
         * Enumeration for text alignment
         */
        enum class enGui3dAlign {
          Left = 0,   /**< Left align */
          Center = 1, /**< Centered text */
          Right = 2   /**< Right align */
        };

        /**
         * Enumeration for vertical aligment
         */
        enum class enGui3dVerticalAlign {
          Top = 0,    /**< Top aligment */
          Middle = 1, /**< Text aligned in the middle of the texture */
          Bottom = 2  /**< Bottom text alignment */
        };

        static std::string g_sTextureName,        /**< The serialization name of the texture attribute */
                           g_sTextName,           /**< The serialization name of the text attribute */
                           g_sAlignmentName,      /**< The serialization name of the alignment attribute */
                           g_sTextColorName,      /**< The serialization name of the text color attribute */
                           g_sBackgroundName,     /**< The serialization name of the background color attribute */
                           g_sHoverColorName,     /**< The serialization name of the hover color attribute */
                           g_sTextureWidthName,   /**< The serialization name of the texture width attribute */
                           g_sTextureHeightName,  /**< The serialization name of the texture height attribute */
                           g_sClickColorName,     /**< The serialization name of the click color attribute */
                           g_sMultilineName,      /**< The serialization name of the multiline attribute */
                           g_sVertAlignName,      /**< The serialization name of the vertical align attribute */
                           g_sOverlayTextName,    /**< The serialization name of the overlay text for images */
                           g_sImageFileName,      /**< The serialization name of the image file */
                           g_sTextBackgroundName, /**< The serialization name of the text background for image overlay text */
                           g_sTextBackAlphaName;  /**< The serialization name of the alpha channel for image text background */
  
      protected:
        irr::core::aabbox3df       m_cBox;            /**< The bounding  */
        irr::video::SMaterial      m_cMaterial;       /**< The material  */
        irr::scene::ISceneManager *m_pSmgr;           /**< The scene manager */
        irr::video::ITexture      *m_pRttTexture;     /**< The Render to Target Texture */
        irr::gui::IGUIFont        *m_pFont;           /**< The font to use for rendering */
        irr::core::rect<irr::s32>  m_cRect;           /**< The rectangle to draw text to */
        irr::video::SColor         m_cBackground,     /**< The background color */
                                   m_cHoverColor,     /**< The "mouse over" color */
                                   m_cTextColor,      /**< The text color */
                                   m_cClickColor,     /**< The color of a button when clicked */
                                   m_cTextBackColor;  /**< The text background color used if the Image element has an overlay text */

        int m_iRttTexture;  /**< The texture number of the parent that will be modified */

        std::wstring m_sText;   /**< The text of the UI element */
        std::string  m_sImage;  /**< The image file name */

        bool m_bMultiLine,      /**< Is the text rendered multiline? */
             m_bShowText;       /**< Does the image show an overlay text? */

        irr::u8 m_iTextBackAlpha; /**< The alpha channel of text background for image elements */

        static std::string m_sNodeTypeName;  /**< The node type name */

        std::map<enGui3dType, std::vector<std::string> > m_mSerializerMap;  /**< This map shows which UI type serializes which properties */

#ifdef _IRREDIT_PLUGIN
        irr::core::dimension2di m_cRttSize; /**< The size of the render target texture */
#else
        irr::core::dimension2du m_cRttSize; /**< The size of the render target texture */
#endif

        enGui3dType          m_eType;
        enGui3dAlign         m_eAlign;
        enGui3dVerticalAlign m_eVerticalAlign;

        std::string typeToString(enGui3dType a_eType);
        std::string alignToString(enGui3dAlign a_eAlign);

        enGui3dAlign stringToAlign(const std::string &a_sAlign);
        enGui3dVerticalAlign stringToVerticalAlign(const std::string &a_sAlign);
        
        virtual void updateRttText(const irr::video::SColor &a_cBackgroundColor, const irr::video::SColor &a_cTextColor);
        void updateRttTexture();

      public:
        CGui3dItemBase(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dItemBase();

        virtual void OnRegisterSceneNode();
        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32> &getBoundingBox() const;
        virtual irr::u32 getMaterialCount();
        virtual irr::video::SMaterial &getMaterial(irr::u32 i);
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode *a_pNewParent, irr::scene::ISceneManager *a_pNewSmgr);

        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        static const std::string &getNodeTypeName();
        static const irr::scene::ESCENE_NODE_TYPE getNodeType();
    };

    /**
     * @class C3dGuiRootBase
     * @author Christian Keimel
     * @brief This is the base class for the 3d GUI that converts it's CGui3dItem children to 3d GUI elements.
     * @see CGui3dItem
     * @see CGui3dRoot
     * 
     * CGui3dRootBase is on one hand the class that is instantiated for the editor and on the other hand the base
     * class for CGui3dRoot which holds functions to handle user interaction
     */
    class CGui3dRootBase : public irr::scene::ISceneNode, public CSceneNodeBase {
      protected:
        irr::core::aabbox3df                m_cBox;       /**< The bounding  */
        irr::video::SMaterial               m_cMaterial;  /**< The material  */
        irr::scene::ISceneManager          *m_pSmgr;      /**< The scene manager */
        irr::scene::ISceneCollisionManager *m_pColMgr;    /**< The scene collision manager necessary to detect hovers and such */

        static std::string m_sNodeTypeName;  /**< The node type name */

      public:
        CGui3dRootBase(irr::scene::ISceneNode *a_pParent, irr::scene::ISceneManager *a_pSmgr, irr::s32 a_iId);
        virtual ~CGui3dRootBase();

        virtual void OnRegisterSceneNode();
        virtual void render();
        virtual const irr::core::aabbox3d<irr::f32> &getBoundingBox() const;
        virtual irr::u32 getMaterialCount();
        virtual irr::video::SMaterial &getMaterial(irr::u32 i);
        virtual irr::scene::ESCENE_NODE_TYPE getType() const;
        virtual irr::scene::ISceneNode *clone(irr::scene::ISceneNode *a_pNewParent, irr::scene::ISceneManager *a_pNewSmgr);

        virtual void serializeAttributes(irr::io::IAttributes *a_pOut, irr::io::SAttributeReadWriteOptions *a_pOptions) const;
        virtual void deserializeAttributes(irr::io::IAttributes *a_pIn, irr::io::SAttributeReadWriteOptions *a_pOptions);

        static const std::string &getNodeTypeName();
        static const irr::scene::ESCENE_NODE_TYPE getNodeType();
    };
  }
}