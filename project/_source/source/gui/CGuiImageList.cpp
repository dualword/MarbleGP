// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel
#include <helpers/CStringHelpers.h>
#include <gui/CGuiImageList.h>
#include <gui/CMenuButton.h>
#include <Defines.h>
#include <CGlobal.h>

namespace dustbin {
  namespace gui {
    CGuiImageList::CGuiImageList(irr::gui::IGUIElement *a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_ImageListId, CGlobal::getInstance()->getGuiEnvironment(), a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), -1, irr::core::recti()),
      m_pGui           (nullptr),
      m_pDrv           (nullptr),
      m_pFs            (nullptr),
      m_iRows          (1),
      m_iPos           (0),
      m_iOffset        (-1),
      m_iMaxScroll     (0),
      m_iMDown         (0),
      m_pBtnLeft       (nullptr),
      m_pBtnRight      (nullptr),
      m_bHover         (false),
      m_bMouseDown     (false),
      m_bCategories    (false),
      m_bShowSelected  (false),
      m_bOneCatPage    (false),
      m_bScrollTrack   (true),
      m_pTextureLeft   (nullptr),
      m_pTextureRght   (nullptr),
      m_pFontSelected  (nullptr),
      m_pFontCategory  (nullptr),
      m_sCategory      (L""),
      m_iCategoryOffset(0),
      m_iCategoryMax   (0)
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

      if (m_pFontSelected == nullptr) m_pFontSelected = CGlobal::getInstance()->getFont(enFont::Huge, m_pDrv->getScreenSize());
      if (m_pFontCategory == nullptr) m_pFontCategory = CGlobal::getInstance()->getFont(enFont::Big , m_pDrv->getScreenSize());

      m_pTextureLeft = m_pDrv->getTexture("data/images/arrow_left.png");
      m_pTextureRght = m_pDrv->getTexture("data/images/arrow_right.png");

      if (m_bShowSelected && m_pFontSelected != nullptr) {
        int l_iHeight = 5 * m_pFontSelected->getDimension(L"TesTexT").Height / 4;
        m_cHeadline = irr::core::recti(l_cRect.UpperLeftCorner, irr::core::vector2di(l_cRect.LowerRightCorner.X, l_cRect.UpperLeftCorner.Y + l_iHeight));

        l_cRect.UpperLeftCorner.Y += l_iHeight;
      }

      if (m_bCategories && m_pFontSelected != nullptr) {
        int l_iHeight = 2 * m_pFontCategory->getDimension(L"TesTexT").Height;

        l_cRect.UpperLeftCorner.Y += l_iHeight / 8;
        
        m_cCategoryOuter = irr::core::recti(l_cRect.UpperLeftCorner, irr::core::vector2di(l_cRect.LowerRightCorner.X, l_cRect.UpperLeftCorner.Y + l_iHeight));

        int l_iOffset = m_cCategoryOuter.getHeight() / 8;

        m_cCategoryInner = irr::core::recti(m_cCategoryOuter.UpperLeftCorner + irr::core::vector2di(l_iOffset, l_iOffset), m_cCategoryOuter.LowerRightCorner - irr::core::vector2di(l_iOffset, l_iOffset));

        m_cCategoryLeft = irr::core::recti(m_cCategoryInner.UpperLeftCorner, m_cCategoryInner.UpperLeftCorner + irr::core::vector2di(m_cCategoryInner.getHeight(), m_cCategoryInner.getHeight()));
        m_cCategoryRght = irr::core::recti(m_cCategoryInner.LowerRightCorner - irr::core::vector2di(m_cCategoryInner.getHeight(), m_cCategoryInner.getHeight()), m_cCategoryInner.LowerRightCorner);

        m_cCategoryInner.UpperLeftCorner .X += m_cCategoryInner.getHeight();
        m_cCategoryInner.LowerRightCorner.X -= m_cCategoryInner.getHeight();

        l_cRect.UpperLeftCorner.Y += l_iHeight;
      }

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

    void CGuiImageList::sendImagePosition() {
      irr::SEvent l_cEvent{};
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventImagePosChanged;
      l_cEvent.UserEvent.UserData2 = c_iEventImagePosChanged;
      CGlobal::getInstance()->getIrrlichtDevice()->postEventFromUser(l_cEvent);
    }

    void CGuiImageList::sendImageSelected() {
      irr::SEvent l_cEvent{};
      l_cEvent.EventType = irr::EET_USER_EVENT;
      l_cEvent.UserEvent.UserData1 = c_iEventImageSelected;
      l_cEvent.UserEvent.UserData2 = c_iEventImageSelected;
      CGlobal::getInstance()->getIrrlichtDevice()->postEventFromUser(l_cEvent);
    }

    /**
    * Set the list of images
    * @param a_vImages the list of images
    */
    void CGuiImageList::setImageList(const std::vector<CGuiImageList::SListImage> a_vImages) {
      std::string l_sSelected = m_itSelected != m_vImages.end() ? (*m_itSelected).m_sData : "";

      m_vImages.clear();

      std::vector<std::wstring> l_vCategories;

      for (std::vector<SListImage>::const_iterator it = a_vImages.begin(); it != a_vImages.end(); it++) {
        m_vImages.push_back(*it);

        std::wstring l_sCategory = helpers::s2ws((*it).m_sName);

        if (l_sCategory.find_first_of(L':') != std::string::npos) {
          l_sCategory = l_sCategory.substr(0, l_sCategory.find_first_of(L':'));
        }
        else {
          l_sCategory = L"";
        }

        if (m_bCategories) {
          if (std::find(l_vCategories.begin(), l_vCategories.end(), l_sCategory) == l_vCategories.end())
            l_vCategories.push_back(l_sCategory);

          m_vImages.back().m_sCategory = l_sCategory;
        }
      }

      if (m_bCategories) {
        std::sort(l_vCategories.begin(), l_vCategories.end(), [](const std::wstring& s1, const std::wstring& s2) {
          std::wstring l1 = s1;
          std::transform(l1.begin(), l1.end(), l1.begin(),
            [](wchar_t c){ return std::tolower(c); });

          std::wstring l2 = s2;
          std::transform(l2.begin(), l2.end(), l2.begin(),
            [](wchar_t c){ return std::tolower(c); });

          return l1 < l2;
        });

        irr::core::position2di l_cPos = m_cCategoryInner.UpperLeftCorner;
        int l_iOffset = m_cCategoryInner.getHeight();
        int l_iWidth  = 0;

        for (std::vector<std::wstring>::iterator it = l_vCategories.begin(); it != l_vCategories.end(); it++) {
          irr::core::dimension2du l_cDim = m_pFontCategory->getDimension((*it).c_str());
          irr::core::recti l_cRect = irr::core::recti(l_cPos, l_cDim);

          l_cRect.LowerRightCorner.Y = m_cCategoryInner.LowerRightCorner.Y;

          m_vCategories.push_back(std::make_tuple(*it, l_cRect));
          l_cPos.X += l_cDim.Width + l_iOffset;
          l_iWidth += l_cDim.Width + l_iOffset;
        }

        // If the categories do not fill the entire inner rect we center them
        if (l_cPos.X < m_cCategoryInner.LowerRightCorner.X) {
          l_iWidth -= l_iOffset;
          m_iCategoryOffset = -(m_cCategoryInner.getWidth() / 2 - l_iWidth / 2);
          m_bOneCatPage = true;
        }
        else {
          m_iCategoryMax = std::get<1>(m_vCategories.back()).LowerRightCorner.X - m_cCategoryInner.getWidth();
        }
      }

      m_itSelected = m_vImages.begin();
      m_itHovered  = m_vImages.end();

      int l_iRow = 0;

      irr::core::position2di l_cPos = m_cImages.UpperLeftCorner + irr::core::position2di(CGlobal::getInstance()->getRasterSize() / 2, CGlobal::getInstance()->getRasterSize() / 2);

      for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++) {
        if ((*it).m_sData == l_sSelected) {
          m_itSelected = it;
          sendImagePosition();
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
          sendImageSelected();
          break;
        }
    }

    /**
    * Clear the selection
    */
    void CGuiImageList::clearSelection() {
      m_itSelected = m_vImages.end();
      sendImagePosition();
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

      if (m_itSelected != m_vImages.end()) {
        m_sCategory = (*m_itSelected).m_sCategory;
      }
      else m_sCategory = L"";
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
              sendImagePosition();
            }

            l_bRet = true;
          }
          else if (a_cEvent.GUIEvent.Caller == m_pBtnRight) {
            if (m_itSelected == m_vImages.end()) {
              m_itSelected = m_vImages.begin();
              m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
              checkPositionAndButtons();
              m_pBtnRight->setVisible(true);
              sendImagePosition();
            }
            else {
              if (m_itSelected + 1 != m_vImages.end()) {
                m_itSelected++;
                m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
                checkPositionAndButtons();

                m_pBtnRight->setVisible(m_itSelected + 1 != m_vImages.end());
                sendImagePosition();
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
            m_bScrollTrack = !m_cCategoryInner.isPointInside(m_cMouse);

            m_bMouseDown = true;
            m_cClick = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            m_iMDown = m_cClick.X;

            for (std::vector<std::tuple<std::wstring, irr::core::recti>>::iterator it = m_vCategories.begin(); it != m_vCategories.end(); it++) {
              irr::core::recti r = std::get<1>(*it);

              r.UpperLeftCorner.X -= m_iCategoryOffset;
              r.LowerRightCorner.X -= m_iCategoryOffset;

              if (r.isPointInside(m_cClick)) {
              if (m_sCategory != std::get<0>(*it)) {
                m_sCategory = std::get<0>(*it);
                for (m_itSelected = m_vImages.begin(); m_itSelected != m_vImages.end(); m_itSelected++) {
                  if ((*m_itSelected).m_sCategory == m_sCategory) {
                    m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
                    checkPositionAndButtons();
                    sendImageSelected();
                    break;
                  }
                }
              }
              break;
              }
            }

            if (m_cCategoryRght.isPointInside(m_cMouse)) {
              for (std::vector<std::tuple<std::wstring, irr::core::recti>>::iterator it = m_vCategories.begin(); it != m_vCategories.end(); it++) {
                if (m_sCategory == std::get<0>(*it) && it + 1 != m_vCategories.end()) {
                  it++;
                  m_sCategory = std::get<0>(*it);
                  for (m_itSelected = m_vImages.begin(); m_itSelected != m_vImages.end(); m_itSelected++) {
                    if ((*m_itSelected).m_sCategory == m_sCategory) {
                      m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
                      checkPositionAndButtons();
                      sendImageSelected();
                      break;
                    }
                  }
                  break;
                }
              }
            }

            if (m_cCategoryLeft.isPointInside(m_cMouse)) {
              for (std::vector<std::tuple<std::wstring, irr::core::recti>>::iterator it = m_vCategories.begin(); it != m_vCategories.end(); it++) {
                if (m_sCategory == std::get<0>(*it) && it != m_vCategories.begin()) {
                  it--;
                  m_sCategory = std::get<0>(*it);
                  for (m_itSelected = m_vImages.begin(); m_itSelected != m_vImages.end(); m_itSelected++) {
                    if ((*m_itSelected).m_sCategory == m_sCategory) {
                      m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
                      checkPositionAndButtons();
                      sendImageSelected();
                      break;
                    }
                  }
                  break;
                }
              }
            }
            l_bRet = true;
          }

          else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
            m_cMouse = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

            if (m_bMouseDown) {
              if (m_bScrollTrack || m_bOneCatPage) {
                m_iPos += m_cClick.X - a_cEvent.MouseInput.X;
              }
              else {
                m_iCategoryOffset += m_cClick.X - a_cEvent.MouseInput.X;
                if (m_iCategoryOffset < 0) m_iCategoryOffset = 0;
                if (m_iCategoryOffset > m_iCategoryMax) m_iCategoryOffset = m_iCategoryMax;
              }

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

              if (l_cRect.isPointInside(m_cMouse)) {
                m_itHovered = it;
              }
            }
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            if (abs(m_cMouse.X - m_iMDown) < CGlobal::getInstance()->getRasterSize() / 4 && m_itHovered != m_vImages.end()) {
              m_itSelected = m_itHovered;
              checkPositionAndButtons();
              sendImageSelected();
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

      if (m_bShowSelected) {
        m_pFontSelected->draw(helpers::s2ws(getSelectedName()).c_str(), m_cHeadline, irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cHeadline);
      }

      if (m_bCategories) {
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), m_cCategoryInner);

        if (m_pTextureLeft != nullptr) {
          if (m_cCategoryLeft.isPointInside(m_cMouse))
            m_pDrv->draw2DRectangle(m_bMouseDown ? irr::video::SColor(128, 0xec, 0xf1, 0x63) : irr::video::SColor(128, 0x33, 0x67, 0xb8), m_cCategoryLeft);

          m_pDrv->draw2DImage(m_pTextureLeft, m_cCategoryLeft, irr::core::recti(irr::core::vector2di(0, 0), m_pTextureLeft->getOriginalSize()), &m_cCategoryLeft, nullptr, true);
        }

        if (m_pTextureRght != nullptr) {
          if (m_cCategoryRght.isPointInside(m_cMouse))
            m_pDrv->draw2DRectangle(m_bMouseDown ? irr::video::SColor(128, 0xec, 0xf1, 0x63) : irr::video::SColor(128, 0x33, 0x67, 0xb8), m_cCategoryRght);

          m_pDrv->draw2DImage(m_pTextureRght, m_cCategoryRght, irr::core::recti(irr::core::vector2di(0, 0), m_pTextureRght->getOriginalSize()), &m_cCategoryRght, nullptr, true);
        }

        for (std::vector<std::tuple<std::wstring, irr::core::recti>>::iterator it = m_vCategories.begin(); it != m_vCategories.end(); it++) {
          irr::core::recti l_cRect = std::get<1>(*it);
          l_cRect.UpperLeftCorner .X -= m_iCategoryOffset;
          l_cRect.LowerRightCorner.X -= m_iCategoryOffset;


          irr::video::SColor l_cColor = 
            l_cRect.isPointInside(m_cMouse) ? 
            (
              m_bMouseDown ? irr::video::SColor(0xFF, 0xec, 0xf1, 0x63) : irr::video::SColor(0xFF, 0x33, 0x67, 0xb8)
            ) 
            : 
            m_sCategory == std::get<0>(*it) ? 
            irr::video::SColor(0xFF, 0xec, 0xf1, 0x63)
            : 
            irr::video::SColor(0xFF, 0, 0, 0);

          m_pFontCategory->draw(std::get<0>(*it).c_str(), l_cRect, l_cColor, true, true, &m_cCategoryInner);
        }
      }

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

      a_pOut->addInt ("Rows"        , m_iRows        );
      a_pOut->addBool("Categories"  , m_bCategories  );
      a_pOut->addBool("ShowSelected", m_bShowSelected);
    }

    void CGuiImageList::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      
      if (a_pIn->existsAttribute("Rows"        )) m_iRows         = a_pIn->getAttributeAsInt ("Rows"        );
      if (a_pIn->existsAttribute("Categories"  )) m_bCategories   = a_pIn->getAttributeAsBool("Categories"  );
      if (a_pIn->existsAttribute("ShowSelected")) m_bShowSelected = a_pIn->getAttributeAsBool("ShowSelected");

      if (a_pIn->existsAttribute("SelectedFont")) {
        std::string s = a_pIn->getAttributeAsString("SelectedFont").c_str();

        dustbin::enFont l_eFont = s == "tiny" ? enFont::Tiny : s == "small" ? enFont::Small : s == "big" ? enFont::Big : s == "huge" ? enFont::Huge : enFont::Regular;
        m_pFontSelected = CGlobal::getInstance()->getFont(l_eFont, m_pDrv->getScreenSize());
      }

      if (a_pIn->existsAttribute("CategoryFont")) {
        std::string s = a_pIn->getAttributeAsString("CategoryFont").c_str();

        dustbin::enFont l_eFont = s == "tiny" ? enFont::Tiny : s == "small" ? enFont::Small : s == "big" ? enFont::Big : s == "huge" ? enFont::Huge : enFont::Regular;
        m_pFontCategory = CGlobal::getInstance()->getFont(l_eFont, m_pDrv->getScreenSize());
      }

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