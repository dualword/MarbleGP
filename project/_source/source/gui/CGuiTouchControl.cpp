// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGuiTouchControl.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace gui {
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

    enTouchCtrlType IGuiMarbleControl::getType() {
      return m_eType;
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

    CGuiGyroControl::CGuiGyroControl(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent),
      m_fX(0.0),
      m_fY(0.0),
      m_fZ(0.0)
    {
      AbsoluteClippingRect = irr::core::recti(irr::core::position2di(0, 0), m_pDrv->getScreenSize());

      irr::s32 l_iSize   = AbsoluteClippingRect.getWidth() / 16;
      irr::s32 l_iOffset = AbsoluteClippingRect.getWidth() / 256;

      irr::core::dimension2du l_cDim = irr::core::dimension2du(l_iSize, l_iSize);

      irr::core::position2di l_cPos[2] = {
        irr::core::position2di(                                            l_iOffset, l_iOffset),
        irr::core::position2di(AbsoluteClippingRect.getWidth() - l_iSize - l_iOffset, l_iOffset)
      };

      for (int i = 0; i < 2; i++) {
        STouchItem *p = new STouchItem();
        p->setRectAndColor(irr::core::recti(l_cPos[i], l_cDim), irr::core::recti(l_cPos[i], l_cDim), irr::video::SColor(128, 255, 255, 128)); l_cPos[i].Y += l_iSize;
        p->setTexture(m_pDrv->getTexture("data/images/gyro_reset.png"));
        m_vItems[(int)enItemIndex::ItemResetGyro].push_back(p);
        
        p = new STouchItem();
        p->setRectAndColor(irr::core::recti(l_cPos[i], l_cDim), irr::core::recti(l_cPos[i], l_cDim), irr::video::SColor(128, 255, 128, 128)); l_cPos[i].Y += l_iSize;
        p->setTexture(m_pDrv->getTexture("data/images/respawn.png"   ));
        m_vItems[(int)enItemIndex::ItemRespawn].push_back(p);

        p = new STouchItem();
        p->setRectAndColor(irr::core::recti(l_cPos[i], l_cDim), irr::core::recti(l_cPos[i], l_cDim), irr::video::SColor(128, 128, 128, 255));
        p->setTexture(m_pDrv->getTexture("data/images/rearview.png"  ));
        m_vItems[(int)enItemIndex::ItemRearview].push_back(p);
      }
    }

    CGuiGyroControl::~CGuiGyroControl() {
    }

    bool CGuiGyroControl::OnEvent(const irr::SEvent& a_cEvent) {
#ifdef _ANDROID
      if (a_cEvent.EventType == irr::EET_GYROSCOPE_EVENT) {
        m_fX += a_cEvent.GyroscopeEvent.X;
        m_fY += a_cEvent.GyroscopeEvent.Y;
        m_fZ += a_cEvent.GyroscopeEvent.Z;
      }
#endif
      return false;
    }

    void CGuiGyroControl::getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      a_iCtrlX = m_fZ > 5.0 ? -127 : m_fZ < -5.0 ?  127 : 0;
      a_iCtrlY = m_fY > 7.5 ?  127 : m_fY < -7.5 ? -127 : 0;

      a_bBrake = m_fY < -12.5;
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

      // The size of the buttons
      irr::core::dimension2du d = irr::core::dimension2du(l_iSize, l_iSize);

      // The outer (touch) position of the steering items (_1 == Left, _2 == Neutral, _3 == Right)
      irr::core::position2di ps1_1 = irr::core::position2di(m_eType == enTouchCtrlType::SteerLeft ? l_iOffset : a_cRect.getWidth() - 3 * (l_iOffset + l_iSize) - l_iOffset, a_cRect.getHeight() - 3 * (l_iOffset + l_iSize));
      irr::core::position2di ps1_2 = ps1_1 + irr::core::position2di(l_iSize + l_iOffset, 0);
      irr::core::position2di ps1_3 = ps1_2 + irr::core::position2di(l_iSize + l_iOffset, 0);

      // The inner (button) position of the steering items (_1 == Left, _2 == Neutral, _3 == Right)
      irr::core::position2di ps2_1 = ps1_1 + irr::core::position2di(l_iOffset, l_iSize + l_iOffset);
      irr::core::position2di ps2_2 = ps1_2 + irr::core::position2di(l_iOffset, l_iSize + l_iOffset);
      irr::core::position2di ps2_3 = ps1_3 + irr::core::position2di(l_iOffset, l_iSize + l_iOffset);

      // The outer (touch) size of the steering items
      irr::core::dimension2du ds = irr::core::dimension2du(l_iSize, 3 * l_iSize);

      // The outer (touch) position of the first items (_1 == Forward, _2 == Neutral, _3 == Backward)
      irr::core::position2di pt1_1 = irr::core::position2di(m_eType == enTouchCtrlType::SteerLeft ? a_cRect.getWidth() - 3 * (l_iOffset + l_iSize) : l_iOffset, a_cRect.getHeight() - 5 * (l_iOffset + l_iSize));
      irr::core::position2di pt1_2 = pt1_1 + irr::core::position2di(0, l_iSize + l_iOffset);
      irr::core::position2di pt1_3 = pt1_2 + irr::core::position2di(0, l_iSize + l_iOffset);

      // The brake position
      irr::core::position2di brk = pt1_3 + irr::core::position2di(0, 2 * (l_iSize + l_iOffset));

      // The inner (button) position of the throttle items (_1 == Forward, _2 == Neutral, _3 == Backward)
      irr::core::position2di pt2_1 = pt1_1 + irr::core::position2di(l_iSize, l_iSize);
      irr::core::position2di pt2_2 = pt1_2 + irr::core::position2di(l_iSize, l_iSize);
      irr::core::position2di pt2_3 = pt1_3 + irr::core::position2di(l_iSize, l_iSize);

      // The outer (touch) size of the throttle items
      irr::core::dimension2du dt = irr::core::dimension2du(3 * l_iSize, l_iSize);

      // Position of the rearview button
      irr::core::position2di prv = irr::core::position2di(m_eType == enTouchCtrlType::SteerLeft ? l_iOffset : a_cRect.getWidth() - l_iOffset - l_iSize, l_iOffset);

      // Position of the respawn button
      irr::core::position2di prs = irr::core::position2di(m_eType == enTouchCtrlType::SteerLeft ? a_cRect.getWidth() - l_iOffset - l_iSize : l_iOffset, l_iOffset);

      // 0 == index, 1 == texture file, 2 == button rect, 3 == touch rect, 4 == touch color
      std::tuple<int, std::string, irr::core::recti, irr::core::recti, irr::video::SColor> l_aTextures[] = {
        std::make_tuple((int)enItemIndex::ItemForeward, "data/images/control1.png", irr::core::recti(pt2_1,  d), irr::core::recti(pt1_1, dt), irr::video::SColor(128,   0, 255,   0)),
        std::make_tuple((int)enItemIndex::ItemBackward, "data/images/control7.png", irr::core::recti(pt2_3,  d), irr::core::recti(pt1_3, dt), irr::video::SColor(128, 255,   0,   0)),
        std::make_tuple((int)enItemIndex::ItemLeft    , "data/images/control3.png", irr::core::recti(ps2_1,  d), irr::core::recti(ps1_1, ds), irr::video::SColor(128,   0,   0, 255)),
        std::make_tuple((int)enItemIndex::ItemRight   , "data/images/control5.png", irr::core::recti(ps2_3,  d), irr::core::recti(ps1_3, ds), irr::video::SColor(128,   0,   0, 255)),
        std::make_tuple((int)enItemIndex::ItemNeutralP, "data/images/control4.png", irr::core::recti(pt2_2,  d), irr::core::recti(pt1_2, dt), irr::video::SColor(128, 255,   0,   0)),
        std::make_tuple((int)enItemIndex::ItemNeutralS, "data/images/control4.png", irr::core::recti(ps2_2,  d), irr::core::recti(ps1_2, dt), irr::video::SColor(128,   0,   0, 255)),
        std::make_tuple((int)enItemIndex::ItemBrake   , "data/images/brake.png"   , irr::core::recti(brk  , dt), irr::core::recti(brk  , dt), irr::video::SColor(128, 255,   0,   0)),
        std::make_tuple((int)enItemIndex::ItemRespawn , "data/images/respawn.png" , irr::core::recti(prs  ,  d), irr::core::recti(prs  ,  d), irr::video::SColor(128, 255, 255,   0)),
        std::make_tuple((int)enItemIndex::ItemRearview, "data/images/rearview.png", irr::core::recti(prv  ,  d), irr::core::recti(prv  ,  d), irr::video::SColor(128, 255, 255,   0)),
        std::make_tuple((int)enItemIndex::ItemCount   , ""                        , irr::core::recti(         ), irr::core::recti(         ), irr::video::SColor(255, 255, 255, 255))
      };

      for (int i = 0; std::get<0>(l_aTextures[i]) != (int)enItemIndex::ItemCount; i++) {
        STouchItem *p = new STouchItem;
        p->setTexture(m_pDrv->getTexture(std::get<1>(l_aTextures[i]).c_str()));
        p->setRectAndColor(std::get<2>(l_aTextures[i]), std::get<3>(l_aTextures[i]), std::get<4>(l_aTextures[i]));
        m_vItems[i].push_back(p);
      }

      m_aItemMap[(int)enTouchId::IdPower   ] = { enItemIndex::ItemForeward , enItemIndex::ItemBackward, enItemIndex::ItemNeutralP, enItemIndex::ItemBrake };
      m_aItemMap[(int)enTouchId::IdSteer   ] = { enItemIndex::ItemLeft     , enItemIndex::ItemRight   , enItemIndex::ItemNeutralS };
      m_aItemMap[(int)enTouchId::IdBrake   ] = { enItemIndex::ItemBrake    }; 
      m_aItemMap[(int)enTouchId::IdRearView] = { enItemIndex::ItemRearview };
      m_aItemMap[(int)enTouchId::IdRespawn ] = { enItemIndex::ItemRespawn  };
    }

    void CGuiTouchControl_Split::handleTouchEvent() {
    }

    void CGuiTouchControl_Split::getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
    }

    CGuiTouchControl::CGuiTouchControl(irr::gui::IGUIElement* a_pParent) : IGuiMarbleControl(a_pParent)
    {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
    }

    void CGuiTouchControl::initialize(const irr::core::recti &a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16;
      irr::s32 l_iOffset = a_cRect.getWidth() / 256;

      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);

      irr::core::recti l_cRespawn = irr::core::recti(
        m_eType == enTouchCtrlType::ControlRight || m_eType == enTouchCtrlType::SteerRIght ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, l_iOffset) : irr::core::position2di(l_iOffset, l_iOffset),
        l_cSize
      );

      irr::core::recti l_cRearView = irr::core::recti(
        m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::SteerLeft ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, l_iOffset) : irr::core::position2di(l_iOffset, l_iOffset),
        l_cSize
      );

      STouchItem *p = new STouchItem();
      p->setRectAndColor(l_cRespawn, l_cRespawn, irr::video::SColor(128, 255, 255, 0));
      p->setTexture(m_pDrv->getTexture("data/images/respawn.png"));
      m_vItems[(int)enItemIndex::ItemRespawn].push_back(p);

      p = new STouchItem();
      p->setRectAndColor(l_cRearView, l_cRearView, irr::video::SColor(128, 255, 255, 0));
      p->setTexture(m_pDrv->getTexture("data/images/rearview.png"));
      m_vItems[(int)enItemIndex::ItemRearview].push_back(p);

      // Adjust the size for the brake item
      l_cSize.Width = 3 * l_cSize.Width;

      irr::core::recti l_cBrake = irr::core::recti(
        m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::SteerLeft ? 
          irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, a_cRect.getHeight() - l_cSize.Height - l_iOffset)
        :
          irr::core::position2di(l_iOffset                                     , a_cRect.getHeight() - l_cSize.Height - l_iOffset), l_cSize
      );

      p = new STouchItem();
      p->setRectAndColor(l_cBrake, l_cBrake, irr::video::SColor(128, 255, 0, 0));
      p->setTexture(m_pDrv->getTexture("data/images/brake.png"));
      m_vItems[(int)enItemIndex::ItemBrake].push_back(p);

      irr::core::position2di  l_cPosO  = irr::core::position2di (m_eType == enTouchCtrlType::ControlLeft ? 0 : a_cRect.getWidth() - 3 * (l_iSize + l_iOffset) - l_iOffset, a_cRect.getHeight() - 3 * (l_iSize + l_iOffset) - l_iOffset);
      irr::core::position2di  l_cPosI  = l_cPosO + irr::core::position2di(l_iOffset, l_iOffset);
      irr::core::dimension2du l_cOuter = irr::core::dimension2du(l_iSize + l_iOffset, l_iSize + l_iOffset);
      irr::core::dimension2du l_cInner = irr::core::dimension2du(l_iSize            , l_iSize            );

      enItemIndex l_aIndex[] = {
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

      for (int y = 0; y < 3; y++) {
        irr::core::position2di l_cOffset = irr::core::position2di(0, y * (l_iSize + l_iOffset));
        for (int x = 0; x < 3; x++) {
          STouchItem *p = new STouchItem();

          std::string s = "data/images/control" + std::to_string(x + 3 * y) + ".png";

          p->setRectAndColor(irr::core::recti(l_cPosI + l_cOffset, l_cInner), irr::core::rect(l_cPosO + l_cOffset, l_cOuter), irr::video::SColor(128, 0, 0, 255));
          p->setTexture(m_pDrv->getTexture(s.c_str()));

          m_vItems[(int)l_aIndex[x + 3 * y]].push_back(p);

          l_cOffset.X += l_iSize + l_iOffset;
        }
      }
    }

    CGuiTouchControl::~CGuiTouchControl() {
    }

    void CGuiTouchControl::getControl(irr::s8& a_iCtrlX, irr::s8& a_iCtrlY, bool& a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
    }
  }
}
