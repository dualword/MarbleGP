// (w) 2020 - 2022 by Dustbin::Games / Christian Keimel

#include <gui/CGuiTouchControl.h>
#include <CGlobal.h>
#include <string>

namespace dustbin {
  namespace gui {
    IGuiTouchControl::IGuiTouchControl(irr::gui::IGUIElement* a_pParent) :
      irr::gui::IGUIElement((irr::gui::EGUI_ELEMENT_TYPE)g_TouchControlId, 
        CGlobal::getInstance()->getGuiEnvironment(), 
        a_pParent != nullptr ? a_pParent : CGlobal::getInstance()->getGuiEnvironment()->getRootGUIElement(), 
        -1, 
        irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize())
      ),
      m_eType  ((enTouchCtrlType)CGlobal::getInstance()->getSettingData().m_iTouchType),
      m_pGlobal(CGlobal::getInstance()),
      m_pDrv   (CGlobal::getInstance()->getVideoDriver())
    {
    }

    IGuiTouchControl::~IGuiTouchControl() {
    }


    CGuiTouchControl_Split::STouchItem::STouchItem() : m_bTouched(false), m_pTexture(nullptr) {
    }

    CGuiTouchControl_Split::STouchItem::STouchItem(const STouchItem& a_cOther) :
      m_bTouched(a_cOther.m_bTouched),
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
    void CGuiTouchControl_Split::STouchItem::setTexture(irr::video::ITexture* a_pTexture) {
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
    void CGuiTouchControl_Split::STouchItem::setRectAndColor(const irr::core::recti& a_cButton, const irr::core::recti& a_cTouch, const irr::video::SColor& a_cColor) {
      m_cButton = a_cButton;
      m_cTouch  = a_cTouch;
      m_cColor  = a_cColor;
    }


    CGuiTouchControl_Split::CGuiTouchControl_Split(irr::gui::IGUIElement* a_pParent) : IGuiTouchControl(a_pParent) {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));

      for (int i = 0; i < (int)enTouchId::IdCount; i++)
        m_aTouchIDs[i] = -1;
    }

    CGuiTouchControl_Split::~CGuiTouchControl_Split() {
    }

    void CGuiTouchControl_Split::initialize(const irr::core::recti& a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16;
      irr::s32 l_iOffset = a_cRect.getWidth() / 256;

      std::string l_aTextures[] = {
        "data/images/control1.png",
        "data/images/control7.png",
        "data/images/control3.png",
        "data/images/control5.png",
        "data/images/control4.png",
        "data/images/control4.png",
        "data/images/brake.png",
        "data/images/respawn.png",
        "data/images/rearview.png",
        ""
      };

      for (int i = 0; l_aTextures[i] != ""; i++) {
        m_aItems[i].setTexture(m_pGlobal->getVideoDriver()->getTexture(l_aTextures[i].c_str()));
      }

      // The total size of the steering items
      irr::core::dimension2du l_cSteerSize = irr::core::dimension2du(
        3 * l_iSize + 4 * l_iOffset,
        3 * l_iSize + 4 * l_iOffset
      );

      // The position of the steer items
      irr::core::position2di l_cSteerPosOuter = irr::core::position2di(
        m_eType == enTouchCtrlType::SteerLeft ? l_iOffset : a_cRect.getWidth() - l_cSteerSize.Width - l_iOffset,
        a_cRect.getHeight() - l_cSteerSize.Height - l_iOffset
      );

      // The total size of the power items
      irr::core::dimension2du l_cPowerSize = irr::core::dimension2du(
        3 * l_iSize + 4 * l_iOffset,
        4 * l_iSize + 4 * l_iOffset
      );

      // The position of the power items
      irr::core::position2di l_cPowerPosOuter = irr::core::position2di(
        m_eType == enTouchCtrlType::SteerLeft ? a_cRect.getWidth() - l_cPowerSize.Width - l_iOffset : l_iOffset,
        a_cRect.getHeight() - l_cPowerSize.Height - l_iOffset
      );

      m_cSteer = irr::core::recti(l_cSteerPosOuter, l_cSteerSize);
      m_cPower = irr::core::recti(l_cPowerPosOuter, l_cPowerSize);

      irr::core::position2di l_cPowerPos = irr::core::position2di(
        m_cPower.getCenter().X - l_iSize / 2 - l_iOffset,
        m_cPower.UpperLeftCorner.Y + l_iOffset
      );

      irr::core::position2di l_cPowerTch = irr::core::position2di(
        m_cPower.getCenter().X - 3 * l_iSize / 2 - l_iOffset,
        m_cPower.UpperLeftCorner.Y + l_iOffset
      );

      irr::core::position2di l_cSteerPos = irr::core::position2di(
        m_cSteer.UpperLeftCorner.X + l_iOffset,
        m_cSteer.getCenter().Y - l_iSize / 2 - l_iOffset
      );

      irr::core::position2di l_cSteerTch = irr::core::position2di(
        m_cSteer.UpperLeftCorner.X + l_iOffset,
        m_cSteer.getCenter().Y - 3 * l_iSize / 2 - l_iOffset
      );

      irr::core::dimension2du l_cSize  = irr::core::dimension2du(    l_iSize, l_iSize);
      irr::core::dimension2du l_cTouch = irr::core::dimension2du(3 * l_iSize, l_iSize);
      irr::core::position2di  l_cStep  = irr::core::position2di (          0, l_iSize + l_iOffset);

      m_aItems[enItemIndex::ItemForeward].setRectAndColor(irr::core::recti(l_cPowerPos, l_cSize ), irr::core::recti(l_cPowerTch, l_cTouch), irr::video::SColor(128,   0, 255,   0)); l_cPowerPos += l_cStep; l_cPowerTch += l_cStep;
      m_aItems[enItemIndex::ItemNeutralP].setRectAndColor(irr::core::recti(l_cPowerPos, l_cSize ), irr::core::recti(l_cPowerTch, l_cTouch), irr::video::SColor(128,  96,  96,  96)); l_cPowerPos += l_cStep; l_cPowerTch += l_cStep;
      m_aItems[enItemIndex::ItemBackward].setRectAndColor(irr::core::recti(l_cPowerPos, l_cSize ), irr::core::recti(l_cPowerTch, l_cTouch), irr::video::SColor(128, 255, 255,   0)); l_cPowerPos += l_cStep; l_cPowerTch += l_cStep;
      m_aItems[enItemIndex::ItemBrake   ].setRectAndColor(irr::core::recti(l_cPowerTch, l_cTouch), irr::core::recti(l_cPowerTch, l_cTouch), irr::video::SColor(128, 225,   0,   0)); l_cPowerPos += l_cStep; l_cPowerTch += l_cStep;

      l_cTouch = irr::core::dimension2du(l_iSize, 3 * l_iSize);
      l_cStep  = irr::core::position2di (l_iSize + l_iOffset, 0);

      m_aItems[enItemIndex::ItemLeft    ].setRectAndColor(irr::core::recti(l_cSteerPos, l_cSize ), irr::core::recti(l_cSteerTch, l_cTouch), irr::video::SColor(128,   0, 255,   0)); l_cSteerPos += l_cStep; l_cSteerTch += l_cStep;
      m_aItems[enItemIndex::ItemNeutralS].setRectAndColor(irr::core::recti(l_cSteerPos, l_cSize ), irr::core::recti(l_cSteerTch, l_cTouch), irr::video::SColor(128,   0, 255,   0)); l_cSteerPos += l_cStep; l_cSteerTch += l_cStep;
      m_aItems[enItemIndex::ItemRight   ].setRectAndColor(irr::core::recti(l_cSteerPos, l_cSize ), irr::core::recti(l_cSteerTch, l_cTouch), irr::video::SColor(128,   0, 255,   0)); l_cSteerPos += l_cStep; l_cSteerTch += l_cStep;

      irr::core::recti l_cRespawn;
      irr::core::recti l_cRearView;

      if (m_eType == enTouchCtrlType::SteerLeft) {
        l_cRespawn  = irr::core::recti(l_iOffset                               , l_iOffset, l_iOffset + l_iSize           , l_iOffset + l_iSize);
        l_cRearView = irr::core::recti(a_cRect.getWidth() - l_iSize - l_iOffset, l_iOffset, a_cRect.getWidth() - l_iOffset, l_iOffset + l_iSize);
      }
      else {
        l_cRearView = irr::core::recti(l_iOffset                               , l_iOffset, l_iOffset + l_iSize           , l_iOffset + l_iSize);
        l_cRespawn  = irr::core::recti(a_cRect.getWidth() - l_iSize - l_iOffset, l_iOffset, a_cRect.getWidth() - l_iOffset, l_iOffset + l_iSize);
      }

      m_aItems[enItemIndex::ItemRespawn ].setRectAndColor(l_cRespawn , l_cRespawn , irr::video::SColor(128, 192, 192,  64));
      m_aItems[enItemIndex::ItemRearview].setRectAndColor(l_cRearView, l_cRearView, irr::video::SColor(128,  64,  64, 192));

      m_aItemMap[enTouchId::IdPower   ] = { enItemIndex::ItemForeward , enItemIndex::ItemBackward, enItemIndex::ItemNeutralP, enItemIndex::ItemBrake };
      m_aItemMap[enTouchId::IdSteer   ] = { enItemIndex::ItemLeft     , enItemIndex::ItemRight   , enItemIndex::ItemNeutralS };
      m_aItemMap[enTouchId::IdBrake   ] = { enItemIndex::ItemBrake    }; 
      m_aItemMap[enTouchId::IdRearView] = { enItemIndex::ItemRearview };
      m_aItemMap[enTouchId::IdRespawn ] = { enItemIndex::ItemRespawn  };
    }

    void CGuiTouchControl_Split::draw() {
      if (IsVisible) {
        for (int i = 0; i < (int)enItemIndex::ItemCount; i++) {
          m_pDrv->draw2DRectangleOutline(m_aItems[i].m_cButton, irr::video::SColor(0xFF, 0xFF, 0, 0));
          m_pDrv->draw2DRectangle(m_aItems[i].m_bTouched ? m_aItems[i].m_cColor : irr::video::SColor(224, 192, 192, 192), m_aItems[i].m_cButton);
          if (m_aItems[i].m_pTexture != nullptr) {
            m_pDrv->draw2DImage(m_aItems[i].m_pTexture, m_aItems[i].m_cButton, m_aItems[i].m_cSource, nullptr, nullptr, true);
          }
        }
      }
    }

    bool CGuiTouchControl_Split::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          if (m_cPower.isPointInside(l_cPos)) {
            if (m_aTouchIDs[enTouchId::IdPower] == -1) {
              m_aTouchIDs[enTouchId::IdPower] = (int)a_cEvent.TouchInput.ID;

              for (std::vector<enItemIndex>::iterator it = m_aItemMap[enTouchId::IdPower].begin(); it != m_aItemMap[enTouchId::IdPower].end(); it++) {
                m_aItems[*it].m_bTouched = m_aItems[*it].m_cTouch.isPointInside(l_cPos);
              }
            }
          }
          else if (m_cSteer.isPointInside(l_cPos)) {
            if (m_aTouchIDs[enTouchId::IdSteer] == -1) {
              m_aTouchIDs[enTouchId::IdSteer] = (int)a_cEvent.TouchInput.ID;

              for (std::vector<enItemIndex>::iterator it = m_aItemMap[enTouchId::IdSteer].begin(); it != m_aItemMap[enTouchId::IdSteer].end(); it++) {
                m_aItems[*it].m_bTouched = m_aItems[*it].m_cTouch.isPointInside(l_cPos);
              }
            }
          }
          else if (m_aItems[enItemIndex::ItemBrake].m_cTouch.isPointInside(l_cPos)) {
            if (m_aTouchIDs[enTouchId::IdBrake] == -1) {
              m_aTouchIDs[enTouchId::IdBrake] = (int)a_cEvent.TouchInput.ID;

              m_aItems[enItemIndex::ItemBrake   ].m_bTouched = true;
              m_aItems[enItemIndex::ItemBackward].m_bTouched = true;
            }
          }
          else if (m_aItems[enItemIndex::ItemRearview].m_cTouch.isPointInside(l_cPos)) {
            if (m_aTouchIDs[enTouchId::IdRearView] == -1) {
              m_aTouchIDs[enTouchId::IdRearView] = (int)a_cEvent.TouchInput.ID;

              m_aItems[enItemIndex::ItemRearview].m_bTouched = true;
            }
          }
          else if (m_aItems[enItemIndex::ItemRespawn].m_cTouch.isPointInside(l_cPos)) {
            if (m_aTouchIDs[enTouchId::IdRespawn] == -1) {
              m_aTouchIDs[enTouchId::IdRespawn] = (int)a_cEvent.TouchInput.ID;

              m_aItems[enItemIndex::ItemRespawn].m_bTouched = true;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          for (int i = 0; i < enTouchId::IdCount; i++) {
            if ((int)a_cEvent.TouchInput.ID == m_aTouchIDs[i]) {
              for (std::vector<enItemIndex>::iterator it2 = m_aItemMap[i].begin(); it2 != m_aItemMap[i].end(); it2++) {
                m_aItems[*it2].m_bTouched = m_aItems[*it2].m_cTouch.isPointInside(l_cPos);

                if (*it2 == enItemIndex::ItemBrake)
                  m_aItems[enItemIndex::ItemBackward].m_bTouched = m_aItems[*it2].m_cTouch.isPointInside(l_cPos);
              }
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          for (int i = 0; i < enTouchId::IdCount; i++) {
            if ((int)a_cEvent.TouchInput.ID == m_aTouchIDs[i]) {
              for (std::vector<enItemIndex>::iterator it2 = m_aItemMap[i].begin(); it2 != m_aItemMap[i].end(); it2++) {
                m_aItems[*it2].m_bTouched = false;
              }
              m_aTouchIDs[i] = -1;
            }
          }
        }
      }

      return l_bRet;
    }

    void CGuiTouchControl_Split::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
    }

    void CGuiTouchControl_Split::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
    }

    void CGuiTouchControl_Split::getControl(irr::s8 &a_iCtrlX, irr::s8 &a_iCtrlY, bool &a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      a_iCtrlX = (m_aItems[enItemIndex::ItemLeft    ].m_bTouched ? -127 : 0) + (m_aItems[enItemIndex::ItemRight   ].m_bTouched ? 127 : 0);
      a_iCtrlY = (m_aItems[enItemIndex::ItemBackward].m_bTouched ? -127 : 0) + (m_aItems[enItemIndex::ItemForeward].m_bTouched ? 127 : 0);

      a_bBrake    = m_aItems[enItemIndex::ItemBrake   ].m_bTouched;
      a_bRespawn  = m_aItems[enItemIndex::ItemRespawn ].m_bTouched;
      a_bRearView = m_aItems[enItemIndex::ItemRearview].m_bTouched;
    }

    CGuiTouchControl::CGuiTouchControl(irr::gui::IGUIElement* a_pParent) : IGuiTouchControl(a_pParent),
      m_iTouchIdTh(-1),
      m_iTouchIdBk(-1),
      m_iTouchIdRs(-1),
      m_iTouchIdRv(-1),
      m_iCtrlX    (0),
      m_iCtrlY    (0),
      m_bBrake    (false),
      m_bRespawn  (false),
      m_bRearView (false),
      m_bOneSide  (false),
      m_pRespawn  (nullptr),
      m_pRearView (nullptr),
      m_pBrake    (nullptr)
    {
      initialize(irr::core::recti(irr::core::position2di(0, 0), CGlobal::getInstance()->getVideoDriver()->getScreenSize()));
    }

    void CGuiTouchControl::initialize(const irr::core::recti &a_cRect) {
      irr::s32 l_iSize   = a_cRect.getWidth() / 16;
      irr::s32 l_iOffset = a_cRect.getWidth() / 256;

      irr::core::dimension2du l_cSize = irr::core::dimension2du(l_iSize, l_iSize);

      m_cRespawn = irr::core::recti(
        m_eType == enTouchCtrlType::ControlRight || m_eType == enTouchCtrlType::SteerRIght ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, l_iOffset) : irr::core::position2di(l_iOffset, l_iOffset),
        l_cSize
      );

      m_cRearView = irr::core::recti(
        m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::SteerLeft ? irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, l_iOffset) : irr::core::position2di(l_iOffset, l_iOffset),
        l_cSize
      );

      if (m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::ControlRight) {
        irr::core::position2di l_cStart = irr::core::position2di(
          m_eType == enTouchCtrlType::ControlLeft ? l_iOffset : a_cRect.LowerRightCorner.X - 3 * (l_iSize + l_iOffset),
          a_cRect.LowerRightCorner.Y - 3 * (l_iSize + l_iOffset) - 3 * l_iOffset
        );

        for (int y = 0; y < 3; y++) {
          for (int x = 0; x < 3; x++) {
            irr::core::position2di l_cOffset = irr::core::position2di(x * (l_iSize + l_iOffset), y * (l_iSize + l_iOffset));

            int l_iIndex = x + 3 * y;

            m_cDigital[l_iIndex] = irr::core::recti(l_cStart + l_cOffset, l_cSize);
            m_aControl[l_iIndex] = m_pDrv->getTexture((std::string("data/images/control") + std::to_string(l_iIndex) + ".png").c_str());
          }
        }

        m_cTotal = irr::core::recti(m_cDigital[0].UpperLeftCorner, m_cDigital[8].LowerRightCorner);
        m_bOneSide = true;

        m_aCtrl[0][0] = -127; m_aCtrl[0][1] =  127;
        m_aCtrl[1][0] =    0; m_aCtrl[1][1] =  127;
        m_aCtrl[2][0] =  127; m_aCtrl[2][1] =  127;
        m_aCtrl[3][0] = -127; m_aCtrl[3][1] =    0;
        m_aCtrl[4][0] =    0; m_aCtrl[4][1] =    0;
        m_aCtrl[5][0] =  127; m_aCtrl[5][1] =    0;
        m_aCtrl[6][0] = -127; m_aCtrl[6][1] = -127;
        m_aCtrl[7][0] =    0; m_aCtrl[7][1] = -127;
        m_aCtrl[8][0] =  127; m_aCtrl[8][1] = -127;
      }
      else {
        irr::s32 l_iX = m_eType == enTouchCtrlType::SteerLeft ? a_cRect.getWidth() - (3 * l_iSize / 2) - (l_iSize / 2) - l_iOffset : l_iOffset + (3 * l_iSize / 2) - (l_iSize / 2);
        irr::s32 l_iY = a_cRect.getHeight() - 2 * (l_iSize + l_iOffset);

        irr::core::position2di l_cBackward = irr::core::position2di(l_iX, l_iY);
        irr::core::position2di l_cNeutral  = l_cBackward - irr::core::position2di(0, l_iSize + l_iOffset);
        irr::core::position2di l_cForeward = l_cNeutral  - irr::core::position2di(0, l_iSize + l_iOffset);

        m_cDigital[0] = irr::core::recti(l_cForeward, l_cSize);
        m_cDigital[1] = irr::core::recti(l_cNeutral , l_cSize);
        m_cDigital[2] = irr::core::recti(l_cBackward, l_cSize);

        m_aControl[0] = m_pDrv->getTexture("data/images/control1.png");
        m_aControl[1] = m_pDrv->getTexture("data/images/control4.png");
        m_aControl[2] = m_pDrv->getTexture("data/images/control7.png");

        l_iX = m_eType == enTouchCtrlType::SteerLeft ? l_iOffset : a_cRect.getWidth() - 3 * (l_iSize + l_iOffset);

        irr::core::position2di l_cLeft = irr::core::position2di(l_iX, l_iY);
        l_cNeutral = l_cLeft + irr::core::position2di(l_iSize + l_iOffset, 0);
        irr::core::position2di l_cRight = l_cNeutral + irr::core::position2di(l_iSize + l_iOffset, 0);

        m_cDigital[3] = irr::core::recti(l_cLeft   , l_cSize);
        m_cDigital[4] = irr::core::recti(l_cNeutral, l_cSize);
        m_cDigital[5] = irr::core::recti(l_cRight  , l_cSize);

        m_aControl[3] = m_pDrv->getTexture("data/images/control3.png");
        m_aControl[4] = m_pDrv->getTexture("data/images/control4.png");
        m_aControl[5] = m_pDrv->getTexture("data/images/control5.png");
      }

      l_cSize.Width *= 3;

      m_cBrake = irr::core::recti(
        m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::SteerLeft ? 
          irr::core::position2di(a_cRect.getWidth() - l_cSize.Width - l_iOffset, a_cRect.getHeight() - l_cSize.Height - l_iOffset)
        :
          irr::core::position2di(l_iOffset                                     , a_cRect.getHeight() - l_cSize.Height - l_iOffset), l_cSize
      );

      m_pRearView = m_pDrv->getTexture("data/images/rearview.png");
      m_pRespawn  = m_pDrv->getTexture("data/images/respawn.png" );
      m_pBrake    = m_pDrv->getTexture("data/images/brake.png"   );
    }

    CGuiTouchControl::~CGuiTouchControl() {
    }

    void CGuiTouchControl::draw() {
      if (IsVisible) {
        m_pDrv->draw2DRectangle((m_iTouchIdRs != -1 && m_cRespawn.isPointInside(m_cTouchPos[2])) ? irr::video::SColor(192, 192, 192, 96) : irr::video::SColor(192, 192, 192, 192), m_cRespawn);
        m_pDrv->draw2DRectangleOutline(m_cRespawn, irr::video::SColor(0xFF, 0, 0, 0));

        if (m_pRespawn != nullptr)
          m_pDrv->draw2DImage(m_pRespawn, m_cRespawn, irr::core::recti(irr::core::position2di(0, 0), m_pRespawn->getSize()), nullptr, nullptr, true);

        m_pDrv->draw2DRectangle((m_iTouchIdRv != -1 && m_cRearView.isPointInside(m_cTouchPos[3])) ? irr::video::SColor(192, 96, 96, 192) : irr::video::SColor(192, 192, 192, 192), m_cRearView);
        m_pDrv->draw2DRectangleOutline(m_cRearView, irr::video::SColor(0xFF, 0, 0, 0));

        if (m_pRearView != nullptr)
          m_pDrv->draw2DImage(m_pRearView, m_cRearView, irr::core::recti(irr::core::position2di(0, 0), m_pRearView->getSize()), nullptr, nullptr, true);

        int l_iMax = m_bOneSide ? 9 : 6;

        for (int i = 0; i < l_iMax; i++) {
          m_pDrv->draw2DRectangle((m_iTouchIdTh != -1 && m_cDigital[i].isPointInside(m_cTouchPos[0])) ? irr::video::SColor(192, 96, 192, 96) : irr::video::SColor(192, 192, 192, 192), m_cDigital[i]);
          m_pDrv->draw2DRectangleOutline(m_cDigital[i], irr::video::SColor(0xFF, 0, 0, 0));

          if (m_aControl[i] != nullptr)
            m_pDrv->draw2DImage(m_aControl[i], m_cDigital[i], irr::core::recti(irr::core::position2di(0, 0), m_aControl[i]->getSize()), nullptr, nullptr, true);
        }

        m_pDrv->draw2DRectangle((m_iTouchIdBk != -1 && m_cBrake.isPointInside(m_cTouchPos[1])) ? irr::video::SColor(192, 255, 96, 96) : irr::video::SColor(192, 192, 192, 192), m_cBrake);

        if (m_pBrake != nullptr)
          m_pDrv->draw2DImage(m_pBrake, m_cBrake, irr::core::recti(irr::core::position2di(0, 0), m_pBrake->getSize()), nullptr, nullptr, true);
      }
    }

    bool CGuiTouchControl::OnEvent(const irr::SEvent& a_cEvent) {
      bool l_bRet = false;

      if (a_cEvent.EventType == irr::EET_TOUCH_INPUT_EVENT) {
        irr::core::position2di l_cPos = irr::core::position2di(a_cEvent.TouchInput.X, a_cEvent.TouchInput.Y);

        if (a_cEvent.TouchInput.Event == irr::ETIE_PRESSED_DOWN) {
          if (m_cRespawn.isPointInside(l_cPos) && m_iTouchIdRs == -1) {
            m_iTouchIdRs = (int)a_cEvent.TouchInput.ID;
            m_cTouchPos[2] = l_cPos;
            m_bRespawn = true;
            l_bRet = true;
          }

          if (m_cRearView.isPointInside(l_cPos) && m_iTouchIdRv == -1) {
            m_iTouchIdRv = (int)a_cEvent.TouchInput.ID;
            m_cTouchPos[3] = l_cPos;
            m_bRearView = true;
            l_bRet = true;
          }

          if (m_eType == enTouchCtrlType::ControlLeft || m_eType == enTouchCtrlType::ControlRight) {
            if (m_iTouchIdTh == -1) {
              if (m_cTotal.isPointInside(l_cPos)) {
                m_iTouchIdTh = (int)a_cEvent.TouchInput.ID;
                m_cTouchPos[0] = l_cPos;

                for (int i = 0; i < 9; i++)
                  if (m_cDigital[i].isPointInside(l_cPos)) {
                    m_iCtrlX = m_aCtrl[i][0];
                    m_iCtrlY = m_aCtrl[i][1];

                    l_bRet = true;
                  }
              }
            }
          }

          if (m_iTouchIdBk == -1) {
            if (m_cBrake.isPointInside(l_cPos)) {
              m_iTouchIdBk = (int)a_cEvent.TouchInput.ID;
              m_cTouchPos[1] = l_cPos;
              m_bBrake = true;

              l_bRet = true;
            }
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_MOVED) {
          if (m_iTouchIdRs != -1) {
            m_cTouchPos[2] = l_cPos;
            m_bRespawn = m_cRespawn.isPointInside(l_cPos);
            l_bRet = true;
          }

          if (m_iTouchIdRv != -1) {
            m_cTouchPos[3] = l_cPos;
            m_bRearView = m_cRearView.isPointInside(l_cPos);
            l_bRet = true;
          }

          if (m_iTouchIdTh == a_cEvent.TouchInput.ID) {
            m_cTouchPos[0] = l_cPos;

            for (int i = 0; i < 9; i++)
              if (m_cDigital[i].isPointInside(l_cPos)) {
                m_iCtrlX = m_aCtrl[i][0];
                m_iCtrlY = m_aCtrl[i][1];
              }

            l_bRet = true;
          }

          if (m_iTouchIdBk == a_cEvent.TouchInput.ID) {
            m_cTouchPos[1] = l_cPos;
            m_bBrake = m_cBrake.isPointInside(l_cPos);

            l_bRet = true;
          }
        }
        else if (a_cEvent.TouchInput.Event == irr::ETIE_LEFT_UP) {
          if (m_iTouchIdRs == a_cEvent.TouchInput.ID) {
            m_iTouchIdRs = -1;
            m_bRespawn = false;
          }

          if (m_iTouchIdRv == a_cEvent.TouchInput.ID) {
            m_iTouchIdRv = -1;
            m_bRearView = false;
          }

          if (m_iTouchIdTh == a_cEvent.TouchInput.ID) {
            m_iTouchIdTh = -1;
            m_iCtrlX = 0;
            m_iCtrlY = 0;

            l_bRet = true;
          }

          if (m_iTouchIdBk == a_cEvent.TouchInput.ID) {
            m_iTouchIdBk = -1;
            m_bBrake = false;

            l_bRet = true;
          }
        }
      }

      return l_bRet;
    }

    void CGuiTouchControl::serializeAttributes(irr::io::IAttributes* a_pOut, irr::io::SAttributeReadWriteOptions* a_pOptions) const {
    }

    void CGuiTouchControl::deserializeAttributes(irr::io::IAttributes* a_pIn, irr::io::SAttributeReadWriteOptions* a_pOptions) {
    }

    void CGuiTouchControl::getControl(irr::s8& a_iCtrlX, irr::s8& a_iCtrlY, bool& a_bBrake, bool &a_bRespawn, bool &a_bRearView) {
      a_iCtrlX    = m_iCtrlX;
      a_iCtrlY    = m_iCtrlY;
      a_bBrake    = m_bBrake;
      a_bRespawn  = m_bRespawn;
      a_bRearView = m_bRearView;
    }
  }
}
