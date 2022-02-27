// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>
#include <string>
#include <vector>

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

          irr::video::ITexture *m_pImage;   /**< The image texture */

          irr::core::recti m_cDrawRect;   /**< The screen rect the texture is drawn to */

          SListImage(const std::string a_sPath, const std::string a_sName, const std::string a_sData) : m_sPath(a_sPath), m_sName(a_sName), m_sData(a_sData), m_pImage(nullptr) {
          }

          SListImage() : m_sPath(""), m_sName(""), m_sData(""), m_pImage(nullptr) {
          }

          SListImage(const SListImage& a_cOther) : m_sPath(a_cOther.m_sPath), m_sName(a_cOther.m_sName), m_sData(a_cOther.m_sData), m_pImage(a_cOther.m_pImage), m_cDrawRect(a_cOther.m_cDrawRect) {
          }

          irr::video::ITexture *getImage();
        };

      private:
        irr::gui::IGUIEnvironment *m_pGui;  /**< The Irrlicht gui environment */
        irr::video::IVideoDriver  *m_pDrv;  /**< The Irrlicht video driver */
        irr::io::IFileSystem      *m_pFs;   /**< The Irrlicht file system */

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

        CMenuButton *m_pBtnLeft,    /**< The "Left" button */
                    *m_pBtnRight;   /**< The "right" button */

        bool m_bHover,      /**< Is the element hovered? */
             m_bMouseDown;  /**< Was a button pressed while hovering? */

        irr::core::position2di m_cClick;  /**< The mouse position for dragging */
        irr::core::position2di m_cMouse;  /**< The current mouse position */

        void prepareUi();
        void checkPositionAndButtons();
        void sendImagePosition();
        void sendImageSelected();

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
        */
        void setSelected(const std::string &a_sImage);
        
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
