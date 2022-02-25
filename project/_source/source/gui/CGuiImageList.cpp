// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <gui/CGuiImageList.h>
#include <gui/CMenuButton.h>
#include <Defines.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CGuiImageList::CGuiImageList(irr::gui::IGUIElement *a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_ImageListId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pGui      (nullptr),
      m_pDrv      (nullptr),
      m_pFs       (nullptr),
      m_iRows     (1),
      m_iPos      (0),
      m_iOffset   (-1),
      m_iMaxScroll(0),
      m_iMDown    (0),
      m_pBtnLeft  (nullptr),
      m_pBtnRight (nullptr),
      m_bHover    (false),
      m_bMouseDown(false)
    {
      m_pDrv = CGlobal::getInstance()->getVideoDriver   ();
      m_pGui = CGlobal::getInstance()->getGuiEnvironment();
      m_pFs  = CGlobal::getInstance()->getFileSystem    ();

      m_cImageSrc = irr::core::rectf(0.0f, 0.0f, 1.0f, 1.0f);

      m_itSelected = m_vImages.end();
      m_itHovered  = m_vImages.end();

      prepareUi();
    }

    CGuiImageList::~CGuiImageList() {
    }

    void CGuiImageList::prepareUi() {
      irr::core::recti l_cRect = AbsoluteClippingRect;

      int l_iRasterSize = CGlobal::getInstance()->getRasterSize();

      irr::core::recti l_cBtnLeft = irr::core::recti(
        l_cRect.UpperLeftCorner.X,
        l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 - 2 * l_iRasterSize,
        l_cRect.UpperLeftCorner.X + 4 * l_iRasterSize,
        l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 + 2 * l_iRasterSize
      );

      l_cBtnLeft.UpperLeftCorner  -= AbsoluteClippingRect.UpperLeftCorner;
      l_cBtnLeft.LowerRightCorner -= AbsoluteClippingRect.UpperLeftCorner;

      irr::core::recti l_cBtnRight = irr::core::recti(
        l_cRect.LowerRightCorner.X - 4 * l_iRasterSize,
        l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 - 2 * l_iRasterSize,
        l_cRect.LowerRightCorner.X,
        l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 + 2 * l_iRasterSize
      );

      l_cBtnRight.UpperLeftCorner  -= AbsoluteClippingRect.UpperLeftCorner;
      l_cBtnRight.LowerRightCorner -= AbsoluteClippingRect.UpperLeftCorner;

      m_cImages = irr::core::recti(
        l_cRect.UpperLeftCorner.X + 5 * l_iRasterSize,
        l_cRect.UpperLeftCorner.Y + l_iRasterSize,
        l_cRect.LowerRightCorner.X - 5 * l_iRasterSize,
        l_cRect.LowerRightCorner.Y - l_iRasterSize
      );

      if (m_pBtnLeft != nullptr) {
        m_pBtnLeft->setVisible(false);
        m_pBtnLeft = nullptr;
      }

      if (m_pBtnRight != nullptr) {
        m_pBtnRight->setVisible(false);
        m_pBtnRight = nullptr;
      }

      irr::io::IAttributes *l_pAttr = m_pFs->createEmptyAttributes();

      m_pBtnLeft = new CMenuButton(this);
      m_pBtnLeft->serializeAttributes(l_pAttr);
      l_pAttr->setAttribute("ImagePath", "data/images/arrow_left.png");
      m_pBtnLeft->deserializeAttributes(l_pAttr);
      m_pBtnLeft->setRelativePosition(l_cBtnLeft);
      m_pBtnLeft->drop();

      m_pBtnRight = new CMenuButton(this);
      m_pBtnRight->serializeAttributes(l_pAttr);
      l_pAttr->setAttribute("ImagePath", "data/images/arrow_right.png");
      m_pBtnRight->deserializeAttributes(l_pAttr);
      m_pBtnRight->setRelativePosition(l_cBtnRight);
      m_pBtnRight->drop();

      l_pAttr->drop();

      m_cImageSize = irr::core::dimension2du(
        (m_cImages.getHeight() - CGlobal::getInstance()->getRasterSize()) / m_iRows - CGlobal::getInstance()->getRasterSize(),
        (m_cImages.getHeight() - CGlobal::getInstance()->getRasterSize()) / m_iRows - CGlobal::getInstance()->getRasterSize()
      );

      if (m_cImages.getWidth() > 0) {
        int l_iImgWidth = m_cImageSize.Width + 2 * CGlobal::getInstance()->getRasterSize(),
            l_iColumns  = m_cImages.getWidth() / l_iImgWidth;

        if (l_iColumns > 0) {
          m_iOffset = m_cImages.getWidth() / l_iColumns;
        }
      }
    }

    void CGuiImageList::sendUserEvent() {
      irr::SEvent l_cEvent;
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventImagePosChanged;
      l_cEvent.UserEvent.UserData2 = c_iEventImagePosChanged;
      CGlobal::getInstance()->getIrrlichtDevice()->postEventFromUser(l_cEvent);
    }

    /**
    * Set the list of images
    * @param a_vImages the list of images
    */
    void CGuiImageList::setImageList(const std::vector<CGuiImageList::SListImage> a_vImages) {
      std::string l_sSelected = m_itSelected != m_vImages.end() ? (*m_itSelected).m_sData : "";

      m_vImages.clear();

      for (std::vector<SListImage>::const_iterator it = a_vImages.begin(); it != a_vImages.end(); it++) {
        m_vImages.push_back(*it);
      }

      m_itSelected = m_vImages.begin();
      m_itHovered  = m_vImages.end();

      int l_iRow = 0;

      irr::core::position2di l_cPos = m_cImages.UpperLeftCorner + irr::core::position2di(CGlobal::getInstance()->getRasterSize() / 2, CGlobal::getInstance()->getRasterSize() / 2);

      for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++) {
        if ((*it).m_sData == l_sSelected) {
          m_itSelected = it;
          sendUserEvent();
        }

        (*it).m_cDrawRect = irr::core::recti(
          l_cPos,
          m_cImageSize
        );

        l_cPos.Y += m_cImageSize.Height + CGlobal::getInstance()->getRasterSize();

        l_iRow++;
        if (l_iRow >= m_iRows) {
          l_iRow = 0;
          l_cPos.X += m_iOffset;
          l_cPos.Y  = m_cImages.UpperLeftCorner.Y;
        }
      }

      m_iMaxScroll = l_cPos.X - m_cImages.getWidth();
      m_iPos = 0;

      checkPositionAndButtons();
    }

    /**
    * Get the "name" property of the selected image
    * @return the "name" property of the selected image
    */
    std::string CGuiImageList::getSelectedName() {
      return m_itSelected != m_vImages.end() ? (*m_itSelected).m_sName : "";
    }

    /**
    * Get the "path" property of the selected image
    * @return the "path" property of the selected image
    */
    std::string CGuiImageList::getSelectedPath() {
      return m_itSelected != m_vImages.end() ? (*m_itSelected).m_sPath : "";
    }

    /**
    * Get the "data" property of the selected image
    * @return the "data" property of the selected image
    */
    std::string CGuiImageList::getSelectedData() {
      return m_itSelected != m_vImages.end() ? (*m_itSelected).m_sData : "";
    }

    /**
    * Set the selected image
    * @param a_sImage the image data to select
    */
    void CGuiImageList::setSelected(const std::string& a_sImage) {
      m_itSelected = m_vImages.end();

      for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++)
        if ((*it).m_sData == a_sImage) {
          m_itSelected = it;
          m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
          checkPositionAndButtons();
          break;
        }
    }

    /**
    * Clear the selection
    */
    void CGuiImageList::clearSelection() {
      m_itSelected = m_vImages.end();
      sendUserEvent();
    }

    /**
    * Set the source rect of the images to show
    * @param a_cImageSrc a rect<f32> with values 0..1
    */
    void CGuiImageList::setImageSourceRect(const irr::core::rectf a_cImageSrc) {
      m_cImageSrc = a_cImageSrc;
    }


    void CGuiImageList::checkPositionAndButtons() {
      if (m_iPos < 0)
        m_iPos = 0;
      else if (m_iPos > m_iMaxScroll) {
        m_iPos = m_iMaxScroll;
      }

      m_pBtnLeft->setVisible(m_itSelected != m_vImages.begin());
      m_pGui->setFocus(this);
    }

    /** Inherited from irr::gui::IGUIElement **/
    bool CGuiImageList::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_BUTTON_CLICKED) {
          if (a_cEvent.GUIEvent.Caller == m_pBtnLeft) {
            if (m_itSelected != m_vImages.begin()) {
              m_itSelected--;

              m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;

              checkPositionAndButtons();
              m_pBtnRight->setVisible(true);
              sendUserEvent();
            }

            l_bRet = true;
          }
          else if (a_cEvent.GUIEvent.Caller == m_pBtnRight) {
            if (m_itSelected == m_vImages.end()) {
              m_itSelected = m_vImages.begin();
              m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
              checkPositionAndButtons();
              m_pBtnRight->setVisible(true);
              sendUserEvent();
            }
            else {
              if (m_itSelected + 1 != m_vImages.end()) {
                m_itSelected++;
                m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
                checkPositionAndButtons();

                m_pBtnRight->setVisible(m_itSelected + 1 != m_vImages.end());
                sendUserEvent();
              }
            }

            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          if (a_cEvent.GUIEvent.Caller == this) {
            m_bHover = true;
            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          if (a_cEvent.GUIEvent.Caller == this) {
            m_bHover = false;
            l_bRet = true;
          }
        }
      }
      else if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        if (m_bHover) {
          if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
            m_bMouseDown = true;
            m_cClick = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            m_iMDown = m_cClick.X;
            l_bRet = true;
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            m_cMouse = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            if (m_bMouseDown) {
              m_iPos += m_cClick.X - a_cEvent.MouseInput.X;
              m_cClick = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
              l_bRet = true;
              checkPositionAndButtons();
            }

            irr::core::position2di l_cPos = irr::core::position2di(m_iPos, 0);

            m_itHovered = m_vImages.end();
            for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++) {
              irr::core::recti l_cRect = (*it).m_cDrawRect;
              l_cRect -= l_cPos;

              m_pDrv->draw2DImage((*it).getImage(), l_cRect, irr::core::recti(irr::core::position2di(0, 0), (*it).getImage()->getOriginalSize()), &m_cImages, nullptr, true);

              if (l_cRect.isPointInside(m_cMouse))
                m_itHovered = it;
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            if (abs(m_cMouse.X - m_iMDown) < CGlobal::getInstance()->getRasterSize() / 4 && m_itHovered != m_vImages.end()) {
              m_itSelected = m_itHovered;
              irr::SEvent l_cEvent;
              l_cEvent.EventType = irr::EET_USER_EVENT;
              l_cEvent.UserEvent.UserData1 = c_iEventImageSelected;
              l_cEvent.UserEvent.UserData2 = c_iEventImageSelected;
              CGlobal::getInstance()->getIrrlichtDevice()->postEventFromUser(l_cEvent);
              l_bRet = true;
            }
          }
        }

        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseDown = false;
          l_bRet = true;
        }
      }

      return l_bRet;
    }

    void CGuiImageList::draw() {
      irr::core::position2di l_cPos = irr::core::position2di(m_iPos, 0);

      int l_iRow = 0;

      for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++) {
        irr::core::recti l_cRect = (*it).m_cDrawRect;
        l_cRect -= l_cPos;

        irr::core::recti l_cSource = irr::core::recti(
          (irr::s32)((irr::f32)((*it).getImage()->getOriginalSize().Width ) * m_cImageSrc.UpperLeftCorner .X),
          (irr::s32)((irr::f32)((*it).getImage()->getOriginalSize().Height) * m_cImageSrc.UpperLeftCorner .Y),
          (irr::s32)((irr::f32)((*it).getImage()->getOriginalSize().Width ) * m_cImageSrc.LowerRightCorner.X),
          (irr::s32)((irr::f32)((*it).getImage()->getOriginalSize().Height) * m_cImageSrc.LowerRightCorner.Y)
        );

        m_pDrv->draw2DImage((*it).getImage(), l_cRect, l_cSource, &m_cImages, nullptr, true);

        if ((!m_bHover || !l_cRect.isPointInside(m_cMouse)) && it != m_itSelected) {
          m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), l_cRect, &m_cImages);
        }

        if (it == m_itSelected) {
          m_pDrv->draw2DRectangle(irr::video::SColor(64, 0, 0xFF, 0), l_cRect, &m_cImages);
        }
      }

      for (irr::core::list<irr::gui::IGUIElement *>::ConstIterator it = Children.begin(); it != Children.end(); it++)
        (*it)->draw();
    }

    void CGuiImageList::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      IGUIElement::serializeAttributes(a_pOut, a_pOptions);

      a_pOut->addInt("Rows", m_iRows);
    }

    void CGuiImageList::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      
      if (a_pIn->existsAttribute("Rows")) m_iRows = a_pIn->getAttributeAsInt("Rows");

      prepareUi();
    }

    irr::video::ITexture *CGuiImageList::SListImage::getImage() {
      if (m_pImage == nullptr) {
        m_pImage = CGlobal::getInstance()->getVideoDriver()->getTexture(m_sPath.c_str());
        if (m_pImage == nullptr) {
          m_pImage = CGlobal::getInstance()->getVideoDriver()->getTexture("data/images/no_image.png");
        }
      }
      return m_pImage;
    }
  }
}