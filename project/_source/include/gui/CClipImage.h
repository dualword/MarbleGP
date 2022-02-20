// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#pragma once

#include <irrlicht.h>

#include <string>

namespace dustbin {
  namespace gui {
    const int g_ClipImageId = MAKE_IRR_ID('d', 'g', 'c', 'i');
    const irr::c8 g_ClipImageName[] = "ClipImage";

    /**
    * @class CClipImage
    * @author Christian Keimel
    * A GUI Image object that only shows a part of it's image
    */
    class CClipImage : public irr::gui::IGUIElement {
    private:
      irr::gui::IGUIEnvironment *m_pGui;
      irr::video::IVideoDriver  *m_pDrv;
      irr::video::ITexture      *m_pTexture;

      std::string m_sTexture; /**< The texture string */

      irr::video::SColor m_cHover,  /**< The hover color */
                         m_cClick;  /**< The click color */

      int m_iX1,  /**< The X coordinate of the upper left corner of the clipping rectangle */
          m_iY1,  /**< The Y coordinate of the upper left corner of the clipping rectangle */
          m_iX2,  /**< The X coordinate of the lower right corner of the clipping rectangle */
          m_iY2;  /**< The Y coordinate of the upper left corner of the clipping rectangle */

      bool m_bLDown,  /**< Is the left mouse button pressed? */
           m_bHover;  /**< Is this element hovered? */

    public:
      CClipImage(irr::gui::IGUIElement* a_pParent);
      virtual ~CClipImage();

      virtual void draw();

      /**
      * Change the image of the item
      * @param a_pTexture the new image
      */
      void setImage(irr::video::ITexture* a_pTexture);

      /**
      * Get the image of the item
      * @return the image of the item
      */
      irr::video::ITexture *getImage();

      virtual bool OnEvent(const irr::SEvent& a_cEvent);

      virtual void serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions = 0) const;
      virtual void deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions = 0);
    };
  }
}
