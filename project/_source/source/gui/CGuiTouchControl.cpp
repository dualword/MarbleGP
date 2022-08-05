// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGuiTouchControl.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace gui {
    /**
    * @class CControlLayout
    * @author Christian Keimel
    * This is an extra class that allows the layout
    * of the controls. Only used here
    */
    class CControlLayout {
      public:
        typedef struct SItem {
          IGuiMarbleControl::enItemIndex m_eIndex;

          irr::core::dimension2du m_cOuter;   /**< The outer size of the item (touchable) */
          irr::core::dimension2du m_cInner;   /**< The inner size of the item (at least the outer size + offset) */

          irr::core::recti m_cOuterRect;      /**< The outer rectangle that will be calculated */
          irr::core::recti m_cInnerRect;      /**< The inner rectangle that will be calculated */

          SItem() : m_eIndex(IGuiMarbleControl::enItemIndex::ItemCount) {
          }

          SItem(const SItem& a_cOther) : m_eIndex(a_cOther.m_eIndex), m_cOuter(a_cOther.m_cOuter), m_cInner(a_cOther.m_cInner), m_cOuterRect(a_cOther.m_cOuterRect), m_cInnerRect(a_cOther.m_cInnerRect) {
          }
        } SItem;

        typedef struct SLine {
          std::vector<SItem> m_vItems;
          irr::core::dimension2du m_cSize;

          SLine() {
          }

        } SLine;

        enum class enPosition {
          UpperLeft,
          Upper,
          UpperRight,
          Left,
          Center,
          Right,
          LowerLeft,
          Lower,
          LowerRight,
          Count
        };

        int m_iBtnSize;
        int m_iOffset;

        enPosition m_eCurrent;

        /**
        * This map holds data for every possible layout position.
        */
        std::map<enPosition, std::vector<SLine>> m_mLayout;

        /**
        * A vector with all the items, filled when calculating the layout
        */
        std::vector<SItem> m_vItems;

        /**
        * Iterator of the item
        */
        std::vector<SItem>::iterator m_itItem;

        std::vector<SLine>::iterator m_itCurrentLine;

      public:
        CControlLayout(int a_iBtnSize, int a_iOffset) : m_iBtnSize(a_iBtnSize), m_iOffset(a_iOffset), m_eCurrent(enPosition::Count) {
        }

        ~CControlLayout() {
        }

        /**
        * Adds a new container which will be used to add all
        * layout elements until a new container is added. Acess
        * to already available containers can be given if an
        * already added position is added again
        * @param a_ePosition the position of the new container
        */
        void addContainer(enPosition a_ePosition) {
          if (m_mLayout.find(a_ePosition) == m_mLayout.end()) {
            m_mLayout[a_ePosition] = std::vector<SLine>();
          }

          m_itCurrentLine = m_mLayout[a_ePosition].begin();
          m_eCurrent = a_ePosition;
        }

        /**
        * Start a new row in the container
        */
        void addRow() {
          if (m_eCurrent != enPosition::Count) {
            m_mLayout[m_eCurrent].push_back(SLine());
            m_itCurrentLine = m_mLayout[m_eCurrent].end() - 1;
          }
        }

        /**
        * Add a new item
        */
        void addItem(IGuiMarbleControl::enItemIndex a_eIndex, const irr::core::dimension2du& a_cOuter, const irr::core::dimension2du& a_cInner) {
          if (m_eCurrent != enPosition::Count && m_mLayout.find(m_eCurrent) != m_mLayout.end() && m_mLayout[m_eCurrent].size() > 0) {
            SItem l_cNewItem;
            l_cNewItem.m_eIndex = a_eIndex;
            l_cNewItem.m_cOuter = a_cOuter;
            l_cNewItem.m_cInner = a_cInner;
            (*m_itCurrentLine).m_vItems.push_back(l_cNewItem);

            if ((int)a_cOuter.Height > (*m_itCurrentLine).m_cSize.Height)
              (*m_itCurrentLine).m_cSize.Height = a_cOuter.Height;

            (*m_itCurrentLine).m_cSize.Width += a_cOuter.Width;
          }
        }

        /**
        * Calculate the layout
        */
        void calculateLayout(const irr::core::dimension2du &a_cViewport) {
          for (std::map<enPosition, std::vector<SLine>>::iterator it = m_mLayout.begin(); it != m_mLayout.end(); it++) {
            if (it->second.size() > 0) {
              irr::core::position2di  l_cStart;
              irr::core::dimension2du l_cTotal;

              // Calculate the dimensions of the layout position
              for (std::vector<SLine>::iterator l_itLine = it->second.begin(); l_itLine != it->second.end(); l_itLine++) {
                l_cTotal.Height += (*l_itLine).m_cSize.Height;
              
                if ((*l_itLine).m_cSize.Width > l_cTotal.Width)
                  l_cTotal.Width = (*l_itLine).m_cSize.Width;
              }

              switch (it->first) {
                case enPosition::UpperLeft : l_cStart = irr::core::position2di(0                                         ,                                            0); break;
                case enPosition::Upper     : l_cStart = irr::core::position2di(a_cViewport.Width / 2 - l_cTotal.Width / 2, 0                                           ); break;
                case enPosition::UpperRight: l_cStart = irr::core::position2di(a_cViewport.Width     - l_cTotal.Width    , 0                                           ); break;
                case enPosition::Left      : l_cStart = irr::core::position2di(0                                         , a_cViewport.Height / 2 - l_cTotal.Height / 2); break;
                case enPosition::Center    : l_cStart = irr::core::position2di(a_cViewport.Width / 2 - l_cTotal.Width / 2, a_cViewport.Height / 2 - l_cTotal.Height / 2); break;
                case enPosition::Right     : l_cStart = irr::core::position2di(a_cViewport.Width     - l_cTotal.Width    , a_cViewport.Height / 2 - l_cTotal.Height / 2); break;
                case enPosition::LowerLeft : l_cStart = irr::core::position2di(0                                         , a_cViewport.Height     - l_cTotal.Height    ); break;
                case enPosition::Lower     : l_cStart = irr::core::position2di(a_cViewport.Width / 2 - l_cTotal.Width / 2, a_cViewport.Height     - l_cTotal.Height    ); break;
                case enPosition::LowerRight: l_cStart = irr::core::position2di(a_cViewport.Width     - l_cTotal.Width    , a_cViewport.Height     - l_cTotal.Height    ); break;
                default: break;
              }

              irr::core::position2di l_cPos = l_cStart;

              for (std::vector<SLine>::iterator l_itLine = it->second.begin(); l_itLine != it->second.end(); l_itLine++) {
                for (std::vector<SItem>::iterator l_itItem = (*l_itLine).m_vItems.begin(); l_itItem != (*l_itLine).m_vItems.end(); l_itItem++) {
                  (*l_itItem).m_cOuterRect = irr::core::recti(l_cPos, (*l_itItem).m_cOuter);

                  irr::core::dimension2du l_cInnerOffset = ((*l_itItem).m_cOuter - (*l_itItem).m_cInner) / 2;
                  (*l_itItem).m_cInnerRect = irr::core::recti(l_cPos + irr::core::position2di(l_cInnerOffset.Width, l_cInnerOffset.Height), (*l_itItem).m_cInner);

                  m_vItems.push_back(*l_itItem);

                  l_cPos.X += (*l_itItem).m_cOuter.Width;
                }

                l_cPos.X = l_cStart.X;
                l_cPos.Y += (*l_itLine).m_cSize.Height;
              }
            }
          }
          m_itItem = m_vItems.size() > 0 ? m_vItems.begin() : m_vItems.end();
        }

        /**
        * Use this method to get the data
        * @param a_eIndex the item index
        * @param a_cInner the inner rect
        * @param a_cOuter the outer rect
        * @return true if the end of items is not yet reached, false otherwise
        */
        bool getNextItem(IGuiMarbleControl::enItemIndex &a_eIndex, irr::core::recti &a_cInner, irr::core::recti &a_cOuter) {
          if (m_itItem == m_vItems.end())
            return false;

          a_eIndex = (*m_itItem).m_eIndex;
          a_cInner = (*m_itItem).m_cInnerRect;
          a_cOuter = (*m_itItem).m_cOuterRect;

          m_itItem++;

          return true;
        }
    };


    IGuiMarbleControl::IGuiMarbleControl(irr::gui::IGUIElement* a_pParent) : irr::gui::IGUIElement(
        (irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId,
        CGlobal::getInstance()->getGuiEnvironment(), 
        CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
        -1, 
        irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
      ),
      m_eType  ((enTouchCtrlType)CGlobal::getInstance()->getSettingData().m_iTouchControl),
      m_pGlobal(CGlobal::getInstance()),
      m_pDrv   (CGlobal::getInstance()->getVideoDriver())
    {
    }

    IGuiMarbleControl::~IGuiMarbleControl() {
      for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
        while (m_vItems[i].size() > 0) {
          STouchItem *p = *m_vItems[i].begin();
          m_vItems[i].erase(m_vItems[i].begin());
          if (p != nullptr)
            delete p;
        }
      }
    }
    /**
    * Build the UI
    * @param a_pLayout the layout
    * @param a_cScreen the screen size
    */
    void IGuiMarbleControl::buildUI(CControlLayout *a_pLayout, irr::core::dimension2du a_cScreen) {
      a_pLayout->calculateLayout(a_cScreen);

      IGuiMarbleControl::enItemIndex l_eIndex;

      irr::core::recti l_cInnerRect;
      irr::core::recti l_cOuterRect;

      while (a_pLayout->getNextItem(l_eIndex, l_cInnerRect, l_cOuterRect)) {
        irr::video::SColor l_cColor = irr::video::SColor(128, 128, 128, 128);
        std::string        l_sPath  = "";

        switch (l_eIndex) {
          case enItemIndex::ItemForeward : l_cColor = irr::video::SColor(128, 128, 255, 128); l_sPath  = "data/images/control1.png"  ; break;
          case enItemIndex::ItemBackward : l_cColor = irr::video::SColor(128, 255, 255, 128); l_sPath  = "data/images/control7.png"  ; break;
          case enItemIndex::ItemLeft     : l_cColor = irr::video::SColor(128, 128, 128, 255); l_sPath  = "data/images/control3.png"  ; break;
          case enItemIndex::ItemRight    : l_cColor = irr::video::SColor(128, 128, 128, 255); l_sPath  = "data/images/control5.png"  ; break;
          case enItemIndex::ItemForeLeft : l_cColor = irr::video::SColor(128, 128, 255, 255); l_sPath  = "data/images/control0.png"  ; break;
          case enItemIndex::ItemForeRight: l_cColor = irr::video::SColor(128, 128, 255, 255); l_sPath  = "data/images/control2.png"  ; break;
          case enItemIndex::ItemBackLeft : l_cColor = irr::video::SColor(128, 128, 255, 255); l_sPath  = "data/images/control6.png"  ; break;
          case enItemIndex::ItemBackRight: l_cColor = irr::video::SColor(128, 128, 255, 255); l_sPath  = "data/images/control8.png"  ; break;
          case enItemIndex::ItemNeutralP :
          case enItemIndex::ItemNeutralS : l_cColor = irr::video::SColor(128, 192, 192, 192); l_sPath  = "data/images/control4.png"  ; break;
          case enItemIndex::ItemBrake    : l_cColor = irr::video::SColor(128, 255, 128, 128); l_sPath  = "data/images/brake.png"     ; break;
          case enItemIndex::ItemRespawn  : l_cColor = irr::video::SColor(128, 255, 255, 128); l_sPath  = "data/images/respawn.png"   ; break;
          case enItemIndex::ItemRearview : l_cColor = irr::video::SColor(128, 192, 192, 192); l_sPath  = "data/images/rearview.png"  ; break;
          case enItemIndex::ItemResetGyro: l_cColor = irr::video::SColor(128, 192, 192, 255); l_sPath  = "data/images/gyro_reset.png"; break;
          default: break;
        }

        if (l_sPath != "") {
          printf("New Touch Item: (%4i, %4i) - (%4i, %4i), (%4i, %4i) - (%4i, %4i): %s\n", 
            l_cInnerRect.UpperLeftCorner.X, 
            l_cInnerRect.UpperLeftCorner.Y,
            l_cInnerRect.LowerRightCorner.X,
            l_cInnerRect.LowerRightCorner.Y,
            l_cOuterRect.UpperLeftCorner.X, 
            l_cOuterRect.UpperLeftCorner.Y,
            l_cOuterRect.LowerRightCorner.X,
            l_cOuterRect.LowerRightCorner.Y,
            l_sPath.c_str()
          );
          STouchItem *l_pItem = new STouchItem();
          l_pItem->setRectAndColor(l_cInnerRect, l_cOuterRect, l_cColor);
          l_pItem->setTexture(m_pDrv->getTexture(l_sPath.c_str()));

          m_vItems[(int)l_eIndex].push_back(l_pItem);
        }
      }
    }

    enTouchCtrlType IGuiMarbleControl::getType() {
      return m_eType;
    }

    void IGuiMarbleControl::resetGyro() {
    }

    void IGuiMarbleControl::draw() {
      if (IsVisible) {
        for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
          for (std::vector<STouchItem*>::iterator it = m_vItems[i].begin(); it != m_vItems[i].end(); it++) {
            if (*it != nullptr) {
              m_pDrv->draw2DRectangle((*it)->m_bTouched ? (*it)->m_cColor : irr::video::SColor(224, 192, 192, 192), (*it)->m_cButton);
              m_pDrv->draw2DRectangleOutline((*it)->m_cButton, irr::video::SColor(0xFF, 0, 0, 0));

              if ((*it)->m_pTexture != nullptr) {
                m_pDrv->draw2DImage((*it)->m_pTexture, (*it)->m_cButton, (*it)->m_cSource, nullptr, nullptr, true);
              }
            }
          }
        }
      }
    }

    /**
    * Implementation of the serialization method which does nothing in this case
    */
    void IGuiMarbleControl::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
      // Nothing to do
    }

    /**
    * Implementation of the deserialization method which does nothing in this case
    */
    void IGuiMarbleControl::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
      // Nothing to do
    }

    void IGuiMarbleControl::handleTouchEvent() {
      for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
        for (std::vector<STouchItem*>::iterator l_itItem = m_vItems[i].begin(); l_itItem != m_vItems[i].end(); l_itItem++) {
          (*l_itItem)->m_bTouched = false;
        }
      }

      for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
        for (std::vector<STouchItem*>::iterator l_itItem = m_vItems[i].begin(); l_itItem != m_vItems[i].end(); l_itItem++) {
          for (int j = 0; j < 5; j++) {
            if (m_aTouch[j].m_iIndex != -1 && (*l_itItem)->m_cTouch.isPointInside(m_aTouch[j].m_cPos)) {
              (*l_itItem)->m_bTouched = (i < (int)enItemIndex::ItemRespawn || (*l_itItem)->m_cTouch.isPointInside(m_aTouch[j].m_cDown)) ? true : false;
            }
          }
        }
      }
    }

    void IGuiMarbleControl::getControl(irr::s8& a_iCtrlX, irr::s8& a_iCtrlY, bool& a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      bool l_bXPlus    = false;
      bool l_bXMinus   = false;
      bool l_bYPlus    = false;
      bool l_bYMinus   = false;

      a_bBrake    = false;
      a_bRespawn  = false;
      a_bRearView = false;

      for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
        for (std::vector<STouchItem*>::iterator l_itItem = m_vItems[i].begin(); l_itItem != m_vItems[i].end(); l_itItem++) {
          if ((*l_itItem)->m_bTouched) {
            switch ((enItemIndex)i) {
              case enItemIndex::ItemForeward : l_bYPlus    = true; break;
              case enItemIndex::ItemBackward : l_bYMinus   = true; break;
              case enItemIndex::ItemLeft     : l_bXMinus   = true; break;
              case enItemIndex::ItemRight    : l_bXPlus    = true; break;
              case enItemIndex::ItemForeLeft : l_bYPlus    = true; l_bXMinus = true; break;
              case enItemIndex::ItemForeRight: l_bYPlus    = true; l_bXPlus  = true; break;
              case enItemIndex::ItemBackLeft : l_bYMinus   = true; l_bXMinus = true; break;
              case enItemIndex::ItemBackRight: l_bYMinus   = true; l_bXPlus  = true; break;
              case enItemIndex::ItemBrake    : a_bBrake    = true; break;
              case enItemIndex::ItemRespawn  : a_bRespawn  = true; break;
              case enItemIndex::ItemRearview : a_bRearView = true; break;
              case enItemIndex::ItemResetGyro: resetGyro()       ; break;
              default: break;
            }
          }
        }
      }

      if (l_bXPlus && !l_bXMinus) a_iCtrlX = 127; else if (!l_bXPlus && l_bXMinus) a_iCtrlX = -127;
      if (l_bYPlus && !l_bYMinus) a_iCtrlY = 127; else if (!l_bYPlus && l_bYMinus) a_iCtrlY = -127;

      if (a_bBrake) {
        if (l_bYPlus) a_iCtrlY = 0; else a_iCtrlY = -127;
      }
    }

    /**
    * Is the touched point a throttle control?
    * @param a_cPos the position of the touch
    * @return true if a throttle item was touched
    */
    bool IGuiMarbleControl::isThrottleTouchhed(const irr::core::position2di& a_cPos) {
      for (int j = (int)enItemIndex::ItemForeward; j <= (int)enItemIndex::ItemBackward; j++) {
        for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
          if ((*it)->m_cTouch.isPointInside(a_cPos)) {
            return true;
          }
        }
      }

      for (int j = (int)enItemIndex::ItemForeLeft; j <= (int)enItemIndex::ItemBackRight; j++) {
        for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
          if ((*it)->m_cTouch.isPointInside(a_cPos)) {
            return true;
          }
        }
      }

      return false;
    }

    /**
    * Is the touched point a steering control?
    * @param a_cPos the position of the touch
    * @return true if a steering item was touched
    */
    bool IGuiMarbleControl::isSteeringTouchhed(const irr::core::position2di& a_cPos) {
      for (int j = (int)enItemIndex::ItemLeft; j <= (int)enItemIndex::ItemRight; j++) {
        for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
          if ((*it)->m_cTouch.isPointInside(a_cPos)) {
            return true;
          }
        }
      }

      for (int j = (int)enItemIndex::ItemForeLeft; j <= (int)enItemIndex::ItemBackRight; j++) {
        for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
          if ((*it)->m_cTouch.isPointInside(a_cPos)) {
            return true;
          }
        }
      }

      return false;
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool IGuiMarbleControl::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == -1) {
              m_aTouch[i].m_iIndex    = a_cEvent.TouchInput.ID;
              m_aTouch[i].m_cDown     = l_cPos;
              m_aTouch[i].m_cPos      = l_cPos;
              m_aTouch[i].m_bSteering = isSteeringTouchhed(l_cPos);
              m_aTouch[i].m_bThrottle = isThrottleTouchhed(l_cPos);

              handleTouchEvent();
              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == a_cEvent.TouchInput.ID) {
              m_aTouch[i].m_cPos = l_cPos;
              handleTouchEvent();
              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == a_cEvent.TouchInput.ID) {
              m_aTouch[i].m_iIndex = -1;
              handleTouchEvent();
              break;
            }
          }
        }
      }
#else
      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          m_aTouch[0].m_cDown  = l_cPos;
          m_aTouch[0].m_cPos   = l_cPos;
          m_aTouch[0].m_iIndex = 0;

          handleTouchEvent();
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          if (m_aTouch[0].m_iIndex != -1) 
            m_aTouch[0].m_cPos = l_cPos;

          handleTouchEvent();
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          m_aTouch[0].m_iIndex = -1;
          handleTouchEvent();
        }
      }
#endif

      return l_bRet;
    }

    CGuiGyroControl::CGuiGyroControl(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent),
      m_fX(0.0),
      m_fY(0.0),
      m_fZ(0.0)
    {
      AbsoluteClippingRect = irr::core::recti(irr::core::position2di(0, 0), m_pDrv->getScreenSize());

      irr::s32 l_iSize   = AbsoluteClippingRect.getWidth() / 16;
      irr::s32 l_iOffset = AbsoluteClippingRect.getWidth() / 256;

      CControlLayout l_cLayout = CControlLayout(l_iSize, l_iOffset);

      // Button size
      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);
      // Outer size of single buttons
      irr::core::dimension2du l_cOuter = irr::core::dimension2du(l_iSize + l_iOffset, l_iSize + l_iOffset);

      for (int i = 0; i < 2; i++) {
        l_cLayout.addContainer(i == 0 ? CControlLayout::enPosition::UpperLeft : CControlLayout::enPosition::UpperRight);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemResetGyro, l_cOuter, l_cSize);

        l_cLayout.addContainer(i == 0 ? CControlLayout::enPosition::LowerLeft : CControlLayout::enPosition::LowerRight);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);

        l_cLayout.addContainer(i == 0 ? CControlLayout::enPosition::Left : CControlLayout::enPosition::Right);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);
      }
      irr::core::dimension2du l_cScreen = irr::core::dimension2du(AbsoluteClippingRect.getWidth(), AbsoluteClippingRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);
    }

    CGuiGyroControl::~CGuiGyroControl() {
    }

    bool CGuiGyroControl::OnEvent(const irr::SEvent& a_cEvent) {
      if (!IGuiMarbleControl::OnEvent(a_cEvent)) {
#ifdef _ANDROID
        if (a_cEvent.EventType == irr::EET_GYROSCOPE_EVENT) {
          m_fX += a_cEvent.GyroscopeEvent.X;
          m_fY += a_cEvent.GyroscopeEvent.Y;
          m_fZ += a_cEvent.GyroscopeEvent.Z;
        }
#endif
      }
      else return true;

      return false;
    }

    void CGuiGyroControl::getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      IGuiMarbleControl::getControl(a_iCtrlX, a_iCtrlY, a_bBrake, a_bRespawn, a_bRearView);

      a_iCtrlX = m_fZ > 5.0 ? -127 : m_fZ < -5.0 ?  127 : 0;
      a_iCtrlY = m_fY > 7.5 ?  127 : m_fY < -7.5 ? -127 : 0;

      a_bBrake = m_fY < -12.5;
    }

    void CGuiGyroControl::resetGyro() {
      m_fX = 0.0;
      m_fY = 0.0;
      m_fZ = 0.0;
    }

    IGuiMarbleControl::STouchItem::STouchItem() : m_bTouched(false), m_bActive(true), m_pTexture(nullptr) {
    }

    IGuiMarbleControl::STouchItem::STouchItem(const STouchItem& a_cOther) :
      m_bTouched(a_cOther.m_bTouched),
      m_bActive (a_cOther.m_bActive),
      m_cButton (a_cOther.m_cButton),
      m_cTouch  (a_cOther.m_cTouch),
      m_cSource (a_cOther.m_cSource),
      m_cColor  (a_cOther.m_cColor),
      m_pTexture(a_cOther.m_pTexture)
    {
    }

    /**
    * Set the texture and initialize the source rect
    * @param a_pTexture the texture
    */
    void IGuiMarbleControl::STouchItem::setTexture(irr::video::ITexture* a_pTexture) {
      m_pTexture = a_pTexture;

      if (m_pTexture != nullptr) {
        m_cSource = irr::core::recti(irr::core::position2di(0, 0), m_pTexture->getSize());
      }
    }

    /**
    * Set the rectangles and the touch color
    * @param a_cButton the button rectangle on the screen
    * @param a_cTouch the touch rectangle on the screen
    * @param a_cColor the touched color of the item
    */
    void IGuiMarbleControl::STouchItem::setRectAndColor(const irr::core::recti& a_cButton, const irr::core::recti& a_cTouch, const irr::video::SColor& a_cColor) {
      m_cButton = a_cButton;
      m_cTouch  = a_cTouch;
      m_cColor  = a_cColor;
    }

    IGuiMarbleControl::STouch::STouch() : m_iIndex(-1), m_bSteering(false), m_bThrottle(false) {
    }

    CGuiTouchControl_Split::CGuiTouchControl_Split(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent) {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
    }

    CGuiTouchControl_Split::~CGuiTouchControl_Split() {
    }

    /**
    * handle Irrlicht events
    * @param a_cEvent the Irrlicht event to handle
    */
    bool CGuiTouchControl_Split::OnEvent(const irr::SEvent &a_cEvent) {
      bool l_bRet = false;

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          for (int i = 0; i < 5; i++) {
            if (a_cEvent.TouchInput.ID == m_aTouch[i].m_iIndex) {
              if (m_aTouch[i].m_bSteering) {
                irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);
                m_aTouch[i].m_cPos = l_cPos;

                for (int j = (int)enItemIndex::ItemLeft; j <= (int)enItemIndex::ItemRight; j++) {
                  for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
                    switch ((enItemIndex)j) {
                      case enItemIndex::ItemLeft:
                        (*it)->m_bTouched = l_cPos.X <= (*it)->m_cTouch.LowerRightCorner.X;
                        break;

                      case enItemIndex::ItemRight:
                        (*it)->m_bTouched = l_cPos.X >= (*it)->m_cTouch.UpperLeftCorner.X;
                        break;

                      default:
                        break;
                    }
                  }
                }

                l_bRet = true;
              }
              
              if (m_aTouch[i].m_bThrottle) {
                irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);
                m_aTouch[i].m_cPos = l_cPos;

                for (int j = (int)enItemIndex::ItemForeward; j <= (int)enItemIndex::ItemBrake; j++) {
                  for (std::vector<STouchItem*>::iterator it = m_vItems[j].begin(); it != m_vItems[j].end(); it++) {
                    switch ((enItemIndex)j) {
                      case enItemIndex::ItemForeward:
                        (*it)->m_bTouched = l_cPos.Y <= (*it)->m_cTouch.LowerRightCorner.Y;
                        break;

                      case enItemIndex::ItemBackward:
                        (*it)->m_bTouched = l_cPos.Y >= (*it)->m_cTouch.UpperLeftCorner.Y;
                        break;

                      case enItemIndex::ItemBrake:
                        (*it)->m_bTouched = l_cPos.Y >= (*it)->m_cTouch.UpperLeftCorner.Y;
                        break;

                      default:
                        break;
                    }
                  }
                }

                l_bRet = true;
              }
            }
          }
        }
      }
#endif

      if (!l_bRet)
        l_bRet = IGuiMarbleControl::OnEvent(a_cEvent);

      return l_bRet;
    }

    void CGuiTouchControl_Split::initialize(const irr::core::recti& a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16;
      irr::s32 l_iOffset = a_cRect.getWidth() / 256;

      CControlLayout l_cLayout = CControlLayout(l_iSize, l_iOffset);

      // Button size
      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);
      // Outer size of single buttons
      irr::core::dimension2du l_cOuter = irr::core::dimension2du(l_iSize + l_iOffset, l_iSize + l_iOffset);
      // Outer size of the steering items
      irr::core::dimension2du l_cSteerOuter = irr::core::dimension2du(l_iSize + l_iOffset, 3 * (l_iSize + l_iOffset));
      // Outer size of the throttle items
      irr::core::dimension2du l_cThrottleOuter = irr::core::dimension2du(3 * (l_iSize + l_iOffset), l_iSize + l_iOffset);

      // Add the steering items
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::LowerLeft : m_eType == enTouchCtrlType::SteerRight ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::Lower);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemLeft    , m_eType == enTouchCtrlType::SteerCenter ? l_cSize : l_cSteerOuter, l_cSize);
                          l_cLayout.addItem(enItemIndex::ItemNeutralS, m_eType == enTouchCtrlType::SteerCenter ? l_cSize : l_cSteerOuter, l_cSize);
                          l_cLayout.addItem(enItemIndex::ItemRight   , m_eType == enTouchCtrlType::SteerCenter ? l_cSize : l_cSteerOuter, l_cSize);

      // Add the throttle items
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::LowerLeft);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemForeward, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemNeutralP, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBackward, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBrake   , l_cThrottleOuter, irr::core::dimension2du(3 * l_iSize, l_iSize));

      if (m_eType == enTouchCtrlType::SteerCenter) {
        l_cLayout.addContainer(CControlLayout::enPosition::LowerRight);
        l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemForeward, l_cThrottleOuter, l_cSize);
        l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemNeutralP, l_cThrottleOuter, l_cSize);
        l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBackward, l_cThrottleOuter, l_cSize);
        l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBrake   , l_cThrottleOuter, irr::core::dimension2du(3 * l_iSize, l_iSize));
      }

      // Add the rearview item
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::UpperLeft : CControlLayout::enPosition::UpperRight);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);

      // Add the respawn item
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::UpperRight : CControlLayout::enPosition::UpperLeft);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);

      irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);
    }

    CGuiTouchControl::CGuiTouchControl(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent)
    {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));

      irr::core::dimension2du l_cScreen  = m_pDrv->getScreenSize();
      irr::core::recti        l_cNeutral = (*m_vItems[(int)enItemIndex::ItemNeutralS].begin())->m_cTouch;

      m_cRects[0] = irr::core::recti(irr::core::position2di(                            0,                             0), irr::core::position2di(l_cNeutral.UpperLeftCorner .X, l_cNeutral.UpperLeftCorner .Y));
      m_cRects[1] = irr::core::recti(irr::core::position2di(l_cNeutral.UpperLeftCorner .X,                             0), irr::core::position2di(l_cNeutral.LowerRightCorner.X, l_cNeutral.UpperLeftCorner .Y));
      m_cRects[2] = irr::core::recti(irr::core::position2di(l_cNeutral.LowerRightCorner.X,                             0), irr::core::position2di(l_cScreen .Width             , l_cNeutral.UpperLeftCorner .Y));
      m_cRects[3] = irr::core::recti(irr::core::position2di(                            0, l_cNeutral.UpperLeftCorner .Y), irr::core::position2di(l_cNeutral.UpperLeftCorner .X, l_cNeutral.LowerRightCorner.Y));
      m_cRects[4] = l_cNeutral;
      m_cRects[5] = irr::core::recti(irr::core::position2di(l_cNeutral.LowerRightCorner.X, l_cNeutral.UpperLeftCorner .Y), irr::core::position2di(l_cScreen.Width              , l_cNeutral.LowerRightCorner.Y));
      m_cRects[6] = irr::core::recti(irr::core::position2di(                            0, l_cNeutral.LowerRightCorner.Y), irr::core::position2di(l_cNeutral.UpperLeftCorner .X, l_cScreen.Height             ));
      m_cRects[7] = irr::core::recti(irr::core::position2di(l_cNeutral.UpperLeftCorner .X, l_cNeutral.LowerRightCorner.Y), irr::core::position2di(l_cNeutral.LowerRightCorner.X, l_cScreen.Height             ));
      m_cRects[8] = irr::core::recti(irr::core::position2di(l_cNeutral.LowerRightCorner.X, l_cNeutral.LowerRightCorner.Y), irr::core::position2di(l_cScreen.Width              , l_cScreen.Height             ));
    }

    bool CGuiTouchControl::checkForTouchEvents() {
      bool l_bRet = false;

      for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
        for (std::vector<STouchItem *>::iterator it = m_vItems[i].begin(); it != m_vItems[i].end(); it++)
          (*it)->m_bTouched = false;
      }

      for (int i = 0; i < 5; i++) {
        if (m_aTouch[i].m_iIndex != -1) {
          if (m_aTouch[i].m_bSteering || m_aTouch[i].m_bThrottle) {
            enItemIndex l_aIndices[] = {
              enItemIndex::ItemForeLeft,
              enItemIndex::ItemForeward,
              enItemIndex::ItemForeRight,
              enItemIndex::ItemLeft,
              enItemIndex::ItemNeutralS,
              enItemIndex::ItemRight,
              enItemIndex::ItemBackLeft,
              enItemIndex::ItemBackward,
              enItemIndex::ItemBackRight
            };

            for (int j = 0; j < 9; j++) {
              if (m_cRects[j].isPointInside(m_aTouch[i].m_cPos)) {
                (*m_vItems[(int)l_aIndices[j]].begin())->m_bTouched = true;
                l_bRet = true;
              }
            }
          }

          enItemIndex l_aIndices[] = {
            enItemIndex::ItemBrake,
            enItemIndex::ItemRespawn,
            enItemIndex::ItemRearview
          };

          for (int j = 0; j < 3; j++) {
            for (std::vector<STouchItem*>::iterator it = m_vItems[(int)l_aIndices[j]].begin(); it != m_vItems[(int)l_aIndices[j]].end(); it++) {
              if ((*it)->m_cTouch.isPointInside(m_aTouch[i].m_cDown)) {
                if ((*it)->m_cTouch.isPointInside(m_aTouch[i].m_cPos)) {
                  (*it)->m_bTouched = true;
                  l_bRet = true;
                }
              }
            }
          }
        }
      }

      return l_bRet;
    }

    bool CGuiTouchControl::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == -1) {
              irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

              m_aTouch[i].m_iIndex    = a_cEvent.TouchInput.ID;
              m_aTouch[i].m_cDown     = l_cPos;
              m_aTouch[i].m_cPos      = l_cPos;
              m_aTouch[i].m_bSteering = isSteeringTouchhed(l_cPos);
              m_aTouch[i].m_bThrottle = isThrottleTouchhed(l_cPos);
             
              checkForTouchEvents();
              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          for (int i = 0; i < 5; i++) {
            if (a_cEvent.TouchInput.ID == m_aTouch[i].m_iIndex) {
              m_aTouch[i].m_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);
              checkForTouchEvents();
              break;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          for (int i = 0; i < 5; i++) {
            if (m_aTouch[i].m_iIndex == a_cEvent.TouchInput.ID) {
              m_aTouch[i].m_iIndex    = -1;
              m_aTouch[i].m_bSteering = false;
              m_aTouch[i].m_bThrottle = false;

              checkForTouchEvents();
              break;
            }
          }
        }
      }
#endif

      return l_bRet;
    }

    void CGuiTouchControl::initialize(const irr::core::recti &a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16;
      irr::s32 l_iOffset = a_cRect.getWidth() / 256;

      CControlLayout l_cLayout = CControlLayout(l_iSize, l_iOffset);

      // Button size
      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);
      // Outer size of single buttons
      irr::core::dimension2du l_cOuter = irr::core::dimension2du(l_iSize + l_iOffset, l_iSize + l_iOffset);
      // Outer size of the brake item
      irr::core::dimension2du l_cBrakeOuter = irr::core::dimension2du(3 * (l_iSize + l_iOffset), l_iSize + l_iOffset);
      // Inner size of the brake item
      irr::core::dimension2du l_cBrakeInner = irr::core::dimension2du(3 * l_iSize, l_iSize);
      // Outer size of the marble control buttons
      irr::core::dimension2du l_cCtrlOuter = m_eType == enTouchCtrlType::ControlCenter ? irr::core::dimension2du(a_cRect.getWidth() / 20, a_cRect.getWidth() / 20) : irr::core::dimension2du(l_iSize, l_iSize);
      // Inner size of the marble control buttons
      irr::core::dimension2du l_cCtrlInner = l_cCtrlOuter - irr::core::dimension2du(l_iOffset, l_iOffset);

      l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::LowerLeft : m_eType == enTouchCtrlType::ControlRight ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::Lower);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemForeLeft , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemForeward , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemForeRight, l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemLeft    , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemNeutralS, l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemRight   , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemBackLeft , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemBackward , l_cCtrlOuter, l_cCtrlInner);
      l_cLayout.addItem(enItemIndex::ItemBackRight, l_cCtrlOuter, l_cCtrlInner);

      if (m_eType == enTouchCtrlType::ControlCenter) {
        l_cLayout.addContainer(CControlLayout::enPosition::LowerLeft);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemBrake, l_cBrakeOuter, l_cBrakeInner);

        l_cLayout.addContainer(CControlLayout::enPosition::LowerRight);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemBrake, l_cBrakeOuter, l_cBrakeInner);

        l_cLayout.addContainer(CControlLayout::enPosition::UpperLeft);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRespawn , l_cOuter, l_cSize);
        l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);

        l_cLayout.addContainer(CControlLayout::enPosition::UpperRight);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRespawn , l_cOuter, l_cSize);
        l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);
      }
      else {
        l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::LowerLeft);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemBrake, l_cBrakeOuter, l_cBrakeInner);

        l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::UpperLeft : CControlLayout::enPosition::UpperRight);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);

        l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::UpperRight : CControlLayout::enPosition::UpperLeft);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);
      }

      irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);
    }

    CGuiTouchControl::~CGuiTouchControl() {
    }
  }

  bool controlAllowsRanking(int a_iControl) {
    return a_iControl != (int)enTouchCtrlType::ControlCenter && a_iControl != (int)enTouchCtrlType::SteerCenter;
  }
}
