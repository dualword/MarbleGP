// (w) 2020 - 2024 by Dustbin::Games / Christian Keimel
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
      m_pCursor        (nullptr),
      m_iRows          (1),
      m_iPos           (0),
      m_iOffset        (-1),
      m_iMaxScroll     (0),
      m_iMDown         (0),
      m_bHover         (false),
      m_bMouseDown     (false),
      m_bCategories    (false),
      m_bShowSelected  (false),
      m_bOneCatPage    (false),
      m_bScrollTrack   (true),
      m_bImgChangeEv   (false),
      m_pFontSelected  (nullptr),
      m_pFontCategory  (nullptr),
      m_sCategory      (L""),
      m_iCategoryOffset(0),
      m_iCategoryMax   (0)
    {
      m_pDrv = CGlobal::getInstance()->getVideoDriver   ();
      m_pGui = CGlobal::getInstance()->getGuiEnvironment();
      m_pFs  = CGlobal::getInstance()->getFileSystem    ();

      m_pCursor = CGlobal::getInstance()->getIrrlichtDevice()->getCursorControl();

      m_cImageSrc = irr::core::rectf(0.0f, 0.0f, 1.0f, 1.0f);

      m_itSelected = m_vImages.end();
      m_itHovered  = m_vImages.end();

      for (int i = 0; i < 4; i++) {
        m_aBtnClick[i] = false;
        m_aBtnHover[i] = false;

        m_aButtons[i] = std::make_tuple(irr::core::recti(), nullptr);
      }

      prepareUi();
    }

    CGuiImageList::~CGuiImageList() {
    }

    void CGuiImageList::prepareUi() {
      irr::core::recti l_cRect = AbsoluteClippingRect;

      if (m_pFontSelected == nullptr) m_pFontSelected = CGlobal::getInstance()->getFont(enFont::Huge, m_pDrv->getScreenSize());
      if (m_pFontCategory == nullptr) m_pFontCategory = CGlobal::getInstance()->getFont(enFont::Big , m_pDrv->getScreenSize());

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

        m_aButtons[(int)enInternalButtons::PrevCategory] = std::make_tuple(
          irr::core::recti(m_cCategoryInner.UpperLeftCorner, m_cCategoryInner.UpperLeftCorner + irr::core::vector2di(m_cCategoryInner.getHeight(), m_cCategoryInner.getHeight())),
          m_pDrv->getTexture("data/images/arrow_left.png")
        );

        m_aButtons[(int)enInternalButtons::NextCategory] = std::make_tuple(
          irr::core::recti(m_cCategoryInner.LowerRightCorner - irr::core::vector2di(m_cCategoryInner.getHeight(), m_cCategoryInner.getHeight()), m_cCategoryInner.LowerRightCorner),
          m_pDrv->getTexture("data/images/arrow_right.png")
        );

        m_cCategoryInner.UpperLeftCorner .X += m_cCategoryInner.getHeight();
        m_cCategoryInner.LowerRightCorner.X -= m_cCategoryInner.getHeight();

        l_cRect.UpperLeftCorner.Y += l_iHeight;
      }

      int l_iRasterSize = CGlobal::getInstance()->getRasterSize();

      m_cImages = irr::core::recti(
        l_cRect.UpperLeftCorner.X + 5 * l_iRasterSize,
        l_cRect.UpperLeftCorner.Y + l_iRasterSize,
        l_cRect.LowerRightCorner.X - 5 * l_iRasterSize,
        l_cRect.LowerRightCorner.Y - l_iRasterSize
      );

      m_aButtons[(int)enInternalButtons::PrevImage] = std::make_tuple(
        irr::core::recti(l_cRect.UpperLeftCorner.X, l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 - 2 * l_iRasterSize, l_cRect.UpperLeftCorner.X + 4 * l_iRasterSize, l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 + 2 * l_iRasterSize),
        m_pDrv->getTexture("data/images/arrow_left.png")
      );

      m_aButtons[(int)enInternalButtons::NextImage] = std::make_tuple(
        irr::core::recti(l_cRect.LowerRightCorner.X - 4 * l_iRasterSize, l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 - 2 * l_iRasterSize, l_cRect.LowerRightCorner.X, l_cRect.UpperLeftCorner.Y + l_cRect.getHeight() / 2 + 2 * l_iRasterSize),
        m_pDrv->getTexture("data/images/arrow_right.png")
      );

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
    void CGuiImageList::setImageList(std::vector<CGuiImageList::SListImage> &a_vImages) {
      std::string l_sSelected = m_itSelected != m_vImages.end() ? (*m_itSelected).m_sData : "";

      if (m_bCategories && m_mCategoryRanking.size() > 0) {
        std::sort(a_vImages.begin(), a_vImages.end(), [=](const SListImage &a_cImg1, const SListImage &a_cImg2) {
          std::string l_sCategory1 = a_cImg1.m_sName;
          std::string l_sCategory2 = a_cImg2.m_sName;

          std::transform(l_sCategory1.begin(), l_sCategory1.end(), l_sCategory1.begin(), [](wchar_t c){ return std::tolower(c); });
          std::transform(l_sCategory2.begin(), l_sCategory2.end(), l_sCategory2.begin(), [](wchar_t c){ return std::tolower(c); });

          if (l_sCategory1.find_first_of(':') != std::string::npos) l_sCategory1 = l_sCategory1.substr(0, l_sCategory1.find_first_of(':'));
          if (l_sCategory2.find_first_of(':') != std::string::npos) l_sCategory2 = l_sCategory2.substr(0, l_sCategory2.find_first_of(':'));

          if (l_sCategory1 != l_sCategory2) {
            bool b1 = m_mCategoryRanking.find(l_sCategory1) != m_mCategoryRanking.end();
            bool b2 = m_mCategoryRanking.find(l_sCategory2) != m_mCategoryRanking.end();

            if (b1 && b2)
              return m_mCategoryRanking.find(l_sCategory1)->second < m_mCategoryRanking.find(l_sCategory2)->second;
            else if (b1)
              return true;
            else if (b2)
              return false;
            else
              return l_sCategory1 < l_sCategory2;
          }
          else {
            if (a_cImg1.m_iRanking != a_cImg2.m_iRanking)
              return a_cImg1.m_iRanking < a_cImg2.m_iRanking;
            else
              return a_cImg1.m_sName < a_cImg2.m_sName;
          }
        });
      }

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
        std::sort(l_vCategories.begin(), l_vCategories.end(), [=](const std::wstring& s1, const std::wstring& s2) {
          std::wstring l1 = s1;
          std::transform(l1.begin(), l1.end(), l1.begin(),
            [](wchar_t c){ return std::tolower(c); });

          std::wstring l2 = s2;
          std::transform(l2.begin(), l2.end(), l2.begin(),
            [](wchar_t c){ return std::tolower(c); });

          if (m_mCategoryRanking.find(helpers::ws2s(l1)) != m_mCategoryRanking.end() && m_mCategoryRanking.find(helpers::ws2s(l2)) != m_mCategoryRanking.end()) {
            return m_mCategoryRanking[helpers::ws2s(l1)] < m_mCategoryRanking[helpers::ws2s(l2)];
          }
          else return l1 < l2;
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
    void CGuiImageList::setSelected(const std::string& a_sImage, bool a_bSendEvent) {
      m_itSelected = m_vImages.end();

      for (std::vector<SListImage>::iterator it = m_vImages.begin(); it != m_vImages.end(); it++)
        if ((*it).m_sData == a_sImage) {
          m_itSelected = it;
          m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
          checkPositionAndButtons();

          if (a_bSendEvent)
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

    /**
    * Set the category ranking
    * @param a_mRanking the category ranking (key == name of the category, lowercase, value == rank)
    */
    void CGuiImageList::setCategoryRanking(std::map<std::string, int>& a_mRanking) {
      m_mCategoryRanking = a_mRanking;
    }


    void CGuiImageList::checkPositionAndButtons() {
      if (m_iPos < 0)
        m_iPos = 0;
      else if (m_iPos > m_iMaxScroll) {
        m_iPos = m_iMaxScroll;
      }

      if (m_itSelected != m_vImages.end()) {
        m_sCategory = (*m_itSelected).m_sCategory;
      }
      else m_sCategory = L"";
    }

    void CGuiImageList::selectPrevImage() {
      if (m_itSelected != m_vImages.begin()) {
        m_itSelected--;

        m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;

        checkPositionAndButtons();
        sendImagePosition();
      }
    }

    void CGuiImageList::selectNextImage() {
      if (m_itSelected == m_vImages.end()) {
        m_itSelected = m_vImages.begin();
        m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
        checkPositionAndButtons();
        sendImagePosition();
      }
      else {
        if (m_itSelected + 1 != m_vImages.end()) {
          m_itSelected++;
          m_iPos = (*m_itSelected).m_cDrawRect.getCenter().X - m_cImages.getCenter().X;
          checkPositionAndButtons();

          sendImagePosition();
        }
      }
    }

    void CGuiImageList::selectPrevCategory() {
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

    void CGuiImageList::selectNextCategory() {
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

      /** Inherited from irr::gui::IGUIElement **/
    bool CGuiImageList::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_GUI_EVENT) {
        if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_HOVERED) {
          if (a_cEvent.GUIEvent.Caller == this) {
            m_bHover = true;
            l_bRet = true;
          }
        }
        else if (a_cEvent.GUIEvent.EventType == irr::gui::EGET_ELEMENT_LEFT) {
          if (a_cEvent.GUIEvent.Caller == this) {
            for (int i = 0; i < 4; i++) {
              m_aBtnHover[i] = false;
              m_aBtnClick[i] = false;
            }

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

            for (int i = 0; i < 4; i++) {
              m_aBtnClick[i] = std::get<0>(m_aButtons[i]).isPointInside(m_cMouse);
            }

            l_bRet = true;
          }
          else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
            if (m_cImages.isPointInside(m_cMouse) && abs(m_cMouse.X - m_iMDown) < CGlobal::getInstance()->getRasterSize() / 4 && m_itHovered != m_vImages.end()) {
              m_itSelected = m_itHovered;
              checkPositionAndButtons();
              sendImageSelected();
              l_bRet = true;
            }

            if (std::get<0>(m_aButtons[(int)enInternalButtons::NextImage   ]).isPointInside(m_cMouse) && m_aBtnClick[(int)enInternalButtons::NextImage   ]) { selectNextImage   (); m_aBtnClick[(int)enInternalButtons::NextImage   ] = false; }
            if (std::get<0>(m_aButtons[(int)enInternalButtons::PrevImage   ]).isPointInside(m_cMouse) && m_aBtnClick[(int)enInternalButtons::PrevImage   ]) { selectPrevImage   (); m_aBtnClick[(int)enInternalButtons::PrevImage   ] = false; }
            if (std::get<0>(m_aButtons[(int)enInternalButtons::NextCategory]).isPointInside(m_cMouse) && m_aBtnClick[(int)enInternalButtons::NextCategory]) { selectNextCategory(); m_aBtnClick[(int)enInternalButtons::NextCategory] = false; }
            if (std::get<0>(m_aButtons[(int)enInternalButtons::PrevCategory]).isPointInside(m_cMouse) && m_aBtnClick[(int)enInternalButtons::PrevCategory]) { selectPrevCategory(); m_aBtnClick[(int)enInternalButtons::PrevCategory] = false; }
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

            for (int i = 0; i < 4; i++) {
              if (std::get<1>(m_aButtons[i]) != nullptr) {
                m_aBtnHover[i] = std::get<0>(m_aButtons[i]).isPointInside(m_cMouse);
              }
            }
          }
        }

        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_bMouseDown = false;
          l_bRet = true;
        }
      }
      else if (a_cEvent.EventType == irr::EET_USER_EVENT) {
        if (a_cEvent.UserEvent.UserData1 == c_iEventMouseClicked) {
          if (!std::get<0>(m_aButtons[(int)enInternalButtons::NextImage]).isPointInside(m_cMouse) && !std::get<0>(m_aButtons[(int)enInternalButtons::PrevImage]).isPointInside(m_cMouse)) {
            if (a_cEvent.UserEvent.UserData2 != 0) {
              if (m_bImgChangeEv)
                sendImageSelected();
            }
            l_bRet = true;
          }
        }
      }

      if (!l_bRet)
        l_bRet = IGUIElement::OnEvent(a_cEvent);

      return l_bRet;
    }

    /**
    * Get a position to move to depending on the direction and the given mouse position
    * @param a_cMousePos the mouse position
    * @param a_iDirection the direction (0 == up, 1 == down, 2 == left, 3 == right)
    * @param a_cOut the position to move to
    * @return true if a position was found, false otherwise
    */
    bool CGuiImageList::getMoveOption(const irr::core::position2di& a_cMousePos, int a_iDirection, irr::core::position2di& a_cOut) {
      switch (a_iDirection) {
        case 0:
          if (!m_bCategories) {
            if (AbsoluteClippingRect.isPointInside(a_cMousePos))
              return false;
            else {
              a_cOut = AbsoluteClippingRect.getCenter();
              return true;
            }
          }
          else {
            if (m_cImages.getCenter().Y < a_cMousePos.Y) {
              a_cOut = m_cImages.getCenter();
              return true;
            }
            else if (m_cCategoryOuter.getCenter().Y < a_cMousePos.Y) {
              a_cOut = m_cCategoryOuter.getCenter();
              return true;
            }
            else return false;
          }
          break;

        case 1:
          if (!m_bCategories) {
            if (AbsoluteClippingRect.isPointInside(a_cMousePos))
              return false;
            else {
              a_cOut = AbsoluteClippingRect.getCenter();
              return true;
            }
          }
          else {
            if (m_cImages.getCenter().Y > a_cMousePos.Y) {
              a_cOut = m_cImages.getCenter();
              return true;
            }
            else if (m_cCategoryOuter.getCenter().Y > a_cMousePos.Y) {
              a_cOut = m_cCategoryOuter.getCenter();
              return true;
            }
            else return false;
          }
          break;

        case 2:
        case 3:
          if (m_cImages.isPointInside(a_cMousePos)) {
            if (a_iDirection == 2) selectPrevImage(); else selectNextImage();
            a_cOut = a_cMousePos;
            return true;
          }
          else if (m_cCategoryOuter.isPointInside(a_cMousePos)) {
            if (a_iDirection == 2) selectPrevCategory(); else selectNextCategory();
            a_cOut = a_cMousePos;
            return true;
          }
          else return false;
          break;
      }

      return false;
    }

    void CGuiImageList::draw() {
      irr::core::position2di l_cPos = irr::core::position2di(m_iPos, 0);

      if (m_bShowSelected) {
        m_pFontSelected->draw(helpers::s2ws(getSelectedName()).c_str(), m_cHeadline, irr::video::SColor(0xFF, 0, 0, 0), true, true, &m_cHeadline);
      }

      for (int i = 0; i < 4; i++) {
        if (std::get<1>(m_aButtons[i]) != nullptr) {
          m_cBtnRenderer.renderBackground(std::get<0>(m_aButtons[i]), m_aBtnHover[i] ? m_aBtnClick[i] ? irr::video::SColor(0xff, 0xec, 0xf1, 0x63) : irr::video::SColor(0xff, 0x33, 0x67, 0xb8) : irr::video::SColor(0xff, 0xb8, 0xc8, 0xff));
          m_pDrv->draw2DImage(std::get<1>(m_aButtons[i]), std::get<0>(m_aButtons[i]), irr::core::recti(irr::core::position2di(), std::get<1>(m_aButtons[i])->getOriginalSize()), &AbsoluteClippingRect, nullptr, true);
        }
      }

      if (m_bCategories) {
        m_pDrv->draw2DRectangle(irr::video::SColor(96, 192, 192, 192), m_cCategoryInner);
 
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

      a_pOut->addInt ("Rows"         , m_iRows        );
      a_pOut->addBool("Categories"   , m_bCategories  );
      a_pOut->addBool("ShowSelected" , m_bShowSelected);
      a_pOut->addBool("EventOnChange", m_bImgChangeEv );
    }

    void CGuiImageList::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      IGUIElement::deserializeAttributes(a_pIn, a_pOptions);
      
      if (a_pIn->existsAttribute("Rows"         )) m_iRows         = a_pIn->getAttributeAsInt ("Rows"         );
      if (a_pIn->existsAttribute("Categories"   )) m_bCategories   = a_pIn->getAttributeAsBool("Categories"   );
      if (a_pIn->existsAttribute("ShowSelected" )) m_bShowSelected = a_pIn->getAttributeAsBool("ShowSelected" );
      if (a_pIn->existsAttribute("EventOnChange")) m_bImgChangeEv  = a_pIn->getAttributeAsBool("EventOnChange");

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