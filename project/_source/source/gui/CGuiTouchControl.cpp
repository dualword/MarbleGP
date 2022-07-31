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


    IGuiMarbleControl::IGuiMarbleControl(irr::gui::IGUIElement* a_pParent) :irr::gui::IGUIElement(
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
              m_aTouch[i].m_iIndex = a_cEvent.TouchInput.ID;
              m_aTouch[i].m_cDown  = l_cPos;
              m_aTouch[i].m_cPos   = l_cPos;

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

    CGuiTouchControl_Split::STouch::STouch() : m_iIndex(-1) {
    }

    CGuiTouchControl_Split::CGuiTouchControl_Split(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent), m_pFont(CGlobal::getInstance()->getFontBySize(48)) {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
    }

    CGuiTouchControl_Split::~CGuiTouchControl_Split() {
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
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::LowerLeft : CControlLayout::enPosition::LowerRight);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemLeft    , l_cSteerOuter, l_cSize);
                          l_cLayout.addItem(enItemIndex::ItemNeutralS, l_cSteerOuter, l_cSize);
                          l_cLayout.addItem(enItemIndex::ItemRight   , l_cSteerOuter, l_cSize);

      // Add the throttle items
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::LowerLeft);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemForeward, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemNeutralP, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBackward, l_cThrottleOuter, l_cSize);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemBrake   , l_cThrottleOuter, irr::core::dimension2du(3 * l_iSize, l_iSize));

      // Add the rearview item
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::UpperLeft : CControlLayout::enPosition::UpperRight);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);

      // Add the respawn item
      l_cLayout.addContainer(m_eType == enTouchCtrlType::SteerLeft ? CControlLayout::enPosition::UpperRight : CControlLayout::enPosition::UpperLeft);
      l_cLayout.addRow(); l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);

      irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);

      m_aItemMap[(int)enTouchId::IdPower   ] = { enItemIndex::ItemForeward , enItemIndex::ItemBackward, enItemIndex::ItemNeutralP, enItemIndex::ItemBrake };
      m_aItemMap[(int)enTouchId::IdSteer   ] = { enItemIndex::ItemLeft     , enItemIndex::ItemRight   , enItemIndex::ItemNeutralS };
      m_aItemMap[(int)enTouchId::IdBrake   ] = { enItemIndex::ItemBrake    }; 
      m_aItemMap[(int)enTouchId::IdRearView] = { enItemIndex::ItemRearview };
      m_aItemMap[(int)enTouchId::IdRespawn ] = { enItemIndex::ItemRespawn  };
    }

    CGuiTouchControl::CGuiTouchControl(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent)
    {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
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

      l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::LowerLeft : CControlLayout::enPosition::LowerRight);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemForeLeft , l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemForeward , l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemForeRight, l_cOuter, l_cSize);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemLeft    , l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemNeutralS, l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemRight   , l_cOuter, l_cSize);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemBackLeft , l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemBackward , l_cOuter, l_cSize);
      l_cLayout.addItem(enItemIndex::ItemBackRight, l_cOuter, l_cSize);

      l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::LowerRight : CControlLayout::enPosition::LowerLeft);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemBrake, l_cBrakeOuter, l_cBrakeInner);

      l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::UpperLeft : CControlLayout::enPosition::UpperRight);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);

      l_cLayout.addContainer(m_eType == enTouchCtrlType::ControlLeft ? CControlLayout::enPosition::UpperRight : CControlLayout::enPosition::UpperLeft);
      l_cLayout.addRow();
      l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);

      irr::core::dimension2du l_cScreen = irr::core::dimension2du(a_cRect.getWidth(), a_cRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);
    }

    CGuiTouchControl::~CGuiTouchControl() {
    }


    CGuiMarbleTouchControl::CGuiMarbleTouchControl(irr::gui::IGUIElement* a_pParent) : 
      IGuiMarbleControl(a_pParent), 
      m_iControl  (-1), 
      m_iLine     (25),
      m_iBrakeY   (0),
      m_iThickness(3) 
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
        l_cLayout.addItem(enItemIndex::ItemRespawn, l_cOuter, l_cSize);
        l_cLayout.addRow();
        l_cLayout.addItem(enItemIndex::ItemRearview, l_cOuter, l_cSize);
      }
      irr::core::dimension2du l_cScreen = irr::core::dimension2du(AbsoluteClippingRect.getWidth(), AbsoluteClippingRect.getHeight());

      buildUI(&l_cLayout, l_cScreen);

      irr::core::position2di  l_cCenter = AbsoluteClippingRect.getCenter() + irr::core::vector2di(0, l_cScreen.Height / 8);
      irr::core::position2di  l_cOffset = irr::core::position2di(l_iSize / 2, l_iSize / 2);

      m_cCenter = irr::core::recti(l_cCenter - l_cOffset, l_cCenter + l_cOffset);

      m_iThickness = l_iOffset / 2;

      if (m_iThickness < 2)
        m_iThickness = 2;

      m_iLine = l_iSize / 3;

      m_iBrakeY = m_cCenter.getCenter().Y + (l_cScreen.Height - m_cCenter.getCenter().Y) / 2;
    }

    CGuiMarbleTouchControl::~CGuiMarbleTouchControl() {

    }

    bool CGuiMarbleTouchControl::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          if (m_iControl == -1 && m_cCenter.isPointInside(l_cPos)) {
            m_iControl = a_cEvent.TouchInput.ID;
            l_bRet = true;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (m_iControl == a_cEvent.TouchInput.ID) {
            m_cPos = l_cPos;
            l_bRet = true;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (m_iControl == a_cEvent.TouchInput.ID) {
            m_iControl = -1;
            l_bRet = true;
          }
        }
      }
#else
      if (a_cEvent.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);

        if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_PRESSED_DOWN) {
          if (m_iControl == -1 && m_cCenter.isPointInside(l_cPos)) {
            m_iControl = 0;
            m_cPos = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
            l_bRet = true;
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_MOUSE_MOVED) {
          if (m_iControl != -1) {
            m_cPos = irr::core::position2di(a_cEvent.MouseInput.X, a_cEvent.MouseInput.Y);
          }
        }
        else if (a_cEvent.MouseInput.Event == irr::EMIE_LMOUSE_LEFT_UP) {
          if (m_iControl != -1) {
            m_iControl = -1;
            l_bRet = 0;
          }
        }
      }
#endif

      if (!l_bRet) {
        l_bRet = IGuiMarbleControl::OnEvent(a_cEvent);
      }

      return l_bRet;
    }

    void CGuiMarbleTouchControl::getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      IGuiMarbleControl::getControl(a_iCtrlX, a_iCtrlY, a_bBrake, a_bRespawn, a_bRearView);

      a_iCtrlX = 0;
      a_iCtrlY = 0;

      if (m_iControl != -1) {
        a_iCtrlX = m_cPos.X > m_cCenter.LowerRightCorner.X ?  127 : m_cPos.X < m_cCenter.UpperLeftCorner.X ? -127 : 0;
        a_iCtrlY = m_cPos.Y > m_cCenter.LowerRightCorner.Y ? -127 : m_cPos.Y < m_cCenter.UpperLeftCorner.Y ?  127 : 0;

        if (m_cPos.Y > m_iBrakeY)
          a_bBrake = true;
      }
    }

    void CGuiMarbleTouchControl::drawControlRectangle(const irr::video::SColor &a_cColor, const irr::core::vector2di &a_cCenter, const irr::core::vector2di &a_cSize) {
      m_pDrv->draw2DRectangle(a_cColor, irr::core::recti(a_cCenter - a_cSize, a_cCenter + a_cSize));
    }

    void CGuiMarbleTouchControl::draw() {
      IGuiMarbleControl::draw();

      irr::core::vector2di l_cCenter = m_cCenter.getCenter();

      if (m_iControl == -1) {
        drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(l_cCenter.X, m_cCenter.UpperLeftCorner .Y), irr::core::vector2di(m_iLine, m_iThickness));
        drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(l_cCenter.X, m_cCenter.LowerRightCorner.Y), irr::core::vector2di(m_iLine, m_iThickness));
        drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cCenter.UpperLeftCorner .X, l_cCenter.Y), irr::core::vector2di(m_iThickness, m_iLine));
        drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cCenter.LowerRightCorner.X, l_cCenter.Y), irr::core::vector2di(m_iThickness, m_iLine));
      }
      else {
        if (m_cCenter.isPointInside(m_cPos)) {
          drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(l_cCenter.X, m_cCenter.UpperLeftCorner .Y), irr::core::vector2di(m_iLine, m_iThickness));
          drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(l_cCenter.X, m_cCenter.LowerRightCorner.Y), irr::core::vector2di(m_iLine, m_iThickness));
          drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cCenter.UpperLeftCorner .X, l_cCenter.Y), irr::core::vector2di(m_iThickness, m_iLine));
          drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cCenter.LowerRightCorner.X, l_cCenter.Y), irr::core::vector2di(m_iThickness, m_iLine));
        }
        else {
          if (m_cPos.Y < m_cCenter.UpperLeftCorner.Y) {
            drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cPos.X, m_cCenter.UpperLeftCorner.Y), irr::core::vector2di(m_iLine, m_iThickness));
          }
          else if (m_cPos.Y > m_cCenter.LowerRightCorner.Y) {
            drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cPos.X, m_cCenter.LowerRightCorner.Y), irr::core::vector2di(m_iLine, m_iThickness));
          }
          else {
            drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cPos.X, m_cCenter.UpperLeftCorner .Y), irr::core::vector2di(m_iLine, m_iThickness));
            drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cPos.X, m_cCenter.LowerRightCorner.Y), irr::core::vector2di(m_iLine, m_iThickness));
          }

          if (m_cPos.X < m_cCenter.UpperLeftCorner.X) {
            drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cCenter.UpperLeftCorner.X, m_cPos.Y), irr::core::vector2di(m_iThickness, m_iLine));
          }
          else if (m_cPos.X > m_cCenter.LowerRightCorner.X) {
            drawControlRectangle(irr::video::SColor(224, 128, 128, 255), irr::core::vector2di(m_cCenter.LowerRightCorner.X, m_cPos.Y), irr::core::vector2di(m_iThickness, m_iLine));
          }
          else {
            drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cCenter.UpperLeftCorner .X, m_cPos.Y), irr::core::vector2di(m_iThickness, m_iLine));
            drawControlRectangle(irr::video::SColor(224, 128, 255, 128), irr::core::vector2di(m_cCenter.LowerRightCorner.X, m_cPos.Y), irr::core::vector2di(m_iThickness, m_iLine));
          }
        }
      }
    }
  }
}
