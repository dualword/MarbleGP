// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/CButtonRenderer.h>
#include <irrlicht.h>
#include <string>
#include <vector>
#include <tuple>

namespace dustbin {
  namespace gui {
    class CMenuButton;  /**< Forward declaration of the menu button class */

    const int g_ImageListId = MAKE_IRR_ID('i', 'm', 'g', 's');
    const irr::c8 g_ImageListName[] = "DustbinImageList";

    /**
    * @class CGuiImageList
    * @author Christian Keimel
    * This class provides a selection for images. The images are shown in a list
    * with a configurable number of lines and can be moved by dragging.
    */
    class CGuiImageList : public irr::gui::IGUIElement {
      public:
        /**
        * @class SListImage
        * @author Christian Keimel
        * This data structure holds all data necessary for the image selection
        */
        struct SListImage {
          std::string m_sPath;    /**< The path of the image */
          std::string m_sName;    /**< The displayed name of the image */
          std::string m_sData;    /**< Some data that can be used to identify the selected image */

          std::wstring m_sCategory; /**< Category for tracks */

          irr::video::ITexture *m_pImage;   /**< The image texture */

          irr::core::recti m_cDrawRect;   /**< The screen rect the texture is drawn to */

          SListImage(const std::string a_sPath, const std::string a_sName, const std::string a_sData) : m_sPath(a_sPath), m_sName(a_sName), m_sData(a_sData), m_sCategory(L""), m_pImage(nullptr) {
          }

          SListImage() : m_sPath(""), m_sName(""), m_sData(""), m_sCategory(L""), m_pImage(nullptr) {
          }

          SListImage(const SListImage& a_cOther) : m_sPath(a_cOther.m_sPath), m_sName(a_cOther.m_sName), m_sData(a_cOther.m_sData), m_sCategory(a_cOther.m_sCategory), m_pImage(a_cOther.m_pImage), m_cDrawRect(a_cOther.m_cDrawRect) {
          }

          irr::video::ITexture *getImage();
        };

      private:
        enum class enInternalButtons {
          PrevImage    = 0,
          NextImage    = 1,
          PrevCategory = 2,
          NextCategory = 3
        };

        irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht gui environment */
        irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht video driver */
        irr::io::IFileSystem      *m_pFs;     /**< The Irrlicht file system */
        irr::gui::ICursorControl  *m_pCursor; /**< The cursor control */

        int m_iRows;        /**< The number of rows of the list */
        int m_iPos;         /**< The scroll position */
        int m_iOffset;      /**< The horizontal offset from one image columnn to the next */
        int m_iMaxScroll;   /**< The maximum scroll position */
        int m_iMDown;       /**< The last mouse-down X-Coordinate */
        
        irr::core::rectf m_cImageSrc;   /**< The rect of the display to show */

        irr::core::recti m_cImages;     /**< The rect of the image list */

        irr::core::dimension2du m_cImageSize; /**< Size of the images */

        std::vector<SListImage> m_vImages;    /**< The list of images */

        std::vector<SListImage>::iterator m_itSelected;   /**< The selected image */
        std::vector<SListImage>::iterator m_itHovered;    /**< The hovered image */

        bool m_bHover;        /**< Is the element hovered? */
        bool m_bMouseDown;    /**< Was a button pressed while hovering? */
        bool m_bCategories;   /**< Show categories (for the tracks) */
        bool m_bShowSelected; /**< Add a headline showing the name of the selected item */
        bool m_bOneCatPage;   /**< Categories fit on one page */
        bool m_bScrollTrack;  /**< Scroll the track list (true) or the categories (false)? */
        bool m_bImgChangeEv;  /**< Send image change events */

        bool m_aBtnHover[4];    /**< Is any of the internal buttons hovered? */
        bool m_aBtnClick[4];    /**< Is any of the internal buttons clicked? */

        std::tuple<irr::core::recti, irr::video::ITexture *> m_aButtons[4]; /**< Rects and images of the internal buttons */

        irr::core::position2di m_cClick;  /**< The mouse position for dragging */
        irr::core::position2di m_cMouse;  /**< The current mouse position */

        irr::core::recti m_cHeadline;       /**< The headline rect (if wanted) */
        irr::core::recti m_cCategoryOuter;  /**< The outer rect of the category list (if wanted) */
        irr::core::recti m_cCategoryInner;  /**< The inner rect of the category list (if wanted) */

        irr::gui::IGUIFont *m_pFontSelected;    /**< The font for the selected image */
        irr::gui::IGUIFont *m_pFontCategory;    /**< The font for the category */

        CButtonRenderer m_cBtnRenderer;

        std::wstring m_sCategory;   /**< The currently selected category */
        
        std::vector<std::tuple<std::wstring, irr::core::recti>> m_vCategories;    /**< Categories of the items */

        int m_iCategoryOffset;
        int m_iCategoryMax;

        void prepareUi();
        void checkPositionAndButtons();
        void sendImagePosition();
        void sendImageSelected();

        void selectPrevImage();
        void selectNextImage();

        void selectPrevCategory();
        void selectNextCategory();

    public:
        CGuiImageList(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiImageList();

        /**
        * Set the list of images
        * @param a_vImages the list of images
        */
        void setImageList(const std::vector<CGuiImageList::SListImage> a_vImages);

        /**
        * Get the "name" property of the selected image
        * @return the "name" property of the selected image
        */
        std::string getSelectedName();

        /**
        * Get the "path" property of the selected image
        * @return the "path" property of the selected image
        */
        std::string getSelectedPath();

        /**
        * Get the "data" property of the selected image
        * @return the "data" property of the selected image
        */
        std::string getSelectedData();

        /**
        * Set the selected image
        * @param a_sImage the image data to select
        * @param a_bSendEvent send an image selected event?
        */
        void setSelected(const std::string &a_sImage, bool a_bSendEvent);
        
        /**
        * Clear the selection
        */
        void clearSelection();

        /**
        * Set the source rect of the images to show
        * @param a_cImageSrc a rect<f32> with values 0..1
        */
        void setImageSourceRect(const irr::core::rectf a_cImageSrc);

        /** Inherited from irr::gui::IGUIElement **/
        virtual bool OnEvent(const irr::SEvent& a_cEvent);
        virtual void draw();

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
    };
  }
}
