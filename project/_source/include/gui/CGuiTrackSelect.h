// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <gui/IGuiMoveOptionElement.h>
#include <gui/CButtonRenderer.h>
#include <gui/CGuiImageList.h>
#include <irrlicht.h>
#include <string>
#include <vector>
#include <tuple>
#include <map>

namespace dustbin {
  namespace gui {
    class CMenuButton;  /**< Forward declaration of the menu button class */

    const int g_TrackSelectId = MAKE_IRR_ID('d', 'g', 't', 's');
    const irr::c8 g_TrackSelectName[] = "DustbinTrackSelect";

    /**
    * @class CGuiImageList
    * @author Christian Keimel
    * This class provides a selection for images. The images are shown in a list
    * with a configurable number of lines and can be moved by dragging.
    */
    class CGuiTrackSelect : public irr::gui::IGUIElement, public IGuiMoveOptionElement {
      private:
        enum class enInternalButtons {
          PrevImage    = 0,
          NextImage    = 1
        };

        irr::gui::IGUIEnvironment *m_pGui;    /**< The Irrlicht gui environment */
        irr::video::IVideoDriver  *m_pDrv;    /**< The Irrlicht video driver */
        irr::io::IFileSystem      *m_pFs;     /**< The Irrlicht file system */
        irr::gui::ICursorControl  *m_pCursor; /**< The cursor control */

        int m_iPos;         /**< The scroll position */
        int m_iOffset;      /**< The horizontal offset from one image columnn to the next */
        int m_iMaxScroll;   /**< The maximum scroll position */
        int m_iMinScroll;   /**< The minimum scroll position */
        int m_iMDown;       /**< The last mouse-down X-Coordinate */
        int m_iCenterSize;  /**< Border size of the central rectangle showing the selected track */

        irr::core::rectf m_cImageSrc;   /**< The rect of the display to show */

        irr::core::recti m_cImages;     /**< The rect of the image list */

        irr::core::dimension2du m_cImageSize; /**< Size of the images */

        std::vector<CGuiImageList::SListImage> m_vImages;    /**< The list of images */

        std::vector<CGuiImageList::SListImage>::iterator m_itSelected;   /**< The selected image */

        bool m_bHover;        /**< Is the element hovered? */
        bool m_bMouseDown;    /**< Was a button pressed while hovering? */
        bool m_bOneCatPage;   /**< Categories fit on one page */
        bool m_bScrollTrack;  /**< Scroll the track list (true) or the categories (false)? */
        bool m_bCategory;     /**< Was the category rectangle clicked? */
        bool m_bImgChangeEv;  /**< Send image change events */

        bool m_aBtnHover[2];    /**< Is any of the internal buttons hovered? */
        bool m_aBtnClick[2];    /**< Is any of the internal buttons clicked? */

        std::tuple<irr::core::recti, irr::video::ITexture *> m_aButtons[2]; /**< Rects and images of the internal buttons */

        irr::core::position2di m_cClick;  /**< The mouse position for dragging */
        irr::core::position2di m_cMouse;  /**< The current mouse position */

        irr::core::recti m_cHeadline;       /**< The headline rect (if wanted) */
        irr::core::recti m_cCategoryOuter;  /**< The outer rect of the category list (if wanted) */
        irr::core::recti m_cCategoryInner;  /**< The inner rect of the category list (if wanted) */
        irr::core::recti m_cCenter;         /**< The center rect showing the selected track */

        irr::gui::IGUIFont *m_pFontSelected;    /**< The font for the selected image */
        irr::gui::IGUIFont *m_pFontCategory;    /**< The font for the category */

        CButtonRenderer m_cBtnRenderer;

        std::wstring m_sCategory;   /**< The currently selected category */

        std::map<std::string, int> m_mCategoryRanking;    /**< Category ranking */

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

        void checkSelection();

      public:
        CGuiTrackSelect(irr::gui::IGUIElement *a_pParent);
        virtual ~CGuiTrackSelect();

        /**
        * Set the list of images
        * @param a_vImages the list of images
        */
        void setImageList(std::vector<CGuiImageList::SListImage> &a_vImages);

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

        /**
        * Set the category ranking
        * @param a_mRanking the category ranking (key == name of the category, lowercase, value == rank)
        */
        void setCategoryRanking(std::map<std::string, int> &a_mRanking);

        /**
        * Get a position to move to depending on the direction and the given mouse position
        * @param a_cMousePos the mouse position
        * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
        * @param a_cOut the position to move to
        * @return true if a position was found, false otherwise
        */
        virtual bool getMoveOption(const irr::core::position2di &a_cMousePos, int a_iDirection, irr::core::position2di &a_cOut) override;

        /** Inherited from irr::gui::IGUIElement **/
        virtual bool OnEvent(const irr::SEvent& a_cEvent) override;
        virtual void draw() override;

        virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const override;
        virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) override;
    };
  }
}
